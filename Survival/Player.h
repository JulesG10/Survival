#pragma once


#include "stdafx.h"
#include "SEntity.h"
#include "SCamera.h"



class Player : public SEntity
{
public:
	Player(SCamera* worldCam);

	void UpdateFrame() override;
	void Load() override;
	void UnLoad() override;

	Vector3 GetVelocity();
	Camera3D GetGunCamera();
private:
	void AnimateVector3(Vector3 target, Vector3* val, float speed, float margin);
	float walkGunMode = 0;

	Vector3 velocity;

	SCamera* camera;
	SCameraMode camMode = SCameraMode::FirstPerson;

	RenderTexture playerHands;
	Camera3D playerhCam;

	Model gun;
	Model skin;
};

