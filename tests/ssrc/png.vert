#version 450

layout(push_constant) uniform PushConstants {
	vec2 scale;
};

layout(location = 0) out vec2 vUV;

void main()
{
	// A quad covering the screen, scaled down to keep the image's aspect ratio.
	const vec2 corners[6] = vec2[](
		vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0),
		vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(1.0, 1.0));
	const vec2 uv = corners[gl_VertexIndex];
	gl_Position = vec4(((uv * 2.0) - 1.0) * scale, 0.0, 1.0);
	// The viewport is flipped (Y up), but PNG rows go top to bottom.
	vUV = vec2(uv.x, 1.0 - uv.y);
}
