#include <PuppetBox.h>

#include "CustomSceneHandler.h"

// Entry point is in SDL2
int main(int argc, char** argv)
{
	PB::Init("PuppetBox - My Window", 800, 600);

	PB::CreateScene("MyScene");
	PB::SetActiveScene("MyScene");
	PB::LoadAssetLibrary("../spritelib1.zip");
	CustomSceneHandler handler{};
	PB::SetSceneHandler(&handler);

	PB::Run();

	return 0;
}