#include "Shader.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)

using namespace std;

struct Point
{
	double x;
	double y;
	Point(double xPos, double yPos) {
		x = xPos;
		y = yPos;
	}
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);	//鼠标按钮点击事件
void framebuffer_size_callback(GLFWwindow* window, int width, int height);			
void processInput(GLFWwindow *window);												//处理输入
void setPoint(double x, double y);													//将鼠标点击的点存入向量
void removePoint();																	//移除最新添加的点
void renderPoint();																	//画出点击的点
void renderBezierLine();															//画出Bezier曲线
void renderAnimation();																//动画效果实现
void setVertice(vector<Point> *points, float vertices[3000000]);					//将vector中的点存入float数组
void bezierCurve();																	//bezier算法
int computeNumC(int num);															//计算组合数
int computeFactorial(int num);														//计算阶乘

float normalizeWidth(double num);													//对长标准化
float normalizeHeight(double num);													//对高标准化

void getAnimationPoints(double step, int time, bool isFirst, int alreadyPush);		//获取动画每一步的点
Point getPointFromLine(Point A, Point B, double step);								//根据当前步数获取线段上对应的点
void showAnimationLine(int time, int alreadyShow);									//动画连线