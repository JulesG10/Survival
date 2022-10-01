#pragma once

#include "stdafx.h"

#define MAX_CONTACTS 8

/*
  https://www.reddit.com/r/raylib/comments/jyfh1h/how_to_properly_collide_with_mesh_height_map/ -> ?

  https://gamedev.stackexchange.com/questions/31844/checking-for-collisions-on-a-3d-heightmap or https://gamedev.stackexchange.com/questions/79431/collision-between-player-and-heightmap -> hard

  http://bedroomcoders.co.uk/raylib-adding-a-static-terrain-ode/ and http://bedroomcoders.co.uk/aligning-a-model-with-a-terrain-raylib/ -> best (ODE)
  */

typedef struct SPhysicsEntity {
	bool collision;
} SPhysicsEntity;

class SPhysics {
public:
	SPhysics();
	~SPhysics();

	bool Init(Vector3 gravity, float _physicSlice = -1.f, int _maxSteps = -1);


	bool SetTerrain(int vertexCount, float* vertices);
	void SetTerrainPosition(Vector3 position);

	dGeomID CreateBoxObject(Vector3 position, Vector3 size, int density = 1);

	void Update();

	void RemoveObject(dGeomID id);
	void UpdateObjectData(dGeomID id, Vector3 velocity, Vector3 rotation);
	Vector3* GetObjectData(dGeomID id);

	int GetInterationsPerStep();

	friend void OnCollision(void*, dGeomID, dGeomID);

private:
	float time = 0.0f;
	float physicSlice = 1.f/240.f;
	float physicTime = 0.f;
	int maxSteps = 6;

	dWorldID world = NULL;
	dSpaceID space = NULL;
	dJointGroupID contactGroup = NULL;

	dTriMeshDataID terrainData = NULL;
	dGeomID terrainId = NULL;
	int* groundInd = nullptr;
};

static void OnCollision(void*, dGeomID, dGeomID);