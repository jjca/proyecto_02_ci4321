#include <cstdlib>
#include <map>
#include <iostream>
#include <string>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <filesystem>

#include "stb_image/stb_image.h"
#include "Shader.h"
#include "Geometry.h"
#include "Tank.h"
#include "Skybox.h"
#include "IceCream.h"
#include "Text.h"

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
// Posicion de la luz
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
int lastX = WIDTH / 2;
int lastY = HEIGHT / 2;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

map<std::string,unsigned int> textures;

bool CheckCollision(Cube& one, Tank& two);
bool CheckCollisionProjectile(Cube& one, Cylinder& two);

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

unsigned int LoadTexture(std::string path)
{
	unsigned int texture;
	glGenTextures(1, &texture);

	// Seteamos a la textura como textura actual
	glBindTexture(GL_TEXTURE_2D, texture);

	// Seteamos los parametros de wrapping de la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Steamos parametros de filtro en la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Cargamos la imagen, creamos la textura y generamos los mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // Es necesario girar la textura en el eje Y, por como funciona OpenGL
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

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
	return texture;
}

void LoadTextures()
{

	textures["metalgreen"] = LoadTexture("resources/textures/metal_green.png");
	textures["block"] = LoadTexture("resources/textures/blocks.png");
	textures["metal"] = LoadTexture("resources/textures/metal.png");
	textures["ground"] = LoadTexture("resources/textures/SFloor.jpg");
	textures["choco"] = LoadTexture("resources/textures/choco.jpg");
	textures["strawberry"] = LoadTexture("resources/textures/strawberry.jpg");
	textures["vanilla"] = LoadTexture("resources/textures/vanilla.jpg");
	textures["cone"] = LoadTexture("resources/textures/cone.jpg");
	textures["cherry"] = LoadTexture("resources/textures/cherry.jpg");

	textures["metal_normal"] = LoadTexture("resources/textures/metal_normal.png");
	textures["block_normal"] = LoadTexture("resources/textures/blocks_normal.jpg");
	textures["ground_normal"] = LoadTexture("resources/textures/SFloor_normal.jpg");
	textures["flavor_normal"] = LoadTexture("resources/textures/choco_normal.png");
	textures["cone_normal"] = LoadTexture("resources/textures/cone_normal.png");
	textures["cherry_normal"] = LoadTexture("resources/textures/cherry_normal.png");

}

void moveLight(Sphere* light) {

	glm::vec3 pos;

	pos.x = 1.0f + sin(glfwGetTime()) * 5.0f;
	pos.y = sin(glfwGetTime() / 2.0f) * 1.0f;
	pos.z = cos(glfwGetTime()) * 5.0f;

	light->SetPosition(pos);
}


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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	LoadTextures();

	Shader lightShader("src/Shaders/LightVertexShader.vs", "src/Shaders/LightFragmentShader.fs");
	Shader shader("src/Shaders/VertexShader.vs", "src/Shaders/FragmentShader.fs");

	Sphere lightSource = Sphere(0.4f);
	lightSource.SetPosition(lightPos);
	lightSource.Load();

	Tank tank(textures["metalgreen"], textures["block"], textures["metal"], textures["metal_normal"], textures["block_normal"]);

	Cube cube = Cube(2.0f, 2.0f, 2.0f);
	cube.SetPosition(glm::vec3(0.0f, 0.0f, 15.0f));
	cube.Load();

	Cube floor = Cube(100.0f, 0.1f, 100.0f);
	floor.SetPosition(glm::vec3(0.0f, -1.6f, 0.0f));
	floor.Load();

	Sphere sphere2 = Sphere(1.0f, 36, 18, true);
	sphere2.SetPosition(glm::vec3(3.0f, 0.0f, 15.0f));
	sphere2.Load();

	IceCream iceCream(textures["choco"], textures["strawberry"], textures["vanilla"], textures["cherry"], textures["cone"], textures["flavor_normal"], 
		textures["cone_normal"], textures["cherry_normal"]);

	// Skybox area
	Shader skyboxShader("src/Shaders/SkyboxVertexShader.vs", "src/Shaders/SkyboxFragmentShader.fs");

	vector<std::string> faces
	{
		"resources/textures/right2.png",
			"resources/textures/left2.png",
			"resources/textures/up2.png",
			"resources/textures/down2.png",
			"resources/textures/front2.png",
			"resources/textures/back2.png"
	};

	Skybox skybox = Skybox();
	skybox.Load(faces);

	// Configuracion del shader
	shader.use();
	shader.setInt("material.diffuse", 0);

	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", projection);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	lightShader.use();
	lightShader.setMat4("projection", projection);

	/* Text area*/
	glm::mat4 textProj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
	Shader textShader("src/Shaders/text.vs", "src/Shaders/text.fs");
	Text texto = Text(HEIGHT, WIDTH);

	string tecla = " ";
	string titulo = "Camionetica poderosa";
	string mensaje1 = "Moviendo hacia ";
	string Mensajito = mensaje1.append(tecla);
		
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

		shader.setVec3("light.position", lightSource.position);
		shader.setVec3("light.ambient", 0.6f, 0.6f, 0.6f);
		shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);

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
			//tecla.replace(0, 8, "adelante");
			tecla = "adelante";
			Mensajito.replace(16, 9, tecla);
			tank.moveForward(shader);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			//cube.moveForward(ourShader);
			//tecla.replace(0, 5, "atras");
			tecla = "atras";
			Mensajito.replace(16, 9, tecla);
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
			texto.RenderText("PLOMO", 570.0f, 240.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f), textShader);
			tank.fire();
			
		}

		if (CheckCollision(cube, tank)) {
			cube.Clean();
		}

		if (tank.hasBeenShotF()) {
			Cylinder *projectile = tank.getProjectile();
			if (CheckCollisionProjectile(cube, *projectile)) {
				projectile->Clean();
				cube.Clean();
				tank.setHasBeenShot();
			}
			if (projectile->position.z >= 40.0f) {
				projectile->Clean();
				tank.setHasBeenShot();
			}
		}

		floor.Bind(textures["ground"], textures["ground_normal"]);
		floor.Draw(shader);

		sphere2.Bind(textures["metal"], textures["metal_normal"]);
		sphere2.Draw(shader);

		tank.Draw(shader);

		iceCream.Draw(shader);

		lightShader.use();
		moveLight(&lightSource);
		lightShader.setMat4("view", view);
		lightSource.Draw(lightShader);

		skybox.Draw(skyboxShader,cameraPos,cameraFront,cameraUp,projection);

		glBindVertexArray(0);

		textShader.use();
		texto.LoadText("hola", 48);
		textShader.setMat4("projection", textProj);
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			//cout << "Antes" << tecla << endl;
			tecla = "O";
			Mensajito.replace(16, 9, tecla);
			//cout << tecla << endl;
			//cout << Mensajito << endl;
		}
		texto.RenderText(titulo, 270.0f, 570.0f, 0.5f, glm::vec3(0, 0, 0), textShader);
		texto.RenderText(Mensajito, 25.0f, 25.0f, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f), textShader);
		texto.RenderText("Esto tiene lag jeje", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textShader);
		

		/* Intercambio entre buffers */
		glfwSwapBuffers(window);

		/* Recepcion de eventos */
		glfwPollEvents();

	}

	// Borramos el contenido de los buffers
	tank.Clear();
	texto.CleanGL();

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