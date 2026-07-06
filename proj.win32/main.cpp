#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"
#include "GameLayer.h"
#include "DualGameLayer.h"
#include "GearLayer.h"
#include "PauseLayer.h"

USING_NS_CC;

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

    // create the application instance
    AppDelegate app;
    CCEGLView* eglView = CCEGLView::sharedOpenGLView();
    eglView->setViewName("NarutoSenki");
    eglView->setFrameSize(480, 320);
    eglView->setWndProc(gameWindowProc);
    return CCApplication::sharedApplication()->run();
}
