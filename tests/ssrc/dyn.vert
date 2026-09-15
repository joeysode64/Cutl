#version 450
#extension GL_EXT_buffer_reference2 : require

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer VertexBuffer {
	vec2 positions[3];
};

layout(push_constant) uniform PushConstants {
	VertexBuffer vertices;
} pc;

layout(location = 0) out vec3 vPos;

void main() {
	gl_Position = vec4(pc.vertices.positions[gl_VertexIndex].xy, 0.0, 1.0);
}
