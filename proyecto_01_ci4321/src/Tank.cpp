#include "Tank.h"

Tank::Tank()
{
	body = Cube(5.0, 2.0, 5.0);
	body.SetupGL();
	
	glm::vec3 topPos = body.position + glm::vec3(0.0, 1, 0.0);
	top = Sphere(1.5f, 36, 18, topPos);
	top.SetupGL();
	canon = Cylinder(0.5f, 3.0f, 64, top.position + glm::vec3(0.0, 0.75, 1.5));
	canon.SetupGL();
	for (int i = 0; i < 4; i++) {

		glm::vec3 wheelPos = body.position;
		
		switch (i)
		{
		case 0:
			wheelPos += glm::vec3(body.width / 2, -1.0, body.depth / 2);
			break;
		case 1:
			wheelPos += glm::vec3(body.width / 2, -1.0, -body.depth / 2);
			break;
		case 2:
			wheelPos += glm::vec3(-body.width / 2, -1.0, body.depth / 2);
			break;
		case 3:
			wheelPos += glm::vec3(-body.width / 2, -1.0, -body.depth / 2);
			break;
		}

		wheels[i] = Sphere(0.5, 36, 18, wheelPos);
		wheels[i].SetupGL();
	}
}

void Tank::Draw(const Shader& ourShader)
{
	canon.Draw(ourShader);
	body.Draw(ourShader);
	top.Draw(ourShader);

	for (int i = 0; i < 4; i++) {
		wheels[i].Draw(ourShader);
	}
}

void Tank::Clear()
{
	canon.CleanGL();
	body.CleanGL();
	top.CleanGL();

	for (int i = 0; i < 4; i++) {

		wheels[i].CleanGL();

	}
}
