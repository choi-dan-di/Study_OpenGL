#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 v_color;

uniform mat4 ModelMat, ViewMat, ProjMat;

out vec3 fragColor;

void main()
{
	gl_Position = ProjMat * ViewMat * ModelMat * vec4(position, 1.0f);
	fragColor = v_color;
}