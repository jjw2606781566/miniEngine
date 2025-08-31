#pragma once
#include "Engine/Component/Component.h"
#include "Engine/render/Color.h"
#include "Engine/render/MeshData.h"
#include "Engine/render/RenderItem.h"

class MaterialInstance;

class ImageTGUI:public Component
{
public:
    void awake() override;
    void setMesh(const TpString& meshName){mMeshName = meshName;}
    void setShader(const TpString& shaderName);
    void setTexture(const TpString& textureName);
    
    void setColor(Color color){mColor = color;}
    void setAlpha(float alpha){mAlpha = alpha;}
    void setBlendFactor(float blendFactor){mBlendFactor = blendFactor;}
    Color getColor() const{ return mColor;}
    float getAlpha() const{ return mAlpha;}
    float getBlendFactor() const{ return mBlendFactor;}

    virtual void prepareRenderList();
    ~ImageTGUI() override;
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
                                    const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    void showSelf() override;

protected:
    std::unique_ptr<MaterialInstance> mMaterialGpu = nullptr;
    TpString mMeshName;
    TpString mShaderName;
    TpString mTextureName;

    Color mColor;
    float mAlpha = 1;
    float mBlendFactor = 0;
};
