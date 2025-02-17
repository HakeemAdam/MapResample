
#include "MapResample.h"
#include <UT/UT_DSOVersion.h>

#include <OP/OP_OperatorTable.h>
#include <OP/OP_Operator.h>
#include <OP/OP_AutoLockInputs.h>

#include <GU/GU_Detail.h>

#include <PRM/PRM_Include.h>
#include  <PRM/PRM_Default.h>
#include <PRM/PRM_Template.h>

#include <cmath>


static PRM_Name parmNames[] =
    {
        PRM_Name("target", "Target"),
        PRM_Name("showDebug", "Show Debug"),
        PRM_Name(0)
    };

static PRM_Default parmDefaults[]=
    {
        PRM_Default(0.5f),
        PRM_Default(0)
    };

static PRM_Template resampleParms[]=
    {
    PRM_Template(PRM_FLT,1,&parmNames[0],&parmDefaults[0]),
    PRM_Template(PRM_TOGGLE,1,&parmNames[1],&parmDefaults[1]),
    PRM_Template()
    };

void newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "PG::VisvalingamLineSimplification",
        "PG::VisvalingamLineSimplification",
        MapResample::myConstructor,
        resampleParms,
       1,
       1,
       nullptr));
}

OP_Node* MapResample::myConstructor(OP_Network* net, const char* name, OP_Operator* op)
{
    return new MapResample(net, name, op);
}

MapResample::MapResample(OP_Network* net, const char* name, OP_Operator* op)
    :SOP_Node(net,name, op)
{
    mySopFlags.setManagesDataIDs(true);
}

MapResample::~MapResample()
=default;

OP_ERROR MapResample::cookMySop(OP_Context& context)
{
    OP_AutoLockInputs inputs(this);
    if (inputs.lock(context)>= UT_ERROR_ABORT)
        return error();

    fpreal now = context.getTime();
    duplicateSource(0, context);
    setVariableOrder(3,2,0,1);
    setCurGdh(0, myGdpHandle);

    setupLocalVars();

    fpreal target = evalFloat("target", 0, now);
    target /= gdp->getNumPoints();

    bool showDebug=evalInt("showDebug", 0, now);
    

    GA_RWHandleV3 pos_h(gdp->getP());
    if (!pos_h.isValid())
        return error();

    GA_PointGroup* doDelete = gdp->newPointGroup("toDelete");
    if (!doDelete)
        return error();
    
    UT_Array<GA_Offset> pointsToDelete;

    GA_Range pts_range = gdp->getPointRange();
    GA_Size numPoints = pts_range.getMaxEntries();

    UT_Array<GA_Offset> debugLines;
    GU_Detail debugGdp;
    GA_PrimitiveGroup* debugGroup = nullptr;

    if (showDebug)
    {
        debugGroup=gdp->newPrimitiveGroup("debugLines");
    }

    for (GA_Iterator it(pts_range); !it.atEnd(); ++it)
    {
        GA_Offset ptoff = *it;
        GA_Index idx = gdp->pointIndex(ptoff);

        myCurPtOff[0] = ptoff;

        if (idx ==0 || idx == numPoints-1)
            continue;

        UT_Vector3 curPos = pos_h.get(ptoff);

        if (idx + 2 >=numPoints)
            continue;

        GA_Offset nextOffset = gdp->pointOffset(idx+1);
        UT_Vector3 nextPos = pos_h.get(nextOffset);

        GA_Offset secondNextOffset = gdp->pointOffset(idx+2);
        UT_Vector3 secondNextPos = pos_h.get(secondNextOffset);

        UT_Vector3 edge1 = curPos - nextPos;
        UT_Vector3 edge2 = curPos - secondNextPos;

        UT_Vector3 crossVector = cross(edge1, edge2);
        fpreal area = crossVector.length() /2.0f;

        if (area< target)
        {
            pointsToDelete.append(ptoff);
            //doDelete->addOffset(ptoff);
        }

        if (showDebug)
        {
           
        }
        
    }
    
    for (GA_Offset i = 0; i < pointsToDelete.size(); i++)
   {
         gdp->destroyPointOffset(pointsToDelete(i));
        gdp->getAttributes().bumpAllDataIds(GA_ATTRIB_POINT);
        gdp->getAttributes().bumpAllDataIds(GA_ATTRIB_VERTEX);
        gdp->getAttributes().bumpAllDataIds((GA_ATTRIB_PRIMITIVE));
        gdp->getPrimitiveList().bumpDataId();
         gdp->normal();
         gdp->refreshCachedHandles();
        
        
    }
   
    gdp->normal();
    gdp->refreshCachedHandles();
        
    resetLocalVarRefs();
    return error();
}


// int pts[] = expandpointgroup(0,"");
// int remains[];
// float Areas[];
//
//
// float target = chf("target");
// target /= npoints(0);
//
// foreach(int pt; pts)
// {
//     if(pt !=0 && pt != len(pts)-1)
//     {
//     
//         //addprim(0, "polyline", pt, pt+2);
//         
//         vector edge1 = point(0, "P", pt) -point(0, "P", pt+1);
//         vector edge2 = point(0, "P", pt) - point(0, "P", pt+2);
//         
//         vector crossVector = cross(edge1, edge2);
//         float area = length(crossVector) /2;
//         push(Areas, area);
//         push(remains, pt);
//    
//         
//         if(area<target)
//         {
//             removepoint(0, pt);
//         }
//     }
// }
//
//
// i[]@testPoints=pts;
// i[]@remains=remains;
// f[]@Areas=Areas;