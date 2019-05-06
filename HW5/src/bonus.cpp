#include "Shader.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#define PI 3.14159265

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	//初始化和配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 为了Mac OS X系统
#endif

														 //创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW5", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//鼠标输入
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//将鼠标锁定在程序里
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader shaderProgram = Shader("vertex.vs", "fragment.fs");

	//顶点输入
	float vertices[] = {
		//坐标			  //颜色
		2.0f, 2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		-2.0f, 2.0f, 2.0f, 1.0f, 0.0f, 0.0f,

		2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		-2.0f, 2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		-2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 0.0f,

		2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
		2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
		-2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 0.0f,

		2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
		-2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 0.0f,

		2.0f, 2.0f, 2.0f, 0.0f, 0.0f, 1.0f,
		2.0f, -2.0f, 2.0f, 0.0f, 0.0f, 1.0f,
		2.0f, 2.0f, -2.0f, 0.0f, 0.0f, 1.0f,

		2.0f, -2.0f, 2.0f, 0.0f, 0.0f, 1.0f,
		2.0f, 2.0f, -2.0f, 0.0f, 0.0f, 1.0f,
		2.0f, -2.0f, -2.0f, 0.0f, 0.0f, 1.0f,

		-2.0f, 2.0f, 2.0f, 0.0f, 1.0f, 1.0f,
		-2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 1.0f,
		-2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 1.0f,

		-2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 1.0f,
		-2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 1.0f,
		-2.0f, -2.0f, -2.0f, 0.0f, 1.0f, 1.0f,

		2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f,
		-2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f,
		2.0f, 2.0f, -2.0f, 1.0f, 1.0f, 0.0f,

		-2.0f, 2.0f, 2.0f, 1.0f, 1.0f, 0.0f,
		2.0f, 2.0f, -2.0f, 1.0f, 1.0f, 0.0f,
		-2.0f, 2.0f, -2.0f, 1.0f, 1.0f, 0.0f,

		2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 1.0f,
		2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 1.0f,

		-2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 1.0f,
		2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 1.0f,
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	//把新创建的缓冲绑定到GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//链接顶点属性
	// 位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//循环渲染
	while (!glfwWindowShouldClose(window))
	{
		//每帧的时间差
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//深度测试
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//输入
		processInput(window);

		//渲染
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.use();
		// create transformations
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		// retrieve the matrix uniform locations
		shaderProgram.setMat4("model", model);
		shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

//处理输入
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}

//调整窗口大小的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//鼠标移动
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

//滚轮缩放
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}