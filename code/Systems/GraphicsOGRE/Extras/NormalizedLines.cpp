//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//system
#include "Systems/GraphicsOGRE/Extras/DynamicRenderable.hpp"
#include "Systems/GraphicsOGRE/Extras/NormalizedLines.hpp"

NormalizedLines::NormalizedLines(pcstr pcstrMaterialName)
{
    initialize(Ogre::RenderOperation::OT_LINE_LIST, false);
    setMaterial(pcstrMaterialName);
    m_bDirty = true;
}

NormalizedLines::~NormalizedLines()
{
}

void NormalizedLines::addPoint(const Base::Vector3& pos, const Base::Vector3& norm)
{
    m_Points.push_back(pos);
    m_Normals.push_back(norm);
    m_bDirty = true;
}

u16 NormalizedLines::getNumPoints(void) const
{
    return (u16)m_Points.size();
}

void NormalizedLines::clear()
{
    m_Points.clear();
    m_Normals.clear();
    m_bDirty = true;
}

void NormalizedLines::update()
{
    if (m_bDirty) {
        fillHardwareBuffers();
    }
}

void NormalizedLines::createVertexDeclaration()
{
    Ogre::VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration;

    size_t offset = 0;
    decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
}

void NormalizedLines::fillHardwareBuffers()
{
    size_t size = m_Points.size();

    prepareHardwareBuffers(size,0);

    if (!size) { 
        mBox.setExtents(Ogre::Vector3::ZERO, Ogre::Vector3::ZERO);
        m_bDirty=false;
        return;
    }

    Base::Vector3 vaabMin = m_Points[0];
    Base::Vector3 vaabMax = m_Points[0];

    Ogre::HardwareVertexBufferSharedPtr vbuf =
        mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);

    f32 *prPos = static_cast<f32*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));
    {
        for(size_t i = 0; i < size; i++)
        {
            // Fill Position data
            *prPos++ = m_Points[i].x;
            *prPos++ = m_Points[i].y;
            *prPos++ = m_Points[i].z;

            // Fill Normal data
            *prPos++ = m_Normals[i].x;
            *prPos++ = m_Normals[i].y;
            *prPos++ = m_Normals[i].z;

            vaabMin.x = Base::Min(m_Points[i].x, vaabMin.x);
            vaabMin.y = Base::Min(m_Points[i].y, vaabMin.y);
            vaabMin.z = Base::Min(m_Points[i].z, vaabMin.z);

            vaabMax.x = Base::Max(m_Points[i].x, vaabMax.x);
            vaabMax.y = Base::Max(m_Points[i].y, vaabMax.y);
            vaabMax.z = Base::Max(m_Points[i].z, vaabMax.z);
        }
    }
    vbuf->unlock();

    mBox.setExtents(vaabMin.x, vaabMin.y, vaabMin.z,
        vaabMax.x, vaabMax.y, vaabMax.z);

    m_bDirty = false;
}
