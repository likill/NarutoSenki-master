#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"
#include "GameLayer.h"
#include "DualGameLayer.h"
#include "GearLayer.h"
#include "PauseLayer.h"
#include "LocalPvPResolution.h"
#include <windows.h>

USING_NS_CC;

// ============================================================================
// Window size selection (runs BEFORE Cocos2d-x engine initializes)
// ============================================================================
static HWND s_hCombo = NULL;

static LRESULT CALLBACK SizeSelectWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        HINSTANCE hInst = ((CREATESTRUCT*)lParam)->hInstance;
        HWND hLabel = CreateWindowA("STATIC", "Select your window size:",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            20, 15, 310, 20, hWnd, NULL, hInst, NULL);

        s_hCombo = CreateWindowA("COMBOBOX", "",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            80, 45, 200, 120, hWnd, (HMENU)1001, hInst, NULL);
        SendMessageA(s_hCombo, CB_ADDSTRING, 0, (LPARAM)"720 x 480");
        SendMessageA(s_hCombo, CB_ADDSTRING, 0, (LPARAM)"960 x 640");
        SendMessage(s_hCombo, CB_SETCURSEL, 1, 0);

        HWND hOK = CreateWindowA("BUTTON", "OK",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            130, 90, 100, 30, hWnd, (HMENU)IDOK, hInst, NULL);

        HFONT hFont = CreateFontA(-13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
        SendMessage(hLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(s_hCombo, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hOK, WM_SETFONT, (WPARAM)hFont, TRUE);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            int sel = (int)SendMessage(s_hCombo, CB_GETCURSEL, 0, 0);
            LocalPvPResolution::g_selectedSmallSize = (sel == 0);
            DestroyWindow(hWnd);
            return 0;
        }
        break;
    case WM_CLOSE:
        LocalPvPResolution::g_selectedSmallSize = false;
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

static void showSizeSelectDialog()
{
    HINSTANCE hInst = GetModuleHandle(NULL);
    s_hCombo = NULL;

    WNDCLASSEXA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = SizeSelectWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = "NarutoSizeSelect";
    RegisterClassExA(&wc);

    HWND hWnd = CreateWindowExA(
        WS_EX_DLGMODALFRAME, "NarutoSizeSelect",
        "NarutoSenki - Select Window Size",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 360, 180,
        NULL, NULL, hInst, NULL);
    if (!hWnd) return;

    RECT rc;
    GetWindowRect(hWnd, &rc);
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    SetWindowPos(hWnd, HWND_TOP,
        (screenW - (rc.right - rc.left)) / 2,
        (screenH - (rc.bottom - rc.top)) / 2, 0, 0, SWP_NOSIZE);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnregisterClassA("NarutoSizeSelect", hInst);
}

static bool s_keyDown[256] = { false };

static GameLayer* getRunningGameLayer()
{
    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene)
    {
        return NULL;
    }

    CCArray* children = scene->getChildren();
    if (!children)
    {
        return NULL;
    }

    CCObject* object = NULL;
    CCARRAY_FOREACH(children, object)
    {
        CCNode* node = (CCNode*)object;
        if (node && node->getZOrder() == GlTag)
        {
            DualGameLayer* dualLayer = dynamic_cast<DualGameLayer*>(node);
            if (dualLayer)
            {
                return dualLayer->getGameLayer();
            }

            return dynamic_cast<GameLayer*>(node);
        }
    }

    return NULL;
}

static void syncMovementKeys(GameLayer* gameLayer)
{
    if (!gameLayer)
    {
        return;
    }

    gameLayer->resetKeyboardControl();
    gameLayer->resetKeyboardControlP2();

    const unsigned int moveKeys[] = { 'W', 'S', 'A', 'D' };
    for (int i = 0; i < 4; ++i)
    {
        unsigned int keyCode = moveKeys[i];
        if (s_keyDown[keyCode])
        {
            gameLayer->handleKeyboard(keyCode, true);
        }
    }

    const unsigned int p2MoveKeys[] = { P2_VK_UP, P2_VK_DOWN, P2_VK_LEFT, P2_VK_RIGHT };
    for (int i = 0; i < 4; ++i)
    {
        unsigned int keyCode = p2MoveKeys[i];
        if (s_keyDown[keyCode])
        {
            gameLayer->handleKeyboardP2(keyCode, true);
        }
    }
}

static GameLayer* getPushedLayerDelegate()
{
    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene)
    {
        return NULL;
    }

    CCArray* children = scene->getChildren();
    if (!children)
    {
        return NULL;
    }

    CCObject* object = NULL;
    CCARRAY_FOREACH(children, object)
    {
        CCNode* node = (CCNode*)object;
        GearLayer* gearLayer = dynamic_cast<GearLayer*>(node);
        if (gearLayer)
        {
            return gearLayer->getDelegate();
        }

        PauseLayer* pauseLayer = dynamic_cast<PauseLayer*>(node);
        if (pauseLayer)
        {
            return pauseLayer->getDelegate();
        }
    }

    return NULL;
}

static bool handlePushedLayerHotkey(unsigned int keyCode)
{
    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene)
    {
        return false;
    }

    CCArray* children = scene->getChildren();
    if (!children)
    {
        return false;
    }

    CCObject* object = NULL;
    CCARRAY_FOREACH(children, object)
    {
        CCNode* node = (CCNode*)object;
        if (keyCode == 'G' || keyCode == P2_VK_ADD || keyCode == P2_VK_OEM_PLUS)
        {
            GearLayer* gearLayer = dynamic_cast<GearLayer*>(node);
            if (gearLayer)
            {
                GameLayer* gameLayer = gearLayer->getDelegate();
                gearLayer->resumeByKeyboard();
                syncMovementKeys(gameLayer);
                return true;
            }
        }
        else if (keyCode == 'P')
        {
            PauseLayer* pauseLayer = dynamic_cast<PauseLayer*>(node);
            if (pauseLayer)
            {
                GameLayer* gameLayer = pauseLayer->getDelegate();
                pauseLayer->resumeByKeyboard();
                syncMovementKeys(gameLayer);
                return true;
            }
        }
    }

    return false;
}

static LRESULT gameWindowProc(UINT message, WPARAM wParam, LPARAM lParam, BOOL* pProcessed)
{
    if (message == WM_KEYDOWN || message == WM_KEYUP)
    {
        unsigned int keyCode = (unsigned int)wParam;
        bool isPressed = (message == WM_KEYDOWN);
        bool wasDown = false;

        if (keyCode < 256)
        {
            wasDown = s_keyDown[keyCode];
            s_keyDown[keyCode] = isPressed;
        }

        if (isPressed)
        {
            bool isGearHotkey = (keyCode == 'G' || keyCode == P2_VK_ADD || keyCode == P2_VK_OEM_PLUS);
            if (wasDown && (isGearHotkey || keyCode == 'P'))
            {
                *pProcessed = TRUE;
                return 0;
            }

            if (handlePushedLayerHotkey(keyCode))
            {
                *pProcessed = TRUE;
                return 0;
            }
        }

        GameLayer* gameLayer = getRunningGameLayer();
        if (!gameLayer)
        {
            return 0;
        }

        if (gameLayer->handleKeyboardP2(keyCode, isPressed) || gameLayer->handleKeyboard(keyCode, isPressed))
        {
            *pProcessed = TRUE;
        }
    }
    else if (message == WM_KILLFOCUS)
    {
        ZeroMemory(s_keyDown, sizeof(s_keyDown));
        GameLayer* gameLayer = getRunningGameLayer();
        if (!gameLayer)
        {
            gameLayer = getPushedLayerDelegate();
        }

        if (gameLayer)
        {
            gameLayer->resetKeyboardControl();
            gameLayer->resetKeyboardControlP2();
        }
    }

    return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Show window size selection BEFORE engine initializes
    showSizeSelectDialog();

    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("NarutoSenki");
    eglView->setFrameSize(LocalPvPResolution::getNormalFrameWidth(),
                          LocalPvPResolution::getNormalFrameHeight());
    eglView->setWndProc(gameWindowProc);
    return CCApplication::sharedApplication()->run();
}
