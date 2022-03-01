#include <PuppetBox.h>

#include "MultiplayerSceneGraph.h"
#include "SingleplayerSceneGraph.h"

// Required signature, Entry point is in SDL2
int main(int argc, char** argv)
{
    PB::Init("PuppetBox - My Window", 800, 600, 1000);

    PB::Run([]() {
        auto singleplayerScene = std::make_shared<SingleplayerSceneGraph>("Singleplayer");

        PB::CreateScene(singleplayerScene);
        PB::SetActiveScene("Singleplayer");

        auto multiplayerScene = std::make_shared<MultiplayerSceneGraph>("Multiplayer");
        PB::CreateScene(multiplayerScene);

        return true;
    });

    return 0;
}