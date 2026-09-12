#pragma once

#include <d3d11.h>
#include "Runtime/Math/FVector.h"
#include <iterator>

// 공용 정점 구조체
struct FVertexData
{
	float x = 0.0f, y = 0.0f, z = 0.0f;          // Position
	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f; // Color
	float u = 0.0f, v = 0.0f;                      // UV
	float nx = 0.0f, ny = 0.0f, nz = 0.0f;         // Normal
};

// 공용 Direct3D 입력 레이아웃 메타데이터
struct FVertexLayouts
{
	static inline const D3D11_INPUT_ELEMENT_DESC Layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static constexpr UINT NumElements = 4;
};

// 큐브 정점 배열
inline const FVertexData CubeVertices[] = {
	// 앞면
	{  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f },
	{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f },
	{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f },
	{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f },

	// 뒷면
	{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f },
	{ -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f },
	{ -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f },
	{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f },

	// 우측면
	{ -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f },
	{  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f },
	{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f },
	{ -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f },

	// 좌측면
	{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f },
	{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f },
	{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f },
	{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f },

	// 윗면
	{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f },
	{  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f },
	{ -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f },
	{ -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f },

	// 밑면
	{ -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f },
	{ -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f },
	{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f },
	{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f },
};

// 큐브 인덱스 배열
inline const unsigned int CubeIndices[] = {
	0, 1, 2, 0, 2, 3,
	4, 5, 6, 4, 6, 7,
	8, 9, 10, 8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23,
};

// 색상 큐브 정점 배열
inline const FVertexData ColoredCubeVertices[] = {
	{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f },
	{  0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
	{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  1.0f },
	{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  1.0f },
	{  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f },
	{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f },
};

// 색상 큐브 인덱스 배열
inline const unsigned int ColoredCubeIndices[] = {
	0, 2, 1, 0, 3, 2,
	4, 5, 6, 4, 6, 7,
	0, 1, 5, 0, 5, 4,
	3, 7, 6, 3, 6, 2,
	0, 4, 7, 0, 7, 3,
	1, 2, 6, 1, 6, 5,
};

// 선 정점 배열
inline const FVertexData LineVertices[] = {
	{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f },
};

// 평면 정점 배열
inline const FVertexData PlaneVertices[] = {
	// pos              color                 uv          normal
	{ -0.5f, -0.5f, 0.0f,  1,1,1,1,  0.0f, 1.0f,  0,0,1 },  // 좌하
	{  0.5f, -0.5f, 0.0f,  1,1,1,1,  1.0f, 1.0f,  0,0,1 },  // 우하
	{  0.5f,  0.5f, 0.0f,  1,1,1,1,  1.0f, 0.0f,  0,0,1 },  // 우상
	{ -0.5f,  0.5f, 0.0f,  1,1,1,1,  0.0f, 0.0f,  0,0,1 },  // 좌상
};

