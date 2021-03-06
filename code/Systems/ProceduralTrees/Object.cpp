#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/Common/Vertex.hpp"
#include "Systems/ProceduralTrees/Trees/Tree.hpp"
#include "Systems/ProceduralTrees/Trees/Observer.hpp"
#include "Systems/ProceduralTrees/Object.hpp"


#define NUM_VERTEXDECL_ELEMENTS         3




pcstr TreeObject::sm_kapszPropertyNames[] =
{
    "Position", "Seed", "LevelCount","PrimitiveType" 
};

const Properties::Property TreeObject::sm_kaDefaultProperties[] =
{
    Properties::Property( sm_kapszPropertyNames[ Property_Position ],
                          Properties::Values::Vector3,
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          Base::Vector3::Zero ),
    Properties::Property( sm_kapszPropertyNames[ Property_Seed ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_LevelCount ],
                          VALUE1( Properties::Values::Int32 ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          0 ),
    Properties::Property( sm_kapszPropertyNames[ Property_PrimitiveType ],
                          VALUE1( Properties::Values::String ),
                          Properties::Flags::Valid | Properties::Flags::InitOnly,
                          NULL, NULL, NULL, NULL,
                          "" ),
};



TreeObject::TreeObject(
    ISystemScene* pSystemScene,
    pcstr pszType
    )
    : ISystemObject( pSystemScene, NULL )
    ,m_Position(Base::Vector3::Zero)
    ,m_Seed(0) 
    ,m_LevelCount(0)
    ,m_Initialized(False)
    ,m_pPostedData(0)
{    
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames /
                                sizeof sm_kapszPropertyNames[ 0 ] );
    ASSERT( Property_Count == sizeof sm_kapszPropertyNames /
                                sizeof sm_kapszPropertyNames[ 0 ] );

    m_GrammarType = pszType;

}


TreeObject::~TreeObject(
    void
    )
{
    SAFE_DELETE( m_pPostedData );

    reinterpret_cast<TreeScene*>(GetSystemScene())->GetForest().remove(this);

    std::vector<BranchBase *>::iterator i, iend;
    iend = m_TreeNodeList.end();
    for (i = m_TreeNodeList.begin(); i != iend; ++i)
    {
        delete *i;
    }
    m_TreeNodeList.empty();
 }


System::Type
TreeObject::GetSystemType(
    void
    )
{
    return System::Types::MakeCustom( 0 );
}


Error 
TreeObject::Initialize( std::vector<Properties::Property> Properties )
{    
    for ( Properties::Iterator it=Properties.begin(); it != Properties.end(); it++ )
    {
            std::string sName = it->GetName();

            if ( sName == sm_kapszPropertyNames[ Property_LevelCount ] )
            {
                    m_LevelCount = it->GetInt32( 0 );
            } else if (sName == sm_kapszPropertyNames[ Property_Position ] )
            {
                m_Position.x = it->GetFloat32( 0 );
                m_Position.y = it->GetFloat32( 1 );
                m_Position.z = it->GetFloat32( 2 );
            } else if (sName == sm_kapszPropertyNames[ Property_Seed ] )
            {
                m_Seed = it->GetInt32( 0 );
            } else if (sName == sm_kapszPropertyNames[ Property_PrimitiveType ] )
            {
                pcstr pt = it->GetStringPtr(0);
                if(!strcmp(pt,"Branches"))
                {
                    m_PrimitiveType = Primitive_Branches;
                }else if(!strcmp(pt,"Canopy")){
                    m_PrimitiveType = Primitive_Canopy;
                }                
            }
    }

    if (observer::Instance().DXRS) {delete observer::Instance().DXRS;}
    observer::Instance().DXRS = new RenderStructure();
    observer::Instance().DXRS->CurrentIndex    = 0;
    observer::Instance().DXRS->CurrentVIndex   = 0;
    observer::Instance().DXRS->ptrIBData;
    observer::Instance().DXRS->BranchCount = 0;
    observer::Instance().DXRS->ReverseWindingOrder = true;



    m_Tree = new Tree(m_GrammarType.c_str(),Base::Vector3(m_Position.x,m_Position.y,m_Position.z));

    m_Tree->growTree();
    m_Tree->BoundingBox(m_Tree->m_nodeTree);
    m_ObjectBoundingBox = observer::Instance().DXRS->aabb;
    if(m_PrimitiveType == Primitive_Branches)
    {
        m_Tree->BranchToList(m_Tree->m_nodeTree, &m_TreeNodeList);
    }else if(m_PrimitiveType == Primitive_Canopy)
    {
        m_Tree->CanopyToList(m_Tree->m_nodeTree, &m_TreeNodeCanopyList);
    }
    FillPostedData();
    
    reinterpret_cast<TreeScene*>(GetSystemScene())->GetForest().push_back(this);
    observer::Instance().DXRS->CurrentIndex    = 0;
    observer::Instance().DXRS->CurrentVIndex   = 0;
    observer::Instance().DXRS->ptrIBData;
    observer::Instance().DXRS->BranchCount = 0;
    observer::Instance().DXRS->ReverseWindingOrder = true;

    PostChanges( System::Changes::Graphics::AllMesh | System::Changes::Custom );

    return Errors::Success;
}
void TreeObject::FillPostedData(){
    if(m_PrimitiveType == Primitive_Branches)
    {
        std::vector<BranchBase *>::iterator i, iend;
        iend = m_TreeNodeList.end();
        if(m_pPostedData){
        }else{
            m_pPostedData = new PostedData();
        }
        for (i = m_TreeNodeList.begin(); i != iend; ++i)
        {
           //branchBase *pbb = *i;
           PointPair * pp = new PointPair();
           pp->basePoint = (*i)->position;
           pp->extendPoint = (*i)->segments[(*i)->segmentCount-1].m_tipPointList[0];
           pp->aabb = (*i)->aabb;
           pp->burning = (*i)->burning;
           m_pPostedData->pointPairs.push_back(pp);           
        }
    }else if(m_PrimitiveType == Primitive_Canopy)
    {
        std::vector<Canopy *>::iterator i, iend;
        iend = m_TreeNodeCanopyList.end();
        if(m_pPostedData){
        }else{
            m_pPostedData = new PostedData();
        }
        for (i = m_TreeNodeCanopyList.begin(); i != iend; ++i)
        {
           Canopy *cnpy = *i;
           PointPair * pp = new PointPair();
           pp->basePoint = *(cnpy->m_Canopy[((cnpy->m_width)/2) + ((cnpy->m_height)/2)*cnpy->m_width ]);
           pp->basis = cnpy->Basis;
           
           pp->aabb = cnpy->m_AABB;
           pp->burning = cnpy->m_burning;
           m_pPostedData->pointPairs.push_back(pp);           

        }
    }


}
void TreeObject::update(f32 DeltaTime)
{
    UNREFERENCED_PARAM( DeltaTime );
    //PostChanges( System::Changes::Custom);

}


void* TreeObject::GetObjectPostedVariables(void *Params)
{
    UNREFERENCED_PARAM( Params );
    //PostChanges( System::Changes::Custom |
    //    System::Changes::Graphics::AllPointList);

    return m_pPostedData;

    
}

void
TreeObject::GetProperties(
    Properties::Array& Properties
    )
{
    //
    // Get the index of our first item.
    //
    i32 iProperty = static_cast<i32>(Properties.size());

    //
    // Add the common properties.
    //
    Properties.reserve( Properties.size() + Property_Count );

    for ( i32 i=0; i < Property_Count; i++ )
    {
        Properties.push_back( sm_kaDefaultProperties[ i ] );
    }

    //
    // Modify the default values.
    //
    Properties[ iProperty+Property_Position ].SetValue( m_Position );
    Properties[ iProperty+Property_Seed ].SetValue( 0, m_Seed );
    Properties[ iProperty+Property_LevelCount ].SetValue( 0, m_LevelCount );

}


void
TreeObject::SetProperties(
    Properties::Array Properties
    )
{
    //
    // Read in the properties.
    //
    UNREFERENCED_PARAM(Properties);
}


System::Types::BitMask
TreeObject::GetDesiredSystemChanges(
    void
    )
{
    return System::Changes::Graphics::AllMesh | System::Changes::Custom;
}


Error
TreeObject::ChangeOccurred(
    ISubject* pSubject,
    System::Changes::BitMask ChangeType
    )
{
   /*if ( ChangeType & (System::Changes::Custom) )
    {
                //
        ISystemObject* pSysObj = dynamic_cast<ISystemObject*>(pSubject);
        pcstr name = pSysObj->GetSystemScene()->GetSystem()->GetName();
        //IGraphicsObject* pGfxObj = dynamic_cast<IGraphicsObject*>(pSubject);
        
        if (!strcmp(name,"ProceduralFire")) {
            FireObject* pFireObj = dynamic_cast<FireObject*>(pSubject);
            UNREFERENCED_PARAM(pFireObj);
            //pFireObj->somefunction();
        }
        if (!strcmp(name,"ProceduralTrees")) {
            TreeObject* pTreeObj = dynamic_cast<TreeObject*>(pSubject);
            //pTreeObj->somefuction();
            UNREFERENCED_PARAM(pTreeObj);
        }

    }*/

    return Errors::Success;
}


System::Changes::BitMask
TreeObject::GetPotentialSystemChanges(
    void
    )
{
    return System::Changes::Graphics::AllMesh | System::Changes::Custom;
}


u32 
TreeObject::GetSubMeshCount(
    void
    )
{
    // We use only one SubMesh
    return 1;
}


u32
TreeObject::GetIndexDeclaration(
    In  u16 nSubMeshIndex
    )
{
    // Ignored as we use only one SubMesh
    UNREFERENCED_PARAM( nSubMeshIndex );

    return IndexDecl::Type::Index16;
}


u32
TreeObject::GetVertexDeclarationCount(
   In  u16 nSubMeshIndex
    )
{
    // Ignored as we use only one SubMesh
    UNREFERENCED_PARAM( nSubMeshIndex );

    return NUM_VERTEXDECL_ELEMENTS;
}


void
TreeObject::GetVertexDeclaration(
    Out VertexDecl::Element* pVertexDecl,
    In  u16 nSubMeshIndex
    )
{
    // Ignored as we use only one SubMesh
    UNREFERENCED_PARAM( nSubMeshIndex );

    static const u32 Size = sizeof (VertexDecl::Element) * NUM_VERTEXDECL_ELEMENTS;
    static const VertexDecl::Element Decl[ NUM_VERTEXDECL_ELEMENTS ] =
    {
        { VertexDecl::Type ::Float3, VertexDecl::Usage::Position, 0, 0 },
        { VertexDecl::Type ::Float3, VertexDecl::Usage::Normal, 0, 0 },
        { VertexDecl::Type ::Float2, VertexDecl::Usage::Texture, 0, 0 },
    };

    ::memcpy_s( pVertexDecl, Size, Decl, Size );
}


u32
TreeObject::GetIndexCount(
    In  u16 nSubMeshIndex
    )
{
    // Ignored as we use only one SubMesh
    UNREFERENCED_PARAM( nSubMeshIndex );

    if(m_PrimitiveType == Primitive_Branches)
    {
        return m_Tree->m_EndBranchIndexBuffer - m_Tree->m_StartBranchIndexBuffer;
    }else if(m_PrimitiveType == Primitive_Canopy)
    {
        return m_Tree->m_EndCanopyIndexBuffer - m_Tree->m_StartCanopyIndexBuffer;
    }

    // NOTE: what is appropriate default value?
    ASSERT( False );
    return 0;
}


u32
TreeObject::GetVertexCount(
    In  u16 nSubMeshIndex
    )
{
    // Ignored as we use only one SubMesh
    UNREFERENCED_PARAM( nSubMeshIndex );

    if(m_PrimitiveType == Primitive_Branches)
    {
        return m_Tree->m_EndBranchVertexBuffer - m_Tree->m_StartBranchVertexBuffer;
    }else if(m_PrimitiveType == Primitive_Canopy)
    {
        return m_Tree->m_EndCanopyVertexBuffer - m_Tree->m_StartCanopyVertexBuffer;
    }

    // NOTE: what is appropriate default value?
    ASSERT( False );
    return 0;
}


void
TreeObject::GetIndices(
    Out void* pIndices,
    In  u16 nSubMeshIndex
    )
{
    // Ignored as we use only one SubMesh
    UNREFERENCED_PARAM( nSubMeshIndex );

    //added code
    observer::Instance().DXRS->CurrentIndex    = 0;
    observer::Instance().DXRS->CurrentVIndex   = 0;
    observer::Instance().DXRS->ptrIBData;
    observer::Instance().DXRS->BranchCount = 0;
    observer::Instance().DXRS->ReverseWindingOrder = true;
    //end added Code
    observer::Instance().DXRS->ptrIBData = reinterpret_cast<std::uint16_t*>(pIndices);
    observer::Instance().DXRS->vpnt = NULL;
    if(m_PrimitiveType == Primitive_Branches)
    {
        m_Tree->fillBranches( m_Tree->m_nodeTree );
    }else if(m_PrimitiveType == Primitive_Canopy)
    {
        m_Tree->fillCanopies( m_Tree->m_nodeTree );
    }

}


void
TreeObject::GetVertices(
    Out void* pVertices,
    In  u16 nSubMeshIndex,
    In  u16 nStreamIndex,
    In  u32 nVertexDeclCount,
    In  VertexDecl::Element* pVertexDecl
    )
{
    // Ignored as we use only one SubMesh
    UNREFERENCED_PARAM( nSubMeshIndex );
    // Ignored as we use only one stream
    UNREFERENCED_PARAM( nStreamIndex );

    // nVertexDeclCount should be 0 to indicate the full vertex list
    ASSERT( 0 == nVertexDeclCount );
    UNREFERENCED_PARAM( pVertexDecl );
    UNREFERENCED_PARAM( nVertexDeclCount );

    //added code
    observer::Instance().DXRS->CurrentIndex    = 0;
    observer::Instance().DXRS->CurrentVIndex   = 0;
    observer::Instance().DXRS->ptrIBData;
    observer::Instance().DXRS->BranchCount = 0;
    observer::Instance().DXRS->ReverseWindingOrder = true;
    //end added Code

    observer::Instance().DXRS->ptrIBData = NULL;
    observer::Instance().DXRS->vpnt = reinterpret_cast<VertexPNT*>(pVertices);
    if(m_PrimitiveType == Primitive_Branches)
    {
        m_Tree->fillBranches( m_Tree->m_nodeTree );
    }else if(m_PrimitiveType == Primitive_Canopy)
    {
        m_Tree->fillCanopies( m_Tree->m_nodeTree );
    }

}


void
TreeObject::GetAABB(
    Out Base::Vector3& Min,
    Out Base::Vector3& Max
    )
{
    //observer *Overseer = observer::Instance();
    //ASSERT( Overseer != NULL );

    //Min.x = observer::Instance().DXRS->AABB.xMin;
    //Min.y = observer::Instance().DXRS->AABB.yMin;
    //Min.z = observer::Instance().DXRS->AABB.zMin;

    //Max.x = observer::Instance().DXRS->AABB.xMax;
    //Max.y = observer::Instance().DXRS->AABB.yMax;
    //Max.z = observer::Instance().DXRS->AABB.zMax;
    Min.x = m_ObjectBoundingBox.xMin;
    Min.y = m_ObjectBoundingBox.yMin;
    Min.z = m_ObjectBoundingBox.zMin;

    Max.x = m_ObjectBoundingBox.xMax;
    Max.y = m_ObjectBoundingBox.yMax;
    Max.z = m_ObjectBoundingBox.zMax;
}
