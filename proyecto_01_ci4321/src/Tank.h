#ifndef TANK_H
#define TANK_H

#include "Geometry.h"

using namespace std;

class Tank
{
public:
	Tank();
	void Draw(const Shader& ourShader);
	void Clear();

private:
	Cube body;
	Sphere top;
	// Cylinder canon;
	Sphere wheels[4];

};

#endif

