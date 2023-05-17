#version 450

in vec3 fragColor;

out vec4 color;

void main()
{
	// color = vec4(0.8, 0.5, 0.5, 1.0);
	color = vec4(fragColor, 1.0);
}