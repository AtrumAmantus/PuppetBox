#pragma once

#include "DataStructures.h"

namespace PB
{
	/**
	* \brief Interface for defining GFX API specific data to be used in rendering calls.
	*/
	class IModel
	{
	public:
		virtual void updateModelMatrix(mat3 transform) = 0;

		/**
		* \brief Method used by application logic to render an object using the GFX API invocations of
		* the derived implementation.
		*/
		virtual void render() const = 0;
	};
}