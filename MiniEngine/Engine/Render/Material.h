#pragma once
#include "RHIDescriptors.h"
#include "RenderResource.h"
#include "Shader.h"


struct ConstantProperty
{
    ConstantProperty() = default;
    ConstantProperty(const String& name, uint8_t registerSlot, uint64_t size) : mName(name),
        mRegisterSlot(registerSlot), mConstantSize(size)
    {
    }

	String mName;
    uint8_t mRegisterSlot;
    uint64_t mConstantSize;
};

struct TextureProperty
{
    TextureProperty() = default;
    TextureProperty(const String& name, uint64_t registerSlot, TextureDimension dimension) : mName(name),
        mRegisterSlot(registerSlot), mDimension(dimension)
    {
    }

    String mName;
    uint64_t mRegisterSlot;
    TextureDimension mDimension;
};

struct SamplerProperty  // TODO: complete SamplerProperty
{
	
};

// TODO: should be serializable
struct Material
{
    RHIShader* mShader = nullptr;
    std::unique_ptr<ConstantProperty[]> mConstants = nullptr;
    std::unique_ptr<TextureProperty[]> mTextures = nullptr;
    std::unique_ptr<SamplerProperty[]> mSamplers = nullptr;
    uint8_t mNumConstants = 0;
    uint8_t mNumTextures = 0;
    uint8_t mNumSamplers = 0;

    // bool mEnableAlphaClip = false;
    // CullMode mCullMode = CullMode::BACK;
    // DrawMode mDrawMode = DrawMode::SOLID;
    // DepthTestDesc mDepthTest;
    // StencilTestDesc mStencilTest;
    // BlendDesc mBlend;
};

class MaterialInstance
{
    friend class Renderer;
public:
    void InstantiateFrom(const Material* pMaterial);
    void SetMaterialInstanceId(uint64_t id);
    uint64_t GetMaterialInstanceId() const;
    RHIShader* GetShader() const;
    uint32_t GetCBufferIndex(const String& cbufferName) const;
    uint32_t GetTextureIndex(const String& name) const;
    //uint32_t GetSamplerIndex(const String& name) const;   // TODO: 

    void SetTexture(uint32_t index, TextureDimension dimension, TextureRef texture);
    void SetTexture(uint32_t index, TextureRef texture);
    TextureRef GetTexture(uint32_t index) const;
    const Blob& GetConstantBuffer(uint32_t index) const;
    void UpdateConstantBuffer(uint32_t index, void* pData, uint32_t size);
    uint8_t NumTextures() const;
    uint8_t NumConstantBuffers() const;

    bool AlphaClipEnabled() const;
    DrawMode GetDrawMode() const;
    CullMode GetCullMode() const;
    const DepthTestDesc& DepthTest() const;
    const StencilTestDesc& StencilTest() const;
    const BlendDesc& BlendMode() const;

    void SetEnableAlphaClip(bool m_enable_alpha_clip)
    {
        mEnableAlphaClip = m_enable_alpha_clip;
    }

    void SetDrawMode(DrawMode m_draw_mode)
    {
        mDrawMode = m_draw_mode;
    }

    void SetCullMode(CullMode m_cull_mode)
    {
        mCullMode = m_cull_mode;
    }

    void setDepthTest(const DepthTestDesc& m_depth_test)
    {
        mDepthTest = m_depth_test;
    }

    void setStencilTest(const StencilTestDesc& m_stencil_test)
    {
        mStencilTest = m_stencil_test;
    }

    void setBlend(const BlendDesc& m_blend)
    {
        mBlend = m_blend;
    }

    MaterialInstance() = default;
    
private:
    const Material* mMaterial = nullptr;
    uint64_t mInstanceId = 0;
    
    std::unique_ptr<Blob[]> mConstants{};
    std::unique_ptr<ConstantBufferRef[]> mConstantsGPU{};
    std::unique_ptr<TextureRef[]> mTexGPU{};   // since the CPU rarely changes texture content, we don't cache texture content.
    bool mIsDirty = true;
	// std::unique_ptr<RHISamplerRef[]> mSamplers;   // TODO: implement samplers

    bool mEnableAlphaClip = false;
    DrawMode mDrawMode = DrawMode::SOLID;
    CullMode mCullMode = CullMode::BACK;
    DepthTestDesc mDepthTest = DepthTestDesc::Default();
    StencilTestDesc mStencilTest = StencilTestDesc::Default();
    BlendDesc mBlend = BlendDesc::Color();
};

inline void MaterialInstance::InstantiateFrom(const Material* pMaterial)
{
    mMaterial = pMaterial;
    mConstants.reset(new Blob[pMaterial->mNumConstants]);
    mConstantsGPU.reset(new ConstantBufferRef[pMaterial->mNumConstants]);
    mTexGPU.reset(new TextureRef[pMaterial->mNumTextures]);
    for (size_t i = 0; i < pMaterial->mNumConstants; ++i)
    {
        mConstants[i].Reserve(pMaterial->mConstants[i].mConstantSize);
    }
    mIsDirty = true;

    mEnableAlphaClip = false; // pMaterial->mEnableAlphaClip;
    mCullMode = CullMode::BACK; // pMaterial->mCullMode;
    mDrawMode = DrawMode::SOLID; //pMaterial->mDrawMode;
    mDepthTest = DepthTestDesc::Default(); // pMaterial->mDepthTest;
    mStencilTest = StencilTestDesc::Default(); // pMaterial->mStencilTest;
    mBlend = BlendDesc::Disabled(); // pMaterial->mBlend;
}

inline void MaterialInstance::SetMaterialInstanceId(uint64_t id) { mInstanceId = id; }

inline uint64_t MaterialInstance::GetMaterialInstanceId() const { return mInstanceId; }

inline RHIShader* MaterialInstance::GetShader() const { return mMaterial->mShader; }

inline uint32_t MaterialInstance::GetCBufferIndex(const String& cbufferName) const
{
	for (uint32_t i = 0; i < mMaterial->mNumConstants; ++i)
	{
		if (mMaterial->mConstants[i].mName == cbufferName)
		{
			return i;
		}
	}
    return MAXUINT32;
}

inline uint32_t MaterialInstance::GetTextureIndex(const String& name) const
{
	for (uint32_t i = 0; i < mMaterial->mNumTextures; ++i)
	{
		if (mMaterial->mTextures[i].mName == name)
		{
			return i;
		}
	}
	return MAXUINT32;
}

inline void MaterialInstance::SetTexture(uint32_t index, TextureDimension dimension, TextureRef texture)
{
    if (index >= mMaterial->mNumTextures)
    {
        WARN("index of material texture out of bound!");
        return;
    }
    if (dimension != mMaterial->mTextures[index].mDimension)
    {
        WARN("dimension of material texture mismatch!");
        return;
    }
    mTexGPU[index] = texture;
}

inline void MaterialInstance::SetTexture(uint32_t index, TextureRef texture)
{
    if (index >= mMaterial->mNumTextures)
    {
        WARN("index of material texture out of bound!");
        return;
    }
    mTexGPU[index] = texture;
}

inline TextureRef MaterialInstance::GetTexture(uint32_t index) const
{
    if (index >= mMaterial->mNumTextures)
    {
        WARN("index of material texture out of bound!");
        return {};
    }
    return mTexGPU[index];
}

inline const Blob& MaterialInstance::GetConstantBuffer(uint32_t index) const
{
    if (index >= mMaterial->mNumConstants)
    {
        WARN("index of material constants out of bound!");
    }
    return mConstants[index];
}

inline void MaterialInstance::UpdateConstantBuffer(uint32_t index, void* pData, uint32_t size)
{
    //  MASSERT(index < mMaterial->mNumConstants, TEXT("index out of range."));
    Blob& blob = mConstants[index];
    blob.CopyFrom(pData, size);
    mIsDirty = true;
}

inline uint8_t MaterialInstance::NumTextures() const
{
    return mMaterial->mNumTextures;
}

inline uint8_t MaterialInstance::NumConstantBuffers() const
{
    return mMaterial->mNumConstants;
}

inline bool MaterialInstance::AlphaClipEnabled() const
{
    return mEnableAlphaClip;
}

inline DrawMode MaterialInstance::GetDrawMode() const
{
    return mDrawMode;
}

inline CullMode MaterialInstance::GetCullMode() const
{
    return mCullMode;
}

inline const DepthTestDesc& MaterialInstance::DepthTest() const
{
    return mDepthTest;
}

inline const StencilTestDesc& MaterialInstance::StencilTest() const
{
    return mStencilTest;
}

inline const BlendDesc& MaterialInstance::BlendMode() const
{
    return mBlend;
}