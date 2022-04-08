#include <PuppetBox.h>

#include "SinglePlayerSceneGraph.h"

// Required signature, Entry point is in SDL2
int main(int argc, char** argv)
{
    PB::Init("PuppetBox - My Window", 800, 600, 1000);

    PB::Run([]() {
        auto singlePlayerScene = std::make_shared<SinglePlayerSceneGraph>("SinglePlayer");

        PB::CreateScene(singlePlayerScene);
        PB::SetActiveScene("SinglePlayer");

        return true;
    });

    return 0;
}