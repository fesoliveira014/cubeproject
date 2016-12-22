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
				NAIVE_SURFACE_NET,
				NAIVE_WITH_CULLING
			};

			static void GenerateSurfaceNets(Chunk& chunk)
			{
				int dims = chunk.GetSize();
				std::vector<glm::vec3> vertices;
				std::vector<int> buffer(dims * dims * 2);

				for (int k = 0; k < dims - 1; ++k) {
					for (int j = 0; j < dims - 1; ++j) {
						for (int i = 0; i < dims - 1; ++i) {
							glm::vec3 p(i, j, k);

							// get density data around current vertex
							byte vs[8] = {
								chunk.GetVoxel(p + glm::vec3(0, 0, 0)),
								chunk.GetVoxel(p + glm::vec3(1, 0, 0)),
								chunk.GetVoxel(p + glm::vec3(0, 1, 0)),
								chunk.GetVoxel(p + glm::vec3(1, 1, 0)),
								chunk.GetVoxel(p + glm::vec3(0, 0, 1)),
								chunk.GetVoxel(p + glm::vec3(1, 0, 1)),
								chunk.GetVoxel(p + glm::vec3(0, 1, 1)),
								chunk.GetVoxel(p + glm::vec3(1, 1, 1))
							};

							// used to check whether this is a solid vertex
							int config_n =
								((vs[0] != 0) << 0) |
								((vs[1] != 0) << 1) |
								((vs[2] != 0) << 2) |
								((vs[3] != 0) << 3) |
								((vs[4] != 0) << 4) |
								((vs[5] != 0) << 5) |
								((vs[6] != 0) << 6) |
								((vs[7] != 0) << 7);

							// if the vertex configuration is 0 or 255, don't generate mesh for this vertex
							if (config_n == 0 || config_n == 0xff)
								continue;

							std::map<int, int> m;
							for (std::size_t i = 0; i < 8; ++i) {
								m[vs[i]]++;
							}

							// finds the mode (the most common) of voxel types surrounding this vertex
							byte count = 0, type = 0;
							for (std::map<int, int>::iterator iter = m.begin(); iter != m.end(); iter++) {
								if ((*iter).first == 0)
									continue;

								if (count < (*iter).second) {
									count = (*iter).second;
									type = (*iter).first;
								}

							}

							glm::vec3 average(0);
							int average_n = 0;
							// gets the average vertex positions for each edge
							auto do_edge = [&](byte va, byte vb, int axis, const glm::vec3& p) {
								if ((va != 0) == (vb != 0))
									return;

								glm::vec3 v = p;
								v[axis] += 0.5f;
								average += v;
								average_n++;
							};

							do_edge(vs[0], vs[1], 0, p + glm::vec3(0, 0, 0));
							do_edge(vs[2], vs[3], 0, p + glm::vec3(0, 1, 0));
							do_edge(vs[4], vs[5], 0, p + glm::vec3(0, 0, 1));
							do_edge(vs[6], vs[7], 0, p + glm::vec3(0, 1, 1));
							do_edge(vs[0], vs[2], 1, p + glm::vec3(0, 0, 0));
							do_edge(vs[1], vs[3], 1, p + glm::vec3(1, 0, 0));
							do_edge(vs[4], vs[6], 1, p + glm::vec3(0, 0, 1));
							do_edge(vs[5], vs[7], 1, p + glm::vec3(1, 0, 1));
							do_edge(vs[0], vs[4], 2, p + glm::vec3(0, 0, 0));
							do_edge(vs[1], vs[5], 2, p + glm::vec3(1, 0, 0));
							do_edge(vs[2], vs[6], 2, p + glm::vec3(0, 1, 0));
							do_edge(vs[3], vs[7], 2, p + glm::vec3(1, 1, 0));

							auto offset = [&](const glm::vec3& pos) {
								int ret = pos.x + dims * pos.y + dims * dims * (int(pos.z) % 2);
								return ret;
							};

							glm::vec3 v(average.x / average_n, average.y / average_n, average.z / average_n);
							buffer[offset(p)] = vertices.size();
							vertices.push_back(v);

							// adds a quad to the mesh
							auto quad = [&](byte flip, int ia, int ib, int ic, int id) {
								if (flip)
									std::swap(ib, id);

								if (ia >= vertices.size() || ib >= vertices.size() || ic >= vertices.size() || id >= vertices.size())
									return;

								std::lock_guard<std::mutex> lock(globalMutex);
								render::geometry::AddQuad<render::Vertex3f3f4f>(
									vertices[ia], vertices[ib],
									vertices[ic], vertices[id],
									chunk.GetMesh()->vertices,
									chunk.GetMesh()->indices,
									type
									);
							};

							// checks if we can add a quad and if we should flip it
							const byte flip = vs[0] != 0;
							if (p.y > 0 && p.z > 0 && (vs[0] == 0) != (vs[1] == 0)) {
								quad(flip,
									buffer[offset(glm::vec3(p.x, p.y, p.z))],
									buffer[offset(glm::vec3(p.x, p.y, p.z - 1.0f))],
									buffer[offset(glm::vec3(p.x, p.y - 1.0f, p.z - 1.0f))],
									buffer[offset(glm::vec3(p.x, p.y - 1.0f, p.z))]
									);
							}
							if (p.x > 0 && p.z > 0 && (vs[0] == 0) != (vs[2] == 0)) {
								quad(flip,
									buffer[offset(glm::vec3(p.x, p.y, p.z))],
									buffer[offset(glm::vec3(p.x - 1.0f, p.y, p.z))],
									buffer[offset(glm::vec3(p.x - 1.0f, p.y, p.z - 1.0f))],
									buffer[offset(glm::vec3(p.x, p.y, p.z - 1.0f))]
									);
							}
							if (p.x > 0 && p.y > 0 && (vs[0] == 0) != (vs[4] == 0)) {
								quad(flip,
									buffer[offset(glm::vec3(p.x, p.y, p.z))],
									buffer[offset(glm::vec3(p.x, p.y - 1.0f, p.z))],
									buffer[offset(glm::vec3(p.x - 1.0f, p.y - 1.0f, p.z))],
									buffer[offset(glm::vec3(p.x - 1.0f, p.y, p.z))]
									);
							}
						}
					}
				}
				std::lock_guard<std::mutex> lock(globalMutex);
				render::geometry::CalculateNormals<render::Vertex3f3f4f>(chunk.GetMesh()->vertices, chunk.GetMesh()->indices);
			}

			inline static void Greedy(Chunk& chunk)
			{
				// cleanup
				//delete chunk.GetMesh()->vao;
				//delete chunk.GetMesh()->ibo;
				//chunk.GetMesh()->vertices.clear();
				//chunk.GetMesh()->indices.clear();

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
								int a = (0 <= x[axis] ? (chunk.IsFaceVisible(pos, face) ? chunk.GetVoxel(pos) : 0) : 0);

								// checks if the negative face is visible
								face.face = (render::geometry::Face::FaceName)(axis + 3);
								pos += glm::vec3(q[0], q[1], q[2]);
								int b = (x[axis] < chunk.GetSize() - 1 ? (chunk.IsFaceVisible(pos, face) ? chunk.GetVoxel(pos) : 0) : 0);

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
									{
										// race condition here
										std::lock_guard<std::mutex> lock(globalMutex);
										render::geometry::AddQuad<render::Vertex3f3f4f>(
											glm::vec3(x[0], x[1], x[2]),
											glm::vec3(x[0] + du[0], x[1] + du[1], x[2] + du[2]),
											glm::vec3(x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2]),
											glm::vec3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]),
											chunk.GetMesh()->vertices, chunk.GetMesh()->indices, c);
									}

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

				// race condition here
				std::lock_guard<std::mutex> lock(globalMutex);
				render::geometry::CalculateNormals<render::Vertex3f3f4f>(chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

				/*
				std::vector<render::VertexAttribute> attributes;
				attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
				attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));
				attributes.push_back(render::VertexAttribute(3, 4, GLType::FLOAT));

				chunk.GetMesh()->vao = new render::VertexArray();
				chunk.GetMesh()->ibo = new render::IndexBuffer(chunk.GetMesh()->indices.data(),
					(GLsizei)chunk.GetMesh()->indices.size());
				chunk.GetMesh()->vao->AddBuffer(new render::Buffer(chunk.GetMesh()->vertices.data(),
					(GLsizei)chunk.GetMesh()->vertices.size() * sizeof(render::Vertex3f3f4f), attributes));

				chunk.GetMesh()->vertices.clear();
				chunk.GetMesh()->indices.clear();

				chunk.Updated();
				*/
				delete[] mask;
			}

			inline static void MarchingCubes(Chunk& chunk)
			{
				// TODO
			}

			inline static void NaiveWithCulling(Chunk& chunk)
			{
				//delete chunk.GetMesh()->vao;
				//delete chunk.GetMesh()->ibo;
				//chunk.GetMesh()->vertices.clear();
				//chunk.GetMesh()->indices.clear();

				for (int k = 0; k < chunk.GetSize(); ++k) {
					for (int j = 0; j < chunk.GetSize(); ++j) {
						for (int i = 0; i < chunk.GetSize(); ++i) {
							glm::vec3 pos(i, j, k);
							byte type = chunk.GetVoxel(pos);

							render::geometry::Face face;

							// check top face
							// race condition here
							std::lock_guard<std::mutex> lock(globalMutex);
							if (type != 0) {
								face.face = render::geometry::Face::TOP;
								if (chunk.IsFaceVisible(pos, face)) {
									render::geometry::AddQuad<render::Vertex3f3f4f>(
										pos + glm::vec3(1, 1, 1),
										pos + glm::vec3(1, 1, 0),
										pos + glm::vec3(0, 1, 0),
										pos + glm::vec3(0, 1, 1),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices, type);
								}

								face.face = render::geometry::Face::BOTTOM;
								if (chunk.IsFaceVisible(pos, face)) {
									render::geometry::AddQuad<render::Vertex3f3f4f>(
										pos + glm::vec3(1, 0, 0),
										pos + glm::vec3(1, 0, 1),
										pos + glm::vec3(0, 0, 1),
										pos + glm::vec3(0, 0, 0),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices, type);
								}

								face.face = render::geometry::Face::LEFT;
								if (chunk.IsFaceVisible(pos, face)) {
									render::geometry::AddQuad<render::Vertex3f3f4f>(
										pos + glm::vec3(0, 1, 0),
										pos + glm::vec3(0, 0, 0),
										pos + glm::vec3(0, 0, 1),
										pos + glm::vec3(0, 1, 1),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices, type);
								}

								face.face = render::geometry::Face::RIGHT;
								if (chunk.IsFaceVisible(pos, face)) {
									render::geometry::AddQuad<render::Vertex3f3f4f>(
										pos + glm::vec3(1, 1, 1),
										pos + glm::vec3(1, 0, 1),
										pos + glm::vec3(1, 0, 0),
										pos + glm::vec3(1, 1, 0),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices, type);
								}

								face.face = render::geometry::Face::FRONT;
								if (chunk.IsFaceVisible(pos, face)) {
									render::geometry::AddQuad<render::Vertex3f3f4f>(
										pos + glm::vec3(0, 0, 0),
										pos + glm::vec3(0, 1, 0),
										pos + glm::vec3(1, 1, 0),
										pos + glm::vec3(1, 0, 0),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices, type);
								}

								face.face = render::geometry::Face::BACK;
								if (chunk.IsFaceVisible(pos, face)) {
									render::geometry::AddQuad<render::Vertex3f3f4f>(
										pos + glm::vec3(0, 1, 1),
										pos + glm::vec3(0, 0, 1),
										pos + glm::vec3(1, 0, 1),
										pos + glm::vec3(1, 1, 1),
										chunk.GetMesh()->vertices, chunk.GetMesh()->indices, type);
								}
							}
						}
					}
				}

				// race condition here
				std::lock_guard<std::mutex> lock(globalMutex);
				render::geometry::CalculateNormals<render::Vertex3f3f4f>(chunk.GetMesh()->vertices, chunk.GetMesh()->indices);

				/*
				std::vector<render::VertexAttribute> attributes;
				attributes.push_back(render::VertexAttribute(0, 3, GLType::FLOAT));
				attributes.push_back(render::VertexAttribute(1, 3, GLType::FLOAT));
				attributes.push_back(render::VertexAttribute(3, 4, GLType::FLOAT));

				chunk.GetMesh()->vao = new render::VertexArray();
				chunk.GetMesh()->ibo = new render::IndexBuffer(chunk.GetMesh()->indices.data(),
					(GLsizei)chunk.GetMesh()->indices.size());
				chunk.GetMesh()->vao->AddBuffer(new render::Buffer(chunk.GetMesh()->vertices.data(),
					(GLsizei)chunk.GetMesh()->vertices.size() * sizeof(render::Vertex3f3f4f), attributes));

				chunk.GetMesh()->vertices.clear();
				chunk.GetMesh()->indices.clear();

				chunk.Updated();
				*/
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
		}
	}
}
#endif