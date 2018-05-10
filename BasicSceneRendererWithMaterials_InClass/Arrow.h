#pragma once
#include "Entity.h"
#include "Prefabs.h"
#include "glshell.h"
#include "Camera.h"
#include <algorithm>
#include "AABB.h"


class Arrow :
	public Entity
{
public:
	Arrow();
	Arrow(const Mesh* mesh, Material* material, const Transform& transform);
	Arrow(const Mesh* mesh, Material* material, const Transform& transform, glm::vec3 min, glm::vec3 max);

	void update(float dt);
	void draw();
	void shoot();
	bool isIntersecting(Entity* entity);

	Entity* targetEntity;
	Mesh* directionRay;
	bool isMoving;
	float elapsedTime = 0;
};

