#include "EditorUI/Hierachy.h"
#include "ImguiManager.h"

#include "Engine/render/Renderer.h"
#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui_impl_dx12.h"
#include "Engine/Dependencies/imGui/imgui_impl_win32.h"
#include "Engine/Render/ImGuiInitialize.h"

ImguiManager* ImguiManager::sInstance = nullptr;


ImguiManager* ImguiManager::sGetInstance()
{
    if (sInstance == nullptr)
    {
        sInstance = new ImguiManager();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Assets/Fonts/NotoSansSC[wght].ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
        io.FontGlobalScale = 1.5f;
        (void)io;
        ImGui::StyleColorsDark();
    }

    return sInstance;
}

void ImguiManager::init()
{
    ::ImGuiInitialize(Renderer::GetInstance().getRHI<D3D12RHI>());
}

void ImguiManager::setWindowAndHeap(HWND hWindow, ID3D12Device* device, int num_frames_in_flight,
                                    DXGI_FORMAT rtv_format, ID3D12DescriptorHeap* srv_descriptor_heap,
                                    D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE font_srv_gpu_desc_handle) const
{
    ImGui_ImplWin32_Init(hWindow);
    ImGui_ImplDX12_Init(device, num_frames_in_flight,
        rtv_format, srv_descriptor_heap,
        font_srv_cpu_desc_handle, font_srv_gpu_desc_handle);
}

void ImguiManager::render(ID3D12GraphicsCommandList* commandList) const
{
    if (!ImGui::GetDrawData()) return;
    Renderer::GetInstance().EnqueueRenderPass(::ImGuiRender);
}

void ImguiManager::flushFrame()
{
    ::ImGuiNewFrame();
    
    if (isShow)
    {
        ImGui::Begin("EditorControl", nullptr, ImGuiWindowFlags_NoResize);
        if (ImGui::Button("HideAllEditor"))
        {
            isShow = false;
        }
        ImGui::End();
        //call all draw function
        for (auto& editorUi : registry)
        {
            editorUi.second();
        }
    }
    else
    {
        ImGui::Begin("EditorControl", nullptr, ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoResize);
        if (ImGui::Button("ShowAllEditor"))
        {
            isShow = true;
        }
        ImGui::End();
    }
    
    
    ImGui::Render();
}

ImguiManager::~ImguiManager()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

#endif
