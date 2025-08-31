#pragma once
#include "Engine/pch.h"
#include "Engine/common/Exception.h"
#include "Engine/Memory/TankinMemory.h"

enum class Layer : unsigned char
{
    LAYER_NONE = 0x00,
    LAYER_Default = 0x01,
    LAYER_UI = 0x02,
    LAYER_CustomLayer1 = 0x04,
    LAYER_All = 0xFF,
    LAYER_NUM = 5
};

class LayerUtility
{
public:
    static LayerUtility* sGetInstance()
    {
        if (sInstance == nullptr)
        {
            sInstance = new LayerUtility();
        }
        return sInstance;
    }
    TpString getLayerName(Layer layer)
    {
        auto itor = mLayer2Name.find(layer);
        ASSERT(itor != mLayer2Name.end(), TEXT("Layer not found!"));
        return itor->second;
    }
    Layer getLayerByName(const TpString& layerName)
    {
        auto itor = mName2Layer.find(layerName);
        ASSERT(itor != mName2Layer.end(), TEXT("Layer not found!"));
        return itor->second;
    }
    const char** getLayerNames()
    {
        return mLayerNames;
    }
    int getLayerNumber(Layer layer)
    {
        int result = 0;
        for (auto& itor: mLayer2Name)
        {
            if (itor.first == layer)
            {
                return result;
            }
            ++result;
        }
        ASSERT(false, TEXT("Layer not found!"));
        return 0;
    }
private:
    LayerUtility()
    {
        mName2Layer["LAYER_NONE"] = Layer::LAYER_NONE;
        mLayer2Name[Layer::LAYER_NONE] = "LAYER_NONE";

        mName2Layer["LAYER_Default"] = Layer::LAYER_Default;
        mLayer2Name[Layer::LAYER_Default] = "LAYER_Default";

        mName2Layer["LAYER_UI"] = Layer::LAYER_UI;
        mLayer2Name[Layer::LAYER_UI] = "LAYER_UI";

        mName2Layer["LAYER_CustomLayer1"] = Layer::LAYER_CustomLayer1;
        mLayer2Name[Layer::LAYER_CustomLayer1] = "LAYER_CustomLayer1";

        mName2Layer["LAYER_All"] = Layer::LAYER_All;
        mLayer2Name[Layer::LAYER_All] = "LAYER_All";

        int i = 0;
        for (auto& itor : mLayer2Name)
        {
            mLayerNames[i] = itor.second.c_str();
            ++i;
        }
    }
    static LayerUtility* sInstance;
    TpUnorderedMap<TpString, Layer> mName2Layer;
    TpMap<Layer, TpString> mLayer2Name; //sort by layer order
    const char* mLayerNames[static_cast<int>(Layer::LAYER_NUM)];
};
