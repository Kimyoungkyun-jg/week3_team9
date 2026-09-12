#include "UAnimatedBillboardComp.h"
#include "Runtime/Engine/UScene.h"
#include "UClass.h"
#include <algorithm>

IMPLEMENT_UCLASS(UAnimatedBillboardComp, UBillBoardComp)
UCLASS_META(UAnimatedBillboardComp, DisplayName, "AnimatedBillboard")

void UAnimatedBillboardComp::SetSpriteSheet(int InGridX, int InGridY,
                                            float InFrameRate,
                                            int InTotalFrames) {
  GridX = (InGridX > 0) ? InGridX : 1;
  GridY = (InGridY > 0) ? InGridY : 1;
  FrameRate = (InFrameRate > 0.0f) ? InFrameRate : 1.0f;

  if (InTotalFrames > 0) {
    TotalFrames = InTotalFrames;
  } else {
    TotalFrames = GridX * GridY;
  }

  CurrentFrame = 0;
  ElapsedTime = 0.0f;
  RefreshUV();
}

void UAnimatedBillboardComp::Stop() {
  bPlaying = false;
  CurrentFrame = 0;
  ElapsedTime = 0.0f;
  RefreshUV();
}

void UAnimatedBillboardComp::SetCurrentFrame(int InFrame) {
  if (TotalFrames > 0) {
    CurrentFrame = std::clamp(InFrame, 0, TotalFrames - 1);
    RefreshUV();
  }
}

void UAnimatedBillboardComp::Update(float DeltaTime) {
  if (!bPlaying || TotalFrames <= 1 || FrameRate <= 0.0f) {
    return;
  }

  ElapsedTime += DeltaTime;
  const float FrameDuration = 1.0f / FrameRate;

  while (ElapsedTime >= FrameDuration) {
    ElapsedTime -= FrameDuration;
    CurrentFrame++;

    if (CurrentFrame >= TotalFrames) {
      if (bLoop) {
        CurrentFrame = 0;
      } else {
        CurrentFrame = TotalFrames - 1;
        bPlaying = false;
        break;
      }
    }
  }

  RefreshUV();
}

void UAnimatedBillboardComp::RefreshUV() {
  if (GridX <= 0 || GridY <= 0) {
    CurrentUVScale = FVector2{1.0f, 1.0f};
    CurrentUVOffset = FVector2{0.0f, 0.0f};
    return;
  }

  CurrentUVScale.X = 1.0f / static_cast<float>(GridX);
  CurrentUVScale.Y = 1.0f / static_cast<float>(GridY);

  const int Col = CurrentFrame % GridX;
  const int Row = CurrentFrame / GridX;

  CurrentUVOffset.X = static_cast<float>(Col) * CurrentUVScale.X;
  CurrentUVOffset.Y = static_cast<float>(Row) * CurrentUVScale.Y;
}

void UAnimatedBillboardComp::UpdateUVinfo(FObjectConstants &InputConstant) {
  InputConstant.UVScale = CurrentUVScale;
  InputConstant.UVOffset = CurrentUVOffset;
}
