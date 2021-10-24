#version 330 core
out vec4 FragColor;

in VS_OUT
{
	vec2 uvCoord;
} vs_out;

struct Material
{
	sampler2D diffuseMap;
	sampler2D emissionMap;
	sampler2D normalMap;
	sampler2D specularMap;
	uint specularValue;
	float emissionValue;
};

uniform Material material;

void main()
{
	vec4 _diffuseTexel = texture(material.diffuseMap, vs_out.uvCoord);
	
	if (_diffuseTexel.a < 0.1) discard;
	
	FragColor = _diffuseTexel;
}