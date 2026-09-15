// 외곽선 후처리 픽셀 셰이더
Texture2D<float4> SceneTexture   : register(t0);
// DXGI_FORMAT_X24_TYPELESS_G8_UINT SRV는 스텐실 값을 단일 uint로 노출한다.
Texture2D<uint>   StencilTexture : register(t1);

struct PS_IN
{
    float4 Pos : SV_POSITION;
    float2 UV  : TEXCOORD0;
};

float4 main(PS_IN input) : SV_Target
{
    const int2 pixelCoord = int2(input.Pos.xy);
    uint width;
    uint height;
    StencilTexture.GetDimensions(width, height);

    // 현재 픽셀의 스텐실 값 확인
    const uint currentStencil = StencilTexture.Load(int3(pixelCoord, 0));
    const float4 sceneColor = SceneTexture.Load(int3(pixelCoord, 0));

    // 본체 영역은 원래 색상 유지
    if (currentStencil == 1)
    {
        return sceneColor;
    }

    // 주변 스텐실 검사
    const int thickness = 2;
    const int2 offsets[8] = {
        int2( thickness,  0), int2(-thickness,  0),
        int2( 0,  thickness), int2( 0, -thickness),
        int2( thickness,  thickness), int2(-thickness,  thickness),
        int2( thickness, -thickness), int2(-thickness, -thickness)
    };

    uint neighborStencil = 0;
    for (int i = 0; i < 8; ++i)
    {
        const int2 neighbor = pixelCoord + offsets[i];
        if (neighbor.x >= 0 && neighbor.y >= 0 &&
            neighbor.x < int(width) && neighbor.y < int(height))
        {
            neighborStencil |= StencilTexture.Load(int3(neighbor, 0));
        }
    }

    // 주변에 본체가 닿아 있으면 외곽선 출력
    if (neighborStencil > 0)
    {
        return float4(1.0f, 0.55f, 0.0f, 1.0f);
    }

    return sceneColor;
}
