#pragma once

#include <string>

#include "TypeDef.h"

namespace PB
{
	/**
	* \brief Interface to be used in application logic to invoke hardware interactions using
	* the specifically loaded hardware library.
	*/
	class IHardwareInitializer
	{
	public:
		/**
		* \brief Invokes the hardware library specific functions to initialize hardware configuration
		* for future interactions.
		*
		* \param windowTitle	The desired title for the window to be created.
		* \param windowWidth	The desired width for the window to be created.
		* \param windowHeight	The desired height for the window to be created.
		*/
		virtual void init(std::string windowTitle, std::int32_t windowWidth, std::int32_t windowHeight) = 0;

		/**
		* \brief Releases any allocated resources and cleans up hardware specific library configurations.
		*/
		virtual void destroy() = 0;

		/**
		* \brief Determines if an error occured during hardware initialization.
		*
		* \return True if an error occured during initialization, False otherwise.
		*/
		virtual bool hadError() const = 0;

		/**
		* \brief The hardware library specific commands to be executed after each loop, such as buffer swapping.
		*/
		virtual void postLoopCommands() const = 0;

		/**
		* \brief Initialize the game time for later tracking time deltas between frames.
		*/
		virtual void initializeGameTime() = 0;

		/**
		* \brief Re-calculate time elapsed since last invocation.
		*
		* \return The amount of time (in Milliseconds) since the last time the method was invoked.
		*/
		virtual float updateElapsedTime() = 0;

		/**
		* \brief Get a reference to the process address for configuring function pointers.
		*
		* \return The hardware library specific process address for function pointers.
		*/
		virtual ProcAddress getProcAddress() const = 0;

		/**
		* \brief Identifies the specific IHardwareInitializer by a string value.
		*
		* \return The specific IHardwareInitializer identifier for this hardware library implementation.
		*/
		virtual std::string initializerName() const = 0;
	};
}