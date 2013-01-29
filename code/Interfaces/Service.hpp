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

//
// forward declarations
//
class ISystemObject;

//
// service api
//
#include "Interfaces/Services/CollisionAPI.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Interface class for providing services to the systems.
/// </summary>
////////////////////////////////////////////////////////////////////////////////////////////////////

class IService
{
public:

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Interface class for providing access to other systems' ISystem, ISystemScene, and
    ///    ISystemObject.
    /// </summary>
    ////////////////////////////////////////////////////////////////////////////////////////////////

    class ISystemAccess
    {
    public:

        /// <summary>
        ///   Gets a system by its name.
        /// </summary>
        /// <param name="pszSystemName">The name of the system to get.</param>
        /// <returns>A handle to the system or null if the system doesn't exist.</returns>
        virtual Handle GetSystem( pcstr pszSystemName ) = 0;

        /// <summary>
        ///   Gets a system by its type.
        /// </summary>
        /// <param name="Type">The type of the system to get.</param>
        /// <returns>A handle to the system or null if the system doesn't exist.</returns>
        virtual Handle GetSystem( System::Type Type ) = 0;

        /// <summary>
        ///   Gets a system scene by its system name.
        /// </summary>
        /// <param name="pszSystemName">The name of the system scene to get.</param>
        /// <returns>A handle to the system scene or null if it doesn't exist.</returns>
        virtual Handle GetScene( pcstr pszSystemName ) = 0;

        /// <summary>
        ///   Gets a system scene by its system type.
        /// </summary>
        /// <param name="Type">The type of the system scene to get.</param>
        /// <returns>A handle to the system scene or null if it doesn't exist.</returns>
        virtual Handle GetScene( System::Type Type ) = 0;

        /// <summary>
        ///   Gets a system object by its system name.
        /// </summary>
        /// <param name="pszSystemName">The name of the system object to get.</param>
        /// <param name="pszName">The name of the object.</param>
        /// <returns>A handle to the system object or null if it doesn't exist.</returns>
        virtual Handle GetSystemObject( pcstr pszSystemName, pcstr pszName ) = 0;

        /// <summary>
        ///   Gets a system object by its system type and name.
        /// </summary>
        /// <param name="Type">The type of the system object to get.</param>
        /// <param name="pszName">Name of the object.</param>
        /// <returns>A handle to the system object or null if it doesn't exist.</returns>
        virtual Handle GetSystemObject( System::Type Type, pcstr pszName ) = 0;

        /// <summary>
        ///   Gets a single system property.
        /// </summary>
        /// <param name="hSystem">The handle of the system.</param>
        /// <param name="Property">The property to store the data in.  The name of the property
        ///  must be set prior to entering this function.</param>
        virtual void GetSystemProperty( Handle hSystem, InOut Properties::Property& Property ) = 0;

        /// <summary>
        ///   Sets a single system property.
        /// </summary>
        /// <param name="hSystem">The handle of the system.</param>
        /// <param name="Property">The property to containing the property data.  This must be
        ///  properly filled in prior to calling this function.</param>
        virtual void SetSystemProperty( Handle hSystem, In Properties::Property& Property ) = 0;

        /// <summary>
        ///   Gets a single system scene property.
        /// </summary>
        /// <param name="hScene">The handle of the system scene.</param>
        /// <param name="Property">The property to store the data in.  The name of the property
        ///  must be set prior to entering this function.</param>
        virtual void GetSceneProperty( Handle hScene, InOut Properties::Property& Property ) = 0;

        /// <summary>
        ///   Sets a single system scene property.
        /// </summary>
        /// <param name="hScene">The handle of the system scene.</param>
        /// <param name="Property">The property to containing the property data.  This must be
        ///  properly filled in prior to calling this function.</param>
        virtual void SetSceneProperty( Handle hScene, In Properties::Property& Property ) = 0;

        /// <summary>
        ///   Gets a single object property.
        /// </summary>
        /// <param name="hObject">The handle of the system object.</param>
        /// <param name="Property">The property to store the data in.  The name of the property
        ///  must be set prior to entering this function.</param>
        virtual void GetObjectProperty( Handle hObject, InOut Properties::Property& Property ) = 0;

        /// <summary>
        ///   Sets a single object property.
        /// </summary>
        /// <param name="hObject">The handle of the system object.</param>
        /// <param name="Property">The property to containing the property data.  This must be
        ///  properly filled in prior to calling this function.</param>
        virtual void SetObjectProperty( Handle hObject, In Properties::Property& Property ) = 0;
    };

    /// <summary>
    ///   Gets a reference to the ISystemAccess class.
    /// </summary>
    /// <returns>A reference to the ICollision class.</returns>
    virtual ISystemAccess& SystemAccess() = 0;

    /// <summary>
    ///   Used by a system/framework to register itself as a provider for ISystemAccess.
    /// </summary>
    /// <param name="pSystemAccess">A pointer to the provider to register.</param>
    virtual void RegisterSystemAccessProvider( ISystemAccess* pSystemAccess ) = 0;

    /// <summary>
    ///   Used by a system/framework to de-register itself as a provider for ISystemAccess.
    /// </summary>
    /// <param name="pSystemAccess">A pointer to the provider to de-register.</param>
    virtual void UnregisterSystemAccessProvider( ISystemAccess* pSystemAccess ) = 0;



    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// <summary>
    ///   Interface class for providing collision tests to the systems.
    /// </summary>
    ////////////////////////////////////////////////////////////////////////////////////////////////

    class ICollision
    {
    public:
        /// <summary>
        ///   Requests a collision test.
        /// </summary>
        /// <param name="Request">Collision request.</param>
        /// <returns>A handle used to get the results.</returns>
        virtual Coll::Handle Test( const Coll::Request& Request ) = 0;

        /// <summary>
        ///   Requests a collision line test.
        /// </summary>
        /// <param name="Position0">Vector position to start test.</param>
        /// <param name="Position1">Vector position to end test.</param>
        /// <param name="Request">Collision request.</param>
        /// <returns>A handle used to get the results.</returns>
        virtual Coll::Handle LineTest( const Math::Vector3& Position0, const Math::Vector3& Position1, Coll::Request& Request ) = 0;

        /// <summary>
        ///   Gets results for the given handle.
        /// </summary>
        /// <param name="Handle">Collision test handle.</param>
        /// <param name="Result">Pointer to structure to be filled with results.</param>
        /// <returns>Returns True if test has finished.</returns>
        virtual Bool Finalize( Coll::Handle Handle, Coll::Result* Result ) = 0;
    };

    /// <summary>
    ///   Gets a reference to the ICollision class.
    /// </summary>
    /// <returns>A reference to the ICollision class.</returns>
    virtual ICollision& Collision() = 0;

    /// <summary>
    ///   Used by a system/framework to register itself as a provider for ICollision.
    /// </summary>
    /// <param name="pCollision">A pointer to the provider to register.</param>
    virtual void RegisterCollisionProvider( ICollision* pCollision ) = 0;

    /// <summary>
    ///   Used by a system/framework to de-register itself as a provider for ICollision.
    /// </summary>
    /// <param name="pCollision">A pointer to the provider to de-register.</param>
    virtual void UnregisterCollisionProvider( ICollision* pCollision ) = 0;
};
