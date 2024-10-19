#define UNICODE
#define _UNICODE
#include "Window.h"
#include <iostream>
#include <tchar.h>
#include <stdbool.h>
#include <stdint.h>
#include <gdiplus.h>
#include <windowsx.h>
//#include "Gdi32.lib"
#pragma comment (lib,"Gdiplus.lib")

static BITMAPINFO frame_bitmap_info;
static HBITMAP frame_bitmap = 0;
static HBITMAP frame_bitmap_zBuffer = 0;
static HDC frame_device_context = 0;
static HDC frame_device_context_zBuffer = 0;

struct {
    int width;
    int height;
    uint32_t *pixels;
    uint32_t *zBuffer;
} frame = {0};


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    char buffer [10];


    // Create a solid color brush
    
    switch(uMsg){
        case WM_CREATE:
            
            break;
        case WM_SIZE:
		
            frame_bitmap_info.bmiHeader.biWidth  = LOWORD(lParam);
            frame_bitmap_info.bmiHeader.biHeight = HIWORD(lParam);

            if(frame_bitmap) DeleteObject(frame_bitmap);
            frame_bitmap = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.pixels, 0, 0);
            SelectObject(frame_device_context, frame_bitmap);
            frame_bitmap_zBuffer = CreateDIBSection(NULL, &frame_bitmap_info, DIB_RGB_COLORS, (void**)&frame.zBuffer, 0, 0);
            SelectObject(frame_device_context_zBuffer, frame_bitmap_zBuffer);

            frame.width =  LOWORD(lParam);
            frame.height = HIWORD(lParam);
               
			// Event fired when the window is resized
			
            break;

        case WM_ERASEBKGND:
            std::cout << "ERASING BKGND MAYBE??";
            break;

        
        case WM_PAINT:
            
             static PAINTSTRUCT paint;
            static HDC device_context;
            device_context = BeginPaint(hWnd, &paint);
            BitBlt(device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   paint.rcPaint.right - paint.rcPaint.left, paint.rcPaint.bottom - paint.rcPaint.top,
                   frame_device_context,
                   paint.rcPaint.left, paint.rcPaint.top,
                   SRCCOPY);

            EndPaint(hWnd, &paint);

            //graphics.FillPolygon(&solidBrush, tri1, 3);
            // Create a GDI+ Graphics object
            

            // Use the solid brush to draw a rectangle
            //graphics.FillRectangle(&solidBrush, 0, 0, desktop.right, desktop.bottom);

            // Don't forget to release resources
            
            break;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}




Window::Window() : m_hInstance(GetModuleHandle(nullptr)){ //m_hInstance is a macro
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != 0) {
    // Handle error
       std::cout << "GDIPLUS ERROR"; 
    }
    const wchar_t* CLASS_NAME = L"MY NAME";

    

    WNDCLASSEXW wndClass = {};
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpszClassName = CLASS_NAME;
    wndClass.hInstance = m_hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW); // What type of cursor?
    wndClass.lpfnWndProc = WindowProc;  
    //wndClass.hbrBackground = (HBRUSH) (RGB(255, 0, 0));

    RegisterClassExW(&wndClass);

    frame_bitmap_info.bmiHeader.biSize = sizeof(frame_bitmap_info.bmiHeader);
    frame_bitmap_info.bmiHeader.biPlanes = 1;
    frame_bitmap_info.bmiHeader.biBitCount = 32;
    frame_bitmap_info.bmiHeader.biCompression = BI_RGB;
    frame_device_context = CreateCompatibleDC(0);

    DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_HSCROLL | WS_VSCROLL;
    
    int width = 640;
    int height = 480;

    RECT rect;
    rect.left = 250;
    rect.top = 250;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;
    AdjustWindowRect(&rect, style, false); 

    m_hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Title",
        style,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL,
        NULL, //FOR MENUS
        m_hInstance,
        NULL
    );
    
	SetWindowLongPtr((HWND)m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow((HWND)m_hWnd);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    if (!m_hWnd){
        std::cout << "Failed Start\n";
        throw std::runtime_error("CreateWindowEx failed");
    } 
}

Window::~Window(){
    const wchar_t* CLASS_NAME = L"THOMAS WINDOW CLASS";

    UnregisterClassW(CLASS_NAME, m_hInstance);

}

bool Window::ProcessMessages(){
    MSG msg = {};

    while(PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)){
        if(msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg); 
    }

    InvalidateRect(m_hWnd, NULL, FALSE);
    UpdateWindow(m_hWnd);

    return true;
}

HWND Window::gethWnd(){
    return m_hWnd;
}

/*void Window::drawObject(Gdiplus::Point *points, int size, Gdiplus::Color coloring){
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != 0) {
    // Handle error
       std::cout << "GDIPLUS ERROR"; 
    }
    UpdateWindow((HWND)m_hWnd);
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    Gdiplus::Graphics graphics(hdc);
    Gdiplus::Color color1 = coloring;
    Gdiplus::Color color2(RGB(50,50,50));
    Gdiplus::SolidBrush solidBrush(color1);
    Gdiplus::Pen pen(color2);
    std::cout << "Painting shapes";

    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    graphics.FillRectangle(&solidBrush, 0,0,100, 100);
    for(int i = 0; i < size; i++){
        graphics.FillPolygon(&solidBrush, points, 3);
        graphics.DrawPolygon(&pen, points, 3);
        points+=3;
    }
    EndPaint(m_hWnd, &ps);
    Gdiplus::GdiplusShutdown(gdiplusToken);
}*/