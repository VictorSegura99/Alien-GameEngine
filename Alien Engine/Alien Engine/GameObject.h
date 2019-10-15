#pragma once

#include "Globals.h"
#include "Component.h"
#include <vector>
#include <string>
#include "MathGeoLib/include/MathGeoLib.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	void Enable();
	void Disable();
	bool IsEnabled();

	void Draw();

	void AddComponent(Component* component);
	Component* GetComponent(const ComponentType& type);
	void AddChild(GameObject* child);

	void SetName(const char* name);
	const char* GetName();

	bool IsSelected();
	bool IsParentSelected();
	void ChangeSelected(const bool& select);

	void SayChildrenParentIsEnabled(const bool& enabled);
	bool IsParentEnabled();

	void ScaleNegative(const bool& is_negative);

	void ChangeWireframe(const bool& wireframe);
	void ChangeMeshView(const bool& wireframe);
	void ChangeVertexNormalsView(const bool& wireframe);
	void ChangeFaceNormalsView(const bool& wireframe);

	bool HasChildren();
	
		
private:

	void SayChildrenParentIsSelected(const bool& selected);

public:

	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent = nullptr;
	bool enabled = true;

	float3 size{ 0,0,0 };

private:

	bool parent_selected = false;
	bool selected = false;
	bool parent_enabled = true;

	std::string name;
};