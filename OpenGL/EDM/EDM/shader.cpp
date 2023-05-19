#include <GLEW/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "shader.h"

// 쉐이더 파일 읽어오기
const string ReadStringFromFile(const string& filename)
{
	ifstream f(filename);
	return !f.is_open() ? "" : string(istreambuf_iterator<char>(f), istreambuf_iterator<char>());
}

// 쉐이더 오브젝트 생성하기
GLuint CreateShader(GLenum shaderType, const string& source)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0)
		return 0;

	// 쉐이더 소스 세팅
	const char* raw_str = source.c_str();
	glShaderSource(shader, 1, &raw_str, NULL);

	// 쉐이더 오브젝트 컴파일
	glCompileShader(shader);

	// Check
	if (!CheckShader(shader))
	{
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool CheckShader(GLuint shader)
{
	GLint state;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
	if (GL_FALSE == state)
	{
		int infologLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1)
		{
			int charsWritten = 0;
			char* infoLog = new char[infologLength];
			glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
			cout << infoLog << endl;
			delete[] infoLog;
		}
		return false;
	}
	return true;
}

bool CheckProgram(GLuint program)
{
	GLint state;
	glGetProgramiv(program, GL_LINK_STATUS, &state);
	if (GL_FALSE == state)
	{
		int infologLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1)
		{
			int charsWritten = 0;
			char* infoLog = new char[infologLength];
			glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
			cout << infoLog << endl;
			delete[] infoLog;
		}
		return false;
	}
	return true;
}

// -------------------- 쉐이더 디버깅 --------------------
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