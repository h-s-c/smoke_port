#pragma once

//external
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>


class OGREGraphicsScene;

///////////////////////////////////////////////////////////////////////////////
/// <summary>
///   This is the base class for a GUI type window. It consists of a colored
///   background with a caption and body text.
/// </summary>
///////////////////////////////////////////////////////////////////////////////
class OGREGraphicsObjectWindow : public OGREGraphicsObject
{
    friend OGREGraphicsScene;

protected:

    OGREGraphicsObjectWindow( ISystemScene* pSystemScene, pcstr pszName );
    ~OGREGraphicsObjectWindow( void );

    /// <summary cref="OGREGraphicsObjectWindow::Initialize">
    ///   Implementation of the <c>ISystem::Initialize</c> function.
    /// </summary>
    /// <param name="Properties">Initializes the object with the properties specified by <paramref name="Properties"/>.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="OGREGraphicsObject::Initialize"/>
    virtual Error Initialize( std::vector<Properties::Property> Properties );

    /// <summary cref="OGREGraphicsObjectWindow::GetProperties">
    ///   Implementation of the <c>ISystem::GetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Gets the properties of the object</param>
    /// <seealso cref="ISystem::GetProperties"/>
    virtual void GetProperties( Properties::Array& Properties );

    /// <summary cref="OGREGraphicsObjectWindow::SetProperties">
    ///   Implementation of the <c>ISystem::SetProperties</c> function.
    /// </summary>
    /// <param name="Properties">Sets the properties of the object</param>
    /// <seealso cref="ISystem::SetProperties"/>
    virtual void SetProperties( Properties::Array Properties );

    /// <summary cref="OGREGraphicsObjectWindow::GetDesiredSystemChanges">
    ///   Implementation of the <c>IGeometryObject::GetDesiredSystemChanges</c> function.
    /// </summary>
    /// <returns>System::Types::BitMask - System changes desired by the object.</returns>
    /// <seealso cref="ISystem::GetSystemType"/>
    virtual System::Types::BitMask GetDesiredSystemChanges(void);

    /// <summary cref="OGREGraphicsObjectWindow::ChangeOccurred">
    ///   Implementation of the <c>IObserver::ChangeOccurred</c> function.
    /// </summary>
    /// <param name="pSubject">Subject of this notification.</param>
    /// <param name="ChangeType">Type of notification for this object.</param>
    /// <returns>Error.</returns>
    /// <seealso cref="ISystem::ChangeOccurred"/>
    virtual Error ChangeOccurred(ISubject* pSubject, System::Changes::BitMask ChangeType);

    /// <summary cref="OGREGraphicsObjectWindow::SetVisibility">
    ///   Enable/disable visibility
    /// </summary>
    /// <param name="bVisible">bool - window is visable?</param>
    void SetVisibility(bool bVisible);

    /// <summary cref="OGREGraphicsObjectWindow::SetCaption">
    ///   Changes the caption of the window.
    /// </summary>
    /// <param name="szText">std::string - desired caption</param>
    void SetCaption(std::string szText);

    /// <summary cref="OGREGraphicsObjectWindow::SetText">
    ///   Changes the text that is displayed in the window.
    /// </summary>
    /// <param name="szText">std::string - desired text</param>
    void SetText(std::string szText);

    /// <summary cref="OGREGraphicsObjectChart::ApplyChanges">
    ///   Update this window with the given data.
    /// </summary>
    /// <param name="WindowData">const WindowData * - New window data</param>
    /// <seealso cref="OGREGraphicsObjectWindow::ApplyChanges"/>
    virtual void ApplyChanges(const WindowData *);

protected:

    //
    // Window Graphics Object
    //
    enum PropertyTypes
    {
        Property_Color,
        Property_Position,
        Property_Size,
        Property_Count
    };

    static pcstr                        sm_kapszPropertyNames[];
    static const Properties::Property   sm_kaDefaultProperties[];

    f32                                 m_Left, m_Top;
    f32                                 m_Width, m_Height;
    f32                                 m_ManualLeft, m_ManualTop;
    f32                                 m_ManualWidth, m_ManualHeight;
    f32                                 m_BodyLeft, m_BodyTop;
    f32                                 m_BodyWidth, m_BodyHeight;
    f32                                 m_CharHeight;

    u32                                 m_nRed, m_nGreen, m_nBlue, m_nAlpha;

    Ogre::Overlay                      *m_pOverlay;
    Ogre::PanelOverlayElement          *m_pWindow;
    Ogre::TextAreaOverlayElement       *m_pTextAreaCaption;
    Ogre::TextAreaOverlayElement       *m_pTextAreaBody;

    Ogre::SceneNode                    *m_pSceneNode;
    Ogre::ManualObject                 *m_pManualBackground;
    Ogre::ManualObject                 *m_pManualForeground;

private:
    void                                CreateMaterial(void);

};
