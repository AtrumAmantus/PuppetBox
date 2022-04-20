#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 uvCoord;
    vec2 uvCoordOffset;
    vec2 uvDimensions;
    int instanceIndex; //TODO: Still sorting out the UV coordinate mapping
} vs_out[3];

out GS_OUT
{
  vec2 uvCoord;
} gs_out;

void main()
{
    /* This will re-map all the UV coordinate values to what they should actually be for each vertex, per instance */
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        gs_out.uvCoord.x = vs_out[i].uvCoordOffset.x + (vs_out[i].uvCoord.x * vs_out[i].uvDimensions.x);
        gs_out.uvCoord.y = vs_out[i].uvCoordOffset.y + (vs_out[i].uvCoord.y * vs_out[i].uvDimensions.y);
        EmitVertex();
    }
}