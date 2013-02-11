#pragma once

// Base
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
// Standard Library
#include <cstdint>
#include <cstring> // memset
#include <string>

// Collision Data Types
namespace Collision
{
    // Collision Handle
    typedef std::uint32_t Handle;
    static const Handle InvalidHandle = std::uint32_t(-1);

    // Collision Test Types
    enum Type
    {
        e_None,
        e_LineTest,

        e_Invalid
    };

    enum Flags
    {
        e_Defaults     = 0x00000000,  // No flags set (use defaults)
        e_Ground       = 0x00000001,  // Only test against the ground
        e_IgnoreGround = 0x00000002,  // Exclude the ground from tests
    };

    // Collision Request
    struct Request
    {
        Base::Vector3 m_Position0;  // Start position of the test
        Base::Vector3 m_Position1;  // End position of the test
        Type          m_Type;       // Type of test
        Handle        m_Handle;     // Unique handle for this request
        std::string   m_Ignore;     // Name of object to ignore in collision
        Flags         m_Flags;      // Flags (see Collision::Flags)

        Request() { std::memset( this, 0, sizeof( *this ) ); }
        void SetIgnore( std::string ignore ) { m_Ignore = ignore; }
        void SetFlags( Collision::Flags Flags ) { m_Flags = Flags; }
    };

    // Collision Result
    struct Result
    {
        Base::Vector3   m_Position;   // Contact position
        Base::Vector3   m_Normal;     // Normal vector (vector pernedicular to contact surface)
        std::string     m_Hit;        // Name of object hit (null is no collision detected)
        float           m_Depth;      // Penetration depth (along normal vector)
        std::uint32_t   m_Finalized;  // Collision test has finished (0 = no, 1 = yes, >1 = delete)
        bool            m_Valid;      // A valid collision was detected
    };
}
