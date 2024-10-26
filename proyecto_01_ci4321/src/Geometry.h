#pragma once

#include <vector>
#include <numbers>

using namespace std;

double PI = numbers::pi;

class Geometry
{
public:

	std::vector<float> vertices;
	std::vector<float> texCoords;

	virtual void Draw() = 0;

};

class Sphere : public Geometry
{
public:

	std::vector<float> normals;
	float radius;
	int sectorCount;
	int stackCount;

	Sphere(float radius, int sectorCount, int stackCount);

	void Draw();

};

class Cube : public Geometry
{
public:

	float width;
	float height;

	Cube(float width, float height, float depth);

	void Draw();

};


