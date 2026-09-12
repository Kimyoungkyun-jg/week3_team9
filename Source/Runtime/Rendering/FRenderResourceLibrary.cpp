#include "FRenderResourceLibrary.h"
#include "Vertices.h"

#include "FRenderer.h"
#include "Runtime/Core/TArray.h"
#include "Runtime/Geometry/Sphere.h"
#include "Runtime/Math/FVector.h"
#include "Runtime/Rendering/FRenderer.h"
#include <cmath>
#include <numbers>
#include "FTexture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "ThirdParty/stb/stb_image.h"

bool FRenderResourceLibrary::Initialize(FRenderer &Renderer) {
  RendererRef = &Renderer;
  if (!CreateCubeMesh(Renderer) ||
      !CreateCylinderMesh(Renderer, 1.0f, 24u, 1.0f, 1.0f) ||
      !CreateConeMesh(Renderer) || !CreateArrowMesh(Renderer) ||
      !CreateCircleMesh(Renderer) || !CreateRotationGizmoMesh(Renderer) ||
      !CreateSquareArrowMesh(Renderer) || !CreateGridMesh(Renderer) ||
      !CreateSphereMesh(Renderer) || !CreateLineMesh(Renderer) ||
      !CreatePlaneMesh(Renderer) || !CreateRectMesh(Renderer) ||
      !CreateSimpleMaterial(Renderer) || !CreateGridMaterial(Renderer) ||
      !CreateRotationGizmoMaterial(Renderer) || !CreateTextures(Renderer) ||
      !CreateTexturedMaterial(Renderer) || !CreateTextMesh(Renderer) ||
      !CreateTextMaterial(Renderer)) {
    return false;
  }

  return true;
}

bool FRenderResourceLibrary::CreateCubeMesh(FRenderer &Renderer) {
  FMeshDesc MeshDesc{
      .VertexData = CubeVertices,
      .VertexDataSize = static_cast<uint32>(sizeof(CubeVertices)),
      .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
      .VertexCount = static_cast<uint32>(std::size(CubeVertices)),
      .IndexData = CubeIndices,
      .IndexDataSize = static_cast<uint32>(sizeof(CubeIndices)),
      .IndexCount = static_cast<uint32>(std::size(CubeIndices)),
  };

  CubeMesh = RegisterMesh("Cube", Renderer.CreateMesh(MeshDesc));
  return CubeMesh != nullptr;
}

bool FRenderResourceLibrary::CreateCylinderMesh(FRenderer &Renderer,
                                                float Height, uint32 SliceCount,
                                                float TopRadius,
                                                float BottomRadius) {
  constexpr float TAU = std::numbers::pi_v<float> * 2.0f;
  const float DTheta = TAU / static_cast<float>(SliceCount);

  TArray<FVertexData> Vertices;
  TArray<uint32> Indices;

  Vertices.reserve(SliceCount * 4 + 2);
  Indices.reserve(SliceCount * 12);

  const float HalfH = Height * 0.5f;

  const uint32 TopCenterIndex = static_cast<uint32>(Vertices.size());
  Vertices.push_back({0.0f, HalfH, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, 0.5f,
                      0.0f, 1.0f, 0.0f});

  const uint32 TopRingStart = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({TopRadius * std::cos(Theta), HalfH,
                        TopRadius * std::sin(Theta), 0.0f, 0.0f, 1.0f, 1.0f,
                        0.5f + 0.5f * std::cos(Theta),
                        0.5f + 0.5f * std::sin(Theta), 0.0f, 1.0f, 0.0f});
  }

  const uint32 BottomCenterIndex = static_cast<uint32>(Vertices.size());
  Vertices.push_back({0.0f, -HalfH, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, 0.5f,
                      0.0f, -1.0f, 0.0f});

  const uint32 BottomRingStart = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({BottomRadius * std::cos(Theta), -HalfH,
                        BottomRadius * std::sin(Theta), 0.0f, 0.0f, 1.0f, 1.0f,
                        0.5f + 0.5f * std::cos(Theta),
                        0.5f + 0.5f * std::sin(Theta), 0.0f, -1.0f, 0.0f});
  }

  const uint32 SideTopStart = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({TopRadius * std::cos(Theta), HalfH,
                        TopRadius * std::sin(Theta), 0.0f, 0.0f, 1.0f, 1.0f,
                        static_cast<float>(i) / static_cast<float>(SliceCount),
                        0.0f, std::cos(Theta), 0.0f, std::sin(Theta)});
  }

  const uint32 SideBottomStart = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({BottomRadius * std::cos(Theta), -HalfH,
                        BottomRadius * std::sin(Theta), 0.0f, 0.0f, 1.0f, 1.0f,
                        static_cast<float>(i) / static_cast<float>(SliceCount),
                        1.0f, std::cos(Theta), 0.0f, std::sin(Theta)});
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    Indices.push_back(TopCenterIndex);
    Indices.push_back(TopRingStart + Next);
    Indices.push_back(TopRingStart + i);
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    Indices.push_back(BottomCenterIndex);
    Indices.push_back(BottomRingStart + i);
    Indices.push_back(BottomRingStart + Next);
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;

    const uint32 TL = SideTopStart + i;
    const uint32 TR = SideTopStart + Next;
    const uint32 BL = SideBottomStart + i;
    const uint32 BR = SideBottomStart + Next;

    Indices.push_back(BL);
    Indices.push_back(TL);
    Indices.push_back(BR);

    Indices.push_back(BR);
    Indices.push_back(TL);
    Indices.push_back(TR);
  }

  FMeshDesc MeshDesc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  CylinderMesh = RegisterMesh("Cylinder", Renderer.CreateMesh(MeshDesc));
  return CylinderMesh != nullptr;
}

bool FRenderResourceLibrary::CreateConeMesh(FRenderer &Renderer) {
  constexpr float BottomRadius = 0.5f;
  constexpr float Height = 1.0f;
  constexpr uint32 SliceCount = 24;
  constexpr float TAU = std::numbers::pi_v<float> * 2.0f;
  const float DTheta = TAU / static_cast<float>(SliceCount);

  TArray<FVertexData> Vertices;
  TArray<uint32> Indices;

  Vertices.reserve(SliceCount * 2 + 2);
  Indices.reserve(SliceCount * 6);

  const float HalfH = Height * 0.5f;

  const uint32 ApexIndex = static_cast<uint32>(Vertices.size());
  Vertices.push_back({0.0f, HalfH, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, 0.0f,
                      0.0f, 1.0f, 0.0f});

  const uint32 SideBaseStart = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({BottomRadius * std::cos(Theta), -HalfH,
                        BottomRadius * std::sin(Theta), 0.0f, 0.0f, 1.0f, 1.0f,
                        static_cast<float>(i) / static_cast<float>(SliceCount),
                        1.0f, std::cos(Theta), 0.0f, std::sin(Theta)});
  }

  const uint32 BottomCenterIndex = static_cast<uint32>(Vertices.size());
  Vertices.push_back({0.0f, -HalfH, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f, 0.5f,
                      0.0f, -1.0f, 0.0f});

  const uint32 BottomRingStart = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({BottomRadius * std::cos(Theta), -HalfH,
                        BottomRadius * std::sin(Theta), 0.0f, 0.0f, 1.0f, 1.0f,
                        0.5f + 0.5f * std::cos(Theta),
                        0.5f + 0.5f * std::sin(Theta), 0.0f, -1.0f, 0.0f});
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    Indices.push_back(ApexIndex);
    Indices.push_back(SideBaseStart + Next);
    Indices.push_back(SideBaseStart + i);
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    Indices.push_back(BottomCenterIndex);
    Indices.push_back(BottomRingStart + Next);
    Indices.push_back(BottomRingStart + i);
  }

  FMeshDesc MeshDesc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  ConeMesh = RegisterMesh("Cone", Renderer.CreateMesh(MeshDesc));
  return ConeMesh != nullptr;
}

bool FRenderResourceLibrary::CreateArrowMesh(FRenderer &Renderer) {
  constexpr uint32 SliceCount = 16u;
  constexpr float ShaftLength = 0.75f;
  constexpr float ShaftRadius = 0.025f;
  constexpr float HeadRadius = 0.075f;
  constexpr float HeadLength = 0.25f;
  constexpr float DTheta =
      2.0f * std::numbers::pi_v<float> / static_cast<float>(SliceCount);

  TArray<FVertexData> Vertices;
  TArray<uint32> Indices;

  Vertices.reserve(SliceCount * 5 + 3);
  Indices.reserve(SliceCount * 18);

  const uint32 ShaftBottomCenter = static_cast<uint32>(Vertices.size());
  Vertices.push_back({0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f,
                      -1.0f, 0.0f, 0.0f});

  const uint32 ShaftBottomRing = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({0.0f, ShaftRadius * std::cos(Theta),
                        ShaftRadius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, -1.0f, 0.0f, 0.0f});
  }

  const uint32 ShaftSideBottom = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({0.0f, ShaftRadius * std::cos(Theta),
                        ShaftRadius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 0.0f, std::cos(Theta), std::sin(Theta)});
  }

  const uint32 ShaftSideTop = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({ShaftLength, ShaftRadius * std::cos(Theta),
                        ShaftRadius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f,
                        1.0f, 0.0f, 0.0f, std::cos(Theta), std::sin(Theta)});
  }

  const uint32 HeadBaseCenter = static_cast<uint32>(Vertices.size());
  Vertices.push_back({ShaftLength, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f,
                      0.5f, -1.0f, 0.0f, 0.0f});

  const uint32 HeadBaseRing = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({ShaftLength, HeadRadius * std::cos(Theta),
                        HeadRadius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, -1.0f, 0.0f, 0.0f});
  }

  const uint32 HeadSideBase = static_cast<uint32>(Vertices.size());
  for (uint32 i = 0; i < SliceCount; ++i) {
    const float Theta = static_cast<float>(i) * DTheta;
    Vertices.push_back({ShaftLength, HeadRadius * std::cos(Theta),
                        HeadRadius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 0.0f, std::cos(Theta), std::sin(Theta)});
  }

  const uint32 HeadTip = static_cast<uint32>(Vertices.size());
  Vertices.push_back({ShaftLength + HeadLength, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                      1.0f, 1.0f, 0.5f, 1.0f, 0.0f, 0.0f});

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    Indices.push_back(ShaftBottomCenter);
    Indices.push_back(ShaftBottomRing + Next);
    Indices.push_back(ShaftBottomRing + i);
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    const uint32 BL = ShaftSideBottom + i;
    const uint32 BR = ShaftSideBottom + Next;
    const uint32 TL = ShaftSideTop + i;
    const uint32 TR = ShaftSideTop + Next;

    Indices.push_back(BL);
    Indices.push_back(TL);
    Indices.push_back(BR);

    Indices.push_back(BR);
    Indices.push_back(TL);
    Indices.push_back(TR);
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    Indices.push_back(HeadBaseCenter);
    Indices.push_back(HeadBaseRing + Next);
    Indices.push_back(HeadBaseRing + i);
  }

  for (uint32 i = 0; i < SliceCount; ++i) {
    const uint32 Next = (i + 1) % SliceCount;
    Indices.push_back(HeadSideBase + i);
    Indices.push_back(HeadTip);
    Indices.push_back(HeadSideBase + Next);
  }

  FMeshDesc MeshDesc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  ArrowMesh = RegisterMesh("Arrow", Renderer.CreateMesh(MeshDesc));
  return ArrowMesh != nullptr;
}

bool FRenderResourceLibrary::CreateCircleMesh(FRenderer &Renderer) {
  constexpr uint32 SliceCount = 32u;
  constexpr float Radius = 1.0f;
  constexpr float Width = 0.07f;

  TArray<FVertexData> Vertices;
  TArray<uint32> Indices;

  Vertices.reserve(SliceCount * 2);
  Indices.reserve(SliceCount * 6);

  for (uint32 i = 0; i < SliceCount; ++i) {
    float Theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) /
                  static_cast<float>(SliceCount);
    float InnerRadius = Radius - Width * 0.5f;
    float OuterRadius = Radius + Width * 0.5f;

    Vertices.push_back({0.0f, InnerRadius * std::cos(Theta),
                        InnerRadius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f,
                        0.0f, 0.0f, 1.0f, 0.0f, 0.0f});
    Vertices.push_back({0.0f, OuterRadius * std::cos(Theta),
                        OuterRadius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f,
                        1.0f, 1.0f, 1.0f, 0.0f, 0.0f});

    uint32 InnerCurrent = 2 * i;
    uint32 OuterCurrent = 2 * i + 1;
    uint32 InnerNext = (2 * (i + 1)) % (SliceCount * 2);
    uint32 OuterNext = (2 * (i + 1) + 1) % (SliceCount * 2);

    Indices.push_back(InnerCurrent);
    Indices.push_back(OuterCurrent);
    Indices.push_back(InnerNext);

    Indices.push_back(InnerNext);
    Indices.push_back(OuterCurrent);
    Indices.push_back(OuterNext);

    Indices.push_back(OuterCurrent);
    Indices.push_back(InnerCurrent);
    Indices.push_back(InnerNext);

    Indices.push_back(OuterCurrent);
    Indices.push_back(InnerNext);
    Indices.push_back(OuterNext);
  }

  const FMeshDesc Desc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  CircleMesh = RegisterMesh("Circle", Renderer.CreateMesh(Desc));
  return CircleMesh != nullptr;
}

bool FRenderResourceLibrary::CreateRotationGizmoMesh(FRenderer &Renderer) {
  constexpr uint32 SliceCount = 32u;
  constexpr float Radius = 1.0f;

  TArray<FVertexData> Vertices;
  TArray<uint32> Indices;

  Vertices.reserve(SliceCount * 2);
  Indices.reserve(SliceCount * 6);

  for (uint32 i = 0; i < SliceCount; ++i) {
    float Theta = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) /
                  static_cast<float>(SliceCount);

    Vertices.push_back({0.0f, Radius * std::cos(Theta),
                        Radius * std::sin(Theta), -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, -1.0f, 0.0f, 0.0f});
    Vertices.push_back({0.0f, Radius * std::cos(Theta),
                        Radius * std::sin(Theta), 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                        0.0f, 1.0f, 0.0f, 0.0f});

    if (i == 0)
      continue;

    Indices.push_back(2u * i - 2u);
    Indices.push_back(2u * i + 1u);
    Indices.push_back(2u * i - 1u);

    Indices.push_back(2u * i - 2u);
    Indices.push_back(2u * i);
    Indices.push_back(2u * i + 1u);

    Indices.push_back(2u * i - 2u);
    Indices.push_back(2u * i - 1u);
    Indices.push_back(2u * i + 1u);

    Indices.push_back(2u * i - 2u);
    Indices.push_back(2u * i);
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
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  RotationGizmoMesh = RegisterMesh("RotationGizmo", Renderer.CreateMesh(Desc));
  return RotationGizmoMesh != nullptr;
}

bool FRenderResourceLibrary::CreateSquareArrowMesh(FRenderer &Renderer) {
  constexpr float ShaftLength = 0.85f;
  constexpr float ShaftRadius = 0.025f;
  constexpr float ArrowLength = 1.0f;
  constexpr float TipSize = ArrowLength - ShaftLength;

  TArray<FVertexData> Vertices;
  TArray<uint32> Indices;

  Vertices.reserve(16u);
  Indices.reserve(72u);

  for (const auto &v : ColoredCubeVertices) {
    float ScaledX = v.x * ShaftLength;
    float ScaledY = v.y * ShaftRadius;
    float ScaledZ = v.z * ShaftRadius;
    Vertices.push_back({ScaledX + ShaftLength * 0.5f, ScaledY, ScaledZ, v.r,
                        v.g, v.b, v.a, v.u, v.v, v.nx, v.ny, v.nz});
  }

  for (const auto &Index : CubeIndices) {
    Indices.push_back(Index);
  }

  for (const auto &v : ColoredCubeVertices) {
    float ScaledX = v.x * TipSize;
    float ScaledY = v.y * TipSize;
    float ScaledZ = v.z * TipSize;
    Vertices.push_back({ScaledX + TipSize * 0.5f + ShaftLength, ScaledY,
                        ScaledZ, v.r, v.g, v.b, v.a, v.u, v.v, v.nx, v.ny,
                        v.nz});
  }

  for (const auto &Index : CubeIndices) {
    Indices.push_back(Index + 8u);
  }

  const FMeshDesc Desc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  SquareArrowMesh = RegisterMesh("SquareArrow", Renderer.CreateMesh(Desc));
  return SquareArrowMesh != nullptr;
}

bool FRenderResourceLibrary::CreateGridMesh(FRenderer &Renderer) {
  constexpr float HalfW = 10.0f;
  constexpr float HalfH = 10.0f;

  const TArray<FVertexData> Vertices = {
      {-HalfW, -HalfH, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
       1.0f},
      {HalfW, -HalfH, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
       1.0f},
      {HalfW, HalfH, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
       1.0f},
      {-HalfW, HalfH, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
       1.0f},
  };
  const TArray<uint32> Indices = {0, 1, 2, 0, 2, 3};

  FMeshDesc MeshDesc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = sizeof(FVertexData),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  GridMesh = RegisterMesh("Grid", Renderer.CreateMesh(MeshDesc));
  return GridMesh != nullptr;
}

bool FRenderResourceLibrary::CreateSphereMesh(FRenderer &Renderer) {
  auto Vertices = CreateSphereVertices(0.5f, 20, 20, false);

  FMeshDesc MeshDesc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = sizeof(FVertexData),
      .VertexCount = static_cast<uint32>(Vertices.size()),
  };

  SphereMesh = RegisterMesh("Sphere", Renderer.CreateMesh(MeshDesc));
  return SphereMesh != nullptr;
}

bool FRenderResourceLibrary::CreateLineMesh(FRenderer &Renderer) {
  FMeshDesc Desc{.VertexData = LineVertices,
                 .VertexDataSize = static_cast<uint32>(sizeof(LineVertices)),
                 .VertexStride = sizeof(FVertexData),
                 .VertexCount = static_cast<uint32>(std::size(LineVertices)),
                 .bIsLine = true};

  LineMesh = RegisterMesh("Line", Renderer.CreateMesh(Desc));
  return LineMesh != nullptr;
}

bool FRenderResourceLibrary::CreatePlaneMesh(FRenderer &Renderer) {
  FMeshDesc Desc{
      .VertexData = PlaneVertices,
      .VertexDataSize = static_cast<uint32>(sizeof(PlaneVertices)),
      .VertexStride = sizeof(FVertexData),
      .VertexCount = static_cast<uint32>(std::size(PlaneVertices)),
  };

  PlaneMesh = RegisterMesh("Plane", Renderer.CreateMesh(Desc));
  return PlaneMesh != nullptr;
}

bool FRenderResourceLibrary::CreateRectMesh(FRenderer &Renderer) {
  // 사각형 정점 배열
  const TArray<FVertexData> Vertices = {
      {0.0f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f},
  };

  // 양면 인덱스 배열
  const TArray<uint32> Indices = {
      0, 1, 2, 0, 2, 3,
      0, 2, 1, 0, 3, 2
  };

  FMeshDesc MeshDesc{
      .VertexData = Vertices.data(),
      .VertexDataSize =
          static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
      .VertexStride = sizeof(FVertexData),
      .VertexCount = static_cast<uint32>(Vertices.size()),
      .IndexData = Indices.data(),
      .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
      .IndexCount = static_cast<uint32>(Indices.size()),
  };

  // 사각형 메쉬 생성 및 등록
  RectMesh = RegisterMesh("Rect", Renderer.CreateMesh(MeshDesc));
  if (RectMesh) {
    AllMeshMap["Rectangle"] = RectMesh;
  }
  return RectMesh != nullptr;
}

bool FRenderResourceLibrary::CreateSimpleMaterial(FRenderer &Renderer) {
  FWString Path = GetExecutableDirectory();

  FMaterialDesc Desc = {
      .VertexShaderFileName = Path + L"/Shader/ExampleVS.cso",
      .PixelShaderFileName = Path + L"/Shader/ExamplePS.cso",
  };

  SimpleMaterial = RegisterMaterial("Simple", Renderer.CreateMaterial(Desc));

  if (SimpleMaterial) {
    SimpleMaterial->SetPipeLine(
        Renderer.GetPipeline(EBuiltinPipeline::Simple_Solid));
    return true;
  } else {
    return false;
  }
}

bool FRenderResourceLibrary::CreateTexturedMaterial(FRenderer &Renderer) {
  FWString Path = GetExecutableDirectory();

  FMaterialDesc Desc = {
      .VertexShaderFileName = Path + L"/Shader/ExampleVS.cso",
      .PixelShaderFileName = Path + L"/Shader/TexturedPS.cso",
  };

  TSharedPtr<FMaterial> Material =
      RegisterMaterial("Textured", Renderer.CreateMaterial(Desc));
  if (!Material) {
    return false;
  }

  TSharedPtr<FRenderPipeline> Pipeline =
      Renderer.GetPipeline(EBuiltinPipeline::Textured);
  if (!Pipeline) {
    // TexturedPS.cso가 없거나 파이프라인 생성이 실패한 경우
    return false;
  }
  Material->SetPipeLine(Pipeline);

  // CreateTextures가 먼저 돌아야 여기서 찾을 수 있다
  Material->SetTexture(GetTexture("sandclock"));

  return true;
}

bool FRenderResourceLibrary::CreateGridMaterial(FRenderer &Renderer) {
  FWString Path = GetExecutableDirectory();

  FMaterialDesc Desc = {
      .VertexShaderFileName = Path + L"/Shader/GridVS.cso",
      .PixelShaderFileName = Path + L"/Shader/GridPS.cso",
  };

  GridMaterial = RegisterMaterial("Grid", Renderer.CreateMaterial(Desc));

  if (GridMaterial) {
    GridMaterial->SetPipeLine(Renderer.GetPipeline(EBuiltinPipeline::Grid));
    return true;
  } else {
    return false;
  }
}

bool FRenderResourceLibrary::CreateRotationGizmoMaterial(FRenderer &Renderer) {
  FWString Path = GetExecutableDirectory();

  FMaterialDesc Desc = {
      .VertexShaderFileName = Path + L"/Shader/RotationGizmoVS.cso",
      .PixelShaderFileName = Path + L"/Shader/RotationGizmoPS.cso",
  };

  RotationGizmoMaterial = RegisterMaterial("RotationGizmo", Renderer.CreateMaterial(Desc));

  if (RotationGizmoMaterial) {
    RotationGizmoMaterial->SetPipeLine(
        Renderer.GetPipeline(EBuiltinPipeline::RotationGizmo));
    return true;
  } else {
    return false;
  }
}

bool FRenderResourceLibrary::CreateTextures(FRenderer& Renderer)
{
    const std::filesystem::path Root = std::filesystem::path(GetExecutableDirectory()) / L"Textures";
    if (!std::filesystem::exists(Root))
    {
        return true;   // 폴더가 없는 건 실패가 아님
    }

    for (const auto& Entry : std::filesystem::recursive_directory_iterator(Root))
    {
        if (!Entry.is_regular_file()) continue;

        FWString Ext = Entry.path().extension().wstring();
        std::transform(Ext.begin(), Ext.end(), Ext.begin(), ::towlower);
        if (Ext != L".png" && Ext != L".jpg") continue;

        // 확장자 제거는 stem()이 해줌
        FString KeyWide = Entry.path().stem().string();          // "icon"
        std::transform(KeyWide.begin(), KeyWide.end(), KeyWide.begin(), ::tolower);

        int W = 0, H = 0, ChannelsInFile = 0;
        unsigned char* Pixels = stbi_load(
            Entry.path().string().c_str(),   
            &W, &H, &ChannelsInFile, 4);   
        if (!Pixels) continue;

        FTextureDesc Desc{
            .PixelData = Pixels,
            .Width = static_cast<uint32>(W),
            .Height = static_cast<uint32>(H),
            .RowPitch = static_cast<uint32>(W) * 4u,
        };

        TSharedPtr<FTexture> Texture = Renderer.CreateTexture(Desc);
        stbi_image_free(Pixels);

        if (!Texture) continue;   // 실패한 텍스처는 맵에 넣지 않는다

        RegisterTexture(KeyWide, Texture);
    }

    return true;
}

bool FRenderResourceLibrary::CreateCommonMaterial(FRenderer& Renderer, FString& textureName)
{


    return false;
}

bool FRenderResourceLibrary::CreateTextMesh(FRenderer& Renderer)
{
    TArray<FVertexData> Vertices;
    TArray<uint32> Indices;
    FFont Font;
    Font.Initialize(16);
    FString Text{ "Welcome To Jungle" };

    // TODO: PlaneGenerator 만들어야 함.
    FTextVertex plane[4] =
    {
        { { 0.0f, -0.5f, 0.5f }, 0.0f, 0.0f },
        { { 0.0f, 0.5f, 0.5f }, 0.0f, 0.0f },
        { { 0.0f, -0.5f, -0.5f }, 0.0f, 0.0f },
        { { 0.0f, 0.5f, -0.5f }, 0.0f, 0.0f }
    };
    TArray<uint32> IndexSet = { 0, 1, 2, 1, 3, 2 };

    const float size = 1.0f;
    for (uint16 i = 0; i < Text.length(); ++i)
    {
        const FCharacterInfo& CharInfo = Font.GetCharInfo(Text.at(i));
        for (uint16 j = 0; j < 4; ++j)
        {	// ranged-for 로 수정?
            FVertexData tv;
            float sizeAmount = size * i;
            tv.x = plane[j].Pos.X;
            tv.y = plane[j].Pos.Y + sizeAmount;
            tv.z = plane[j].Pos.Z;

            bool bIsRight = (j == 1) || (j == 3);
            bool bIsBottom = (j == 2) || (j == 3);

            float width = (bIsRight) ? CharInfo.width : 0.0f;
            float height = (bIsBottom) ? CharInfo.height : 0.0f;
            tv.u = CharInfo.u + width;
            tv.v = CharInfo.v + height;
            Vertices.push_back(tv);
        }

        uint32 VertexOffset = i * 4;
        for (uint32 index : IndexSet)
        {
            Indices.push_back(index + VertexOffset);
        }
    }

    // 메시 빌드 추가하기
    FMeshDesc MeshData{
        .VertexData = Vertices.data(),
        .VertexDataSize = static_cast<uint32>(sizeof(FVertexData) * Vertices.size()),
        .VertexStride = sizeof(FVertexData),
        .VertexCount = static_cast<uint32>(Vertices.size()),
        .IndexData = Indices.data(),
        .IndexDataSize = static_cast<uint32>(sizeof(uint32) * Indices.size()),
        .IndexCount = static_cast<uint32>(Indices.size())
    };
    
    TextMesh = RegisterMesh("Text", Renderer.CreateMesh(MeshData));
    return TextMesh != nullptr;
}

bool FRenderResourceLibrary::CreateTextMaterial(FRenderer& Renderer)
{
    FWString Path = GetExecutableDirectory();

    FMaterialDesc Desc = {
        .VertexShaderFileName = Path + L"/Shader/ExampleVS.cso",
        .PixelShaderFileName = Path + L"/Shader/TextPS.cso",
    };

    TSharedPtr<FMaterial> Material =
        RegisterMaterial("Text", Renderer.CreateMaterial(Desc));
    if (!Material) {
        return false;
    }

    TSharedPtr<FRenderPipeline> Pipeline =
        Renderer.GetPipeline(EBuiltinPipeline::Textured);
    if (!Pipeline) {
        // TexturedPS.cso가 없거나 파이프라인 생성이 실패한 경우
        return false;
    }
    Material->SetPipeLine(Pipeline);

    // CreateTextures가 먼저 돌아야 여기서 찾을 수 있다
    Material->SetTexture(GetTexture("sandclock"));

    return true;
}



