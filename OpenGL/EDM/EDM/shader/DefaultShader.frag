#version 450

in vec2 v_texCoord;
in vec3 v_lightTS, v_viewTS;

uniform sampler2D colorMap, normalMap;
uniform vec3 lightPos;

out vec4 color;

void main()
{
	// Æþ ¶óÀÌÆÃ
	// ±¤¿øÀÇ »ö»ó
	vec3 lightColor = vec3(1.0);
	// ½ºÆäÅ§·¯ °è¼ö
	vec3 matSpec = vec3(1.0);
	// ¾Úºñ¾ðÆ® ±¤¿ø »ö»ó
	vec3 srcAmbi = vec3(0.3, 0.3, 0.3);

	// normalization
	vec3 normal = normalize(2.0 * texture(normalMap, v_texCoord).xyz - 1.0);
	vec3 light = normalize(v_lightTS);
	vec3 view = normalize(v_viewTS);

	// Diffuse Term
	vec3 matDiff = texture(colorMap, v_texCoord).rgb;
	vec3 diff = max(dot(normal, light), 0.0) * lightColor * matDiff;

	// Specular Term
	vec3 refl = 2.0 * normal * dot(normal, light) - light;
	vec3 spec = pow(max(dot(refl, view), 0.0), 30.0) * lightColor * matSpec;

	// Ambient Term
	vec3 ambi = srcAmbi * matDiff;

	// Emissive Term


	color = vec4(diff + spec + ambi, 1.0);
}