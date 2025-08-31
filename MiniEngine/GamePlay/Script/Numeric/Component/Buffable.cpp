#include "Buffable.h"

#include "Engine/Component/Audio/AudioSource.h"
#include "Engine/Component/Particle/ParticleSystem.h"

REGISTER_COMPONENT(Buffable, "Buffable")

void Buffable::awake()
{
    mBaseNumerical = NumericalObject(NumericalType::NT_TANK);
    mResultNumerical = mBaseNumerical;
}

void Buffable::update()
{
    //更新自己身上的buff
    if (mBuffMap.size() > 0)
    {
        auto itor = mBuffMap.begin();
        while (itor != mBuffMap.end())
        {
            itor->second->buffUpdate(mCommandLists, mResultNumerical);
            if (itor->second->isDead())
            {
                if (itor->second->isConflictSelf())
                { //只有和自己冲突时，才需要用到map
                    mBuffIdMap[itor->second->getId()] = nullptr;
                    if (itor->second->getId() == BuffId::CONTINUOUS_DAMAGE)
                    {
                        ParticleSystem* particle = dynamic_cast<ParticleSystem*>(mGameObject->getComponent("ParticleSystem"));
                        particle->stopGenerate();
                        AudioSource* audioSource = dynamic_cast<AudioSource*>(mGameObject->getComponent("AudioSource"));
                        audioSource->stop("fire");
                    }
                }
                SAFE_DELETE_POINTER(itor->second)
                itor = mBuffMap.erase(itor);
            }
            else
            {
                ++itor;
            }
        }
    }
    mBaseNumerical.executeCommondList(mCommandLists, mResultNumerical);
    mIsDie = mResultNumerical.isDie();
}

void Buffable::onDestory()
{
    //析构buff
    for (auto& buff : mBuffMap)
    {
        SAFE_DELETE_POINTER(buff.second)
    }
    mBuffMap.clear();
}

void Buffable::attachBuff(const TpList<BuffBase*>& buffList)
{
    for (auto& buff : buffList)
    {
        //判断如果是燃烧buff，则开启自己的粒子效果
        if (buff->getId() == BuffId::CONTINUOUS_DAMAGE)
        {
            Component* cm = mGameObject->getComponent("ParticleSystem");
            ParticleSystem* particle = nullptr;
            if (cm != nullptr)
            {
                particle = dynamic_cast<ParticleSystem*>(cm);
            }
            else
            {
                particle = dynamic_cast<ParticleSystem*>(mGameObject->addComponent("ParticleSystem"));
                particle->mEmitAngle = 30;
                particle->mRandomRadius = 1;
                particle->mEmitPerFrame = 5;
                particle->mAcceleration = {0,8,0};
                particle->mColor = Color::YELLOW;
                particle->mAirResistanceCoefficient = 0.05;
                particle->setParticleCount(5000);
                particle->isFire = true;
            }
            AudioSource* audioSource = dynamic_cast<AudioSource*>(mGameObject->getComponent("AudioSource"));
            audioSource->playLoop("fire");
            particle->startGenerate();
        }
        
        if (buff->isConflictSelf() && mBuffIdMap[buff->getId()])
        {
            //当buff是自我冲突类型，并且发现当前buffIDMap中还存在，则去刷新这个buff
            mBuffIdMap[buff->getId()]->refreshSelf();
        }
        else
        {
            //克隆一个新的buff存入
            BuffBase* newBuff = buff->clone();
            mBuffMap.insert(std::pair<UINT32, BuffBase*>(buff->getPriority(), newBuff));
            mBuffIdMap[buff->getId()] = newBuff;
        }
    }
}

