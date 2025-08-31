#include "RenderResources.h"

#include <iostream>
#include <cassert>
#include <cmath>
#include "RenderResources.h"
#include "../../../Dependencies/tiny_obj/tiny_obj_loader.h"
#include "Engine/Utility/MacroUtility.h"

struct MeshVertexTempStorage
{
	float position[3];
};

struct MeshTexCoordTempStorage
{
	float texcoord[2];
};

struct MeshLoadingState
{
	MeshLoadingState(RenderMeshResource* _owner) : Owner(_owner) { }
	RenderMeshResource* Owner = nullptr;
	int NumVertexPositionLoaded = 0;
	int NumVertexColorLoaded = 0;
	int NumTexCoordLoaded = 0;
	int NumIndicesLoaded = 0;
	int NumVertexNormalLoaded = 0;
	RenderMeshResource::MeshBufferCPU<MeshVertexTempStorage> VertexPositionTempstorage;
	RenderMeshResource::MeshBufferCPU<MeshVertexTempStorage> VertexNormalTempstorage;
	RenderMeshResource::MeshBufferCPU<MeshTexCoordTempStorage> VertexTexCoordTempstorage;
};

const int MeshLoadingBufferGrowStep = 1024;
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

static int FixupObjIndex(int idx, int n) 
{
	assert(idx != 0);
	if (idx > 0)
		return idx - 1;
	if (idx < 0)
		return n + idx;  // negative value = relative

	return idx;  // never reach here.
}

const float DefaultVtxColor[3] = { 1.0f, 1.0f, 1.0f };

static void LoadVertexPositionCallback(void* userData, float x, float y, float z, float w)
{
	(void)w;

	MeshLoadingState* state = (MeshLoadingState*)userData;
	state->VertexPositionTempstorage.GrowIfNeeded(MeshLoadingBufferGrowStep);
	auto& vtx = state->VertexPositionTempstorage.Data[(state->NumVertexPositionLoaded)++];
	
	
	vtx.position[0] = x;
	vtx.position[1] = y;
	vtx.position[2] = z;

	state->VertexPositionTempstorage.NumElements = max(state->VertexPositionTempstorage.NumElements, state->NumVertexPositionLoaded);
	assert(state->VertexPositionTempstorage.NumElements * sizeof(decltype(*state->VertexPositionTempstorage.Data)) <= state->VertexPositionTempstorage.AllocBytes);
}

static void LoadVertexNormalCallback(void* userData, float x, float y, float z)
{
	MeshLoadingState* state = (MeshLoadingState*)userData;
	state->VertexNormalTempstorage.GrowIfNeeded(MeshLoadingBufferGrowStep);
	auto& vtNormal = state->VertexNormalTempstorage.Data[(state->NumVertexNormalLoaded)++];
	
	
	vtNormal.position[0] = x;
	vtNormal.position[1] = y;
	vtNormal.position[2] = z;

	state->VertexNormalTempstorage.NumElements = max(state->VertexNormalTempstorage.NumElements, state->NumVertexNormalLoaded);
	assert(state->VertexNormalTempstorage.NumElements * sizeof(decltype(*state->VertexNormalTempstorage.Data)) <= state->VertexNormalTempstorage.AllocBytes);
}

static void LoadTexCoordCallback(void* userData, float x, float y, float z)
{
	(void)z;

	MeshLoadingState* state = (MeshLoadingState*)userData;
	state->VertexTexCoordTempstorage.GrowIfNeeded(MeshLoadingBufferGrowStep);
	auto& vtx = state->VertexTexCoordTempstorage.Data[(state->NumTexCoordLoaded)++];
	
	
	vtx.texcoord[0] = x;
	vtx.texcoord[1] = y;

	state->VertexTexCoordTempstorage.NumElements = max(state->VertexTexCoordTempstorage.NumElements , state->NumTexCoordLoaded);
	assert(state->Owner->VerticesCPU.NumElements * sizeof(decltype(*state->Owner->VerticesCPU.Data)) <= state->Owner->VerticesCPU.AllocBytes);
}

static void LoadIndexCallback(void* userData, tinyobj::index_t* indices, int numIndices)
{
	MeshLoadingState* state = (MeshLoadingState*)userData;
	assert(numIndices == 3);	// only triangle is supported for rendering. quad is not implemented.

	for (int i=0; i < numIndices; i++)
	{
		//assert(state->NumVertexPositionLoaded >= state->NumTexCoordLoaded);
		//assert(indices->texcoord_index < 0); ???????????为什么会小于0呢？
		int indicesIndex = state->NumIndicesLoaded;
		state->NumIndicesLoaded++;
		
		//压缩算法有一点复杂，暂时先不压缩，每次读取到一个新的顶点就直接加进去
		state->Owner->IndicesCPU.NumElements = max(state->Owner->IndicesCPU.NumElements, state->NumIndicesLoaded);
		state ->Owner->VerticesCPU.NumElements = max(state ->Owner->VerticesCPU.NumElements, state->NumIndicesLoaded);
		//这个不一定的，神经病assert
		//assert(state->Owner->IndicesCPU.NumElements * sizeof(decltype(*state->Owner->IndicesCPU.Data)) >= state->Owner->IndicesCPU.AllocBytes);
		
		//更新数量之后再更新内存大小
		state->Owner->IndicesCPU.GrowIfNeeded(MeshLoadingBufferGrowStep);
		state->Owner->VerticesCPU.GrowIfNeeded(MeshLoadingBufferGrowStep);
		
		// sort vertex position by texcoord_index
		{
			int vIndex = (indices+i)->vertex_index-1;
			int vtIndex = (indices+i)->texcoord_index-1;
			int vnIndex = (indices+i)->normal_index-1;
			assert(vIndex < state->VertexPositionTempstorage.NumElements);
			assert(vtIndex < state->VertexTexCoordTempstorage.NumElements);
			assert(vnIndex < state->VertexNormalTempstorage.NumElements);
			memcpy(state->Owner->VerticesCPU.Data[indicesIndex].position, state->VertexPositionTempstorage.Data[vIndex].position, sizeof(RenderMeshResource::MeshVertex::position));
			//DEBUG_PRINT("%f ", *(state->VertexPositionTempstorage.Data[vIndex].position));
			memcpy(state->Owner->VerticesCPU.Data[indicesIndex].uv, state->VertexTexCoordTempstorage.Data[vtIndex].texcoord, sizeof(RenderMeshResource::MeshVertex::uv));
			//DEBUG_PRINT("%f ", *(state->VertexTexCoordTempstorage.Data[vtIndex].texcoord));
			memcpy(state->Owner->VerticesCPU.Data[indicesIndex].noraml, state->VertexNormalTempstorage.Data[vnIndex].position, sizeof(RenderMeshResource::MeshVertex::noraml));
			//DEBUG_PRINT("%f \n", *(state->VertexNormalTempstorage.Data[vnIndex].position));
			memcpy(state->Owner->VerticesCPU.Data[indicesIndex].vtxcolor, DefaultVtxColor, sizeof(DefaultVtxColor));
		}
		//保存indices数据
		state->Owner->IndicesCPU.Data[indicesIndex] = indicesIndex;
	}
}

bool RenderMeshResource::LoadMesh(std::istream& inputStream)
{
	tinyobj::callback_t cb;
	cb.vertex_cb = LoadVertexPositionCallback;
	cb.normal_cb = LoadVertexNormalCallback;
	cb.texcoord_cb = LoadTexCoordCallback;
	cb.index_cb = LoadIndexCallback;
	//cb.usemtl_cb = usemtl_cb;
	//cb.mtllib_cb = mtllib_cb;
	//cb.group_cb = group_cb;
	//cb.object_cb = object_cb;

	VerticesCPU.Reset();
	IndicesCPU.Reset();

	MeshLoadingState loadingState(this);
	
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObjWithCallback(inputStream, cb, &loadingState, nullptr, &warn, &err);

	/*if (!warn.empty())
	{
		printf("Warning when load mesh: %s\n", warn.c_str());
	}*/

	if (!err.empty())
	{
		printf("Error when load mesh: %s\n", err.c_str());
		return false;
	}

	//assert(loadingState.NumVertexPositionLoaded <= loadingState.NumTexCoordLoaded);
	//assert(loadingState.NumVertexColorLoaded == 0 || loadingState.NumVertexColorLoaded == loadingState.NumVertexPositionLoaded);
	//assert(loadingState.NumVertexPositionLoaded < loadingState.NumIndicesLoaded);

	return ret;
}

bool RenderMeshResource::LoadMesh_Plane(float quadWidth)
{
	static const RenderMeshResource::MeshVertex kVertexData[] =
	{
		// 3    4
		// +----+
		// |\   |
		// | \  |
		// |  \ |
		// |   \|
		// +----+
		// 1    2

		//   POSITION                COLOR               UV
		{{-quadWidth/2, -quadWidth/2, 0.0f},    {0.7f, 0.7f, 1.0f},    {0.0f, 1.0f}},
		{{quadWidth/2, -quadWidth/2, 0.0f},    {0.7f, 0.7f, 1.0f},    {1.0f, 1.0f}},
		{{-quadWidth/2,  quadWidth/2, 0.0f},    {0.7f, 1.0f, 1.0f},    {0.0f, 0.0f}},
		{{quadWidth/2,  quadWidth/2, 0.0f},    {1.0f, 0.7f, 1.0f},    {1.0f, 0.0f}},
	};

	// CCW_FRONT
	// https://stackoverflow.com/questions/4346556/loading-indices-from-wavefront-obj-format
	static const tinyobj::index_t kIndexData[] =
	{
		{1, 0, 1},
		{2, 0, 2},
		{3, 0, 3},

		{2, 0, 2},
		{4, 0, 4},
		{3, 0, 3},
	};

	VerticesCPU.Reset();
	IndicesCPU.Reset();

	MeshLoadingState loadingState(this);

	for (int v = 0; v < sizeof(kVertexData) / sizeof(kVertexData[0]); ++v)
	{
		LoadVertexPositionCallback(&loadingState, kVertexData[v].position[0], kVertexData[v].position[1], kVertexData[v].position[2], 1.0f);
		LoadTexCoordCallback(&loadingState, kVertexData[v].uv[0], kVertexData[v].uv[1], 1.0f);
	}
	for (int i = 0; i < sizeof(kIndexData) / sizeof(kIndexData[0]); i += 3)
	{
		LoadIndexCallback(&loadingState, const_cast<tinyobj::index_t*>(&(kIndexData[i])), 3);
	}
	
	return true;
}


bool RenderMeshResource::LoadMesh_Sphere(float radius, int xdiv, int ydiv, float tx, float ty, float tz)
{
	VerticesCPU.Reset();
	IndicesCPU.Reset();

	VerticesCPU.NumElements = (xdiv + 1) * (ydiv + 1);
	VerticesCPU.Resize(VerticesCPU.NumElements);
	IndicesCPU.NumElements = (xdiv + 1) * (xdiv * (ydiv - 1) * 2) * 3;
	IndicesCPU.Resize(IndicesCPU.NumElements);

	memset(VerticesCPU.Data, 0, VerticesCPU.AllocBytes);
	memset(IndicesCPU.Data, 0, IndicesCPU.AllocBytes);

	// Everything else is just filling in the vertex and index buffer.
	MeshVertex* outV = VerticesCPU.Data;
	uint32_t* outI = IndicesCPU.Data;

	const float pi = 3.14159265358979323846f;
	const float gx = 2 * pi / xdiv;
	const float gy = pi / ydiv;

	for (long i = 0; i < xdiv; ++i)
	{
		const float theta = (float)i * gx;
		const float ct = cosf(theta);
		const float st = sinf(theta);

		const long k = i * (ydiv + 1);
		for (long j = 1; j < ydiv; ++j)
		{
			const float phi = (float)j * gy;
			const float sp = sinf(phi);
			const float x = ct * sp;
			const float y = st * sp;
			const float z = cosf(phi);

			float _pos[3] = { x * radius + tx , y * radius + ty , z * radius + tz };
			float _uv[2] = { theta * 0.1591549430918953f, phi * 0.31830988618379f };
			memcpy(outV[k + j].position, _pos, sizeof(_pos));
			// normal = Vector3(x, y, z);
			memcpy(outV[k + j].vtxcolor, DefaultVtxColor, sizeof(DefaultVtxColor));
			memcpy(outV[k + j].uv, _uv, sizeof(_uv));
		}
	}

	const long kk = xdiv * (ydiv + 1);
	for (long j = 1; j < ydiv; ++j)
	{
		const float phi = (float)j * gy;
		const float x = sinf(phi);
		const float z = cosf(phi);

		float _pos[3] = { x * radius + tx, ty, z * radius + tz };
		float _uv[2] = { 1.0f, phi * 0.31830988618379f };
		memcpy(outV[kk + j].position, _pos, sizeof(_pos));
		// normal = Vector3(x, 0, z);
		memcpy(outV[kk + j].vtxcolor, DefaultVtxColor, sizeof(DefaultVtxColor));
		memcpy(outV[kk + j].uv, _uv, sizeof(_uv));
	}

	for (long i = 0; i < xdiv; i++)
	{
		const long k1 = i * (ydiv + 1) + 1;
		const long k2 = (i + 1) * (ydiv + 1) + 1;
		const float s = (outV[k1].uv[0] + outV[k2].uv[0]) * 0.5f;

		float _pos[3] = { tx, ty, radius + tz };
		float _uv[2] = { s, 0 };
		memcpy(outV[k1 - 1].position, _pos, sizeof(_pos));
		// normal = Vector3(0, 0, 1);
		memcpy(outV[k1 - 1].vtxcolor, DefaultVtxColor, sizeof(DefaultVtxColor));
		memcpy(outV[k1 - 1].uv, _uv, sizeof(_uv));

		float _pos2[3] = { tx, ty, -radius + tz };
		float _uv2[2] = { s, 1 };
		memcpy(outV[k1 + ydiv - 1].position, _pos2, sizeof(_pos2));
		// normal = Vector3(0, 0, -1);
		memcpy(outV[k1 + ydiv - 1].vtxcolor, DefaultVtxColor, sizeof(DefaultVtxColor));
		memcpy(outV[k1 + ydiv - 1].uv, _uv2, sizeof(_uv2));
	}

	memcpy(outV[xdiv * (ydiv + 1)].position, outV[0].position, sizeof(float)*3);
	memcpy(outV[xdiv * (ydiv + 1)].vtxcolor, outV[0].vtxcolor, sizeof(DefaultVtxColor));
	memcpy(outV[xdiv * (ydiv + 1)].uv, outV[0].uv, sizeof(float)*2);

	memcpy(outV[xdiv * (ydiv + 1) + ydiv].position, outV[ydiv].position, sizeof(float) * 3);
	memcpy(outV[xdiv * (ydiv + 1) + ydiv].vtxcolor, outV[ydiv].vtxcolor, sizeof(DefaultVtxColor));
	memcpy(outV[xdiv * (ydiv + 1) + ydiv].uv, outV[ydiv].uv, sizeof(float) * 2);

	long ii = 0;
	for (long i = 0; i < xdiv; ++i)
	{
		const long k = i * (ydiv + 1);
		outI[ii + 0] = k;
		outI[ii + 1] = k + 1;
		outI[ii + 2] = k + ydiv + 2;
		ii += 3;

		for (long j = 1; j < ydiv - 1; ++j)
		{
			outI[ii + 0] = k + j;
			outI[ii + 1] = k + j + 1;
			outI[ii + 2] = k + j + ydiv + 2;
			outI[ii + 3] = k + j;
			outI[ii + 4] = k + j + ydiv + 2;
			outI[ii + 5] = k + j + ydiv + 1;
			ii += 6;
		}

		outI[ii + 0] = k + ydiv - 1;
		outI[ii + 1] = k + ydiv;
		outI[ii + 2] = k + ydiv * 2;
		ii += 3;
	}

	// Double texcoords
	for (int i = 0; i < VerticesCPU.NumElements; ++i)
	{
		outV[i].uv[0] *= 4;
		outV[i].uv[1] *= 2;
	}

	return true;
}