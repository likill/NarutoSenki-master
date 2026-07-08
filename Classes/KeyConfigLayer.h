#pragma once
#include "cocos2d.h"
#include "KeyConfigManager.h"

USING_NS_CC;

class KeyConfigLayer : public CCLayer
{
public:
    KeyConfigLayer();
    ~KeyConfigLayer();

    bool init();
    CREATE_FUNC(KeyConfigLayer);

    // Called by main.cpp when a key is pressed while listening.
    void onKeyCaptured(unsigned int keyCode);

    // Returns true if the layer is currently waiting for a key press.
    bool isListening() const { return _isListening; }

    // Static accessor for main.cpp to find the active layer.
    static KeyConfigLayer* getActiveInstance() { return s_activeInstance; }

private:
    void buildUI();
    void refreshKeyLabels();
    void onStartListen(CCObject* sender);
    void onStopListen();
    void onResetDefaults(CCObject* sender);
    void onBack(CCObject* sender);

    CCLabelTTF* _p1Labels[KeyAction_Count];
    CCLabelTTF* _p2Labels[KeyAction_Count];
    CCLabelTTF* _listenHint;
    CCLayerColor* _listenHighlight;

    bool _isListening;
    int  _listeningPlayer;    // 1 or 2
    int  _listeningAction;    // KeyAction index

    static KeyConfigLayer* s_activeInstance;
};
