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

class DynamicBuffer;
class OGREGraphicsSystem;
class OGREGraphicsScene;
class FireObject;


///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   A class that extends OGREGraphicsObject for particle system objects.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class OGREGraphicsObjectParticles : public OGREGraphicsObject, public IGraphicsParticleObject
{
    friend OGREGraphicsSystem;
    friend OGREGraphicsScene;


protected:

    OGREGraphicsObjectParticles( ISystemScene* pSystemScene, pcstr pszName );
    ~OGREGraphicsObjectParticles( void );

    /// <summary cref="OGREGraphicsObjectParticles::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectParticles::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObject::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectParticles::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the object</param>
    /// <seealso cref="OGREGraphicsObject::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="OGREGraphicsObjectParticles::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the object.</returns>
    /// <seealso cref="OGREGraphicsObject::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectParticles::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::ChangeOccurred"/>
    virtual Error ChangeOccurred( ISubject* pSubject, System::Changes::BitMask ChangeType );

    /// <summary cref="OGREGraphicsObjectParticles::GetPotentialSystemChanges">
    ///   Implementation of the <c>ISubject::GetPotentialSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Changes::BitMask - Returns systems changes possible for this object.</returns>
    /// <seealso cref="OGREGraphicsObject::GetPotentialSystemChanges"/>
    virtual System::Changes::BitMask GetPotentialSystemChanges( void );

    /// <summary cref="OGREGraphicsObjectParticles::GetParticleGroupCount">
    ///   Implementation of the IGraphicsParticleObject GetParticleGroupCount function.
    /// </summary>
    /// <seealso cref="IGraphicsParticleObject::GetParticleGroupCount"/>
    virtual u32 GetParticleGroupCount( void );

    /// <summary cref="OGREGraphicsObjectParticles::GetParticleGroup">
    ///   Implementation of the IGraphicsParticleObject GetParticleGroup function.
    /// </summary>
    /// <seealso cref="IGraphicsParticleObject::GetParticleGroup"/>
    virtual void GetParticleGroup( u32 iParticleGroup, Out ParticleGroupData& ParticleGroup );

    /// <summary cref="OGREGraphicsObjectParticles::GetParticleCount">
    ///   Implementation of the IGraphicsParticleObject GetParticleCount function.
    /// </summary>
    /// <seealso cref="IGraphicsParticleObject::GetParticleCount"/>
    virtual u32 GetParticleCount( u32 iParticleGroup );

    /// <summary cref="OGREGraphicsObjectParticles::GetParticles">
    ///   Implementation of the IGraphicsParticleObject GetParticles function.
    /// </summary>
    /// <seealso cref="IGraphicsParticleObject::GetParticles"/>
    virtual void GetParticles( u32 iParticleGroup, Out ParticleData* pParticles );
 
    /// <summary cref="OGREGraphicsObjectParticles::GetParticleGroupTechnique">
    ///   Implementation of the IGraphicsParticleObject GetParticleGroupTechnique function.
    /// </summary>
    /// <seealso cref="IGraphicsParticleObject::GetParticleGroupTechnique"/>
    virtual std::string GetParticleGroupTechnique(void);

    /// <summary cref="OGREGraphicsObjectParticles::GetVertexDeclarationCount">
    ///   Implementation of the IGraphicsObject GetVertexDeclarationCount function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertexDeclarationCount"/>
    virtual u32 GetVertexDeclarationCount( In i32 BufferBindingIndex = -1 );

    /// <summary cref="OGREGraphicsObjectParticles::GetVertexDeclaration">
    ///   Implementation of the IGraphicsObject GetVertexDeclaration function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertexDeclaration"/>
    virtual void GetVertexDeclaration( Out VertexDecl::Element* pVertexDecl, In i32 BufferBindingIndex = -1 );

    /// <summary cref="OGREGraphicsObjectParticles::GetVertexCount">
    ///   Implementation of the IGraphicsObject GetVertexCount function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertexCount"/>
    virtual u32 GetVertexCount( void );

    /// <summary cref="OGREGraphicsObjectParticles::GetVertices">
    ///   Implementation of the IGraphicsObject GetVertices function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetVertices"/>
    virtual void GetVertices( Out void* pVertices );

    /// <summary cref="OGREGraphicsObjectParticles::GetAABB">
    ///   Implementation of the IGraphicsObject GetAABB function.
    /// </summary>
    /// <seealso cref="IGraphicsObject::GetAABB"/>
    virtual void GetAABB( Out Math::Vector3& Min, Out Math::Vector3& Max );

protected:

    static u32                          sm_EntityId;

    //
    // Entity Graphics Object
    //
    enum PropertyTypes
    {
        Property_PointList, Property_ProceduralPointList,
        Property_Material, Property_CastShadows, 
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    Ogre::VertexDeclaration*            m_pOgreVertexDecl;
    DynamicBuffer*                      m_pDynamicObject;

    static const f32                    m_fFireBBHeight;
};



