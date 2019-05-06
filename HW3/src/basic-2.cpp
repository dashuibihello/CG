#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void show(float points[], int length);
float normalizeWidth(int num);
float normalizeHeight(int num);
void bresenhamLine(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x2, int y2);
void bresenhamCircle(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int r);
void putPointInCircle(vector<int> * xVec, vector<int> * yVec, int x1, int y1, int x, int y, int r);
void swap(int *a, int *b);

const int SCR_WIDTH = 600;
const int SCR_HEIGHT = 600;

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

int main()
{
	vector<int> xVec, yVec;

	int x, y, r;
	cout << "Please input the center of the circle and radius(like:x y r)" << endl;
	cin >> x >> y >> r;
	/*
	int x1, x2, x3, y1, y2, y3;
	cout << "Please input the first point(like:x y)(x in(0," << SCR_WIDTH - 1 << "), y in(0," << SCR_HEIGHT - 1 << "))" << endl;
	cin >> x1 >> y1;
	cout << "Please input the second point(like:x y)(x in(0," << SCR_WIDTH - 1 << "), y in(0," << SCR_HEIGHT - 1 << "))" << endl;
	cin >> x2 >> y2;
	cout << "Please input the third point(like:x y)(x in(0," << SCR_WIDTH - 1 << "), y in(0," << SCR_HEIGHT - 1 << "))" << endl;
	cin >> x3 >> y3;
	int length1 = 0;
	bresenhamLine(&xVec, &yVec, x1, y1, x2, y2);
	bresenhamLine(&xVec, &yVec, x1, y1, x3, y3);
	bresenhamLine(&xVec, &yVec, x2, y2, x3, y3);
	*/
	bresenhamCircle(&xVec, &yVec, x, y, r);
	float *points = new float[xVec.size() * 6];
	for (size_t i = 0; i < xVec.size(); i++) {
		points[i * 6] = normalizeWidth(xVec[i]);
		points[i * 6 + 1] = normalizeHeight(yVec[i]);
		points[i * 6 + 2] = 0.0f;
		points[i * 6 + 3] = 1.0f;
		points[i * 6 + 4] = 1.0f;
		points[i * 6 + 5] = 1.0f;
	}
	show(points, xVec.size());
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

//生成窗口及图像
void show(float points[], int length) {
	//初始化和配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 为了Mac OS X系统
#endif

														 //创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

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

	//顶点输入
	float vertices[50000] = { 0 };
	for (int i = 0; i < length * 6; i++) {
		vertices[i] = points[i];
	}
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
		//输入
		processInput(window);

		//渲染
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//画圆
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, length);

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
}