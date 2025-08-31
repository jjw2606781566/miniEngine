#include "ImageTGUI.h"

#include "RectTransform.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/RenderComponent/Camera.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/render/Renderer.h"

REGISTER_COMPONENT(ImageTGUI, "ImageTGUI")

void ImageTGUI::awake()
{
    mMeshName = "Image";
    mShaderName = "ui";
    mTextureName = "DefaultImage";
    setShader(mShaderName);
    setTexture(mTextureName);
    mGameObject->setLayer(Layer::LAYER_UI);
}

void ImageTGUI::setShader(const TpString& shaderName)
{
    mShaderName = shaderName;
    mMaterialGpu = Renderer::GetInstance().createMaterialInstance(*FileManager::sGetLoadedBolbFile<Material*>(mShaderName));
    DepthTestDesc depthTestDesc = DepthTestDesc::Default();
    depthTestDesc.mEnableDepthTest = false;
    BlendDesc blendDesc = BlendDesc::Color();
    blendDesc.mEnableBlend = true;
    mMaterialGpu->setDepthTest(depthTestDesc);
    mMaterialGpu->setBlend(blendDesc);
    mMaterialGpu->setDepthTest(depthTestDesc);
}

void ImageTGUI::setTexture(const TpString& textureName)
{
    mMaterialGpu->SetTexture(0, FileManager::sGetLoadedBolbFile<TextureRef>(textureName));

    mTextureName = textureName;
}

void ImageTGUI::prepareRenderList()
{
    DEBUG_PRINT("Render %s Image\n", getGameObject()->getName().c_str());
    
    //Mesh
    RenderItem renderItem;
    renderItem.mMeshData = FileManager::sGetLoadedBolbFile<MeshData>(mMeshName);
    
    //Matrix, only position influence collider
    RectTransform* transform = dynamic_cast<RectTransform*>(getGameObject()->getComponent("Transform"));
    renderItem.mModel = transform->getModelMatrix();

    //Material
    renderItem.mMaterial = mMaterialGpu.get();

    // const Blob& cbuffer = mMaterialGpu->GetConstantBuffer(0);
    struct alignas(256) UniversalCBuffer
    {
        float mColor[4];
        float mBlendFactor[4];
    };
    UniversalCBuffer universalCBuffer;
    universalCBuffer.mColor[0] = mColor.value.v.x;
    universalCBuffer.mColor[1] = mColor.value.v.y;
    universalCBuffer.mColor[2] = mColor.value.v.z;
    universalCBuffer.mColor[3] = mAlpha;
    universalCBuffer.mBlendFactor[0] = mBlendFactor;
    mMaterialGpu->UpdateConstantBuffer(0, &universalCBuffer, sizeof(universalCBuffer));
    // cbuffer.CopyFrom(&universalCBuffer, sizeof(universalCBuffer));

    Camera* currentCamera = Camera::sGetCurrentCamera();
    currentCamera->addRenderItem(renderItem);
}

ImageTGUI::~ImageTGUI()
{
}

rapidxml::xml_node<>* ImageTGUI::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
                                           const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("ImageTGUI")));
    mXmlNode->append_attribute(doc->allocate_attribute("MeshName", doc->allocate_string(mMeshName.c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("ShaderName", doc->allocate_string(mShaderName.c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("TextureName", doc->allocate_string(mTextureName.c_str())));

    auto colorNode = doc->allocate_node(rapidxml::node_element, "Color");
    mXmlNode->append_node(colorNode);
    colorNode->append_attribute(doc->allocate_attribute("R", doc->allocate_string(std::to_string(mColor.value.v.x).c_str())));
    colorNode->append_attribute(doc->allocate_attribute("G", doc->allocate_string(std::to_string(mColor.value.v.y).c_str())));
    colorNode->append_attribute(doc->allocate_attribute("B", doc->allocate_string(std::to_string(mColor.value.v.z).c_str())));
    colorNode->append_attribute(doc->allocate_attribute("Alpha", doc->allocate_string(std::to_string(mAlpha).c_str())));
    colorNode->append_attribute(doc->allocate_attribute("BlendFactor", doc->allocate_string(std::to_string(mBlendFactor).c_str())));

    return mXmlNode;
}

void ImageTGUI::deSerialize(const rapidxml::xml_node<>* node)
{
    mMeshName = node->first_attribute("MeshName")->value();
    setShader(node->first_attribute("ShaderName")->value());
    setTexture(node->first_attribute("TextureName")->value());

    auto currentNode = node->first_node("Color");
    mColor.value.v.x = std::stof(currentNode->first_attribute("R")->value());
    mColor.value.v.y = std::stof(currentNode->first_attribute("G")->value());
    mColor.value.v.z = std::stof(currentNode->first_attribute("B")->value());
    mAlpha = std::stof(currentNode->first_attribute("Alpha")->value());
    mBlendFactor = std::stof(currentNode->first_attribute("BlendFactor")->value());
}

void ImageTGUI::showSelf()
{
#ifdef WIN32
    const static int inputWidth = 50;
    const static float step = 0.01f;
    
    if (ImGui::TreeNode(ComponentRegister::sGetClassName(this).c_str()))
    {
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("R##xx", &(mColor.value.v.x));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mColor.value.v.x += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("G##XX", &(mColor.value.v.y));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mColor.value.v.y += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("B##xx", &(mColor.value.v.z));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mColor.value.v.z += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::Text("Color");

        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Alpha", &(mAlpha));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mAlpha += wheel * step;
            }
        }

        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("BlenderFactor", &(mBlendFactor));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mBlendFactor += wheel * step;
            }
        }
        
        ImGui::TreePop();
    }
#endif
}
