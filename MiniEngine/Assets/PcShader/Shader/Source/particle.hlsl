#include "Common/Common.hlsl"

struct SimpleVertexInput
{
    float3 position : POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct FragInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

BEGIN_OBJECT_DATA
END_OBJECT_DATA

BEGIN_MATERIAL_DATA(b2)
float4 color;
float4 blendFactor;
float4 TextUV;
END_MATERIAL_DATA

FragInput VsMain(SimpleVertexInput input)
{
    FragInput o;
    float4 worldPosition = float4(input.position, 1);
    o.position = mul(mul(mul(worldPosition, m_model), m_view), m_projection);
    o.uv = input.uv;
    return o;
}

float4 PsMain(FragInput input) : SV_TARGET
{
    return color;
}