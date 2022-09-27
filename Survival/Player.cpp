#include "Player.h"

Player::Player(SCamera* worldCam) : SEntity()
{
	this->camera = worldCam;
}

void Player::UpdateFrame()
{
    velocity = Vector3Multiply(velocity, { 0.9f,0.9f ,0.9f });
    float speed = 2.0f;
    if (IsKeyDown(32)) // space
    {
        speed *= 3;
    }


    if (IsKeyDown(87)) // front/back
    {
        velocity.x += speed;
        // velocity.y -= this->camera.GetRotation().y * speed;
    }
    else if (IsKeyDown(83))
    {
        velocity.x -= speed;
    }

    if (IsKeyDown(68)) // right/left
    {
        velocity.z += speed;

    }
    else if (IsKeyDown(65))
    {
        velocity.z -= speed;
    }

    if (IsKeyDown(82)) // up/down
    {
        velocity.y += speed;
    }
    else if (IsKeyDown(70))
    {
        velocity.y -= speed;
    }

    if (IsKeyDown(KEY_ONE) && this->camMode == SCameraMode::FirstPerson)
    {
        this->camMode = SCameraMode::ThirdPerson;
        this->camera->targetDistance = 10.f;
        this->camera->SwitchUpdate(this->camMode);
        velocity = { 0 };
    }
    else if ((IsKeyDown(KEY_TWO) || this->camera->targetDistance <= (this->camera->targetDistMinMax.x + 1.f)) && this->camMode == SCameraMode::ThirdPerson)
    {
        this->camMode = SCameraMode::FirstPerson;
        this->camera->SwitchUpdate(this->camMode);
        velocity = { 0 };
    }

    if (this->camMode == SCameraMode::ThirdPerson)
    {
        camera->UpdateThird(velocity);
        
        BeginMode3D(this->camera->GetCamera());
        
        this->skin.transform = MatrixRotateY(this->camera->rotation.x);
        DrawModel(this->skin,this->camera->GetCamera().target, 1, RED);

        EndMode3D();
    }
    else {
        camera->UpdateFirst(velocity);


        bool targetMode = false;
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            this->AnimateVector3({ 0.323f,0.158, -0.001 }, &this->playerhCam.position, 1 * GetFrameTime(), 0.01f);
            this->AnimateVector3({ -0.670,0.040,0.007 }, &this->playerhCam.target, 1 * GetFrameTime(), 0.01f);

            targetMode = true;
        }
        else {
            this->AnimateVector3({ 0.318f,0.183,0.134 }, &this->playerhCam.position, 1 * GetFrameTime(), 0.01f);
            this->AnimateVector3({ -0.681,0.167,0.140 }, &this->playerhCam.target, 1 * GetFrameTime(), 0.01f);
            this->playerhCam.up.z = 0;
        }


        bool targetRotation = false;
        if (IsKeyDown(262) && targetMode)
        {
            targetRotation = true;
            this->playerhCam.up.z -= GetFrameTime();
            if (this->playerhCam.up.z < -1)
            {
                this->playerhCam.up.z = -1;
            }
        }
        else if (IsKeyDown(263) && targetMode)
        {
            targetRotation = true;
            this->playerhCam.up.z += GetFrameTime();
            if (this->playerhCam.up.z > 1)
            {
                this->playerhCam.up.z = 1;
            }
        }
        else {
            this->playerhCam.up.z = 0;
        }


        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) // shoot
        {
            walkGunMode += 40.f * GetFrameTime();
        }
        else if (targetRotation)
        {
            walkGunMode += GetFrameTime();
        }
        else {
            walkGunMode += 2.f * GetFrameTime();
        }

        this->playerhCam.up.x = sin(walkGunMode);

        
        BeginTextureMode(playerHands);
        ClearBackground(BLANK);

        BeginMode3D(playerhCam);
        DrawModel(gun, { 0 }, 1.f, WHITE);
        EndMode3D();

        EndTextureMode();

        DrawTexturePro(playerHands.texture, { 0, 0, (float)GetRenderWidth(), (float)-GetRenderHeight() }, { 0, 0, (float)GetRenderWidth(), (float)GetRenderHeight() }, { 0,0 }, 0, WHITE);
        DrawCircle(GetRenderWidth() / 2, GetRenderHeight() / 2, 3, RED);
    }
}

void Player::Load()
{
    this->playerHands = LoadRenderTexture(GetRenderWidth(), GetRenderHeight());
    this->playerhCam = {
        {0.318f,0.183,0.134},
        {-0.681,0.167,0.140},
        {0,1,0},
        90.0f,
        CAMERA_PERSPECTIVE
    };

    gun = LoadModel("./assets/models/gun/model.obj");
    skin = LoadModelFromMesh(GenMeshCube(2, 2, 2));
}

void Player::UnLoad()
{
    UnloadModel(gun);
}

Vector3 Player::GetVelocity()
{
    return this->velocity;
}

Camera3D Player::GetGunCamera() {
    return this->playerhCam;
}

void Player::AnimateVector3(Vector3 target, Vector3* val, float speed, float margin)
{
    
    if (!(val->x + margin >= target.x && val->x <= target.x + margin))
    {
        if (val->x > target.x)
        {
            val->x -= speed;
        }
        else {
            val->x += speed;
        }
    }
    else {
        val->x = target.x;
    }

    if (!(val->y + margin >= target.y && val->y <= target.y + margin))
    {
        if (val->y > target.y)
        {
            val->y -= speed;
        }
        else {
            val->y += speed;
        }
    }
    else {
        val->y = target.y;
    }

    if (!(val->z + margin >= target.z && val->z <= target.z + margin))
    {
        if (val->z > target.z)
        {
            val->z -= speed;
        }
        else {
            val->z += speed;
        }
    }
    else {
        val->z = target.z;
    }
   
}