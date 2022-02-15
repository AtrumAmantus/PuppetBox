#pragma once

namespace PB
{
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