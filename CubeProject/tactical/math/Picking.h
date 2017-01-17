#ifndef _PICKING_H_
#define _PICKING_H_
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

		inline static RayCastResult PickVoxel(Ray& ray, ChunkMap& map, int maxDepth, int chunkSize)
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

#endif