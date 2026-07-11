#include "KeyConfigManager.h"
#include "Defines.h"

USING_NS_CC;

KeyConfigManager* KeyConfigManager::s_instance = NULL;

KeyConfigManager* KeyConfigManager::getInstance()
{
    if (!s_instance)
    {
        s_instance = new KeyConfigManager();
        s_instance->loadFromUserDefault();
    }
    return s_instance;
}

KeyConfigManager::KeyConfigManager()
{
    memset(_p1Keys, 0, sizeof(_p1Keys));
    memset(_p2Keys, 0, sizeof(_p2Keys));
    initDefaults();
}

void KeyConfigManager::initDefaults()
{
    // Player 1 defaults: WASD + J/K/L/U/I/O/E/F/R/T
    _p1Keys[KeyAction_MoveUp]    = 'W';
    _p1Keys[KeyAction_MoveDown]  = 'S';
    _p1Keys[KeyAction_MoveLeft]  = 'A';
    _p1Keys[KeyAction_MoveRight] = 'D';
    _p1Keys[KeyAction_Attack]    = 'J';
    _p1Keys[KeyAction_Skill1]    = 'K';
    _p1Keys[KeyAction_Skill2]    = 'L';
    _p1Keys[KeyAction_Skill3]    = ';';
    _p1Keys[KeyAction_Skill4]    = 'U';
    _p1Keys[KeyAction_Skill5]    = 'I';
    _p1Keys[KeyAction_Item1]     = 'E';
    _p1Keys[KeyAction_Gear06]    = 'Q';
    _p1Keys[KeyAction_Gear00]    = 'R';
    _p1Keys[KeyAction_Gear03]    = 'F';

    // Player 2 defaults: Arrow keys + Numpad 0-9
    _p2Keys[KeyAction_MoveUp]    = P2_VK_UP;
    _p2Keys[KeyAction_MoveDown]  = P2_VK_DOWN;
    _p2Keys[KeyAction_MoveLeft]  = P2_VK_LEFT;
    _p2Keys[KeyAction_MoveRight] = P2_VK_RIGHT;
    _p2Keys[KeyAction_Attack]    = P2_VK_NUMPAD0;
    _p2Keys[KeyAction_Skill1]    = P2_VK_NUMPAD1;
    _p2Keys[KeyAction_Skill2]    = P2_VK_NUMPAD2;
    _p2Keys[KeyAction_Skill3]    = P2_VK_NUMPAD3;
    _p2Keys[KeyAction_Skill4]    = P2_VK_NUMPAD4;
    _p2Keys[KeyAction_Skill5]    = P2_VK_NUMPAD5;
    _p2Keys[KeyAction_Item1]     = P2_VK_NUMPAD6;
    _p2Keys[KeyAction_Gear06]    = P2_VK_NUMPAD7; 
    _p2Keys[KeyAction_Gear00]    = P2_VK_NUMPAD8;
    _p2Keys[KeyAction_Gear03]    = P2_VK_NUMPAD9;
}

unsigned int KeyConfigManager::getKeyCode(int player, KeyAction action) const
{
    if (action < 0 || action >= KeyAction_Count) return 0;
    if (player == 1) return _p1Keys[action];
    if (player == 2) return _p2Keys[action];
    return 0;
}

void KeyConfigManager::setKeyCode(int player, KeyAction action, unsigned int keyCode)
{
    if (action < 0 || action >= KeyAction_Count) return;
    if (player == 1) _p1Keys[action] = keyCode;
    else if (player == 2) _p2Keys[action] = keyCode;
    saveToUserDefault();
}

void KeyConfigManager::resetToDefaults(int player)
{
    if (player == 0 || player == 1)
    {
        // Reset P1 defaults
        _p1Keys[KeyAction_MoveUp]    = 'W';
        _p1Keys[KeyAction_MoveDown]  = 'S';
        _p1Keys[KeyAction_MoveLeft]  = 'A';
        _p1Keys[KeyAction_MoveRight] = 'D';
        _p1Keys[KeyAction_Attack]    = 'J';
        _p1Keys[KeyAction_Skill1]    = 'K';
        _p1Keys[KeyAction_Skill2]    = 'L';
        _p1Keys[KeyAction_Skill3]    = 'U';
        _p1Keys[KeyAction_Skill4]    = 'I';
        _p1Keys[KeyAction_Skill5]    = 'O';
        _p1Keys[KeyAction_Item1]     = 'E';
        _p1Keys[KeyAction_Gear06]    = 'F';
        _p1Keys[KeyAction_Gear00]    = 'R';
        _p1Keys[KeyAction_Gear03]    = 'T';
    }
    if (player == 0 || player == 2)
    {
        _p2Keys[KeyAction_MoveUp]    = P2_VK_UP;
        _p2Keys[KeyAction_MoveDown]  = P2_VK_DOWN;
        _p2Keys[KeyAction_MoveLeft]  = P2_VK_LEFT;
        _p2Keys[KeyAction_MoveRight] = P2_VK_RIGHT;
        _p2Keys[KeyAction_Attack]    = P2_VK_NUMPAD0;
        _p2Keys[KeyAction_Skill1]    = P2_VK_NUMPAD1;
        _p2Keys[KeyAction_Skill2]    = P2_VK_NUMPAD2;
        _p2Keys[KeyAction_Skill3]    = P2_VK_NUMPAD3;
        _p2Keys[KeyAction_Skill4]    = P2_VK_NUMPAD4;
        _p2Keys[KeyAction_Skill5]    = P2_VK_NUMPAD5;
        _p2Keys[KeyAction_Item1]     = P2_VK_NUMPAD6;
        _p2Keys[KeyAction_Gear06]    = P2_VK_NUMPAD7;
        _p2Keys[KeyAction_Gear00]    = P2_VK_NUMPAD8;
        _p2Keys[KeyAction_Gear03]    = P2_VK_NUMPAD9;
    }
    saveToUserDefault();
}

void KeyConfigManager::loadFromUserDefault()
{
    CCUserDefault* ud = CCUserDefault::sharedUserDefault();
    if (!ud->getBoolForKey("isKeyConfigSaved"))
    {
        // First run — use defaults and mark as saved.
        initDefaults();
        saveToUserDefault();
        return;
    }

    const char* p1Names[] = {
        "P1_MoveUp", "P1_MoveDown", "P1_MoveLeft", "P1_MoveRight",
        "P1_Attack", "P1_Skill1", "P1_Skill2", "P1_Skill3",
        "P1_Skill4", "P1_Skill5", "P1_Item1", "P1_Gear06",
        "P1_Gear00", "P1_Gear03"
    };
    const char* p2Names[] = {
        "P2_MoveUp", "P2_MoveDown", "P2_MoveLeft", "P2_MoveRight",
        "P2_Attack", "P2_Skill1", "P2_Skill2", "P2_Skill3",
        "P2_Skill4", "P2_Skill5", "P2_Item1", "P2_Gear06",
        "P2_Gear00", "P2_Gear03"
    };

    for (int i = 0; i < KeyAction_Count; ++i)
    {
        _p1Keys[i] = (unsigned int)ud->getIntegerForKey(p1Names[i], _p1Keys[i]);
        _p2Keys[i] = (unsigned int)ud->getIntegerForKey(p2Names[i], _p2Keys[i]);
    }
}

void KeyConfigManager::saveToUserDefault()
{
    CCUserDefault* ud = CCUserDefault::sharedUserDefault();
    ud->setBoolForKey("isKeyConfigSaved", true);

    const char* p1Names[] = {
        "P1_MoveUp", "P1_MoveDown", "P1_MoveLeft", "P1_MoveRight",
        "P1_Attack", "P1_Skill1", "P1_Skill2", "P1_Skill3",
        "P1_Skill4", "P1_Skill5", "P1_Item1", "P1_Gear06",
        "P1_Gear00", "P1_Gear03"
    };
    const char* p2Names[] = {
        "P2_MoveUp", "P2_MoveDown", "P2_MoveLeft", "P2_MoveRight",
        "P2_Attack", "P2_Skill1", "P2_Skill2", "P2_Skill3",
        "P2_Skill4", "P2_Skill5", "P2_Item1", "P2_Gear06",
        "P2_Gear00", "P2_Gear03"
    };

    for (int i = 0; i < KeyAction_Count; ++i)
    {
        ud->setIntegerForKey(p1Names[i], (int)_p1Keys[i]);
        ud->setIntegerForKey(p2Names[i], (int)_p2Keys[i]);
    }
    ud->flush();
}

const char* KeyConfigManager::getActionName(KeyAction action)
{
    switch (action)
    {
    case KeyAction_MoveUp:    return "Move Up";
    case KeyAction_MoveDown:  return "Move Down";
    case KeyAction_MoveLeft:  return "Move Left";
    case KeyAction_MoveRight: return "Move Right";
    case KeyAction_Attack:    return "Attack";
    case KeyAction_Skill1:    return "Skill 1";
    case KeyAction_Skill2:    return "Skill 2";
    case KeyAction_Skill3:    return "Skill 3";
    case KeyAction_Skill4:    return "Skill 4";
    case KeyAction_Skill5:    return "Skill 5";
    case KeyAction_Item1:     return "Gear";
    case KeyAction_Gear06:    return "Item 2";
    case KeyAction_Gear00:    return "Item 3";
    case KeyAction_Gear03:    return "Item 4";
    default: return "???";
    }
}

const char* KeyConfigManager::getKeyName(unsigned int keyCode)
{
    // Use a rotating buffer so multiple calls in one expression don't collide.
    static char bufs[4][16];
    static int idx = 0;
    char* buf = bufs[idx++ & 3];

    // Letters A-Z
    if (keyCode >= 'A' && keyCode <= 'Z')
    {
        buf[0] = (char)keyCode;
        buf[1] = 0;
        return buf;
    }
    // Digits 0-9
    if (keyCode >= '0' && keyCode <= '9')
    {
        buf[0] = (char)keyCode;
        buf[1] = 0;
        return buf;
    }

    switch (keyCode)
    {
    case 0x26: return "Up";
    case 0x28: return "Down";
    case 0x25: return "Left";
    case 0x27: return "Right";
    case 0x60: return "Num0";
    case 0x61: return "Num1";
    case 0x62: return "Num2";
    case 0x63: return "Num3";
    case 0x64: return "Num4";
    case 0x65: return "Num5";
    case 0x66: return "Num6";
    case 0x67: return "Num7";
    case 0x68: return "Num8";
    case 0x69: return "Num9";
    case 0x6B: return "Num+";
    case 0xBB: return "+=";
    case 0x20: return "Space";
    case 0x0D: return "Enter";
    case 0x10: return "Shift";
    case 0x11: return "Ctrl";
    case 0x12: return "Alt";
    case 0x1B: return "Esc";
    case 0x09: return "Tab";
    case 0x08: return "Back";
    default:
    {
        sprintf(buf, "0x%02X", keyCode);
        return buf;
    }
    }
}
