#include "EnemyController.h"

#include "Engine/Component/GameObject.h"
#include "Engine/Component/RenderComponent/MeshFilter.h"
#include "Engine/Component/RenderComponent/MeshRenderer.h"
#include "Engine/Component/TGUI/Canvas.h"
#include "Engine/Component/TGUI/ImageTGUI.h"
#include "Engine/FileManager/FileManager.h"
#include "GamePlay/Script/Numeric/Component/Buffable.h"
#include "GamePlay/Script/Utility/DelayDestruction.h"

REGISTER_COMPONENT(EnemyController, "EnemyController")

void EnemyController::awake()
{
    mTransform = mGameObject->getTransform();
    mRigidBody = dynamic_cast<RigidBody*>(mGameObject->getComponent("RigidBody"));
    mLifeBarPoint = mGameObject->getTransform()->getChildWithName("LifeBarPoint");
    mTankCamera = dynamic_cast<Camera*>(
        Transform::sFindGameObject("tankCamera")->getGameObject()->getComponent("Camera"));
    mCanvas = Canvas::sGetCurrentCanvas();
    mBuffable = dynamic_cast<Buffable*>(mGameObject->getComponent("Buffable"));

    //获取所有坦克部件
    mBodyMeshFilter = dynamic_cast<MeshFilter*>(mTransform->getChildWithName("TankBody")->getGameObject()->getComponent("MeshFilter"));
    mBodyMeshRenderer = dynamic_cast<MeshRenderer*>(mTransform->getChildWithName("TankBody")->getGameObject()->getComponent("MeshRenderer"));
    mBatteryMeshFilter = dynamic_cast<MeshFilter*>(mTransform->getChildWithName("TankBattery")->getGameObject()->getComponent("MeshFilter"));
    mBatteryMeshRenderer = dynamic_cast<MeshRenderer*>(mTransform->getChildWithName("TankBattery")->getGameObject()->getComponent("MeshRenderer"));
}

void EnemyController::start()
{
    GameObject* lifeBack = GameObjectFactory::sCreateGameObject("lifeBack");
    lifeBack->addRectTransform(mCanvas->getTransform(), mCanvas);
    ImageTGUI* imageBack = dynamic_cast<ImageTGUI*>(lifeBack->addComponent("ImageTGUI"));
    imageBack->setBlendFactor(1);
    imageBack->setColor({0,1,1});
    mLifeBackRect = dynamic_cast<RectTransform*>(lifeBack->getTransform());
    mLifeBackRect->setWidth(mRectWidth);
    mLifeBackRect->setHeight(mRectHeight);
    mLifeBackRect->setPivotPos({0,0});
    mLifeBackRect->setPivotType(PivotType::PIVOT_CENTER_LEFT);
    
    GameObject* lifeBar = GameObjectFactory::sCreateGameObject("lifeBar");
    lifeBar->addRectTransform(mCanvas->getTransform(), mCanvas);
    ImageTGUI* image = dynamic_cast<ImageTGUI*>(lifeBar->addComponent("ImageTGUI"));
    image->setTexture("HP_R");
    mLifeRect= dynamic_cast<RectTransform*>(lifeBar->getTransform());
    mLifeRect->setWidth(mRectWidth);
    mLifeRect->setHeight(mRectHeight);
    mLifeRect->setPivotPos({0,0});
    mLifeRect->setPivotType(PivotType::PIVOT_CENTER_LEFT);
}

void EnemyController::update()
{
    Vector3 worldPos = mLifeBarPoint->getWorldPosition();
    float distance = Vector3::Distance(worldPos,mTankCamera->getGameObject()->getTransform()->getWorldPosition());
    static const float MAX_DISTANCE = 100.0f;
    if (distance > MAX_DISTANCE)
    {
        mLifeRect->getGameObject()->deactiveGameObject();
        mLifeBackRect->getGameObject()->deactiveGameObject();
        return;
    }
    else
    {
        mLifeRect->getGameObject()->activeGameObject();
        mLifeBackRect->getGameObject()->activeGameObject();
    }
    mLifeRect->setWidth(mBuffable->getLifePecentage() * mRectWidth);

    //偏移半个width
    Vector2 ScreenPos = mTankCamera->transformWorldToScreen(worldPos);
    ScreenPos.v.x -= mRectWidth/2;
    mLifeRect->setPivotPos(ScreenPos);
    mLifeBackRect->setPivotPos(ScreenPos);

    //处理模型转换
    if (mEnemyState != EnemyState::ES_DEAD && mBuffable->isDie())
    {
        mEnemyState = EnemyState::ES_DEAD;
        mBodyMeshFilter->setMesh("TankBodyDestroy");
        mBodyMeshRenderer->setTexture("TankTexDestroy");
        mBatteryMeshFilter->setMesh("TankBatteryDestroy");
        mBatteryMeshRenderer->setTexture("TankTexDestroy");
        DelayDestruction* delay = dynamic_cast<DelayDestruction*>(mGameObject->addComponent("DelayDestruction"));
        delay->setDelayTime(2);
        delay->startTiming();
    }
}

void EnemyController::onDestory()
{
    GameObject* lifebar = mLifeRect->getGameObject();
    GameObjectFactory::sDestroyGameObject(lifebar);
    GameObject* lifeBack = mLifeBackRect->getGameObject();
    GameObjectFactory::sDestroyGameObject(lifeBack);
}
