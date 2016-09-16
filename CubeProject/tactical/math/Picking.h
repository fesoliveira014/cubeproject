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
			
			glm::vec3 origin(std::floorf(ray.GetOrigin().x), std::floorf(ray.GetOrigin().y), std::floorf(ray.GetOrigin().z)); // ?

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

			for (int i = 0; i < maxDepth; ++i) {

				result.pos = origin;
				if (GetVoxel(result.pos, map, chunkSize) != 0) {
					result.hit = true;
					return result;
				}

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

		static float mod(float value) {
			int q = static_cast<int>(value);
			if (q < 0)
				--q;
			return value - static_cast<float>(q);
		}

		static float intbound(float s, float ds) {
			// Find the smallest positive t such that s+t*ds is an integer.
			if (ds < 0) {
				return intbound(-s, -ds);
			}
			else {
				s = mod(s); // mod(s,1)
							// problem is now s+t*ds = 1
				return (1 - s) * ds;
			}
		}

		inline static RayCastResult PickVoxel2(Ray& ray, ChunkMap& map, int maxDepth, int chunkSize)
		{

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
			
			auto signum = [](float x) {
				return x > 0 ? 1 : x < 0 ? -1 : 0;
			};

			glm::vec3 origin;
			origin.x = floorf(ray.GetOrigin().x);
			origin.y = floorf(ray.GetOrigin().y);
			origin.z = floorf(ray.GetOrigin().z);

			glm::vec3 step;
			step.x = signum(ray.GetDirection().x);
			step.y = signum(ray.GetDirection().y);
			step.z = signum(ray.GetDirection().z);

			glm::vec3 tmax;
			tmax.x = intbound(ray.GetOrigin().x, ray.GetInverse().x);
			tmax.y = intbound(ray.GetOrigin().y, ray.GetInverse().y);
			tmax.z = intbound(ray.GetOrigin().z, ray.GetInverse().z);

			glm::vec3 tdelta = step * ray.GetInverse();

			math::RayCastResult result;

			for (int i = 0; i < maxDepth; ++i) {

				result.pos = origin;
				if (GetVoxel(result.pos, map, chunkSize) != 0) {
					result.hit = true;
					return result;
				}

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
			}
			result.hit = false;
			return result;
		}
	}
}