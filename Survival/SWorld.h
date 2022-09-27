#pragma once

#include "stdafx.h"
#include "SEntity.h"
#include "SCamera.h"
#include "Player.h"


class SWorld : public SEntity
{
public:
	SWorld();

	void UpdateFrame() override;
	void Load() override;
	void UnLoad() override;
private:
	Camera2D hud;
	SCamera camera;

	Vector2 Per2Pix(Vector2);
	Vector2 Pix2Per(Vector2);

	Model floor;
	Model wall;
	Model sky;

	Player* player;
};
