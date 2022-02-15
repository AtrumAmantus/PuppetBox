#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
out vec2 TexCoords;

layout(std140) uniform Transforms
{
    mat4 uiProjection;
    mat4 projection;
    mat4 view;
};

void main()
{
    gl_Position = uiProjection * vec4(position.xyz, 1.0);
    TexCoords = uv;
}