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

#ifndef TINYMIXER_H
#define TINYMIXER_H

namespace tinymixer {

struct buffer;
typedef unsigned char loop;

typedef void (*music_callback)(int* samples_, int nsamples_, float gain_);


// only accepts 16-bit 44100Hz PCM audio
void create_buffer( int channels_, const void* pcm_data_, int pcm_data_size_, const buffer** buffer_ );
void release_buffer( const buffer* buffer_ );

void add( const buffer* buffer_, int baseGainIndex_, float gain_ );
void add( const buffer* buffer_, int baseGainIndex_, float gain_, const float* position_, float min_distance_, float max_distance_ );

void add_loop( const buffer* buffer_, int baseGainIndex_, float gain_, loop* loop_ );
void add_loop( const buffer* buffer_, int baseGainIndex_, float gain_, const float* position_, float min_distance_, float max_distance_, loop* loop_ );
void remove_loop( loop loop_ );
void loop_set_position( loop loop_, const float* position_ );
void loop_set_gain( loop loop_, float gain_ );
void loop_fadeout( loop loop_, float seconds_ );
void loop_set_frequency( loop loop_, float frequency_ );

void init( music_callback musicCallback_ );
void update_listener( const float* position_ );
void set_basegain( int index_, float gain_ );
void set_music_gain( float gain_ );
void getsamples( short* samples_, int nsamples_ );

void effects_compressor( const float threshold[2], const float multiplier[2], float attackSeonds_, float delaySeconds_ );

}

#endif
