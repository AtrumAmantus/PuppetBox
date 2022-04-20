#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in mat4 instanceTransform;
layout (location = 6) in vec3 instanceDimensions;
layout (location = 7) in vec2 instanceUvCoords;
layout (location = 8) in vec2 instanceUvDimensions;

out VS_OUT
{
    vec2 uvCoord;
    vec2 uvCoordOffset;
    vec2 uvDimensions;
    int instanceIndex;
} vs_out;

layout(std140) uniform Transforms
{
    mat4 uiProjection;
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
    int instanceIndex = gl_InstanceID * 9;
    vec3 adjustedPosition;

    /* Transform the vertices to match desired mesh dimensions */
    adjustedPosition.x = instanceDimensions.x * position.x;
    adjustedPosition.y = instanceDimensions.y * position.y;
    adjustedPosition.z = instanceDimensions.z;

    /* Transform vertices to desired world location */
    gl_Position = uiProjection * instanceTransform * vec4(adjustedPosition, 1.0);

    vs_out.instanceIndex = instanceIndex;
    vs_out.uvCoord = uv;
    vs_out.uvCoordOffset = instanceUvCoords;
    vs_out.uvDimensions = instanceUvDimensions;
}