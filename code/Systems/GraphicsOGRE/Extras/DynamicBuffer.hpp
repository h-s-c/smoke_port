#pragma once
//external
#include <OgreSimpleRenderable.h>

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   Class to provide a SimpleRenderable wrapper for a dynamically changing
///   vertex buffer.
/// </summary>
///////////////////////////////////////////////////////////////////////////////

class DynamicBuffer : public Ogre::SimpleRenderable
{
public:
    /// Constructor
    DynamicBuffer();
    /// Destructor
    ~DynamicBuffer();

    /// <summary cref="DynamicBuffer::getBoundingRadius">
    /// Implementation of Ogre::getBoundingRadius  
    /// </summary>
    Ogre::Real getBoundingRadius(void) const;

    /// <summary cref="DynamicBuffer::getSquaredViewDepth">
    /// Implementation of Ogre::getSquaredViewDepth  
    /// </summary>
    Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

    /// <summary cref="DynamicBuffer::setVertexDeclaration">
    /// Creates the vertex declaration. 
    /// </summary>
    void setVertexDeclaration(Ogre::VertexDeclaration* VD);

    /// <summary cref="DynamicBuffer::getVertexDeclaration">
    /// Returns the vertex declaration. 
    /// </summary>
    Ogre::VertexDeclaration* getVertexDeclaration(void);
  
    /// <summary cref="DynamicBuffer::setOperationType">
    /// Set the type of operation to draw with.
    /// </summary>
    /// <param name="opType">opType Can be one of
    ///  RenderOperation::OT_LINE_STRIP, 
    ///  RenderOperation::OT_LINE_LIST, 
    ///  RenderOperation::OT_POINT_LIST, 
    ///  RenderOperation::OT_TRIANGLE_LIST, 
    ///  RenderOperation::OT_TRIANGLE_STRIP, 
    ///  RenderOperation::OT_TRIANGLE_FAN...
    ///  The default is OT_LINE_STRIP. </param>
    void setOperationType(Ogre::RenderOperation::OperationType opType);

    /// <summary cref="DynamicBuffer::prepareHardwareBuffers">
    /// Prepares the hardware buffers for the requested vertex and index counts.
    /// This function must be called before locking the buffers in
    /// fillHardwareBuffers(). It guarantees that the hardware buffers
    /// are large enough to hold at least the requested number of
    /// vertices and indices (if using indices). The buffers are
    /// possibly reallocated to achieve this.
    ///
    /// The vertex and index count in the render operation are set to
    /// the values of vertexCount and indexCount respectively.
    /// </summary>
    /// <param name="vertexCount">The number of vertices the buffer must hold.</param>
    /// <param name="indexCount">The number of indices the buffer must hold. This parameter is ignored if not using indices.</param>
    void prepareHardwareBuffers(size_t vertexCount, size_t indexCount = 0);

    /// <summary cref="DynamicBuffer::fillHardwareBuffers">
    /// Fills the hardware vertex and index buffers with data.
    ///
    /// This function must call prepareHardwareBuffers() before locking
    /// the buffers to ensure the they are large enough for the data to
    /// be written. Afterwards the vertex and index buffers (if using
    /// indices) can be locked, and data can be written to them.
    /// </summary>
    virtual void fillHardwareBuffers();

    /// <summary cref="DynamicBuffer::fillHardwareBuffers">
    /// Fills the hardware vertex and index buffers with data.
    ///
    /// This function must call prepareHardwareBuffers() before locking
    /// the buffers to ensure the they are large enough for the data to
    /// be written. Afterwards the vertex and index buffers (if using
    /// indices) can be locked, and data can be written to them.
    /// </summary>
    virtual void fillHardwareBuffers(void *p);

    /// <summary cref="DynamicBuffer::getOperationType">
    /// Returns mRenderOp.operationType
    /// </summary>
    /// <returns>Ogre::RenderOperation::OperationType</returns>
    virtual Ogre::RenderOperation::OperationType getOperationType() const;

    /// <summary cref="DynamicBuffer::lockBuffer">
    /// Locks the buffer; returns a pointer to the locked buffer.
    /// </summary>
    /// <returns>void* - a pointed to the locked buffer</returns>
    void* lockBuffer(void);

    /// <summary cref="DynamicBuffer::unlockBuffer">
    /// Unlocks the buffer
    /// </summary>
    void  unlockBuffer(void);

    /// <summary cref="DynamicBuffer::setExtents">
    /// Set the bounds for the buffer
    /// </summary>
    /// <param name="AABBmin">Mins of the Axis Aligned Bounding Box.</param>
    /// <param name="AABBmax">Maxs of the Axis Aligned Bounding Box.</param>
    void  setExtents(Base::Vector3 AABBmin, Base::Vector3 AABBmax);


    /// <summary cref="DynamicBuffer::setDistanceFromCamera">
    /// Set a modifier to the distance from the camera. This amount will be added
    /// to the actual distance.
    /// </summary>
    /// <param name="dis">Distance modifier.</param>
    void  setDistanceFromCamera( int dis = 0 );

protected:
    /// Maximum capacity of the currently allocated vertex buffer.
    size_t mVertexBufferCapacity;
    /// Maximum capacity of the currently allocated index buffer.
    size_t mIndexBufferCapacity;

    Ogre::HardwareVertexBufferSharedPtr m_pVB;

    // Distance from camera
    int m_Distance;
};

