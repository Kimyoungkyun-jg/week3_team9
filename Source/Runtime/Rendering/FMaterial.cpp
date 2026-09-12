#include "FMaterial.h"
#include "FRenderer.h"
#include "FRenderResourceLibrary.h"
#include "Runtime/Core/Log.h"
#include <d3d11.h>
#include <algorithm>

void FMaterial::SetPipeLine(const TSharedPtr<FRenderPipeline>& InPipeline)
{
    Pipeline = InPipeline;
}


void FMaterial::SetTexture(const TSharedPtr<FTexture>& InTexture)
{
    Texture = InTexture;
}

bool FMaterial::SetTextureByName(const FString& InTextureName)
{
    if (!ResourceLibrary)
    {
        UE_LOG("[Material] Resource library is null. Texture '%s' cannot be found.", InTextureName.c_str());
        return false;
    }

    return SetTextureByName(InTextureName, *ResourceLibrary);
}

bool FMaterial::SetTextureByName(const FString& InTextureName, const FRenderResourceLibrary& InLibrary)
{
    return SetTextureByName(InTextureName, InLibrary.AllTextureMap);
}

bool FMaterial::SetTextureByName(const FString& InTextureName, const TMap<FString, TSharedPtr<FTexture>>& InTextureMap)
{
    // 원본 키로 검색
    auto it = InTextureMap.find(InTextureName);
    if (it != InTextureMap.end() && it->second)
    {
        SetTexture(it->second);
        return true;
    }

    // 소문자 키로 검색
    FString LowerKey = InTextureName;
    std::transform(LowerKey.begin(), LowerKey.end(), LowerKey.begin(), ::tolower);
    it = InTextureMap.find(LowerKey);
    if (it != InTextureMap.end() && it->second)
    {
        SetTexture(it->second);
        return true;
    }

    // 텍스처 미존재 로그 출력
    UE_LOG("[Material] Texture '%s' not found in texture map.", InTextureName.c_str());
    return false;
}

void FMaterial::BindResources(ID3D11DeviceContext& Context) const
{
    // 텍스처가 없어도 반드시 바인딩한다.
    // D3D 상태는 끈끈해서, 건너뛰면 이전 드로우의 SRV가 슬롯에 남는다.
    ID3D11ShaderResourceView* SRVs[1] = { Texture ? Texture->GetSRV() : nullptr };
    Context.PSSetShaderResources(0u, 1u, SRVs);
}
