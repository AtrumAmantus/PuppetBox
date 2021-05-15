#pragma once

namespace PB
{
	/**
	* \brief OpenGL specific mesh object for storing mesh related rendering data to be used
	* in rendering calls.
	*/
	struct Mesh
	{
		uint32_t VBO = 0;
		uint32_t EBO = 0;
		uint32_t VAO = 0;
		uint32_t drawCount = 0;
	};
}