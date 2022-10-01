#include "SPhysics.h"

SPhysics::SPhysics()
{

}

SPhysics::~SPhysics()
{
    RL_FREE(this->groundInd);

    dGeomTriMeshDataDestroy(this->terrainData);
    dGeomDestroy(this->terrainId);

    dJointGroupEmpty(contactGroup);
    dJointGroupDestroy(contactGroup);
    dSpaceDestroy(space);
    dWorldDestroy(world);
    dCloseODE();
}

bool SPhysics::Init(Vector3 gravity, float _physicSlice, int _maxSteps)
{
    if (_physicSlice != -1)
    {
        this->physicSlice = _physicSlice;
    }

    if (_maxSteps != -1)
    {
        this->maxSteps = _maxSteps;
    }

    if(!dInitODE2(0))
    {
        return false;
    }
    if (!dAllocateODEDataForThread(dAllocateMaskAll))
    {
        return false;
    }

    this->world = dWorldCreate();
    this->space = dHashSpaceCreate(0);
    this->contactGroup = dJointGroupCreate(0);

    dWorldSetGravity(world, gravity.x, gravity.y, gravity.z);
    
    // save CPU
    dWorldSetAutoDisableFlag(world, 1);
    dWorldSetAutoDisableLinearThreshold(world, 0.05);
    dWorldSetAutoDisableAngularThreshold(world, 0.05);
    dWorldSetAutoDisableSteps(world, 4);
    
    return true;
}

bool SPhysics::SetTerrain(int vertexCount, float* vertices)
{
    this->groundInd = (int*)RL_MALLOC(vertexCount * sizeof(int));

    if (groundInd != NULL)
    {
        for (int i = 0; i < vertexCount; i++)
        {
            groundInd[i] = i;
        }

        this->terrainData = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSingle(terrainData, vertices,
            3 * sizeof(float), vertexCount,
            groundInd, vertexCount,
            3 * sizeof(int));
        this->terrainId = dCreateTriMesh(space, terrainData, NULL, NULL, NULL);
        
        return true;
    }

    return false;
}

void SPhysics::SetTerrainPosition(Vector3 position)
{
    dGeomSetPosition(this->terrainId, position.x, position.y, position.z);
}

int SPhysics::GetInterationsPerStep()
{
    return dWorldGetQuickStepNumIterations(world);
}


void SPhysics::Update()
{
    this->time += GetFrameTime();
    this->physicTime = GetTime();

    int steps = 0;
    while (this->time >= this->physicSlice)
    {
        dSpaceCollide(space, (void*)this, &OnCollision);
        dWorldQuickStep(world, this->physicSlice);
        dJointGroupEmpty(this->contactGroup);

        this->time -= this->physicSlice;

        steps++;
        if (steps > this->maxSteps)
        {
            this->time = 0.f;
            break;
        }
    }

    this->physicTime = GetTime() - this->physicTime;
}


dGeomID SPhysics::CreateBoxObject(Vector3 position, Vector3 size, int density)
{
    dBodyID body = dBodyCreate(world);
    dGeomID geom = dCreateBox(space, size.x, size.y, size.z);

    dMass mass;
    dMassSetBox(&mass, density, size.x, size.y, size.z);

    dBodySetPosition(body, position.x, position.y, position.z);
    dGeomSetBody(geom, body);
    dBodySetMass(body, &mass);

    SPhysicsEntity entity = { .collision = true };
    dGeomSetData(geom, &entity);

    return geom;
}

void SPhysics::RemoveObject(dGeomID id)
{
    dGeomDestroy(id);
}

Vector3* SPhysics::GetObjectData(dGeomID id)
{
    const dReal* position = dGeomGetPosition(id);
    const dReal* angle = dGeomGetRotation(id);

    Vector3 res[2] = { {
        (float)position[0],
        (float)position[1],
        (float)position[2]

        },
        {
        (float)angle[0],
        (float)angle[1],
        (float)angle[2]
        }
    };

    return res;
}

void SPhysics::UpdateObjectData(dGeomID id, Vector3 velocity, Vector3 rotation)
{
    dBodyID body = dGeomGetBody(id);

    const dReal* vel = dBodyGetLinearVel(body);
    dBodySetLinearVel(body, vel[0] + velocity.x  * GetFrameTime(), vel[1] + velocity.y * GetFrameTime(), vel[2] + velocity.z * GetFrameTime());

    dMatrix3 R;
    dRFromEulerAngles(R, rotation.x, rotation.y, rotation.z);
    dBodySetRotation(body, R);
}



static void OnCollision(void* data, dGeomID obj1, dGeomID obj2)
{
    if (!data)
    {
        return;
    }

    SPhysics* self = (SPhysics*)data;
    if (!self)
    {
        return;
    }

    dBodyID b1 = dGeomGetBody(obj1);
    dBodyID b2 = dGeomGetBody(obj2);
    
    if (b1 && b2 && dAreConnectedExcluding(b1, b2, dJointTypeContact))
    {
        return;
    }

    
    SPhysicsEntity* ent1 = (SPhysicsEntity*)dGeomGetData(obj1);
    SPhysicsEntity* ent2 = (SPhysicsEntity*)dGeomGetData(obj2);
    if (ent1 && ent2 && (!ent1->collision || !ent2->collision))
    {
        return;
    }
    
    dContact contacts[MAX_CONTACTS];
    int contactPoints = dCollide(obj1, obj2, MAX_CONTACTS, &contacts[0].geom, sizeof(dContact));
    if (!contactPoints)
    {
        return;
    }

    for (int i = 0; i < contactPoints; i++) {
        
        contacts[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
        contacts[i].surface.mu = 1000;
        contacts[i].surface.slip1 = 0.0001;
        contacts[i].surface.slip2 = 0.001;
        contacts[i].surface.soft_erp = 0.5;
        contacts[i].surface.soft_cfm = 0.0003;
        contacts[i].surface.bounce = 0.1;
        contacts[i].surface.bounce_vel = 0.1;

        dJointID contactj = dJointCreateContact(self->world, self->contactGroup, contacts + i);
        dJointAttach(contactj, b1, b2);
    }
}