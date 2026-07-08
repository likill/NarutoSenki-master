#include "KeyConfigLayer.h"
#include "Defines.h"

USING_NS_CC;

KeyConfigLayer* KeyConfigLayer::s_activeInstance = NULL;

KeyConfigLayer::KeyConfigLayer()
    : _isListening(false)
    , _listeningPlayer(0)
    , _listeningAction(0)
    , _listenHint(NULL)
    , _listenHighlight(NULL)
{
    memset(_p1Labels, 0, sizeof(_p1Labels));
    memset(_p2Labels, 0, sizeof(_p2Labels));
}

KeyConfigLayer::~KeyConfigLayer()
{
    if (s_activeInstance == this)
    {
        s_activeInstance = NULL;
    }
}

bool KeyConfigLayer::init()
{
    if (!CCLayer::init())
        return false;

    s_activeInstance = this;
    buildUI();
    return true;
}

void KeyConfigLayer::buildUI()
{
    CCSize sz = winSize;

    // Semi-transparent background
    CCLayerColor* bg = CCLayerColor::create(ccc4(0, 0, 0, 180), sz.width, sz.height);
    this->addChild(bg, 0);

    // Title
    CCLabelTTF* title = CCLabelTTF::create("Key Settings", FONT_TYPE, 20);
    title->setPosition(ccp(sz.width / 2, sz.height - 20));
    this->addChild(title, 1);

    // Column headers
    CCLabelTTF* p1Header = CCLabelTTF::create("Player 1", FONT_TYPE, 14);
    p1Header->setPosition(ccp(sz.width * 0.25f, sz.height - 42));
    this->addChild(p1Header, 1);

    CCLabelTTF* p2Header = CCLabelTTF::create("Player 2", FONT_TYPE, 14);
    p2Header->setPosition(ccp(sz.width * 0.70f, sz.height - 42));
    this->addChild(p2Header, 1);

    float startY = sz.height - 60;
    float rowH = 16.5f;

    for (int i = 0; i < KeyAction_Count; ++i)
    {
        float y = startY - i * rowH;

        // --- P1 column ---
        CCLabelTTF* p1Name = CCLabelTTF::create(
            KeyConfigManager::getActionName((KeyAction)i), FONT_TYPE, 10);
        p1Name->setAnchorPoint(ccp(1, 0.5f));
        p1Name->setPosition(ccp(sz.width * 0.18f, y));
        this->addChild(p1Name, 1);

        _p1Labels[i] = CCLabelTTF::create("", FONT_TYPE, 10);
        _p1Labels[i]->setPosition(ccp(sz.width * 0.28f, y));
        this->addChild(_p1Labels[i], 1);

        // "修改" button as a label-based menu item
        CCLabelTTF* editLabel1 = CCLabelTTF::create(
            "Edit", FONT_TYPE, 10);
        CCMenuItemLabel* editBtn1 = CCMenuItemLabel::create(
            editLabel1, this, menu_selector(KeyConfigLayer::onStartListen));
        editBtn1->setTag(100 + i); // encode player=1, action=i
        editBtn1->setPosition(ccp(sz.width * 0.35f, y));

        CCMenu* menu1 = CCMenu::create(editBtn1, NULL);
        menu1->setPosition(CCPointZero);
        this->addChild(menu1, 1);

        // --- P2 column ---
        CCLabelTTF* p2Name = CCLabelTTF::create(
            KeyConfigManager::getActionName((KeyAction)i), FONT_TYPE, 10);
        p2Name->setAnchorPoint(ccp(1, 0.5f));
        p2Name->setPosition(ccp(sz.width * 0.60f, y));
        this->addChild(p2Name, 1);

        _p2Labels[i] = CCLabelTTF::create("", FONT_TYPE, 10);
        _p2Labels[i]->setPosition(ccp(sz.width * 0.70f, y));
        this->addChild(_p2Labels[i], 1);

        CCLabelTTF* editLabel2 = CCLabelTTF::create(
            "Edit", FONT_TYPE, 10);
        CCMenuItemLabel* editBtn2 = CCMenuItemLabel::create(
            editLabel2, this, menu_selector(KeyConfigLayer::onStartListen));
        editBtn2->setTag(200 + i); // encode player=2, action=i
        editBtn2->setPosition(ccp(sz.width * 0.78f, y));

        CCMenu* menu2 = CCMenu::create(editBtn2, NULL);
        menu2->setPosition(CCPointZero);
        this->addChild(menu2, 1);
    }

    // Listen hint (hidden by default)
    _listenHint = CCLabelTTF::create(
        "Press a key...",
        FONT_TYPE, 14);
    _listenHint->setPosition(ccp(sz.width / 2, 45));
    _listenHint->setVisible(false);
    this->addChild(_listenHint, 2);

    // Highlight rect for the row being edited (CCLayerColor: position = bottom-left corner)
    _listenHighlight = CCLayerColor::create(ccc4(255, 255, 0, 80), 160, 14);
    _listenHighlight->setVisible(false);
    this->addChild(_listenHighlight, 1);

    // Bottom buttons
    CCLabelTTF* resetLabel = CCLabelTTF::create(
        "Reset Defaults", FONT_TYPE, 14);
    CCMenuItemLabel* resetBtn = CCMenuItemLabel::create(
        resetLabel, this, menu_selector(KeyConfigLayer::onResetDefaults));
    resetBtn->setPosition(ccp(sz.width / 2 - 70, 22));

    CCLabelTTF* backLabel = CCLabelTTF::create(
        "Back", FONT_TYPE, 14);
    CCMenuItemLabel* backBtn = CCMenuItemLabel::create(
        backLabel, this, menu_selector(KeyConfigLayer::onBack));
    backBtn->setPosition(ccp(sz.width / 2 + 70, 22));

    CCMenu* bottomMenu = CCMenu::create(resetBtn, backBtn, NULL);
    bottomMenu->setPosition(CCPointZero);
    this->addChild(bottomMenu, 1);

    refreshKeyLabels();
}

void KeyConfigLayer::refreshKeyLabels()
{
    KeyConfigManager* mgr = KeyConfigManager::getInstance();
    for (int i = 0; i < KeyAction_Count; ++i)
    {
        unsigned int k1 = mgr->getKeyCode(1, (KeyAction)i);
        _p1Labels[i]->setString(KeyConfigManager::getKeyName(k1));

        unsigned int k2 = mgr->getKeyCode(2, (KeyAction)i);
        _p2Labels[i]->setString(KeyConfigManager::getKeyName(k2));
    }
}

void KeyConfigLayer::onStartListen(CCObject* sender)
{
    CCMenuItemLabel* item = (CCMenuItemLabel*)sender;
    int tag = item->getTag();

    int player = tag / 100;   // 1 or 2
    int action = tag % 100;   // KeyAction index

    if (player < 1 || player > 2 || action < 0 || action >= KeyAction_Count)
        return;

    _isListening = true;
    _listeningPlayer = player;
    _listeningAction = action;
    _listenHint->setVisible(true);

    // Position highlight over the target row (CCLayerColor anchor = bottom-left)
    float startY = winSize.height - 60;
    float y = startY - action * 16.5f - 7;
    float x = (player == 1) ? winSize.width * 0.25f - 80 : winSize.width * 0.70f - 80;
    _listenHighlight->setPosition(ccp(x, y));
    _listenHighlight->setVisible(true);

    // The actual key capture happens in main.cpp -> onKeyCaptured()
}

void KeyConfigLayer::onStopListen()
{
    _isListening = false;
    _listenHint->setVisible(false);
    _listenHighlight->setVisible(false);
}

void KeyConfigLayer::onKeyCaptured(unsigned int keyCode)
{
    if (!_isListening) return;

    // Ignore modifier keys alone
    if (keyCode == 0x10 || keyCode == 0x11 || keyCode == 0x12)
        return;

    KeyConfigManager::getInstance()->setKeyCode(
        _listeningPlayer, (KeyAction)_listeningAction, keyCode);

    onStopListen();
    refreshKeyLabels();
}

void KeyConfigLayer::onResetDefaults(CCObject* sender)
{
    KeyConfigManager::getInstance()->resetToDefaults(0);
    refreshKeyLabels();
}

void KeyConfigLayer::onBack(CCObject* sender)
{
    this->removeFromParentAndCleanup(true);
}
