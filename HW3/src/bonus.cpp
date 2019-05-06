#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

#define checkSameSide(x1, y1, x2, y2, x3, y3, x, y) ((y1 - y2) * x + (x2 - x1) * y + x1 * y2 - x2 * y1) / (float)((y1 - y2) * x3 + (x2 - x1) * y3 + x1 * y2 - x2 * y1)

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int getShader();
float normalizeWidth(int num);
float normalizeHeight(int num);
void bresenhamLine(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x2, int y2);
void bresenhamCircle(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int r);
void putPointInCircle(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x, int y, int r);
void swap(int *a, int *b);
void padding(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x2, int y2, int x3, int y3);
bool checkInTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y);

const int SCR_WIDTH = 1280;
const int SCR_HEIGHT = 720;
//顶点输入
float vertices[3000000] = { 0 };

//顶点着色器源代码
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
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

int main(int, char**)
{
	//初始化和配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

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

	bool isCircle = false;

	int pointA[2] = { 640, 360 };
	int pointB[2] = { 200, 200 };
	int pointC[2] = { 1000, 200 };
	int r = 100;
	int length = 0;

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Triangle Or Circle");
			ImGui::SetWindowFontScale((float)1.4);
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Text("Sets the coordinates of the vertices of a triangle!");

			ImGui::InputInt2("point A(x, y)", pointA);
			ImGui::InputInt2("point B(x, y)", pointB);
			ImGui::InputInt2("point C(x, y)", pointC);

			ImGui::Checkbox("Show circle", &isCircle);
			ImGui::InputInt("radius", &r);

			vector<int> xVec, yVec;
			if (isCircle) {
				bresenhamCircle(&xVec, &yVec, 640, 360, r);
			}
			else {
				bresenhamLine(&xVec, &yVec, pointA[0], pointA[1], pointB[0], pointB[1]);
				bresenhamLine(&xVec, &yVec, pointA[0], pointA[1], pointC[0], pointC[1]);
				bresenhamLine(&xVec, &yVec, pointB[0], pointB[1], pointC[0], pointC[1]);
				padding(&xVec, &yVec, pointA[0], pointA[1], pointB[0], pointB[1], pointC[0], pointC[1]);
			}


			length = xVec.size();
			for (size_t i = 0; i < xVec.size(); i++) {
				vertices[i * 6] = normalizeWidth(xVec[i]);
				vertices[i * 6 + 1] = normalizeHeight(yVec[i]);
				vertices[i * 6 + 2] = 0.0f;
				vertices[i * 6 + 3] = 1.0f;
				vertices[i * 6 + 4] = 1.0f;
				vertices[i * 6 + 5] = 1.0f;
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}
		unsigned int shaderProgram = getShader();

		//渲染
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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

		//画图
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, length);

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

	glfwDestroyWindow(window);
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

//对x坐标进行标准化
float normalizeWidth(int num) {
	float answer = (float)(num - (SCR_WIDTH / 2 - 1)) / (SCR_WIDTH / 2 - 1);
	if (answer > 1.0f)
		answer = 1.0f;
	return answer;
}

//对y坐标进行标准化
float normalizeHeight(int num) {
	float answer = (float)(num - (SCR_HEIGHT / 2 - 1)) / (SCR_HEIGHT / 2 - 1);
	if (answer > 1.0f)
		answer = 1.0f;
	return answer;
}

//Bresenham画线算法
void bresenhamLine(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x2, int y2) {
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	bool isLegal = true;
	if (dy > dx) {
		swap(&x1, &y1);
		swap(&x2, &y2);
		swap(&dx, &dy);
		isLegal = false;
	}
	int xi = (x2 - x1) > 0 ? 1 : -1;
	int yi = (y2 - y1) > 0 ? 1 : -1;
	int d = dy * 2 - dx;
	int x = x1, y = y1;
	if (isLegal) {
		(*xVec).push_back(x);
		(*yVec).push_back(y);
		while (x != x2) {
			if (d <= 0) {
				d += dy * 2;
			}
			else {
				d += (dy - dx) * 2;
				y += yi;
			}
			x += xi;
			(*xVec).push_back(x);
			(*yVec).push_back(y);
		}
	}
	else {
		(*xVec).push_back(y);
		(*yVec).push_back(x);
		while (x != x2) {
			if (d <= 0) {
				d += dy * 2;
			}
			else {
				d += (dy - dx) * 2;
				y += yi;
			}
			x += xi;
			(*xVec).push_back(y);
			(*yVec).push_back(x);
		}
	}
}

//Bresenham画圆算法
void bresenhamCircle(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int r) {
	int x = 0, y = r;
	int d = 3 - 2 * r;
	while (x <= y) {
		putPointInCircle(xVec, yVec, x1, y1, x, y, r);
		if (d < 0) {
			d += 4 * x + 6;
		}
		else {
			d += 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
}

//将所得坐标构成其8对称点放入圆中
void putPointInCircle(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x, int y, int r) {
	(*xVec).push_back(x1 + x);
	(*yVec).push_back(y1 + y);
	(*xVec).push_back(x1 + x);
	(*yVec).push_back(y1 - y);

	(*xVec).push_back(x1 - x);
	(*yVec).push_back(y1 + y);
	(*xVec).push_back(x1 - x);
	(*yVec).push_back(y1 - y);

	(*xVec).push_back(x1 + y);
	(*yVec).push_back(y1 + x);
	(*xVec).push_back(x1 + y);
	(*yVec).push_back(y1 - x);

	(*xVec).push_back(x1 - y);
	(*yVec).push_back(y1 + x);
	(*xVec).push_back(x1 - y);
	(*yVec).push_back(y1 - x);
}

//交换两个数
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

//得到需要填充的点
void padding(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x2, int y2, int x3, int y3) {
	int xMax = max(max(x1, x2), x3);
	int yMax = max(max(y1, y2), y3);
	int xMin = min(min(x1, x2), x3);
	int yMin = min(min(y1, y2), y3);

	for (int i = xMin; i <= xMax; i++) {
		for (int j = yMin; j <= yMax; j++) {
			if (checkInTriangle(x1, y1, x2, y2, x3, y3, i, j)) {
				(*xVec).push_back(i);
				(*yVec).push_back(j);
			}
		}
	}
}

//检查点是否在三角形中
bool checkInTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y) {
	return checkSameSide(x1, y1, x2, y2, x3, y3, x, y) > 0
		&& checkSameSide(x2, y2, x3, y3, x1, y1, x, y) > 0
		&& checkSameSide(x3, y3, x1, y1, x2, y2, x, y) > 0;
}