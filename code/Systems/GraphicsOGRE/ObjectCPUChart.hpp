#pragma once

class OGREGraphicsScene;

/// <summary>
///     CPU chart.  Show chart of CPU loads.
/// </summary>
class OGREGraphicsObjectCPUChart : public OGREGraphicsObjectChart
{

    friend OGREGraphicsScene;

public:
    /// <summary>
    ///     Add a new data point to this CPU chart.  Recent data points are cached.
    ///     During the next screen update, the contents of the window will now show
    ///     this new data point plus all cached data points.
    /// </summary>
    /// <param name="fDataPoint">f32 - Data point to add to the chart.</param>
    virtual void AddDataPoint(f32 fDataPoint);

protected:

    /// <summary cref="OGREGraphicsObjectCPUChart::OGREGraphicsObjectCPUChart">
    ///     Basic constructor; the only real one.
    /// </summary>
    /// <param name="pSystemScene">ISystemScene* - Pointer to the scene containing this window.</param>
    /// <param name="pszName">pcstr - Name of this window.</param>
   OGREGraphicsObjectCPUChart( ISystemScene* pSystemScene, pcstr pszName );

   /// <summary cref="OGREGraphicsObjectCPUChart::~OGREGraphicsObjectCPUChart">
   ///      Basic destructor; the only one.
   /// </summary>
    ~OGREGraphicsObjectCPUChart( void );

    /// <summary cref="OGREGraphicsObjectCPUChart::Update">
    ///     Called by the framework every frame, so this window can update itself.
    /// </summary>
    /// <param name="DeltaTime">f32 - Elapsed wall-clock time since the last call to this function.</param>
    virtual void Update( f32 DeltaTime );

private:
    static const f32                    m_secondsPerUpdate;
    f32                                 m_secondsSinceLastUpdate;
    i32                                 m_framesSinceLastUpdate;
    WindowData                          m_windowData;

    /// <summary cref="OGREGraphicsObjectCPUChart::GetWindowDataUpdate">
    ///     Implementation function, used by the Update function.
    /// </summary>
    /// <returns>WindowData* - Result structure containing all data to update in the UI.</returns>
    virtual WindowData*                 GetWindowDataUpdate();
};
