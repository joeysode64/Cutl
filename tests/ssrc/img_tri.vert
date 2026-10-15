#version 450
#extension GL_EXT_buffer_reference2 : require

layout(buffer_reference, std430, buffer_reference_align = 8) readonly buffer VertexBuffer {
	vec2 positions[];
};

layout(push_constant) uniform PushConstants {
	vec4 color;
	VertexBuffer vertexBuffer;
} pc;

void main()
{
	gl_Position = vec4(pc.vertexBuffer.positions[gl_VertexIndex], 0.0, 1.0);
}
