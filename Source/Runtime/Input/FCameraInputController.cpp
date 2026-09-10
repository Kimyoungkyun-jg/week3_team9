#include "FCameraInputController.h"

#include "Runtime/Engine/FCamera.h"
#include "Runtime/Math/FMatrix.h"
#include <Windows.h>

#include "FInputManager.h"

void FCameraInputController::UpdateKeyInput(FCamera& Camera, float DeltaTime) const
{
	const FMatrix Rotation =
		FMatrix::MakeRotation(FVector(0.0f, Camera.Pitch, Camera.Yaw));

	const FVector Forward{
		Rotation.M[0][0], Rotation.M[0][1], Rotation.M[0][2]
	};
	const FVector Right{
		Rotation.M[1][0], Rotation.M[1][1], Rotation.M[1][2]
	};
	// 카메라 상방 벡터
	const FVector Up{
		Rotation.M[2][0], Rotation.M[2][1], Rotation.M[2][2]
	};

	FVector Direction{};

	if (FInputManager::Get().IsKeyDown(VK_LEFT) || FInputManager::Get().IsKeyDown('A'))
	{
		Direction -= Right;
	}

	if (FInputManager::Get().IsKeyDown(VK_RIGHT) || FInputManager::Get().IsKeyDown('D'))
	{
		Direction += Right;
	}

	if (FInputManager::Get().IsKeyDown(VK_UP) || FInputManager::Get().IsKeyDown('W'))
	{
		Direction += Forward;
	}

	if (FInputManager::Get().IsKeyDown(VK_DOWN) || FInputManager::Get().IsKeyDown('S'))
	{
		Direction -= Forward;
	}

	// 하강 이동
	if (FInputManager::Get().IsKeyDown('Q'))
	{
		Direction -= Up;
	}

	// 상승 이동
	if (FInputManager::Get().IsKeyDown('E'))
	{
		Direction += Up;
	}

	Camera.Position += Direction * CameraMoveSpeed * DeltaTime;
}

void FCameraInputController::UpdateMouseInput(FCamera& Camera) const
{
	if (FInputManager::Get().IsMouseDown(EMouseButton::Right))
	{
		FVector2 Delta = FInputManager::Get().GetMouseDelta() * CameraRotateSpeed;
		Camera.Yaw += Delta.X;
		Camera.Pitch -= Delta.Y;
	}
}