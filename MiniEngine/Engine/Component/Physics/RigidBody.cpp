#include "RigidBody.h"

#include "Engine/AISystem/NavigationMap.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/RenderComponent/Camera.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/Physical/PhysicSystem.h"
#include "Engine/Physical/Shape/BoxShape.h"
#include "Engine/Physical/Shape/SphereShape.h"
#include "Engine/render/MeshData.h"
#include "Engine/render/Renderer.h"
#include "Engine/Utility/GameTime/GameTime.h"

REGISTER_COMPONENT(RigidBody,"RigidBody")

RigidBody::RigidBody()
{
    mass = 1.0f;
    invMass = (mass != 0.0f) ? 1.0f / mass : 0.0f;
    mShape = nullptr;
    velocity = Vector3(0, 0, 0);
    force = Vector3(0, 0, 0);
    PhysicSystem::getInstance().addRigidBody(this);
}

RigidBody::~RigidBody()
{
    PhysicSystem::getInstance().removeRigidBody(this);
    delete mShape;
}

void RigidBody::awake()
{
    mMaterialGpu = Renderer::GetInstance().createMaterialInstance(*FileManager::sGetLoadedBolbFile<Material*>("debug"));
    mMaterialGpu->SetDrawMode(DrawMode::WIREFRAME);
}

// 获取质量
float RigidBody::getMass() const
{
    return mass;
}

float RigidBody::getInvMass() const
{
    return invMass;
}

// 设置质量和质量倒数
void RigidBody::setMass(float newMass)
{
    mass = newMass;
    invMass = (newMass != 0.0f) ? 1.0f / newMass : 0.0f;
}

void RigidBody::setShape(BaseShape* newShape)
{
    mShape = newShape;
    if (mShape) {
        volume = mShape->getVolume();  
    }
    if (mShape != nullptr) {
        mShape->setPosition(getTransform()->getWorldPosition());
    }
}

// 从GameObject中获取对应的位置坐标。
Vector3 RigidBody::getPosition() const
{
    return getGameObject()->getTransform()->getWorldPosition();
}

// 获得速度
Vector3 RigidBody::getVelocity() const
{
    return velocity;
}

void RigidBody::applyVelocity(Vector3 newVelocity)
{
    velocity += newVelocity;
}

// 设置速度
void RigidBody::setVelocity(const Vector3& newVelocity)
{
    velocity = newVelocity;
}

Vector3 RigidBody::getForce() const
{
    return force;
}

// 施加力
void RigidBody::applyForce(const Vector3& newForce)
{
    force += newForce;
}

void RigidBody::setForce(const Vector3& newForce)
{
    force = newForce;
}

ShapeType RigidBody::getShapeType() const
{
    if (mShape) {
        return mShape->getType();
    }
    return ShapeType::Sphere;
}

void RigidBody::getAABB(Vector3& min, Vector3& max) const
{
    Vector3 position = getPosition();
    if (mShape) {
        mShape->getAABB(position, min, max);
    }
}

bool RigidBody::localIsInside(const Vector3& point) const
{
    if (mShape) {
        return mShape->localIsInside(point);
    }
    return false;
}

Transform* RigidBody::getTransform() const
{
    return getGameObject()->getTransform();
}


void RigidBody::isGravity(bool newIsGravity)
{
    useGravity = newIsGravity;
}


bool RigidBody::getIsGravity() const
{
    return useGravity;
}

void RigidBody::addCollisonCallback(const std::function<void(RigidBody*, RigidBody*)>& newCallback)
{
    onCollisionList.push_back(newCallback);
}

void RigidBody::clearCollisionCallback()
{
    onCollisionList.clear();
}

void RigidBody::prepareRenderList() const
{
    if (mShape == nullptr)
        return;
    DEBUG_PRINT("Render %s Collider\n", getGameObject()->getName().c_str());

    if (mShape->getType() == ShapeType::Box)
    {
        BoxShape* boxShape = dynamic_cast<BoxShape*>(mShape);
        //Mesh
        MeshData meshData = FileManager::sGetLoadedBolbFile<MeshData>("DefaultCube");

        RenderItem renderItem;
        renderItem.mMeshData = meshData;
    
        //Matrix, only position influence collider
        Transform* transform = dynamic_cast<Transform*>(getGameObject()->getComponent("Transform"));
        Matrix4x4 model;
        model.setModelMatrix(transform->getWorldPosition(), {0,0,0}, boxShape->getSize());
        renderItem.mModel = model;
        
        renderItem.mMaterial = mMaterialGpu.get();

        Camera* currentCamera = Camera::sGetCurrentCamera();
        currentCamera->addRenderItem(renderItem);
    }
}

rapidxml::xml_node<>* RigidBody::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("RigidBody")));

    mXmlNode->append_attribute(doc->allocate_attribute("mass", doc->allocate_string(std::to_string(mass).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("useGravity", doc->allocate_string(useGravity ? "1" : "0")));

    if (mShape != nullptr)
    {
        if (mShape->getType() == ShapeType::Box)
        {
            mShape->serialize(doc, mXmlNode, "BoxShape");
        }
        else if (mShape->getType() == ShapeType::Sphere)
        {
            mShape->serialize(doc, mXmlNode, "SphereShape");
        }
    }
    
    return mXmlNode;
}

void RigidBody::deSerialize(const rapidxml::xml_node<>* node)
{
    float newMass = std::stof(node->first_attribute("mass")->value());
    setMass(newMass);
    useGravity = std::string(node->first_attribute("useGravity")->value()) == "1";
    
    auto boxNode = node->first_node("BoxShape");
    if (boxNode)
    {
        Vector3 size;
        auto sizeNode = boxNode->first_node("Vector3");
        if (sizeNode) {
            size.deSerialize(sizeNode);
        } else {
            size = Vector3(1.0f, 1.0f, 1.0f);
        }
        
        setShape(new BoxShape(size));
        return;
    }
    auto sphereNode = node->first_node("SphereShape");
    if (sphereNode)
    {
        float radius = std::stof(sphereNode->first_attribute("radius")->value());
        
        setShape(new SphereShape(radius));
        return;
    }
}

void RigidBody::showSelf()
{
#ifdef WIN32
    const static int inputWidth = 50;
    const static float step = 0.1f;
    
    static Vector3 currentSize;
    static float currentRadius = 0;
    if (ImGui::TreeNode(ComponentRegister::sGetClassName(this).c_str()))
    {
        if (mShape != nullptr)
        {
            switch (mShape->getType())
            {
                case ShapeType::Box:
                    {
                        auto boxShape = dynamic_cast<BoxShape*>(mShape);
                        currentSize = boxShape->getSize();
                        //更改大小
                        ImGui::SetNextItemWidth(inputWidth);
                        ImGui::InputFloat("X", &(currentSize.v.x));
                        if (ImGui::IsItemHovered())
                        {
                            float wheel = ImGui::GetIO().MouseWheel;
                            if (wheel != 0.0f)
                            {
                                currentSize.v.x += wheel * step;
                            }
                            boxShape->setSize(currentSize);
                        }
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(inputWidth);
                        ImGui::InputFloat("Y", &(currentSize.v.y));
                        if (ImGui::IsItemHovered())
                        {
                            float wheel = ImGui::GetIO().MouseWheel;
                            if (wheel != 0.0f)
                            {
                                currentSize.v.y += wheel * step;
                            }
                            boxShape->setSize(currentSize);
                        }
                        ImGui::SameLine();
                        ImGui::SetNextItemWidth(inputWidth);
                        ImGui::InputFloat("Z", &(currentSize.v.z));
                        if (ImGui::IsItemHovered())
                        {
                            float wheel = ImGui::GetIO().MouseWheel;
                            if (wheel != 0.0f)
                            {
                                currentSize.v.z += wheel * step;
                            }
                            boxShape->setSize(currentSize);
                        }
                        ImGui::SameLine();
                        ImGui::Text("Box Size");

                        if (ImGui::Button("Delete BoxBoundingBox"))
                        {
                            delete mShape;
                            mShape = nullptr;
                        }
                        break;
                    }
                default:
                    ASSERT(false, TEXT("Unknown Box shape!"));
            }
        }
        else
        {
            if (ImGui::Button("Add BoxBoundingBox"))
            {
                mShape = new BoxShape(Vector3(1, 1, 1));
            }
        }
        ImGui::TreePop();
    }
    
#endif
}

