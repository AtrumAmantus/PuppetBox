#version 330 core
in GS_OUT
{
    vec2 uvCoord;
} gs_out;

out vec4 color;

uniform sampler2D text;
uniform vec3 textColour;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, gs_out.uvCoord).r);
    color = vec4(textColour, 1.0) * sampled;
}