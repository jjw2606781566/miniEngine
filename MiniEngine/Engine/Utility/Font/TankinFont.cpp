#include "TankinFont.h"

#include "Engine/Dependencies/rapidxml/rapidxml_utils.hpp"
#include "Engine/FileManager/FileManager.h"
#include "Engine/render/Renderer.h"

TankinFont* TankinFont::sInstance = nullptr;

void TankinFont::FontChar::deserialize(rapidxml::xml_node<>* node)
{
    x = std::stoi(node->first_attribute("x")->value());
    y = std::stoi(node->first_attribute("y")->value());
    width = std::stoi(node->first_attribute("width")->value());
    height = std::stoi(node->first_attribute("height")->value());
    xoffset = std::stoi(node->first_attribute("xoffset")->value());
    yoffset = std::stoi(node->first_attribute("yoffset")->value());
    xadvance = std::stoi(node->first_attribute("xadvance")->value());
    page = std::stoi(node->first_attribute("page")->value());
}

void TankinFont::DeserializeFontFromFile()
{
    rapidxml::file<>* xmlFile = new rapidxml::file<>("Assets/Fonts/GameplayFont/TankinFont.fnt");
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();

    doc->parse<0>(xmlFile->data());

    rapidxml::xml_node<>* rootnode = doc->first_node("font");

    auto commonNode = rootnode->first_node("common");
    lineHeight = std::stof(commonNode->first_attribute("lineHeight")->value());
    textureWidth = std::stof(commonNode->first_attribute("scaleW")->value());
    textureHeight = std::stof(commonNode->first_attribute("scaleH")->value());
    
    auto pagesNode = rootnode->first_node("pages");
    auto pageChildNode = pagesNode->first_node("page");
    while (pageChildNode != nullptr)
    {
        int id = std::stoi(pageChildNode->first_attribute("id")->value());
        TpString fileName = pageChildNode->first_attribute("file")->value();
        FontPage fontPage;
        fontPage.textureName = fileName;
        /*mMaterialGpu = Renderer::GetInstance().createMaterialInstance(*FileManager::sGetLoadedBolbFile<Material*>("font"));
        fontPage.mMaterial = new Material();
        fontPage.mMaterial->shader = FileManager::sGetLoadedBolbFile<ShaderData>("font").shader;
        fontPage.mMaterial->mTextures.push_back(FileManager::sGetLoadedBolbFile<TextureData>(fileName));*/
        mPages[id] = fontPage;
        pageChildNode = pageChildNode->next_sibling("page");
    }

    auto charsNode = rootnode->first_node("chars");
    auto charsChildNode = charsNode->first_node("char");
    while (charsChildNode != nullptr)
    {
        int id = std::stoi(charsChildNode->first_attribute("id")->value());
        wchar_t charId = static_cast<wchar_t>(id);
        FontChar fontChar;
        fontChar.deserialize(charsChildNode);

        //设置material
        fontChar.mMaterialGpu = Renderer::GetInstance().createMaterialInstance(*FileManager::sGetLoadedBolbFile<Material*>("font"));
        DepthTestDesc depthTest = DepthTestDesc::Default();
        depthTest.mEnableDepthTest = false;
        fontChar.mMaterialGpu->setDepthTest(depthTest);
        fontChar.mMaterialGpu->SetTexture(0, FileManager::sGetLoadedBolbFile<TextureRef>(mPages[fontChar.page].textureName));
        
        mChars[charId] = std::move(fontChar);

        charsChildNode = charsChildNode->next_sibling("char");
    }

    auto kerningsNode = rootnode->first_node("kernings");
    auto kerningsChildNode = kerningsNode->first_node("kerning");
    while (kerningsChildNode != nullptr)
    {
        int first = std::stoi(kerningsChildNode->first_attribute("first")->value());
        int second = std::stoi(kerningsChildNode->first_attribute("second")->value());
        int amount = std::stoi(kerningsChildNode->first_attribute("amount")->value());
        mKernings[static_cast<wchar_t>(first)][static_cast<wchar_t>(second)] = amount;
        kerningsChildNode = kerningsChildNode->next_sibling("kerning");
    }
}
