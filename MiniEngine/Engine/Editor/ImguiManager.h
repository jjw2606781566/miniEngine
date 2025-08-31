#pragma once
#include "Engine/Memory/TankinMemory.h"
#ifdef WIN32
#include "Engine/common/Exception.h"
#include "Engine/Utility/MacroUtility.h"
#include "EditorUi.h"


class ImguiManager
{
public:
    DELETE_CONSTRUCTOR_FIVE(ImguiManager)
    static ImguiManager* sGetInstance();
    void init();
    void setWindowAndHeap(HWND hWindow, ID3D12Device* device, int num_frames_in_flight
        , DXGI_FORMAT rtv_format, ID3D12DescriptorHeap* srv_descriptor_heap
        , D3D12_CPU_DESCRIPTOR_HANDLE font_srv_cpu_desc_handle, D3D12_GPU_DESCRIPTOR_HANDLE font_srv_gpu_desc_handle) const;
    void render(ID3D12GraphicsCommandList* commandList) const;
    void flushFrame();

    //reflection
    template<class T>
    class Register
    {
    public:
        Register(const TpString& name)
        {
            ASSERT((std::is_base_of<EditorUi, T>::value), TEXT("class T is not derived from EditorUi!"));
            sGetInstance()->registerUi(name,[]()
            {
                if (T::sGetInstance()->getIsShow())
                {
                    T::sGetInstance()->drawSelf();
                }
            });
        }
    };

    using DrawFunction = std::function<void()>;
    
    void registerUi(const TpString& name, const DrawFunction& func)
    {
        auto itor = registry.find(name);
        ASSERT(itor == registry.end(), TEXT("EditorUi class name already registered!"));
        registry[name] = func;
    }
    
private:
    bool isShow = false;
    static ImguiManager* sInstance;
    TpUnorderedMap<TpString, DrawFunction> registry;
    ImguiManager() = default;
    ~ImguiManager();
};

#define REGISTER_EDITOR_UI(type, name)\
    static ImguiManager::Register<type> componentRegister(name);

#endif