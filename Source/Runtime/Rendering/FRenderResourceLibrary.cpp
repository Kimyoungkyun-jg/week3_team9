#include "FRenderResourceLibrary.h"

#include "Runtime/Core/TArray.h"
#include "Runtime/Rendering/FRenderer.h"
#include "Runtime/Math/FVector.h"
#include <Windows.h>
#include <cmath>
#include <filesystem>
#include <numbers>
#include "Runtime/Geometry/Sphere.h"

namespace
{
	FWString GetExecutableDirectory()
	{
		wchar_t Buffer[256];
		GetModuleFileNameW(nullptr, Buffer, 256);
		return std::filesystem::path(Buffer).parent_path();
	}
}

bool FRenderResourceLibrary::Initialize(FRenderer& Renderer)
{
	if (!CreateCubeMesh(Renderer) ||
		!CreateCylinderMesh(Renderer, 1.0f, 24u, 1.0f ,1.0f) ||
		!CreateConeMesh(Renderer) ||
		!CreateArrowMesh(Renderer) ||
		!CreateSimpleMaterial(Renderer) ||
		!CreateGridMaterial(Renderer) ||
		!CreateCircleMesh(Renderer) ||
		!CreateRotationGizmoMesh(Renderer) ||
		!CreateSquareArrowMesh(Renderer) ||
		!CreateGridMesh(Renderer) ||
		!CreateSphereMesh(Renderer) ||
		!CreateLineMesh(Renderer) ||
		!CreatePlaneMesh(Renderer) ||
		!CreateRotationGizmoMaterial(Renderer))
	{
		return false;
	}

	return true;
}

bool FRenderResourceLibrary::CreateCubeMesh(FRenderer& Renderer)
{
	TArray<FVertexPositionColor> Vertices = {
		{ FVector(-0.5, -0.5, -0.5), FVector(0.0, 0.0, 0.0) },
		{ FVector(0.5, -0.5, -0.5), FVector(1.0, 0.0, 0.0) },
		{ FVector(0.5,  0.5, -0.5), FVector(1.0, 1.0, 0.0) },
		{ FVector(-0.5,  0.5, -0.5), FVector(0.0, 1.0, 0.0) },
		{ FVector(-0.5, -0.5,  0.5), FVector(0.0, 0.0, 1.0) },
		{ FVector(0.5, -0.5,  0.5), FVector(1.0, 0.0, 1.0) },
		{ FVector(0.5,  0.5,  0.5), FVector(1.0, 1.0, 1.0) },
		{ FVector(-0.5,  0.5,  0.5), FVector(0.0, 1.0, 1.0) },
	};

	const TArray<uint32> Indices = {
		0, 2, 1, 0, 3, 2, // -Z
		4, 5, 6, 4, 6, 7, // +Z
		0, 1, 5, 0, 5, 4, // -Y
		3, 7, 6, 3, 6, 2, // +Y
		0, 4, 7, 0, 7, 3, // -X
		1, 2, 6, 1, 6, 5, // +X
	};

	FMeshDesc MeshDesc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexPositionColor)),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	CubeMesh = Renderer.CreateMesh(MeshDesc);

	return CubeMesh != nullptr;
}

// TODO: 컬러는 테스트용
bool FRenderResourceLibrary::CreateCylinderMesh(FRenderer& Renderer, float Height, uint32 SliceCount, float TopRadius, float BottomRadius)
{
	constexpr float TAU = std::numbers::pi_v<float> * 2.0f;
	const float DTheta = TAU / static_cast<float>(SliceCount);

	TArray<FVertexPositionColor> Vertices;

	for (int Ring = 0; Ring < 2; ++Ring)
	{
		const float Radius = (Ring == 0) ? BottomRadius : TopRadius;
		const float Y = (Ring == 0) ? -0.5f * Height : 0.5f * Height;
		for (uint32 i = 0u; i <= SliceCount; ++i)
		{
			const float Theta = DTheta * static_cast<float>(i);
			const float c = cosf(Theta);
			const float s = sinf(Theta);
			const FVector Pos(Radius * c, Y, Radius * s);
			const float t = static_cast<float>(i) / static_cast<float>(SliceCount);
			const FVector Color(t, static_cast<float>(Ring), 1.0f - t);
			Vertices.push_back({ Pos, Color });
		}
	}

	TArray<uint32> Indices;
	Indices.reserve(static_cast<uint64>(SliceCount * 6u));

	for (uint32 i = 0u; i < SliceCount; ++i)
	{
		Indices.push_back(i);
		Indices.push_back(i + SliceCount + 1);
		Indices.push_back(i + 1);

		Indices.push_back(i + 1);
		Indices.push_back(i + SliceCount + 1);
		Indices.push_back(i + SliceCount + 2);
	}

	// 위 뚜껑
	{
		//중심
		const uint32 Center = static_cast<uint32>(Vertices.size());
		Vertices.emplace_back(FVector(0.0f, 0.5f * Height, 0.0f), FVector{ 1.0f, 1.0f, 1.0f });

		const uint32 First = static_cast<uint32>(Vertices.size());
		for (uint32 i = 0u; i <= SliceCount; ++i)
		{
			const float Theta = DTheta * static_cast<float>(i);
			Vertices.push_back({
				FVector(TopRadius * cosf(Theta), 0.5f * Height, TopRadius * sinf(Theta)),
				FVector(0.9f, 0.9f, 0.2f) });
		}

		for (uint32 i = 0u; i < SliceCount; ++i)
		{
			Indices.push_back(Center);
			Indices.push_back(First + i + 1);
			Indices.push_back(First + i);
		}
	}

	// 아래 뚜껑
	{
		const uint32 Center = static_cast<uint32>(Vertices.size());
		Vertices.emplace_back(FVector(0.0f, -0.5f * Height, 0.0f), FVector{ 0.3f, 0.3f, 0.3f });

		const uint32 First = static_cast<uint32>(Vertices.size());
		for (uint32 i = 0u; i <= SliceCount; ++i)
		{
			const float Theta = DTheta * static_cast<float>(i);
			Vertices.push_back({
				FVector(BottomRadius * cosf(Theta), -0.5f * Height, BottomRadius * sinf(Theta)),
				FVector(0.2f, 0.2f, 0.5f) });
		}

		for (uint32 i = 0u; i < SliceCount; ++i)
		{
			Indices.push_back(Center);
			Indices.push_back(First + i);
			Indices.push_back(First + i + 1);
		}
	}

	FMeshDesc MeshDesc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexPositionColor)),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	CylinderMesh = Renderer.CreateMesh(MeshDesc);
	return CylinderMesh != nullptr;
}

bool FRenderResourceLibrary::CreateConeMesh(FRenderer& Renderer)
{
	constexpr float BottomRadius = 0.5f;
	constexpr float Height = 1.0f;
	constexpr uint32 SliceCount = 24;
	constexpr float TAU = std::numbers::pi_v<float> * 2.0f;
	constexpr float DTheta = TAU / static_cast<float>(SliceCount);

	FVector Color;
	TArray<FVertexPositionColor> Vertices;
	const float Radius = BottomRadius;

		for (int Ring = 0; Ring < 2; ++Ring)
		{

			for (uint32 i = 0u; i <= SliceCount; ++i)
			{
				const float t = static_cast<float>(i) / static_cast<float>(SliceCount);
				const FVector Color(t, static_cast<float>(i), 1.0f - t);
				if(Ring == 0)
				{
					const FVector Pos(0.0f, 0.5f, 0.0f);
					Vertices.push_back({ Pos,Color });
					continue;
				}
					
				const float Theta = DTheta * static_cast<float>(i);
				const float c = cosf(Theta);
				const float s = sinf(Theta);

				const FVector Pos(Radius * c, -0.5f, Radius * s);

				Vertices.push_back({ Pos, Color });
			}
		}


	

	TArray<uint32> Indices;
	Indices.reserve(SliceCount * 6);

	for (uint32 i = 0u; i < SliceCount; ++i)
	{
		Indices.push_back(i);
		Indices.push_back(i + SliceCount + 1);
		Indices.push_back(i + 1);

		Indices.push_back(i + 1);
		Indices.push_back(i + SliceCount + 1);
		Indices.push_back(i + SliceCount + 2);
	}

	// 아래 뚜껑
	{
		const uint32 Center = static_cast<uint32>(Vertices.size());
		Vertices.push_back({ FVector(0.0f, -0.5f * Height, 0.0f), Color });
		const uint32 First = static_cast<uint32>(Vertices.size());
		for (int i = 0; i <= SliceCount; ++i)
		{
			const float Theta = DTheta * static_cast<float>(i);
			Vertices.push_back({
				FVector(BottomRadius * cosf(Theta), -0.5f * Height, BottomRadius * sinf(Theta)),
				//FVector(0.2f, 0.2f, 0.5f) });
				Color });
		}

		for (int i = 0; i < SliceCount; ++i)
		{
			Indices.push_back(Center);
			Indices.push_back(i + First);
			Indices.push_back(i + First + 1);
		}
	}

	FMeshDesc MeshDesc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexPositionColor)),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	ConeMesh = Renderer.CreateMesh(MeshDesc);
	return ConeMesh != nullptr;
}

bool FRenderResourceLibrary::CreateArrowMesh(FRenderer& Renderer)
{
	constexpr uint32 SliceCount = 16u;
	constexpr float ShaftLength = 0.75f;
	constexpr float ShaftRadius = 0.025f;
	constexpr float HeadRadius = 0.075f;
	constexpr float ArrowLength = 1.0f;
	constexpr float Tau = std::numbers::pi_v<float> * 2.0f;
	constexpr FVector Color{ 1.0f, 1.0f, 1.0f };

	TArray<FVertexPositionColor> Vertices;
	TArray<uint32> Indices;

	Vertices.reserve((SliceCount + 1u) * 3u + 3u);
	Indices.reserve(SliceCount * 15u);

	const uint32 ShaftStartRing = static_cast<uint32>(Vertices.size());
	for (uint32 i = 0u; i <= SliceCount; ++i)
	{
		const float Theta = Tau * static_cast<float>(i) / static_cast<float>(SliceCount);
		Vertices.push_back({
			FVector{ 0.0f, ShaftRadius * cosf(Theta), ShaftRadius * sinf(Theta) },
			Color });
	}

	const uint32 ShaftEndRing = static_cast<uint32>(Vertices.size());
	for (uint32 i = 0u; i <= SliceCount; ++i)
	{
		const float Theta = Tau * static_cast<float>(i) / static_cast<float>(SliceCount);
		Vertices.push_back({
			FVector{ ShaftLength, ShaftRadius * cosf(Theta), ShaftRadius * sinf(Theta) },
			Color });
	}

	for (uint32 i = 0u; i < SliceCount; ++i)
	{
		Indices.push_back(ShaftStartRing + i);
		Indices.push_back(ShaftStartRing + i + 1u);
		Indices.push_back(ShaftEndRing + i);

		Indices.push_back(ShaftStartRing + i + 1u);
		Indices.push_back(ShaftEndRing + i + 1u);
		Indices.push_back(ShaftEndRing + i);
	}

	const uint32 ShaftStartCenter = static_cast<uint32>(Vertices.size());
	Vertices.push_back({ FVector{ 0.0f, 0.0f, 0.0f }, Color });
	for (uint32 i = 0u; i < SliceCount; ++i)
	{
		Indices.push_back(ShaftStartCenter);
		Indices.push_back(ShaftStartRing + i + 1u);
		Indices.push_back(ShaftStartRing + i);
	}

	const uint32 HeadBaseRing = static_cast<uint32>(Vertices.size());
	for (uint32 i = 0u; i <= SliceCount; ++i)
	{
		const float Theta = Tau * static_cast<float>(i) / static_cast<float>(SliceCount);
		Vertices.push_back({
			FVector{ ShaftLength, HeadRadius * cosf(Theta), HeadRadius * sinf(Theta) },
			Color });
	}

	const uint32 HeadTip = static_cast<uint32>(Vertices.size());
	Vertices.push_back({ FVector{ ArrowLength, 0.0f, 0.0f }, Color });

	for (uint32 i = 0u; i < SliceCount; ++i)
	{
		Indices.push_back(HeadBaseRing + i);
		Indices.push_back(HeadBaseRing + i + 1u);
		Indices.push_back(HeadTip);
	}

	const uint32 HeadBaseCenter = static_cast<uint32>(Vertices.size());
	Vertices.push_back({ FVector{ ShaftLength, 0.0f, 0.0f }, Color });
	for (uint32 i = 0u; i < SliceCount; ++i)
	{
		Indices.push_back(HeadBaseCenter);
		Indices.push_back(HeadBaseRing + i + 1u);
		Indices.push_back(HeadBaseRing + i);
	}

	const FMeshDesc MeshDesc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexPositionColor)),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	ArrowMesh = Renderer.CreateMesh(MeshDesc);
	return ArrowMesh != nullptr;
}

bool FRenderResourceLibrary::CreateCircleMesh(FRenderer& Renderer)
{
	constexpr uint32 SliceCount = 32u;
	constexpr float Radius = 1.0f;
	constexpr float Width = 0.07f;

	FVector Color{ 0.0f, 0.0f, 0.0f };

	TArray<FVertexPositionColor> Vertices;
	TArray<uint32> Indices;

	constexpr uint32 TubeSliceCount = 8u;
	constexpr uint32 RingVertexCount = TubeSliceCount + 1u;
	constexpr float TubeRadius = Width * 0.5f;
	constexpr float HalfPi = std::numbers::pi_v<float> * 0.5f;
	Vertices.reserve(SliceCount * RingVertexCount);
	Indices.reserve(SliceCount * TubeSliceCount * 6u);

	const float Step = std::numbers::pi_v<float> * 2.0f / static_cast<float>(SliceCount);
	for (uint32 i = 0; i < SliceCount; ++i)
	{
		const float Cos = std::cosf(Step * static_cast<float>(i));
		const float Sin = std::sinf(Step * static_cast<float>(i));
		for (uint32 j = 0; j <= TubeSliceCount; ++j)
		{
			const float Angle = -HalfPi + std::numbers::pi_v<float> * static_cast<float>(j) / static_cast<float>(TubeSliceCount);
			const float RingRadius = (j == 0u || j == TubeSliceCount) ? Radius : Radius + TubeRadius * std::cosf(Angle);
			Vertices.push_back({ FVector{ TubeRadius * std::sinf(Angle), Cos * RingRadius, Sin * RingRadius }, Color });
		}
	}

	for (uint32 i = 0; i < SliceCount; ++i)
	{
		const uint32 Next = (i + 1u) % SliceCount;
		for (uint32 j = 0; j < TubeSliceCount; ++j)
		{
			const uint32 A = i * RingVertexCount + j;
			const uint32 B = Next * RingVertexCount + j;
			
			Indices.push_back(A);
			Indices.push_back(B);
			Indices.push_back(B + 1u);
			Indices.push_back(A);
			Indices.push_back(B + 1u);
			Indices.push_back(A + 1u);
		}
	}

	const FMeshDesc Desc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexPositionColor)),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	CircleMesh = Renderer.CreateMesh(Desc);
	return CircleMesh != nullptr;
}

bool FRenderResourceLibrary::CreateRotationGizmoMesh(FRenderer& Renderer)
{
	constexpr uint32 SliceCount = 32u;
	constexpr float Radius = 1.0f;

	FVector In{ -1.0f, 0.0f, 0.0f };
	FVector Out{ 1.0f, 0.0f, 0.0f };

	TArray<FVertexPositionColor> Vertices;
	TArray<uint32> Indices;
	Vertices.reserve(SliceCount * 4u);
	Indices.reserve(SliceCount * 12u);

	float Step = std::numbers::pi_v<float> *2.0f / static_cast<float>(SliceCount);
	for (uint32 i = 0; i < SliceCount; ++i)
	{
		float Cos = std::cosf(Step * static_cast<float>(i));
		float Sin = std::sinf(Step * static_cast<float>(i));

		Vertices.push_back({ FVector{ 0.0f, Cos * Radius, Sin * Radius }, Out });
		Vertices.push_back({ FVector{ 0.0f, Cos * Radius, Sin * Radius }, In });

		Indices.push_back(2u * i - 2u);
		Indices.push_back(2u * i - 1u);
		Indices.push_back(2u * i + 1u);

		Indices.push_back(2u * i - 2u);
		Indices.push_back(2u * i + 1u);
		Indices.push_back(2u * i - 1u);

		Indices.push_back(2u * i - 2u);
		Indices.push_back(2u * i + 1u);
		Indices.push_back(2u * i);

		Indices.push_back(2u * i - 2u);
		Indices.push_back(2u * i);
		Indices.push_back(2u * i + 1u);
	}
	Indices[0] = 2u * SliceCount - 2u;
	Indices[1] = 2u * SliceCount - 1u;
	Indices[3] = 2u * SliceCount - 2u;
	Indices[5] = 2u * SliceCount - 1u;
	Indices[6] = 2u * SliceCount - 2u;
	Indices[9] = 2u * SliceCount - 2u;

	const FMeshDesc Desc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexPositionColor)),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	RotationGizmoMesh = Renderer.CreateMesh(Desc);
	return RotationGizmoMesh != nullptr;
}

bool FRenderResourceLibrary::CreateSquareArrowMesh(FRenderer& Renderer)
{
	constexpr float ShaftLength = 0.85f;
	constexpr float ShaftRadius = 0.025f;
	constexpr float ArrowLength = 1.0f;
	constexpr float TipSize = ArrowLength - ShaftLength;

	const TArray<FVertexPositionColor> CubeVertices = {
		{ FVector(-0.5, -0.5, -0.5), FVector(1.0, 0.0, 0.0) },
		{ FVector(0.5, -0.5, -0.5), FVector(1.0, 0.0, 0.0) },
		{ FVector(0.5,  0.5, -0.5), FVector(1.0, 0.0, 0.0) },
		{ FVector(-0.5,  0.5, -0.5), FVector(1.0, 0.0, 0.0) },
		{ FVector(-0.5, -0.5,  0.5), FVector(0.0, 0.0, 1.0) },
		{ FVector(0.5, -0.5,  0.5), FVector(1.0, 0.0, 1.0) },
		{ FVector(0.5,  0.5,  0.5), FVector(1.0, 1.0, 1.0) },
		{ FVector(-0.5,  0.5,  0.5), FVector(0.0, 1.0, 1.0) },
	};
	const TArray<uint32> CubeIndices = {
		0, 2, 1, 0, 3, 2, // -Z
		4, 5, 6, 4, 6, 7, // +Z
		0, 1, 5, 0, 5, 4, // -Y
		3, 7, 6, 3, 6, 2, // +Y
		0, 4, 7, 0, 7, 3, // -X
		1, 2, 6, 1, 6, 5, // +X
	};

	TArray <FVertexPositionColor> Vertices;
	TArray<uint32> Indices;

	Vertices.reserve(16u);
	Indices.reserve(72u);

	for (const auto& [Position, Color] : CubeVertices)
	{
		FVector ScaledPosition = Position;
		ScaledPosition.X *= ShaftLength;
		ScaledPosition.Y *= ShaftRadius;
		ScaledPosition.Z *= ShaftRadius;
		Vertices.push_back({ ScaledPosition + FVector{ ShaftLength * 0.5f, 0.0f, 0.0f }, Color });
	}

	for (const auto& Index : CubeIndices)
	{
		Indices.push_back(Index);
	}

	for (const auto& [Position, Color] : CubeVertices)
	{
		FVector ScaledPosition = Position;
		ScaledPosition.X *= TipSize;
		ScaledPosition.Y *= TipSize;
		ScaledPosition.Z *= TipSize;
		Vertices.push_back({ ScaledPosition + FVector{ TipSize * 0.5f + ShaftLength, 0.0f, 0.0f }, Color });
	}

	for (const auto& Index : CubeIndices)
	{
		Indices.push_back(Index + 8u);
	}

	const FMeshDesc Desc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = static_cast<uint32>(sizeof(FVertexPositionColor)),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	SquareArrowMesh = Renderer.CreateMesh(Desc);
	return SquareArrowMesh != nullptr;
}

bool FRenderResourceLibrary::CreateGridMesh(FRenderer& Renderer)
{
	constexpr float HalfW = 50.0f;   // width  100
	constexpr float HalfH = 50.0f;   // height 100

	// XY 평면 (Z=0), 위(+Z)를 향하는 감김 — 큐브의 +Z 면과 동일
	const TArray<FVertexPositionColor> Vertices = {
		{ FVector{ -HalfW, -HalfH, 0.0f }, FVector{} },   // 0
		{ FVector{  HalfW, -HalfH, 0.0f }, FVector{} },   // 1
		{ FVector{  HalfW,  HalfH, 0.0f }, FVector{} },   // 2
		{ FVector{ -HalfW,  HalfH, 0.0f }, FVector{} },   // 3
	};
	const TArray<uint32> Indices = { 0, 1, 2, 0, 2, 3 };

	FMeshDesc MeshDesc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = sizeof(FVertexPositionColor),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.IndexData = Indices.data(),
		.IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
		.IndexCount = static_cast<uint32>(Indices.size()),
	};

	GridMesh = Renderer.CreateMesh(MeshDesc);
	return GridMesh != nullptr;
}

bool FRenderResourceLibrary::CreateSphereMesh(FRenderer& Renderer)
{
	TArray<FVertexPositionColor> Vertices;
	for (int i = 2399; i >= 0; --i)
	{
		Vertices.push_back(sphere_vertices[i]);
	}
	
	FMeshDesc MeshDesc{
	.VertexLayout = EVertexLayout::PositionColor,
	.VertexData = Vertices.data(),
	.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
	.VertexStride = sizeof(FVertexPositionColor),
	.VertexCount = static_cast<uint32>(Vertices.size()),
	};





	SphereMesh = Renderer.CreateMesh(MeshDesc);
	return SphereMesh != nullptr;
}

bool FRenderResourceLibrary::CreateLineMesh(FRenderer& Renderer)
{
	TArray<FVertexPositionColor> Vertices = {
		{ FVector{ 0.0f, 0.0f, 0.0f }, FVector{ 1.0f, 1.0f, 1.0f } },
		{ FVector{ 1.0f, 0.0f, 0.0f }, FVector{ 1.0f, 1.0f, 1.0f } },
	};

	FMeshDesc Desc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = sizeof(FVertexPositionColor),
		.VertexCount = static_cast<uint32>(Vertices.size()),
		.bIsLine = true
	};

	LineMesh = Renderer.CreateMesh(Desc);
	return LineMesh != nullptr;
}

bool FRenderResourceLibrary::CreatePlaneMesh(FRenderer& Renderer)
{
	TArray<FVertexPositionColor> Vertices = {
		{ FVector{ 0.0f, 0.0f, 1.0f }, FVector{ 1.0f, 0.0f, 0.0f } },
		{ FVector{ 0.0f, 0.866f, -0.5f }, FVector{ 0.0f, 1.0f, 0.0f } },
		{ FVector{ 0.0f, -0.866f, -0.5f }, FVector{ 0.0f, 0.0f, 1.0f } }
	};

	FMeshDesc Desc{
		.VertexLayout = EVertexLayout::PositionColor,
		.VertexData = Vertices.data(),
		.VertexDataSize = static_cast<uint32>(sizeof(FVertexPositionColor) * Vertices.size()),
		.VertexStride = sizeof(FVertexPositionColor),
		.VertexCount = static_cast<uint32>(Vertices.size()),
	};

	PlaneMesh = Renderer.CreateMesh(Desc);
	return PlaneMesh != nullptr;
}

bool FRenderResourceLibrary::CreateSimpleMaterial(FRenderer& Renderer)
{
	FWString Path = GetExecutableDirectory();

	FMaterialDesc Desc = {
		.VertexShaderFileName = Path + L"/Shader/ExampleVS.cso",
		.PixelShaderFileName = Path + L"/Shader/ExamplePS.cso",
		.VertexLayout = EVertexLayout::PositionColor,
	};
	
	SimpleMaterial = Renderer.CreateMaterial(Desc);

	return SimpleMaterial != nullptr;
}

bool FRenderResourceLibrary::CreateGridMaterial(FRenderer& Renderer)
{
	FWString Path = GetExecutableDirectory();

	FMaterialDesc Desc = {
		.VertexShaderFileName = Path + L"/Shader/GridVS.cso",
		.PixelShaderFileName = Path + L"/Shader/GridPS.cso",
		.VertexLayout = EVertexLayout::PositionColor,
	};

	GridMaterial = Renderer.CreateMaterial(Desc);

	return GridMaterial != nullptr;
}

bool FRenderResourceLibrary::CreateRotationGizmoMaterial(FRenderer& Renderer)
{
	FWString Path = GetExecutableDirectory();

	FMaterialDesc Desc = {
		.VertexShaderFileName = Path + L"/Shader/RotationGizmoVS.cso",
		.PixelShaderFileName = Path + L"/Shader/RotationGizmoPS.cso",
		.VertexLayout = EVertexLayout::PositionColor,
	};

	RotationGizmoMaterial = Renderer.CreateMaterial(Desc);

	return RotationGizmoMaterial != nullptr;
}
