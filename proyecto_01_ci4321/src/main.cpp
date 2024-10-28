#include "Shader.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image/stb_image.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <filesystem>

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
	tank.LoadTextures(shader);

	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", projection);

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

		shader.use();

		// Aplicamos la matriz del view (hacia donde esta viendo la camara)
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(
			cameraPos,
			cameraPos + cameraFront,
			cameraUp
		);
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


		tank.Draw(shader);
		glBindVertexArray(0);

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