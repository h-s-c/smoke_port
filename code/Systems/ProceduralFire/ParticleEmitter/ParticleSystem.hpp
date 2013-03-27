//=============================================================================
// PSystem.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

// This file was originally inspired by Frank Luna from his book 
// Introduction to Game Programming with DX9 a shader approach.  
// It is likely to be unrecognizable from Franks original work but the
// intent is to give credit for the basic inspiration for the particle system structure 
// that I developed.  I would encourage you to get a copy of his book as it might very
// well inspire you as it did me.

//Writen by Hugh Smith summer and fall 2007
// Part of Smoke Framework implementation for procedural Fire system.

// Copyright © 2008-2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
#pragma once

// Most of the methods in this namespace are invoked from the deeply nested loops, 
// and thus it is critical for them to bear as little overhead as possible. To this
// end they should be made inline and non-virtual wherever possible.
// 
namespace ParticleEmitter {

struct ParticleBase
{
    Base::Vector3 initialPos;
    Base::Vector3 initialVelocity;
    float       initialSize; // In pixels.
    float       initialTime;
    float       lifeTime;
    float       mass;
    std::uint32_t       initialColor;
};

struct Particle : public ParticleBase
{
    float       age;
};

//===============================================================
// Type P is supposed to be derived from ParticleBase
template<typename P>
class ParticleSystemBase
{
public:
    ParticleSystemBase () {}

    ParticleSystemBase (
            const Base::Vector3& accelbias, 
            const Base::Vector3& accelshift, 
            const AABB& box,
            int maxNumParticles,
            float timePerParticle,
            float minLifeTime = 1.0f,
            float maxLifeTime = 2.0f,
            float minSize = 10.0f,
            float maxSize = 15.0f,
            float minAmplitude = 1.0f,
            float maxAmplitude = 2.0f)
         : mAccelImpulse(accelbias), mAccelShift(accelshift)
         , mBox(box), mTime(0.0f), mPrevTime(0.0f)
         , mMaxNumParticles(maxNumParticles), mTimePerParticle(timePerParticle)
         , mMinLifeTime(minLifeTime), mMaxLifeTime(maxLifeTime) 
         , mMinSize(minSize), mMaxSize(maxSize)
         , mMinAmplitude(minAmplitude), mMaxAmplitude(maxAmplitude)
    {
        init();
        mIsActive = false;
    }
    
    // Access Methods
    void setImpulse(Base::Vector3 accel)
    {
        mAccelImpulse = accel;
    }
    void setShift(Base::Vector3 shift)
    {
        mAccelShift = shift;
    }

    float getTime() const
    {
        return mPrevTime;
    }
    void  setTime(float t)
    {
        mTime = t;
    }

    const AABB& getAABB() const
    {
        return mBox;
    }

    void setActive()
    {
        mIsActive = true;
    }
    void setInactive()
    {
        mIsActive = false;
    }
    Bool isActive() const
    {
        return mIsActive;
    }

    u32 getAliveParticleCount() const
    {
        return (u32)mAliveParticles.size();
    }

    const std::vector<P*>& getAliveParticles() const
    {
        return mAliveParticles;
    }

    void getAliveParticles( std::vector<P> &particles ) const
    {
        size_t size = mAliveParticles.size();
        for( size_t i = 0; i < size; ++i )
        {
            particles.push_back(*mAliveParticles[i]);
        }
    }

protected:
    void init()
    {
        // Allocate memory for maximum number of particles.
        mParticles.resize(mMaxNumParticles);
        mAliveParticles.reserve(mMaxNumParticles);
        mDeadParticles.reserve(mMaxNumParticles);

        // They start off all dead.
        for(int i = 0; i < mMaxNumParticles; ++i)
        {
            mParticles[i].lifeTime = -1.0f;
            mParticles[i].initialTime = 0.0f;
        }
    }
    float mPrevTime;

    Base::Vector3 mAccelImpulse;
    Base::Vector3 mAccelShift;

    Bool mIsActive;
    float mTime;
    AABB mBox;
    int mMaxNumParticles;
    float mTimePerParticle;
    float mMinLifeTime;
    float mMaxLifeTime;
    float mMinSize;
    float mMaxSize;
    float mMinAmplitude;
    float mMaxAmplitude;

    std::vector<P> mParticles;
    std::vector<P*> mAliveParticles;
    std::vector<P*> mDeadParticles; 

}; // template class ParticleSystemBase

class ParticleSystem : public ParticleSystemBase<Particle>
{
public:
    ParticleSystem(
        const Base::Vector3& accelbias, 
        const Base::Vector3& accelshift, 
        const AABB& box,
        int maxNumParticles,
        float timePerParticle,
        float minLifeTime = 1.0f,
        float maxLifeTime = 2.0f,
        float minSize = 10.0f,
        float maxSize = 15.0f,
        float minAmplitude = 1.0f,
        float maxAmplitude = 2.0f)
        : ParticleSystemBase(accelbias, accelshift, box, maxNumParticles, timePerParticle,
                    minLifeTime, maxLifeTime, minSize, maxSize, minAmplitude, maxAmplitude)
    {}

    virtual ~ParticleSystem() {}

    virtual void initParticle(Particle& out) = 0;
    virtual void update(float dt);
    virtual void reinitialize();

    void addParticle();

    // Returns a random Vector.
    static void GetRandomVec(Base::Vector3& out)
    {
        out.x = Base::Random::GetRandomFloat(-1.0f, 1.0f);
        out.y = Base::Random::GetRandomFloat(-1.0f, 1.0f);
        out.z = Base::Random::GetRandomFloat(-1.0f, 1.0f);

        // Project onto unit sphere.
        out.Normalize();
    }
};

} //end namespace ParticleEmitter
