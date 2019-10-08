#pragma once

#include "Module.h"
#include "Globals.h"
#include "Objects.h"
#include "Primitive.h"

#include <vector>

class ModuleObjects : public Module
{
public:
	ModuleObjects(bool start_enabled = true);
	virtual ~ModuleObjects();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
	void LoadConfig(JSONfilepack*& config);
	void SaveConfig(JSONfilepack*& config);

	Primitive* CreatePrimitive(const PrimitiveType& type);

	void ChangeWireframeMode();
	void ChangeViewMeshMode();

	void DeleteAllObjects();

public:
	std::vector<Object*> objects;

	bool wireframe_mode = false;
	bool view_mesh_mode = true;
	uint vertex_normal_length = 5;
	uint face_normal_length = 1;
	bool draw_vertex_normals = true;
	bool draw_face_normals = true;

};