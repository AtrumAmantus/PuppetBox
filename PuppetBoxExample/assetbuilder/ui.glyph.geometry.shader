#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 uvCoord;
    int instanceIndex;
} vs_out[3];

out GS_OUT
{
  vec2 uvCoord;
} gs_out;

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
    // This will re-map all the UV coordinate values to what they should actually be for each vertex, per isntance
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        gs_out.uvCoord.x = instanceData[vs_out[i].instanceIndex + 5] + (vs_out[i].uvCoord.x * instanceData[vs_out[i].instanceIndex + 7]);
        gs_out.uvCoord.y = instanceData[vs_out[i].instanceIndex + 6] + (vs_out[i].uvCoord.y * instanceData[vs_out[i].instanceIndex + 8]);
        EmitVertex();
    }
}