#pragma once

#include "puppetbox/DataStructures.h"

namespace PB
{
	/**
	* \brief Interface for defining GFX API specific data to be used in rendering calls.
	*/
	class IModel
	{
	public:
		/**
		* \brief Method used by application logic to render an object using the GFX API invocations of
		* the derived implementation.
		*/
		virtual void render(mat3 transform) const = 0;
	};
}