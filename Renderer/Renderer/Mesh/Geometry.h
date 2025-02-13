#pragma once

#include <Renderer/Mesh/Scene.h>

namespace GLEngine::Renderer::MeshData {

class C_Geometry {
public:
	[[nodiscard]] static Mesh CreatePlane(const float size = 2.f, const int subdivisions = 0)
	{
		GLE_ASSERT(subdivisions >= 0, "Can't subdivide by negative number.");
		Mesh mesh;
		mesh.vertices.reserve(6 * static_cast<std::size_t>(std::pow(4.0, static_cast<double>(subdivisions))));

		if (subdivisions == 0)
		{
			AddSquare(mesh, size, 0, 0, subdivisions + 1);
		}
		else
		{
			const auto squareSize = size / subdivisions;
			for (int i = 0; i < subdivisions; ++i)
			{
				for (int j = 0; j < subdivisions; ++j)
				{
					AddSquare(mesh, squareSize, i, j, subdivisions);
				}
			}
		}

		mesh.bitangent.resize(mesh.vertices.size());
		mesh.tangent.resize(mesh.vertices.size());
		std::fill(mesh.bitangent.begin(), mesh.bitangent.end(), glm::vec3(1.f, 0.f, 0.f));
		std::fill(mesh.tangent.begin(), mesh.tangent.end(), glm::vec3(0.f, 0.f, 1.f));

		mesh.bbox.Add(glm::vec3(-1.0f, 0, -1.f));
		mesh.bbox.Add(glm::vec3(1.0f, 0, -1.f));
		mesh.bbox.Add(glm::vec3(-1.0f, 0, 1.f));
		mesh.bbox.Add(glm::vec3(1.0f, 0, 1.f));

		return mesh;
	}

	[[nodiscard]] static Mesh CreateOctahedron(float height, float width)
	{
		Mesh mesh;
		mesh.vertices.reserve(3 * 4);
		const glm::vec3 top(0, height, 0);
		const glm::vec3 bottom(0, -height, 0);

		const auto getNormalBinromal = [&]() {
			auto	   it	  = mesh.vertices.rbegin();
			const auto A	  = glm::vec3(*(it));
			const auto B	  = glm::vec3(*(++it));
			const auto C	  = glm::vec3(*(++it));
			const auto tang	  = glm::normalize(C - A);
			const auto biTang = B - A;
			const auto normal = glm::normalize(glm::cross(tang, biTang));
			mesh.normals.push_back(normal);
			mesh.normals.push_back(normal);
			mesh.normals.push_back(normal);
			mesh.tangent.push_back(tang);
			mesh.tangent.push_back(tang);
			mesh.tangent.push_back(tang);
			const auto realBitang = glm::cross(normal, tang);
			mesh.bitangent.push_back(realBitang);
		};
		mesh.vertices.emplace_back(top);
		mesh.vertices.emplace_back(width, 0, -width);
		mesh.vertices.emplace_back(-width, 0, -width);
		getNormalBinromal();

		mesh.vertices.emplace_back(top);
		mesh.vertices.emplace_back(width, 0, width);
		mesh.vertices.emplace_back(width, 0, -width);
		getNormalBinromal();

		mesh.vertices.emplace_back(top);
		mesh.vertices.emplace_back(-width, 0, width);
		mesh.vertices.emplace_back(width, 0, width);
		getNormalBinromal();

		mesh.vertices.emplace_back(top);
		mesh.vertices.emplace_back(-width, 0, -width);
		mesh.vertices.emplace_back(-width, 0, width);
		getNormalBinromal();

		mesh.vertices.emplace_back(bottom);
		mesh.vertices.emplace_back(-width, 0, -width);
		mesh.vertices.emplace_back(width, 0, -width);
		getNormalBinromal();

		mesh.vertices.emplace_back(bottom);
		mesh.vertices.emplace_back(width, 0, width);
		mesh.vertices.emplace_back(-width, 0, width);
		getNormalBinromal();

		mesh.vertices.emplace_back(bottom);
		mesh.vertices.emplace_back(width, 0, -width);
		mesh.vertices.emplace_back(width, 0, width);
		getNormalBinromal();

		mesh.vertices.emplace_back(bottom);
		mesh.vertices.emplace_back(-width, 0, width);
		mesh.vertices.emplace_back(-width, 0, -width);
		getNormalBinromal();

		for (std::size_t i = 0; i < mesh.vertices.size(); ++i)
		{
			mesh.texcoords.emplace_back(1, 0);
		}

		return mesh;
	}

private:
	static void AddSquare(Mesh& mesh, const float size, const int xPos, const int yPos, const int subdivisions)
	{
		GLE_ASSERT(size > 0.0, "Negative square size not supported");
		const float divisionSize = 1.f / subdivisions;
		const float halfSize = size / 2.f;
		const auto leftEdge	  = -halfSize + (size * xPos);
		const auto rightEdge  = leftEdge + size;
		const auto topEdge	  = halfSize - (size * yPos);
		const auto bottomEdge = topEdge - size;
		mesh.vertices.emplace_back(rightEdge, 0, topEdge);	  // 3
		mesh.vertices.emplace_back(leftEdge, 0, bottomEdge);  // 2
		mesh.vertices.emplace_back(leftEdge, 0, topEdge);	  // 1
		mesh.vertices.emplace_back(rightEdge, 0, topEdge);	  // 6 = 3
		mesh.vertices.emplace_back(rightEdge, 0, bottomEdge); // 5
		mesh.vertices.emplace_back(leftEdge, 0, bottomEdge);  // 4 = 2

		// I am drawing square [-1,1][-1,1] so I need to divide size by 2
		// clamp is needed due to float imprecision
		const auto	uvLeftEdge	 = std::clamp(0.0 + divisionSize * xPos, 0.0, 1.0);
		const auto	uvRightEdge	 = std::clamp(uvLeftEdge + divisionSize, 0.0, 1.0);
		const auto	uvTopEdge	 = std::clamp(1.0 - (divisionSize * yPos), 0.0, 1.0);
		const auto	uvBottomEdge = std::clamp(uvTopEdge - divisionSize, 0.0, 1.0);

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

} // namespace GLEngine::Renderer::MeshData
