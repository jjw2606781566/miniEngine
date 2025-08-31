#pragma once

#include "../Component.h"
#include "../Layer.h"

#include "Engine/Memory/TankinMemory.h"
#include "Engine/Utility/MacroUtility.h"
#include "Engine/render/Color.h"
#include "Engine/Render/RenderItem.h"

#include "Engine/render/DataCPU/RenderData.h"


class Transform;
enum class CameraType:bool
{
    Projective,
    Orthographic
};

class Camera final:public Component
{
public:
    friend class ComponentFactory;
    friend class ComponentRegister;
    
    //render
    static void sRenderScene();
    static Camera* sGetCurrentCamera(){return sCurrentCamera;}

    void start() override;
    void onEnable() override;
    void update() override;

    void setProjectiveMatrix(const float FOV, const float aspect, const float nearClip, const float farClip);
    void setOrthographicMatrix(const float width, const float height, const float nearClip, const float farClip);

    void clearRenderList();
    void addRenderItem(const RenderItem& item);

    //depth
    void setDepth(const unsigned char depth);
    unsigned char getDepth() const {return mDepth;}

    //layer
    void setRenderLayer(const Layer layer) {renderLayer = static_cast<unsigned char>(layer);}
    Layer getRenderLayer() const {return static_cast<Layer>(renderLayer);}

    //transform 3d pos and 2d pos
    Vector2 transformWorldToScreen(const Vector3& worldPos) const;

    static Camera* sGetMainCamera() {return sMainCamera;}
    static void sSetMainCamera(Camera* camera) {sMainCamera = camera; camera->isMainCamera = true;}

    //render collider
    void setRenderCollider(bool renderCollider) {mRenderCollider = renderCollider;}
    bool getRenderCollider() const {return mRenderCollider;}

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;

    class CameraCompare
    {
    public:
        bool operator() (const Camera* left, const Camera* right) const
        {
            if (left->getDepth() != right->getDepth())
                return left->getDepth() < right->getDepth();
            else //depth equal
            {
                return left<right;
            }
        }
    };
    
private:
    Camera();
    ~Camera() override;
    
    void uploadMatrix();
    
    //projection
    Matrix4x4 mProjectiveMatrix;
    bool mNeedReUploadMatrix = true;
    CameraType mType = CameraType::Projective;

    //Camera Parameter
    struct CameraParameter:ISerializable
    {
        rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
            const TpString& value) override;
        void deSerialize(const rapidxml::xml_node<>* node) override;
        float mFov;
        float mAspect;
        float mWidth;
        float mHeight;
        float mNearClip;
        float mFarClip;
    }mParameter;

    //depth
    unsigned char mDepth;

    //render item list which need to be rendered each frame
    RenderList mRenderList;

    //whether render collider
    bool mRenderCollider = false;

    //is main camera? use for deserialization
    bool isMainCamera = false;

    //renderLayer
    unsigned char renderLayer = static_cast<unsigned char>(Layer::LAYER_All);
    
    //all Camera in this scene
    static Camera* sCurrentCamera;
    static Camera* sMainCamera;
    static TpMultiSet<Camera*, CameraCompare> sCameras;
};
