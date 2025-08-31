#pragma once
#include <cstdint>
#include <functional>

enum class ResourceHandleType:uint64_t
{
    RESOURCE_HANDLE_NULL = 0xFFFFFFFFFFFFFFFF
};

struct ResourceHandle
{
public:
    uint64_t mIndex;
    ResourceHandle():mIndex(0xFFFFFFFFFFFFFFFF){} //0x00 is nullHandle
    ResourceHandle(uint64_t idx) : mIndex(idx) {}
    ResourceHandle(ResourceHandleType type):mIndex(static_cast<uint64_t>(type)){};
    bool operator==(const ResourceHandle& other)const
    {
        return mIndex == other.mIndex;
    }
    bool operator==(const ResourceHandleType& type)const
    {
        return mIndex == static_cast<uint64_t>(type);
    }
    bool operator!=(const ResourceHandleType& type)const
    {
        return mIndex != static_cast<uint64_t>(type);
    } 
};

namespace std
{
    template<>
    struct hash<ResourceHandle>
    {
        size_t operator()(const ResourceHandle& resourceHandle)const
        {
            return std::hash<uint64_t>()(resourceHandle.mIndex);
        }
    };
}
