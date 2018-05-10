#include "Arrow.h"
#include <iostream>


Arrow::Arrow()
{
	this->isMoving = false;

	glm::vec3 start = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 end = glm::vec3(0.0f, 0.0f, 40.0f);

	//Direction Ray Mesh
	directionRay = CreateLine(start, end);

	//Create ARROW mesh, material and transform
	mMesh = LoadMesh("meshes/arrow3.obj");
	Texture* tex = new Texture("textures/water_drops_on_metal.tga", GL_REPEAT, GL_LINEAR);
	mMaterial = new Material(tex);
	mMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	mMaterial->shininess = 255;
	mMaterial->emissive = glm::vec3(0.1f, 0.1f, 0.1f);

	mTransform = Transform(0.0f, 0.0f, -10.0f);
	mMin = start, 
	mMax = end;

	//Create child targetEntity
	Texture* target = new Texture("textures/target.tga", GL_REPEAT, GL_LINEAR);
	Material* myMaterial = new Material(target);
	float width = 10.0f;
	glm::vec3 min = glm::vec3(-0.5f, -0.5f, -0.5f) * width;
	glm::vec3 max = glm::vec3(0.5f, 0.5f, 0.5f) * width;
	Mesh* cubeMesh = CreateTexturedCube(width);
	targetEntity = new Entity(cubeMesh, myMaterial, Transform(0.0f, 0.0f, 30.0f), min, max);
}

Arrow::Arrow(const Mesh* mesh, Material* material, const Transform& transform) {
	mTransform = transform;
	mMesh = mesh;
	mMaterial = material;
}

Arrow::Arrow(const Mesh* mesh, Material* material, const Transform& transform, glm::vec3 min, glm::vec3 max) {
	mTransform = transform;
	mMesh = mesh;
	mMaterial = material;
	mMin = min;
	mMax = max;
}

bool Arrow::isIntersecting(Entity* entity) {
	glm::vec3 org = this->getMin();
	glm::vec3 dir = glm::normalize(this->getMax() - this->getMin());

	glm::vec3 lb = entity->getMin();  //left bottom of box
	glm::vec3 rt = entity->getMax();  //right top of box

	glm::vec3 dirfrac;
	float t;

	// r.dir is unit direction vector of ray
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;
	// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
	// r.org is origin of ray
	float t1 = (lb.x - org.x)*dirfrac.x;
	float t2 = (rt.x - org.x)*dirfrac.x;
	float t3 = (lb.y - org.y)*dirfrac.y;
	float t4 = (rt.y - org.y)*dirfrac.y;
	float t5 = (lb.z - org.z)*dirfrac.z;
	float t6 = (rt.z - org.z)*dirfrac.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0)
	{
		t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		t = tmax;
		return false;
	}

	t = tmin;
	return true;
}

void Arrow::update(float dt)
{
	Entity* entity = this->targetEntity;
	bool val = isIntersecting(entity);
	std::cout << val << std::endl;

	if (this->isMoving)
	{
		//go straight
		elapsedTime += dt;

		//this->rotate(90.0f, glm::vec3(0, 1.0f, 0));

		//go in the direction of arrow's forward vector
		glm::mat4 world = this->getWorldMatrix();
		glm::vec3 dir(world[2]);  //forward unit vector

		float speed = 1.0f;
		this->translate(dir * speed);
	}
	if (elapsedTime > 0.7f)
	{
		isMoving = false;
		elapsedTime = 0;
		this->setPosition(glm::vec3(0.0f, 0.0f, -10.0f));
	}
}

void Arrow::draw()
{

}

void Arrow::shoot()
{
	
	

}
