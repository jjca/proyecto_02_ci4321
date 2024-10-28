#include "Tank.h"
#include "stb_image/stb_image.h"

Tank::Tank()
{
	body = new Cube(4.0, 1.0, 4.25);
	body->SetupGL();
	
	glm::vec3 topPos = body->position + glm::vec3(0.0, 0.5, -0.25);
	top = new Sphere(1.25f, 36, 18, false);
	top->SetPosition(topPos);
	top->SetupGL();

	glm::vec3 canonPos = top->position + glm::vec3(0.0, 0.5, 1.0);
	canon = new Cylinder(0.25f, 3.0f, 64);
	canon->SetPosition(canonPos);
	canon->SetupGL();

	for (int i = 0; i < wheelsCount; i++) {

		glm::vec3 wheelPos = body->position;
		
		switch (i)
		{
		case 0:
			wheelPos += glm::vec3(0, -1.0, -body->depth/2);
			break;
		case 1:
			wheelPos += glm::vec3(0, -1.0, -body->depth/4);
			break;
		case 2:
			wheelPos += glm::vec3(0, -1.0, 0);
			break;
		case 3:
			wheelPos += glm::vec3(0, -1.0, body->depth / 4);
			break;
		case 4:
			wheelPos += glm::vec3(0, -1.0, body->depth / 2);
			break;
		}

		wheels[i] = new Cylinder(0.52, 4, 18);
		wheels[i]->SetPosition(wheelPos);
		wheels[i]->SetRotation(glm::vec3(0.0, glm::radians(90.0), 0.0));

		wheels[i]->SetupGL();

		float centerHeight = wheels[i]->height/2;
		float faceRadius = wheels[i]->radius/2;

		for (int j = boltsCount*i; j < boltsCount*(i + 1); j++) {
			cout << j << endl;
			glm::vec3 boltPos = wheels[i]->position;

			switch (j % boltsCount)
			{
			case 0:
				boltPos += glm::vec3(-centerHeight, 0.0, 0.0);
				break;
			case 1:
				boltPos += glm::vec3(centerHeight, 0.0, 0.0);
				break;
			}

			bolts[j] = new Cube(0.1, 0.4, 0.4);
			bolts[j]->SetPosition(boltPos);

			bolts[j]->SetupGL();
		}
	}
}

void Tank::Draw(const Shader& shader)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture3);

	canon->Draw(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);

	body->Draw(shader);
	top->Draw(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	for (int i = 0; i < wheelsCount; i++) {
		wheels[i]->Draw(shader);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture3);
	for (int j = 0; j < boltsCount*wheelsCount; j++) {
		bolts[j]->Draw(shader);
	}


}

void Tank::Clear()
{
	canon->CleanGL();
	body->CleanGL();
	top->CleanGL();

	for (int i = 0; i < wheelsCount; i++) {

		wheels[i]->CleanGL();

	}

	for (int j = 0; j < boltsCount*wheelsCount; j++) {
		bolts[j]->CleanGL();
	}

}

void Tank::moveForward(const Shader& ourShader) {
	
	body->moveForward();
	canon->moveForward();
	top->moveForward();
	for (int i = 0; i < wheelsCount; ++i) {
		wheels[i]->moveForward();
		wheels[i]->rotation -= normalize(glm::vec3(0.0f, 0.0f, 1.0f)) * 0.05f;
	}
	for (int i = 0; i < boltsCount*wheelsCount; i++) {
		bolts[i]->moveForward();
		bolts[i]->rotation += normalize(glm::vec3(1.0f, 0.0f, 0.0f)) * 0.05f;
	}
	
}

void Tank::moveBackwards(const Shader& ourShader) {

	body->moveBackwards();
	canon->moveBackwards();
	top->moveBackwards();
	for (int i = 0; i < wheelsCount; ++i) {
		wheels[i]->moveBackwards();
		wheels[i]->rotation += normalize(glm::vec3(0.0f, 0.0f, 1.0f)) * 0.05f;
	}
	for (int i = 0; i < boltsCount * wheelsCount; i++) {
		bolts[i]->moveBackwards();
		bolts[i]->rotation -= normalize(glm::vec3(1.0f, 0.0f, 0.0f)) * 0.05f;
	}

}

void Tank::LoadTextures(Shader& shader)
{
	// Texturas
	glGenTextures(1, &texture1);
	glGenTextures(1, &texture2);
	glGenTextures(1, &texture3);

	// Textura 1
	// Seteamos a la textura 1 como textura actual
	glBindTexture(GL_TEXTURE_2D, texture1);

	// Seteamos los parametros de wrapping de la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Steamos parametros de filtro en la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Cargamos la imagen, creamos la textura y generamos los mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // Es necesario girar la textura en el eje Y, por como funciona OpenGL
	unsigned char* data = stbi_load("resources/textures/metal_green.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// Textura 2
	// Seteamos a la textura 2 como textura actual
	glBindTexture(GL_TEXTURE_2D, texture2);

	// Seteamos los parametros de wrapping de la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Steamos parametros de filtro en la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Cargamos la imagen, creamos la textura y generamos los mipmaps
	stbi_set_flip_vertically_on_load(true); // Es necesario girar la textura en el eje Y, por como funciona OpenGL
	data = stbi_load("resources/textures/blocks.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// Textura 3
	// Seteamos a la textura 3 como textura actual
	glBindTexture(GL_TEXTURE_2D, texture3);

	// Seteamos los parametros de wrapping de la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Steamos parametros de filtro en la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Cargamos la imagen, creamos la textura y generamos los mipmaps
	stbi_set_flip_vertically_on_load(true); // Es necesario girar la textura en el eje Y, por como funciona OpenGL
	data = stbi_load("resources/textures/metal.png", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	shader.use();
	shader.setInt("texture1", 0);
}


void Tank::moveCanonUp(float deltaTime) {
	
	if (canon->rotation.x <= -0.70f) {
		canon->rotation.x = -0.70f;
	}
	canon->rotation -= normalize(glm::vec3(1.0f, 0.0f, 0.0f)) * deltaTime;

	cout << canon->rotation.x << " " << canon->rotation.y << " " << canon->rotation.z << " " << endl;
}

void Tank::moveCanonDown(float deltaTime) {
	if (canon->rotation.x >= 0.00f) {
		canon->rotation.x = 0.00f;
	}
	canon->rotation += normalize(glm::vec3(1.0f, 0.0f, 0.0f)) * deltaTime;
}

void Tank::moveCanonRight(float deltaTime) {

	if (canon->rotation.y <= -0.60f) {
		canon->rotation.y = -0.60f;
	}
	canon->rotation -= normalize(glm::vec3(0.0f, 1.0f, 0.0f)) * deltaTime;
}

void Tank::moveCanonLeft(float deltaTime) {
	if (canon->rotation.y >= 0.60f) {
		canon->rotation.y = 0.60f;
	}
	canon->rotation += normalize(glm::vec3(0.0f, 1.0f, 0.0f)) * deltaTime;
}