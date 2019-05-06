#include "Shader.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#define PI 3.14159265

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

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
		//深度测试
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		//输入
		processInput(window);

		//渲染
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// create transformations
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
		model = glm::rotate(model, glm::radians(60.0f), glm::vec3(2.0f, 2.0f, 1.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
		
		//透视投影
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//正交投影
		//projection = glm::ortho(-5.0f, 5.0f, 0.0f, 5.0f, 0.1f, 100.0f);
		
		// retrieve the matrix uniform locations
		unsigned int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shaderProgram.ID, "projection");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(shaderProgram.ID);

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
}

//调整窗口大小的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}