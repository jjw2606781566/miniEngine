#include "AmingLine.h"

#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Physical/Shape/BoxShape.h"
#include "Engine/Utility/GameTime/GameTime.h"

REGISTER_COMPONENT(AmingLine, "AmingLine")

void AmingLine::awake()
{
    maxCubeCount = 100;
    
    cubeTransforms.resize(maxCubeCount);

    mAmingRoot = GameObjectFactory::sCreateGameObject("AmingLine");
    mAmingRoot->addTransform();

    for (UINT32 i = 0; i < maxCubeCount; i++)
    {
        GameObject* subPoint = GameObjectFactory::sCreateGameObject("AmingSubPoint");
        subPoint->addTransform(mAmingRoot->getTransform());
        Transform* trans = subPoint->getTransform();
        trans->setLocalScale({0.05,0.05,0.05});

        subPoint->addComponent("MeshFilter");
        subPoint->addComponent("MeshRenderer");
        subPoint->setTag("Aming");
        RigidBody* rb = dynamic_cast<RigidBody*>(subPoint->addComponent("RigidBody"));
        BoxShape* box = new BoxShape({0.05,0.05,0.05});
        rb->setShape(box);
        rb->setMass(0.0001);
        rb->isGravity(false);
        rb->addCollisonCallback([this, i](RigidBody* self, RigidBody* other)
        {
            if (other->getGameObject()->getTag() == "Shell")
            {// 如果碰到子弹，则不更改
                return;
            }
            mCurrentAimingTarget = other->getGameObject();
            currentCollisionCube = std::min(i, currentCollisionCube);
        });
        
        cubeTransforms[i] = trans;
    }
    mAmingRoot->deactiveGameObject();
}

void AmingLine::drawLine(const Vector3& startPos, const Vector3& startDir, float speed, float splitTime)
{
    if (!mAmingRoot->isActive())
        mAmingRoot->activeGameObject();

    const Vector3 acceleration = {0,-9.8,0};
    Vector3 currentPos = startPos;
    Vector3 currentSpeed = startDir*speed;
    for (UINT32 i = 0; i < maxCubeCount; i++)
    {
        //首先模拟子弹轨迹
        //计算当帧位移, x = v0t + 0.5*a*t^2
        //currentPos += currentSpeed * splitTime + acceleration * splitTime * splitTime * 0.5;

        //更新速度
        currentSpeed += (acceleration * splitTime);
        currentPos += (currentSpeed * splitTime);

        //更新小方块位置
        if (i > currentCollisionCube)//第一个撞到粒子之后的粒子不渲染
        {
            cubeTransforms[i]->getGameObject()->deactiveGameObject();
        }
        else if (i == currentCollisionCube) //撞到的变大
        {
            cubeTransforms[i]->getGameObject()->activeGameObject();
            cubeTransforms[i]->setLocalScale({0.6,0.6,0.6});
            cubeTransforms[i]->setWorldPosition(currentPos);
            //有个旋转效果好看一下
            cubeTransforms[i]->rotateAroundLocalAxis({0,1,0}, GameTime::sGetDeltaTime()*10);
        }
        else
        {
            cubeTransforms[i]->getGameObject()->activeGameObject();
            cubeTransforms[i]->setWorldPosition(currentPos);
            //有个旋转效果好看一下
            cubeTransforms[i]->rotateAroundLocalAxis({0,1,0}, GameTime::sGetDeltaTime()*10);
            cubeTransforms[i]->setLocalScale({0.05,0.05,0.05});
        }
    }
    //每帧结束恢复状态
    currentCollisionCube = 999999;
}
