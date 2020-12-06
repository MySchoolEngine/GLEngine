#pragma once

#include <Renderer/Mesh/Scene.h>


namespace GLEngine::Renderer::MeshData {

class C_Geometry
{
public:
	[[nodiscard]] static Mesh CreatePlane(int subdivisions = 0)
	{
		GLE_ASSERT(subdivisions >= 0, "Can't subdivide by negative number.");
		Mesh mesh;
		mesh.vertices.reserve(6 * static_cast<std::size_t>(std::pow(4.0, static_cast<double>(subdivisions))));
		
		if (subdivisions == 0)
		{
			AddSquare(mesh, 2.0, 0, 0);
		}
		else
		{
			const auto squareSize = 2.0f / subdivisions;
			for (int i = 0; i < subdivisions; ++i)
			{
				for (int j = 0; j < subdivisions; ++j)
				{
					AddSquare(mesh, squareSize, i, j);
				}
			}
		}

		return mesh;
	}

private:
	static void AddSquare(Mesh& mesh, float size, int xPos, int yPos)
	{
		GLE_ASSERT(size > 0.0, "Negative square size not supported");
		const auto leftEdge = -1.0f + (size * xPos);
		const auto rightEdge = leftEdge + size;
		const auto topEdge = 1.0f - (size * yPos);
		const auto bottomEdge = topEdge - size;
		mesh.vertices.emplace_back(rightEdge, 0, topEdge, 1); // 3
		mesh.vertices.emplace_back(leftEdge, 0, bottomEdge, 1); // 2
		mesh.vertices.emplace_back(leftEdge, 0, topEdge, 1); // 1
		mesh.vertices.emplace_back(rightEdge, 0, topEdge, 1); // 6 = 3
		mesh.vertices.emplace_back(rightEdge, 0, bottomEdge, 1); // 5
		mesh.vertices.emplace_back(leftEdge, 0, bottomEdge, 1); // 4 = 2

		// I am drawing square [-1,1][-1,1] so I need to divide size by 2
		// clamp is needed due to float imprecision
		const auto uvLeftEdge = std::clamp(0.0 + (size/2.0) * xPos, 0.0, 1.0);
		const auto uvRightEdge = std::clamp(uvLeftEdge + (size / 2.0), 0.0, 1.0);
		const auto uvTopEdge = std::clamp(1.0f - (size / 2.0) * yPos, 0.0, 1.0);
		const auto uvBottomEdge = std::clamp(uvTopEdge - (size / 2.0), 0.0, 1.0);

		mesh.texcoords.emplace_back(uvRightEdge, uvTopEdge);
		mesh.texcoords.emplace_back(uvLeftEdge, uvBottomEdge);
		mesh.texcoords.emplace_back(uvLeftEdge, uvTopEdge);
		mesh.texcoords.emplace_back(uvRightEdge, uvTopEdge);
		mesh.texcoords.emplace_back(uvRightEdge, uvBottomEdge);
		mesh.texcoords.emplace_back(uvLeftEdge, uvBottomEdge);


		mesh.normals.reserve(6);
		mesh.normals.emplace_back(0, 1, 0);
		mesh.normals.emplace_back(0, 1, 0);
		mesh.normals.emplace_back(0, 1, 0);
		mesh.normals.emplace_back(0, 1, 0);
		mesh.normals.emplace_back(0, 1, 0);
		mesh.normals.emplace_back(0, 1, 0);
	}
};

}
