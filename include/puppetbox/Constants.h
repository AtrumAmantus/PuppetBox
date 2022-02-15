#pragma once

namespace PB
{
    namespace LibraryAsset
    {
        /**
        * \brief The publicly exposed Asset types to be used by consuming application to indicate
        * desired asset types to be loaded.
        */
        enum Type
        {
            MODEL_2D
        };
    }

    namespace SceneView
    {
        /**
        * \brief The publicly exposed SceneView types to be used by the consuming application.
        */
        enum Mode
        {
            ORTHO,
            PERSPECTIVE,
            UI
        };
    }

    namespace AI
    {
        /**
        * \brief The publicly exposed AI types to be used by the consuming application.
        */
        enum Behavior
        {
            WANDER
        };
    }
}