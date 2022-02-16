#include <PuppetBox.h>

#include "CustomSceneHandler.h"

// Required signature, Entry point is in SDL2
int main(int argc, char** argv)
{
    PB::Init("PuppetBox - My Window", 800, 600, 1000);

    std::string primaryScene = "MyScene";

    PB::CreateScene(primaryScene);
    PB::SetActiveScene(primaryScene);
    PB::LoadAssetPack("Assets1");
    CustomSceneHandler handler{};
    if (!PB::SetSceneHandler(&handler))
    {
        std::cout << "Failed to set up scene" << std::endl;
        return 1;
    }

    PB::Run();

    return 0;
}