#pragma once

#include "FMaterial.h"
#include "FMesh.h"
#include "FRenderPipeline.h"
#include "Runtime/Core/FString.h"
#include "Runtime/Core/PointerTypes.h"
#include "Runtime/Core/TArray.h"
#include "Runtime/Core/TMap.h"
#include "Vertices.h"
#include "FFont.h"


class FRenderer;
class FTexture;
struct FTextVertex
{
    FVector Pos;
    float u, v;
};


class FRenderResourceLibrary final {
public:
  bool Initialize(FRenderer &Renderer);

  // 메쉬 보관 맵
  TMap<FString, TSharedPtr<FMesh>> AllMeshMap;
  // 머티리얼 보관 맵
  TMap<FString, TSharedPtr<FMaterial>> AllMaterialMap;
  //텍스쳐 보관 맵
  TMap<FString, TSharedPtr<FTexture>> AllTextureMap;

  // 메쉬 조회
  TSharedPtr<FMesh> GetMesh(const FString &name) const {
    auto it = AllMeshMap.find(name);
    if (it != AllMeshMap.end())
      return it->second;
    return nullptr;
  }

  // 메쉬 등록
  TSharedPtr<FMesh> RegisterMesh(const FString &name,
                                 TSharedPtr<FMesh> inMesh) {
    AllMeshMap[name] = inMesh;
    return inMesh;
  }

  // 개별 메쉬 접근자
  [[nodiscard]] TSharedPtr<FMesh> GetCubeMesh() const { return CubeMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetCylinderMesh() const { return CylinderMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetConeMesh() const { return ConeMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetArrowMesh() const { return ArrowMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetCircleMesh() const { return CircleMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetRotationGizmoMesh() const { return RotationGizmoMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetSquareArrowMesh() const { return SquareArrowMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetGridMesh() const { return GridMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetSphereMesh() const { return SphereMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetLineMesh() const { return LineMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetPlaneMesh() const { return PlaneMesh; }
  [[nodiscard]] TSharedPtr<FMesh> GetRectMesh() const { return RectMesh; }

  // 머티리얼 조회
  TSharedPtr<FMaterial> GetMaterial(const FString &name) const {
    auto it = AllMaterialMap.find(name);
    if (it != AllMaterialMap.end())
      return it->second;
    return nullptr;
  }

  // 머티리얼 등록
  TSharedPtr<FMaterial> RegisterMaterial(const FString &name,
                                         TSharedPtr<FMaterial> inMaterial) {
    AllMaterialMap[name] = inMaterial;
    return inMaterial;
  }

  void RegisterTexture(const FString& name, TSharedPtr<FTexture> texture)
  {
      AllTextureMap[name] = texture;
  }

  // 텍스처 조회. 없으면 nullptr
  [[nodiscard]] TSharedPtr<FTexture> GetTexture(const FString& name) const
  {
      auto it = AllTextureMap.find(name);
      if (it != AllTextureMap.end())
          return it->second;
      return nullptr;
  }

  // 개별 머티리얼 접근자
  [[nodiscard]] TSharedPtr<FMaterial> GetSimpleMaterial() const { return SimpleMaterial; }
  [[nodiscard]] TSharedPtr<FMaterial> GetGridMaterial() const { return GridMaterial; }
  [[nodiscard]] TSharedPtr<FMaterial> GetRotationGizmoMaterial() const { return RotationGizmoMaterial; }

  // 메쉬 전체 해제
  void DestroyAllMeshes() {
    AllMeshMap.clear();
    CubeMesh.reset();
    CylinderMesh.reset();
    ConeMesh.reset();
    ArrowMesh.reset();
    CircleMesh.reset();
    RotationGizmoMesh.reset();
    SquareArrowMesh.reset();
    GridMesh.reset();
    SphereMesh.reset();
    LineMesh.reset();
    PlaneMesh.reset();
    RectMesh.reset();
    TextMesh.reset();
  }

  // 머티리얼 전체 해제
  void DestroyAllMaterials() {
    AllMaterialMap.clear();
    SimpleMaterial.reset();
    GridMaterial.reset();
    RotationGizmoMaterial.reset();
  }

  // 전체 머티리얼 맵 조회
  const TMap<FString, TSharedPtr<FMaterial>> &GetAllMaterials() const {
    return AllMaterialMap;
  }

  // 렌더러 참조 조회
  FRenderer *GetRenderer() const { return RendererRef; }

  // 등록된 모든 머티리얼 이름 목록 반환
  TArray<FString> GetMaterialNames() const {
    TArray<FString> Names;
    for (const auto &Pair : AllMaterialMap) {
      Names.push_back(Pair.first);
    }
    return Names;
  }

  // 셰이더 파일 경로로 머티리얼 생성 및 맵에 등록
  TSharedPtr<FMaterial> CreateAndRegisterMaterial(const FString &Name,
                                                  const FWString &VsPath,
                                                  const FWString &PsPath);

  // 정점 배열 메쉬 캐싱 생성
  TSharedPtr<FMesh> GetOrCreateMesh(const FString &name,
                                    const TArray<FVertexData> &vertices);

private:
  bool CreateCubeMesh(FRenderer &Renderer);
  bool CreateCylinderMesh(FRenderer &Renderer, float Height, uint32 SliceCount,
                          float TopRadius, float BottomRadius);
  bool CreateConeMesh(FRenderer &Renderer);
  bool CreateArrowMesh(FRenderer &Renderer);
  bool CreateCircleMesh(FRenderer &Renderer);
  bool CreateRotationGizmoMesh(FRenderer &Renderer);
  bool CreateSquareArrowMesh(FRenderer &Renderer);
  bool CreateGridMesh(FRenderer &Renderer);
  bool CreateSphereMesh(FRenderer &Renderer);
  bool CreateLineMesh(FRenderer &Renderer);
  bool CreatePlaneMesh(FRenderer &Renderer);
  bool CreateRectMesh(FRenderer &Renderer);


  bool CreateSimpleMaterial(FRenderer &Renderer);
  bool CreateGridMaterial(FRenderer &Renderer);
  bool CreateRotationGizmoMaterial(FRenderer &Renderer);
  //시작시 1번만 호출
  bool CreateTextures(FRenderer& Renderer);
  // 텍스처를 샘플링하는 머티리얼. CreateTextures 이후에 호출해야 함
  bool CreateTexturedMaterial(FRenderer& Renderer);
  bool CreateCommonMaterial(FRenderer& Renderer, FString& textureName);

  // Text
  bool CreateTextMesh(FRenderer& Renderer);
  bool CreateTextMaterial(FRenderer& Renderer);
  // Kor
  bool CreateKorTextMesh(FRenderer& Renderer);
  bool CreateKorTextMaterial(FRenderer& Renderer);

  // 개별 리소스 멤버 변수
  TSharedPtr<FMesh> CubeMesh;
  TSharedPtr<FMesh> CylinderMesh;
  TSharedPtr<FMesh> ConeMesh;
  TSharedPtr<FMesh> ArrowMesh;
  TSharedPtr<FMesh> CircleMesh;
  TSharedPtr<FMesh> RotationGizmoMesh;
  TSharedPtr<FMesh> SquareArrowMesh;
  TSharedPtr<FMesh> GridMesh;
  TSharedPtr<FMesh> SphereMesh;
  TSharedPtr<FMesh> LineMesh;
  TSharedPtr<FMesh> PlaneMesh;
  TSharedPtr<FMesh> RectMesh;
  TSharedPtr<FMesh> TextMesh;
  TSharedPtr<FMesh> KorTextMesh;

  TSharedPtr<FMaterial> SimpleMaterial;
  TSharedPtr<FMaterial> GridMaterial;
  TSharedPtr<FMaterial> RotationGizmoMaterial;

  FRenderer *RendererRef = nullptr;
};

#include "FRenderer.h"

inline TSharedPtr<FMesh>
FRenderResourceLibrary::GetOrCreateMesh(const FString &name,
                                        const TArray<FVertexData> &vertices) {
  auto it = AllMeshMap.find(name);
  if (it != AllMeshMap.end())
    return it->second;

  FMeshDesc Desc{.VertexData = vertices.data(),
                 .VertexDataSize =
                     static_cast<uint32>(sizeof(FVertexData) * vertices.size()),
                 .VertexStride = static_cast<uint32>(sizeof(FVertexData)),
                 .VertexCount = static_cast<uint32>(vertices.size())};
  TSharedPtr<FMesh> newMesh =
      RendererRef ? RendererRef->CreateMesh(Desc) : nullptr;
  if (newMesh) {
    AllMeshMap[name] = newMesh;
  }
  return newMesh;
}

inline TSharedPtr<FMaterial> FRenderResourceLibrary::CreateAndRegisterMaterial(
    const FString &Name, const FWString &VsPath, const FWString &PsPath) {
  auto it = AllMaterialMap.find(Name);
  if (it != AllMaterialMap.end())
    return it->second;

  if (!RendererRef)
    return nullptr;

  FMaterialDesc Desc = {
      .VertexShaderFileName = VsPath,
      .PixelShaderFileName = PsPath,
  };

  TSharedPtr<FMaterial> NewMat = RendererRef->CreateMaterial(Desc);
  if (NewMat) {
    AllMaterialMap[Name] = NewMat;
  }
  return NewMat;
}