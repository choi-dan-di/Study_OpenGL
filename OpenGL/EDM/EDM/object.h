#include <GLEW/glew.h>

// 모델 로드
bool loadOBJ(
	const char* path, 
	vector<vec3>& out_vertices, 
	vector<vec2>& out_uvs, 
	vector<vec3>& out_normals
);

// Tangent Space 위해서 추가 (탄젠트만 구하는 함수)
void getTangent(
	vector<vec3>& vertices,
	vector<vec2>& uvs,
	vector<vec3>& normals,
	vector<vec3>& tangents
);

// 텍스처 로드 (BMP 파일)
GLuint loadBMP_stb(const char* imagepath);