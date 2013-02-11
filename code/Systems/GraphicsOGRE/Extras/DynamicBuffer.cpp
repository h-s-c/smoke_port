//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
//system
#include "Systems/GraphicsOGRE/Extras/DynamicBuffer.hpp"

// NOTE: this setting reveals some concurrency errors
//#define USE_INFINITE_BOXES 1

DynamicBuffer::DynamicBuffer(void)
{
  // Clear members to initial state
  mVertexBufferCapacity = 0;
  mIndexBufferCapacity = 0;
  mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
  mRenderOp.useIndexes = false;
  mRenderOp.vertexData = new Ogre::VertexData;
  ASSERT( mRenderOp.vertexData != NULL );

  mRenderOp.vertexData->vertexDeclaration->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
  setMaterial("BaseWhiteNoLighting");

  // NOTE: although using an infinite bounding box should work just fine,
  // this reveals a problem with concurrency where the mBox is not updated before rendering.
  // We're ignoring this problem for now and choosing a zero box as a fail-soft case.
  #ifdef USE_INFINITE_BOXES
  mBox.setExtents( Ogre::Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX),
                   Ogre::Vector3(FLT_MAX, FLT_MAX, FLT_MAX) );
  #else
  mBox.setExtents( Ogre::Vector3(0.0f, 0.0f, 0.0f),
                   Ogre::Vector3(0.0f, 0.0f, 0.0f) );
  #endif

  m_Distance = 0;
}

DynamicBuffer::~DynamicBuffer()
{
  delete mRenderOp.vertexData;
}

void DynamicBuffer::setDistanceFromCamera(int dis)
{
 m_Distance = dis;

 return;
}

void 
DynamicBuffer::setOperationType(Ogre::RenderOperation::OperationType opType)
{
  mRenderOp.operationType = opType;
}

Ogre::RenderOperation::OperationType 
DynamicBuffer::getOperationType() const
{
  return mRenderOp.operationType;
}

Ogre::VertexDeclaration* DynamicBuffer::getVertexDeclaration(void)
{
  return mRenderOp.vertexData->vertexDeclaration;
}


void DynamicBuffer::setVertexDeclaration(Ogre::VertexDeclaration* VD)
{
  mRenderOp.vertexData->vertexDeclaration = VD;
}


void DynamicBuffer::prepareHardwareBuffers(size_t vertexCount, 
                                           size_t indexCount)
{
  // Prepare vertex buffer
  size_t newVertCapacity = mVertexBufferCapacity;
  if ((vertexCount > mVertexBufferCapacity) ||
      (mVertexBufferCapacity == 0))
  {
    // vertexCount exceeds current capacity!
    // It is necessary to reallocate the buffer.

    // Check if this is the first call
    if (newVertCapacity == 0)
      newVertCapacity = 1;

    // Make capacity the next power of two
    while (newVertCapacity < vertexCount)
      newVertCapacity <<= 1;
  }
  else if (vertexCount < mVertexBufferCapacity>>1) {
    // Make capacity the previous power of two
    while (vertexCount < newVertCapacity>>1)
      newVertCapacity >>= 1;
  }

  if (newVertCapacity != mVertexBufferCapacity) 
  {
    mVertexBufferCapacity = newVertCapacity;
    // Create new vertex buffer
    Ogre::HardwareVertexBufferSharedPtr vbuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
        mVertexBufferCapacity,
        Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?

    // Bind buffer
    mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vbuf);
  }
  // Update vertex count in the render operation
  mRenderOp.vertexData->vertexCount = vertexCount;

  if (mRenderOp.useIndexes && (indexCount > 0) )
  {
    ASSERT( indexCount <= std::numeric_limits<unsigned short>::max() );

    size_t newIndexCapacity = mIndexBufferCapacity;
    // Prepare index buffer
    if ((indexCount > newIndexCapacity) ||
        (newIndexCapacity == 0))
    {
      // indexCount exceeds current capacity!
      // It is necessary to reallocate the buffer.

      // Check if this is the first call
      if (!newIndexCapacity)
        newIndexCapacity = 1;

      // Make capacity the next power of two
      while (newIndexCapacity < indexCount)
        newIndexCapacity <<= 1;

    }
    else if (indexCount < newIndexCapacity>>1) 
    {
      // Make capacity the previous power of two
      while (indexCount < newIndexCapacity>>1)
        newIndexCapacity >>= 1;
    }

    if (newIndexCapacity != mIndexBufferCapacity)
    {
      mIndexBufferCapacity = newIndexCapacity;
      // Create new index buffer
      mRenderOp.indexData->indexBuffer =
          Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
          Ogre::HardwareIndexBuffer::IT_16BIT,
          mIndexBufferCapacity,
          Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
    }

    // Update index count in the render operation
    mRenderOp.indexData->indexCount = indexCount;
  }
}

void DynamicBuffer::fillHardwareBuffers()
{
    // Dummy Implementation to satisfy Dynamic Renderable
    ASSERT( False );
}

void DynamicBuffer::fillHardwareBuffers(void *p)
{
  size_t size = 0;

  prepareHardwareBuffers(size, 0);
  
  Ogre::Vector3 vaabMin = Ogre::Vector3(0.0f,0.0f,0.0f);
  Ogre::Vector3 vaabMax = Ogre::Vector3(0.0f,0.0f,0.0f);

  mBox.setExtents(vaabMin, vaabMax);

}

Ogre::Real DynamicBuffer::getBoundingRadius(void) const
{
    return Ogre::Math::Sqrt(Base::Max(mBox.getMaximum().squaredLength(), mBox.getMinimum().squaredLength()));
}

Ogre::Real DynamicBuffer::getSquaredViewDepth(const Ogre::Camera* pCam) const
{
   Ogre::Vector3 vMin, vMax, vMid, vDist;

   Ogre::Vector3 vWorld = pCam->getDerivedPosition();
   vMin = mBox.getMinimum();
   vMax = mBox.getMaximum();
   vMid = ((vMax - vMin) * 0.5) + vMin;

   //--------------------------------------------------------------------------
   // The bounding box coordinates are stored in object space and must be
   // translated to world space.
   //
   vMid.x += vWorld.x;
   vMid.y += vWorld.y;
   vMid.z += vWorld.z;

   //--------------------------------------------------------------------------
   // Just use the position, not the bounding box, for calculating the distance
   // from the camera. Combined with a skewing of the distance will force all
   // of the particles of a particular type to be drawn together for each tree.
   //
   vDist = pCam->getDerivedPosition() - vWorld;
  
   Ogre::Real tLength = vDist.squaredLength();
   tLength += m_Distance;
   return tLength;
}


void* DynamicBuffer::lockBuffer(void)
{
   void *pBuf;
   m_pVB = 
      mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);

   pBuf = static_cast<void*>(m_pVB->lock(Ogre::HardwareBuffer::HBL_DISCARD));
   ASSERT(pBuf != NULL);
   return pBuf;
}

void DynamicBuffer::unlockBuffer(void)
{
    m_pVB->unlock();
}

void DynamicBuffer::setExtents(::Base::Vector3 min, ::Base::Vector3 max)
{
    mBox.setExtents( Ogre::Vector3(min.x, min.y, min.z),
                     Ogre::Vector3(max.x, max.y, max.z) );
}
