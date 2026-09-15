//#include "Constants.hlsli"

cbuffer BillboardConstants : register(b0)
{
    row_major float4x4 VP;
    float3 ColorOverride2;
    float ColorOverrideAmount2;
    float2 UVScale2;
    float2 UVOffset2;
    
    float3 Center;
    float Padding1;
    float3 ViewRight;
    float Padding2;
    float3 ViewUp;
    float Padding3;
    float2 BillboardSize;
    float2 Padding4;
}

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
    float3 Normal : NORMAL;
};

PS_INPUT MainVS(VS_INPUT Input)
{
    PS_INPUT Output;
    
    float3 VertexPosition = Center
    + ViewRight * Input.Position.y * BillboardSize.x
    + ViewUp * Input.Position.z * BillboardSize.y;
  
    Output.Position = mul(float4(VertexPosition, 1.0f), VP);
    Output.Color = Input.Color;
    Output.UV = Input.UV * UVScale2 + UVOffset2;

    // 월드 공간 법선 변환
    Output.Normal = float4(Input.Normal, 0.0f);

    return Output;
}