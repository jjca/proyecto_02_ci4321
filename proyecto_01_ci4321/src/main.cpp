#include "Shader.h"
#include <cstdlib>
#include <map>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image/stb_image.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <filesystem>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Geometry.h"
#include "Tank.h"

using namespace std;

/* Variables globales */
const int WIDTH = 1280;
const int HEIGHT = 720;

// Dónde está la cámara
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
// el fente
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// vector del eje derech
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
int lastX = WIDTH / 2;
int lastY = HEIGHT / 2;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool CheckCollision(Cube& one, Tank& two);
bool CheckCollisionProjectile(Cube& one, Cylinder& two);

struct Character {
	unsigned int TextureID; // ID
	glm::ivec2	 Size; // Tamaño 
	glm::ivec2	 Bearing; // Offset desde l alínea base hasta el top o la izq
	unsigned int Advance; // Offset hacia el siguiente glifo
};


map<GLchar, Character> Characters;

// Metodos
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {


	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

unsigned int loadSkybox(vector<std::string> faces)
{

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
};

int main(void) {

	GLFWwindow* window;

	/*  Inicializa libreria de glfw */
	if (!glfwInit()) {
		return -1;
	}

	/* Creacion de ventana emergente y contexto */
	window = glfwCreateWindow(WIDTH, HEIGHT, "Camionetica poderosa", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Indicacion de ventana/lienzo actual para glfw */
	glfwMakeContextCurrent(window);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	/* Iniciacion de libreria de glew */
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}

	// Habilitamos la profundidad
	glEnable(GL_DEPTH_TEST);

	Shader shader("src/Shaders/VertexShader.vs", "src/Shaders/FragmentShader.fs");

	Tank tank;
	Cube cube = Cube(2.0f, 2.0f, 2.0f);
	cube.SetPosition(glm::vec3(0.0f, 0.0f, 15.0f));
	cube.SetupGL();

	Cube floor = Cube(100.0f, 0.1f, 100.0f);
	floor.SetPosition(glm::vec3(0.0f, -1.6f, 0.0f));
	floor.SetupGL();

	Sphere sphere2 = Sphere(1.0f, 36, 18, true);
	sphere2.SetPosition(glm::vec3(3.0f, 0.0f, 15.0f));
	sphere2.SetupGL();

	//Cylinder cylinder = Cylinder(2.0f, 3.0f, 36, glm::vec3(0.0f, 0.0f, 3.0f));

	//cylinder.SetupGL();
	tank.LoadTextures(shader);

	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", projection);

	// Skybox area
	Shader skyboxShader("src/Shaders/SkyboxVertexShader.vs", "src/Shaders/SkyboxFragmentShader.fs");

	// Inicializamos freetype

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not initialize FT " << std::endl;
		return -1;
	}
	// Cargamos la fuente. Por pruebas usaremos Arial
	FT_Face face;

	if (FT_New_Face(ft, "resources/fonts/arial.ttf", 0, &face)) {
		cout << "ERROR::FREETYPE: Could not load font" << endl;
		return -1;
	}
	else {
		// Ajustamos el tamaño de la fuente
		FT_Set_Pixel_Sizes(face, 0, 48);
		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			return -1;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (unsigned char c = 0; c < 128; c++) {
			// load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				cout << "ERROR::FREETYTPE: Failed to load Glyph" << endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	
	Cube skyboxCube = Cube(100.0f, 100.0f, 100.0f);
	// skyboxCube.SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
	skyboxCube.SetupGL();

	vector<std::string> faces
	{
		"resources/textures/right.png",
			"resources/textures/left.png",
			"resources/textures/down.png",
			"resources/textures/up.png",
			"resources/textures/front.png",
			"resources/textures/back.png"
	};

	unsigned int cubemapTexture = loadSkybox(faces);

	/* Ciclo hasta que el usuario cierre la ventana */
	while (!glfwWindowShouldClose(window))
	{
		// Clock
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);


		/* Limpieza del buffer y el buffer de profundidad */
		glClearColor(0.761f, 1.0f, 0.992f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::lookAt(
			cameraPos,
			cameraPos + cameraFront,
			cameraUp
		);


		shader.use();

		// Aplicamos la matriz del view (hacia donde esta viendo la camara)
		//glm::mat4 view = glm::mat4(1.0f);

		shader.setMat4("view", view);
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
			tank.moveCanonUp(deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.moveCanonDown(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.moveCanonRight(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.moveCanonLeft(deltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.moveForward(shader);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.moveBackwards(shader);
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.rotateBodyLeft(deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.rotateBodyRight(deltaTime);
			
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			tank.fire();

		}

		if (CheckCollision(cube, tank)) {
			cube.CleanGL();
		}

		if (tank.hasBeenShotF()) {
			Cylinder *projectile = tank.getProjectile();
			if (CheckCollisionProjectile(cube, *projectile)) {
				projectile->CleanGL();
				cube.CleanGL();
				tank.setHasBeenShot();
			}
			if (projectile->position.z >= 40.0f) {
				projectile->CleanGL();
				tank.setHasBeenShot();
			}
		}

		//cylinder.Draw(ourShader);
		floor.Draw(shader);
		sphere2.Draw(shader);
		tank.Draw(shader);
		// glBindVertexArray(0);

		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();

		view = glm::mat4(glm::mat3(glm::lookAt(
			cameraPos,
			cameraPos + cameraFront,
			cameraUp
		)));

		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		skyboxCube.Draw(skyboxShader);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);


		/* Intercambio entre buffers */
		glfwSwapBuffers(window);

		/* Recepcion de eventos */
		glfwPollEvents();

	}

	// Borramos el contenido de los buffers
	tank.Clear();

	/* Cierre de glfw */
	glfwTerminate();

	return 0;
}


bool CheckCollision(Cube& one, Tank& two) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = one.position.x + one.size.x >= two.getPosition().x &&
		two.getPosition().x + two.getSize().x >= one.position.x;
	// collision y-axis?
	bool collisionY = one.position.y + one.size.y >= two.getPosition().y &&
		two.getPosition().y + two.getSize().y >= one.position.y;

	bool collisionZ = one.position.z + one.size.z >= two.getPosition().z &&
		two.getPosition().z + two.getSize().z >= one.position.z;
	// collision only if on both axes
	return collisionX && collisionY && collisionZ;
}

bool CheckCollisionProjectile(Cube& one, Cylinder& two) // AABB - AABB collision
{
	// collision x-axis?
	bool collisionX = one.position.x + one.size.x >= two.getPosition().x &&
		two.getPosition().x + two.getSize().x >= one.position.x;
	// collision y-axis?
	bool collisionY = one.position.y + one.size.y >= two.getPosition().y &&
		two.getPosition().y + two.getSize().y >= one.position.y;

	bool collisionZ = one.position.z + one.size.z >= two.getPosition().z &&
		two.getPosition().z + two.getSize().z >= one.position.z;
	// collision only if on both axes
	return collisionX && collisionY && collisionZ;
}