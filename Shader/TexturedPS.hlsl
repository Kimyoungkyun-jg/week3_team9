#include "Constants.hlsli"

Texture2D DiffuseTexture : register(t0);
SamplerState DiffuseSampler : register(s0);

struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
};

float4 MainPS(PS_INPUT Input) : SV_Target
{
    float4 Sampled = DiffuseTexture.Sample(DiffuseSampler, Input.UV);

    // 알파가 없거나 검은색인 영역 제거
    clip(Sampled.a - 0.1f);
    clip(max(Sampled.r, max(Sampled.g, Sampled.b)) - 0.05f);

    // 정점 색상 반영
    float3 TintedColor = Sampled.rgb * Input.Color.rgb;
    
    float3 FinalColor = lerp(TintedColor, ColorOverride, ColorOverrideAmount);
    return float4(FinalColor, Sampled.a);
}
