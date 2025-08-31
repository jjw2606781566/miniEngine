#pragma once
#include "RHIDescriptors.h"

struct RendererConfiguration
{
    static RendererConfiguration Default()
    {
        return { 3, Format::R8G8B8A8_UNORM, Format::D24_UNORM_S8_UINT };
    }
    uint8_t mNumBackBuffers;
    Format mBackBufferFormat;
    Format mDepthStencilFormat;


};
#ifdef WIN32
#include "PC/Native/D3D12RootSignatureManager.h"


struct RHIConfiguration
{
    static RHIConfiguration Default()
    {
        return {16, 16, {4, 2}};
    }
    uint16_t mMaxNumRenderTarget;
    uint16_t mMaxNumDepthStencil;
    RootSignatureLayout mRootSignatureLayout;
};
#endif