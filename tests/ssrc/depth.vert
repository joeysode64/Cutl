#version 450

layout(push_constant) uniform PushConstants {
	vec4 color;
	float z;
	float scale;
} pc;

layout(location = 0) out vec4 vColor;

void main()
{
	vec2 positions[3] = vec2[3](
		vec2(-0.75, -0.75),
		vec2(0.0, 0.75),
		vec2(0.75, -0.75)
	);

	gl_Position = vec4(positions[gl_VertexIndex] * pc.scale, pc.z, 1.0);
	vColor = pc.color;
}
