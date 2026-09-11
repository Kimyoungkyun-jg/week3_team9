#include "FMaterial.h"
#include "FRenderer.h"
#include <d3d11.h>

void FMaterial::SetPipeLine(const TSharedPtr<FRenderPipeline>& InPipeline)
{
    Pipeline = InPipeline;
}


void FMaterial::BindResources(ID3D11DeviceContext& Context) const
{
}
