#include <windows.h>
#include <gl/gl.h>
#include <math.h>


//___________________CAMERA(P)_____________________________________________
struct SCamera {
    float x,y,z;
    float Xrot,Zrot;
};

void Camera_Apply();
void Camera_Rotation(float xAngle, float zAngle);
void Camera_AutoMoveByMouse(int centerX, int centerY, float speed);
void Camera_MoveDirectional(int forwardMove, int rightMove, float speed);

//______________________________________________________________________


//___________________CAMERA_____________________________________________
struct SCamera camera = {0,0,2.5, 285,0};

void Camera_Apply(){
    glRotatef(-camera.Xrot,1,0,0);
    glRotatef(-camera.Zrot,0,0,1);
    glTranslatef(-camera.x, -camera.y, -camera.z);
}

void Camera_Rotation(float xAngle, float zAngle){
    camera.Zrot+=zAngle;
    if( camera.Zrot<0) camera.Zrot +=360;
    if( camera.Zrot>360) camera.Zrot -=360;
    camera.Xrot+=xAngle;
    if( camera.Xrot<0) camera.Xrot =0;
    if( camera.Xrot<180) camera.Xrot =180;
}

void Camera_AutoMoveByMouse(int centerX, int centerY, float speed){
    POINT cur;
    POINT base = {centerX,centerY};
    GetCursorPos(&cur);
    Camera_Rotation( (base.y - cur.y)/5,(base.x - cur.x)/5);
    SetCursorPos(base.x,base.y);
}

void Camera_MoveDirectional(int forwardMove, int rightMove, float speed){

    float angle= -camera.Zrot/180*M_PI;

    if (forwardMove>0)
            angle+=rightMove>0 ? M_PI_4 :(rightMove<0 ? -M_PI_4 :0);
    if (forwardMove<0)
            angle+=M_PI + (rightMove>0 ? -M_PI_4 :(rightMove<0 ? M_PI_4 :0));
    if (forwardMove==0) {
        angle +=rightMove > 0? M_PI_2 : -M_PI_2;
        if (rightMove==0) speed=0;
    }
    if (speed!=0){
        camera.x+= sin(angle)*speed;
        camera.y+= cos(angle)*speed;
    }
}
//_____________________________________________________________________________




//_____________MAIN________________________________________________
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

void WndResize(int x, int y){
    glViewport(0,0,x,y);
    float k=x/(float)y;
    float sz = 0.1;
    glLoadIdentity();
    glFrustum(-k*sz, k*sz, -sz, sz, sz*2, 100);
}

void MoveCamera(){
    Camera_MoveDirectional(
        GetKeyState('W')< 0 ? 1 : GetKeyState('S')< 0 ? -1 : 0,
        GetKeyState('D')< 0 ? 1 : GetKeyState('A')< 0 ? -1 : 0, 0.1);

    Camera_AutoMoveByMouse(400,400,0.1);
}

void drawCoords() {
    glBegin(GL_LINES);
    // X-axis (red)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    // Y-axis (green)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -100.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    // Z-axis (blue)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -100.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();
}

//___________________Ploat__________________________________________________
void drawChessboard(int n, float tileSize, float posX, float posY, float PosZ){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if ((i + j) % 2 == 0) glColor3f(0.0f, 0.7f, 0.8f);
            else glColor3f(1.0f, 0.0f, 0.0f);
            glPushMatrix();
            glTranslatef(i * tileSize + posX, j * tileSize + posY, PosZ);
            glBegin(GL_QUADS);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(tileSize, 0.0f, 0.0f);
            glVertex3f(tileSize, tileSize, 0.0f);
            glVertex3f(0.0f, tileSize, 0.0f);
            glEnd();
            glPopMatrix();
        }
    }
}
//__________________________________________________________________________
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
                          "lab 5",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          800,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    glEnable(GL_DEPTH_TEST);

    RECT rct;
    GetClientRect(hwnd,&rct);
    WndResize(rct.right,rct.bottom);

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
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glPushMatrix();

            if (GetForegroundWindow()==hwnd) MoveCamera();
            Camera_Apply();

            drawCoords();

            drawChessboard(10, 0.5, -2.5, -2.5,0);

            glBegin(GL_TRIANGLES);

                glColor3f(1.0f, 0.0f, 0.0f);   glVertex3f(0.0f, 1.0f, 1.0f);
                glColor3f(0.0f, 1.0f, 0.0f);   glVertex3f(0.87f, -0.5f, 1.0f);
                glColor3f(0.0f, 0.0f, 1.0f);   glVertex3f(-0.87f, -0.5f, 1.0f);

            glEnd();

            glPopMatrix();

            SwapBuffers(hDC);

            Sleep (10);
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
