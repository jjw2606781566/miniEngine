#pragma once
#include "Engine/pch.h"
#include "PC/D3D12RHI.h"

#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui.h"
#include "Engine/Render/Renderer.h"
#include "Engine/Render/PC/Native/D3D12EnumConversions.h"
#include "Engine/Render/PC/Private/D3D12GraphicsContext.h"

inline void ImGuiInitialize(D3D12RHI* pRenderHardwareInterface, uint32_t numSRVDesctiptors = 4)
{
    Renderer& renderer = Renderer::GetInstance();
    
    // Setup Platform/Renderer backends
    const RendererConfiguration& config = renderer.getConfiguration();
    BlockDescriptorAllocator* pDescriptorAllocator = new BlockDescriptorAllocator();
    pDescriptorAllocator->Initialize(pRenderHardwareInterface->GetD3D12Device(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numSRVDesctiptors, true);
    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = pRenderHardwareInterface->GetD3D12Device()->GetD3D12Device();
    init_info.CommandQueue = pRenderHardwareInterface->GetCommandQueue();
    init_info.NumFramesInFlight = config.mNumBackBuffers;
    init_info.RTVFormat = ::ConvertToDXGIFormat(config.mBackBufferFormat); // Or your render target format.
    init_info.UserData = pDescriptorAllocator;
	init_info.SrvDescriptorHeap = pDescriptorAllocator->GetHeap()->GetD3D12DescriptorHeap();
    init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* init_info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
        { 
		    D3D12DescriptorHandle handle = static_cast<BlockDescriptorAllocator*>(init_info->UserData)->Allocate();
		    *out_cpu_handle = handle.mCPUHandle;
		    *out_gpu_handle = handle.mGPUHandle;
        };
    init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo* init_info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
        {
            bool succeeded = false;
            static_cast<BlockDescriptorAllocator*>(init_info->UserData)->Free(cpu_handle, succeeded);
            if (!succeeded) WARN("Failed to free cpu descriptor handle!");
        };
    ImGui_ImplDX12_Init(&init_info);
}

inline void ImGuiNewFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

// TODO: 
inline void ImGuiRender(/*AHCommandList& commandList*/)
{
    D3D12GraphicsContext* pGConstext = nullptr; // static_cast<D3D12GraphicsContext*>(commandList.GetRHIGraphicsContext());
    ID3D12GraphicsCommandList* pCommandList = pGConstext->GetD3D12CommandList();
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
}

inline void ImGuiShutdown()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
#endif