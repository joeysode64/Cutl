#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0) uniform texture2D images[];
layout(set = 0, binding = 1) uniform sampler samplers[];

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(sampler2D(images[0], samplers[0]), vUV);
}
