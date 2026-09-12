#pragma once

#include "UBillBoardComp.h"

// 애니메이션 빌보드 컴포넌트
class UAnimatedBillboardComp : public UBillBoardComp {
  DECLARE_UCLASS(UAnimatedBillboardComp, UBillBoardComp)
  GENERATED_BODY()

protected:
  explicit UAnimatedBillboardComp() = default;

public:
  // 매 프레임 애니메이션 갱신
  void Update(float DeltaTime) override;

  // 상수 버퍼 좌표 갱신
  void UpdateUVinfo(FObjectConstants &InputConstant) override;

  // 스프라이트 시트 설정
  void SetSpriteSheet(int InGridX, int InGridY, float InFrameRate = 10.0f,
                      int InTotalFrames = -1);

  // 재생 시작
  void Play() { bPlaying = true; }

  // 일시 정지
  void Pause() { bPlaying = false; }

  // 재생 정지
  void Stop();

  // 프레임 지정
  void SetCurrentFrame(int InFrame);

  // 현재 프레임 반환
  int GetCurrentFrame() const { return CurrentFrame; }

  // 재생 속도 설정
  void SetFrameRate(float InRate) { FrameRate = InRate; }

  // 재생 속도 반환
  float GetFrameRate() const { return FrameRate; }

  // 반복 재생 설정
  void SetLooping(bool bInLoop) { bLoop = bInLoop; }

  // 반복 여부 확인
  bool IsLooping() const { return bLoop; }

  // 재생 여부 확인
  bool IsPlaying() const { return bPlaying; }

private:
  // 좌표 계산
  void RefreshUV();

private:
  int GridX = 1;
  int GridY = 1;
  int TotalFrames = 1;
  int CurrentFrame = 0;

  float FrameRate = 10.0f;
  float ElapsedTime = 0.0f;

  bool bPlaying = true;
  bool bLoop = true;

  FVector2 CurrentUVScale{1.0f, 1.0f};
  FVector2 CurrentUVOffset{0.0f, 0.0f};
};
