#pragma once

#include "stdafx.h"

typedef enum {
	FirstPerson,
	ThirdPerson
} SCameraMode;

class SCamera
{
public:
	SCamera();
	void Init(Vector3 position, Vector3 target, float fov);

	void UpdateFirst(Vector3 velocity);
	void UpdateThird(Vector3 velocity);
	
	void SwitchUpdate(SCameraMode mode);

	Camera3D GetCamera();
	void SetPosition(Vector3);
	void SetTarget(Vector3);

	float smoothness = 0.001f;
	float sensitivity = 2.0f;

	Vector2 TPSdegMinMax = { 5.0f,  -85.0f };
	Vector2 FPSdegMinMax = { 89.9f,  -89.9f };
	Vector2 targetDistMinMax = { 1.f, 100.f };

	float targetDistance = 0.f;
	
	Vector3 rotation = { 0 };
private:
	Camera3D camera = { 0 };
};

