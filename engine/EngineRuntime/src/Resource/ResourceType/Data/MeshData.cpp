#include <fstream>
#include <yaml-cpp/yaml.h>
#include "EngineRuntime/include/Resource/ResourceType/Data/MeshData.h"

#include "EngineRuntime/include/Core/Math/Math.h"

namespace Engine
{
	Vertex::Vertex(const Vector3& position, const Vector3& normal, const Vector3& tangent, const Vector2& texCoords)
		:Position(position), Normal(normal), Tangent(tangent), TexCoords(texCoords), BoneWeights(Vector4::ZERO), BoneNum(0)
	{ }

	Vertex::Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v)
		:Position(px,py,pz), Normal(nx,ny,nz), Tangent(tx,ty,tz), TexCoords(u,v), BoneWeights(Vector4::ZERO), BoneNum(0)
	{ }

	IMPLEMENT_RTTI(Mesh, Object);

	void SubMesh::Serialize(SerializerDataFrame& stream) const
	{
		stream << Vertices;
		stream << Indices;
		stream << mBoundingBox.mMin;
		stream << mBoundingBox.mMax;
		stream << MaterialIndex;
		stream << mGuid;
	}

	bool SubMesh::Deserialize(SerializerDataFrame& stream)
	{
		stream >> Vertices;
		stream >> Indices;
		stream >> mBoundingBox.mMin;
		stream >> mBoundingBox.mMax;
		stream >> MaterialIndex;
		stream >> mGuid;

		return true;
	}

	void SubMesh::Subdivide()
	{
		// Save a copy of the input geometry.
		std::vector<Vertex> VerticesCopy = Vertices;
		std::vector<uint32_t> Indices32Copy = Indices;


		Vertices.resize(0);
		Indices.resize(0);

		//       v1
		//       *
		//      / \
		//     /   \
		//  m0*-----*m1
		//   / \   / \
		//  /   \ /   \
		// *-----*-----*
		// v0    m2     v2

		uint32_t numTris = (uint32_t)Indices32Copy.size() / 3;
		for (uint32_t i = 0; i < numTris; ++i)
		{
			Vertex v0 = VerticesCopy[Indices32Copy[i * 3 + 0]];
			Vertex v1 = VerticesCopy[Indices32Copy[i * 3 + 1]];
			Vertex v2 = VerticesCopy[Indices32Copy[i * 3 + 2]];

			//
			// Generate the midpoints.
			//

			Vertex m0 = MidPoint(v0, v1);
			Vertex m1 = MidPoint(v1, v2);
			Vertex m2 = MidPoint(v0, v2);

			//
			// Add new geometry.
			//

			Vertices.push_back(v0); // 0
			Vertices.push_back(v1); // 1
			Vertices.push_back(v2); // 2
			Vertices.push_back(m0); // 3
			Vertices.push_back(m1); // 4
			Vertices.push_back(m2); // 5

			Indices.push_back(i * 6 + 0);
			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 5);

			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 4);
			Indices.push_back(i * 6 + 5);

			Indices.push_back(i * 6 + 5);
			Indices.push_back(i * 6 + 4);
			Indices.push_back(i * 6 + 2);

			Indices.push_back(i * 6 + 3);
			Indices.push_back(i * 6 + 1);
			Indices.push_back(i * 6 + 4);
		}
	}

	Vertex SubMesh::MidPoint(const Vertex& v0, const Vertex& v1)
	{
		// Compute the midpoints of all the attributes.  Vectors need to be normalized
		// since linear interpolating can make them not unit length.  
		Vector3 pos = 0.5f * (v0.Position + v1.Position);
		Vector3 normal = 0.5f * (v0.Normal + v1.Normal);
		normal.Normalize();
		Vector3 tangent = 0.5f * (v0.Tangent + v1.Tangent);
		tangent.Normalize();
		Vector2 tex = 0.5f * (v0.TexCoords + v1.TexCoords);

		Vertex v(pos, normal, tangent, tex);

		return v;
	}

	void SubMesh::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		uint32_t baseIndex = (uint32_t)Vertices.size();

		float y = 0.5f * height;
		float dTheta = 2.0f * Math_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (uint32_t i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius * cosf(i * dTheta);
			float z = topRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			Vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		}

		// Cap center vertex.
		Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

		// Index of center vertex.
		uint32_t centerIndex = (uint32_t)Vertices.size() - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			Indices.push_back(centerIndex);
			Indices.push_back(baseIndex + i + 1);
			Indices.push_back(baseIndex + i);
		}
	}

	void SubMesh::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		// 
		// Build bottom cap.
		//

		uint32_t baseIndex = (uint32_t)Vertices.size();
		float y = -0.5f * height;

		// vertices of ring
		float dTheta = 2.0f * Math_PI / sliceCount;
		for (uint32_t i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius * cosf(i * dTheta);
			float z = bottomRadius * sinf(i * dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			Vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		}

		// Cap center vertex.
		Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

		// Cache the index of center vertex.
		uint32_t centerIndex = (uint32_t)Vertices.size() - 1;

		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			Indices.push_back(centerIndex);
			Indices.push_back(baseIndex + i);
			Indices.push_back(baseIndex + i + 1);
		}
	}

	void Mesh::Serialize(Engine::SerializerDataFrame& stream) const
	{
		stream << Meshes;
	}

	bool Mesh::Deserialize(Engine::SerializerDataFrame& stream)
	{
		stream >> Meshes;

		return true;
	}

	void Mesh::CreateBox(Mesh& mesh, float width, float height, float depth, uint32_t numSubdivisions)
	{
		mesh.Meshes.push_back(SubMesh());
		SubMesh& sub_mesh = mesh.Meshes.back();
		//sub_mesh.Vertices
		Vertex v[24];

		float w2 = 0.5f * width;
		float h2 = 0.5f * height;
		float d2 = 0.5f * depth;

		// Fill in the front face vertex data.
		v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

		sub_mesh.Vertices.assign(&v[0], &v[24]);

		//
		// Create the indices.
		//

		uint32_t i[36];

		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7] = 5; i[8] = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] = 9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		sub_mesh.Indices.assign(&i[0], &i[36]);

		// Put a cap on the number of subdivisions.
		numSubdivisions = std::min<uint32_t>(numSubdivisions, 6u);
		for (uint32_t i = 0; i < numSubdivisions; ++i)
			sub_mesh.Subdivide();

		sub_mesh.mBoundingBox.mMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
		sub_mesh.mBoundingBox.mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		for (uint32_t i = 0; i < sub_mesh.Vertices.size(); i++)
		{
			sub_mesh.mBoundingBox.mMax.x = Math::Max(sub_mesh.mBoundingBox.mMax.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMax.y = Math::Max(sub_mesh.mBoundingBox.mMax.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMax.z = Math::Max(sub_mesh.mBoundingBox.mMax.z, sub_mesh.Vertices[i].Position.z);

			sub_mesh.mBoundingBox.mMin.x = Math::Min(sub_mesh.mBoundingBox.mMin.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMin.y = Math::Min(sub_mesh.mBoundingBox.mMin.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMin.z = Math::Min(sub_mesh.mBoundingBox.mMin.z, sub_mesh.Vertices[i].Position.z);
		}
	}

	void Mesh::CreateSphere(Mesh& mesh, float radius, uint32_t sliceCount, uint32_t stackCount)
	{
		//
		// Compute the vertices stating at the top pole and moving down the stacks.
		//

		// Poles: note that there will be texture coordinate distortion as there is
		// not a unique point on the texture map to assign to the pole when mapping
		// a rectangular texture onto a sphere.
		mesh.Meshes.push_back(SubMesh());
		SubMesh& sub_mesh = mesh.Meshes.back();
		
		Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		
		sub_mesh.Vertices.push_back(topVertex);
		
		float phiStep = Math_PI / stackCount;
		float thetaStep = 2.0f * Math_PI / sliceCount;

		// Compute vertices for each stack ring (do not count the poles as rings).
		for (uint32 i = 1; i <= stackCount - 1; ++i)
		{
			float phi = i * phiStep;

			// Vertices of ring.
			for (uint32 j = 0; j <= sliceCount; ++j)
			{
				float theta = j * thetaStep;

				Vertex v;

				// spherical to cartesian
				v.Position.x = radius * sinf(phi) * cosf(theta);
				v.Position.y = radius * cosf(phi);
				v.Position.z = radius * sinf(phi) * sinf(theta);

				// Partial derivative of P with respect to theta
				v.Tangent.x = -radius * sinf(phi) * sinf(theta);
				v.Tangent.y = 0.0f;
				v.Tangent.z = +radius * sinf(phi) * cosf(theta);
				v.Tangent.Normalize();

				v.Normal = v.Position;
				v.Normal.Normalize();

				v.TexCoords.x = theta / Math_2PI;
				v.TexCoords.y = phi / Math_PI;

				sub_mesh.Vertices.push_back(v);
			}
		}

		sub_mesh.Vertices.push_back(bottomVertex);

		//
		// Compute indices for top stack.  The top stack was written first to the vertex buffer
		// and connects the top pole to the first ring.
		//
		
		for (uint32 i = 1; i <= sliceCount; ++i)
		{
			sub_mesh.Indices.push_back(0);
			sub_mesh.Indices.push_back(i + 1);
			sub_mesh.Indices.push_back(i);
		}

		//
		// Compute indices for inner stacks (not connected to poles).
		//

		// Offset the indices to the index of the first vertex in the first ring.
		// This is just skipping the top pole vertex.
		uint32 baseIndex = 1;
		uint32 ringVertexCount = sliceCount + 1;
		for (uint32 i = 0; i < stackCount - 2; ++i)
		{
			for (uint32 j = 0; j < sliceCount; ++j)
			{
				sub_mesh.Indices.push_back(baseIndex + i * ringVertexCount + j);
				sub_mesh.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				sub_mesh.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

				sub_mesh.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				sub_mesh.Indices.push_back(baseIndex + i * ringVertexCount + j + 1);
				sub_mesh.Indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
			}
		}

		//
		// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
		// and connects the bottom pole to the bottom ring.
		//

		// South pole vertex was added last.
		uint32 southPoleIndex = (uint32)sub_mesh.Vertices.size() - 1;
		
		// Offset the indices to the index of the first vertex in the last ring.
		baseIndex = southPoleIndex - ringVertexCount;

		for (uint32 i = 0; i < sliceCount; ++i)
		{
			sub_mesh.Indices.push_back(southPoleIndex);
			sub_mesh.Indices.push_back(baseIndex + i);
			sub_mesh.Indices.push_back(baseIndex + i + 1);
		}

		sub_mesh.mBoundingBox.mMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
		sub_mesh.mBoundingBox.mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		for (uint32_t i = 0; i < sub_mesh.Vertices.size(); i++)
		{
			sub_mesh.mBoundingBox.mMax.x = Math::Max(sub_mesh.mBoundingBox.mMax.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMax.y = Math::Max(sub_mesh.mBoundingBox.mMax.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMax.z = Math::Max(sub_mesh.mBoundingBox.mMax.z, sub_mesh.Vertices[i].Position.z);

			sub_mesh.mBoundingBox.mMin.x = Math::Min(sub_mesh.mBoundingBox.mMin.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMin.y = Math::Min(sub_mesh.mBoundingBox.mMin.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMin.z = Math::Min(sub_mesh.mBoundingBox.mMin.z, sub_mesh.Vertices[i].Position.z);
		}
	}

	void Mesh::CreateCylinder(Mesh& mesh, float bottomRadius, float topRadius, float height, uint32_t sliceCount, uint32_t stackCount)
	{
		mesh.Meshes.push_back(SubMesh());
		SubMesh& sub_mesh = mesh.Meshes.back();

		//
		// Build Stacks.
		// 

		float stackHeight = height / stackCount;

		// Amount to increment radius as we move up each stack level from bottom to top.
		float radiusStep = (topRadius - bottomRadius) / stackCount;

		uint32 ringCount = stackCount + 1;

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for (uint32 i = 0; i < ringCount; ++i)
		{
			float y = -0.5f * height + i * stackHeight;
			float r = bottomRadius + i * radiusStep;

			// vertices of ring
			float dTheta = 2.0f * Math_PI / sliceCount;
			for (uint32 j = 0; j <= sliceCount; ++j)
			{
				Vertex vertex;

				float c = cosf(j * dTheta);
				float s = sinf(j * dTheta);

				vertex.Position = Vector3(r * c, y, r * s);

				vertex.TexCoords.x = (float)j / sliceCount;
				vertex.TexCoords.y = 1.0f - (float)i / stackCount;

				// Cylinder can be parameterized as follows, where we introduce v
				// parameter that goes in the same direction as the v tex-coord
				// so that the bitangent goes in the same direction as the v tex-coord.
				//   Let r0 be the bottom radius and let r1 be the top radius.
				//   y(v) = h - hv for v in [0,1].
				//   r(v) = r1 + (r0-r1)v
				//
				//   x(t, v) = r(v)*cos(t)
				//   y(t, v) = h - hv
				//   z(t, v) = r(v)*sin(t)
				// 
				//  dx/dt = -r(v)*sin(t)
				//  dy/dt = 0
				//  dz/dt = +r(v)*cos(t)
				//
				//  dx/dv = (r0-r1)*cos(t)
				//  dy/dv = -h
				//  dz/dv = (r0-r1)*sin(t)

				// This is unit length.
				vertex.Tangent = Vector3(-s, 0.0f, c);

				float dr = bottomRadius - topRadius;
				Vector3 bitangent(dr * c, -height, dr * s);

				vertex.Normal = vertex.Tangent.CrossProduct(bitangent);
				vertex.Normal.Normalize();

				sub_mesh.Vertices.push_back(vertex);
			}
		}

		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		uint32 ringVertexCount = sliceCount + 1;

		// Compute indices for each stack.
		for (uint32 i = 0; i < stackCount; ++i)
		{
			for (uint32 j = 0; j < sliceCount; ++j)
			{
				sub_mesh.Indices.push_back(i * ringVertexCount + j);
				sub_mesh.Indices.push_back((i + 1) * ringVertexCount + j);
				sub_mesh.Indices.push_back((i + 1) * ringVertexCount + j + 1);

				sub_mesh.Indices.push_back(i * ringVertexCount + j);
				sub_mesh.Indices.push_back((i + 1) * ringVertexCount + j + 1);
				sub_mesh.Indices.push_back(i * ringVertexCount + j + 1);
			}
		}

		sub_mesh.BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount);
		sub_mesh.BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount);

		sub_mesh.mBoundingBox.mMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
		sub_mesh.mBoundingBox.mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		for (uint32_t i = 0; i < sub_mesh.Vertices.size(); i++)
		{
			sub_mesh.mBoundingBox.mMax.x = Math::Max(sub_mesh.mBoundingBox.mMax.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMax.y = Math::Max(sub_mesh.mBoundingBox.mMax.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMax.z = Math::Max(sub_mesh.mBoundingBox.mMax.z, sub_mesh.Vertices[i].Position.z);

			sub_mesh.mBoundingBox.mMin.x = Math::Min(sub_mesh.mBoundingBox.mMin.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMin.y = Math::Min(sub_mesh.mBoundingBox.mMin.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMin.z = Math::Min(sub_mesh.mBoundingBox.mMin.z, sub_mesh.Vertices[i].Position.z);
		}
	}

	void Mesh::CreateGrid(Mesh& mesh, float width, float depth, uint32_t m, uint32_t n)
	{
		mesh.Meshes.push_back(SubMesh());
		SubMesh& sub_mesh = mesh.Meshes.back();

		uint32 vertexCount = m * n;
		uint32 faceCount = (m - 1) * (n - 1) * 2;

		//
		// Create the vertices.
		//

		float halfWidth = 0.5f * width;
		float halfDepth = 0.5f * depth;

		float dx = width / (n - 1);
		float dz = depth / (m - 1);

		float du = 1.0f / (n - 1);
		float dv = 1.0f / (m - 1);

		sub_mesh.Vertices.resize(vertexCount);
		for (uint32 i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dz;
			for (uint32 j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dx;

				sub_mesh.Vertices[i * n + j].Position = Vector3(x, 0.0f, z);
				sub_mesh.Vertices[i * n + j].Normal = Vector3(0.0f, 1.0f, 0.0f);
				sub_mesh.Vertices[i * n + j].Tangent = Vector3(1.0f, 0.0f, 0.0f);

				// Stretch texture over grid.
				sub_mesh.Vertices[i * n + j].TexCoords.x = j * du;
				sub_mesh.Vertices[i * n + j].TexCoords.y = i * dv;
			}
		}

		//
		// Create the indices.
		//

		sub_mesh.Indices.resize(faceCount * 3); // 3 indices per face

		// Iterate over each quad and compute indices.
		uint32 k = 0;
		for (uint32 i = 0; i < m - 1; ++i)
		{
			for (uint32 j = 0; j < n - 1; ++j)
			{
				sub_mesh.Indices[k] = i * n + j;
				sub_mesh.Indices[k + 1] = i * n + j + 1;
				sub_mesh.Indices[k + 2] = (i + 1) * n + j;

				sub_mesh.Indices[k + 3] = (i + 1) * n + j;
				sub_mesh.Indices[k + 4] = i * n + j + 1;
				sub_mesh.Indices[k + 5] = (i + 1) * n + j + 1;

				k += 6; // next quad
			}
		}

		sub_mesh.mBoundingBox.mMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
		sub_mesh.mBoundingBox.mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		for (uint32_t i = 0; i < sub_mesh.Vertices.size(); i++)
		{
			sub_mesh.mBoundingBox.mMax.x = Math::Max(sub_mesh.mBoundingBox.mMax.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMax.y = Math::Max(sub_mesh.mBoundingBox.mMax.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMax.z = Math::Max(sub_mesh.mBoundingBox.mMax.z, sub_mesh.Vertices[i].Position.z);

			sub_mesh.mBoundingBox.mMin.x = Math::Min(sub_mesh.mBoundingBox.mMin.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMin.y = Math::Min(sub_mesh.mBoundingBox.mMin.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMin.z = Math::Min(sub_mesh.mBoundingBox.mMin.z, sub_mesh.Vertices[i].Position.z);
		}
	}

	void Mesh::CreateQuad(Mesh& mesh, float x, float y, float w, float h, float depth)
	{
		mesh.Meshes.push_back(SubMesh());
		SubMesh& sub_mesh = mesh.Meshes.back();

		sub_mesh.Vertices.resize(4);
		sub_mesh.Indices.resize(6);

		// Position coordinates specified in NDC space.
		sub_mesh.Vertices[0] = Vertex(
			x, y - h, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f);

		sub_mesh.Vertices[1] = Vertex(
			x, y, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f);

		sub_mesh.Vertices[2] = Vertex(
			x + w, y, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f);

		sub_mesh.Vertices[3] = Vertex(
			x + w, y - h, depth,
			0.0f, 0.0f, -1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f);

		sub_mesh.Indices[0] = 0;
		sub_mesh.Indices[1] = 1;
		sub_mesh.Indices[2] = 2;

		sub_mesh.Indices[3] = 0;
		sub_mesh.Indices[4] = 2;
		sub_mesh.Indices[5] = 3;

		sub_mesh.mBoundingBox.mMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
		sub_mesh.mBoundingBox.mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		for (uint32_t i = 0; i < sub_mesh.Vertices.size(); i++)
		{
			sub_mesh.mBoundingBox.mMax.x = Math::Max(sub_mesh.mBoundingBox.mMax.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMax.y = Math::Max(sub_mesh.mBoundingBox.mMax.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMax.z = Math::Max(sub_mesh.mBoundingBox.mMax.z, sub_mesh.Vertices[i].Position.z);

			sub_mesh.mBoundingBox.mMin.x = Math::Min(sub_mesh.mBoundingBox.mMin.x, sub_mesh.Vertices[i].Position.x);
			sub_mesh.mBoundingBox.mMin.y = Math::Min(sub_mesh.mBoundingBox.mMin.y, sub_mesh.Vertices[i].Position.y);
			sub_mesh.mBoundingBox.mMin.z = Math::Min(sub_mesh.mBoundingBox.mMin.z, sub_mesh.Vertices[i].Position.z);
		}
	}

	void MeshMeta::Load(const std::filesystem::path& metaPath)
	{
		YAML::Node node = YAML::LoadFile(ProjectFileSystem::GetInstance()->GetActualPath(metaPath).generic_string());

		ASSERT(node["guid"].IsDefined());

		if (mGuid != nullptr)
		{
			delete mGuid;
		}

		mGuid = new GUID(node["guid"].as<std::string>());
	}

	void MeshMeta::Save(const std::filesystem::path& metaPath)
	{
		ASSERT(mGuid != nullptr);

		YAML::Node node;
		node["guid"] = mGuid->Data();

		std::ofstream fout(ProjectFileSystem::GetInstance()->GetActualPath(metaPath));
		fout << node;
	}
}

