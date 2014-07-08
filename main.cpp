#include <windows.h>
#include "resource.h"

const char *ClassName = "DemoIcon";

// Declaration of the window procedure, to be used in the WNDCLASSEX struct:
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd) {

    WNDCLASSEX wc;
    HWND hWnd;
    MSG msg;

    // Filling the structure:
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    // Remember this just loads 32x32, use LoadImage() instead for other dimensions (16x16, 48x48, ...):
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(AppIcon));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = ClassName;
    // Here we'll use LoadImage, as we need a 16x16 mini icon:
    wc.hIconSm = static_cast<HICON>(LoadImage(hInstance,MAKEINTRESOURCE(AppIcon),IMAGE_ICON,16,16, LR_DEFAULTCOLOR));

    // Registering the class:
    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL,
                   "Could not register window.",
                   "Error",
                   MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    // Create the window using the "MainWindow" class:
    hWnd = CreateWindowEx(WS_EX_WINDOWEDGE,
                          ClassName,
                          "Demo Icon",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          200,
                          150,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    // If the window was not created show error and exit:
    if(hWnd == NULL) {
        MessageBox(NULL,
                   "Could not create window.",
                   "Error",
                   MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    // Set the windows show state, to show it:
    ShowWindow(hWnd, nShowCmd);
    // Draw the window:
    UpdateWindow(hWnd);

    // Retrieve messages from the message queue:
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// Implementation of the window procedure, will handle the messages:
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch(uMsg) {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}
