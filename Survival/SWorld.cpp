#include "SWorld.h"


SWorld::SWorld() : SEntity()
{
	hud = { 0 };
	hud.zoom = 0.5f;

    player = new Player(&this->camera, &this->physics);
}

void SWorld::UpdateFrame()
{
    BeginMode3D(camera.GetCamera());
    rlDisableBackfaceCulling();
    rlDisableDepthMask();

    DrawModel(sky, { 0, 0, 0 }, 1.f, WHITE);

    rlEnableBackfaceCulling();
    rlEnableDepthMask();

    DrawModel(floor, this->floorPosition, 1.f, GREEN);
    //DrawModelWires(floor, { -100, 0, -100 }, 1.f, RED);

#ifdef _DEBUG
    DrawGrid(200, 1.f);
#endif // _DEBUG

    EndMode3D();
    
    this->physics.Update();
    this->player->UpdateFrame();


    BeginMode2D(hud);
    Camera3D cam = this->camera.GetCamera();
    Vector3 vel = this->player->GetVelocity();
    Camera3D playerhCam = this->player->GetGunCamera();

    std::string debug = "FPS: " + std::to_string(GetFPS()) +
        "\n\nPosition" + Vector3String(cam.position) +
        "\n\nUp" + Vector3String(cam.up) +
        "\n\nTarget" + Vector3String(cam.target) +
        "\n\Rotation" + Vector3String(this->camera.rotation) +
        "\n\nVelocity" + Vector3String(vel) +
        "\n\nTPS Distance: "+std::to_string(this->camera.targetDistance) +
        "\nInterations per step: " +std::to_string(this->physics.GetInterationsPerStep());

    static Vector2  rectSize = MeasureTextEx(GetFontDefault(), debug.c_str(), 20, 0);

    DrawRectangle(10, 10, rectSize.x + 200, rectSize.y + 100, { 200,200,200,255 });
    DrawRectangleLinesEx({ 10, 10, rectSize.x + 200, rectSize.y + 100 }, 2.f, BLACK);

    DrawText(debug.c_str(), 30, 60, 20, BLACK);

    EndMode2D();
}

void SWorld::Load()
{
    this->camera.Init({ 0, 1, 0 }, { 0 }, 90.f);
    this->physics.Init({ 0, -9.8, 0 }, -1);

    Image m = LoadImage("./assets/textures/heightmap.png");
    floor = LoadModelFromMesh(GenMeshHeightmap(m, { 200, 15, 200 }));
    floor.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("./assets/textures/grass.png");//LoadTextureFromImage(m);
    UnloadImage(m);
    
    this->floorPosition = { -100, 0, -100 };
    this->physics.SetTerrain(floor.meshes[0].vertexCount, floor.meshes[0].vertices);
    this->physics.SetTerrainPosition(this->floorPosition);

    sky = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
    sky.materials[0].shader = LoadShader("./assets/shaders/sky.vs", "./assets/shaders/sky.fs");

    int envMap[1] = {MATERIAL_MAP_CUBEMAP};
    SetShaderValue(sky.materials[0].shader, GetShaderLocation(sky.materials[0].shader, "environmentMap"), (void*)envMap, SHADER_UNIFORM_INT);

    Image img = LoadImage("./assets/textures/sky2.png");
    sky.materials[0].maps[MATERIAL_MAP_CUBEMAP].texture = LoadTextureCubemap(img, CUBEMAP_LAYOUT_AUTO_DETECT);    // CUBEMAP_LAYOUT_PANORAMA
    UnloadImage(img);

    this->player->Load();
}

void SWorld::UnLoad()
{
    this->player->UnLoad();

    UnloadModel(this->sky);
    UnloadModel(this->floor);
}

Vector2 SWorld::Per2Pix(Vector2 percent)
{
   
	return { (float)GetRenderWidth() * percent.x, (float)GetRenderHeight() * percent.y };
}

Vector2 SWorld::Pix2Per(Vector2 pixel)
{
	return { pixel.x / (float)GetRenderWidth(), pixel.y / (float)GetRenderHeight()};
}
