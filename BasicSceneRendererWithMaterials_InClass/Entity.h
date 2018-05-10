#ifndef ENTITY_H_
#define ENTITY_H_

#include "glshell.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "AABB.h"
#include "Prefabs.h"



class Entity {
    
public:
	Entity()
	{}

    Entity(const Mesh* mesh, Material* material, const Transform& transform)
        : mTransform(transform)
        , mMesh(mesh)
        , mMaterial(material)

    { }

	Entity(const Mesh* mesh, Material* material, const Transform& transform, glm::vec3 min, glm::vec3 max)
		: mTransform(transform)
		, mMesh(mesh)
		, mMaterial(material)
		, mMin(min)
		, mMax(max)

	{ }

	Transform           mTransform;
	const Mesh*         mMesh;
	Material*     mMaterial;

	glm::vec3     mMin;
	glm::vec3     mMax;

	AABB* boundingBox = new AABB;

	bool hasBoundingBox = false;

    //
    // a bunch of useful getters
    //

    const Mesh*         getMesh() const         { return mMesh; }
    Material*			getMaterial() const     { return mMaterial; }
    const Transform&    getTransform() const    { return mTransform; }
    const glm::vec3&    getPosition() const     { return mTransform.position; }
    const glm::quat&    getOrientation() const  { return mTransform.orientation; }

	glm::vec3 getMin() { 
		glm::vec4 worldPos = mTransform.toMatrix() * glm::vec4(mMin.x, mMin.y, mMin.z, 1);
		return glm::vec3(worldPos);
	}
	
	glm::vec3 getMax() { 
		glm::vec4 worldPos = mTransform.toMatrix() * glm::vec4(mMax.x, mMax.y, mMax.z, 1); 
		return glm::vec3(worldPos);
	}

	void createBoundingBox()
	{
		float maxX=0, minX=0, maxY=0, minY=0, maxZ=0, minZ=0;

		for (VertexPositionNormal v : this->getMesh()->mVertices)
		{
			if (v.x > maxX) maxX = v.x;
			if (v.x < minX) minX = v.x;
			
			if (v.y > maxY) maxY = v.y;
			if (v.y < minY) minY = v.y;
			
			if (v.z > maxZ) maxZ = v.z;
			if (v.z < minZ) minZ = v.z;
		}

		this->mMin = glm::vec3(minX, minY, minZ);
		this->mMax = glm::vec3(maxX, maxY, maxZ);
			
		//build mesh

		float width = maxX - minX;
		float height = maxY - minY;
		float depth = maxZ - minZ;

		//green
		this->boundingBox->mMesh = CreateWireframeBox(height, width, depth);

		//red
		this->boundingBox->mMesh2 = CreateWireframeBox2(height, width, depth);
		
		this->boundingBox->active = this->boundingBox->mMesh;

		this->hasBoundingBox = true;
	}
	

    //
    // The renderer can use the getWorldMatrix method to obtain the modeling matrix for this entity.
    // The modeling matrix combines the position and orientation transformations.
    //
    glm::mat4           getWorldMatrix() const  { return mTransform.toMatrix(); }

    //
    // setters for position and orientation
    //

    void setTransform(const Transform& transform)
    {
        mTransform = transform;
    }

    void setPosition(const glm::vec3& pos)
    {
        mTransform.position = pos;
    }

    void setPosition(float x, float y, float z)
    {
        mTransform.position.x = x;
        mTransform.position.y = z;
        mTransform.position.z = z;
    }

    void setOrientation(const glm::quat& orientation)
    {
        mTransform.orientation = orientation;
    }

    //
    // rotation about an arbitrary axis
    //

    void rotate(float angle, const glm::vec3& axis)
    {
        // combine rotation with existing orientation using quaternion multiplication
        glm::quat q = glm::angleAxis(glm::radians(angle), axis);
        mTransform.orientation = mTransform.orientation * q;
    }

    void rotate(float angle, float x, float y, float z)
    {
        // combine rotation with existing orientation using quaternion multiplication
        glm::quat q = glm::angleAxis(glm::radians(angle), glm::vec3(x, y, z));
        mTransform.orientation = mTransform.orientation * q;
    }

    //
    // translation along world axes
    //

    void translate(const glm::vec3& disp)
    {
        mTransform.position += disp;
    }

    void translate(float dx, float dy, float dz)
    {
        translate(glm::vec3(dx, dy, dz));
    }

    //
    // translation along local axes
    //

    void translateLocal(const glm::vec3& disp)
    {
        // multiply the displacement by our orientation quaternion
        translate(mTransform.orientation * disp);
    }

    void translateLocal(float dx, float dy, float dz)
    {
        translateLocal(glm::vec3(dx, dy, dz));
    }
};

#endif
