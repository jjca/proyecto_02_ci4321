#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <numbers>
#include "Shader.h"

using namespace std;

const double PI = numbers::pi;

class Geometry
{
public:
	unsigned int VBO, VAO;
	std::vector<float> attributes;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 pivot;
	glm::vec3 size; // width, height, depth

	virtual void Load() = 0;
	virtual void Clean() = 0;
	virtual void Draw(const Shader& shader) = 0;
	virtual void Bind(unsigned int textureID) = 0;

	inline void SetPosition(glm::vec3 newPos) 
	{
		position = newPos;

	};

	inline glm::vec3 getPosition()
	{
		return position;
	};

	inline void SetRotation(glm::vec3 newRot)
	{
		rotation = newRot;

	};

	inline void setPivot(glm::vec3 newPivot) 
	{
		pivot = newPivot;
	};

	inline void setSize(glm::vec3 creationSize) {
		size = creationSize;
};

	inline glm::vec3 getSize() {
		return size;
	}

};

class Sphere : public Geometry
{
public:
	unsigned int IBO;

	std::vector<unsigned int> indices;
	float radius;
	int sectorCount;
	int stackCount;

	Sphere(float radius = 1.0, int sectorCount = 36, int stackCount = 18, bool full = true);

	void Load() override;
	void Clean() override;
	void Draw(const Shader& shader) override;
	void Bind(unsigned int textureID) override;
	void moveForward();
	void moveBackwards();
};

class Cube : public Geometry
{
public:

	float width;
	float height;
	float depth;

	Cube(float width = 1.0, float height = 1.0, float depth = 1.0);

	void Load() override;
	void Clean() override;
	void Draw(const Shader& shader) override;
	void Bind(unsigned int textureID) override;
	void moveForward();
	void moveBackwards();
	void moveRight();
	void moveLeft();
};

class Cubemap : public Cube
{
public:

	Cubemap(float width = 1.0, float height = 1.0, float depth = 1.0) :Cube(width, height, depth) {}

	void Bind(unsigned int textureID) override;

};

class Cylinder : public Geometry
{
public:
	unsigned int IBO;

	std::vector<float> unitCircleVertices;
	std::vector<unsigned int> indices;
	
	float radius;
	float height;
	float sectorCount;	


	Cylinder(float radius = 1.0, float height = 1.0, int sectorCount = 36);

	void Load() override;
	void Clean() override;
	void Draw(const Shader& shader) override;
	void DrawCanon(const Shader& shader);
	void DrawProjectile(const Shader& shader, glm::vec3 canonPosition);
	void Bind(unsigned int textureID) override;
	void moveForward();
	void moveBackwards();
};


#endif