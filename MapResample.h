#ifndef __MAPRESAMPLE_H__
#define __MAPRESAMPLE_H__
#include <SOP/SOP_Node.h>


class MapResample : public SOP_Node
{

public:
    static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator* op);
    static const  PRM_Template MyNodeParams;
protected:
    MapResample(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~MapResample() override;

    virtual OP_ERROR cookMySop(OP_Context &context) override;

private:
};


#endif