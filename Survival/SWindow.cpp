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
    }
    else {
        SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    }

    SetTraceLogLevel(LOG_NONE);
    InitWindow(0, 0, "");

    DisableCursor();
    SetTargetFPS(0);

    this->Load();

    while (!WindowShouldClose())
    {
        this->UpdateFrame();
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
}

void SWindow::UnLoad()
{
    this->world.UnLoad();
}
