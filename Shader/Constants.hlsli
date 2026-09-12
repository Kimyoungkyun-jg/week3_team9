cbuffer ObjectConstants : register(b0)
{
    row_major float4x4 MVP;
    float3 ColorOverride;
    float ColorOverrideAmount;
    float2 UVScale;
    float2 UVOffset;
}

cbuffer FrameConstants : register(b1)
{
    float2 ViewportSize;
    float2 Padding;
}


cbuffer LightConstants : register(b2)
{
    float3 LightPosition; // 광원 위치(월드 좌표)
    float LightRange; // 최대 도달 거리
    float3 LightDirection; // 빛 비추는 방향 (단위 벡터)
    float InnerCos; // 내부 각도 코사인 
    float3 LightColor; // 빛 색상
    float OuterCos; // 외부 각도 코사인 
};