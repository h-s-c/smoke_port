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

#include "Systems/ProceduralFire/ParticleEmitter/HeatParticle.hpp"

class B3
{
public:
    B3(){};
    B3(Base::Vector3 i,Base::Vector3 j, Base::Vector3 k)
    { 
        iAxis = i;
        jAxis = j;
        kAxis = k;
    };
    void normalize()
    {
        iAxis.Normalize();
        jAxis.Normalize();
        kAxis.Normalize();
    };
    Base::Vector3 iAxis;
    Base::Vector3 jAxis;
    Base::Vector3 kAxis;

};

namespace ParticleEmitter{


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   A class that extends ParticleSystemWithEmitter to represent a patch of fire.
/// </summary>
///////////////////////////////////////////////////////////////////////////////
class FirePatch : public ParticleSystemWithEmitter
{
public:
    B3 mBasis;

    FirePatch(
        const Base::Vector3& accelbias, 
        const Base::Vector3& accelshift, 
        const B3 basis,
        const Base::Vector3 pos, 
        const AABB& box,
        int maxNumParticles,
        float timePerParticle,
        float minLifeTime = 1.0f,
        float maxLifeTime = 2.0f,
        float minSize = 10.0f,
        float maxSize = 15.0f,
        float minAmplitude = 1.0f,
        float maxAmplitude = 2.0f
        )
        : ParticleSystemWithEmitter(
            HeatEmitter::Type_Fire,
            accelbias, accelshift, pos, box, 
            maxNumParticles, timePerParticle,
            minLifeTime, maxLifeTime, 
            minSize, maxSize,
            minAmplitude, maxAmplitude)
        , mBasis(basis)
    {}

    void setInitPos(Base::Vector3 pos)
    {
        mInitPos = pos;
    }
    void setBasis(B3 basis)
    {
        mBasis = basis;
    }
    void normalizeBasis()
    {
        mBasis.normalize();
    }
    virtual void reinitialize();
    void initParticle(Particle& out);
};

} //end namespace
