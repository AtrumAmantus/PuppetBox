#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out VS_OUT
{
    vec2 uvCoord;
    int instanceIndex;
} vs_out;

layout(std140) uniform Transforms
{
    mat4 uiProjection;
    mat4 projection;
    mat4 view;
};

/*
    Instance data is a flat float array that follows the given schema:

    struct GlyphData
    {
        vec3 position;
        vec2 dimension;
        vec2 uvCoord;
        vec2 uvDimensions;
    };
*/

uniform float instanceData[225];

void main()
{
    int instanceIndex = gl_InstanceID * 9;
    vec3 adjustedPosition;

    /* This remaps all the vertex positions to what they should actually be based on the instance data */
    adjustedPosition.x = instanceData[instanceIndex] + (instanceData[instanceIndex + 3] * position.x);
    adjustedPosition.y = instanceData[instanceIndex + 1] + (instanceData[instanceIndex + 4] * position.y);
    adjustedPosition.z = instanceData[instanceIndex + 2];

    gl_Position = uiProjection * vec4(adjustedPosition, 1.0);
    vs_out.instanceIndex = instanceIndex;
    vs_out.uvCoord = uv;
}