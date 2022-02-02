#pragma once
#include "btBulletDynamicsCommon.h"
#include <external/bullet3/examples/CommonInterfaces/CommonRigidBodyBase.h>

class ColliderComponent : public CommonRigidBodyBase
{
public:
    ColliderComponent(struct GUIHelperInterface* helper) : CommonRigidBodyBase(helper){}
    virtual ~ColliderComponent(){}
    virtual void initColliders();


private:
    
};
