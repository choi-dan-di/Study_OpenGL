#include <GLEW/glew.h>

// �� �ε�
bool loadOBJ(
	const char* path, 
	vector<vec3>& out_vertices, 
	vector<vec2>& out_uvs, 
	vector<vec3>& out_normals
);

// Tangent Space ���ؼ� �߰� (ź��Ʈ�� ���ϴ� �Լ�)
void getTangent(
	vector<vec3>& vertices,
	vector<vec2>& uvs,
	vector<vec3>& normals,
	vector<vec3>& tangents
);

// �ؽ�ó �ε� (BMP ����)
GLuint loadBMP_stb(const char* imagepath);