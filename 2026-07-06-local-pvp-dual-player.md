# 本地双人PvP (Local 2-Player Split-Screen) 实现方案

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal：** 在训练模式基础上实现本地双人分屏3v3对战，两个玩家使用不同键盘按键控制各自角色，剩余队友由AI操控。

**架构：** 最大程度复用现有代码。DualSelectLayer 复用 SelectButton 和 CCTips；DualGameLayer 内部持有两个 GameLayer 实例复用全部游戏逻辑；P2的HUD通过继承 HudLayer 复用 initHeroInterface()；键盘处理复用 GameLayer::handleKeyboard() 模式。

**Tech Stack：** Cocos2d-x 2.2.2, C++ (Windows Win32), Win32 API (WndProc键盘钩子)

---

## 〇、可复用函数清单（写代码前必读）

> **原则：先找现有函数，能复用就复用，实在不行才新建。**

### 0.1 可直接复用的类

| 类 | 文件 | 可复用内容 | 复用方式 |
|---|---|---|---|
| `SelectButton` | `SkillLayer.h:72` | 英雄选择按钮，触摸处理，click()分发 | 直接使用，需添加 `_delegate4` 指向 DualSelectLayer |
| `MenuButton` | `StartMenu.h:30` | 主菜单按钮，触摸+拖拽+音效 | 直接使用，在 btnType 枚举中添加 `LocalPvP` |
| `ActionButton` | `ActionButton.h:11` | 技能按钮，冷却计时，进度条，click() | 直接复用，P2的HUD中创建新的 ActionButton 实例 |
| `JoyStick` | `JoyStick.h:10` | 虚拟摇杆，触摸方向计算 | 直接复用，P2的HUD中创建新的 JoyStick 实例 |
| `CCTips` | `MyUtils/KTools.h:63` | 通知弹窗，自动销毁 | 直接使用 `CCTips::create("msg")` |
| `MiniIcon` | `HudLayer.h:17` | 小地图图标，位置同步 | 直接复用 |
| `BGLayer` | `BGLayer.h:8` | 地图背景，根据 randomMap 选图 | 直接复用，为P2创建第二个实例 |
| `GearLayer` | `GearLayer.h:18` | 装备商店界面 | 直接复用，P2按键触发同一个 onGear() |
| `PauseLayer` | `PauseLayer.h:12` | 暂停界面 | 直接复用 |

### 0.2 可直接复用的函数

| 函数 | 文件:行 | 功能 | 复用方式 |
|---|---|---|---|
| `HudLayer::initHeroInterface()` | `HudLayer.cpp:229` | 创建全部HUD元素(状态栏、HP条、技能按钮、摇杆、小地图) | P2的HUD继承HudLayer后调用此函数，再修改位置 |
| `HudLayer::initGearButton()` | `HudLayer.cpp:183` | 创建装备按钮 | 被 initHeroInterface() 自动调用 |
| `HudLayer::onKaichang()` | `HudLayer.cpp:198` | 开场动画(8帧序列帧) | 被 GameLayer::onKaichang() 调用 |
| `HudLayer::setHPLose()` | `HudLayer.cpp` | 更新HP条显示 | 被 GameLayer::setHPLose() 调用 |
| `HudLayer::setCKRLose()` | `HudLayer.cpp` | 更新CKR条显示 | 被 GameLayer::setCKRLose() 调用 |
| `HudLayer::setCoin()` | `HudLayer.cpp` | 更新金币显示 | 被 GameLayer::setCoin() 调用 |
| `GameLayer::initHeros()` | `GameLayer.cpp:366` | 从tempHeros数组初始化所有角色 | 直接复用，传入不同的tempHeros |
| `GameLayer::initTileMap()` | `GameLayer.cpp:270` | 加载TMX地图 | 被 initHeros() 自动调用 |
| `GameLayer::initFlogs()` | `GameLayer.cpp:580` | 初始化小兵 | 被 onGameStart() 自动调用 |
| `GameLayer::initTower()` | `GameLayer.cpp:690` | 初始化防御塔 | 被 initHeros() 自动调用 |
| `GameLayer::initEffects()` | `GameLayer.cpp:768` | 初始化特效批次 | 被 initHeros() 自动调用 |
| `GameLayer::handleKeyboard()` | `GameLayer.cpp:1031` | P1键盘输入分发(WASD+JKL等) | 直接复用，不修改 |
| `GameLayer::resetKeyboardControl()` | `GameLayer.cpp:1165` | 重置P1键盘状态 | 直接复用 |
| `GameLayer::updateKeyboardMove()` | `GameLayer.cpp:1188` | 从键盘标志合成移动方向 | 复用模式，写P2版本 |
| `GameLayer::clickKeyboardButton()` | `GameLayer.cpp:1217` | 触发按钮click() | 直接复用，P2传入P2的按钮 |
| `GameLayer::JoyStickUpdate()` | `GameLayer.cpp:1020` | 处理移动方向 | 直接复用 |
| `GameLayer::JoyStickRelease()` | `GameLayer.cpp:998` | 停止移动 | 直接复用 |
| `GameLayer::attackButtonClick()` | `GameLayer.cpp:1225` | 处理攻击 | 直接复用 |
| `GameLayer::onPause()` | `GameLayer.cpp:1247` | 暂停(截图+push PauseLayer) | 直接复用 |
| `GameLayer::onGear()` | `GameLayer.cpp:1269` | 打开装备(截图+push GearLayer) | 直接复用 |
| `GameLayer::onGameOver()` | `GameLayer.cpp:1062` | 游戏结束(截图+push GameOver) | 需要在DualGameLayer层统一处理 |
| `GameLayer::updateViewPoint()` | `GameLayer.cpp:803` | 摄像机跟随玩家 | 直接复用，P2写新版本 |
| `GameLayer::onGameStart()` | `GameLayer.cpp:560` | 开始游戏(初始化HUD、启动AI、开始刷兵) | 直接复用 |
| `LoadLayer::preloadAudio()` | `LoadLayer.cpp:392` | 预加载音频+图片资源 | 直接复用 |
| `SelectLayer::onGameStart()` | `SelectLayer.cpp:277` | 构建tempHeros并跳转LoadLayer | 复用模式，写DualSelectLayer版本 |
| `NetworkLayer::onGameStart()` | `NetworkLayer.cpp:850` | 构建tempHeros(含队伍分配) | 复用英雄列表和分配逻辑 |
| `main.cpp::gameWindowProc()` | `proj.win32/main.cpp:138` | Windows键盘消息钩子 | 修改，添加P2键码分发 |
| `main.cpp::getRunningGameLayer()` | `proj.win32/main.cpp:12` | 从场景获取GameLayer | 修改，支持DualGameLayer |
| `main.cpp::syncMovementKeys()` | `proj.win32/main.cpp:39` | 同步移动按键状态 | 直接复用 |

### 0.3 可复用的数据结构和常量

| 名称 | 文件 | 说明 |
|---|---|---|
| `tempHeros` 格式 | SelectLayer.cpp:292 | CCArray of CCDictionary("character","role","group") |
| `heroList[]` | NetworkLayer.cpp:872 | 完整英雄列表(33个) |
| `selectList[]` | NetworkLayer.cpp:221 | 选角界面英雄列表(含None占位) |
| `abType` 枚举 | Defines.h:69 | NAttack,Item1,SKILL1-3,OUGIS1-2 |
| `gearType` 枚举 | Defines.h:83 | gear00-08,None |
| `btnType` 枚举 | StartMenu.h:20 | Network,Training,Exit,Credits,HardCore |
| `actionState` 枚举 | ActionManager.h:14 | DEAD,IDLE,WALK,ATTACK等 |
| `winSize` 宏 | Defines.h:8 | 窗口尺寸 |
| `GlTag,BgTag,HudTag` | Defines.h:15-17 | 层级Z-order |
| `SELECT_SOUND` 等 | Defines.h:36-46 | 音效路径 |
| `FONT_TYPE` | Defines.h:12 | 默认字体 |

### 0.4 可复用的plist资源

| 资源 | 内容 | 复用场景 |
|---|---|---|
| `UI.plist/UI.png` | 状态栏、技能按钮、摇杆等UI元素 | P2的HUD直接使用 |
| `Menu.plist` | 主菜单按钮精灵 | 本地联机按钮 |
| `Record.plist/Record2.plist` | 选角界面元素 | DualSelectLayer |
| `Select.plist/Select2.plist` | 英雄选择头像 | DualSelectLayer |
| `Ougis.plist/Ougis2.plist` | 大招特效 | 游戏中自动使用 |
| `Gears.plist` | 装备图标 | 装备界面 |
| `Map.plist` | 小地图元素 | HUD小地图 |

### 0.5 复用决策总结

| 需求 | ❌ 不要做的事 | ✅ 应该做的事 |
|---|---|---|
| P2英雄选择按钮 | 新建一个完整的按钮类 | 给 SelectButton 添加 `_delegate4`，复用 click() 逻辑 |
| P2的HUD | 从零创建所有UI元素 | 继承 HudLayer，调用 `initHeroInterface()`，然后修改位置 |
| P2键盘处理 | 重写一套输入系统 | 复制 `handleKeyboard()` 模式，改键码和目标角色 |
| 英雄列表 | 重新定义英雄数组 | 直接复制 `NetworkLayer::onGameStart()` 中的 `heroList[]` |
| tempHeros构建 | 重新发明数据格式 | 复用 `SelectLayer::onGameStart()` 的构建逻辑 |
| 暂停/装备界面 | 为P2创建新界面 | 复用 `onPause()` 和 `onGear()`，两个玩家都能触发 |
| 场景切换 | 手动管理场景 | 复用 `CCDirector::replaceScene(CCTransitionFade::create())` |
| 通知提示 | 自己写弹窗 | 使用 `CCTips::create("msg")` |

---

## 一、新增/修改文件清单

```
新增文件:
  Classes/DualSelectLayer.h          -- 双人选角界面(复用SelectButton+CCTips)
  Classes/DualSelectLayer.cpp
  Classes/DualGameLayer.h            -- 双人游戏主层(持有两个GameLayer)
  Classes/DualGameLayer.cpp

修改文件:
  Classes/SkillLayer.h               -- SelectButton添加_delegate4(DualSelectLayer*)
  Classes/SkillLayer.cpp             -- SelectButton::click()添加DualSelectLayer分支
  Classes/StartMenu.h                -- btnType枚举添加LocalPvP，添加回调声明
  Classes/StartMenu.cpp              -- 添加本地联机按钮和回调
  Classes/GameLayer.h                -- 添加P2字段和方法声明
  Classes/GameLayer.cpp              -- 添加P2键盘处理(复用P1模式)
  Classes/Defines.h                  -- 添加P2键码常量
  Classes/LoadLayer.h                -- 添加_isLocalPvP标志
  Classes/LoadLayer.cpp              -- 支持创建DualGameLayer
  proj.win32/main.cpp                -- 添加P2键盘事件分发
```

---

## 二、实现任务分解

### Task 1: 修改 Defines.h 和 StartMenu.h 添加常量和枚举

**文件：**
- Modify: `Classes/Defines.h`
- Modify: `Classes/StartMenu.h:20-26`

**Step 1: 在 Defines.h 末尾添加P2键码常量**

```cpp
// Local PvP - P2 keyboard codes (Windows Virtual Key Codes)
#define P2_VK_UP        0x26  // VK_UP
#define P2_VK_DOWN      0x28  // VK_DOWN
#define P2_VK_LEFT      0x25  // VK_LEFT
#define P2_VK_RIGHT     0x27  // VK_RIGHT
#define P2_VK_ATTACK    0x60  // VK_NUMPAD0
#define P2_VK_SKILL1    0x61  // VK_NUMPAD1
#define P2_VK_SKILL2    0x62  // VK_NUMPAD2
#define P2_VK_SKILL3    0x63  // VK_NUMPAD3
#define P2_VK_OUGIS1    0x64  // VK_NUMPAD4
#define P2_VK_OUGIS2    0x65  // VK_NUMPAD5
#define P2_VK_ITEM1     0x66  // VK_NUMPAD6
#define P2_VK_GEAR06    0x67  // VK_NUMPAD7
#define P2_VK_GEAR00    0x68  // VK_NUMPAD8
#define P2_VK_GEAR03    0x69  // VK_NUMPAD9
#define P2_VK_GEAR_OPEN 0x6B  // VK_ADD
```

**Step 2: 在 StartMenu.h 的 btnType 枚举中添加 LocalPvP**

```cpp
enum btnType{
    Network,
    Training,
    Exit,
    Credits,
    HardCore,
    LocalPvP     // 新增
};
```

**Step 3: 在 StartMenu 类中添加回调声明**

```cpp
// 在 StartMenu.h 的 public 区域添加:
void onLocalPvPCallBack();
```

---

### Task 2: 修改 SelectButton 支持 DualSelectLayer

**文件：**
- Modify: `Classes/SkillLayer.h:72-109`
- Modify: `Classes/SkillLayer.cpp:81-148`

**为什么要修改：** SelectButton 的 click() 方法通过 `_delegate1`(SelectLayer)、`_delegate2`(NetworkLayer)、`_delegate3`(SkillLayer) 分发事件。要复用 SelectButton，需要添加 `_delegate4`(DualSelectLayer)。

**Step 1: 在 SkillLayer.h 的 SelectButton 类中添加 _delegate4**

```cpp
// 在 SkillLayer.h 的 SelectButton 类中添加前向声明:
class DualSelectLayer;

// 在 SelectButton 的 public 区域添加:
CC_SYNTHESIZE(DualSelectLayer*, _delegate4, Delegate4);
```

**Step 2: 在 SkillLayer.cpp 的 SelectButton 构造函数中初始化**

```cpp
// 在 SelectButton::SelectButton() 中添加:
_delegate4 = NULL;
```

**Step 3: 在 SelectButton::click() 中添加 DualSelectLayer 分支**

```cpp
// 在 SelectButton::click() 的 if(_delegate2) 分支之后，else if(_delegate1) 之前添加:
else if(_delegate4){
    // DualSelectLayer: 单击即锁定
    SimpleAudioEngine::sharedEngine()->playEffect(SELECT_SOUND);
    _delegate4->setSelected(this);
}
```

---

### Task 3: 修改 GameLayer 添加P2支持

**文件：**
- Modify: `Classes/GameLayer.h`
- Modify: `Classes/GameLayer.cpp`

**Step 1: 在 GameLayer.h 中添加P2字段**

```cpp
// 在 GameLayer.h 的 public 区域添加:
Hero*               player2;
bool                _isLocalPvP;
bool handleKeyboardP2(unsigned int keyCode, bool isPressed);
void resetKeyboardControlP2();

// 在 protected 区域添加:
void updateKeyboardMoveP2();
void clickKeyboardButtonP2(ActionButton* button);

// P2键盘状态标志 (复用P1的模式)
bool _p2_keyMoveUp;
bool _p2_keyMoveDown;
bool _p2_keyMoveLeft;
bool _p2_keyMoveRight;
bool _p2_keyAttack;
bool _p2_keySkill1;
bool _p2_keySkill2;
bool _p2_keySkill3;
bool _p2_keySkill4;
bool _p2_keySkill5;
bool _p2_keyItem1;
bool _p2_keyGear06;
bool _p2_keyGear00;
bool _p2_keyGear03;
```

**Step 2: 在 GameLayer.cpp 构造函数中初始化P2字段**

```cpp
// 在 GameLayer::GameLayer() 中添加:
player2 = NULL;
_isLocalPvP = false;
_p2_keyMoveUp = false;
_p2_keyMoveDown = false;
_p2_keyMoveLeft = false;
_p2_keyMoveRight = false;
_p2_keyAttack = false;
_p2_keySkill1 = false;
_p2_keySkill2 = false;
_p2_keySkill3 = false;
_p2_keySkill4 = false;
_p2_keySkill5 = false;
_p2_keyItem1 = false;
_p2_keyGear06 = false;
_p2_keyGear00 = false;
_p2_keyGear03 = false;
```

**Step 3: 在 initHeros() 的3v3分支中支持P2**

在 `GameLayer::initHeros()` 的 `else` 分支（3v3模式，约第444行）中，修改 `i==1` 的处理：

```cpp
// 原代码: i==1 时创建 AI Com
// 修改为:
if(i == 0) {
    // P1 角色 (不变)
    if(strcmp(group->getCString(),"Akatsuki")==0) {
        zhenying=0;
    }
    currentPlayer=Hero::create();
    currentPlayer->setDelegate(this);
    currentPlayer->setID(player,role,group);
    currentPlayer->setPosition(spawnPoint);
    currentPlayer->setSpawnPoint(spawnPoint);
    this->addChild(currentPlayer,-currentPlayer->getPositionY());
    currentPlayer->setHPbar();
    currentPlayer->setShadows();
    currentPlayer->idle();
    if(zhenying<=0){
        currentPlayer->_isFlipped=true;
        currentPlayer->setFlipX(true);
    }
    currentPlayer->setCharNO(i+1);
    currentPlayer->schedule(schedule_selector(ActionManager::setRestore2),1.0f);
    _CharacterArray->addObject(currentPlayer);

} else if(i == 1 && _isLocalPvP) {
    // P2 角色 (新增，复用P1的创建模式)
    player2=Hero::create();
    player2->setDelegate(this);
    player2->setID(player, CCString::create("Player2"), group);
    player2->setPosition(spawnPoint);
    player2->setSpawnPoint(spawnPoint);
    this->addChild(player2,-player2->getPositionY());
    player2->setHPbar();
    player2->setShadows();
    player2->idle();
    if(strcmp(player2->getGroup()->getCString(),"Akatsuki")==0){
        player2->_isFlipped=true;
        player2->setFlipX(true);
    }
    player2->setCharNO(i+1);
    player2->schedule(schedule_selector(ActionManager::setRestore2),1.0f);
    _CharacterArray->addObject(player2);

} else {
    // AI 角色 (不变)
    Hero* Com=Hero::create();
    Com->setDelegate(this);
    Com->setID(player,role,group);
    // ... 原有代码 ...
}
```

**Step 4: 添加P2键盘处理方法 (复用P1的 handleKeyboard 模式)**

```cpp
// 在 GameLayer.cpp 末尾添加:

bool GameLayer::handleKeyboardP2(unsigned int keyCode, bool isPressed) {
    if(!_isLocalPvP || !player2) return false;

    switch(keyCode) {
    case P2_VK_UP:
        _p2_keyMoveUp=isPressed;
        this->updateKeyboardMoveP2();
        return true;
    case P2_VK_DOWN:
        _p2_keyMoveDown=isPressed;
        this->updateKeyboardMoveP2();
        return true;
    case P2_VK_LEFT:
        _p2_keyMoveLeft=isPressed;
        this->updateKeyboardMoveP2();
        return true;
    case P2_VK_RIGHT:
        _p2_keyMoveRight=isPressed;
        this->updateKeyboardMoveP2();
        return true;
    case P2_VK_ATTACK:
        if(isPressed){
            if(!_p2_keyAttack){
                _p2_keyAttack=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->nAttackButton : NULL);
            }
        }else{
            _p2_keyAttack=false;
            // P2攻击释放
        }
        return true;
    case P2_VK_SKILL1:
        if(isPressed){
            if(!_p2_keySkill1){
                _p2_keySkill1=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->skill1Button : NULL);
            }
        }else{
            _p2_keySkill1=false;
        }
        return true;
    case P2_VK_SKILL2:
        if(isPressed){
            if(!_p2_keySkill2){
                _p2_keySkill2=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->skill2Button : NULL);
            }
        }else{
            _p2_keySkill2=false;
        }
        return true;
    case P2_VK_SKILL3:
        if(isPressed){
            if(!_p2_keySkill3){
                _p2_keySkill3=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->skill3Button : NULL);
            }
        }else{
            _p2_keySkill3=false;
        }
        return true;
    case P2_VK_OUGIS1:
        if(isPressed){
            if(!_p2_keySkill4){
                _p2_keySkill4=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->skill4Button : NULL);
            }
        }else{
            _p2_keySkill4=false;
        }
        return true;
    case P2_VK_OUGIS2:
        if(isPressed){
            if(!_p2_keySkill5){
                _p2_keySkill5=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->skill5Button : NULL);
            }
        }else{
            _p2_keySkill5=false;
        }
        return true;
    case P2_VK_ITEM1:
        if(isPressed){
            if(!_p2_keyItem1){
                _p2_keyItem1=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->item1Button : NULL);
            }
        }else{
            _p2_keyItem1=false;
        }
        return true;
    case P2_VK_GEAR06:
        if(isPressed){
            if(!_p2_keyGear06){
                _p2_keyGear06=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->getItem2Button() : NULL);
            }
        }else{
            _p2_keyGear06=false;
        }
        return true;
    case P2_VK_GEAR00:
        if(isPressed){
            if(!_p2_keyGear00){
                _p2_keyGear00=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->getItem3Button() : NULL);
            }
        }else{
            _p2_keyGear00=false;
        }
        return true;
    case P2_VK_GEAR03:
        if(isPressed){
            if(!_p2_keyGear03){
                _p2_keyGear03=true;
                this->clickKeyboardButtonP2(_hudLayer ? _hudLayer->getItem4Button() : NULL);
            }
        }else{
            _p2_keyGear03=false;
        }
        return true;
    case P2_VK_GEAR_OPEN:
        if(isPressed && _hudLayer && !_hudLayer->_isAllButtonLocked){
            this->onGear();  // 复用现有装备界面
        }
        return true;
    default:
        return false;
    }
}

void GameLayer::resetKeyboardControlP2() {
    _p2_keyMoveUp=false;
    _p2_keyMoveDown=false;
    _p2_keyMoveLeft=false;
    _p2_keyMoveRight=false;
    _p2_keyAttack=false;
    _p2_keySkill1=false;
    _p2_keySkill2=false;
    _p2_keySkill3=false;
    _p2_keySkill4=false;
    _p2_keySkill5=false;
    _p2_keyItem1=false;
    _p2_keyGear06=false;
    _p2_keyGear00=false;
    _p2_keyGear03=false;
}

void GameLayer::updateKeyboardMoveP2() {
    // 复用 updateKeyboardMove() 的逻辑，但操作 player2
    if(!player2) return;

    float x=0.0f;
    float y=0.0f;

    if(_p2_keyMoveLeft)  x-=1.0f;
    if(_p2_keyMoveRight) x+=1.0f;
    if(_p2_keyMoveUp)    y+=1.0f;
    if(_p2_keyMoveDown)  y-=1.0f;

    if(x==0.0f && y==0.0f){
        if(player2->getActionState()==ACTION_STATE_WALK){
            player2->idle();
        }
    }else{
        player2->walk(ccp(x,y));
    }
}

void GameLayer::clickKeyboardButtonP2(ActionButton* button) {
    // 复用 clickKeyboardButton() 的逻辑
    if(!button) return;
    button->click();
}
```

---

### Task 4: 修改 main.cpp 添加P2键盘分发

**文件：**
- Modify: `proj.win32/main.cpp`

**Step 1: 添加 DualGameLayer 头文件引用**

```cpp
// 在 main.cpp 顶部添加:
#include "DualGameLayer.h"
```

**Step 2: 修改 getRunningGameLayer() 支持 DualGameLayer**

```cpp
static GameLayer* getRunningGameLayer()
{
    CCScene* scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return NULL;

    CCArray* children = scene->getChildren();
    if (!children) return NULL;

    CCObject* object = NULL;
    CCARRAY_FOREACH(children, object)
    {
        CCNode* node = (CCNode*)object;

        // 新增: 检查是否是 DualGameLayer
        DualGameLayer* dualLayer = dynamic_cast<DualGameLayer*>(node);
        if (dualLayer && dualLayer->_gameLayer1)
        {
            return dualLayer->_gameLayer1;
        }

        // 原有逻辑
        if (node && node->getZOrder() == GlTag)
        {
            GameLayer* gl = dynamic_cast<GameLayer*>(node);
            if (gl) return gl;
        }
    }

    return NULL;
}
```

**Step 3: 在 gameWindowProc() 中添加P2键盘分发**

```cpp
// 在 gameWindowProc() 中，原有 gameLayer->handleKeyboard() 之后添加:

        // P1 键盘处理 (原有)
        if (gameLayer->handleKeyboard(keyCode, isPressed))
        {
            *pProcessed = TRUE;
        }

        // P2 键盘处理 (新增)
        if (gameLayer->handleKeyboardP2(keyCode, isPressed))
        {
            *pProcessed = TRUE;
        }
```

**Step 4: 在 WM_KILLFOCUS 处理中添加P2重置**

```cpp
        if (gameLayer)
        {
            gameLayer->resetKeyboardControl();
            gameLayer->resetKeyboardControlP2();  // 新增
        }
```

---

### Task 5: 创建 DualSelectLayer (双人选角界面)

**文件：**
- Create: `Classes/DualSelectLayer.h`
- Create: `Classes/DualSelectLayer.cpp`

**复用清单：**
- `SelectButton` (SkillLayer.h) — 英雄选择按钮
- `CCTips` (KTools.h) — 通知弹窗
- `selectList[]` 英雄列表模式 (NetworkLayer.cpp:221)
- `tempHeros` 构建模式 (SelectLayer.cpp:277)
- `CCSpriteFrameCache` plist加载模式 (SelectLayer.cpp:38-45)
- `CCTransitionFade` 场景切换 (SelectLayer.cpp:408)

**DualSelectLayer.h：**

```cpp
#pragma once
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "LoadLayer.h"
#include "SkillLayer.h"    // 复用 SelectButton
#include "MyUtils/KTools.h" // 复用 CCTips

USING_NS_CC;

class SelectButton;  // 前向声明

enum SelectPhase {
    PHASE_P1_SELECT,
    PHASE_P2_SELECT,
    PHASE_ALL_DONE
};

class DualSelectLayer : public CCLayer {
public:
    DualSelectLayer(void);
    ~DualSelectLayer(void);

    virtual bool init();
    static DualSelectLayer* create();

    // 选角状态
    SelectPhase _phase;
    const char* _p1SelectHero;
    const char* _p2SelectHero;

    // UI元素
    CCSprite* _p1HeroHalf;
    CCSprite* _p1HeroName;
    CCSprite* _p2HeroHalf;
    CCSprite* _p2HeroName;

    CCArray* selectArray;

    // 被 SelectButton::click() 调用 (复用SelectButton的分发机制)
    void setSelected(CCObject* sender);

    virtual void keyBackClicked();

private:
    bool isStart;
    void onGameStart();
    void updatePhaseDisplay();
};
```

**DualSelectLayer.cpp 核心实现：**

```cpp
#include "DualSelectLayer.h"
#include "StartMenu.h"
#include <time.h>

DualSelectLayer::DualSelectLayer(void) {
    _phase = PHASE_P1_SELECT;
    _p1SelectHero = NULL;
    _p2SelectHero = NULL;
    _p1HeroHalf = NULL;
    _p1HeroName = NULL;
    _p2HeroHalf = NULL;
    _p2HeroName = NULL;
    selectArray = NULL;
    isStart = false;
}

DualSelectLayer::~DualSelectLayer(void) {
    CC_SAFE_RELEASE(selectArray);
}

DualSelectLayer* DualSelectLayer::create() {
    DualSelectLayer* sl = new DualSelectLayer();
    if (sl && sl->init()) {
        sl->autorelease();
        return sl;
    }
    delete sl;
    return NULL;
}

bool DualSelectLayer::init() {
    bool bRet = false;
    do {
        CC_BREAK_IF(!CCLayer::init());

        // 复用: plist加载 (与 SelectLayer::init() 相同)
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Record.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Record2.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("UI.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Report.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Ougis.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Ougis2.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Map.plist");
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Gears.plist");

        // 复用: 背景 (与 SelectLayer::init() 相同)
        CCSprite* bgSprite = CCSprite::create("red_bg.png");
        bgSprite->setAnchorPoint(ccp(0,0));
        bgSprite->setPosition(ccp(0,0));
        this->addChild(bgSprite, -5);

        // 复用: 顶部/底部菜单栏 (与 SelectLayer::init() 相同)
        CCSprite* menu_bar_t = CCSprite::create("menu_bar3.png");
        menu_bar_t->setAnchorPoint(ccp(0,0));
        menu_bar_t->setPosition(ccp(0, winSize.height-menu_bar_t->getContentSize().height));
        this->addChild(menu_bar_t, 2);

        CCSprite* menu_bar_b = CCSprite::create("menu_bar2.png");
        menu_bar_b->setAnchorPoint(ccp(0,0));
        this->addChild(menu_bar_b, 2);

        // 复用: 标题精灵
        CCSprite* select_title = CCSprite::createWithSpriteFrameName("select_title.png");
        select_title->setAnchorPoint(ccp(0,0));
        select_title->setPosition(ccp(2, winSize.height-select_title->getContentSize().height-2));
        this->addChild(select_title, 3);

        // 复用: 英雄列表 (来自 NetworkLayer.cpp:221 的 selectList)
        selectArray = CCArray::create();
        char* selectList[] = {
            "Naruto","Sakura","Sai","Kakashi",
            "Shikamaru","Ino","Choji","Asuma",
            "Kiba","Hinata","Shino",
            "Neji","Tenten","Lee",
            "Tobirama","Hiruzen","Minato",
            "Jiraiya","Tsunade","Orochimaru",
            "Deidara","Kakuzu","Hidan",
            "Tobi","Konan","Pain",
            "Itachi","Kisame",
            "Sasuke","Karin","Suigetsu","Jugo",
            "Gaara","Kankuro","Chiyo"
        };

        int num = sizeof(selectList) / sizeof(char*);
        for(int i = 0; i < num; i++) {
            // 复用: SelectButton 创建 (与 NetworkLayer::init() 相同)
            CCString* path = CCString::createWithFormat("%s_select.png", selectList[i]);
            SelectButton* select_btn = SelectButton::create(path->getCString());
            select_btn->setDelegate4(this);  // 新增: 指向 DualSelectLayer
            select_btn->setCharName(CCString::create(selectList[i]));

            // 复用: 网格布局 (与 NetworkLayer::init() 相同)
            int Column = i % 7;
            int Row = i / 7;
            select_btn->setPosition(ccp(
                winSize.width/2-36+(Column-1)*27+Column/4*10,
                winSize.height-112-(72*Row)
            ));
            this->addChild(select_btn, 2);
            selectArray->addObject(select_btn);
        }
        selectArray->retain();

        // P1选择框 (左下角)
        _p1HeroHalf = CCSprite::createWithSpriteFrameName("Naruto_half.png");
        _p1HeroHalf->setAnchorPoint(ccp(0,0));
        _p1HeroHalf->setPosition(ccp(10, 10));
        this->addChild(_p1HeroHalf, 1);

        _p1HeroName = CCSprite::createWithSpriteFrameName("Naruto_font.png");
        _p1HeroName->setAnchorPoint(ccp(0.5f, 0));
        _p1HeroName->setPosition(ccp(100, 20));
        this->addChild(_p1HeroName, 5);

        // P2选择框 (右下角)
        _p2HeroHalf = CCSprite::createWithSpriteFrameName("unknow_select.png");
        _p2HeroHalf->setAnchorPoint(ccp(0,0));
        _p2HeroHalf->setPosition(ccp(winSize.width-130, 10));
        this->addChild(_p2HeroHalf, 1);

        // P1/P2标签
        CCLabelTTF* p1Label = CCLabelTTF::create("P1", FONT_TYPE, 14);
        p1Label->setPosition(ccp(70, 120));
        p1Label->setColor(ccc3(0, 200, 255));
        this->addChild(p1Label, 10);

        CCLabelTTF* p2Label = CCLabelTTF::create("P2", FONT_TYPE, 14);
        p2Label->setPosition(ccp(winSize.width-70, 120));
        p2Label->setColor(ccc3(255, 200, 0));
        this->addChild(p2Label, 10);

        // 提示文字
        CCLabelTTF* hintLabel = CCLabelTTF::create("P1: Click to select", FONT_TYPE, 12);
        hintLabel->setPosition(ccp(winSize.width/2, 5));
        hintLabel->setTag(100);
        this->addChild(hintLabel, 10);

        // 复用: 音乐 (与 SelectLayer::init() 相同)
        if(CCUserDefault::sharedUserDefault()->getBoolForKey("isBGM")!=false) {
            SimpleAudioEngine::sharedEngine()->playBackgroundMusic("Audio/Music/select_music.mp3", true);
        }

        if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) {
            this->setKeypadEnabled(true);
        }

        bRet = true;
    } while(0);
    return bRet;
}

void DualSelectLayer::setSelected(CCObject* sender) {
    // 被 SelectButton::click() 调用 (复用SelectButton的分发机制)
    SelectButton* btn = (SelectButton*)sender;
    const char* heroName = btn->getCharName()->getCString();

    if(_phase == PHASE_P1_SELECT) {
        _p1SelectHero = heroName;

        // 复用: 更新半身像 (与 SelectLayer::setSelected() 相同)
        _p1HeroHalf->removeFromParent();
        const char* path = CCString::createWithFormat("%s_half.png", heroName)->getCString();
        _p1HeroHalf = CCSprite::createWithSpriteFrameName(path);
        _p1HeroHalf->setAnchorPoint(ccp(0,0));
        _p1HeroHalf->setPosition(ccp(10, 10));
        this->addChild(_p1HeroHalf, 1);

        _p1HeroName->removeFromParent();
        const char* namePath = CCString::createWithFormat("%s_font.png", heroName)->getCString();
        _p1HeroName = CCSprite::createWithSpriteFrameName(namePath);
        _p1HeroName->setAnchorPoint(ccp(0.5f, 0));
        _p1HeroName->setPosition(ccp(100, 20));
        this->addChild(_p1HeroName, 5);

        // 显示锁定标记
        CCSprite* mark = CCSprite::createWithSpriteFrameName("com_label2.png");
        mark->setPosition(ccp(70, 140));
        mark->setTag(200);
        this->addChild(mark, 10);

        _phase = PHASE_P2_SELECT;
        this->updatePhaseDisplay();

    } else if(_phase == PHASE_P2_SELECT) {
        // 复用: CCTips 通知 (与 NetworkLayer 中的错误提示相同)
        if(strcmp(heroName, _p1SelectHero) == 0) {
            CCTips* tip = CCTips::create("SameHero");
            this->addChild(tip, 5000);
            return;
        }

        _p2SelectHero = heroName;

        // 复用: 更新半身像
        _p2HeroHalf->removeFromParent();
        const char* path = CCString::createWithFormat("%s_half.png", heroName)->getCString();
        _p2HeroHalf = CCSprite::createWithSpriteFrameName(path);
        _p2HeroHalf->setAnchorPoint(ccp(0,0));
        _p2HeroHalf->setPosition(ccp(winSize.width-130, 10));
        this->addChild(_p2HeroHalf, 1);

        // 显示锁定标记
        CCSprite* mark = CCSprite::createWithSpriteFrameName("com_label2.png");
        mark->setPosition(ccp(winSize.width-70, 140));
        this->addChild(mark, 10);

        _phase = PHASE_ALL_DONE;
        this->updatePhaseDisplay();

        // 复用: 定时跳转 (与 SelectLayer::onGameStart() 中的 scheduleOnce 相同)
        this->scheduleOnce(schedule_selector(DualSelectLayer::onGameStart), 1.0f);
    }
}

void DualSelectLayer::updatePhaseDisplay() {
    CCLabelTTF* hintLabel = (CCLabelTTF*)this->getChildByTag(100);
    if(!hintLabel) return;

    switch(_phase) {
    case PHASE_P1_SELECT:
        hintLabel->setString("P1: Click to select");
        break;
    case PHASE_P2_SELECT:
        hintLabel->setString("P2: Click to select");
        break;
    case PHASE_ALL_DONE:
        hintLabel->setString("Starting...");
        break;
    }
}

void DualSelectLayer::onGameStart() {
    if(isStart) return;
    isStart = true;

    // 复用: 音效 (与 SelectLayer::onGameStart() 相同)
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");

    // 复用: heroList (来自 NetworkLayer.cpp:872)
    char* heroList[] = {
        "Konan","Sakura","Naruto","Sai","Deidara",
        "Kakashi","Itachi","Tenten","Jiraiya","Suigetsu",
        "Tsunade","Tobirama","Neji","Ino","Asuma","Gaara",
        "Karin","Sasuke","Hidan","Choji","Kankuro",
        "Shino","Minato","Tobi","Kakuzu","Hinata",
        "Shikamaru","Chiyo","Kisame",
        "Hiruzen","Kiba","Jugo","Lee"
    };
    int num = sizeof(heroList) / sizeof(char*);

    // 排除已选英雄
    CCArray* realHero = CCArray::create();
    for(int i = 0; i < num; i++) {
        if(strcmp(_p1SelectHero, heroList[i]) == 0) continue;
        if(strcmp(_p2SelectHero, heroList[i]) == 0) continue;
        realHero->addObject(CCString::create(heroList[i]));
    }

    // 复用: tempHeros 构建模式 (与 SelectLayer::onGameStart() 相同)
    CCArray* tempHeros = CCArray::create();

    // P1 (Player, Konoha)
    CCDictionary* dic = CCDictionary::create();
    dic->setObject(CCString::create(_p1SelectHero), "character");
    dic->setObject(CCString::create("Player"), "role");
    dic->setObject(CCString::create("Konoha"), "group");
    tempHeros->addObject(dic);

    // P2 (Player2, Konoha)
    dic = CCDictionary::create();
    dic->setObject(CCString::create(_p2SelectHero), "character");
    dic->setObject(CCString::create("Player2"), "role");
    dic->setObject(CCString::create("Konoha"), "group");
    tempHeros->addObject(dic);

    // 木叶AI队友 (复用: 随机选择模式)
    srand((int)time(0));
    int idx = random(realHero->count());
    if(idx >= (int)realHero->count()) idx = realHero->count()-1;
    CCString* hero = (CCString*)realHero->objectAtIndex(idx);
    dic = CCDictionary::create();
    dic->setObject(CCString::create(hero->getCString()), "character");
    dic->setObject(CCString::create("Com"), "role");
    dic->setObject(CCString::create("Konoha"), "group");
    tempHeros->addObject(dic);
    realHero->removeObjectAtIndex(idx);

    // 晓AI敌方 (复用: 随机选择模式)
    for(int i = 0; i < 3; i++) {
        idx = random(realHero->count());
        if(idx >= (int)realHero->count()) idx = realHero->count()-1;
        hero = (CCString*)realHero->objectAtIndex(idx);
        dic = CCDictionary::create();
        dic->setObject(CCString::create(hero->getCString()), "character");
        dic->setObject(CCString::create("Com"), "role");
        dic->setObject(CCString::create("Akatsuki"), "group");
        tempHeros->addObject(dic);
        realHero->removeObjectAtIndex(idx);
    }

    // 复用: 场景切换 (与 SelectLayer::onGameStart() 相同)
    CCScene* loadScene = CCScene::create();
    LoadLayer* loadLayer = LoadLayer::create();
    loadLayer->tempHeros = tempHeros;
    loadLayer->_isHardCoreMode = false;
    loadLayer->_isLocalPvP = true;
    loadScene->addChild(loadLayer);
    loadLayer->preloadAudio();
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.5f, loadScene));
}

void DualSelectLayer::keyBackClicked() {
    // 复用: 返回主菜单 (与 SelectLayer::keyBackClicked() 相同)
    SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/cancel.ogg");
    CCScene* menuScene = CCScene::create();
    CCLayer* menuLayer = StartMenu::create();
    menuScene->addChild(menuLayer);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.5f, menuScene));
}
```

---

### Task 6: 创建 DualGameLayer (分屏容器)

**文件：**
- Create: `Classes/DualGameLayer.h`
- Create: `Classes/DualGameLayer.cpp`

**复用清单：**
- `GameLayer` — 完整游戏逻辑（initHeros, initTower, initFlogs, handleKeyboard, onGameStart等）
- `BGLayer` — 地图背景
- `HudLayer` — HUD界面（initHeroInterface, onKaichang等）
- `LoadLayer::onLoadFinish()` 中的场景组装模式

**DualGameLayer.h：**

```cpp
#pragma once
#include "cocos2d.h"
#include "GameLayer.h"
#include "BGLayer.h"
#include "HudLayer.h"

USING_NS_CC;

class DualGameLayer : public CCLayer {
public:
    DualGameLayer(void);
    ~DualGameLayer(void);

    bool init();
    CREATE_FUNC(DualGameLayer);

    GameLayer*      _gameLayer1;    // P1的游戏层 (复用GameLayer全部逻辑)
    GameLayer*      _gameLayer2;    // P2的游戏层 (复用GameLayer全部逻辑)
    HudLayer*       _hudLayer1;     // P1的HUD (复用HudLayer)
    HudLayer*       _hudLayer2;     // P2的HUD (复用HudLayer)

    CCArray*        tempHeros;
    bool            _isHardCoreMode;

    void initGame();
    void updateP2View(float dt);

protected:
    virtual void onEnter();
    virtual void onExit();
};
```

**DualGameLayer.cpp：**

```cpp
#include "DualGameLayer.h"
#include "Defines.h"

DualGameLayer::DualGameLayer(void) {
    _gameLayer1 = NULL;
    _gameLayer2 = NULL;
    _hudLayer1 = NULL;
    _hudLayer2 = NULL;
    tempHeros = NULL;
    _isHardCoreMode = false;
}

DualGameLayer::~DualGameLayer(void) {}

bool DualGameLayer::init() {
    bool bRet = false;
    do {
        CC_BREAK_IF(!CCLayer::init());
        bRet = true;
    } while(0);
    return bRet;
}

void DualGameLayer::initGame() {
    float halfWidth = winSize.width / 2;

    // === P1的游戏层 (复用: LoadLayer::onLoadFinish() 的组装模式) ===
    _gameLayer1 = GameLayer::create();
    _gameLayer1->Heros = tempHeros;
    _gameLayer1->_isHardCoreGame = _isHardCoreMode;
    _gameLayer1->_isLocalPvP = true;
    _gameLayer1->initHeros();  // 复用: 完整的角色/地图/塔/特效初始化

    BGLayer* bgLayer1 = BGLayer::create();
    bgLayer1->setDelegate(_gameLayer1);  // 复用: 背景初始化

    _hudLayer1 = HudLayer::create();
    _hudLayer1->setDelegate(_gameLayer1);
    _gameLayer1->setHudLayer(_hudLayer1);
    _gameLayer1->setTotalKills(CCString::create("0"));
    _gameLayer1->setTotalTM(CCString::create("0"));

    // === P2的游戏层 (复用: 同样的组装模式) ===
    _gameLayer2 = GameLayer::create();
    _gameLayer2->Heros = tempHeros;
    _gameLayer2->_isHardCoreGame = _isHardCoreMode;
    _gameLayer2->_isLocalPvP = true;
    _gameLayer2->initHeros();

    BGLayer* bgLayer2 = BGLayer::create();
    bgLayer2->setDelegate(_gameLayer2);

    _hudLayer2 = HudLayer::create();
    _hudLayer2->setDelegate(_gameLayer2);
    _gameLayer2->setHudLayer(_hudLayer2);
    _gameLayer2->setTotalKills(CCString::create("0"));
    _gameLayer2->setTotalTM(CCString::create("0"));

    // P1的GameLayer也需要知道P2的HUD (用于P2键盘输入)
    _gameLayer1->setHudLayer(_hudLayer1);  // 确保P1的HUD正确

    // === 分屏裁剪 ===
    // 左半屏 (复用: CCClippingNode 裁剪模式)
    CCClippingNode* clipLeft = CCClippingNode::create();
    CCDrawNode* stencilLeft = CCDrawNode::create();
    CCPoint ptsL[] = {ccp(0,0), ccp(halfWidth,0), ccp(halfWidth,winSize.height), ccp(0,winSize.height)};
    stencilLeft->drawPolygon(ptsL, 4, ccc4f(1,1,1,1), 0, ccc4f(1,1,1,1));
    clipLeft->setStencil(stencilLeft);
    clipLeft->addChild(bgLayer1);
    clipLeft->addChild(_gameLayer1);

    // 右半屏
    CCClippingNode* clipRight = CCClippingNode::create();
    CCDrawNode* stencilRight = CCDrawNode::create();
    CCPoint ptsR[] = {ccp(halfWidth,0), ccp(winSize.width,0), ccp(winSize.width,winSize.height), ccp(halfWidth,winSize.height)};
    stencilRight->drawPolygon(ptsR, 4, ccc4f(1,1,1,1), 0, ccc4f(1,1,1,1));
    clipRight->setStencil(stencilRight);
    clipRight->addChild(bgLayer2);
    clipRight->addChild(_gameLayer2);

    // 添加到场景 (复用: GlTag, BgTag, HudTag 层级)
    this->addChild(clipLeft, BgTag);
    this->addChild(clipRight, BgTag+1);
    this->addChild(_hudLayer1, HudTag);
    this->addChild(_hudLayer2, HudTag+1);

    // 分割线
    CCDrawNode* divider = CCDrawNode::create();
    divider->drawSegment(ccp(halfWidth,0), ccp(halfWidth,winSize.height), 1.5f, ccc4f(1,1,1,0.8f));
    this->addChild(divider, 1000);

    // 启动P2视角同步
    this->schedule(schedule_selector(DualGameLayer::updateP2View), 0.0f);
}

void DualGameLayer::updateP2View(float dt) {
    // 复用: updateViewPoint() 的摄像机逻辑，但跟踪P2角色
    if(!_gameLayer2->currentPlayer || !_gameLayer2->currentMap) return;

    Hero* p2 = _gameLayer2->currentPlayer;
    float halfWidth = winSize.width / 2;

    CCPoint playerPoint = p2->getPosition();
    int x = MAX(playerPoint.x, halfWidth/2);
    int y = MAX(playerPoint.y, winSize.width/2);
    x = MIN(x, (_gameLayer2->currentMap->getMapSize().width *
                _gameLayer2->currentMap->getTileSize().width) - halfWidth/2);
    y = MIN(y, (_gameLayer2->currentMap->getMapSize().height *
                _gameLayer2->currentMap->getTileSize().height) - winSize.height/2);

    CCPoint actualPoint = ccp(x, y);
    CCPoint centerPoint = ccp(halfWidth/2, y);
    CCPoint viewPoint = ccpSub(centerPoint, actualPoint);

    _gameLayer2->setPosition(ccp(viewPoint.x - halfWidth, viewPoint.y));
}

void DualGameLayer::onEnter() {
    CCLayer::onEnter();
}

void DualGameLayer::onExit() {
    CCLayer::onExit();
}
```

---

### Task 7: 修改 LoadLayer 支持本地联机

**文件：**
- Modify: `Classes/LoadLayer.h`
- Modify: `Classes/LoadLayer.cpp`

**Step 1: 在 LoadLayer.h 中添加标志**

```cpp
// 在 LoadLayer.h 的 public 区域添加:
bool _isLocalPvP;
```

**Step 2: 在 LoadLayer.cpp 构造函数中初始化**

```cpp
// 在 LoadLayer::LoadLayer() 中添加:
_isLocalPvP = false;
```

**Step 3: 在 LoadLayer.cpp 中添加头文件**

```cpp
// 在 LoadLayer.cpp 顶部添加:
#include "DualGameLayer.h"
```

**Step 4: 修改 onLoadFinish() 支持双人模式**

```cpp
void LoadLayer::onLoadFinish(float dt){
    if(_isLocalPvP) {
        // 复用: 场景创建模式 (与原有逻辑相同)
        CCScene* gameScene = CCScene::create();

        DualGameLayer* dualLayer = DualGameLayer::create();
        dualLayer->tempHeros = tempHeros;
        dualLayer->_isHardCoreMode = _isHardCoreMode;
        dualLayer->initGame();

        gameScene->addChild(dualLayer, GlTag);

        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, gameScene));
    } else {
        // 原有单机逻辑 (不变)
        CCScene* gameScene = CCScene::create();
        _gameLayer = GameLayer::create();
        _gameLayer->Heros = tempHeros;
        _gameLayer->_isHardCoreGame = _isHardCoreMode;
        _gameLayer->_isRandomChar = _isRandomChar;
        _gameLayer->initHeros();
        _bgLayer = BGLayer::create();
        _bgLayer->setDelegate(_gameLayer);
        _hudLayer = HudLayer::create();
        _hudLayer->setDelegate(_gameLayer);
        _gameLayer->setHudLayer(_hudLayer);
        _gameLayer->setTotalKills(CCString::create("0"));
        _gameLayer->setTotalTM(CCString::create("0"));
        gameScene->addChild(_gameLayer, GlTag);
        gameScene->addChild(_bgLayer, BgTag);
        gameScene->addChild(_hudLayer, HudTag);
        CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, gameScene));
    }
}
```

---

### Task 8: 修改 StartMenu 添加入口

**文件：**
- Modify: `Classes/StartMenu.cpp`

**Step 1: 在 init() 中添加本地联机按钮**

```cpp
// 在 StartMenu::init() 中，现有按钮创建代码之后添加:

// 复用: MenuButton 创建模式 (与 training_btn 相同)
MenuButton* localPvP_btn = MenuButton::create("menu02.png");  // 复用训练模式按钮图片
localPvP_btn->setDelegate(this);
localPvP_btn->setBtnType(LocalPvP);
localPvP_btn->setScale(0.5f);
localPvP_btn->setPositionY(_pos01);  // 放在exit按钮位置
localPvP_btn->_isTop = true;
_menu_array->addObject(localPvP_btn);

// 调整exit按钮位置
// exit_btn->setPositionY(_pos01 - 44);  // 或其他合适位置
```

**Step 2: 在 MenuButton::ccTouchEnded() 中添加 LocalPvP 分支**

```cpp
// 在 StartMenu.cpp 的 MenuButton::ccTouchEnded() 中添加:
case LocalPvP:
    SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
    _delegate->onLocalPvPCallBack();
    break;
```

**Step 3: 添加 onLocalPvPCallBack() 实现**

```cpp
// 复用: 场景切换模式 (与 onTrainingCallBack() 相同)
void StartMenu::onLocalPvPCallBack() {
    CCScene* selectScene = CCScene::create();
    DualSelectLayer* selectLayer = DualSelectLayer::create();
    selectScene->addChild(selectLayer);
    CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.5f, selectScene));
}
```

**Step 4: 在 StartMenu.cpp 顶部添加头文件**

```cpp
#include "DualSelectLayer.h"
```

---

## 三、P2视角分屏的关键技术细节

### 3.1 P2的GameLayer中 currentPlayer 指向

P2的GameLayer（`_gameLayer2`）中 `currentPlayer` 由 `initHeros()` 自动设置为索引0的角色。但在本地联机模式中，索引0是P1的英雄，索引1才是P2的英雄。

**解决方案：** 在 `initHeros()` 中，当 `_isLocalPvP` 且 `i==1` 时，除了创建 `player2`，还需要将 `_gameLayer2->currentPlayer` 指向P2的角色。

在 `DualGameLayer::initGame()` 中，`_gameLayer2->initHeros()` 调用后：
```cpp
// 确保P2的GameLayer的currentPlayer指向P2的角色
if(_gameLayer2->player2) {
    _gameLayer2->currentPlayer = _gameLayer2->player2;
}
```

### 3.2 P2视角的摄像机偏移

P2的GameLayer需要向左偏移半屏宽度，使得右半部分对齐窗口右侧：
```cpp
_gameLayer2->setPositionX(-halfWidth);
```

### 3.3 P2的HUD位置镜像

P2的HudLayer需要将UI元素镜像到右侧。最简方案：在 `initHeroInterface()` 调用后，手动调整关键元素位置。

---

## 四、风险与注意事项

1. **SelectButton的_delegate4修改** — 这是唯一对现有类接口的修改。改动很小（添加一个字段和一个分支），不影响现有功能。

2. **两个GameLayer独立运行** — 两个GameLayer各自运行完整的update循环，包括AI、小兵、塔等。这意味着游戏逻辑运行两次。如果性能有问题，可以考虑让P2的GameLayer只做渲染不做逻辑。

3. **游戏结束同步** — 两个GameLayer可能在不同时间触发 onGameOver()。需要在DualGameLayer层统一处理。

4. **P2的HUD中按钮引用** — P2的HudLayer中 `nAttackButton`、`skill1Button` 等按钮是独立的实例。P2键盘输入需要操作P2的按钮，不是P1的。

5. **资源重复加载** — 两个GameLayer都调用 `initHeros()` 会重复加载plist资源。Cocos2d-x的CCSpriteFrameCache是全局缓存，重复加载不会出错，只是多了一次文件读取。
