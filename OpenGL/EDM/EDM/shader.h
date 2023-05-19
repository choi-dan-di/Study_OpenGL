
// ���̴� ���� ���� ó�� �Լ���
const string ReadStringFromFile(const string& filename);
GLuint CreateShader(GLenum shaderType, const string& source);
bool CheckShader(GLuint shader);
bool CheckProgram(GLuint program);

// Shader Debugging
int printOglError(char* file, int line);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);