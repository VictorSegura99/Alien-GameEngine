#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"
#include "Color.h"

class ComponentMesh : public Component {
public:

	ComponentMesh(GameObject* attach);
	virtual ~ComponentMesh();

	void DrawPolygon();
	void DrawOutLine();
	void DrawMesh();
	void DrawVertexNormals();
	void DrawFaceNormals();
	void DrawInspector();
	void DrawGlobalAABB();
	//void DrawOBB();

	void Reset();
	void SetComponent(Component* component);

	AABB GetGlobalAABB();

private:

	AABB GenerateAABB();

public:

	// buffers id
	uint id_index = 0;
	uint id_vertex = 0;
	uint id_normals = 0;
	uint id_uv = 0;
	// buffers size
	uint num_index = 0;
	uint num_vertex = 0;
	uint num_faces = 0;
	// buffers
	uint* index = nullptr;
	float* vertex = nullptr;
	float* normals = nullptr;
	float* uv_cords = nullptr;
	float* center_point_normal = nullptr;
	float* center_point = nullptr;

	bool view_mesh = false;
	bool wireframe = false;
	bool view_vertex_normals = false;
	bool view_face_normals = false;

	bool draw_AABB = true;
	bool draw_OBB = true;

	Color OBB_color{ 0,1,0 };

private:

	AABB local_aabb;
	OBB obb;
	AABB global_aabb;
};