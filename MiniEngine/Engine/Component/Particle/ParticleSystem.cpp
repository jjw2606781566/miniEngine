#include "ParticleSystem.h"

#include "Engine/Component/GameObject.h"
#include "Engine/Utility/GameTime/GameTime.h"

#include "Engine/Component/RenderComponent/Camera.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/render/MeshData.h"
#include "Engine/render/Renderer.h"
#include "Engine/Utility/Random.h"

REGISTER_COMPONENT(ParticleSystem, "ParticleSystem")

void ParticleSystem::awake()
{
    mTransform = mGameObject->getTransform();
    mParticles.resize(mParticleCount);
}

void ParticleSystem::setParticleCount(int count)
{
    mParticleCount = count;
    mParticles.clear();
    mParticles.resize(mParticleCount);
}

void ParticleSystem::activateParticles()
{
    if (mIsStop)
        return;
    int count = 0;
    for (auto& particle : mParticles)
    {
        if (particle.mIsDie == true)
        {
            particle.mIsDie = false;
            
            //random position
            float randomRadius = mRandomRadius * (Random::Float() * 2 - 1);
            float randomTheta = Random::Float() * 2 * MathUtils::PI;
            float randomPhi = Random::Float() * MathUtils::PI;
            Vector3 randomPos = {
                randomRadius * sin(randomTheta) * cos(randomPhi),
                randomRadius * sin(randomTheta) * sin(randomPhi),
                randomRadius * cos(randomTheta)
            };
            particle.mPosition = mTransform->getWorldPosition() + randomPos;
            particle.mSizeScale = (Random::Float() * (mRandomSizeRange.v.y - mRandomSizeRange.v.x) + mRandomSizeRange.v.x);
            //random direction
            particle.mVelocity =
                getRandomDirectionInCone(mTransform->getForward(),mTransform->getRight(), mEmitAngle*MathUtils::DEG_TO_RAD)
                * mParticleSpeed;
            Quaternion q = mTransform->getGameObject()->getTransform()->getWorldRotation();
            particle.mAcceleration = mAcceleration;
            q.QuaternionRotateVector(particle.mAcceleration);
            particle.mRemainLifeTime = mLifeTimeSecond * (Random::Float() + 0.5);
            if (particle.mMaterialGpu == nullptr)
            {
                particle.mMaterialGpu = Renderer::GetInstance().createMaterialInstance(
                    *FileManager::sGetLoadedBolbFile<Material*>("particle"));
            }
            count++;
            if (count == mEmitPerFrame)
            {
                break;
            }
        }
        if (mIsEmitOnlyOnce && count == 0)
        {
            mIsStop = true;
        }
    }
}

void ParticleSystem::update()
{
    activateParticles();
    for (auto& particle : mParticles)
    {
        if (!particle.mIsDie)
        {
            particle.update(GameTime::sGetDeltaTime(), mAirResistanceCoefficient);
        }
    }
}

void ParticleSystem::prepareRenderList()
{
    for (auto& particle : mParticles)
    {
        if (!particle.mIsDie)
        {
            RenderItem renderItem;

            MeshData meshData = FileManager::sGetLoadedBolbFile<MeshData>(mMeshName);
            renderItem.mMeshData = meshData;

            renderItem.mModel = particle.getModelMatrix();

            renderItem.mMaterial = particle.mMaterialGpu.get();

            // const Blob& cbuffer = particle.mMaterialGpu->GetConstantBuffer(0);
            struct alignas(256) UniversalCBuffer
            {
                float mColor[4];
                float mBlendFactor[4];
            }universalCBuffer;
            
            if (isFire)
            {
                Color finalColor = mColor;
                float lerpFactor = particle.mRemainLifeTime / mLifeTimeSecond;
                if (lerpFactor > 0.75)
                {
                    //大于0.5时，应该是黄色向红色进行插值
                    lerpFactor -= 0.5;
                    lerpFactor *= 2;
                    finalColor.value = Vector3::Lerp(Color::RED.value, Color::YELLOW.value, lerpFactor);
                }
                else
                {
                    //小于0.5时，应该是红色向黑色进行插值
                    //讓黑色快點出來
                    lerpFactor *= 2;
                    finalColor.value = Vector3::Lerp(Color::BLACK.value, Color::RED.value, lerpFactor-0.4);
                }

                universalCBuffer.mColor[0] = finalColor.value.v.x;
                universalCBuffer.mColor[1] = finalColor.value.v.y;
                universalCBuffer.mColor[2] = finalColor.value.v.z;
                universalCBuffer.mColor[3] = 1;
                universalCBuffer.mBlendFactor[0] = 1;
                particle.mMaterialGpu->UpdateConstantBuffer(0, &universalCBuffer, sizeof(universalCBuffer));
            }
            else
            {
                universalCBuffer.mColor[0] = mColor.value.v.x;
                universalCBuffer.mColor[1] = mColor.value.v.y;
                universalCBuffer.mColor[2] = mColor.value.v.z;
                universalCBuffer.mColor[3] = 1;
                universalCBuffer.mBlendFactor[0] = 1;
                particle.mMaterialGpu->UpdateConstantBuffer(0, &universalCBuffer, sizeof(universalCBuffer));
            }
            
            Camera* currentCamera = Camera::sGetCurrentCamera();
            ASSERT(currentCamera, TEXT("current camera is null!"));
            currentCamera->addRenderItem(renderItem);
        }
    }
}

Vector3 ParticleSystem::getRandomDirectionInCone(Vector3 forward, Vector3 right, float coneAngleRad) const
{
    float randomAxisAngle = Random::Float() * 2 * MathUtils::PI;
    Quaternion q;
    q.setToRotateAboutAxis(forward, randomAxisAngle);
    q.QuaternionRotateVector(right);
    float randomRadian = (Random::Float() * 2 - 1) * coneAngleRad;
    q.setToRotateAboutAxis(right, randomRadian);
    q.QuaternionRotateVector(forward);
    return forward;
}

rapidxml::xml_node<>* ParticleSystem::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
                                                const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("ParticleSystem")));
    
    auto particleNode = doc->allocate_node(rapidxml::node_element, "ParticleParameter");
    
    mAcceleration.serialize(doc, particleNode, "Acceleration");
    mRandomSizeRange.serialize(doc, particleNode, "RandomSize");
    auto ColorNode = doc->allocate_node(rapidxml::node_element,"Color");
    particleNode->append_node(ColorNode);
    ColorNode->append_attribute(doc->allocate_attribute("r", doc->allocate_string(std::to_string(mColor.value.v.x).c_str())));
    ColorNode->append_attribute(doc->allocate_attribute("g", doc->allocate_string(std::to_string(mColor.value.v.y).c_str())));
    ColorNode->append_attribute(doc->allocate_attribute("b", doc->allocate_string(std::to_string(mColor.value.v.y).c_str())));
    
    mXmlNode->append_node(particleNode);

    particleNode->append_attribute(doc->allocate_attribute("ParticleCount"
        , doc->allocate_string(std::to_string(mParticleCount).c_str())));
    particleNode->append_attribute(doc->allocate_attribute("ParticleSpeed"
        , doc->allocate_string(std::to_string(mParticleSpeed).c_str())));
    particleNode->append_attribute(doc->allocate_attribute("EmitAngle"
        , doc->allocate_string(std::to_string(mEmitAngle).c_str())));
    particleNode->append_attribute(doc->allocate_attribute("EmitPerFrame"
        , doc->allocate_string(std::to_string(mEmitPerFrame).c_str())));
    particleNode->append_attribute(doc->allocate_attribute("LifeTimeSecond"
        , doc->allocate_string(std::to_string(mLifeTimeSecond).c_str())));
    particleNode->append_attribute(doc->allocate_attribute("AirResistanceCoefficient"
        , doc->allocate_string(std::to_string(mAirResistanceCoefficient).c_str())));
    particleNode->append_attribute(doc->allocate_attribute("RandomRadius"
        , doc->allocate_string(std::to_string(mRandomRadius).c_str())));

    //render
    auto renderNode = doc->allocate_node(rapidxml::node_element, "ParticleRender");
    mXmlNode->append_node(renderNode);
    renderNode->append_attribute(doc->allocate_attribute("TextureName"
        , doc->allocate_string(mTextureName.c_str())));
    renderNode->append_attribute(doc->allocate_attribute("MeshName"
        , doc->allocate_string(mMeshName.c_str())));
    renderNode->append_attribute(doc->allocate_attribute("ShaderName"
        , doc->allocate_string(mShaderName.c_str())));
    
    return mXmlNode;
}

void ParticleSystem::deSerialize(const rapidxml::xml_node<>* node)
{
    auto currentNode = node->first_node("ParticleParameter");
    mParticleCount = std::stoi(currentNode->first_attribute("ParticleCount")->value());
    mParticleSpeed = std::stof(currentNode->first_attribute("ParticleSpeed")->value());
    mEmitAngle = std::stof(currentNode->first_attribute("EmitAngle")->value());
    mEmitPerFrame = std::stoi(currentNode->first_attribute("EmitPerFrame")->value());
    mLifeTimeSecond = std::stof(currentNode->first_attribute("LifeTimeSecond")->value());
    mAirResistanceCoefficient = std::stof(currentNode->first_attribute("AirResistanceCoefficient")->value());
    mRandomRadius = std::stof(currentNode->first_attribute("RandomRadius")->value());

    mAcceleration.deSerialize(currentNode->first_node("Vector3"));
    mRandomSizeRange.deSerialize(currentNode->first_node("Vector2"));
    auto colorNode = currentNode->first_node("Color");
    mColor.value.v.x = std::stof(colorNode->first_attribute("r")->value());
    mColor.value.v.y = std::stof(colorNode->first_attribute("g")->value());
    mColor.value.v.z = std::stof(colorNode->first_attribute("b")->value());

    currentNode = currentNode->next_sibling("ParticleRender");
    mTextureName = currentNode->first_attribute("TextureName")->value();
    mMeshName = currentNode->first_attribute("MeshName")->value();
    mShaderName = currentNode->first_attribute("ShaderName")->value();
}

void ParticleSystem::showSelf()
{
#ifdef WIN32
    if (ImGui::TreeNode(ComponentRegister::sGetClassName(this).c_str()))
    {
        ImGui::Checkbox("Stop##xx", &mIsStop);
        ImGui::Checkbox("EmitOnlyOnce", &mIsEmitOnlyOnce);
        ImGui::InputFloat("ParticleSpeed", &mParticleSpeed);
        ImGui::InputFloat("EmitAngle", &mEmitAngle);
        ImGui::InputInt("EmitPerFrame", &mEmitPerFrame);
        ImGui::InputFloat("LifeTimeSecond", &mLifeTimeSecond);
        ImGui::InputFloat("AirResistanceCoefficient", &mAirResistanceCoefficient);
        ImGui::InputFloat("RandomRadius", &mRandomRadius);
        ImGui::InputFloat3("Acceleration", &mAcceleration.v.x);
        ImGui::InputFloat2("RandomSizeScale", &mRandomSizeRange.v.x);
        if (ImGui::InputInt("ParticleCount", &mParticleCount))
        {
            setParticleCount(mParticleCount);
        }
        ImGui::TreePop();
    }
#endif
}
