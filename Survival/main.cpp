#include "SWindow.h"

#ifdef _DEBUG
int main(int argc, const char* argv)
{
    SWindow* swin = new SWindow();
    return swin->Start(nullptr);
}
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int nCmdShow)
{
    SWindow* swin = new SWindow();
    return swin->Start(pCmdLine);
}
#endif // _DEBUG


