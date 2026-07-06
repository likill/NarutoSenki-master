#pragma once
#include "cocos2d.h"
#include <map>


//global parameter

#define winSize  CCDirector::sharedDirector()->getWinSize()
#define random(x) (rand()%x)


#define  FONT_TYPE "Î¢ÈíÑÅºÚ"

// layer's tag
#define BgTag	1
#define GlTag	2
#define HudTag	3


extern int Cheats;
extern int CError;
extern int adResult;
extern int MemberID;
extern int GroupID;
extern std::string CValue;
extern std::string PWord;
extern bool isPlayed;


#define SERVER "http://localhost/" 
#define CURRENT_VERSION 12200

//menu sound

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define SELECT_SOUND	"Audio/Menu/select.mp3"
#define TRAINING_SOUND	"Audio/Menu/training.ogg"
#define NETWORK_SOUND	"Audio/Menu/arcade.ogg"
#define CREDITS_SOUND	"Audio/Menu/credits.ogg"
#define EXIT_SOUND		"Audio/Menu/exit.ogg"
#else
#define SELECT_SOUND	"Audio/Menu/select.mp3"
#define TRAINING_SOUND	"Audio/Menu/training.ogg"
#define NETWORK_SOUND	"Audio/Menu/arcade.ogg"
#define CREDITS_SOUND	"Audio/Menu/credits.ogg"
#define EXIT_SOUND		"Audio/Menu/exit.ogg"
#endif


//tile map

#define S_MAP01	"Tiles/1.tmx"
#define S_MAP02	"Tiles/2.tmx"
#define S_MAP03	"Tiles/3.tmx"
#define S_MAP04	"Tiles/4.tmx"
#define S_MAP05 "Tiles/5.tmx"

// game layer tag
#define currentMapTag		-5000
#define currentShadowTag    -4000
#define currentCutTag		1000
#define currentcharacterTag	2
#define currentBulletTag	3
#define currentDamageTag	100
#define currentNumberTag	200
#define currentSkillTag		300
#define currentFontTag		400

enum abType{
	NAttack,
	Item1,
	GearItem,
	GearBtn,
	SKILL1,
	SKILL2,
	SKILL3,
	OUGIS1,
	OUGIS2
};


// declare GearButton 
enum gearType{
	gear00,
	gear01,
	gear02,
	gear03,
	gear04,
	gear05,
	gear06,
	gear07,
	gear08,
	None
};
// Local PvP - P2 keyboard codes (Windows virtual key codes).
#define P2_VK_UP             0x26
#define P2_VK_DOWN           0x28
#define P2_VK_LEFT           0x25
#define P2_VK_RIGHT          0x27
#define P2_VK_NUMPAD0        0x60
#define P2_VK_NUMPAD1        0x61
#define P2_VK_NUMPAD2        0x62
#define P2_VK_NUMPAD3        0x63
#define P2_VK_NUMPAD4        0x64
#define P2_VK_NUMPAD5        0x65
#define P2_VK_NUMPAD6        0x66
#define P2_VK_NUMPAD7        0x67
#define P2_VK_NUMPAD8        0x68
#define P2_VK_NUMPAD9        0x69
#define P2_VK_ADD            0x6B
#define P2_VK_OEM_PLUS       0xBB

