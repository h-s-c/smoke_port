<?xml version="1.0" encoding="UTF-8"?>
<Scene Name="Test">

  <Properties SystemType="Geometry">
  </Properties>

  <Properties SystemType="Graphics">
    <Property Name="AmbientLight" R="0.3" G="0.3" B="0.3"></Property>
    <!-- 
         Shadow Value1's:
         SHADOWTYPE_NONE             = 0
         SHADOWTYPE_TEXTURE_ADDITIVE = 33 
     -->
    <Property Name="Shadows" Value1="0" Value2="0"></Property>
    <Property Name="ShadowColor" R="0.67451" G="0.564706" B="0.403922"></Property>
    <Property Name="UseStaticGeom" Value1="0"/>
    <Property Name="UseInstancedGeom" Value1="1"/>
    <Property Name="FogColor" R="0.0" G="0.5" B="0.0"></Property>
    <!--
       This is where we initialize the paged geometry system.  There are three sections that need to be in the .cdf in order (The .cdf
       is loaded serially).  The first part is here, in the Graphics System.  It loads general information such as the height map, draw
       distance and size of terrain.  These parameters will apply to all layers added to the system.  The second portion is in the camera
       object, the third are the actual layer objects. The offset parameters adjust terrain calculations if the center of the terrain mesh
       is not located at 0,0,0 in world coordinates.
    -->
    <!--
    <Property Name="PagedGeometry"  Heightmap="hp_terrain_heightmap_16bit_4096.png" ResourceGroup="farm_scene" Pagesize="500.0" Drawdistance="10000.0"></Property>
    <Property Name="PagedGeometryTerrain" Width="81072.1" Length="81686.7" Height="10254.9"></Property>
    <Property Name="PagedGeometryTerrainOffset" Width="0.0" Length="0.0" Height="-4879.6"></Property>
    -->
    <!--
      Mode - FOG_NONE   = 0
             FOG_EXP    = 1
             FOG_EXP2   = 2
             FOG_LINEAR = 3
             
      "Density" only applies to FOG_EXP and FOG_EXP2
      "Start" and "Stop" only apply to FOG_LINEAR
    -->
    <Property Name="Fog" Mode="0" Density="0.0001" Start="20000.0" Stop="35000.0"></Property>
    <Property Name="Font" FontPath="..//media//fonts" FontName="bluebold.ttf"/>
  </Properties>
  <Properties SystemType="Audio">
    <!-- Priority starts at 0 with highest -->
    <!-- Volume ranges from 0.0 (off) to 1.0 -->
  </Properties>
  <Properties SystemType="Input">
  </Properties>

  <Objects>
    <Object Name="MainCamera">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="-3500.0" Value2="3000.0" Value3="-1500.0"></Property>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Camera">
        <!-- The next line locks the camera to use the LookAt position instead of listening to the Input system -->
        <Property Name="LockCamera" Value1="1"/>
        <!-- Make the camera look at the corral with chickens -->
        <Property Name="LookAt" Value1="100" Value2="-400" Value3="6000"/>
        <Property Name="FOVy" Value1="0.523598"></Property>
        <Property Name="ClipDistances" Value1="5.0" Value2="120000.0"></Property>
      </Properties>

      <Properties SystemType="Input">
        <Property Name="Orientation" Value1="-0.89" Value2="0.2" Value3="0.0"/>
      </Properties>

      <Properties SystemType="Audio">
        <Property Name="Listener" Value1="1"/>
        <Property Name="Source" Value1="1"/>
        <Property Name="Position" Value1="0.0" Value2="1000.0" Value3="4000.0"/>
        <Property Name="SoundFile" Value1="M_loop_05_long.wav" Value2="Looping"/>
        <Property Name="Priority" Value1="0"/>
        <Property Name="Volume" Value1="0.03"/>
      </Properties>
    </Object>

    <!-- Performance Windows -->
    <Object Name="StatWindow">
      <Properties SystemType="Graphics" ObjectType="StatWindow">
        <Property Name="Color" Value1="0" Value2="0" Value3="0" Value4="50"/>
        <Property Name="Position" Value1="0.01" Value2="0.23"/>
        <Property Name="Size" Value1="0.12" Value2="0.33"/>
      </Properties>
      <Properties SystemType="Input" ObjectType="Overlay">
        <Property Name="FKey" Value1="1" />
      </Properties>
    </Object>

    <Object Name="CPUChart">
      <Properties SystemType="Graphics" ObjectType="CPUChart">
        <Property Name="Color" Value1="0" Value2="0" Value3="0" Value4="50"/>
        <Property Name="Position" Value1="0.01" Value2="0.01"/>
        <Property Name="Size" Value1="0.3" Value2="0.2"/>
      </Properties>
      <Properties SystemType="Input" ObjectType="Overlay">
        <Property Name="FKey" Value1="2" />
      </Properties>
    </Object>

    <Object Name="WorkloadWindow">
      <Properties SystemType="Graphics" ObjectType="WorkloadWindow">
        <Property Name="Color" Value1="0" Value2="0" Value3="0" Value4="50"/>
        <Property Name="Position" Value1="0.76" Value2="0.01"/>
        <Property Name="Size" Value1="0.22" Value2="0.39"/>
      </Properties>
      <Properties SystemType="Input" ObjectType="Overlay">
        <Property Name="FKey" Value1="3" />
      </Properties>
    </Object>

    <!-- Lights -->
    <Object Name="Sun">
      <Properties SystemType="Graphics" ObjectType="Light">
        <Property Name="Type" Value1="Point"></Property>
        <Property Name="Position" Value1="-10359.527" Value2="25641.795" Value3="60803.297"></Property>
        <Property Name="Diffuse" R="0.862745" G="0.768628" B="0.607843"></Property>
        <Property Name="Specular" R="0.862745" G="0.768628" B="0.607843"></Property>
      </Properties>
    </Object>

    <!-- Sky -->
    <Object Name="sky">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="2000" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="sky.mesh"/>
        <Property Name="CastShadows" Value1="0"/>
      </Properties>
    </Object>

    <!-- Terrain -->
    <Object Name="terrain_near">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="terrain_near.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="terrain_far">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="terrain_far.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>

    <!-- Powerlines -->
    <Object Name="power_line">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="37118.336" Value2="-765.11011" Value3="19724.113"/>
        <Property Name="Orientation" Value1="-0.21263117" Value2="0.67437965" Value3="-0.67437971" Value4="-0.21263117"/>
        <Property Name="Scale" Value1="0.65000004" Value2="0.65000004" Value3="0.64999998"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="power_line.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>

    <!-- Backdrop trees -->
    <Object Name="backdrop_trees">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="backdrop_trees.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>

    <Object Name="trees_distant">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="trees_distant.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>

    <!-- Corral -->
    <Object Name="corral07">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral07.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral01">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral01.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral02">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral02.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral03">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral03.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral04">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral04.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral05">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral05.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral06">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral06.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral08">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral08.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>

    <Object Name="corral_collision_00">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="200" Value2="-400" Value3="4100"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="0" Value2="0" Value3="0"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral_collision_00.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral_collision_03">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="-2100" Value2="-400" Value3="6650"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="0" Value2="0" Value3="0"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral_collision_03.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral_collision_02">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="200" Value2="-400" Value3="9200"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="0" Value2="0" Value3="0"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral_collision_02.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>
    <Object Name="corral_collision_01">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="2500" Value2="-400" Value3="6650"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="0" Value2="0" Value3="0"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="corral_collision_01.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>

    <Object Name="sky01">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="0" Value2="0" Value3="-0"/>
        <Property Name="Orientation" Value1="0" Value2="0" Value3="-0" Value4="-1"/>
        <Property Name="Scale" Value1="1" Value2="1" Value3="1"/>
      </Properties>
      <Properties SystemType="Graphics" ObjectType="Mesh">
        <Property Name="Mesh" Value1="sky01.mesh"/>
        <Property Name="CastShadows" Value1="1"/>
      </Properties>
    </Object>

    <Object Name="Sinbad" ODF="Sinbad">
      <Properties SystemType="Geometry">
        <Property Name="Position" Value1="-800.0" Value2="200.0" Value3="4600.0"/>
        <Property Name="Orientation" Value1="0.0" Value2="0.0" Value3="0.0" Value4="1.0"/>
        <Property Name="Scale" Value1="50.0" Value2="50.0" Value3="50.0"/>
      </Properties>
    </Object>

  </Objects>

  <Links>
   </Links>

</Scene>
