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
