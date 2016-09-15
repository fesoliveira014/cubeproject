#include "../Common.h"

#include "Ray.h"

#include "../volume/Voxel.h"
#include "../volume/Chunk.h"

namespace tactical
{
	using ChunkMap = std::unordered_map < glm::vec3, std::shared_ptr<volume::Chunk>,
			std::hash<glm::vec3>, std::equal_to<glm::vec3 >>;

	namespace math
	{
		// pick chunk for voxel picking
		inline static RayCastResult PickVoxel(Ray& ray, ChunkMap& map, int maxDepth, int chunkSize)
		{
			math::RayCastResult result;
			result.pos = glm::vec3(0.0f);
			result.hit = false;

			auto World2Chunk = [](const glm::vec3& pos, int chunkSize) {
				glm::vec3 offset(glm::vec3(glm::fmod(pos, (float)chunkSize)));
				glm::vec3 position = pos - offset;
				position = glm::vec3(position.x / chunkSize, position.y / chunkSize, position.z / chunkSize);

				return position;
			};

			auto World2Voxel = [](const glm::vec3& pos, int chunkSize) {
				return glm::vec3(glm::fmod(pos, (float)chunkSize));
			};

			auto GetVoxel = [World2Chunk, World2Voxel](const glm::vec3& pos, ChunkMap& map, int chunkSize) {
				// find chunk index associated with position
				glm::vec3 chunkIndex = World2Chunk(pos, chunkSize);

				// check if chunk exists
				if (map.find(chunkIndex) != map.end()) {
					// find voxel position on chunk coordinates
					glm::vec3 voxelPos = World2Voxel(pos, chunkSize);
					return map[chunkIndex]->GetVoxel(voxelPos);
				}

				return byte(0);
			};

			glm::vec3 tmax;
			glm::vec3 tdelta;
			glm::vec3 currentBoundary;
			glm::vec3 size(1.0f);

			glm::vec3 step(0);
			
			glm::vec3 origin = glm::floor(ray.GetOrigin() + 0.5f); // ?

			step.x = ray.GetDirection().x > 0.0f ? 1.0f : -1.0f;
			step.y = ray.GetDirection().y > 0.0f ? 1.0f : -1.0f;
			step.z = ray.GetDirection().z > 0.0f ? 1.0f : -1.0f;

			tdelta.x = glm::abs(size.x * ray.GetInverse().x);
			tdelta.y = glm::abs(size.y * ray.GetInverse().y);
			tdelta.z = glm::abs(size.z * ray.GetInverse().z);

			currentBoundary.x = step.x > 0 ? origin.x + 1 : origin.x;
			currentBoundary.y = step.y > 0 ? origin.y + 1 : origin.y;
			currentBoundary.z = step.z > 0 ? origin.z + 1 : origin.z;

			tmax.x = ray.GetDirection().x != 0 ? (currentBoundary.x - origin.x) * ray.GetInverse().x : std::numeric_limits<float>::infinity();
			tmax.y = ray.GetDirection().y != 0 ? (currentBoundary.y - origin.y) * ray.GetInverse().y : std::numeric_limits<float>::infinity();
			tmax.z = ray.GetDirection().z != 0 ? (currentBoundary.z - origin.z) * ray.GetInverse().z : std::numeric_limits<float>::infinity();

			result.pos = origin;

			// you are inside a voxel lol (maybe in water this could happen?)
			if (GetVoxel(result.pos, map, chunkSize) != 0) {
				result.hit = true;
				return result;
			}

			for (int i = 0; i < maxDepth; ++i) {
				if (tmax.x < tmax.y && tmax.x < tmax.z) {
					origin.x += step.x;
					tmax.x += tdelta.x;
					result.face[0] = -step.x;
					result.face[1] = 0;
					result.face[2] = 0;
				}
				else if (tmax.y < tmax.z) {
					origin.y += step.y;
					tmax.y += tdelta.y;
					result.face[0] = 0;
					result.face[1] = -step.y;
					result.face[2] = 0;
				}
				else {
					origin.z += step.z;
					tmax.z += tdelta.z;
					result.face[0] = 0;
					result.face[1] = 0;
					result.face[2] = -step.z;
				}

				result.pos = origin;
				if (GetVoxel(result.pos, map, chunkSize) != 0) {
					result.hit = true;
					return result;
				}
			}
			result.hit = false;
			return result;
		}
	}
}