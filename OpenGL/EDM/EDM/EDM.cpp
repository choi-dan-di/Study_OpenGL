#include "pch.h"
#include "shader.h"
#include "object.h"

// -------------------- 전방 선언 --------------------
// 앱 초기화
void InitApp();
// 쉐이더 세팅
void SetShaders();

// 콜백 함수
// Error Check
void ErrorCallBack(int error, const char* description);
// Call Back Functions
void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
void CursorPositionCallBack(GLFWwindow* window, double xpos, double ypos);
void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
void WindowSizeChangeCallback(GLFWwindow* window, int width, int height);

// 카메라 이동에 필요한 변수들 전역으로 지정
vec3 cameraPos = vec3(0.0f, 0.8f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 마우스 이동에 필요한 변수들 전역으로 지정
// 마우스 위치 초기화
// Cursor Position
float lastX = (float)1024 / (float)2;
float lastY = (float)768 / (float)2;
// 각도
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

// 스크롤 시 줌 기능 구현
float fov = 45.0f;

// 윈도우 객체
GLFWwindow* window = NULL;
// 쉐이더 핸들러
GLuint shaderProgramID;

int main(int argc, char** argv)
{
	// -------------------- 윈도우 세팅 --------------------
	// 에러 핸들러 등록
	glfwSetErrorCallback(ErrorCallBack);

	// GLFW 초기화
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// 안티앨리어싱 4배
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	// 커서 위치 초기화
	glfwSetCursorPos(window, lastX, lastY);
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

	// --------------------------------------------------------------
	// OBJ 파일 불러오기
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	bool res = loadOBJ("object/tree.obj", vertices, uvs, normals);

	GLuint vertexbuffer;
	// OpenGL 4.5의 버퍼 생성 방식
	glCreateBuffers(1, &vertexbuffer);
	glNamedBufferData(vertexbuffer, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glCreateBuffers(1, &uvbuffer);
	glNamedBufferData(uvbuffer, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glCreateBuffers(1, &normalbuffer);
	glNamedBufferData(normalbuffer, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);

	// 텍스처 로드
	GLuint Texture = loadBMP_stb("texture/tree_default_Albedo.bmp");

	// 라이팅 위해서 빛 벡터, 뷰 벡터 설정
	vec3 lightPos = vec3(6, 7, 5);
	vec3 eyePos = cameraPos;
	// ortho : 왼, 오, 하, 상, N, F 순
	//mat4 lightProjMat = ortho<float>(-10, 10, -10, 10, 0.1f, 100.0f);
	//mat4 lightViewMat = lookAt(lightPos, vec3(0, 0, 0), vec3(0, 1, 0));

	// 노멀맵 로드
	GLuint NormalMap = loadBMP_stb("texture/tree_default_Normal.bmp");

	// 탄젠트 공간 위해 탄젠트 정의
	vector<vec3> tangents;
	getTangent(vertices, uvs, normals, tangents);
	// Tangent Buffer 생성
	GLuint tangentbuffer;
	glCreateBuffers(1, &tangentbuffer);
	glNamedBufferData(tangentbuffer, tangents.size() * sizeof(vec3), &tangents[0], GL_STATIC_DRAW);

	// 프로그램에 전달할 ID 설정
	// 변환 행렬
	GLuint M_MatID = glGetUniformLocation(shaderProgramID, "ModelMat");
	GLuint V_MatID = glGetUniformLocation(shaderProgramID, "ViewMat");
	GLuint P_MatID = glGetUniformLocation(shaderProgramID, "ProjMat");
	// 텍스처
	GLuint TextureID = glGetUniformLocation(shaderProgramID, "colorMap");
	// 라이팅
	GLuint lightPosID = glGetUniformLocation(shaderProgramID, "lightPos");
	GLuint eyePosID = glGetUniformLocation(shaderProgramID, "eyePos");
	//GLuint lightProjMatID = glGetUniformLocation(shaderProgramID, "lightProjMat");
	//GLuint lightViewMatID = glGetUniformLocation(shaderProgramID, "lightViewMat");
	// 노멀맵
	GLuint NormalMapID = glGetUniformLocation(shaderProgramID, "normalMap");

	// --------------------------------------------------------------

	// 주 렌더링 루프. 윈도우가 종료되기 전까지 반복
	while (!glfwWindowShouldClose(window))
	{
		// -------------------- 렌더링 --------------------
		// 키 콜백 이벤트위해서 deltaTime 지정
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Clear the screen : 버퍼 초기화
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 프로그램 활성화
		glUseProgram(shaderProgramID);

		// 모델, 뷰 및 투영 변환(콜백 때문에 루프문 안에서 실시간으로 변경될 수 있도록 설정)
		// mat4 Model = mat4(1.0);
		mat4 Model = mat4(
			0.2, 0.0, 0.0, 0.0,
			0.0, 0.2, 0.0, 0.0,
			0.0, 0.0, 0.2, 0.0,
			0.0, 0.0, 0.0, 1.0
		);
		mat4 View = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		// perspective : 뷰 프러스텀(절두체) 정의
		// ortho : 직교 투영 변환(원근감 X) 정의
		mat4 Projection = perspective(radians(fov), (float)1024 / (float)768, 0.1f, 100.0f);

		// 유니폼으로 넘겨주기
		// 변환 행렬 MVP
		glUniformMatrix4fv(M_MatID, 1, GL_FALSE, &Model[0][0]);
		glUniformMatrix4fv(V_MatID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(P_MatID, 1, GL_FALSE, &Projection[0][0]);

		// 빛 공간 변환 행렬
		glUniform3fv(lightPosID, 1, &lightPos[0]);
		glUniform3fv(eyePosID, 1, &eyePos[0]);
		//glUniformMatrix4fv(lightProjMatID, 1, GL_FALSE, &lightProjMat[0][0]);
		//glUniformMatrix4fv(lightViewMatID, 1, GL_FALSE, &lightViewMat[0][0]);

		// 텍스처 바인드
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);
		// 노멀맵
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, NormalMap);
		glUniform1i(NormalMapID, 1);

		// 정점 애트리뷰트 활성화하고 버퍼에 바인딩
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		// Draw Call
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		// 활성화 했던 정점 애트리뷰트 비활성
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		// VBO 바인딩 해제
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// 프로그램 사용 중지
		glUseProgram(0);
		// ----------------------------------------

		// 렌더 버퍼 교체 (그린 결과를 디스플레이하는 명령)
		glfwSwapBuffers(window); 
		// 윈도우 이벤트 (키 스트로크 등) 폴링
		glfwPollEvents();
	}

	// 버퍼 제거
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &tangentbuffer);

	// 프로그램 제거
	glDeleteProgram(shaderProgramID);

	// 텍스처 제거
	glDeleteTextures(1, &Texture);
	glDeleteTextures(1, &NormalMap);

	// 윈도우 제거
	glfwDestroyWindow(window);

	// GLFW 종료
	glfwTerminate();

	return 0;
}

// 앱 초기화
void InitApp()
{
	// 클리어 색상(배경색) 지정
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	// Z-버퍼링 사용
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// 뒷면 제거
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// 앞면 삼각형은 채우고 뒷면 삼각형은 라인만
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	/*
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	*/

	// 쉐이더 세팅 및 프로그램 링크
	SetShaders();
}

// -------------------- 쉐이더 세팅 --------------------
void SetShaders()
{
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
	if (!CheckProgram(shaderProgramID))
	{
		glDeleteProgram(shaderProgramID);
		exit(EXIT_FAILURE);
	}

	// 쉐이더 붙인거 있으면 여기서 떼고(Detach) 쉐이더 삭제
	glDetachShader(shaderProgramID, vertShaderObj);
	glDetachShader(shaderProgramID, fragShaderObj);

	// 쉐이더 오브젝트 삭제
	glDeleteShader(vertShaderObj);
	glDeleteShader(fragShaderObj);
}

// -------------------- 콜백 이벤트 함수 --------------------
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

	float cameraSpeed = 2.5f * deltaTime;
	// float cameraSpeed = 1.0f;
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos += cameraSpeed * cameraFront;
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos -= cameraSpeed * cameraFront;
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
	if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos -= cameraSpeed * cameraUp;
	if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraPos += cameraSpeed * cameraUp;
}

// 마우스 콜백 처리 함수
void CursorPositionCallBack(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}
	// 프레임 사이의 움직임 offset 계산
	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;	// y 좌표의 범위는 반대
	lastX = (float)xpos;
	lastY = (float)ypos;

	float sensitivity = 0.1f;	// 감도
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

// 스크롤 콜백 처리 함수
void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
	// 줌 구현
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= (float)yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

// 윈도우 리사이징 콜백 처리 함수
void WindowSizeChangeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}