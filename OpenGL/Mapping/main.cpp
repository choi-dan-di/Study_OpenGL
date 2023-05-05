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
#include "controls.hpp"
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
// void Display();

// ���⼭ ���� �� �ϸ� extern���� ������ ��
GLFWwindow* window = NULL;

int main()
{
	// GLFW ������ �ڵ鷯
	//GLFWwindow* window = NULL;

	// ���� �ڵ鷯 ���
	glfwSetErrorCallback(ErrorCallBack);

	// GLFW �ʱ�ȭ
	if (!glfwInit())
		exit(EXIT_FAILURE);

	/*
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	*/

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
	glfwSetKeyCallback(window, KeyCallBack);
	// ������ ������ ���� �ڵ鷯 ���
	glfwSetWindowSizeCallback(window, WindowSizeChangeCallback);

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

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// ���콺 �����
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// ��ũ�� ���߾����� �ʱ�ȭ
	// glfwPollEvents();
	// glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Z-���۸�
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// �޸� ����
	glEnable(GL_CULL_FACE);

	// ���̴� ����Ϸ��� �غ�
	// ���̴� ������ �ٿ��� ���α׷� �ڵ鷯
	GLuint programID = LoadShaders("shader/NormalMapping.vertexshader", "shader/NormalMapping.fragmentshader");
	// ��� �ڵ鷯
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// �ؽ�ó �ε�
	// GLuint TreeTexture = loadDDS("texture/tree_default_Albedo.png");

	// GLuint TreeTextureID = glGetUniformLocation(programID, "TreeTextureSampler");

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

	mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	mat4 View = lookAt(
		vec3(4, 3, -3), // Camera is at (4,3,-3), in World Space
		vec3(0, 0, 0), // and looks at the origin
		vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	//--------------------------------------------------------------

	// �� ������ ����. �����찡 ����Ǳ� ������ �ݺ�
	while (!glfwWindowShouldClose(window))
	{
		// ���� �����
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ���⿡ Update/Render �ڵ�
		// Display();
		// ���̴� ���
		glUseProgram(programID);

		// MVP ��� ���ϱ�
		glm::mat4 MVP = Projection * View * mat4(1.0);

		// ���̴� ����?
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// �ؽ�ó ���ε�
		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TreeTexture);
		glUniform1i(TreeTextureID, 0);
		*/

		// VBO ����??
		// 1rst attribute buffer : vertices
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

		// 2nd attribute buffer : UVs
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

		// �׸��� (��ο� ��)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		// Ȱ��ȭ �ߴ� ���� ��Ʈ����Ʈ ��Ȱ��
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// ���� ���� ��ü (�׸� ����� ���÷����ϴ� ���)
		glfwSwapBuffers(window);

		// ������ �̺�Ʈ (Ű ��Ʈ��ũ ��) ����
		glfwPollEvents();
	}

	// ���� ����
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	// ���α׷� ����
	glDeleteProgram(programID);
	// �ؽ�ó ����
	// glDeleteTextures(1, &TreeTexture);
	// ������ ����
	glfwDestroyWindow(window);

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



// Not GLFW
/*
#include <iostream>

#include <gl/glew.h>
#include <GL/glut.h>
#include "textfile.h"
#include <glm/glm.hpp>
using namespace glm;

//OpenGL
void initGL();					//opengl �ʱ�ȭ

//GLUT
void changeSize(int w, int h);	//������ ũ�� ���� �� ȣ��Ǵ� callback

//Rendering
void display();				//�⺻ ������ �ڵ�
float rotate_angle = 0.f;	//������ ȸ�� animation�� ���� ȸ�� ���� ����

// Rendering Function
void renderScene(void);

//GLEW
void initGLEW();			//GLEW �ʱ�ȭ

//Shader
GLuint v_shader;			//vertex shader handle
GLuint f_shader;			//fragment shader handle
GLuint program_shader;		//shader program handle
void setShaders();			//Shader ����

//Logging
#define printOpenGLError() printOglError(__FILE__, __LINE__)
int printOglError(char* file, int line);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(500, 500);
	glutInitWindowSize(300, 300);
	glutCreateWindow("Normal Mapping");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutDisplayFunc(renderScene);
	//glutReshapeFunc(changeSize);
	//glutIdleFunc(renderScene);

	initGLEW();
	setShaders();

	initGL();

	glutMainLoop();
	return 0;
}


//////////////////////////
void changeSize(int w, int h) {
	if (h == 0)	h = 1;
	float ratio = 1.f * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
}


void initGL()
{
	//Enable/Disable
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	//Rendering
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CCW);
	glColor3f(0.0f, 0.5f, 1.0f);

	//Light
	GLfloat lightPos[] = { 0.f, 0.f, 10.f, 0.f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_COLOR_MATERIAL);

	//Modelview and projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	//Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw
	glPushMatrix();
	glRotatef(rotate_angle, 0.f, 1.f, 0.f);
	glutSolidTeapot(0.5);
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
	rotate_angle = rotate_angle + 0.1f;
	if (rotate_angle > 360.f) rotate_angle -= 360.f;
}

void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_TRIANGLES);

	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glEnd();
	glFinish();
}

void initGLEW()
{
	//Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(0);
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	//Check Shader
	//ARB
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL (ARB)\n");
	else
	{
		printf("No GLSL support\n");
		exit(0);
	}

	//OpenGL 4.6
	if (glewIsSupported("GL_VERSION_4_6"))
		printf("Ready for OpenGL 4.6\n");
	else {
		printf("OpenGL 4.6 not supported\n");
		exit(0);
	}
}

//GLuint v_shader, f_shader, program_shader
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

*/