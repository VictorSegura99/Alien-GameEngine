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

	// Subdivions only for Sphere and Rock, and extra only for Rock seed
	Primitive* CreatePrimitive(const PrimitiveType &type, const float &position_x=0, const float &position_y=0, const float &position_z=0, const int &subdivions=5, const int &extra=0);

	void DrawPrimitive();

public:
	std::vector<Object*> objects;
};