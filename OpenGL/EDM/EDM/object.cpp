#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

#include "object.h"

// �̹��� �ε� �ܺ� ���̺귯��
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool loadOBJ(const char* path, vector<vec3>& out_vertices, vector<vec2>& out_uvs, vector<vec3>& out_normals)
{
    printf("Loading OBJ File %s ...\n", path);

    vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    vector<vec3> temp_vertices;
    vector<vec2> temp_uvs;
    vector<vec3> temp_normals;

    // ���� ����
    FILE* file = fopen(path, "r");
    if (file == NULL)
    {
        printf("������ �� �� ����! ��� Ȯ��\n");
        getchar();
        return false;
    }

    // ���������� ���� �������� ����
    while (1)
    {
        // �� �پ� �о����
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;  // EOF : End of File

        // ����
        if (strcmp(lineHeader, "v") == 0)
        {
            vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        // �ؽ�ó ��ǥ
        else if (strcmp(lineHeader, "vt") == 0)
        {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            uv.y = -uv.y;   // uv�� y��ǥ�� �ݴ�����̹Ƿ� ��ȣ �ݴ��
            temp_uvs.push_back(uv);
        }
        // ���
        else if (strcmp(lineHeader, "vn") == 0)
        {
            vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        // ��(�ﰢ�� ����, face)
        else if (strcmp(lineHeader, "f") == 0)
        {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                &vertexIndex[2], &uvIndex[2], &normalIndex[2]
            );
            if (matches != 9)
            {
                printf("���� �� ���� ���� �����Դϴ�.");
                fclose(file);
                return false;
            }

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    // �ﰢ�� ������ ���� ����
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];

        vec3 vertex = temp_vertices[vertexIndex - 1];
        vec2 uv = temp_uvs[uvIndex - 1];
        vec3 normal = temp_normals[normalIndex - 1];

        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }
    // ���� �ݱ�
    fclose(file);
    return true;
}

void getTangent(vector<vec3>& vertices, vector<vec2>& uvs, vector<vec3>& normals, vector<vec3>& tangents)
{
    // ������ 3�� ���(������ �޽�)�� ������ ���� ����
    for (unsigned int i = 0; i < vertices.size(); i += 3)
    {
        // �̸� ���̱�
        vec3& v0 = vertices[i + 0];
        vec3& v1 = vertices[i + 1];
        vec3& v2 = vertices[i + 2];

        vec2& uv0 = uvs[i + 0];
        vec2& uv1 = uvs[i + 1];
        vec2& uv2 = uvs[i + 2];

        // ź��Ʈ�� ���Ϸ��� ����� �״�� ����ϰ�
        // �׶�-����Ʈ ����ؾ��ϳ�..?
        // ���� ���鼭 �ﰢ���� ����� �� ���� ���ϱ�
        vec3 deltaPos1 = v1 - v0;
        vec3 deltaPos2 = v2 - v0;

        // UV
        vec2 deltaUV1 = uv1 - uv0;
        vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

        // �� ������ ź��Ʈ ���ʹ� ��� ����
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);
    }
}

GLuint loadBMP_stb(const char* imagepath)
{
    printf("STB Reading image %s ...\n", imagepath);

    int width, height, channel;
    GLuint textureID;

    glGenTextures(1, &textureID);

    // �ؽ�ó ���ε� �� ���͸�
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // ���� ����
    unsigned char* data = stbi_load(imagepath, &width, &height, &channel, 0);
    if (data)
    {
        // �ؽ�ó��, �Ӹ� ����
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Error: No Data!\n");
        getchar();
        return 0;
    }
    // ���� �޸� �Ҵ� ����
    stbi_image_free(data);

    // �ؽ�ó ���ε� ����
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}
