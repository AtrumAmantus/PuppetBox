#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUv;

out VS_OUT
{
	vec2 uvCoord;
} vs_out;

layout(std140) uniform Transforms
{
	mat4 uiProjection;
	mat4 projection;
	mat4 view;
};
uniform mat4 model;
uniform mat4 boneTransform;
uniform mat4 meshTransform;

void main()
{
	vec4 local = vec4(aPos, 1.0);
	
	gl_Position = projection * view * model * boneTransform * meshTransform * local;
	
	vs_out.uvCoord = aUv;
}