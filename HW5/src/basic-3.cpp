#include "Shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <math.h>

#define PI 3.14159265

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 1200;

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

	bool isCamera = false;
	bool isRotate = false;
	bool isTranslate = false;
	bool isScale = false;
	bool autoCamera = false;
	bool autoRotate = false;
	bool autoTranslate = false;
	bool autoScale = false;
	float x = 0, y = 0, z = 0;
	float axis[3] = { 1, 0, 0 };
	float angle = 0;
	float ratio[3] = { 1, 1, 1 };
	int time = 0;
	int isOrtho = 0;
	float left = -5.0f, right = 5.0f, bottom = -5.0f, top = 5.0f, zNear = 0.1f, zFar = 100.0f;
	float fov = 45.0f, zNear1 = 0.1f, zFar1 = 100.0f;
	float radius = 10.0f, camX = 0, camY = 0, camZ = 0;

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
			ImGui::PushItemWidth(50);
			ImGui::RadioButton("Perspective Projection", &isOrtho, 0);
			ImGui::SameLine();
			ImGui::InputFloat("Field of View", &fov);
			ImGui::SameLine();
			ImGui::InputFloat("near", &zNear1);
			ImGui::SameLine();
			ImGui::InputFloat("far", &zFar1);

			ImGui::RadioButton("Orthographic Projection", &isOrtho, 1);
			ImGui::SameLine();
			ImGui::InputFloat("left", &left);
			ImGui::SameLine();
			ImGui::InputFloat("right", &right);
			ImGui::SameLine();
			ImGui::InputFloat("bottom", &bottom);
			ImGui::SameLine();
			ImGui::InputFloat("top", &top);
			ImGui::SameLine();
			ImGui::InputFloat("near", &zNear);
			ImGui::SameLine();
			ImGui::InputFloat("far", &zFar);

			ImGui::PushItemWidth(80);
			ImGui::Checkbox("Camera", &isCamera);
			ImGui::SameLine();
			ImGui::InputFloat("PosX", &camX);
			ImGui::SameLine();
			ImGui::InputFloat("PosY", &camY);
			ImGui::SameLine();
			ImGui::InputFloat("PosZ", &camZ);
			ImGui::SameLine();
			ImGui::Checkbox("AutoCamera", &autoCamera);

			ImGui::Checkbox("Translate", &isTranslate);
			ImGui::SameLine();
			ImGui::SliderFloat("x", &x, -5, 5);
			ImGui::SameLine();
			ImGui::SliderFloat("y", &y, -5, 5);
			ImGui::SameLine();
			ImGui::SliderFloat("z", &z, -5, 5);
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

			if (isOrtho == 0) {
				projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, zNear1, zFar1);
			}
			else if (isOrtho == 1) {
				projection = glm::ortho(left, right, bottom, top, zNear, zFar);
			}
			if (isCamera) {
				if (!autoCamera) {
					view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else {
					camX = sin(glfwGetTime()) * radius;
					camZ = cos(glfwGetTime()) * radius;
					view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}		
			}
			else {
				view = glm::translate(view, glm::vec3(0.0f, 0.0f, -15.0f));
			}
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

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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