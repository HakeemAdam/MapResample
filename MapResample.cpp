
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

static PRM_Template resampleParms[]=
    {
    PRM_Template()
    };

void newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "PG::Visvalingam Line Simplification",
        "PG::Visvalingam Line Simplification",
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

    return error();
}
