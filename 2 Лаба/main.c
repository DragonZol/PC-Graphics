#include <windows.h>
#include <gl/gl.h>
#include<stdio.h>
#include<stdlib.h>

#include"stb-master\stb-master\stb_easy_font.h"

#define nameLen 20

typedef struct {
    char name[nameLen];
    float vert[8];
    char isHower, isDown;
    //text
    float buffer[50 * nameLen];
    int num_quads;
    float textPosX, textPosY, textScale;
} TBtn;
TBtn *btn=0;
int btnCnt=0;


float mouseX, mouseY;

int Menu_AddButton(char *name, float x, float y, float width, float height, float textScale)
{
    btnCnt++;
    btn=realloc(btn, sizeof(btn[0])*btnCnt);

    snprintf(btn[btnCnt-1].name, nameLen, "%s", name);
    float *vert = btn[btnCnt-1].vert;
    vert[0]=vert[6]=x;
    vert[2]=vert[4]=x +width;
    vert[1]=vert[3]=y;
    vert[5]=vert[7]=y+height;
    btn[btnCnt-1].isHower=0;
    btn[btnCnt-1].isDown=0;

    TBtn *b= btn + btnCnt -1;
    b-> num_quads = stb_easy_font_print(0,0,name,0,b->buffer, sizeof(b->buffer));
    b-> textPosX=x+(width - stb_easy_font_width(name) *textScale)/2.0;
    b-> textPosY=y+(height - stb_easy_font_height(name) *textScale);
    b->textScale=textScale;


    return btnCnt - 1;
}

void ShowButton(int buttonId)
{
    TBtn btn1 = btn[buttonId];
    glVertexPointer(2, GL_FLOAT,0,btn1.vert);
    glEnableClientState(GL_VERTEX_ARRAY);
        if (btn1.isDown) glColor3f(0.2,1,0.2);
        else if(btn1.isHower) glColor3f(1,0.4, 0.3);
        else glColor3f(0.8, 0.3, 0.2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glColor3f(1,1,1);
        glLineWidth(1);
        glDrawArrays(GL_LINE_LOOP, 0,4);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPushMatrix();
        glColor3f(0,0,0);
        glTranslatef(btn1.textPosX, btn1.textPosY,0);
        glScalef(btn1.textScale, btn1.textScale, 1);
        glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2, GL_FLOAT, 16, btn1.buffer);
            glDrawArrays(GL_QUADS, 0, btn1.num_quads*4); // отрисовка примитива
        glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();


}

void Menu_ShowMenu()
{
    for (int i=0; i<btnCnt; i++)
        ShowButton(i);
}

char isCoordInButton(int buttonId, float x, float y)
{
    float *vert=btn[buttonId].vert;
    return (x>vert[0])&&(y>vert[1]) &&(x<vert[4]) && (y<vert[5]);
}

int Menu_MouseMove(float x, float y)
{
    mouseX=x;
    mouseY=y;
    int moveBtn=-1;
    for (int i=0; i<btnCnt; i++)
        if (isCoordInButton(i, mouseX,mouseY))
        {
            btn[i].isHower=1;
            moveBtn=i;
        }
        else
        {
            btn[i].isHower=0;
            btn[i].isDown=0;
        }
    return moveBtn;


}

int Menu_MouseDown()
{
    int downBtn =-1;
    for (int i=0; i<btnCnt; i++)
        if (isCoordInButton(i, mouseX,mouseY))
        {
            btn[i].isDown=1;
            downBtn=i;
        }
    return downBtn;
}

void Menu_MouseUp()
{
    for (int i=0; i<btnCnt; i++)
        btn[i].isDown =0;
}

char *Menu_GetButtonName(int buttonId)
{
    return btn[buttonId].name;
}

void MouseDown()
{
    int buttonId=Menu_MouseDown();
    if (buttonId<0) return;
    char *name = Menu_GetButtonName(buttonId);
    printf("%s\n", name);
}




LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void Int()
{
    Menu_AddButton("Hills", 10, 10, 100, 30,2);
    Menu_AddButton("Coast", 120, 10, 100, 30,2);
    Menu_AddButton("Beach", 230, 10, 100, 30,2);
    Menu_AddButton("Ocean", 340, 10, 100, 30,2);
}




int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          900,
                          1024,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    RECT rct;
    GetClientRect(hwnd, &rct);
    glOrtho(0, rct.right, rct.bottom,0,1,-1);

    Int();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.7f, 0.7f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();

            Menu_ShowMenu(0);


            glPopMatrix();

            SwapBuffers(hDC);

            theta += 1.0f;
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_MOUSEMOVE:
            Menu_MouseMove(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_LBUTTONDOWN:
            MouseDown();
        break;

        case WM_LBUTTONUP:
            Menu_MouseUp();
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

