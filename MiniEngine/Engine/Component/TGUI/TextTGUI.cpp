#include "TextTGUI.h"

#include <codecvt>
#include <locale>

#include "RectTransform.h"
#include "Engine/Component/Layer.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/RenderComponent/Camera.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/Utility/Font/TankinFont.h"

REGISTER_COMPONENT(TextTGUI, "TextTGUI")

void TextTGUI::awake()
{
    mMeshName = "Image";

    mGameObject->setLayer(Layer::LAYER_UI);
}

void TextTGUI::prepareRenderList()
{
    Vector2 fontOffset = Vector2(0, 0);
    for (wchar_t ch : mText)
    {
        static wchar_t lastCh = L'烫';
        if (ch == '\r')
            continue;
        if (ch == '\n')
        {
            fontOffset.v.x = 0;
            fontOffset.v.y += TankinFont::sGetInstance()->getLineHeight() * mFontScale.v.y;
            continue;
        }
        //Mesh
        RenderItem renderItem;
        renderItem.mMeshData = FileManager::sGetLoadedBolbFile<MeshData>(mMeshName);
    
        //Matrix, only position influence collider
        TankinFont::FontChar* fontChar = TankinFont::sGetInstance()->getFontChar(ch);
        RectTransform* transform = dynamic_cast<RectTransform*>(getGameObject()->getComponent("Transform"));
        float kerningOffset = TankinFont::sGetInstance()->getKerningAmount(lastCh, ch);
        Vector3 position = transform->getWorldPosition(
            {(fontOffset.v.x + fontChar->xoffset + kerningOffset) * mFontScale.v.x, (fontOffset.v.y + fontChar->yoffset) * mFontScale.v.y});
        Quaternion rotation = transform->getWorldRotation();
        Vector3 scale = {static_cast<float>(fontChar->xadvance), TankinFont::sGetInstance()->getLineHeight(),1};
        scale.v.x *= mFontScale.v.x;
        scale.v.y *= mFontScale.v.y;

        lastCh = ch;
        
        renderItem.mModel.setModelMatrixQuaternion(position, rotation, scale);
        //move to next ch
        fontOffset.v.x += (fontChar->xadvance + fontChar->xoffset + kerningOffset + 2);

        //Material
        renderItem.mMaterial = fontChar->mMaterialGpu.get();
        
        struct alignas(256) UniversalCBuffer
        {
            float mColor[4];
            float mBlendFactor[4];
            float mUv[4];
        };
        UniversalCBuffer universalCBuffer;
        universalCBuffer.mColor[0] = mColor.value.v.x;
        universalCBuffer.mColor[1] = mColor.value.v.y;
        universalCBuffer.mColor[2] = mColor.value.v.z;
        universalCBuffer.mColor[3] = mAlpha;
        universalCBuffer.mBlendFactor[0] = mBlendFactor;

        //sampler uv
        //uv: u start
        //magic number: 2.5, the offset of texture sampler
        universalCBuffer.mUv[0] = (fontChar->x) /
            static_cast<double>(TankinFont::sGetInstance()->getTextureWidth());
        //uv: u end
        universalCBuffer.mUv[1] = static_cast<double>((fontChar->x+fontChar->width)) /
            static_cast<double>(TankinFont::sGetInstance()->getTextureWidth());

        fontChar->mMaterialGpu->UpdateConstantBuffer(0, &universalCBuffer, sizeof(universalCBuffer));

        Camera* currentCamera = Camera::sGetCurrentCamera();
        currentCamera->addRenderItem(renderItem);
    }
}

void TextTGUI::setText(const TpString& newText)
{
    if (sizeof(wchar_t) == 2)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        mText = converter.from_bytes(newText);
    }
    else if (sizeof(wchar_t) == 4)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        mText = converter.from_bytes(newText);
    }
    else
    {
        ASSERT(false, TEXT("Unknown wchar_t size!"))
    }
    
    
}

TpString TextTGUI::getText() const
{
    TpString result;
    if (sizeof(wchar_t) == 2)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        result = converter.to_bytes(mText);
    }
    else if (sizeof(wchar_t) == 4)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        result = converter.to_bytes(mText);
    }
    else
    {
        ASSERT(false, TEXT("Unknown wchar_t size!"))
    }
    return result;
}

void TextTGUI::showSelf()
{
#ifdef WIN32
    const static int inputWidth = 50;
    const static float step = 0.01f;
    const static int MAX_TEXT_LENGTH = 200;
    
    if (ImGui::TreeNode(ComponentRegister::sGetClassName(this).c_str()))
    {
        static char* currentText = new char[MAX_TEXT_LENGTH];
        strcpy_s(currentText, MAX_TEXT_LENGTH, getText().c_str());
        if (ImGui::InputTextMultiline("Text", currentText, MAX_TEXT_LENGTH))
        {
            setText(currentText);
        }
        
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

        //font size
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("x##xx", &(mFontScale.v.x));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mFontScale.v.x += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("y##xx", &(mFontScale.v.y));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mFontScale.v.y += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::Text("FontSize");
        
        ImGui::TreePop();
    }
#endif

}

rapidxml::xml_node<>* TextTGUI::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    ImageTGUI::serialize(doc, father, value);
    mXmlNode->first_attribute("name")->value(doc->allocate_string("TextTGUI"));

    mXmlNode->append_attribute(doc->allocate_attribute("Text", doc->allocate_string(getText().c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("FontScaleX", doc->allocate_string(std::to_string(mFontScale.v.x).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("FontScaleY", doc->allocate_string(std::to_string(mFontScale.v.y).c_str())));
    return  mXmlNode;
}

void TextTGUI::deSerialize(const rapidxml::xml_node<>* node)
{
    mMeshName = node->first_attribute("MeshName")->value();
    
    auto currentNode = node->first_node("Color");
    mColor.value.v.x = std::stof(currentNode->first_attribute("R")->value());
    mColor.value.v.y = std::stof(currentNode->first_attribute("G")->value());
    mColor.value.v.z = std::stof(currentNode->first_attribute("B")->value());
    mAlpha = std::stof(currentNode->first_attribute("Alpha")->value());
    mBlendFactor = std::stof(currentNode->first_attribute("BlendFactor")->value());
    
    setText(node->first_attribute("Text")->value());
    mFontScale.v.x = std::stof(node->first_attribute("FontScaleX")->value());
    mFontScale.v.y = std::stof(node->first_attribute("FontScaleY")->value());
}
