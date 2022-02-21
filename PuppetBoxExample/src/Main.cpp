#include <PuppetBox.h>

#include "CustomSceneHandler.h"

// Required signature, Entry point is in SDL2
int main(int argc, char** argv)
{
    PB::Init("PuppetBox - My Window", 800, 600, 1000);

    std::shared_ptr<PB::AbstractSceneGraph> myScene{nullptr};

    PB::Run([&myScene]() {
        myScene = std::make_shared<CustomSceneHandler>("Main Scene");

        PB::CreateScene(myScene);
        PB::SetActiveScene("Main Scene");
    });

    return 0;
}