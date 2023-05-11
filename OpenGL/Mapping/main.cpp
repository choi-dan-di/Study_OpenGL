// with GLFW
#define GLFW_DLL
#define GLFW_INCLUDE_GLU
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
using namespace std;

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include "shader.hpp"
#include "texture.hpp"
//#include "controls.hpp"
#include "textfile.h"


void InitApp();
// Error Check
void ErrorCallBack(int error, const char* description);
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void WindowSizeChangeCallback(GLFWwindow* window, int width, int height);

// ���̴� ���
GLuint v_shader, f_shader, program_shader;
void setShaders();
int printOglError(char* file, int line);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

// ȭ�鿡 �Ѹ� �׸� �׽�Ʈ
void Display();

// ���⼭ ���� �� �ϸ� extern���� ������ ��
GLFWwindow* window = NULL;

int main()
{
	// ���� �ڵ鷯 ���
	glfwSetErrorCallback(ErrorCallBack);

	// GLFW �ʱ�ȭ
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// ������ ����
	window = glfwCreateWindow(1024, 768, "Normal Mapping - EunJeong", NULL, NULL);
	if (!window)
	{
		// ���� ������ ������ �����ߴٸ� ���ø����̼� ����
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// ���ؽ�Ʈ ����
	glfwMakeContextCurrent(window);

	// swap ���� ����
	// glfwSwapInterval(1);

	// Ű �ڵ鷯 ���
	//glfwSetKeyCallback(window, KeyCallBack);
	// ������ ������ ���� �ڵ鷯 ���
	//glfwSetWindowSizeCallback(window, WindowSizeChangeCallback);

	// GLEW �ʱ�ȭ
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// ���ø����̼� �ʱ�ȭ
	InitApp();

	//--------------------------------------------------------------

	// Z-���۸�
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// �޸� ����
	glEnable(GL_CULL_FACE);

	// �ʾ��Ӵϴ�. �ּ�ó���ϸ� �𵨻�����ϴ�.
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// ���̴� ����Ϸ��� �غ�
	// ���̴� ������ �ٿ��� ���α׷� �ڵ鷯
	GLuint programID = LoadShaders("NormalMapping.vertexshader", "NormalMapping.fragmentshader");
	// ��� �ڵ鷯
	// GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	// - ���� -
	GLuint ProjMatID = glGetUniformLocation(programID, "ProjMat");
	GLuint ViewMatID = glGetUniformLocation(programID, "ViewMat");
	GLuint WorldMatID = glGetUniformLocation(programID, "WorldMat");
	// eyePos �Ѱ��ֱ� : ��� ī�޶� �����̶� ����
	GLuint eyePosID = glGetUniformLocation(programID, "eyePos");
	// lightDir �Ѱ��ֱ� : �� ������ �ҷ���
	GLuint lightDirID = glGetUniformLocation(programID, "lightDir");

	// �ؽ�ó �ε�
	// GLuint Texture = loadDDS("texture/tree_texture.DDS");
	GLuint Texture = loadBMP_custom("texture/tree_default_Albedo.bmp");
	// ���α׷��� ������ ID ����
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// .obj ���� �ҷ�����
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	bool res = loadOBJ("object/tree.obj", vertices, uvs, normals);

	// Vertex Buffer ����
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

	// UV Buffer ����
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);

	// Normal Buffer ����
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);

	mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	mat4 View = lookAt(
		vec3(13, 13, -10), // Camera is at (4,3,-3), in World Space
		vec3(0, 6, 0), // and looks at the origin
		vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	mat4 World = mat4(1.0);
	vec3 eyePos = vec3(13, 13, 10);
	vec3 lightDir = vec3(6, 7, -5);

	//--------------------------------------------------------------

	// �� ������ ����. �����찡 ����Ǳ� ������ �ݺ�
	while (!glfwWindowShouldClose(window))
	{
		// ���� �����
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ���⿡ Update/Render �ڵ�
		glUseProgram(programID);

		// MVP ��� ���ϱ�
		// glm::mat4 MVP = Projection * View * mat4(1.0);

		// ������ ���(MVP) ����
		// glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		// - ���� - : �� ������ �Ϸ���
		glUniformMatrix4fv(ProjMatID, 1, GL_FALSE, &Projection[0][0]);
		glUniformMatrix4fv(ViewMatID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(WorldMatID, 1, GL_FALSE, &World[0][0]);
		glUniform3fv(eyePosID, 1, &eyePos[0]);
		glUniform3fv(lightDirID, 1, &lightDir[0]);

		// �ؽ�ó ���ε�
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);

		// VBO ����??
		// 1. ���� ���� ����
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2. uv ��ǥ ���� ����
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3. ��� ���� ����
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// �׸��� (��ο� ��)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		// Ȱ��ȭ �ߴ� ���� ��Ʈ����Ʈ ��Ȱ��
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// ���� ���� ��ü (�׸� ����� ���÷����ϴ� ���)
		glfwSwapBuffers(window);
		// ������ �̺�Ʈ (Ű ��Ʈ��ũ ��) ����
		glfwPollEvents();
	}

	// ���� ����
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	// ���α׷� ����
	glDeleteProgram(programID);
	// �ؽ�ó ����
	glDeleteTextures(1, &Texture);
	// ������ ����
	//glfwDestroyWindow(window);

	// GLFW ����
	glfwTerminate();

	return 0;
}

void InitApp()
{
	// Ŭ���� ����(����) ����
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void ErrorCallBack(int error, const char* description)
{
	cout << description << endl;
}

void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void WindowSizeChangeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void setShaders()
{
	char* vs = NULL, * fs = NULL, * fs2 = NULL;

	v_shader = glCreateShader(GL_VERTEX_SHADER);
	f_shader = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("NormalMapping.vertexshader");
	fs = textFileRead("NormalMapping.fragmentshader");

	const char* vv = vs;
	const char* ff = fs;

	glShaderSource(v_shader, 1, &vv, NULL);
	glShaderSource(f_shader, 1, &ff, NULL);

	free(vs); free(fs);

	glCompileShader(v_shader);
	glCompileShader(f_shader);

	printShaderInfoLog(v_shader);
	printShaderInfoLog(f_shader);

	program_shader = glCreateProgram();
	glAttachShader(program_shader, v_shader);
	glAttachShader(program_shader, f_shader);

	glLinkProgram(program_shader);
	printProgramInfoLog(program_shader);

	glUseProgram(program_shader);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_TRIANGLES);

	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glEnd();
	glFlush();
}

// ���̴� ���� �� ����� �� �� ���� �Լ���
#define printOpenGLError() printOglError(__FILE__, __LINE__)
int printOglError(char* file, int line)
{
	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}