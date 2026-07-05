#include "main.h"
#include "AppDelegate.h"
#include "CCEGLView.h"
#include "GameLayer.h"

USING_NS_CC;

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
            return (GameLayer*)node;
        }
    }

    return NULL;
}

static LRESULT gameWindowProc(UINT message, WPARAM wParam, LPARAM lParam, BOOL* pProcessed)
{
    GameLayer* gameLayer = getRunningGameLayer();
    if (!gameLayer)
    {
        return 0;
    }

    if (message == WM_KEYDOWN || message == WM_KEYUP)
    {
        if (gameLayer->handleKeyboard((unsigned int)wParam, message == WM_KEYDOWN))
        {
            *pProcessed = TRUE;
        }
    }
    else if (message == WM_KILLFOCUS)
    {
        gameLayer->resetKeyboardControl();
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
