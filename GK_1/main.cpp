#ifndef UNICODE
#define UNICODE
#endif 

#include <utility>
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>

struct VERTEX {
    double values[4];
    VERTEX(double x, double y, double z, double w = 1.0) {
        values[0] = x;
        values[1] = y;
        values[2] = z;
        values[3] = w;
    }

    VERTEX perspective_projection(double z_min)
    {
        //if (values[2] < z_min) {
        //    throw std::exception("Vertex is behind the camera."); // TODO
        //}
        return VERTEX(
            values[0] / values[2],
            values[1] / values[2],
            (values[2] - z_min) / (values[2] * (1 - z_min))
        ); 
    }
};
struct EDGE : std::pair<int, int> {}; // TODO: iterator/pointer?
struct OBJECT {
    std::vector<VERTEX> vertices;
    std::vector<EDGE> edges;
};
struct SCENE {
    std::vector<OBJECT> objects;

    void move_x(double x) {
        for (int i = 0; i < objects.size(); i++) {
            for (int j = 0; j < objects[i].vertices.size(); j++) {
                objects[i].vertices[j].values[0] += x;
            }
        }
    }
    void move_y(double y) {
        for (int i = 0; i < objects.size(); i++) {
            for (int j = 0; j < objects[i].vertices.size(); j++) {
                objects[i].vertices[j].values[1] += y;
            }
        }
    }
    void move_z(double z) {
        for (int i = 0; i < objects.size(); i++) {
            for (int j = 0; j < objects[i].vertices.size(); j++) {
                objects[i].vertices[j].values[2] += z;
            }
        }
    }
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
    case WM_KEYDOWN:
        //swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", wParam);
        //OutputDebugString(msg);

        if (wParam == VK_LEFT) {
            scene.move_x(0.1);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        else if (wParam == VK_RIGHT) {
            scene.move_x(-0.1);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        else if (wParam == VK_PRIOR) {
            scene.move_y(0.1);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        else if (wParam == VK_NEXT) {
            scene.move_y(-0.1);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        else if (wParam == VK_UP) {
            scene.move_z(-0.1);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        else if (wParam == VK_DOWN) {
            scene.move_z(0.1);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        }
        break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        LONG width = ps.rcPaint.right, height = ps.rcPaint.bottom;
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        //GetClientRect(hwnd, &rc);
        //SetMapMode(hdc, MM_ANISOTROPIC);
        //SetWindowExtEx(hdc, rc.right, rc.bottom, NULL);
        //SetViewportExtEx(hdc, rc.right, rc.bottom, NULL);

        double z_min = 0.1;
        std::vector<std::pair<int, int>>vv; // TODO: remove debug info
        for (auto object : scene.objects) {
            for (auto edge : object.edges) {
                VERTEX first = object.vertices[edge.first].perspective_projection(z_min);
                VERTEX second = object.vertices[edge.second].perspective_projection(z_min);

                int x, y;
                x = (width / 2) * (first.values[0] + 1); // TODO: aspect ratio
                y = (height / 2) + ((width / 2) * first.values[1]);
                MoveToEx(hdc, x, y, NULL);
                vv.push_back(std::pair<int, int>(x, y));
                x = (width / 2) * (second.values[0] + 1);
                y = (height / 2) + ((width / 2) * second.values[1]);
                LineTo(hdc, x, y);
                vv.push_back(std::pair<int, int>(x, y));
                x = vv[0].first;
            }
        }


        //Polyline(hdc, aptStar, 6);
        //MoveToEx(hdc, 0, 20, NULL);
        //LineTo(hdc, width / 2, 20);
        //MoveToEx(hdc, width / 2, 40, NULL);
        //LineTo(hdc, width, 40);
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
                double x, y, z;
                ifs >> x >> y >> z;
                object.vertices.push_back(VERTEX(x, y, z));
            }
            for (int n = 0; n < edges_number; n++) {
                EDGE edge;
                int a, b;
                ifs >> a >> b; 
                edge.first = a;
                edge.second = b;
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
    scene = loadScene("3.txt");

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
        L"GK",                          // Window text
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
