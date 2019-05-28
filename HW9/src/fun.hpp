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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);	//��갴ť����¼�
void framebuffer_size_callback(GLFWwindow* window, int width, int height);			
void processInput(GLFWwindow *window);												//��������
void setPoint(double x, double y);													//��������ĵ��������
void removePoint();																	//�Ƴ�������ӵĵ�
void renderPoint();																	//��������ĵ�
void renderBezierLine();															//����Bezier����
void renderAnimation();																//����Ч��ʵ��
void setVertice(vector<Point> *points, float vertices[3000000]);					//��vector�еĵ����float����
void bezierCurve();																	//bezier�㷨
int computeNumC(int num);															//���������
int computeFactorial(int num);														//����׳�

float normalizeWidth(double num);													//�Գ���׼��
float normalizeHeight(double num);													//�Ը߱�׼��

void getAnimationPoints(double step, int time, bool isFirst, int alreadyPush);		//��ȡ����ÿһ���ĵ�
Point getPointFromLine(Point A, Point B, double step);								//���ݵ�ǰ������ȡ�߶��϶�Ӧ�ĵ�
void showAnimationLine(int time, int alreadyShow);									//��������