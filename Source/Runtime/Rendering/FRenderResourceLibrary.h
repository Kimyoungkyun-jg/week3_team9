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


enum class EPipelineID : uint8 {
  Simple_Solid,
  Simple_Wireframe,
  Textured,
  Grid,
  RotationGizmo,
  Count,
  Spotlight,
  Text,
  Instance_Text,
};

enum class EMaterialID : uint8
{
    Simple,
    Grid,
    RotGizmo,
    Spotlight,
    Text,
    Textured,
    Instance_Text,
};

class FRenderResourceLibrary final {
public:
  // 전역 싱글톤 접근자
  static FRenderResourceLibrary& Get();

  bool Initialize(FRenderer &Renderer);

  // 파이프라인 보관 맵
  TMap<EPipelineID, TSharedPtr<FRenderPipeline>> AllPipelineMap;
  // 메쉬 보관 맵
  TMap<FString, TSharedPtr<FMesh>> AllMeshMap;
  // 머티리얼 보관 맵 (ID 기반)
  TMap<EMaterialID, TSharedPtr<FMaterial>> AllMaterialMap;
  // 텍스쳐 보관 맵
  TMap<FString, TSharedPtr<FTexture>> AllTextureMap;

  // 파이프라인 조회
  [[nodiscard]] TSharedPtr<FRenderPipeline> GetPipeline(EPipelineID Id) const {
    auto it = AllPipelineMap.find(Id);
    if (it != AllPipelineMap.end())
      return it->second;
    return nullptr;
  }

  // 머티리얼 조회 (ID 기반 전용)
  [[nodiscard]] TSharedPtr<FMaterial> GetMaterial(EMaterialID Id) const {
    auto it = AllMaterialMap.find(Id);
    if (it != AllMaterialMap.end())
      return it->second;
    return nullptr;
  }

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
  [[nodiscard]] TSharedPtr<FMesh> GetCubeMesh() const { return GetMesh("Cube"); }
  [[nodiscard]] TSharedPtr<FMesh> GetCylinderMesh() const { return GetMesh("Cylinder"); }
  [[nodiscard]] TSharedPtr<FMesh> GetConeMesh() const { return GetMesh("Cone"); }
  [[nodiscard]] TSharedPtr<FMesh> GetSpotlightConeMesh() const { return GetMesh("SpotlightCone"); }
  [[nodiscard]] TSharedPtr<FMesh> GetArrowMesh() const { return GetMesh("Arrow"); }
  [[nodiscard]] TSharedPtr<FMesh> GetCircleMesh() const { return GetMesh("Circle"); }
  [[nodiscard]] TSharedPtr<FMesh> GetRotationGizmoMesh() const { return GetMesh("RotationGizmo"); }
  [[nodiscard]] TSharedPtr<FMesh> GetSquareArrowMesh() const { return GetMesh("SquareArrow"); }
  [[nodiscard]] TSharedPtr<FMesh> GetGridMesh() const { return GetMesh("Grid"); }
  [[nodiscard]] TSharedPtr<FMesh> GetSphereMesh() const { return GetMesh("Sphere"); }
  [[nodiscard]] TSharedPtr<FMesh> GetLineMesh() const { return GetMesh("Line"); }
  [[nodiscard]] TSharedPtr<FMesh> GetPlaneMesh() const { return GetMesh("Plane"); }
  [[nodiscard]] TSharedPtr<FMesh> GetRectMesh() const { return GetMesh("Rect"); }
  [[nodiscard]] TSharedPtr<FMesh> GetTextMesh() const { return GetMesh("Text"); }

  // 머티리얼 등록 (ID 기반 전용)
  TSharedPtr<FMaterial> RegisterMaterial(EMaterialID Id,
                                         TSharedPtr<FMaterial> inMaterial) {
    AllMaterialMap[Id] = inMaterial;
    return inMaterial;
  }

  void RegisterTexture(const FString &name, TSharedPtr<FTexture> texture) {
    AllTextureMap[name] = texture;
  }

  // 텍스처 조회. 없으면 nullptr
  [[nodiscard]] TSharedPtr<FTexture> GetTexture(const FString &name) const {
    auto it = AllTextureMap.find(name);
    if (it != AllTextureMap.end())
      return it->second;
    return nullptr;
  }

  // 메쉬 전체 해제
  void DestroyAllMeshes() {
    AllMeshMap.clear();
  }

  // 머티리얼 전체 해제
  void DestroyAllMaterials() {
    AllMaterialMap.clear();
  }

  // 파이프라인 전체 해제
  void DestroyAllPipelines() {
    AllPipelineMap.clear();
  }

  // 전체 머티리얼 맵 조회
  const TMap<EMaterialID, TSharedPtr<FMaterial>> &GetAllMaterials() const {
    return AllMaterialMap;
  }

  // 렌더러 참조 조회
  FRenderer *GetRenderer() const { return RendererRef; }

  // 정점 배열 메쉬 캐싱 생성
  TSharedPtr<FMesh> GetOrCreateMesh(const FString &name,
                                    const TArray<FVertexData> &vertices);




private:
  bool InitializePipelines(FRenderer &Renderer);
  bool CreateSolidWireframePipeline(FRenderer &Renderer);

  bool CreateCubeMesh(FRenderer &Renderer);
  bool CreateCylinderMesh(FRenderer &Renderer, float Height, uint32 SliceCount,
                          float TopRadius, float BottomRadius);
  bool CreateConeMesh(FRenderer &Renderer);
  bool CreateSpotlightConeMesh(FRenderer &Renderer);
  bool CreateArrowMesh(FRenderer &Renderer);
  bool CreateCircleMesh(FRenderer &Renderer);
  bool CreateRotationGizmoMesh(FRenderer &Renderer);
  bool CreateSquareArrowMesh(FRenderer &Renderer);
  bool CreateGridMesh(FRenderer &Renderer);
  bool CreateSphereMesh(FRenderer &Renderer);
  bool CreateLineMesh(FRenderer &Renderer);
  bool CreatePlaneMesh(FRenderer &Renderer);
  bool CreateRectMesh(FRenderer &Renderer);

  // 텍스처 및 머티리얼 일괄 초기화
  bool CreateTextures(FRenderer &Renderer);
  bool InitializeMaterials(FRenderer &Renderer);

  // Text
  bool CreateTextMesh(FRenderer& Renderer);

  FRenderer *RendererRef = nullptr;
};