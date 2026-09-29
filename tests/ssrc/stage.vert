#version 450
#extension GL_EXT_buffer_reference2 : require

struct Vertex {
	vec2 position;
	vec2 _pad;
	vec4 color;
};

layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer VertexBuffer {
	Vertex vertices[];
};

layout(push_constant) uniform PushConstants {
	VertexBuffer vertexBuffer;
} pc;

layout(location = 0) out vec4 vColor;

void main()
{
	const Vertex v = pc.vertexBuffer.vertices[gl_VertexIndex];
	gl_Position = vec4(v.position, 0.0, 1.0);
	vColor = v.color;
}
