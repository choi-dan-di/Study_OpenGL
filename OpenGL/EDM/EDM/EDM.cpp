// with GLFW
#define GLFW_DLL
#define GLFW_INCLUDE_GLU
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
using namespace std;

// 카메라 이동에 필요한 변수들 전역으로 지정
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 마우스 이동에 필요한 변수들 전역으로 지정
// 마우스 위치 초기화
float lastX = (float)1024 / (float)2;
float lastY = (float)768 / (float)2;
float yaw = lastX;
float pitch = lastY;
bool firstMouse = true;

// 스크롤 시 줌 기능 구현
float fov = 45.0f;

// setShader
const string ReadStringFromFile(const string& filename);
GLuint CreateShader(GLenum shaderType, const string& source);
bool CheckShader(GLuint shader);

void InitApp();

// Error Check
void ErrorCallBack(int error, const char* description);
// Call Back Functions
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPositionCallBack(GLFWwindow* window, double xpos, double ypos);
void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
void WindowSizeChangeCallback(GLFWwindow* window, int width, int height);

// 쉐이더 사용
GLuint v_shader, f_shader, program_shader;
void setShaders();
int printOglError(char* file, int line);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

// 이것 저것 그려보기
void Draw();

// 여기서 선언 안 하면 extern에서 에러남 ㅠ
GLFWwindow* window = NULL;
GLuint shaderProgramID;

int main(int argc, char** argv)
{
	// 에러 핸들러 등록
	glfwSetErrorCallback(ErrorCallBack);

	// GLFW 초기화
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_SAMPLES, 4);	// 안티앨리어싱 4배
	/*
	* // 왜인지 모르겠지만 에러가 나네.. 나중에 찾아보자(숙제)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// 최대 버전 요구??
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);	// 최소 버전 요구??
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 향후 지원되지 않는 것은 제외
	*/

	// 윈도우 생성
	window = glfwCreateWindow(1024, 768, "Endless Death Match", NULL, NULL);
	if (!window)
	{
		// 만약 윈도우 생성에 실패했다면 어플리케이션 종료
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// 컨텍스트 생성
	glfwMakeContextCurrent(window);
	// swap 간격 조정
	glfwSwapInterval(1);

	//window 창에 대해서 입력받기 설정
	//glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	// 커서 숨기기
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 키 핸들러 등록
	glfwSetKeyCallback(window, KeyCallBack);
	// 마우스 핸들러 등록
	glfwSetCursorPosCallback(window, CursorPositionCallBack);
	// 스크롤 핸들러 등록
	glfwSetScrollCallback(window, ScrollCallBack);

	// 윈도우 사이즈 변경 핸들러 등록
	glfwSetWindowSizeCallback(window, WindowSizeChangeCallback);

	// GLEW 초기화
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// 어플리케이션 초기화
	InitApp();

	//--------------------------------------------------------------
	
	// 정점 정의
	vector<vec3> gVertices = {
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 1.0f, 0.0f),

		vec3(0.0f, 0.0f, 1.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),

		vec3(1.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),

		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f)
	};

	GLuint vertexbuffer;
	/*
	* // OpenGL 4.5 이전 방식
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, gVertices.size() * sizeof(vec3), &gVertices[0], GL_STATIC_DRAW);
	*/
	// OpenGL 4.5의 버퍼 생성 방식
	glCreateBuffers(1, &vertexbuffer);
	glNamedBufferData(vertexbuffer, gVertices.size() * sizeof(vec3), &gVertices[0], GL_STATIC_DRAW);

	// 면마다 색상 다르게 해보기
	vector<vec3> gColors = {
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),

		vec3(0.8f, 0.5f, 0.5f),
		vec3(0.8f, 0.5f, 0.5f),
		vec3(0.8f, 0.5f, 0.5f)
	};

	GLuint colorbuffer;
	glCreateBuffers(1, &colorbuffer);
	glNamedBufferData(colorbuffer, gColors.size() * sizeof(vec3), &gColors[0], GL_STATIC_DRAW);

	// 변환 행렬 임시 정의
	mat4 Model = mat4(1.0);
	/*
	mat4 View = lookAt(
		vec3(-1.0f, 2.0f, 3.0f),	// EYE
		vec3(0.0f, 0.0f, 0.0f),	// AT
		vec3(0.0f, 1.0f, 0.0f)	// UP
	);
	*/
	// perspective : 뷰 프러스텀(절두체) 정의
	// ortho : 직교 투영 변환(원근감 X) 정의
	// mat4 Projection = perspective(radians(45.0f), (float)1024 / (float)768, 0.1f, 100.0f);
	
	GLuint M_MatID = glGetUniformLocation(shaderProgramID, "ModelMat");
	GLuint V_MatID = glGetUniformLocation(shaderProgramID, "ViewMat");
	GLuint P_MatID = glGetUniformLocation(shaderProgramID, "ProjMat");

	//--------------------------------------------------------------

	// 주 렌더링 루프. 윈도우가 종료되기 전까지 반복
	while (!glfwWindowShouldClose(window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 프로그램 활성화
		glUseProgram(shaderProgramID);

		// 카메라
		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		/*
		mat4 View = lookAt(
			vec3(camX, 2.0f, camZ),	// EYE
			vec3(0.0f, 0.0f, 0.0f),	// AT
			vec3(0.0f, 1.0f, 0.0f)	// UP
		);
		*/

		mat4 View = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		mat4 Projection = perspective(radians(fov), (float)1024 / (float)768, 0.1f, 100.0f);

		// 유니폼으로 변환 행렬 넘겨주기
		glUniformMatrix4fv(M_MatID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(V_MatID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(P_MatID, 1, GL_FALSE, &Projection[0][0]);

		// 버퍼 바인딩
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glDrawArrays(GL_TRIANGLES, 0, gVertices.size());

		// 활성화 했던 정점 애트리뷰트 비활성
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// 렌더 버퍼 교체 (그린 결과를 디스플레이하는 명령)
		glfwSwapBuffers(window); 
		// 윈도우 이벤트 (키 스트로크 등) 폴링
		glfwPollEvents();
	}

	// 버퍼 제거
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);

	// 프로그램 제거
	glDeleteProgram(shaderProgramID);

	// 텍스처 제거
	// 윈도우 제거
	glfwDestroyWindow(window);

	// GLFW 종료
	glfwTerminate();

	return 0;
}

void InitApp()
{
	// 클리어 색상(배경색) 지정
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Z-버퍼링 사용
	glEnable(GL_DEPTH_TEST);

	// 뒷면 제거 X, 앞면 반시계로 정렬
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// 쉐이더 읽기
	string vertShaderSrc = ReadStringFromFile("shader/DefaultShader.vert");
	string fragShaderSrc = ReadStringFromFile("shader/DefaultShader.frag");

	// Shader Object 생성
	GLuint vertShaderObj = CreateShader(GL_VERTEX_SHADER, vertShaderSrc);
	GLuint fragShaderObj = CreateShader(GL_FRAGMENT_SHADER, fragShaderSrc);
	
	// Shader Program Object 생성
	shaderProgramID = glCreateProgram();
	// 쉐이더 추가되면 여기다가 붙이기(Attach)
	glAttachShader(shaderProgramID, vertShaderObj);
	glAttachShader(shaderProgramID, fragShaderObj);
	glLinkProgram(shaderProgramID);

	// 프로그램 체크
	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(shaderProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shaderProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	// 쉐이더 붙인거 있으면 여기서 떼고(Detach) 쉐이더 삭제
	glDetachShader(shaderProgramID, vertShaderObj);
	glDetachShader(shaderProgramID, fragShaderObj);

	// 쉐이더 오브젝트 삭제
	glDeleteShader(vertShaderObj);
	glDeleteShader(fragShaderObj);
}

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

bool CheckShader(GLuint shader) {
	GLint state;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &state);
	if (GL_FALSE == state) {
		int infologLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 1) {
			int charsWritten = 0;
			char* infoLog = new char[infologLength];
			glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
			std::cout << infoLog << std::endl;
			delete[] infoLog;
		}
		return false;
	}
	return true;
}

void ErrorCallBack(int error, const char* description)
{
	cout << description << endl;
}

// 키 콜백 처리 함수
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// ESC
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	float cameraSpeed = 3.5f * deltaTime;
	// 예제 코드 따라한건데 마음에 안 드네.. 커스텀 해야할 듯
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos += cameraSpeed * cameraFront;
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos -= cameraSpeed * cameraFront;
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
}

// 마우스 콜백 처리 함수
void CursorPositionCallBack(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	// 프레임 사이의 움직임 offset 계산
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;	// y 좌표의 범위는 반대
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;	// 감도
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// 수직만 제한걸기
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	vec3 front;
	front.x = cos(radians(pitch)) * cos(radians(yaw));
	front.y = sin(radians(pitch));
	front.z = cos(radians(pitch)) * sin(radians(yaw));
	cameraFront = normalize(front);
}

// 스크롤 콜백 함수
void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	// 줌 구현
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void WindowSizeChangeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
/*
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
*/

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// glBegin과 glEnd 사이에서 그리기 세팅
	glBegin(GL_POINTS);
	glVertex2f(1.0f, 1.0f); glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(1.5f, 0.5f); glColor3f(0.0f, 1.0f, 1.0f);
	glVertex2f(2.0f, 0.0f); glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(2.5f, 0.5f); glColor3f(0.5f, 0.5f, 0.0f);
	glVertex2f(3.0f, 1.0f); glColor3f(0.0f, 0.5f, 0.5f);
	glEnd();


	/*
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-5.0f, 11.0f, 10.0f); // { Front }
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-5.0f, 1.0f, 10.0f); // { Front }
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(5.0f, 1.0f, 10.0f); // { Front }
	glColor3f(1.0f, 0.0f, 1.0f); glVertex3f(5.0f, 11.0f, 10.0f); // { Front }

	glColor3f(1.0f, 0.0f, 1.0f); glVertex3f(-5.0f, 11.0f, 10.0f); // { Right }
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(-5.0f, 11.0f, 20.0f); // { Right }
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-5.0f, 1.0f, 20.0f); // { Right }
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-5.0f, 1.0f, 10.0f); // { Right }

	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(5.0f, 11.0f, 20.0f); // { Back }
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(5.0f, 1.0f, 20.0f); // { Back }
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(-5.0f, 1.0f, 20.0f); // { Back }
	glColor3f(1.0f, 0.0f, 1.0f); glVertex3f(-5.0f, 11.0f, 20.0f); // { Back }

	glColor3f(1.0f, 0.0f, 1.0f); glVertex3f(5.0f, 11.0f, 20.0f); // { Left }
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(5.0f, 11.0f, 10.0f); // { Left }
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(5.0f, 1.0f, 10.0f); // { Left }
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(5.0f, 1.0f, 20.0f); // { Left }

	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-5.0f, 11.0f, 10.0f); // { Top }
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(5.0f, 11.0f, 10.0f); // { Top }
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(5.0f, 11.0f, 20.0f); // { Top }
	glColor3f(1.0f, 0.0f, 1.0f); glVertex3f(-5.0f, 11.0f, 20.0f); // { Top }

	glColor3f(1.0f, 0.0f, 1.0f); glVertex3f(5.0f, 1.0f, 10.0f); // { Bottom }
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(5.0f, 1.0f, 20.0f); // { Bottom }
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(-5.0f, 1.0f, 20.0f); // { Bottom }
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(-5.0f, 1.0f, 10.0f); // { Bottom }
	glEnd();
	*/

	glFlush();
}

// 쉐이더 적용 시 디버그 할 때 쓰는 함수들
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