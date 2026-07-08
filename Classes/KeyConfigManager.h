#pragma once
#include "cocos2d.h"

// Actions that can be bound to keys for each player.
enum KeyAction
{
    KeyAction_MoveUp = 0,
    KeyAction_MoveDown,
    KeyAction_MoveLeft,
    KeyAction_MoveRight,
    KeyAction_Attack,
    KeyAction_Skill1,
    KeyAction_Skill2,
    KeyAction_Skill3,
    KeyAction_Skill4,
    KeyAction_Skill5,
    KeyAction_Item1,
    KeyAction_Gear06,
    KeyAction_Gear00,
    KeyAction_Gear03,
    KeyAction_Count
};

class KeyConfigManager
{
public:
    static KeyConfigManager* getInstance();

    // Get/set the virtual key code for a player+action.
    // player: 1 or 2
    unsigned int getKeyCode(int player, KeyAction action) const;
    void setKeyCode(int player, KeyAction action, unsigned int keyCode);

    // Reset all keys for a player (or both) to defaults.
    void resetToDefaults(int player); // 1, 2, or 0 for both

    // Human-readable name for an action.
    static const char* getActionName(KeyAction action);

    // Human-readable name for a virtual key code (A-Z, 0-9, arrow keys, etc.).
    static const char* getKeyName(unsigned int keyCode);

private:
    KeyConfigManager();
    void loadFromUserDefault();
    void saveToUserDefault();
    void initDefaults();

    unsigned int _p1Keys[KeyAction_Count];
    unsigned int _p2Keys[KeyAction_Count];

    static KeyConfigManager* s_instance;
};
