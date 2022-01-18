#version 330 core
layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

layout(std140) uniform Transforms
{
    mat4 orthoProjection;
    mat4 projection;
    mat4 view;
};

void main()
{
    gl_Position = orthoProjection * vec4(vertex.xy, 1.0, 1.0);
    TexCoords = vertex.zw;
}