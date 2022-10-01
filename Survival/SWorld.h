#pragma once

#include "stdafx.h"
#include "SEntity.h"
#include "SCamera.h"
#include "SPhysics.h"
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
	SPhysics physics;

	Vector2 Per2Pix(Vector2);
	Vector2 Pix2Per(Vector2);

	Model floor;
	Vector3 floorPosition;

	Model wall;
	Model sky;

	Player* player;

};
