#pragma once
#include "../../Terminal/Renderer.h"
#include "../Entities/Player.h"
class Hud
{
public:
	void drawCrosshair(Renderer& renderer);

	void drawHotbar(const Player& player, Renderer& renderer);

};

