#pragma once 

#include "ModuleObjects.h"

class Action;
class ActionDeleteObject;
class ObjZ;
class CompZ;

class ReturnZ {

public:

	enum class ReturnActions {
		UNKNOWKN = -1,

		DELETE_OBJECT,
		ADD_OBJECT,
	};

	ReturnZ() {}
	~ReturnZ() {}

	static void AddNewAction(const ReturnActions& type, void* data);
	static void GoBackOneAction();

public:

	Action* action = nullptr;

private:

	static void SetDeleteObject(GameObject* obj, ActionDeleteObject* to_fill);
	static void CreateObject(ActionDeleteObject* obj);
	
	void SetAction(const ReturnActions& type, void* data);
};



class Action {

public:

	ReturnZ::ReturnActions type = ReturnZ::ReturnActions::UNKNOWKN;
};

class ActionDeleteObject : public Action {
public:
	ObjZ* object = nullptr;
};

class ObjZ {

public:

	bool enabled = false;
	bool is_static = false;
	u64 ID = 0;
	u64 parentID = 0;
	bool to_delete = false;
	bool parent_selected = false;
	bool selected = false;
	bool parent_enabled = false;

	std::string name = "UnNamed";

	std::vector<ActionDeleteObject*> children;

	std::vector<CompZ*> comps;

};

class CompZ {
public:
	ComponentType type = ComponentType::UNKNOWN;
};

class CompMeshZ : public CompZ{
public:
	u64 objectID = 0;
	u64 resourceID = 0;
	bool view_mesh = false;
	bool wireframe = false;
	bool view_vertex_normals = false;
	bool view_face_normals = false;
	bool draw_AABB = true;
	bool draw_OBB = true;
};

class CompMaterialZ : public CompZ {
public:
	u64 objectID = 0;
	u64 resourceID = 0;
	Color color{ 1,1,1,1 };
	bool texture_activated = true;
};

class CompTransformZ : public CompZ {
public:
	u64 objectID = 0;
	float3 pos = float3::zero;
	float3 scale = float3::zero;
	Quat rot = Quat::identity;
	bool is_scale_negative = false;
};