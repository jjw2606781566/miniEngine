#include "Common/Samplers.hlsl"
#include "Common/Common.hlsl"

BEGIN_OBJECT_DATA
END_OBJECT_DATA

struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BITANGENT;
    float2 uv : TEXCOORD;
};

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
    float4 color : COLOR;
};

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
    o.color = float4(input.position + 0.5, 1);
    //o.position = mul(m_proj, mul(m_view, mul(m_model, worldPosition)));
    //o.position = float4(input.position, 1);
    return o;
}

float4 PsMain(FragInput input) : SV_TARGET
{
    input.color.xyz += color.xyz * blendFactor.x;
    return input.color;
}
