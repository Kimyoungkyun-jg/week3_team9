#include "Constants.hlsli"

struct PS_INPUT
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

float4 MainPS(PS_INPUT Input) : SV_Target
{
    float3 FinalColor = lerp(Input.Color.rgb, ColorOverride, ColorOverrideAmount);
    return float4(FinalColor, Input.Color.a);
}
