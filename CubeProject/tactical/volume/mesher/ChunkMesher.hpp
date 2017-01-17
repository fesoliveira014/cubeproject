#ifndef _CHUNK_MESHER_H_
#define _CHUNK_MESHER_H_

#include "../../Common.h"

#include "../Chunk.h"
//#include "../ChunkManager.h"

#include "../../render/structures/Mesh.h"
#include "../../render/geometry/Geometry.h"

namespace tactical
{
	//class ChunkManager;

	namespace volume
	{
		namespace mesher
		{
			enum MesherType
			{
				GREEDY = 0,
				MARCHING_CUBES,
				NAIVE_SURFACE_NET,
				NAIVE_WITH_CULLING
			};

			class Mesher {
			public:
				static void GenerateSurfaceNets(Chunk& chunk);
				static void Greedy(Chunk& chunk);
				static void NaiveWithCulling(Chunk& chunk);

				inline static void MarchingCubes(Chunk& chunk)
				{
					// TODO
				}

				inline static void GenerateChunkMesh(Chunk& chunk, MesherType type)
				{
					switch (type)
					{
					default:
					case GREEDY:
						Greedy(chunk);
						break;

					case MARCHING_CUBES:
						MarchingCubes(chunk);
						break;

					case NAIVE_SURFACE_NET:
						GenerateSurfaceNets(chunk);
						break;

					case NAIVE_WITH_CULLING:
						NaiveWithCulling(chunk);
						break;
					}
				}
			};
		}
	}
}
#endif