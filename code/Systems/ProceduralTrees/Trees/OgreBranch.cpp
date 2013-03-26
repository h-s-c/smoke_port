#include "Base/Compat.hpp"
#include "Base/Platform.hpp"
#include "Base/Math.hpp"
#include "Interfaces/Interface.hpp"
#include "Systems/ProceduralTrees/Trees/OgreBranch.hpp"


OgreBranch::OgreBranch(int level, Base::Vector3 basePosition):Branch(level, basePosition){
    //
    //nodeLevel = level;
    //position = basePosition;
    //segments = 0;
    //SpeciesLevelGrammar = 0;
}


Branch *OgreBranch::CreateNextBranch(std::int32_t level, Base::Vector3 basePosition){
    return new OgreBranch(level,basePosition);
}

void OgreBranch::fillBuffers(){

    std::uint16_t idx = observer::Instance().DXRS->CurrentVIndex;
    Base::Vector3 normal;
    Base::Vector3 TexCoord;
    float texUCoord;
    float texVCoord;
    //VertexType Vtype = VertexType::VP;
    VertexType Vtype = VertexType::VPNT;

    //initial segment cap  7 vertices 
    for(int j=0;j<7;j++){
       if(observer::Instance().DXRS->ReverseWindingOrder){
           normal = normal.Cross((m_pSegments[0].m_tipPointList[2]-m_pSegments[0].m_tipPointList[0]),(m_pSegments[0].m_tipPointList[1]-m_pSegments[0].m_tipPointList[0])); 
       }else{
           normal = normal.Cross((m_pSegments[0].m_tipPointList[1]-m_pSegments[0].m_tipPointList[0]),(m_pSegments[0].m_tipPointList[2]-m_pSegments[0].m_tipPointList[0])); 
        }
        texUCoord = 0.0f;
        //texVCoord = (float)(j-1)* 1.0f/6.0f;
        if(j==0){
            texVCoord = (float)0.0f;
        }else if(j<4){
            texVCoord = (float)(j-1)* 1.0f/3.0f;
        }else {
            texVCoord = 1.0f - (float)((j-1)%3)* 1.0f/3.0f;
        }
        TexCoord.x = texUCoord;
        TexCoord.y = texVCoord;
        observer::Instance().addVertex(Vtype, m_pSegments[0].m_tipPointList[j],normal.Normalize(), TexCoord);
    }
    //initial segment cap 18 indices
    if(observer::Instance().DXRS->ReverseWindingOrder){
        //fill base cone; 0-6,  021,032,043,054,065,016;
        observer::Instance().addIndexes(idx,idx+2,idx+1);
        observer::Instance().addIndexes(idx,idx+3,idx+2);
        observer::Instance().addIndexes(idx,idx+4,idx+3);
        observer::Instance().addIndexes(idx,idx+5,idx+4);
        observer::Instance().addIndexes(idx,idx+6,idx+5);
        observer::Instance().addIndexes(idx,idx+1,idx+6);
    }else{

        //fill base cone; 0-6,  012,023,034,045,056,061;
        observer::Instance().addIndexes(idx,idx+1,idx+2);
        observer::Instance().addIndexes(idx,idx+2,idx+3);
        observer::Instance().addIndexes(idx,idx+3,idx+4);
        observer::Instance().addIndexes(idx,idx+4,idx+5);
        observer::Instance().addIndexes(idx,idx+5,idx+6);
        observer::Instance().addIndexes(idx,idx+6,idx+1);
    }
    std::uint16_t p = idx+1;
    idx = idx+7;
    //for each additional segment 
    //add 6 vertexes except for last segment which has 7
    for(int i=1;i<m_segmentCount;i++){
        for(int k=0;k<7;k++){
            if(k==0){
                if(i == m_segmentCount-1){
                   if(observer::Instance().DXRS->ReverseWindingOrder){
                       normal = normal.Cross((m_pSegments[i].m_tipPointList[2]-m_pSegments[i].m_tipPointList[0]),(m_pSegments[i].m_tipPointList[1]-m_pSegments[i].m_tipPointList[0])); 
                   }else{
                       normal = normal.Cross((m_pSegments[i].m_tipPointList[1]-m_pSegments[i].m_tipPointList[0]),(m_pSegments[i].m_tipPointList[2]-m_pSegments[i].m_tipPointList[0])); 
                   }

                    //normal = CrossProduct((m_pSegments[i].m_tipPointList[1]-m_pSegments[i].m_tipPointList[0]),(m_pSegments[i].m_tipPointList[2]-m_pSegments[i].m_tipPointList[0])); 
                    TexCoord.x = texUCoord;
                    TexCoord.y = texVCoord;
                    observer::Instance().addVertex(Vtype, m_pSegments[i].m_tipPointList[k], normal.Normalize(), TexCoord);
                    idx++;
                }
            }else{
                normal = m_pSegments[i].m_tipPointList[k]-m_pSegments[i].m_tipPointList[0];
                texUCoord = (float)i * 1.0f/(float)m_segmentCount;
                //texVCoord = (float)(k-1)* 1.0f/6.0f;
                if(k==0){
                    texVCoord = (float)0.0f;
                }else if(k<4){
                    texVCoord = (float)(k-1)* 1.0f/3.0f;
                }else {
                    texVCoord = 1.0f - (float)((k-1)%3)* 1.0f/3.0f;
                }
                
                TexCoord.x = texUCoord;
                TexCoord.y = texVCoord;
                
                observer::Instance().addVertex(Vtype, m_pSegments[i].m_tipPointList[k], normal.Normalize(), TexCoord);
            }
        }
        //Add 36 indices for each segment pair.
        if(observer::Instance().DXRS->ReverseWindingOrder){
       //fill segment[i] & [i-1]== [p]     p0.i1.i0, p0.p1.i1, p1.i2.i1, p1.p2.i2, p2.i3.i2, p2.p3.i3,
        //                                 p3.i4.i3, p3.p4.i4, p4.i5.i4, p4.p5.i5,
        //                                 p5.i0.i5, p5.p0.i0, p6.i0.i6, p6.p0.i0
        observer::Instance().addIndexes(p  ,idx+1,idx   );
        observer::Instance().addIndexes(p  ,p+1  ,idx+1 );
        observer::Instance().addIndexes(p+1,idx+2,idx+1 );
        observer::Instance().addIndexes(p+1,p+2  ,idx+2 );
        observer::Instance().addIndexes(p+2,idx+3,idx+2 );
        observer::Instance().addIndexes(p+2,p+3  ,idx+3 );
        observer::Instance().addIndexes(p+3,idx+4,idx+3 );
        observer::Instance().addIndexes(p+3,p+4  ,idx+4 );
        observer::Instance().addIndexes(p+4,idx+5,idx+4 );
        observer::Instance().addIndexes(p+4,p+5  ,idx+5 );
        observer::Instance().addIndexes(p+5,idx  ,idx+5 );
        observer::Instance().addIndexes(p+5,p    ,idx   ); 
        }else{
       //fill segment[i] & [i-1]== [p]     p0.i0.i1,   p0.i1.p1,  p1.i1.i2, p1.i2.p2, p2.i2.i3, p2.i3.p3,
        //                                  p3.i3.i4,  p3.i4.p4, p4.i4.i5, p4.i5.p5,
        //                                  p5.i5.i0,  p5.i0.p0, p6.i6.i0, p6.i0.p0
        observer::Instance().addIndexes(p,idx,idx+1);
        observer::Instance().addIndexes(p,idx+1,p+1);
        observer::Instance().addIndexes(p+1,idx+1,idx+2);
        observer::Instance().addIndexes(p+1,idx+2,p+2);
        observer::Instance().addIndexes(p+2,idx+2,idx+3);
        observer::Instance().addIndexes(p+2,idx+3,p+3);
        observer::Instance().addIndexes(p+3,idx+3,idx+4);
        observer::Instance().addIndexes(p+3,idx+4,p+4);
        observer::Instance().addIndexes(p+4,idx+4,idx+5);
        observer::Instance().addIndexes(p+4,idx+5,p+5);
        observer::Instance().addIndexes(p+5,idx+5,idx);
        observer::Instance().addIndexes(p+5,idx,p); 
        }
        p=p+6;
        // if it's the last segment add 18 indices
        if(i == m_segmentCount-1){
            idx--;
            if(observer::Instance().DXRS->ReverseWindingOrder){
            //fill tip cone; 0-6,  021,032,043,054,065,016;
            observer::Instance().addIndexes(idx,idx+1,idx+2);
            observer::Instance().addIndexes(idx,idx+2,idx+3);
            observer::Instance().addIndexes(idx,idx+3,idx+4);
            observer::Instance().addIndexes(idx,idx+4,idx+5);
            observer::Instance().addIndexes(idx,idx+5,idx+6);
            observer::Instance().addIndexes(idx,idx+6,idx+1);
            }else{
            //fill tip cone; 0-6,  012,023,034,045,056,061;
            observer::Instance().addIndexes(idx,idx+2,idx+1);
            observer::Instance().addIndexes(idx,idx+3,idx+2);
            observer::Instance().addIndexes(idx,idx+4,idx+3);
            observer::Instance().addIndexes(idx,idx+5,idx+4);
            observer::Instance().addIndexes(idx,idx+6,idx+5);
            observer::Instance().addIndexes(idx,idx+1,idx+6);
            }
            idx=idx+6;

        }
        idx=idx+6;

    }

}
