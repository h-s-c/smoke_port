//core
#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
//interface
#include "Interfaces/Interface.hpp"
//external
#include <Ogre.h>
#include <OgreBorderPanelOverlayElement.h>
#include <OgreHardwarePixelBuffer.h>
//system
#include "Systems/GraphicsOGRE/Object.hpp"
#include "Systems/GraphicsOGRE/ObjectWindow.hpp"
#include "Systems/GraphicsOGRE/Scene.hpp"
#include "Systems/GraphicsOGRE/System.hpp"


#define PSCENE          (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene))
#define PSYSTEM         (reinterpret_cast<OGREGraphicsSystem*>(m_pSystemScene->GetSystem()))
#define POGRESCENEMGR   (PSCENE->GetOGRESceneManager())
#define POGREROOT       (reinterpret_cast<OGREGraphicsSystem*>(m_pSystem)->GetOGRERoot())

#define ASSERT_EXIST(x, y) if (!x){ASSERT(false); return y;}

#define max(a, b) (((a) > (b)) ? (a) : (b))

pcstr OGREGraphicsObjectWindow::sm_kapszPropertyNames[] =
{
    "Color", "Position", "Size"
};

const Properties::Property OGREGraphicsObjectWindow::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_Color ],
                          VALUE1x4( Properties::Values::Int32 ),
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          0, 0, 255, 127),

    Properties::Property( sm_kapszPropertyNames[ Property_Position ],
                          VALUE1x2( Properties::Values::Float32 ),
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          0.0f ),

    Properties::Property( sm_kapszPropertyNames[ Property_Size ],
                          VALUE1x2( Properties::Values::Float32 ),
                          Properties::Flags::Valid,
                          nullptr, nullptr, nullptr, nullptr,
                          0.0f ),
};

OGREGraphicsObjectWindow::OGREGraphicsObjectWindow(ISystemScene* pSystemScene, pcstr pszName) : OGREGraphicsObject( pSystemScene, pszName )
{
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames / sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kaDefaultProperties / sizeof sm_kaDefaultProperties[ 0 ] );

    m_Type          = OGREGraphicsObject::Type_Window;
    m_pWindow       = nullptr;
    m_pTextAreaBody = nullptr;
    m_pTextAreaBody = nullptr;
    m_nRed          = 0;
    m_nGreen        = 0;
    m_nBlue         = 255;
    m_nAlpha        = 127;

    // 0.03f == 33 lines max on the screen.
    m_CharHeight  = 0.03f;
}

OGREGraphicsObjectWindow::~OGREGraphicsObjectWindow(void)
{
    POGRESCENEMGR->destroyManualObject(m_pManualBackground);
    POGRESCENEMGR->destroyManualObject(m_pManualForeground);
}



//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::Initialize
//
// Create the two components of a window. The colored background (Panel) and the
// textual portion (TextArea). Attaches the panel element to the overlay and the
// text area element to the panel element.
//
//-----------------------------------------------------------------------------
Error OGREGraphicsObjectWindow::Initialize(
    std::vector<Properties::Property> Properties
    )
{
    Error Err;

    ASSERT( !m_bInitialized );

    Err = Errors::Failure;
  
    OGREGraphicsObject::Initialize(Properties);

    //----------------------------------------------------------------------------
    // Obtain a pointer to the Ogre overlay manager. We use this to create the text
    // elements and their container.
    //
    Ogre::OverlayManager *pOverlayManager = (reinterpret_cast<OGREGraphicsScene*>(m_pSystemScene)->GetOGREOverlayManager());
    ASSERT_EXIST(pOverlayManager, Err);

    //----------------------------------------------------------------------------
    // Obtain a pointer to the default overlay. We add our overlay elements to this
    // overlay.
    //
    m_pOverlay = PSCENE->GetDefaultOverlay();

    //----------------------------------------------------------------------------
    // Create the text area elements. We create two of them, one for a caption and
    // one for the body.
    //
    std::string szElementName;
    szElementName = m_pszName;
    szElementName.append("TextBody");
    m_pTextAreaBody = static_cast<Ogre::TextAreaOverlayElement*>(pOverlayManager->createOverlayElement("TextArea", szElementName));
    ASSERT_EXIST(m_pTextAreaBody, Err);

    szElementName = m_pszName;
    szElementName.append("TextCaption");
    m_pTextAreaCaption = static_cast<Ogre::TextAreaOverlayElement*>(pOverlayManager->createOverlayElement("TextArea", szElementName));
    ASSERT_EXIST(m_pTextAreaCaption, Err);

    //----------------------------------------------------------------------------
    // Create the panel element. This will act as a container for our text
    // elements.
    //
    szElementName = m_pszName;
    m_pWindow = static_cast<Ogre::PanelOverlayElement*>(pOverlayManager->createOverlayElement("Panel", szElementName));
    ASSERT_EXIST(m_pWindow, Err);

    //----------------------------------------------------------------------------
    // Create the manual object elements. We need to create more than one so that
    // we can set the rendering order. These will be the colored background and the
    // lines overlaying them.
    //
    szElementName = m_pszName;
    szElementName.append("ManualBackground");
    m_pManualBackground = POGRESCENEMGR->createManualObject(szElementName);
    ASSERT_EXIST(m_pManualBackground, Err);

    szElementName = m_pszName;
    szElementName.append("ManualForeground");
    m_pManualForeground = POGRESCENEMGR->createManualObject(szElementName);
    ASSERT_EXIST(m_pManualForeground, Err);

    //----------------------------------------------------------------------------
    // Get our properties that were defined in the .cdf.
    //
    SetProperties(Properties);

    //----------------------------------------------------------------------------
    // Create the material that we will use for the background.
    //
    CreateMaterial();

    //----------------------------------------------------------------------------
    // Add our panel container element to the overlay. We also set the initial
    // settings. The coordinates use a screen of (0.0, 0.0) in the top left corner
    // and (1.0, 1.0) in the lower right corner.
    //
    m_pOverlay->add2D(m_pWindow);
    m_pWindow->setMetricsMode(Ogre::GMM_RELATIVE);
    m_pWindow->setTransparent(true);
    m_pWindow->setPosition(m_Left, m_Top);
    m_pWindow->setDimensions(m_Width, m_Height);

    
    //----------------------------------------------------------------------------
    // Add the text elements to the panel container, creating a parent/child
    // relationship.
    //
    // One thing to note with the text fields is that in relative mode they use a
    // coordinate system of (0.0, 0.0) in the upper left and (1.0, 1.0) in the
    // lower right. This is KIND OF relative to the parent object so (0.0, 0.0)
    // is in the upper left of the parent container but (1.0, 1.0) is not the lower
    // right corner of the parent object. It is as if the parent object has the
    // same dimensions as the entire screen.
    //
    m_pWindow->addChild(m_pTextAreaCaption);
    m_pTextAreaCaption->setMetricsMode(Ogre::GMM_RELATIVE);
    m_pTextAreaCaption->setPosition(.5f * m_Width, 0.0f);
    m_pTextAreaCaption->setDimensions(m_Width, m_CharHeight * 1.0f);
    m_pTextAreaCaption->setCharHeight(m_CharHeight);
    m_pTextAreaCaption->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Center);
    m_pTextAreaCaption->setFontName("MyFont");
    
    m_pWindow->addChild(m_pTextAreaBody);
    m_pTextAreaBody->setMetricsMode(Ogre::GMM_RELATIVE);
    m_pTextAreaBody->setPosition(m_Width * 0.01f, m_CharHeight);
    m_pTextAreaBody->setDimensions(m_Width, m_CharHeight * 9.0f);
    m_pTextAreaBody->setCharHeight(m_CharHeight);
    m_pTextAreaBody->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Left);
    m_pTextAreaBody->setFontName("MyFont");

    //----------------------------------------------------------------------------
    // Create an infinite bounding box to be used by the manual objects so they
    // don't get clipped.
    //
    Ogre::AxisAlignedBox aabbInf;
    aabbInf.setInfinite();

    //----------------------------------------------------------------------------
    // Create a scene node to attach the manual objects to.
    //
    m_pSceneNode = POGRESCENEMGR->getRootSceneNode()->createChildSceneNode();

    //----------------------------------------------------------------------------
    // Unfortunately, since the manual objects use a different coordinate system
    // than the overlay objects we need to translate.
    //
    m_ManualLeft   = (m_Left * 2.0f) - 1.0f;
    m_ManualTop    = (m_Top * -2.0f) + 1.0f;

    m_ManualWidth  = m_Width  * 2.0f;
    m_ManualHeight = m_Height * 2.0f;

    //----------------------------------------------------------------------------
    // Create our foreground manual object. This will contain any lines which we
    // want to display in the window. For example, a simple border or a chart.
    //
    // Manual objects use a coordinate space where (0.0, 0.0) is the center of the
    // screen. The edges of the screen are 1.0 unit away.
    //
    m_pSceneNode->attachObject(m_pManualForeground);
    m_pManualForeground->setUseIdentityProjection(true);
    m_pManualForeground->setUseIdentityView(true);
    m_pManualForeground->setBoundingBox(aabbInf);
    m_pManualForeground->setDynamic(true);
    m_pManualForeground->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);

    m_BodyLeft   = m_ManualLeft;
    m_BodyTop    = m_ManualTop - m_CharHeight * 2.0f;
    m_BodyHeight = m_ManualHeight - (m_ManualTop - m_BodyTop);
    m_BodyWidth  = m_ManualWidth;


    // Begin our object definition
    m_pManualForeground->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

    // Add vertices
    m_pManualForeground->position(m_ManualLeft,                 m_ManualTop,                  0.0f);
    m_pManualForeground->position(m_ManualLeft + m_ManualWidth, m_ManualTop,                  0.0f);
    m_pManualForeground->position(m_ManualLeft + m_ManualWidth, m_ManualTop - m_ManualHeight, 0.0f);
    m_pManualForeground->position(m_ManualLeft,                 m_ManualTop - m_ManualHeight, 0.0f);
    m_pManualForeground->position(m_ManualLeft,                 m_BodyTop,                    0.0f);
    m_pManualForeground->position(m_ManualLeft + m_ManualWidth, m_BodyTop,                    0.0f);

    // Add index list
    m_pManualForeground->index(0);
    m_pManualForeground->index(1);
    m_pManualForeground->index(1);
    m_pManualForeground->index(2);
    m_pManualForeground->index(2);
    m_pManualForeground->index(3);
    m_pManualForeground->index(3);
    m_pManualForeground->index(0);
    m_pManualForeground->index(4);
    m_pManualForeground->index(5);

    // End this section of the object
    m_pManualForeground->end();

    //----------------------------------------------------------------------------
    // Create our background manual object. This contains the colored background
    // of the window.
    //
    // Manual objects use a coordinate space where (0.0, 0.0) is the center of the
    // screen. The edges of the screen are 1.0 units away.
    //
    m_pSceneNode->attachObject(m_pManualBackground);
    m_pManualBackground->setUseIdentityProjection(true);
    m_pManualBackground->setUseIdentityView(true);
    m_pManualBackground->setBoundingBox(aabbInf);
    m_pManualBackground->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 2);

    // Begin our object definition
    szElementName = m_pszName;
    szElementName.append("Material");
    m_pManualBackground->begin(szElementName, Ogre::RenderOperation::OT_TRIANGLE_LIST);

    // Add vertices
    m_pManualBackground->position(m_ManualLeft,                 m_ManualTop,                  0.0f);
    m_pManualBackground->position(m_ManualLeft + m_ManualWidth, m_ManualTop,                  0.0f);
    m_pManualBackground->position(m_ManualLeft + m_ManualWidth, m_ManualTop - m_ManualHeight, 0.0f);
    m_pManualBackground->position(m_ManualLeft,                 m_ManualTop - m_ManualHeight, 0.0f);

    // Add index list
    m_pManualBackground->quad(0, 3, 2, 1);

    // End this section of the object
    m_pManualBackground->end();

    //-------------------------------------------------------------------------
    // Note that we are successful (of course we are!)
    //
    Err = Errors::Success;
    return Err;
}


void OGREGraphicsObjectWindow::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());

    //
    // Add all the properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for (auto & property : sm_kaDefaultProperties)
    {
        Properties.push_back( property );
    }

    //
    // Modify the default values.
    //
    if (m_pWindow)
    {
        Properties[iProperty + Property_Color].SetValue(0, m_nRed);
        Properties[iProperty + Property_Color].SetValue(1, m_nGreen);
        Properties[iProperty + Property_Color].SetValue(2, m_nBlue);
        Properties[iProperty + Property_Color].SetValue(3, m_nAlpha);

        Properties[iProperty + Property_Position].SetValue(0, m_pWindow->getLeft());
        Properties[iProperty + Property_Position].SetValue(0, m_pWindow->getTop());

        Properties[iProperty + Property_Size].SetValue(0, m_pWindow->getWidth());
        Properties[iProperty + Property_Size].SetValue(0, m_pWindow->getHeight());
    }
}


void OGREGraphicsObjectWindow::SetProperties(Properties::Array Properties)
{
    //
    // Read in the properties.
    //
    for (auto & property : Properties)
    {
        if ( property.GetFlags() & Properties::Flags::Valid )
        {
            std::string sName = property.GetName();
 
            if ( sName == sm_kapszPropertyNames[Property_Color] )
            {
                m_nRed   = property.GetInt32(0);
                m_nGreen = property.GetInt32(1);
                m_nBlue  = property.GetInt32(2);
                m_nAlpha = property.GetInt32(3);
            }
            else if ( sName == sm_kapszPropertyNames[Property_Position] )
            {
                m_Left = property.GetFloat32(0);
                m_Top  = property.GetFloat32(1);
            }
            else if ( sName == sm_kapszPropertyNames[Property_Size] )
            {
                m_Width = property.GetFloat32(0);
                m_Height = property.GetFloat32(1);
            }
            else
            {
                ASSERT(False);
            }

            //
            // Set this property to invalid since it's already been read.
            //
            property.ClearFlag(Properties::Flags::Valid);
        }
    }
}



//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::CreateMaterial
//
// Creates a solid color texture based on the color member variables of the
// object. Then create a material that uses the texture and add it to the
// material manager for use.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectWindow::CreateMaterial()
{
    //----------------------------------------------------------------------------
    // Create unique texture and material names using the object's name
    //
    std::string szTextureName, szMaterialName;

    szTextureName = m_pszName;
    szTextureName.append("Texture");

    szMaterialName = m_pszName;
    szMaterialName.append("Material");

    //-------------------------------------------------------------------------
    // Dynamically create a texture and fill it with a solid color.
    //
    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual(
        szTextureName,                      // unique name
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D,
        1, 1,                               // width & height
        0,                                  // number of mipmaps
        Ogre::PF_BYTE_BGRA,    // pixel format
        Ogre::TU_DEFAULT);    // usage; should be TU_DYNAMIC_WRITE_ONLY_DISCARDABLE for
                                            // textures updated very often (e.g. each frame)
    Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();

    // Lock the pixel buffer and get a pixel box
    pixelBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL); // for best performance use HBL_DISCARD!
    const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();

    //----------------------------------------------------------------------------
    // Here is where we actually draw the "art" onto the texture. In this case it
    // is just a solid color.
    //
    typedef unsigned char uint8;
    uint8* pDest = static_cast<uint8*>(pixelBox.data);
    for (size_t j = 0; j < 1; j++) {
        for(size_t i = 0; i < 1; i++) {
            *pDest++ = (u8)m_nBlue;  // B
            *pDest++ = (u8)m_nGreen; // G
            *pDest++ = (u8)m_nRed;   // R
            *pDest++ = (u8)m_nAlpha; // A
        }
    }

    // Unlock the pixel buffer
    pixelBuffer->unlock();

    //----------------------------------------------------------------------------
    // Now we create our material which will use the texture created above.
    //
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(
        szMaterialName,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    material->getTechnique(0)->getPass(0)->createTextureUnitState(szTextureName);
    material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    material->setDepthCheckEnabled(false);
    m_pWindow->setMaterialName(szMaterialName);

    return;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::SetVisibility
//
// Sets the window's visibility. This also sets the text area's visibility since
// it is attached to the window.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectWindow::SetVisibility(bool bVisible)
{
    if (!m_pWindow)
        return;

    if (bVisible) {
        m_pWindow->show();
    } else {
        m_pWindow->hide();
    }
    m_pManualBackground->setVisible(bVisible);
    m_pManualForeground->setVisible(bVisible);

    return;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::SetCaption
//
// Changes the caption of the window.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectWindow::SetCaption(std::string szText)
{
    if (m_pTextAreaCaption) {
        m_pTextAreaCaption->setCaption(szText);
    }
    return;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::SetText
//
// Changes the text that is displayed in the window.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectWindow::SetText(std::string szText)
{
    if (m_pTextAreaBody) {
        m_pTextAreaBody->setCaption(szText);
    }
    return;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::ApplyChanges
//
// Makes changes to the window.
//
//-----------------------------------------------------------------------------
void OGREGraphicsObjectWindow::ApplyChanges(const WindowData *pData)
{
    if (!pData)
        return;

    if (pData->nFlags & WINDOW_SHOW)
    {
        SetVisibility(pData->bShow);
    }
    if (pData->nFlags & WINDOW_CAPTION)
    {
        SetCaption(pData->sCaption);
    }
    if (pData->nFlags & WINDOW_BODY)
    {
        SetText(pData->sBody);
    }

    return;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::GetDesiredSystemChanges
//
// Lets the CCM know what changes we would like to know about.
//
//-----------------------------------------------------------------------------
System::Types::BitMask OGREGraphicsObjectWindow::GetDesiredSystemChanges(void)
{
    return System::Changes::Graphics::GUI;
}

//-----------------------------------------------------------------------------
//
// OGREGraphicsObjectWindow::ChangeOccurred
//
// When a change is made that we are interested in the CCM calls this method.
// Here we need to get the needed information from the system that posted the
// change.
//
//-----------------------------------------------------------------------------
Error OGREGraphicsObjectWindow::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
    UNREFERENCED_PARAM(pSubject);
    static int i = 0;

    Error Err = Errors::Failure;

    if (ChangeType & (System::Changes::Graphics::GUI))
    {
        const WindowData *pWindowData;

        IGUIObject* pGUIObj = dynamic_cast<IGUIObject*>(pSubject);
        pWindowData = pGUIObj->GetWindowData();
        ApplyChanges(pWindowData);
    }

    Err = Errors::Success;
    return Err;
}
