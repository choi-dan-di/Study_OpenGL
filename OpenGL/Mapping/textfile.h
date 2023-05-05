// textfile.h: interface for reading and writing text files
// www.lighthouse3d.com
//
// You may use these functions freely.
// they are provided as is, and no warranties, either implicit,
// or explicit are given
//////////////////////////////////////////////////////////////////////

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
using namespace std;

#include <glm/glm.hpp>
using namespace glm;

char* textFileRead(const char* fn);
int textFileWrite(const char* fn, const char* s);

bool loadOBJ(
	const char* path,
	vector<vec3> &out_vertices,
	vector<vec2> out_uvs,
	vector<vec3> out_normals
);