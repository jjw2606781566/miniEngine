#pragma once
#include "../Component.h"
#include "Engine/Memory/TankinMemory.h"
#include "Engine/render/Color.h"
#include "Engine/render/MeshData.h"
#include "Engine/render/RenderItem.h"
#include "Engine/Utility/MacroUtility.h"

class MeshRenderer:public Component
{
    friend class ComponentFactory;
public:
    virtual void awake() override;

    void setShader(const TpString& shaderName);
    void setTexture(const TpString& textureName);
    
    void prepareRenderList() const;
    ~MeshRenderer() override {}
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    void showSelf() override;
    void setColor(const Color& color){mColor = color;}
    void setBlendFactor(const float blendFactor){mBlendFactor = blendFactor;}

private:
    std::unique_ptr<MaterialInstance> mMaterialGpu;
    TpString mShaderName = "debug";
    TpString mTextureName;

    Color mColor;
    float mAlpha = 1;
    float mBlendFactor = 0;
};