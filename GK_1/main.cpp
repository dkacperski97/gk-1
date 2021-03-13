#ifndef UNICODE
#define UNICODE
#endif 

#include <utility>
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>

struct VERTEX {
    double x, y, z;
};
struct EDGE : std::pair<VERTEX*, VERTEX*> {};
struct OBJECT {
    std::vector<VERTEX> vertices;
    std::vector<EDGE> edges;
};
struct SCENE {
    std::vector<OBJECT> objects;
};

SCENE scene;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;
    POINT aptStar[6] = { 50,2, 2,98, 98,33, 2,33, 98,98, 50,2 };

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        LONG width = ps.rcPaint.right, height = ps.rcPaint.bottom;
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        //GetClientRect(hwnd, &rc);
        //SetMapMode(hdc, MM_ANISOTROPIC);
        //SetWindowExtEx(hdc, rc.right, rc.bottom, NULL);
        //SetViewportExtEx(hdc, rc.right, rc.bottom, NULL);
        Polyline(hdc, aptStar, 6);
        MoveToEx(hdc, 0, 20, NULL);
        LineTo(hdc, width / 2, 20);
        MoveToEx(hdc, width / 2, 40, NULL);
        LineTo(hdc, width, 40);
        EndPaint(hwnd, &ps);
    }
    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

SCENE loadScene(std::string path) {
    SCENE s;
    std::ifstream ifs(path, std::ifstream::in);
    if (ifs.is_open()) {
        int vertices_number, edges_number;
        ifs >> vertices_number >> edges_number;
        while (!ifs.eof()) {
            OBJECT object;
            for (int n = 0; n < vertices_number; n++) {
                VERTEX vertex;
                ifs >> vertex.x >> vertex.y >> vertex.z;
                object.vertices.push_back(vertex);
            }
            for (int n = 0; n < edges_number; n++) {
                EDGE edge;
                int a, b;
                ifs >> a >> b;
                edge.first = &object.vertices[a];
                edge.second = &object.vertices[b];
                object.edges.push_back(edge);
            }
            std::string emptyline;
            std::getline(ifs, emptyline);
            if (emptyline != "") {
                throw std::exception("Expected empty line.");
            }
            s.objects.push_back(object);
            ifs >> vertices_number >> edges_number;
        }
    }
    return s;
}

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    scene = loadScene("input.txt");

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Main GK Window";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClass(&wc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
