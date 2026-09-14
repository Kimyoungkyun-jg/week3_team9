#include "Constants.hlsli"

Texture2D DiffuseTexture : register(t0);
SamplerState DiffuseSampler : register(s0);

struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 UV : TEXCOORD0;
    float3 Normal : NORMAL; // 법선
};

float4 MainPS(PS_INPUT Input) : SV_Target
{
    float4 Sampled = DiffuseTexture.Sample(DiffuseSampler, Input.UV);

    // 투명 및 어두운 영역 제거
    clip(Sampled.a - 0.1f);
    clip(max(Sampled.r, max(Sampled.g, Sampled.b)) - 0.05f);
    
    // 색상 조정
    float3 BaseColor = Sampled.rgb * ColorOverride * ColorOverrideAmount;
    
    // ==== Directional Light 계산 ====
    float3 N = normalize(Input.Normal);
    float NdotL = max(0.0f, dot(N, -normalize(LightDirection)));
    float3 Diffuse = LightColor * (Intensity * NdotL);
    float3 Ambient = LightColor * AmbientIntensity;
    float3 DirectionalLight = Ambient + Diffuse;
    // ================================

    float3 FinalColor = BaseColor * DirectionalLight;
    return float4(FinalColor, Sampled.a);
}
