#include "fun.hpp"

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 800;

unsigned int pointVAO = 0;
unsigned int pointVBO = 0;

unsigned int bezierVAO = 0;
unsigned int bezierVBO = 0;

unsigned int animationVAO = 0;
unsigned int animationVBO = 0;

bool beginAnimation = false;

vector<Point> Points;
vector<Point> bezierLine;
vector<Point> animationLine;

float pointsVertices[3000000] = { 0 };
float bezierLineVertices[3000000] = { 0 };
float animationVertices[3000000] = { 0 };

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		beginAnimation = true;
		
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	if (action == GLFW_PRESS) switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		glfwGetCursorPos(window, &xpos, &ypos);
		setPoint(xpos, ypos);
		setVertice(&Points, pointsVertices);
		bezierCurve();
		setVertice(&bezierLine, bezierLineVertices);
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		glfwGetCursorPos(window, &xpos, &ypos);
		removePoint();
		setVertice(&Points, pointsVertices);
		bezierCurve();
		setVertice(&bezierLine, bezierLineVertices);
		break;
	default:
		return;
	}
	return;
}

void setPoint(double x, double y) {
	Point temp(x, y);
	Points.push_back(temp);
}

void removePoint() {
	if (Points.size() > 0) {
		Points.pop_back();
	}	
}

void renderPoint()
{
	if (pointVAO == 0) {
		glGenVertexArrays(1, &pointVAO);
		glGenBuffers(1, &pointVBO);
		glBindVertexArray(pointVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pointsVertices), pointsVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointsVertices), pointsVertices, GL_STATIC_DRAW);
	glBindVertexArray(pointVAO);
	if (Points.size() > 1) {
		for (size_t i = 0; i < Points.size() - 1; i++) {
			glDrawArrays(GL_LINES, i, 2);
		}
	}
	glPointSize(15.0f);
	glDrawArrays(GL_POINTS, 0, Points.size());
	glPointSize(1.0f);
}

void renderBezierLine()
{
	if (bezierVAO == 0) {
		glGenVertexArrays(1, &bezierVAO);
		glGenBuffers(1, &bezierVBO);
		glBindVertexArray(bezierVAO);
		glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bezierLineVertices), bezierLineVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bezierLineVertices), bezierLineVertices, GL_STATIC_DRAW);
	glBindVertexArray(bezierVAO);
	glDrawArrays(GL_POINTS, 0, bezierLine.size());
}

void renderAnimation()
{
	if (animationVAO == 0) {
		glGenVertexArrays(1, &animationVAO);
		glGenBuffers(1, &animationVBO);
		glBindVertexArray(animationVAO);
		glBindBuffer(GL_ARRAY_BUFFER, animationVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(animationVertices), animationVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, animationVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(animationVertices), animationVertices, GL_STATIC_DRAW);
	glBindVertexArray(animationVAO);
	showAnimationLine(Points.size() - 2, 0);
	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, animationLine.size());
	glPointSize(1.0f);
}

void showAnimationLine(int time, int alreadyShow) {
	if (time == 0) {
		return;
	}
	for (size_t i = alreadyShow; i < time + alreadyShow; i++) {
		glDrawArrays(GL_LINES, i, 2);
	}
	showAnimationLine(time - 1, alreadyShow + time + 1);
}

void setVertice(vector<Point> *points, float vertices[3000000]) {
	for (size_t i = 0; i < (*points).size(); i++) {
		vertices[i * 3] = normalizeWidth((*points)[i].x);
		vertices[i * 3 + 1] = normalizeHeight((*points)[i].y);
		vertices[i * 3 + 2] = 0;
	}
}

//对x坐标进行标准化
float normalizeWidth(double num) {
	float answer = (float)(num - (SCR_WIDTH / 2 - 1)) / (SCR_WIDTH / 2 - 1);
	if (answer > 1.0f)
		answer = 1.0f;
	return answer;
}

//对y坐标进行标准化
float normalizeHeight(double num) {
	float answer = -(float)(num - (SCR_HEIGHT / 2 - 1)) / (SCR_HEIGHT / 2 - 1);
	if (answer > 1.0f)
		answer = 1.0f;
	return answer;
}

void bezierCurve() {
	if (Points.size() > 1) {
		float step = 0.001f;
		int size = 1 / step;

		float t = 0.0f;

		bezierLine.clear();
		for (int i = 0; i < size; i++, t += step) {
			double x = 0, y = 0;
			for (size_t j = 0; j < Points.size(); j++) {
				x += Points[j].x * pow(t, j) * pow(1 - t, Points.size() - 1 - j) * computeNumC(j);
				y += Points[j].y * pow(t, j) * pow(1 - t, Points.size() - 1 - j) * computeNumC(j);
			}
			Point temp(x, y);
			bezierLine.push_back(temp);
		}
	}
	else {
		bezierLine.clear();
	}
}

int computeNumC(int num) {
	return computeFactorial(Points.size() - 1) / (computeFactorial(num) * computeFactorial(Points.size() - 1 - num));
}

int computeFactorial(int num) {
	long int answer = 1;
	for (int i = 1; i <= num; i++)
		answer *= i;
	return answer;
}

void getAnimationPoints(double step, int time, bool isFirst, int alreadyPush) {
	if (time == 0) {
		return;
	}
	if (isFirst) {
		for (int i = 0; i < time; i++) {
			animationLine.push_back(getPointFromLine(Points[i], Points[i + 1], step));
		}
		getAnimationPoints(step, time - 1, false, 0);
	}
	else {
		for (int i = alreadyPush; i < time + alreadyPush; i++) {
			animationLine.push_back(getPointFromLine(animationLine[i], animationLine[i + 1], step));
		}
		getAnimationPoints(step, time - 1, false, alreadyPush + time + 1);
	}
}

Point getPointFromLine(Point A, Point B, double step) {
	double x = A.x + (B.x - A.x) * step;
	double y = A.y + (B.y - A.y) * step;
	Point temp(x, y);
	return temp;
}