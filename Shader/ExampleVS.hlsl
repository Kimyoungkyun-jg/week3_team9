#include "Constants.hlsli"

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 ClipNormal : NORMAL;
};

PS_INPUT MainVS(VS_INPUT Input)
{
    PS_INPUT Output;

    Output.Position = mul(float4(Input.Position, 1.0f), MVP);
    Output.Color = Input.Color;
    Output.UV = Input.UV * UVScale + UVOffset;

    // 시선 공간 법선 변환
    Output.ClipNormal = mul(float4(Input.Normal, 0.0f), MVP).xyz;

    return Output;
}