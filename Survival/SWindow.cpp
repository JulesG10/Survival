#include "SWindow.h"

SWindow::SWindow()
{
}

int SWindow::Start(char* cmdLine)
{
    SYSTEM_POWER_STATUS status;
    if (GetSystemPowerStatus(&status) && status.ACLineStatus == AC_LINE_ONLINE)
    {
        SetConfigFlags(FLAG_FULLSCREEN_MODE); // FLAG_MSAA_4X_HINT

        // shaders
        // l’Anti-Aliasing et anticrénelage
        // https://www.malekal.com/quest-ce-que-anti-aliasing-et-anticrenelage-fxaa-smaa-msaa-ssaa-txaa/
        // MSAA/SSAA	Les plus beaux rendus mais les plus gourmands en ressources
        // TXAA	Se place entre les deux en terme de coût de performances
        // FXAA/SMAA/TAA Bon rendu sans trop impacter les performances
    }
    else {
        SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    }

#ifndef _DEBUG
    SetTraceLogLevel(LOG_NONE);
#endif // _DEBUG

    InitWindow(0, 0, "");

    DisableCursor();
    SetTargetFPS(0);
    
    /*
    pthread_t threadId;
    int error = pthread_create(&threadId, NULL, &WindowLoadThread, this);
    if (error != 0)
    {
        return error;
    }
    */

    while (!WindowShouldClose())
    {
        if (loading)
        {
            BeginDrawing();
            ClearBackground(RAYWHITE);
            Vector2 loadt = MeasureTextEx(GetFontDefault(), "Loading...", 30, 2);
            DrawText("Loading ...", (GetRenderWidth() - loadt.x) / 2, (GetRenderHeight() - loadt.y) / 2, 30, BLACK);
            EndDrawing();

            WindowLoadThread((void*)this);
        }
        else 
        {
            this->UpdateFrame();
        }
    }
    
    this->UnLoad();
    CloseWindow();

    return 0;
}

void SWindow::UpdateFrame()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    this->world.UpdateFrame();
    EndDrawing();
}


void SWindow::Load()
{
    this->world.Load();
    this->loading = false;
}

void SWindow::UnLoad()
{
    this->world.UnLoad();
}

static void* WindowLoadThread(void* data)
{
    if (!data)
    {
        return nullptr;
    }

    SWindow* self = (SWindow*)data;
    if (!self)
    {
        return nullptr;
    }

    self->Load();

    return nullptr;
}
