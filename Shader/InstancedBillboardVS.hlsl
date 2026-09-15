//#include "Constants.hlsli"

cbuffer InstancedBillboardConstants : register(b0)
{
    row_major float4x4 VP;
    
    float3 ViewRight;
    float Padding1;
    float3 ViewUp;
    float Padding2;
}

struct VS_INPUT
{
    // 정점 데이터
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 Normal : NORMAL;

    // 인스턴스 데이터
    row_major float4x4 InstanceWorld : INSTANCE_WORLD;
    float4 InstanceColor : INSTANCE_COLOR;
    float3 InstanceCenter : INSTANCE_CENTER;
    float2 BillboardSize : INSTANCE_SIZE;
    float2 InstanceUVScale : INSTANCE_UV_SCALE;
    float2 InstanceUVOffset : INSTANCE_UV_OFFSET;
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
    
    float3 GlyphLocalPosition = mul(float4(Input.Position, 1.0f), Input.InstanceWorld).xyz;
    
    float3 VertexPosition = Input.InstanceCenter
    + ViewRight * GlyphLocalPosition.y * Input.BillboardSize.x
    + ViewUp * GlyphLocalPosition.z * Input.BillboardSize.y;
  
    Output.Position = mul(float4(VertexPosition, 1.0f), VP);
    Output.Color = Input.InstanceColor;
    Output.UV = Input.UV * Input.InstanceUVScale + Input.InstanceUVOffset;

    // 월드 공간 법선 변환
    Output.Normal = mul(float4(Input.Normal, 0.0f), Input.InstanceWorld).xyz;

    return Output;
}