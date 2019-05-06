#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#define PI 3.14159265

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int getShader();

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//顶点着色器源代码
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
"   ourColor = aColor;\n"
"}\0";
//片段着色器源代码
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW4", NULL, NULL);
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

	const char* glsl_version = "#version 330 core";
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool isRotate = false;
	bool isTranslate = false;
	bool isScale = false;
	bool autoRotate = false;
	bool autoTranslate = false;
	bool autoScale = false;
	float x = 0, y = 0, z = 0;
	float axis[3] = { 1, 0, 0 };
	float angle = 0;
	float ratio[3] = { 1, 1, 1 };
	int time = 0;

	unsigned int shaderProgram = getShader();

	//顶点输入
	float vertices[] = {
		//坐标			  //颜色
		2.0f, 2.0f, 2.0f, 1.0f, 0.0f, 0.0f,
		2.0f, -2.0f, 2.0f, 0.0f, 1.0f, 0.0f,
		-2.0f, 2.0f, 2.0f, 0.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, 2.0f, 1.0f, 1.0f, 1.0f,
		2.0f, 2.0f, -2.0f, 1.0f, 1.0f, 1.0f,
		2.0f, -2.0f, -2.0f, 0.0f, 0.0f, 1.0f,
		-2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, -2.0f, 1.0f, 0.0f, 0.0f,
	};

	unsigned int indices[] = { // 注意索引从0开始! 
		0, 1, 2,
		1, 2, 3,

		4, 5, 6,
		5, 6, 7,

		0, 1, 4,
		1, 4, 5,

		2, 3, 6,
		3, 6, 7,

		0, 2, 4,
		2, 4, 6,

		1, 3, 5,
		3, 5, 7
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	//把新创建的缓冲绑定到GL_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);

		glfwPollEvents();
		processInput(window);
		//渲染
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// create transformations
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Transform");
			ImGui::SetWindowFontScale((float)1.0);

			ImGui::PushItemWidth(80);
			ImGui::Checkbox("Translate", &isTranslate);
			ImGui::SameLine();
			ImGui::SliderFloat("x", &x, -1, 1);
			ImGui::SameLine();
			ImGui::SliderFloat("y", &y, -1, 1);
			ImGui::SameLine();
			ImGui::SliderFloat("z", &z, -1, 1);
			ImGui::SameLine();
			ImGui::Checkbox("AutoTranslate", &autoTranslate);

			ImGui::Checkbox("Rotate", &isRotate);
			ImGui::SameLine();
			ImGui::SliderFloat("angle", &angle, 0, 360);
			ImGui::SameLine();
			ImGui::PushItemWidth(150);
			ImGui::InputFloat3("axis(x, y, z)", axis);
			ImGui::SameLine();
			ImGui::Checkbox("AutoRotate", &autoRotate);

			ImGui::PushItemWidth(80);
			ImGui::Checkbox("Scale", &isScale);
			ImGui::SameLine();
			ImGui::PushItemWidth(150);
			ImGui::InputFloat3("ratio(x, y, z)", ratio);
			ImGui::SameLine();
			ImGui::Checkbox("AutoScale", &autoScale);

			if (isTranslate) {
				if (!autoTranslate) {
					time = 0;
					model = glm::translate(model, glm::vec3(x, y, z));
				}
				else {
					time++;
					model = glm::translate(model, glm::vec3(x, y, z) * (float)sin(time * PI / 200) * 5.0f);
				}			
			}
			if (isRotate) {
				if (!autoRotate) {
					model = glm::rotate(model, glm::radians(angle), glm::vec3(axis[0], axis[1], axis[2]));
				}
				else {
					model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(axis[0], axis[1], axis[2]));
				}				
			}
			if (isScale) {
				if (!autoScale) {
					time = 0;
					model = glm::scale(model, glm::vec3(ratio[0], ratio[1], ratio[2]));
				}
				else {
					time++;
					model = glm::scale(model, glm::vec3(ratio[0], ratio[1], ratio[2]) * ((float)sin(time * PI / 200) * 1.0f + 0.5f));
				}			
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		// retrieve the matrix uniform locations
		unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
		unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
		unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUseProgram(shaderProgram);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
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

//生成着色器程序对象
unsigned int getShader()
{
	//编译着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//检测是否编译成功 
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//编译着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//检测是否编译成功 
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	//链接着色器
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//检测链接是否成功
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}