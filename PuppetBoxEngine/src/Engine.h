#pragma once

#include <sdl2/SDL.h>

#include "puppetbox/AbstractInputProcessor.h"
#include "IGfxApi.h"
#include "IHardwareInitializer.h"
#include "SceneGraph.h"

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
        * implemenations.
        *
        * \param gfxApi					The specific GFX API implementation to be used.
        * \param hardwareInitializer	The specific hardware library implemenation.
        * \param inputProcessor			The specific input processor for the given hardware library implementation.
        */
        Engine(IGfxApi& gfxApi, IHardwareInitializer& hardwareInitializer, AbstractInputProcessor& inputProccessor) :
                gfxApi_(gfxApi), hardwareInitializer_(hardwareInitializer), inputProcessor_(inputProccessor) {};

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
        IHardwareInitializer& hardwareInitializer_;
        AbstractInputProcessor& inputProcessor_;
        SceneGraph* scene_ = nullptr;

    private:
        /**
        * \brief Handles processing of the hardware input.
        */
        void processInput();
    };
}