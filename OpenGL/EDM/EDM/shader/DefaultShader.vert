#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;

uniform mat4 ModelMat, ViewMat, ProjMat;
uniform vec3 lightPos, eyePos;

out vec2 v_texCoord;
out vec3 v_lightTS, v_viewTS;

void main()
{
	gl_Position = ProjMat * ViewMat * ModelMat * vec4(position, 1.0);
	v_texCoord = texCoord;

	// ÅºÁ¨Æ® °ø°£ Á¤ÀÇ
	vec3 Nor = normalize(transpose(inverse(mat3(ModelMat))) * normal);
	vec3 Tan = normalize(transpose(inverse(mat3(ModelMat))) * tangent);
	vec3 Bit = cross(Nor, Tan);
	// ÅºÁ¨Æ® °ø°£ º¯È¯ Çà·Ä
	mat3 tbnMat = transpose(mat3(Tan, Bit, Nor));

	// ºû º¤ÅÍ, ºä º¤ÅÍ¸¦ ÅºÁ¨Æ® °ø°£À¸·Î º¯È¯
	vec3 worldPos = (ModelMat * vec4(position, 1.0)).xyz;
	v_lightTS = tbnMat * normalize(lightPos);
	v_viewTS = tbnMat * normalize(eyePos - worldPos);
}