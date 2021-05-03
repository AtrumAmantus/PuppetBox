#pragma once

#include "Math.h"

namespace PB
{
	/**
	* \brief Struct defining data for a single vertex.
	*/
	struct Vertex
	{
		vec3 position;
		vec3 normal;
		vec2 uv;
		vec3 colour{ 0.0f, 0.0f, 0.0f };
		bool useColour = false;
	};
}