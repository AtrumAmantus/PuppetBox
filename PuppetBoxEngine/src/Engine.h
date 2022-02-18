#pragma once

#include <thread>

#include <sdl2/SDL.h>

#include "puppetbox/AbstractInputReader.h"
#include "puppetbox/Event.h"
#include "EventDef.h"
#include "IGfxApi.h"
#include "MessageBroker.h"
#include "SceneGraph.h"
#include "Sdl2Initializer.h"

namespace PB
{
    /**
    * \brief The primary class of the game engine, used to initialize hardware and GFX APIs, and
    * responsible for executing the primary game loop and basic input handling.
    */
    class Engine
    {
    public:
        /**
        * \brief Creates an engine instance using the given specific IGfxApi, Hardware, and Input
        * implementations.
        *
        * \param gfxApi					The specific GFX API implementation to be used.
        * \param hardwareInitializer	The specific hardware library implementation.
        * \param inputReader			The specific input processor for the given hardware library implementation.
        */
        Engine(IGfxApi& gfxApi, Sdl2Initializer hardwareInitializer, AbstractInputReader& inputReader)
                : gfxApi_(gfxApi), hardwareInitializer_(std::move(hardwareInitializer)), inputReader_(inputReader) {};

        /**
        * \brief Executes the engine, starting the primary game loop and processing input.
        */
        void run();

        /**
        * \brief Attempts to shut down the game engine.
        */
        void shutdown();

        /**
        * \brief Sets the currently active scene to be used in update and render logic.
        */
        void setScene(SceneGraph* scene);

    private:
        IGfxApi& gfxApi_;
        Sdl2Initializer hardwareInitializer_;
        AbstractInputReader& inputReader_;
        SceneGraph* scene_ = nullptr;

    private:
        /**
        * \brief Handles processing of the hardware input.
        */
        void processInput();
    };
}