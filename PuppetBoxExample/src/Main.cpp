#include <PuppetBox.h>
#include <puppetbox/Constants.h>

#include "CustomSceneHandler.h"

// Entry point is in SDL2
int main(int argc, char** argv)
{
	PB::Init("PuppetBox - My Window", 800, 600);

	std::string primaryScene = "MyScene";

	PB::CreateScene(primaryScene);
	PB::SetSceneCameraMode(primaryScene, PB::SceneView::Mode::ORTHO);
	PB::SetActiveScene(primaryScene);
	PB::LoadAssetPack("Assets1");
	CustomSceneHandler handler{};
	PB::SetSceneHandler(&handler);

	PB::Run();

	return 0;
}