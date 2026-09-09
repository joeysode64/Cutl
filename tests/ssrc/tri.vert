#version 450

layout(location = 0) out vec3 vPos;

void main() {
	vec2 positions[3] = vec2[3](
		vec2(0.0, -0.5),
		vec2(0.5, 0.5),
		vec2(-0.5, 0.5)
	);

	vec2 p = positions[gl_VertexIndex];
	gl_Position = vec4(p, 0.0, 1.0);
	vPos = vec3(p, 0.0);
}
