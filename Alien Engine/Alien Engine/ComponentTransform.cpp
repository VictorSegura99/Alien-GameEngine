#include "ComponentTransform.h"
#include "GameObject.h"
#include "imgui/imgui.h"

ComponentTransform::ComponentTransform(GameObject* attach, const float3& pos, const Quat& rot, const float3& scale) : Component(attach)
{
	local_position = pos;
	local_rotation = rot;
	local_scale = scale;
	local_transformation = float4x4::FromTRS(local_position, local_rotation, local_scale);

	if (game_object_attached->parent != nullptr) {
		ComponentTransform* tr = (ComponentTransform*)game_object_attached->parent->GetComponent(ComponentType::TRANSFORM);
		if (tr != nullptr) global_transformation = tr->global_transformation * local_transformation;
	}
	else
		global_transformation = local_transformation;

	type = ComponentType::TRANSFORM;
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::SetLocalPosition(const float& x, const float& y, const float& z)
{
	local_position.x = x;
	local_position.y = y;
	local_position.z = z;

	RecalculateTransform();
}

const float3& ComponentTransform::GetLocalPosition() const
{
	return local_position;
}

const float3& ComponentTransform::GetGlobalPosition() const
{
	float3 pos, scale;
	Quat rot;

	global_transformation.Decompose(pos, rot, scale);

	return pos;
}

void ComponentTransform::SetLocalScale(const float& x, const float& y, const float& z)
{
	local_scale.x = x;
	local_scale.y = y;
	local_scale.z = z;

	RecalculateTransform();
}

const float3& ComponentTransform::GetLocalScale() const
{
	return local_scale;
}

const float3& ComponentTransform::GetGlobalScale() const
{
	float3 pos, scale;
	Quat rot;

	global_transformation.Decompose(pos, rot, scale);

	return scale;
}

void ComponentTransform::SetLocalRotation(const float& x, const float& y, const float& z, const float& angle)
{
	local_rotation.x = x;
	local_rotation.y = y;
	local_rotation.z = z;
	local_rotation.w = angle;

	RecalculateTransform();
}

const Quat& ComponentTransform::GetLocalRotation() const
{
	return local_rotation;
}

const Quat& ComponentTransform::GetGlobalRotation() const
{
	float3 pos, scale;
	Quat rot;

	global_transformation.Decompose(pos, rot, scale);

	return rot;
}

void ComponentTransform::RecalculateTransform()
{	
	local_transformation = float4x4::FromTRS(local_position, local_rotation, local_scale);

	if (game_object_attached->parent != nullptr) {
		ComponentTransform* tr = (ComponentTransform*)game_object_attached->parent->GetComponent(ComponentType::TRANSFORM);
		if (tr != nullptr)
			global_transformation = tr->global_transformation * local_transformation;
		else
			global_transformation = local_transformation;
	}
	else {
		global_transformation = local_transformation;
	}

	std::vector<GameObject*>::iterator item = game_object_attached->children.begin();
	for (; item != game_object_attached->children.end(); ++item) {
		if (*item != nullptr) {
			ComponentTransform* tr = (ComponentTransform*)(*item)->GetComponent(ComponentType::TRANSFORM);
			if (tr != nullptr) tr->RecalculateTransform();
		}
	}
}


void ComponentTransform::DrawInspector()
{
	ImGui::Spacing();

	ImGui::Text("Object");
	ImGui::SameLine();

	ImGui::Checkbox("##ObjectActive", &game_object_attached->enabled);
	ImGui::SameLine();

	static char name[30];
	memcpy(name, game_object_attached->GetName(), 30);

	if (ImGui::InputText("##ObjectName", name, 30, ImGuiInputTextFlags_AutoSelectAll)) {
		game_object_attached->SetName(name);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Spacing();
		ImGui::Text("Position  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(1);
		if (ImGui::DragFloat("X", &local_position.x, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(2);
		if (ImGui::DragFloat("Y", &local_position.y, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		ImGui::PushID(3);
		if (ImGui::DragFloat("Z", &local_position.z, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::PopID();
		ImGui::Spacing();

		ImGui::Text("Rotation  ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("X", &local_rotation.x, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("Y", &local_rotation.y, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("Z", &local_rotation.z, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::Spacing();

		ImGui::Text("Scale     ");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("X", &local_scale.x, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("Y", &local_scale.y, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::SameLine();
		ImGui::SetNextItemWidth(70);
		if (ImGui::DragFloat("Z", &local_scale.z, 0.5F)) {
			RecalculateTransform();
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}
	
}



