#pragma once
#include "Mesh.h"

class AABB
{
public:
	AABB();
	~AABB();

	Mesh*         mMesh;
	Mesh*         mMesh2;
	Mesh* active;
	glm::vec3 min;
	glm::vec3 max;
};

