#include "Tank.h"
#include "Bullet.h"
#include "../../Alien Engine/MathGeoLib/include/Geometry/Plane.h"

Tank::Tank() : Alien()
{
}

Tank::~Tank()
{

}

void Tank::Start()
{
	GameObject* wheels = GameObject::FindWithName("Lower_Tank");
	if (wheels != nullptr) 
	{
		wheels_transform = (ComponentTransform*)wheels->GetComponent(ComponentType::TRANSFORM);
	}

	GameObject* turret = GameObject::FindWithName("TankTurret");
	if (turret != nullptr)
	{
		turret_transform = (ComponentTransform*)turret->GetComponent(ComponentType::TRANSFORM);
	}
}

void Tank::Update()
{
	Movement();
	Rotation();
	Shoot();
}

void Tank::Shoot()
{
	// Shooting
	if (Input::GetMouseButtonDown(Input::MOUSE_LEFT_BUTTON))
	{
		GameObject* bullet_created = bullet.ConvertToGameObject({ transform->GetGlobalPosition().x,transform->GetGlobalPosition().y + 1.5f,transform->GetGlobalPosition().z });

		if (bullet_created != nullptr)
		{
			Bullet* minion = (Bullet*)bullet_created->GetComponentScript("Bullet");
			minion->bullet_direction = turret_transform->forward;
			// TODO: Recoil
		}
	}
}

void Tank::Movement()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_W))
	{
		velocity += acceleration * Time::GetDT();
	}
	if (Input::GetKeyRepeat(SDL_SCANCODE_S))
	{
		velocity -= acceleration * Time::GetDT();
	}

	velocity = Maths::Clamp(velocity, max_velocity_backward, max_velocity_forward);

	transform->SetLocalPosition(transform->GetGlobalPosition() + wheels_transform->forward * velocity * Time::GetDT());

	if (velocity > 0)
	{
		velocity -= friction_force;
		if (velocity < 0)
		{
			velocity = 0;
		}
	}
	else if (velocity < 0)
	{
		velocity += friction_force;
		if (velocity > 0)
		{
			velocity = 0;
		}
	}
}

void Tank::Rotation()
{
	if (Input::GetKeyRepeat(SDL_SCANCODE_A))
	{
		angle += turning_velocity * Time::GetDT();
		wheels_transform->SetLocalRotation(Quat::FromEulerXYZ(0, angle * Maths::Deg2Rad(), 0));
	}

	if (Input::GetKeyRepeat(SDL_SCANCODE_D))
	{
		angle -= turning_velocity * Time::GetDT();
		wheels_transform->SetLocalRotation(Quat::FromEulerXYZ(0, angle * Maths::Deg2Rad(), 0));
	}

	Plane plane = {{ transform->GetGlobalPosition().x,transform->GetGlobalPosition().y + 1.5f,transform->GetGlobalPosition().z },{0,1,0}};
	LineSegment ray = RayCreator::CreateRayScreenToWorld(Input::GetMouseX(), Input::GetMouseY(), Camera::GetCurrentCamera());
	if (ray.Intersects(plane))
	{
		float3 to_look = ray.GetPoint(ray.Distance(plane));
		turret_transform->SetLocalRotation(Quat::LookAt(turret_transform->forward, to_look - turret_transform->GetGlobalPosition(), turret_transform->up, { 0,1,0 }));
	}
}
