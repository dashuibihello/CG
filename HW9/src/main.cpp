#include "fun.hpp"

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

extern bool beginAnimation;

extern vector<Point> Points;
extern vector<Point> animationLine;

extern float animationVertices[3000000];

int main() {
	//初始化和配置
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 为了Mac OS X系统
#endif

														 //创建一个窗口对象
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW9", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//鼠标点击
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader shaderProgram = Shader("Vertex.vs", "Fragment.fs");

	int step = 0;

	while (!glfwWindowShouldClose(window)) {
		//输入
		processInput(window);

		//渲染
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shaderProgram.use();
		renderPoint();
		renderBezierLine();
		if (beginAnimation) {
			step++;
			getAnimationPoints((double)step / 300, Points.size() - 1, true, 0);
			setVertice(&animationLine, animationVertices);
			renderAnimation();
			if (step == 300) {
				beginAnimation = false;
				step = 0;
			}
			animationLine.clear();
		}

		//检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}