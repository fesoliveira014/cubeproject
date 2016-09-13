#ifndef _CHUNK_MESHER_H_
#define _CHUNK_MESHER_H_

#include "../../Common.h"

#include "../Chunk.h"

#include "../../render/structures/Mesh.h"
#include "../../render/geometry/Geometry.h"

namespace tactical
{
	namespace volume
	{
		namespace mesher
		{
			enum MesherType
			{
				GREEDY = 0,
				MARCHING_CUBES,
				NAIVE_WITH_CULLING
			};

			inline static void Greedy(Chunk& chunk)
			{
				// cleanup
				delete chunk.GetMesh()->vao;
				delete chunk.GetMesh()->ibo;
				chunk.GetMesh()->vertices.clear();
				chunk.GetMesh()->indices.clear();

				// this is the variable that will store all of the voxel data for the current plane
				int* mask = new int[chunk.GetSize() * chunk.GetSize()];

				render::geometry::Face face; // stores face data (wheter it is front, back, left, etc)

				int axis = 0; // current plane axis

				// iterates over the three axis
				for (axis = 0; axis < 3; ++axis) {
					int u = (axis + 1) % 3; 
					int v = (axis + 2) % 3;

					int x[3] = { 0, 0, 0 };
					int q[3] = { 0, 0, 0 };

					q[axis] = 1;

					// Iterates through a section of the chunk cut at the current axis, defined by
					// x[axis]. It, then, check if each face of the voxel on the current axis is 
					// visible, setting 1 at the mask when it is true, 0 otherwise
					for (x[axis] = -1; x[axis] < chunk.GetSize(); ) {
						int n = 0; // current mask index

						for (x[v] = 0; x[v] < chunk.GetSize(); ++x[v]) {
							for (x[u] = 0; x[u] < chunk.GetSize(); ++x[u], ++n) {
								face.face = (render::geometry::Face::FaceName)axis;

								// checks if the positive face is visible
								glm::vec3 pos = glm::vec3(x[0], x[1], x[2]);
								int a = (0 <= x[axis] ? (chunk.IsFaceVisible(pos, face) ? chunk.GetVoxel(pos) : 0 ) : 0);

								// checks if the negative face is visible
								face.face = (render::geometry::Face::FaceName)(axis + 3);
								pos += glm::vec3(q[0], q[1], q[2]);
								int b = (x[axis] <  chunk.GetSize() - 1 ? (chunk.IsFaceVisible(pos, face) ? chunk.GetVoxel(pos) : 0) : 0);

								// Determines if we are using front face or backface
								if ((!!a) == (!!b))
									mask[n] = 0;
								else if (!!a) 
									mask[n] = a;
								else
									mask[n] = -b;
							}
						}
						
						++x[axis];
						n = 0; // current mask index

						// Now we iterate over the mask to create the mesh. When an entry of the mask is
						// different than 0, we then add a face at the corresponding position. This takes
						// into consideration the handness of the face, as well as wether it is a front
						// or backface.
						for (int j = 0; j < chunk.GetSize(); ++j) {
							for (int i = 0; i < chunk.GetSize();) {
								int c = mask[n];
								int width, height;

								// Greedy step. 
								// The algorithm tries first to find the width of the current quad. It iterates over
								// the row until it hits a voxel whose type is different than the quad's. When this
								// is found, we do the same for the height, getting the biggest quad of our current
								// type as possible. This is repeated for all mask entries that are not null, and the
								// quads never overlap.
								//
								// The variable `c` is a legacy variable from the original algorithm. It just states the
								// current type, serving as an auxiliary variable.
								if (!!c) {
									for (width = 1; c == mask[n + width] && i + width < chunk.GetSize(); ++width) {}
									bool done = false;
									for (height = 1; height + j < chunk.GetSize(); ++height) {
										for (int k = 0; k < width; ++k) {
											if (c != mask[n + k + height * chunk.GetSize()]) {
												done = true;
												break;
											}
										}

										if (done)
											break;
									}

									x[u] = i; x[v] = j;			// sets the correct indices
									int du[] = { 0, 0, 0 };     // auxiliary vector for width
									int dv[] = { 0, 0, 0 };     // auxiliary vector for height

									if (c > 0) {
										dv[v] = height;
										du[u] = width;
									}
									else {
										c = -c;
										du[v] = height;
										dv[u] = width;
									}

									// check definition of AddQuad to see how each face is stored (Geometry.hpp)
									render::geometry::AddQuad<render::Vertex3f3f>(
										glm::vec3(x[0], x[1], x[2]),
										glm::vec3(x[0] + du[0], x[1] + du[1], x[2] + du[2]),
										glm::vec3(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]),
										glm::vec3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

									// cleanup for next iteration
									for (int l = 0; l < height; ++l) {
										for (int k = 0; k < width; ++k) {
											mask[n + k + l*chunk.GetSize()] = 0;
										}
									}

									i += width;
									n += width;
								}
								else {
									++i;
									++n;
								}
							}
						}
					}
				}

				render::geometry::CalculateNormals<render::Vertex3f3f>(chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

				std::vector<render::VertexAttribute> attributes;
				attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
				attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));

				chunk.GetMesh()->vao = new render::VertexArray();
				chunk.GetMesh()->ibo = new render::IndexBuffer(chunk.GetMesh()->indices.data(),
					(GLsizei)chunk.GetMesh()->indices.size());
				chunk.GetMesh()->vao->AddBuffer(new render::Buffer(chunk.GetMesh()->vertices.data(),
					(GLsizei)chunk.GetMesh()->vertices.size() * sizeof(render::Vertex3f3f), attributes));

				chunk.UpdatedGeometry();
				delete mask;
			}

			inline static void MarchingCubes(Chunk& chunk)
			{
				// TODO
			}

			inline static void NaiveWithCulling(Chunk& chunk)
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

				case NAIVE_WITH_CULLING:
					NaiveWithCulling(chunk);
					break;
				}
			}
		}
	}
}
#endif