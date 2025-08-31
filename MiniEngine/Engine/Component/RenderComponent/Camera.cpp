#include "Camera.h"
#include "MeshRenderer.h"
#include "../GameObject.h"
#include "../Transform.h"
#include "Engine/Application.h"

#include "Engine/common/Exception.h"
#include "Engine/Component/Particle/ParticleSystem.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Component/TGUI/ImageTGUI.h"
#include "Engine/Utility/MacroUtility.h"
#include "Engine/render/Renderer.h"
#include "Engine/Window/Frame.h"

static ComponentRegister::Register<Camera> TankControllerRegister("Camera");

Camera* Camera::sCurrentCamera = nullptr;
Camera* Camera::sMainCamera = nullptr;
TpMultiSet<Camera*, Camera::CameraCompare> Camera::sCameras;

Vector2 Camera::transformWorldToScreen(const Vector3& worldPos) const
{
    Matrix4x4 viewMatrix;
    Transform* transform = mGameObject->getTransform();
    viewMatrix.setView(transform->getWorldPosition(), transform->getForward(), transform->getUp(), transform->getRight());
    viewMatrix = viewMatrix.transpose();
    Vector3 result=worldPos;
    Matrix4x4 transPro = mProjectiveMatrix.transpose();
    result.TransformSelfToScreen(viewMatrix * transPro);
    //DEBUG_FOCUS_PRINT("result: %f, %f\n", result.v.x, result.v.y);
    result.v.x = (result.v.x) * 0.5f * Application::sGetFrame()->Width();
    result.v.y = (-result.v.y) * 0.5f * Application::sGetFrame()->Height();
    return {result.v.x, result.v.y};
}

rapidxml::xml_node<>* Camera::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("Camera")));

    mXmlNode->append_attribute(doc->allocate_attribute("CameraType", doc->allocate_string(std::to_string(static_cast<int>(mType)).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("Depth", doc->allocate_string(std::to_string(static_cast<int>(mDepth)).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("isMainCamera", doc->allocate_string(std::to_string(isMainCamera).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("RenderLayer", doc->allocate_string(std::to_string(renderLayer).c_str())));

    mParameter.serialize(doc, mXmlNode, "");

    return mXmlNode;
}

void Camera::deSerialize(const rapidxml::xml_node<>* node)
{
    mType = static_cast<CameraType>(std::stoi(node->first_attribute("CameraType")->value()));
    
    isMainCamera = static_cast<bool>(std::stoi(node->first_attribute("isMainCamera")->value()));
    renderLayer = static_cast<unsigned char>(std::stoi(node->first_attribute("RenderLayer")->value()));

    setDepth(std::stoi(node->first_attribute("Depth")->value()));

    if (isMainCamera)
    {
        mRenderCollider = true;
    }
    else
    {
        mRenderCollider = false;
    }

    if (mGameObject->getName() == "MainCamera")
    {
        sMainCamera = this;
    }

    auto paraNode = node->first_node("CameraParameter");
    mParameter.deSerialize(paraNode);

    if (mType == CameraType::Projective)
    {
        setProjectiveMatrix(mParameter.mFov,mParameter.mAspect, mParameter.mNearClip, mParameter.mFarClip);
    }
    else
    {
        setOrthographicMatrix(mParameter.mWidth,mParameter.mHeight, mParameter.mNearClip, mParameter.mFarClip);
    }
    
    mNeedReUploadMatrix = true;
}

rapidxml::xml_node<>* Camera::CameraParameter::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element,"CameraParameter");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("FOV", doc->allocate_string(std::to_string(mFov).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("Aspect", doc->allocate_string(std::to_string(mAspect).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("Width", doc->allocate_string(std::to_string(mWidth).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("Height", doc->allocate_string(std::to_string(mHeight).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("NearClip", doc->allocate_string(std::to_string(mNearClip).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("FarClip", doc->allocate_string(std::to_string(mFarClip).c_str())));

    return mXmlNode;
}

void Camera::CameraParameter::deSerialize(const rapidxml::xml_node<>* node)
{
    mFov = std::stof(node->first_attribute("FOV")->value());
    mAspect = std::stof(node->first_attribute("Aspect")->value());
    mWidth = std::stof(node->first_attribute("Width")->value());
    mHeight = std::stof(node->first_attribute("Height")->value());
    mNearClip = std::stof(node->first_attribute("NearClip")->value());
    mFarClip = std::stof(node->first_attribute("FarClip")->value());
}

Camera::Camera():mProjectiveMatrix(),
                 mDepth(0)
{
    sCameras.insert(this);
    if (sMainCamera == nullptr)
        sMainCamera = this;
}

Camera::~Camera()
{
    sCameras.erase(this);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Camera::start()
{
    DEBUG_PRINT("<%s> Camera Component Start()\n", mGameObject->getName().c_str());
}

void Camera::update()
{
    DEBUG_PRINT("<%s> Camera Component Update()\n", mGameObject->getName().c_str());
}

void Camera::onEnable()
{
    DEBUG_PRINT("<%s> Camera Component OnEnable()\n", mGameObject->getName().c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Camera::uploadMatrix()
{
    if (mNeedReUploadMatrix)
    {
        //uploadMatrix to render system---------------------------------------------
        DEBUG_PRINT("%s UploadMatrix Successfully\n", getGameObject()->getName().c_str());
        mRenderList.mCameraConstants.mProjection = mProjectiveMatrix;
        mNeedReUploadMatrix = false;
        //uploadMatrix to render system---------------------------------------------
    }
    Matrix4x4 viewMatrix;
    Transform* transform = mGameObject->getTransform();
    ASSERT(transform, TEXT("camera transform is nullptr!"))
    viewMatrix.setView(transform->getWorldPosition(), transform->getForward(), transform->getUp(), transform->getRight());
    mRenderList.mCameraConstants.mView = viewMatrix;
}

void Camera::setProjectiveMatrix(const float FOV, const float aspect, const float nearClip, const float farClip)
{
    //according to camera parameter generate matrix
    mParameter.mFov = FOV;
    mParameter.mAspect = aspect;
    mParameter.mNearClip = nearClip;
    mParameter.mFarClip = farClip;
    mType = CameraType::Projective;
    this->mProjectiveMatrix.setPerspectiveProjection(FOV, aspect, nearClip, farClip);
    mNeedReUploadMatrix = true;
}

void Camera::setOrthographicMatrix(const float width, const float height, const float nearClip, const float farClip)
{
    mParameter.mWidth = width;
    mParameter.mHeight = height;
    mParameter.mNearClip = nearClip;
    mParameter.mFarClip = farClip;
    mType = CameraType::Orthographic;
    this->mProjectiveMatrix.setOrthographicProjection(static_cast<float>(width),
        static_cast<float>(height),nearClip, farClip);
    mNeedReUploadMatrix = true;
}


void Camera::clearRenderList()
{
    //DEBUG_PRINT("<%s> Camera Component clearRenderList()\n", mGameObject->getName().c_str());
    mRenderList.mOpaqueList.clear();
}

void Camera::addRenderItem(const RenderItem& item)
{
    //DEBUG_PRINT("<%s> Camera Component addRenderItem()\n", mGameObject->getName().c_str());
    mRenderList.mOpaqueList.push_back(item);
}

void Camera::setDepth(const unsigned char depth)
{
    sCameras.erase(this);
    this->mDepth = depth;
    sCameras.insert(this);
}

///render all gameObject which has meshRenderer component in this scene
void Camera::sRenderScene()
{
    std::function<void(const Transform* transform)> func =
        [](const Transform* transform)
        {
            GameObject* go = transform->getGameObject();
            ASSERT(go != nullptr, TEXT("GameObject pointer is nullptr"));
            
            if (!(go->isActive()))
            {
                return;
            }
            if (! ((Camera::sCurrentCamera->renderLayer) & (go->getLayer())) )
            {
                return;
            }

            //only debug camera can render rigid body
            if (Camera::sGetCurrentCamera()->getRenderCollider())
            {
                Component* rigidBodyComponent = go->getComponent("RigidBody");
                if (rigidBodyComponent != nullptr)
                {
                    RigidBody* rigidBody = dynamic_cast<RigidBody*>(rigidBodyComponent);
                    ASSERT(rigidBody != nullptr, TEXT("Dynamic Cast Error RigidBody pointer is nullptr"));
                    rigidBody->prepareRenderList();
                }
            }

            //render Image or Button
            {
                Component* component = go->getComponent("ImageTGUI");
                if (component != nullptr)
                {
                    ImageTGUI* image = dynamic_cast<ImageTGUI*>(component);
                    ASSERT(image != nullptr, TEXT("Dynamic Cast Error Image pointer is nullptr"));
                    image->prepareRenderList();
                }
                component = go->getComponent("Button");
                if (component != nullptr)
                {
                    ImageTGUI* image = dynamic_cast<ImageTGUI*>(component);
                    ASSERT(image != nullptr, TEXT("Dynamic Cast Error Image pointer is nullptr"));
                    image->prepareRenderList();
                }
                component = go->getComponent("TextTGUI");
                if (component != nullptr)
                {
                    ImageTGUI* image = dynamic_cast<ImageTGUI*>(component);
                    ASSERT(image != nullptr, TEXT("Dynamic Cast Error Image pointer is nullptr"));
                    image->prepareRenderList();
                }
            }

            //render particle
            {
                Component* component = go->getComponent("ParticleSystem");
                if (component != nullptr)
                {
                    ParticleSystem* particle = dynamic_cast<ParticleSystem*>(component);
                    ASSERT(particle != nullptr, TEXT("Dynamic Cast Error Particle pointer is nullptr"));
                    particle->prepareRenderList();
                }
            }

            //render 3d object
            {
                Component* component = go->getComponent("MeshRenderer");
                if (component == nullptr)
                {
                    return;
                }
                MeshRenderer* renderer = dynamic_cast<MeshRenderer*>(component);
                ASSERT(renderer != nullptr, TEXT("Dynamic Cast Error MeshRenderer pointer is nullptr"));
                renderer->prepareRenderList();
            }
        };
    
    Renderer& renderer = Renderer::GetInstance();
    
    for (auto& camera : sCameras)
    {
        if (!(camera->getGameObject()->isActive()))
            continue;

        //check running type
        if (camera->isMainCamera && Application::sGetRunningType() == EngineRunningType::Gameplay)
        {
            continue;
        }
        if (!(camera->isMainCamera) &&
            (Application::sGetRunningType() == EngineRunningType::Editor || Application::sGetRunningType() == EngineRunningType::Debug))
        {
            //debug and editor do not render other camera
            continue;
        }
        
        //set current camera
        sCurrentCamera = camera;
        
        //clear renderlist first
        sCurrentCamera->clearRenderList();
        
        //upload view and projection matrix
        camera->uploadMatrix();

        //iterate all go to render
        Transform* root = Transform::sGetRoot();
        root->foreachActiveLevelOrder(func);

        //Upload RenderList and Start Render---------------------------------------------------------------------------
        //sCurrentCamera->mRenderList.printSelf();
        auto temp = sCurrentCamera->mRenderList;
        std::unique_ptr<RenderList[]> renderLists;
        renderLists.reset(new RenderList[1]{sCurrentCamera->mRenderList});
        if (renderer.isRenderListEmpey())
        {
            renderLists[0].mClearRenderTarget = true;
        }
        renderLists[0].mBackGroundColor.v.x = 0.6902f;
        renderLists[0].mBackGroundColor.v.y = 0.7686f;
        renderLists[0].mBackGroundColor.v.z = 0.8706f;
        renderLists[0].mBackGroundColor.v.w = 1;
        renderer.appendRenderLists(std::move(renderLists), 1);
    }
    renderer.render();
}

