/*-
 * Copyright 2011-2012 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <TINYMIXER/mixer.h>

#include <limits.h>

#if !defined(tinymixer_alloc) || !defined(tinymixer_free)
#	include <stdlib.h>
#	define tinymixer_alloc malloc
#	define tinymixer_free free
#endif

#if !defined(tinymixer_memset)
#	include <string.h>
#	define tinymixer_memset memset
#endif

#if !defined(tinymixer_sqrtf)
#	include <math.h>
#	define tinymixer_sqrtf sqrtf
#endif

#if !defined(tinymixer_abs)
#	include <stdlib.h>
#	define tinymixer_abs abs
#endif


namespace tinymixer {

static const int c_maxGainTypes = 2;
static const int c_maxSources = 32;
static const int c_maxSamples = 2048;
static const int c_gainQuantization = 1024;
static const int c_sampleRate = 44100;
static const float c_speakerDist = 0.17677669529663688110021109052621f; // 1/(4 *sqrtf(2))

namespace {

struct SoundSourceFlags
{
	enum Enum
	{
		Playing = 1 << 0,
		Positional = 1 << 1,
		Looping = 1 << 2,
		FadeOut = 1 << 3,
		Frequency = 1 << 4,
	};
};

struct SoundBuffer
{
	int refcnt;
	int nsamples;
	unsigned char nchannels;
	// short sampels[nsamples * nchannels]
};

struct SoundSource
{
	const SoundBuffer* buffer;
	int samplePos;
	float position[3];
	float fadeOutPerSample;
	float baseGain;
	float minDistance;
	float distanceDifference;
	float frequency;
	unsigned char flags;
	unsigned char baseGainIndex;
};

struct PlayingSound
{
	unsigned char sourceIndex;
};

struct SoundMixer
{
	music_callback musicCallback;
	float position[3];
	int compressorFactor;
	float baseGain[c_maxGainTypes];
	float musicGain;
	int lastCompressionSamples[2];
	int compressorThresholds[2];
	int compressorMultipliers[2];
	int compressorAttackPer1KSamples;
	int compressorReleasePer1KSamples;
	struct SoundSource sources[c_maxSources];
	int samplesLeft;
	int mixerBuffer[2 * c_maxSamples];
};

}

static SoundMixer g_mixer;

static inline float mixClamp( float v_, float min_, float max_ ) { return min_ > v_ ? min_ : (v_ > max_ ? max_ : v_); }
static inline int mixClamp( int v_, int min_, int max_ ) { return min_ > v_ ? min_ : (v_ > max_ ? max_ : v_); }
static inline int mixMin( int a_, int b_ ) { return a_ < b_ ? a_ : b_; }
static inline float mixDist( const float* a_, const float* b_ )
{
	const float distSqr = (a_[0] - b_[0]) * (a_[0] - b_[0]) + (a_[1] - b_[1]) * (a_[1] - b_[1]) + (a_[2] - b_[2]) * (a_[2] - b_[2]);
	return tinymixer_sqrtf(distSqr);
}
static inline void mixVCopy( float* dst_, const float* src_ ) { dst_[0] = src_[0]; dst_[1] = src_[1]; dst_[2] = src_[2]; }


static void decRef( SoundBuffer* buffer_ )
{
	if (0 == --buffer_->refcnt)
	{
		tinymixer_free(buffer_);
	}
}


static void addRef( SoundBuffer* buffer_ )
{
	++buffer_->refcnt;
}


static SoundSource* getSource()
{
	SoundSource* bestSource = 0;
	int bestSample = INT_MAX;

	for (int ii = 0; ii < c_maxSources; ++ii)
	{
		SoundSource* source = &g_mixer.sources[ii];
		if (!source->buffer)
		{
			return source;
		}

		if (0 == (source->flags & SoundSourceFlags::Looping))
		{
			int remaining = source->buffer->nsamples - source->samplePos;
			if (remaining < bestSample)
			{
				bestSample = remaining;
				bestSource = source;
			}
		}
	}

	return bestSource;
}


static inline int applyGain( short sample_, int quantGain_ )
{
	return (sample_ * quantGain_) / c_gainQuantization;
}


static void renderSource( SoundSource* source_, int* buffer_, const int quantGain_[2] )
{
	const bool isLooping = 0 != (source_->flags & SoundSourceFlags::Looping);
	const int nchannels = source_->buffer->nchannels;

	int remainingSamples = c_maxSamples;
	while (remainingSamples)
	{
		int samplesRead = mixMin(source_->buffer->nsamples - source_->samplePos, remainingSamples);
		int samplesWritten = samplesRead;
		const short* samples = (const short*)(source_->buffer + 1) + (source_->samplePos * nchannels);

		// handle frequency adjustments
		if (source_->flags & SoundSourceFlags::Frequency)
		{
			// adjust samplesRead for frequency shift
			const int quantFreq = (int)(source_->frequency * c_gainQuantization);
			samplesRead = (samplesRead * quantFreq) / c_gainQuantization;
			if (samplesRead >= source_->buffer->nsamples - source_->samplePos)
			{
				samplesRead = source_->buffer->nsamples - source_->samplePos;
				samplesWritten = (samplesRead * c_gainQuantization) / quantFreq;
				if (samplesRead & (c_gainQuantization -1))
				{
					++samplesWritten;
				}
			}
			
			for (int jj = 0; jj < samplesWritten; ++jj)
			{
				const int firstOffset = (jj * quantFreq) / c_gainQuantization;
				int secondOffset = ((jj + 1) * quantFreq) / c_gainQuantization;
				if (secondOffset >= source_->buffer->nsamples - source_->samplePos)
				{
					secondOffset = 0;
				}

				const int quantInterp = (jj * quantFreq) & (c_gainQuantization - 1);

				int newSamples[2];
				if (nchannels == 1)
				{
					const int sample = samples[firstOffset] + (quantInterp * (samples[secondOffset] - samples[firstOffset])) / c_gainQuantization;
					newSamples[0] = sample;
					newSamples[1] = sample;
				}
				else
				{
					newSamples[0] = samples[2*firstOffset] + (quantInterp * (samples[2*secondOffset] - samples[2*firstOffset])) / c_gainQuantization;
					newSamples[1] = samples[2*firstOffset + 1] + (quantInterp * (samples[2*secondOffset + 1] - samples[2*firstOffset + 1])) / c_gainQuantization;
				}

				
				buffer_[0] += applyGain((short)mixClamp(newSamples[0], SHRT_MIN, SHRT_MAX), quantGain_[0]);
				buffer_[1] += applyGain((short)mixClamp(newSamples[1], SHRT_MIN, SHRT_MAX), quantGain_[1]);
				buffer_ += 2;
			}
		}
		else
		{
			for (int jj = 0; jj < samplesWritten; ++jj)
			{
				if (nchannels == 1)
				{
					buffer_[0] += applyGain(samples[jj], quantGain_[0]);
					buffer_[1] += applyGain(samples[jj], quantGain_[1]);
				}
				else
				{
					buffer_[0] += applyGain(samples[2*jj], quantGain_[0]);
					buffer_[1] += applyGain(samples[2*jj + 1], quantGain_[1]);
				}

				buffer_ += 2;
			}
		}

		source_->samplePos += samplesRead;
		remainingSamples -= samplesWritten;

		if (remainingSamples)
		{
			if (!isLooping)
			{
				break;
			}

			source_->samplePos = 0;
		}
	}
}


static void renderEffects( int* buffer_ )
{
	int compressorFactor = g_mixer.compressorFactor;

	// Get maximum absolute sample and adjust compressor factor
	int maxAbsValue = 0;
	for (int ii = 0; ii < c_maxSampls * 2; ++ii)
	{
		const int absSample = (int)tinymixer_abs(buffer_[ii]);
		if (absSample > maxAbsValue)
		{
			maxAbsValue = absSample;
		}
	}

	int targetCompressorFactor = c_gainQuantization;
	if (maxAbsValue > g_mixer.compressorThresholds[1])
	{
		targetCompressorFactor = g_mixer.compressorMultipliers[1];
	}
	else if (maxAbsValue > g_mixer.compressorThresholds[0])
	{
		targetCompressorFactor = g_mixer.compressorMultipliers[0];
	}

	int attackRelease = c_gainQuantization;
	if (targetCompressorFactor < compressorFactor)
	{
		attackRelease = g_mixer.compressorAttackPer1KSamples;
	}
	else if (targetCompressorFactor > compressorFactor)	
	{
		attackRelease = g_mixer.compressorReleasePer1KSamples;
	}

	compressorFactor += ((targetCompressorFactor - compressorFactor) * attackRelease * c_maxSamples) / (1000 * c_gainQuantization);
	compressorFactor = mixClamp(compressorFactor, g_mixer.compressorMultipliers[1], c_gainQuantization);

	// Simple 2-pass compressor to limit dynamic range of audio near signal clipping levels
	if (compressorFactor < c_gainQuantization)
	{
		int lastCompressionSamples[2] = {g_mixer.lastCompressionSamples[0], g_mixer.lastCompressionSamples[1]};
		for (int ii = 0; ii < c_maxSamples * 2; ++ii)
		{
			int lastSample = lastCompressionSamples[ii & 1];
			int sample = buffer_[ii];

			int sampleDiff = sample - lastSample;
			sample = lastSample + (sampleDiff * compressorFactor) / c_gainQuantization;

			buffer_[ii] = sample;
			
			lastCompressionSamples[ii & 1] = sample;
		}

		g_mixer.lastCompressionSamples[0] = lastCompressionSamples[0];
		g_mixer.lastCompressionSamples[1] = lastCompressionSamples[1];
	}

	g_mixer.compressorFactor = compressorFactor;
}


static void mixSources( int* buffer_ )
{
	int nplaying = 0;
	PlayingSound playing[c_maxSources];
	float gain[c_maxSources][2];


	// build active sounds
	for (int ii = 0; ii < c_maxSources; ++ii)
	{
		SoundSource* source = &g_mixer.sources[ii];
		if (source->flags & SoundSourceFlags::Playing)
		{
			playing[nplaying].sourceIndex = (signed char)ii;
			++nplaying;
		}
	}

	// Update source gains
	for (int ii = 0; ii < nplaying; ++ii)
	{
		const SoundSource* source = &g_mixer.sources[playing[ii].sourceIndex];

		const float baseGain = g_mixer.baseGain[source->baseGainIndex] * source->baseGain;
		gain[ii][0] = baseGain;
		gain[ii][1] = baseGain;

		if (source->flags & SoundSourceFlags::Positional)
		{
			const float distToSound = mixDist(g_mixer.position, source->position);
			if (distToSound > 1.0e-8f)
			{
				// distance attenuation
				const float distanceGain = 1.0f - (distToSound - source->minDistance) / source->distanceDifference;

				// panning
				const float basePanningGain = (source->position[0] - g_mixer.position[0]) / distToSound;

				gain[ii][0] *= distanceGain * (1.0f + basePanningGain * -c_speakerDist);
				gain[ii][1] *= distanceGain * (1.0f + basePanningGain * +c_speakerDist);
			}
		}
	}

	tinymixer_memset(buffer_, 0, sizeof(int) * 2 * c_maxSamples);

	// render sources to target buffer
	for (int ii = 0; ii < nplaying; ++ii)
	{
		const int quantGain[2] =
			{
				(int)(mixClamp(gain[ii][0], 0.0f, 1.0f) * c_gainQuantization),
				(int)(mixClamp(gain[ii][1], 0.0f, 1.0f) * c_gainQuantization),
			};

		SoundSource* source = &g_mixer.sources[playing[ii].sourceIndex];
		renderSource(source, buffer_, quantGain);
	}

	// render music to the target buffer
	if (g_mixer.musicCallback)
	{
		(g_mixer.musicCallback)(buffer_, c_maxSamples, g_mixer.musicGain);
	}

	// render effects
	renderEffects(buffer_);

	// cull finished sources
	for (int ii = 0; ii < nplaying; ++ii)
	{
		SoundSource* source = &g_mixer.sources[playing[ii].sourceIndex];

		if (source->flags & SoundSourceFlags::FadeOut)
		{
			source->baseGain -= source->fadeOutPerSample * c_maxSamples;
			if (source->baseGain <= 0.0f)
			{
				decRef(const_cast<SoundBuffer*>(source->buffer));
				source->buffer = 0;
				source->flags = 0;
			}
		}
		else if (source->samplePos >= source->buffer->nsamples && !(source->flags & SoundSourceFlags::Looping))
		{
			decRef(const_cast<SoundBuffer*>(source->buffer));
			source->buffer = 0;
			source->flags = 0;
		}
	}
}


void getsamples( short* samples_, int nsamples_ )
{
	// mix samples left from previous render
	while (nsamples_ && g_mixer.samplesLeft)
	{
		const int samplesToMix = mixMin(nsamples_, g_mixer.samplesLeft);
		const int offset = 2 * (c_maxSamples - g_mixer.samplesLeft);

		// clip results
		for (int ii = 0; ii < samplesToMix * 2; ++ii)
		{
			samples_[ii] = (int16_t)mixClamp(g_mixer.mixerBuffer[offset + ii], SHRT_MIN, SHRT_MAX);
		}

		g_mixer.samplesLeft -= samplesToMix;
		samples_ += (2 * samplesToMix);
		nsamples_ -= samplesToMix;
	}

	while (nsamples_)
	{
		const int samplesToMix = mixMin(nsamples_, c_maxSamples);

		mixSources(g_mixer.mixerBuffer);
		g_mixer.samplesLeft = c_maxSamples;

		// clip results
		for (int ii = 0; ii < samplesToMix * 2; ++ii)
		{
			samples_[ii] = (short)mixClamp(g_mixer.mixerBuffer[ii], SHRT_MIN, SHRT_MAX);
		}

		g_samples.samplesLeft -= samplesToMix;
		samples_ += (2 * samplesToMix);
		nsamples_ -= samplesToMix;
	}
}


static SoundSource* addToMixer( const buffer* buffer_, int baseGainIndex_, float gain_ )
{
	SoundSource* source = getSource();
	if (!source)
	{
		return 0;
	}

	source->buffer = (const SoundBuffer*)buffer_;
	source->baseGain = gain_;
	source->samplePos = 0;
	source->baseGainIndex = (unsigned char)baseGainIndex_;

	addRef((SoundBuffer*)buffer_);
	return source;
}


static void playSource( SoundSource* source_ )
{
	source_->flags |= SoundSourceFlags::Playing;
}


void create_buffer( int channels_, const void* pcm_data_, int pcm_data_size_, const buffer** buffer_ )
{
	SoundBuffer* buf = (SoundBuffer*)tinymixer_alloc(sizeof(SoundBuffer) + pcm_data_size_);
	buf->refcnt = 1;
	buf->nchannels = (unsigned char)channels_;
	buf->nsamples = pcm_data_size_ / sizeof(short) / channels_;

	// copy samples
	const short* source = (const short*)pcm_data_;
	short* dest = (short*)(buf + 1);
	const int nsamples = pcm_data_size_ / sizeof(short);
	for (int ii = 0; ii < nsamples; ++ii)
	{
		*dest++ = *source++;
	}

	*buffer_ = (buffer*)buf;
}

void release_buffer( const buffer* buffer_ )
{
	decRef((SoundBuffer*)buffer_);
}



void add( const buffer* buffer_, int baseGainIndex_, float gain_ )
{
	SoundSource* source = addToMixer(buffer_, baseGainIndex_, gain_);
	if (source)
	{
		playSource(source);
	}
}

void add( const buffer* buffer_, int baseGainIndex_, float gain_, const float* position_, float min_distance_, float max_distance_ )
{
	SoundSource* source = addToMixer(buffer_, baseGainIndex_, gain_);
	if (source)
	{
		mixVCopy(source->position, position_);
		source->flags |= SoundSourceFlags::Positional;
		source->minDistance = min_distance_;
		source->distanceDifference = max_distance_ - min_distance_;
		playSource(source);
	}	
}

static SoundSource* addLoop( const buffer* buffer_, int baseGainIndex_, float gain_, loop* loop_ )
{
	SoundSource* source = addToMixer(buffer_, baseGainIndex_, gain_);
	if (!source)
	{
		*loop_ = 0;
		return 0;
	}

	source->flags |= SoundSourceFlags::Looping;
	*loop_ = (loop)(source - g_mixer.sources) + 1;
	return source;
}

void add_loop( const buffer* buffer_, int baseGainIndex_, float gain_, loop* loop_ )
{
	SoundSource* source = addLoop(buffer_, baseGainIndex_, gain_, loop_);
	if (source)
	{
		playSource(source);
	}
}

void add_loop( const buffer* buffer_, int baseGainIndex_, float gain_, const float* position_, float min_distance_, float max_distance_, loop* loop_ )
{
	SoundSource* source = addLoop(buffer_, baseGainIndex_, gain_, loop_);
	if (source)
	{
		mixVCopy(source->position, position_);
		source->flags |= SoundSourceFlags::Positional;
		source->minDistance = min_distance_;
		source->distanceDifference = max_distance_ - min_distance_;
		playSource(source);
	}
}

void remove_loop( loop loop_ )
{
	SoundSource* source = &g_mixer.sources[loop_ - 1];
	source->flags &= ~SoundSourceFlags::Looping;
	source->samplePos = source->buffer->nsamples;
}

void loop_set_position( loop loop_, const float* position_ )
{
	SoundSource* source = &g_mixer.sources[loop_ - 1];
	mixVCopy(source->position, position_);
}

void loop_fadeout( loop loop_, float seconds_ )
{
	SoundSource* source = &g_mixer.sources[loop_ - 1];
	source->fadeOutPerSample = 1.0f / (seconds_ * c_sampleRate);
	source->flags |= SoundSourceFlags::FadeOut;
}

void loop_set_gain( loop loop_, float gain_ )
{
	SoundSource* source = &g_mixer.sources[loop_ - 1];
	source->baseGain = gain_;
	source->flags &= ~SoundSourceFlags::FadeOut;
}

void loop_set_frequency( loop loop_, float frequency_ )
{
	SoundSource* source = &g_mixer.sources[loop_ - 1];

	const float diff = frequency_ - 1.0f;
	if (diff * diff < 1.0e-8)
	{
		source->flags &= ~SoundSourceFlags::Frequency;
	}
	else
	{
		source->frequency = frequency_;
		source->flags |= SoundSourceFlags::Frequency;
	}
}

void init( music_callback musicCallback_ )
{
	for (int ii = 0; ii < c_maxGainTypes; ++ii)
	{
		g_mixer.baseGain[ii] = 1.0f;
	}

	const float defaultThresholds[2] = {1.0f, 1.0f};
	const float defaultMultipliers[2] = {1.0f, 1.0f};
	const float defaultAttack = 0.0f;
	const float defaultRelease = 0.0f;
	effects_compressor(defaultThresholds, defaultMultipliers, defaultAttack, defaultRelease);
	g_mixer.compressorFactor = c_gainQuantization;
	g_mixer.lastCompressionSamples[0] = g_mixer.lastCompressionSamples[1] = 0;
	g_mixer.musicCallback = musicCallback_;
	g_mixer.samplesLeft = 0;
}

void update_listener( const float* position_ )
{
	mixVCopy(g_mixer.position, position_);
}

void set_basegain( int index_, float gain_ )
{
	g_mixer.baseGain[index_] = gain_;
}

void set_music_gain( float gain_ )
{
	g_mixer.musicGain = gain_;
}

void effects_compressor( const float threshold[2], const float multiplier[2], float attackSeconds_, float releaseSeconds_ )
{
	g_mixer.compressorThresholds[0] = (int)((SHRT_MAX + 1) * mixClamp(threshold[0], 0.0f, 1.0f));
	g_mixer.compressorThresholds[1] = (int)((SHRT_MAX + 1) * mixClamp(threshold[1], 0.0f, 1.0f));
	g_mixer.compressorMultipliers[0] = (int)(c_gainQuantization * mixClamp(multiplier[0], 0.0f, 1.0f));
	g_mixer.compressorMultipliers[1] = (int)(c_gainQuantization * mixClamp(multiplier[1], 0.0f, 1.0f));
	g_mixer.compressorAttackPer1KSamples = (int)(1000.0f * c_gainQuantization / (attackSeconds_ * c_sampleRate));
	g_mixer.compressorReleasePer1KSamples = (int)(1000.0f * c_gainQuantization / (releaseSeconds_ * c_sampleRate));
}

}
