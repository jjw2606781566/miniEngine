#pragma once
#include "Engine/pch.h"
#include "RHIDefination.h"
#include "Shader.h"

template <typename TRHIObject, typename = std::enable_if_t<std::is_base_of<RHIObject, TRHIObject>::value>>
class RHIRef
{
	friend class Renderer;
public:
	RHIRef() : mIndex(MAXUINT64), mObject(nullptr) { }
	RHIRef(uint64_t index, TRHIObject* object)
		: mIndex(index),
		mObject(object)
	{
	}

	TRHIObject* operator->() const
	{
		return mObject;
	}
	
	TRHIObject* Get() const
	{
		return mObject;
	}

	bool IsValid() const
	{
		return mObject && mIndex != MAXUINT64;
	}

	bool operator==(const RHIRef& other) const noexcept
	{
		return mIndex == other.mIndex && mObject == other.mObject;
	}
private:
	uint64_t mIndex;
	TRHIObject* mObject;
};

using ConstantBufferRef = RHIRef<RHIConstantBuffer>;
using TextureRef = RHIRef<RHINativeTexture>;
using BufferRef = RHIRef<RHINativeBuffer>;
using RenderTargetRef = RHIRef<RHIRenderTarget>;
using DepthStencilRef = RHIRef<RHIDepthStencil>;
using VertexBufferRef = RHIRef<RHIVertexBuffer>;
using IndexBufferRef = RHIRef<RHIIndexBuffer>;
using ShaderRef = RHIRef<RHIShader>;