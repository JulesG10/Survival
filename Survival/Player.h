#pragma once


#include "stdafx.h"
#include "SEntity.h"
#include "SCamera.h"
#include "SPhysics.h"


class Player : public SEntity
{
public:
	Player(SCamera* worldCam, SPhysics* physics);

	void UpdateFrame() override;
	void Load() override;
	void UnLoad() override;

	Vector3 GetVelocity();
	Camera3D GetGunCamera();
private:
	void AnimateVector3(Vector3 target, Vector3* val, float speed, float margin);
	float walkGunMode = 0;

	Vector3 velocity = {};
	Vector3 lastRotation = {};

	SCamera* camera = nullptr;
	SPhysics* physics = nullptr;

	SCameraMode camMode = SCameraMode::FirstPerson;

	RenderTexture playerHands = {0};
	Camera3D playerhCam = {0};

	Model gun = {0};
	Model skin = {0};

	dGeomID id = NULL;
};

