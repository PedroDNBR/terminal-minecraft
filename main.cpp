#include "Renderer/Renderer.h"
#include "Engine/VoxelEngine.h"
#include "World/Camera.h"

int main()
{
	Renderer renderer(80, 50);
	VoxelEngine voxelEngine;
	Camera camera;

	camera.pitch = 0;

	camera.fovRadius = camera.fov * 3.14159f / 180.f;
	camera.focalLen = 1.0f / tanf(camera.fovRadius * .5f);

	camera.updateCatheti();

	while (true)
	{
		renderer.hasWindowResized();

		renderer.clear();

		camera.updateCatheti();

		voxelEngine.render(renderer, camera);

		renderer.present();
	}

}