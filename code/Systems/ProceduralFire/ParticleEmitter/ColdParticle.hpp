#pragma once

// Base
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
// Standard library
#include <cstdint>
#include <list>
#include <vector>
// System
#include "Systems/ProceduralFire/aabb.h"
#include "Systems/ProceduralFire/HeatParticle.h"
#include "Systems/ProceduralFire/PSystem.h"

namespace ParticleEmitter
{
    // A class that extends ParticleSystemWithEmitter to simulate cold emission (e.g. water)
    class ColdParticle : public ParticleSystemWithEmitter
    {
    public:
        Base::Vector3 mParticleRadius;
        /*Base::Vector3 mCameraPos;*/
        float mInitialSize;
        Base::Vector3 m_LastPos;
        HeatEmitter * m_pColdParticles;

        ColdParticle( const Base::Vector3& accelbias, const Base::Vector3& accelshift, const Base::Vector3 Radius, const Base::Vector3 pos,
            const aabb& box, int maxNumParticles, float timePerParticle, float minLifeTime = 1.0f,
            float maxLifeTime = 2.0f, float minSize = 10.0f, float maxSize = 15.0f, float minAmplitude = 1.0f,
            float maxAmplitude = 2.0f
        )
            : ParticleSystemWithEmitter(
                HeatEmitter::Type_Water, accelbias, accelshift, pos, box, 
                maxNumParticles, timePerParticle, minLifeTime, 
                maxLifeTime, minSize, maxSize, minAmplitude, maxAmplitude )
            , mParticleRadius(Radius)
            , m_LastPos(mInitPos)
            , m_pColdParticles(GetEmitter())
        {}

        ~ColdParticle() {}

        void setInitPos(Base::Vector3 pos) { mInitPos = pos; }
        void setRadius(Base::Vector3 radius) { mParticleRadius = radius; }
        void initParticle(Particle& out);
        virtual void reinitialize(){};
        virtual void update(float dt);
    };
}
