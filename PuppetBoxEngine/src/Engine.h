#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

#include <sdl2/SDL.h>

#include "puppetbox/AbstractInputReader.h"
#include "puppetbox/AbstractSceneGraph.h"
#include "puppetbox/Event.h"

#include "IGfxApi.h"
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
        Engine(
                std::shared_ptr<IGfxApi>& gfxApi,
                Sdl2Initializer hardwareInitializer,
                std::shared_ptr<AbstractInputReader>& inputReader);

        /**
         * \brief Initialize engine configurations.
         */
        void init();

        /**
        * \brief Executes the engine, starting the primary game loop and processing input.
        */
        void run();

        /**
        * \brief Attempts to shut down the game engine.
        */
        void shutdown();

    private:
        std::shared_ptr<IGfxApi> gfxApi_;
        Sdl2Initializer hardwareInitializer_;
        std::shared_ptr<AbstractInputReader> inputReader_;
        std::shared_ptr<AbstractSceneGraph> currentScene_;
        std::unordered_map<std::string, std::shared_ptr<AbstractSceneGraph>> sceneGraphs_{};

    private:
        /**
        * \brief Handles processing of the hardware input.
        */
        void processInput();
    };
}