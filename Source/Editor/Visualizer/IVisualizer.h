#pragma once

class UPrimitiveComponent;
class FRenderView;
struct FCamera;
struct FVector4;

/// <summary>
/// 컴포넌트의 시각화를 담당하는 Visualizer 입니다.
/// </summary>
class IVisualizer
{
public:
	virtual ~IVisualizer() = default;

    virtual void Draw(
        const UPrimitiveComponent& Component,
        FRenderView& RenderView,
        const FCamera& Camera,
        const FVector4& Color
    ) const = 0;
};
