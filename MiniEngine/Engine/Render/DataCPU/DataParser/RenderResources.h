#pragma once
#include <istream>

#include "../ResourceHandle.h"
#include "Engine/render/MeshData.h"
#include "Engine/render/RenderResource.h"

class MeshCPU;
class RenderMeshResource
{
    friend class MeshCPU;
public:
    bool LoadMesh(std::istream& inputStream);
    bool LoadMesh_Plane(float quadWidth = 1.0f);
    bool LoadMesh_Sphere(float radius, int xdiv, int ydiv, float tx = 0, float ty = 0, float tz = 0);

    struct MeshVertex
    {
        float position[3];
        float vtxcolor[3];
        float noraml[3];
        float uv[2];
    };

    template<typename ELEMENT_TYPE>
    class MeshBufferCPU final
    {
    public:
        ELEMENT_TYPE* Data = nullptr;
        size_t AllocBytes = 0;
        int NumElements = 0;
        uint32_t GetDataBytes() { return NumElements * sizeof(ELEMENT_TYPE); }
        static uint32_t GetStride() { return sizeof(ELEMENT_TYPE); }
        uint32_t GetNumElements() const { return NumElements; }
        void Reset()
        {
            if (Data)
            {
                free(Data);
                Data = nullptr;
                NumElements = 0;
                AllocBytes = 0;
            }
        }
        void GrowIfNeeded(int numMoreElements)
        {
            if ((NumElements + 1) * sizeof(ELEMENT_TYPE) >= AllocBytes)
            {
                AllocBytes += numMoreElements * sizeof(ELEMENT_TYPE);
                Data = (ELEMENT_TYPE*)realloc(Data, AllocBytes);
            }
        }

        void Resize(int numElements)
        {
            if (numElements * sizeof(ELEMENT_TYPE) > AllocBytes)
            {
                AllocBytes = numElements * sizeof(ELEMENT_TYPE);
                Data = (ELEMENT_TYPE*)realloc(Data, AllocBytes);
            }
            NumElements = numElements;
        }

        ~MeshBufferCPU() { Reset(); }
    };

    MeshBufferCPU<MeshVertex> VerticesCPU;
    MeshBufferCPU<uint32_t> IndicesCPU;
    MeshData mMeshDataGpu;
    bool IsDynamicMesh = false;
};

class RenderTextureResource
{
public:
    bool LoadTGATexture(std::istream& inputStream);
    void CreateDefaultTextureData();
    void ResetTextureData();
    virtual ~RenderTextureResource() { ResetTextureData(); }

    template<typename CHANNEL_TYPE, int NUM_CHANNELS>
    struct PixelType
    {
        CHANNEL_TYPE PixelData[NUM_CHANNELS];
        static constexpr unsigned int GetBytesPerPixel() { return sizeof(CHANNEL_TYPE)* NUM_CHANNELS; }
    };
    typedef PixelType<unsigned char, 4> RGBAPixel;

    int					Width = 0;
    int					Height = 0;
    
    RGBAPixel*			RGBATextureDataCPU = nullptr;
    int					RGBATextureDataBytes = 0;
    TextureRef      mDataGpuHandle;
};