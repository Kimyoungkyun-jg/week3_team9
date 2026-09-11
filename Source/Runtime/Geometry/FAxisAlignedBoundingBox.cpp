#include "FAxisAlignedBoundingBox.h"
#include "Runtime/Rendering/FMesh.h"

#include <algorithm>

FAxisAlignedBoundingBox::FAxisAlignedBoundingBox(const FMesh& Mesh)
{	
	for (auto& Item : Mesh.GetPositions())
	{
		for (int i = 0; i < 3; ++i)
		{
			Min[i] = std::min(Item[i], Min[i]);
			Max[i] = std::max(Item[i], Max[i]);
		}
	}
}