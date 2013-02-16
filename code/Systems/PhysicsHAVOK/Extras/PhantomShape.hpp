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

// External
#include <Physics/Collide/Shape/Misc/PhantomCallback/hkpPhantomCallbackShape.h>

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   <c>PhantomShape</c> Extends hkpPhantomCallbackShape to implement Phantom
///   tracking.  See Havok documentation for more details.  
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class PhantomShape : public hkpPhantomCallbackShape
{
public:

    PhantomShape( HavokPhysicsScene* pSystemScene );
    ~PhantomShape( void );


protected:

    /// <summary cref="PhantomShape::phantomEnterEvent">
    ///   Callback that handle objects entering this phantom
    ///   (see Havok documentation for more details)
    /// </summary>
    /// <param name="pPhantomCollidable">Phantom that was entered</param>
    /// <param name="pOtherCollidable">Collidable that entered the Phantom</param>
	/// <param name="CollisionInput">Collision input</param>
    virtual void phantomEnterEvent( const hkpCollidable* pPhantomCollidable,
                                    const hkpCollidable* pOtherCollidable,
                                    const hkpCollisionInput& CollisionInput );
 
    /// <summary cref="PhantomShape::phantomLeaveEvent">
    ///   Callback that handle objects leaving this phantom
    ///   (see Havok documentation for more details)
    /// </summary>
    /// <param name="pPhantomCollidable">Phantom that was departed</param>
    /// <param name="pOtherCollidable">Collidable that is leaving the Phantom</param>
    virtual void phantomLeaveEvent( const hkpCollidable* pPhantomCollidable,
                                    const hkpCollidable* pOtherCollidable );

    HavokPhysicsScene*              m_pSystemScene;
};

