#pragma once

#include "puppetbox/AbstractSceneGraph.h"

namespace PB
{
    //TODO: Add some default rendered objects to this scene graph
    class DefaultSceneGraph : public AbstractSceneGraph
    {
    public:
        DefaultSceneGraph(
                const std::string sceneName,
                RenderWindow renderWindow,
                std::shared_ptr<AbstractInputReader> reader,
                std::unique_ptr<AbstractRenderComponent> renderComponent)
                : AbstractSceneGraph(sceneName, renderWindow, reader, std::move(renderComponent)) {};
    };
}