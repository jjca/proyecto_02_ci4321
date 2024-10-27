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

	virtual void SetupGL() = 0;
	virtual void CleanGL() = 0;
	virtual void Draw(const Shader& ourShader) = 0;

};

class Sphere : public Geometry
{
public:
	unsigned int IBO;

	std::vector<int> indices;
	float radius;
	int sectorCount;
	int stackCount;

	Sphere(float radius = 1.0, int sectorCount = 36, int stackCount = 18);

	void SetupGL() override;
	void CleanGL() override;
	void Draw(const Shader& ourShader) override;
};

class Cube : public Geometry
{
public:

	float width;
	float height;

	Cube(float width = 1.0f, float height = 1.0f, float depth = 1.0f);

	void SetupGL() override;
	void CleanGL() override;
	void Draw(const Shader& ourShader) override;
};


#endif