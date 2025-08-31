#pragma once
#include "Engine/Component/Component.h"
#include "Engine/Component/Transform.h"
#include "Particle.h"
#include "Engine/render/Color.h"

class ParticleSystem:public Component
{
public:
    void awake() override;
    void update() override;
    
    void setParticleCount(int count);
    
    void stopGenerate(){mIsStop = true;}
    void startGenerate(){mIsStop = false;}
    
    void activateParticles();
    void prepareRenderList();
    Vector3 getRandomDirectionInCone(Vector3 forward, Vector3 right, float coneAngleRad) const;
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    void showSelf() override;

    //particle parameter
    //最大粒子数量
    int mParticleCount = 100;
    //粒子初始速率
    float mParticleSpeed = 1.0f;
    //粒子加速度
    Vector3 mAcceleration = { 0, -PARTICLE_GRAVITY, 0 };
    //初始随机方向角度，是一种锥形发射
    float mEmitAngle = 30.0f;
    //每帧发射数量
    int mEmitPerFrame = 1;
    //粒子存活时间
    float mLifeTimeSecond = 1.0f;
    //空气阻力
    float mAirResistanceCoefficient = 0.1f;
    //发射初始随机位置的半径（球形区域随机初始位置）
    float mRandomRadius = 1;
    //随机粒子大小范围
    Vector2 mRandomSizeRange = {1,5};
    bool mIsEmitOnlyOnce = false;
    Color mColor = Color::BLACK;

    float isFire = false;
    
private:
    Transform* mTransform = nullptr;
    std::vector<Particle> mParticles;

    //render
    TpString mTextureName;
    TpString mMeshName = "DefaultCube";
    TpString mShaderName = "debug";

    bool mIsStop = false;
    float mBlendFactor = 1;
};
