#pragma once

//stdlib
#include <vector>

// Class similar to DynamicLines, but specifically made to render the "Normal Ticks"
// and "Tangent Ticks" for visual debugging. The lines representing the normal ticks
// should be normalized to a fixed length, instead of being scaled 

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Class similar to DynamicLines, but specifically made to render the "Normal Ticks"
/// and "Tangent Ticks" for visual debugging. The lines representing the normal ticks
/// should be normalized to a fixed length, instead of being scaled 
/// </summary>
///////////////////////////////////////////////////////////////////////////////
class NormalizedLines : public DynamicRenderable
{
    typedef Ogre::RenderOperation::OperationType OperationType;

public:
    /// Constructor - see setOperationType() for description of argument.
    NormalizedLines(pcstr MaterialName="BaseWhiteNoLighting");

    virtual ~NormalizedLines();

    /// <summary cref="NormalizedLines::addPoint">
    /// Add a normal to the list
    /// </summary>
    /// <param name="pos">const Math::Vector3& - Location of normal</param>
    /// <param name="norm">const Math::Vector3& - Normal vector</param>
    void addPoint(const Math::Vector3& pos, const Math::Vector3& norm);

    /// <summary cref="NormalizedLines::getNumPoints">
    /// Return the total number of normals
    /// </summary>
    /// <returns>u16 - number of normals</returns>
    u16 getNumPoints(void) const;

    /// <summary cref="NormalizedLines::clear">
    /// Remove all normals from the lise
    /// </summary>
    void clear();

    /// <summary cref="NormalizedLines::update">
    /// Call this to update the hardware buffer after making changes 
    /// </summary>
    void update();

protected:
    /// <summary cref="NormalizedLines::createVertexDeclaration">
    /// Implementation DynamicRenderable, creates a simple position and normal-only decl
    /// </summary>
    virtual void createVertexDeclaration();

    /// <summary cref="NormalizedLines::fillHardwareBuffers">
    /// Implementation DynamicRenderable, pushes point list out to hardware memory
    /// </summary>
    virtual void fillHardwareBuffers();

    std::vector<Math::Vector3> m_Points;
    std::vector<Math::Vector3> m_Normals;
    Bool m_bDirty;
};


