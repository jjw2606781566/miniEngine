#include "MeshRenderer.h"
#include "MeshFilter.h"
#include "Camera.h"

#include "../GameObject.h"
#include "../Transform.h"

#include "Engine/Utility/MacroUtility.h"
#include "Engine/Application.h"
#include "Engine/common/Exception.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/render/Renderer.h"
#include "Engine/render/DataCPU/RenderData.h"

REGISTER_COMPONENT(MeshRenderer, "MeshRenderer")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::awake()
{
    // mMaterialGpu = nullptr;
    // mMaterialGpu->shader = FileManager::sGetLoadedBolbFile<ShaderData>("debug").shader;
    mMaterialGpu = Renderer::GetInstance().createMaterialInstance(*FileManager::sGetLoadedBolbFile<Material*>(mShaderName));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshRenderer::setShader(const TpString& shaderName)
{
    // mMaterialGpu->setBlend(). = FileManager::sGetLoadedBolbFile<ShaderData>(shaderName).shader; // TODO: get material instance by shader name
    mMaterialGpu = Renderer::GetInstance().createMaterialInstance(*FileManager::sGetLoadedBolbFile<Material*>(shaderName));
    mShaderName = shaderName;
}

void MeshRenderer::setTexture(const TpString& textureName)
{
    mMaterialGpu->SetTexture(0, FileManager::sGetLoadedBolbFile<TextureRef>(textureName));

    mTextureName = textureName;
}

void MeshRenderer::prepareRenderList() const
{
    DEBUG_PRINT("Render %s\n", getGameObject()->getName().c_str());
    MeshFilter* filter = dynamic_cast<MeshFilter*>(mGameObject->getComponent("MeshFilter"));
    ASSERT(filter, TEXT("this object do not have MeshFilter Component!"));

    //mesh
    MeshData meshData = filter->getMeshData();
    
    RenderItem renderItem;
    renderItem.mMeshData = meshData;

    //Matrix
    Transform* transform = dynamic_cast<Transform*>(mGameObject->getComponent("Transform"));
    ASSERT(transform, TEXT("transform is null!"))
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
    // renderItem.mColor[0] = mColor.value.v.x;
    // renderItem.mColor[1] = mColor.value.v.y;
    // renderItem.mColor[2] = mColor.value.v.z;
    // renderItem.mColor[3] = mAlpha;
    // renderItem.mBlendFactor = mBlendFactor;

    Camera* currentCamera = Camera::sGetCurrentCamera();
    currentCamera->addRenderItem(renderItem);
}

rapidxml::xml_node<>* MeshRenderer::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("MeshRenderer")));
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

void MeshRenderer::deSerialize(const rapidxml::xml_node<>* node)
{
    setShader(node->first_attribute("ShaderName")->value());
    if (strcmp(node->first_attribute("TextureName")->value(), "") != 0)
    {
        setTexture(node->first_attribute("TextureName")->value());
    }

    auto currentNode = node->first_node("Color");
    mColor.value.v.x = std::stof(currentNode->first_attribute("R")->value());
    mColor.value.v.y = std::stof(currentNode->first_attribute("G")->value());
    mColor.value.v.z = std::stof(currentNode->first_attribute("B")->value());
    mAlpha = std::stof(currentNode->first_attribute("Alpha")->value());
    mBlendFactor = std::stof(currentNode->first_attribute("BlendFactor")->value());
}

void MeshRenderer::showSelf()
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

