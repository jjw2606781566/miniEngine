#include "ShellFactory.h"

#include "ShellController.h"
#include "Engine/Component/Audio/AudioSource.h"
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/ComponentHeader/TankinRenderComponent.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/FileManager/FileManager.h"
#include "GamePlay/Script/Numeric/Buff/Buffs/ContinuousDamageBuff.h"
#include "GamePlay/Script/Numeric/Buff/Buffs/DamageBuff.h"
#include "GamePlay/Script/Numeric/Component/BuffAttacher.h"
#include "GamePlay/Script/Numeric/Component/Buffable.h"

GameObject* ShellFactory::sCreateShell(Transform* BatteryPointTransform, const TpString& meshName,
    const TpString& textureName, const TpString& shaderName)
{
    GameObject* shell = GameObjectFactory::sCreateGameObject("shell");
    shell->setTag("Shell");
    
    //设置transform
    shell->addTransform();
    Transform* shellTransform = shell->getTransform();
    shellTransform->setWorldPosition(BatteryPointTransform->getWorldPosition());
    shellTransform->setLocalScale({0.1,0.1,0.1});
    shellTransform->setWorldRotation(BatteryPointTransform->getWorldRotation());
    shellTransform->rotateLocalPitchYawRoll({90* MathUtils::DEG_TO_RAD,0,0});

    //设置mesh
    MeshFilter* meshFilter = dynamic_cast<MeshFilter*>(shell->addComponent("MeshFilter"));
    meshFilter->setMesh(meshName); //Shell
    MeshRenderer* render = dynamic_cast<MeshRenderer*>(shell->addComponent("MeshRenderer"));
    render->setShader(shaderName); //ui
    render->setTexture(textureName); //Shell
    render->setBlendFactor(0.3);
    render->setColor(Color::WHITE);

    return shell;
}

void ShellFactory::sSetPhysics(GameObject* shell, Transform* BatteryPointTransform, bool isGravity)
{
    //设置物理
    RigidBody* rigidBody = dynamic_cast<RigidBody*>(shell->addComponent("RigidBody"));
    rigidBody->setVelocity(BatteryPointTransform->getForward() * 50);
    rigidBody->setMass(0.0001);
    rigidBody->isGravity(isGravity);
}

ShellController* ShellFactory::sSetController(GameObject* shell)
{
    ShellController* shellController = dynamic_cast<ShellController*>(shell->addComponent("ShellController"));
    return shellController;
}

void ShellFactory::sSetTailGas(ShellController* shellController)
{
    //添加尾气
    GameObject* tempParticle = GameObjectFactory::sCreateGameObject("tempParticle");
    tempParticle->addTransform();
    ParticleSystem* shellParticle =  dynamic_cast<ParticleSystem*>(tempParticle->addComponent("ParticleSystem"));
    shellParticle->startGenerate();
    shellParticle->mAcceleration = {0,0,-5};
    shellParticle->mEmitAngle = 10;
    shellParticle->mRandomRadius = 0.1;
    shellParticle->isFire = true;
    //添加尾气自动删除自己
    DelayDestruction* delayDestruction = dynamic_cast<DelayDestruction*>(tempParticle->addComponent("DelayDestruction"));
    shellController->mParticleSystem = shellParticle;
    shellController->mDelayDestruction = delayDestruction;
}

GameObject* ShellFactory::sSetBoom(ShellController* shellController, const Color& color)
{
    //添加爆炸效果
    GameObject* tempParticle = GameObjectFactory::sCreateGameObject("tempParticle");
    tempParticle->addTransform();
    ParticleSystem* shellParticle =  dynamic_cast<ParticleSystem*>(tempParticle->addComponent("ParticleSystem"));
    shellParticle->mParticleCount = 5000;
    shellParticle->mParticleSpeed = 15;
    shellParticle->mEmitAngle = 360;
    shellParticle->mEmitPerFrame = 500;
    shellParticle->mLifeTimeSecond = 0.5;
    shellParticle->mAcceleration = {0,-50,0};
    shellParticle->mRandomSizeRange = {1,5};
    shellParticle->mIsEmitOnlyOnce = true;
    shellParticle->mColor = color;
    shellParticle->isFire = true;
    shellParticle->stopGenerate();
    
    
    //添加爆炸效果自动删除自己
    DelayDestruction* delayDestruction = dynamic_cast<DelayDestruction*>(tempParticle->addComponent("DelayDestruction"));
    delayDestruction->setDelayTime(2);
    shellController->mBoomParticleSystem = shellParticle;
    shellController->mBoomDelayDestruction = delayDestruction;

    return tempParticle;
}

void ShellFactory::sSetAudio(GameObject* shellGo, GameObject* particleGo)
{
    AudioSource* audio = dynamic_cast<AudioSource*>(particleGo->addComponent("AudioSource"));
    
    audio->addAuidioClip("ShellBoom", FileManager::sGetLoadedBolbFile<AudioClip*>("ShellBoom"));
    audio->set3DMode("ShellBoom", true);
    RigidBody* rb = dynamic_cast<RigidBody*>(shellGo->getComponent("RigidBody"));
    rb->addCollisonCallback([audio](RigidBody* self, RigidBody* other)
    {
        if (other->getGameObject()->getTag() == "Aming" || other->getGameObject()->getTag() == "Shell")
        {
            return;
        }

        audio->play("ShellBoom");
    });
}


GameObject* ShellFactory::sCreateCommonBullet(Transform* BatteryPointTransform, Buffable* buffable, const TpString& targetTag)
{
    GameObject* shell = sCreateShell(BatteryPointTransform, "Shell", "Shell", "ui");
    sSetPhysics(shell, BatteryPointTransform);
    ShellController* shellController = sSetController(shell);
    sSetTailGas(shellController);
    auto tempParticle = sSetBoom(shellController, {1, 0.87f, 0});
    sSetAudio(shell, tempParticle);

    //添加数值
    DamageBuff* damageBuff = new DamageBuff();
    NumericalObject* numerical = buffable->getResultNumerical();
    damageBuff->mDamageValue = numerical->getAttribute(AttributeType::AT_ATTACK);
    BuffAttacher* buffAttacher = dynamic_cast<BuffAttacher*>(shell->addComponent("BuffAttacher"));
    buffAttacher->addTargetTag(targetTag);
    buffAttacher->addBuff(damageBuff);
    buffAttacher->damage = damageBuff->mDamageValue;
    return shell;
}

GameObject* ShellFactory::sCreateBurnBullet(Transform* BatteryPointTransform, Buffable* buffable, const TpString& targetTag)
{
    GameObject* shell = sCreateShell(BatteryPointTransform, "Shell", "Shell", "ui");
    sSetPhysics(shell, BatteryPointTransform);
    ShellController* shellController = sSetController(shell);
    sSetTailGas(shellController);
    auto tempParticle = sSetBoom(shellController, Color::RED);
    sSetAudio(shell, tempParticle);

    //添加数值
    BuffAttacher* buffAttacher = dynamic_cast<BuffAttacher*>(shell->addComponent("BuffAttacher"));
    buffAttacher->addTargetTag(targetTag);

    //创建持续伤害buff
    ContinuousDamageBuff* ConDamageBuff = new ContinuousDamageBuff();
    ConDamageBuff->setLifeTime(5.0f);
    NumericalObject* numerical = buffable->getResultNumerical();
    ConDamageBuff->mDamageValue = 4;
    buffAttacher->addBuff(ConDamageBuff);

    //创建瞬间伤害buff
    DamageBuff* damageBuff = new DamageBuff();
    damageBuff->mDamageValue = numerical->getAttribute(AttributeType::AT_ATTACK)*0.5;
    buffAttacher->addBuff(damageBuff);
    buffAttacher->damage = damageBuff->mDamageValue;
    return shell;
}

