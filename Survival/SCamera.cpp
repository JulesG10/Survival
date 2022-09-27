#include "SCamera.h"


SCamera::SCamera()
{
}

void SCamera::Init(Vector3 position, Vector3 target, float fov)
{
    this->camera = { 0 };
    camera.position = position;
    camera.target = target;
    camera.up = { 0.f, 1.f, 0.f };
    camera.fovy = fov;
    camera.projection = CAMERA_PERSPECTIVE;

    targetDistance = 10.f;
}

void SCamera::UpdateFirst(Vector3 velocity)
{
    Vector2 delta = GetMouseDelta();
    this->rotation.x += delta.x * (sensitivity * smoothness);
    this->rotation.y += delta.y * (sensitivity * smoothness);

    if (this->rotation.y > FPSdegMinMax.x * DEG2RAD) 
    {
        this->rotation.y = FPSdegMinMax.x * DEG2RAD;
    }
    else if (this->rotation.y < FPSdegMinMax.y * DEG2RAD)
    {
        this->rotation.y = FPSdegMinMax.y * DEG2RAD;
    }

    camera.position.x -= ((velocity.x * sinf(-this->rotation.x) - velocity.z * cosf(-this->rotation.x)) * GetFrameTime());
    camera.position.y += (velocity.y * GetFrameTime());
    camera.position.z -= ((velocity.x * cosf(-this->rotation.x) + velocity.z * sinf(-this->rotation.x)) * GetFrameTime());

    Matrix translation = MatrixTranslate(0, 0, 1);
    Matrix rotation =  MatrixRotateZYX({ PI * 2 - this->rotation.y, PI * 2 - this->rotation.x, 0 });
    Matrix transform = MatrixMultiply(translation, rotation);
    
    camera.target.x = camera.position.x - transform.m12;
    camera.target.y = camera.position.y - transform.m13;
    camera.target.z = camera.position.z - transform.m14;
}

void SCamera::UpdateThird(Vector3 velocity)
{
    Vector2 delta = GetMouseDelta();
    this->rotation.x += delta.x * -(sensitivity * smoothness);
    this->rotation.y += delta.y * -(sensitivity * smoothness);

    if (this->rotation.y > TPSdegMinMax.x * DEG2RAD)
    {
        this->rotation.y = TPSdegMinMax.x * DEG2RAD;
    }
    else if (this->rotation.y < TPSdegMinMax.y * DEG2RAD)
    {
        this->rotation.y = TPSdegMinMax.y * DEG2RAD;
    }

 
    this->targetDistance -= (GetMouseWheelMove() * this->sensitivity);

    
    if (this->targetDistance < targetDistMinMax.x) {
        this->targetDistance = targetDistMinMax.x;
    }
    else  if (this->targetDistance > targetDistMinMax.y) {
        this->targetDistance = targetDistMinMax.y;
    }

    camera.target.x -= ((velocity.x * sinf(this->rotation.x) - velocity.z * cosf(this->rotation.x)) * GetFrameTime());
    camera.target.y += (velocity.y * GetFrameTime());
    camera.target.z -= ((velocity.x * cosf(this->rotation.x) + velocity.z * sinf(this->rotation.x)) * GetFrameTime());

    this->camera.position.x = sinf(this->rotation.x) * this->targetDistance * cosf(this->rotation.y) + this->camera.target.x;

    if (this->rotation.y <= 0.0f) {
        this->camera.position.y = sinf(this->rotation.y) * this->targetDistance * sinf(this->rotation.y) + this->camera.target.y;
    }
    else {
        this->camera.position.y = -sinf(this->rotation.y) * this->targetDistance * sinf(this->rotation.y) + this->camera.target.y;
    }

    this->camera.position.z = cosf(this->rotation.x) * this->targetDistance * cosf(this->rotation.y) + this->camera.target.z;
}

void SCamera::SwitchUpdate(SCameraMode mode)
{
    if (mode == SCameraMode::ThirdPerson)
    {
        this->camera.target = this->camera.position;
        this->rotation.y *= TPSdegMinMax.y / FPSdegMinMax.x;

        /*
        if (this->targetDistance == 1.f)
        {
            Vector3 v1 = camera.position;
            Vector3 v2 = camera.target;

            float dx = v2.x - v1.x;
            float dy = v2.y - v1.y;
            float dz = v2.z - v1.z;

            this->targetDistance = sqrtf(dx * dx + dy * dy + dz * dz);
        }
        */
    }
    else 
    {
        camera.position = camera.target;   
        float horizon = 1.0f; // + (FPSdegMinMax.x + FPSdegMinMax.y) = 0
        this->rotation.y *= horizon / TPSdegMinMax.x;
    }
}

Camera3D SCamera::GetCamera()
{
    return this->camera;
}

void SCamera::SetPosition(Vector3 pos)
{
    this->camera.position = pos;
}

void SCamera::SetTarget(Vector3 pos)
{
    this->camera.target = pos;
}
