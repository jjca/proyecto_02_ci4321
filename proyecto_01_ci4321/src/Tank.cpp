#include "Tank.h"

Tank::Tank()
{
	body = Cube(5.0, 2.0, 5.0);
	body.SetupGL();
	
	glm::vec3 topPos = body.position + glm::vec3(0.0, 1, 0.0);
	top = Sphere(3, 36, 18, topPos);

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

		wheels[i] = Sphere(0.1, 36, 18, wheelPos);
		wheels[i].SetupGL();
	}
}

void Tank::Draw(const Shader& ourShader)
{
	body.Draw(ourShader);
	top.Draw(ourShader);

	//for (int i = 0; i < 4; i++) {
	//	wheels[i].Draw(ourShader);
	//}
}

void Tank::Clear()
{
	body.CleanGL();
	top.CleanGL();

	for (int i = 0; i < 4; i++) {

		wheels[i].CleanGL();

	}
}
