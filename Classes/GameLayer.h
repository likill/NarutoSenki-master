#pragma once
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Defines.h"
#include "cocos-ext.h"
#include "PauseLayer.h"
#include "GearLayer.h"
#include "GameOver.h"

#include <vector>


#if (CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID) 
#include "../cocos2dx/platform/android/jni/JniHelper.h"  
#endif  

USING_NS_CC;




class Hero;
class BGLayer;
class HudLayer;
class ActionButton;

class GameLayer :public CCLayer{
public:
	GameLayer(void);
	~GameLayer(void);

	CCTMXTiledMap*		currentMap;
	Hero*				currentPlayer;
	Hero*				player1;
	Hero*				player2;
	CCPoint				spawnPoint;
	
	unsigned int _second;
	unsigned int _minute;
	int randomMap;

	const char* kName;
	const char* aName;
    int kEXPBound;
	int aEXPBound;

	CCArray* Heros;
	bool	_isAttackButtonRelease;
	bool	_isGuardian;
	CCArray* _KonohaFlogArray;
	CCArray* _AkatsukiFlogArray;
	CCArray* _TowerArray;
	CCArray* _CharacterArray;

	
	bool _isShacking;

	int _playNum;
	void checkBackgroundMusic(float dt);


	CC_SYNTHESIZE(HudLayer*,_hudLayer,HudLayer);

	CC_SYNTHESIZE_RETAIN(CCString*,totalKills,TotalKills);
	CC_SYNTHESIZE_RETAIN(CCString*,totalTM,TotalTM);

	CCSpriteBatchNode* skillEffectBatch;
	CCSpriteBatchNode* damageEffectBatch;
	CCSpriteBatchNode* bulletBatch;
	CCSpriteBatchNode* shadowBatch;

	bool isPosting;
	int postTime;

	
		
	bool init();
	void initTileMap();
	void initHeros();
	void initFlogs();
	void initTower();
	void initGard();
	void initEffects();

	//void update(float dt);
	void updateViewPoint(float dt);
	void updateGameTime(float dt);

	void addFlog(float dt);
	void onStart();
	
	void attackButtonClick(abType type);
	void gearButtonClick(gearType type);	
	void attackButtonRelease();
	bool handleKeyboard(unsigned int keyCode,bool isPressed);
	void resetKeyboardControl();
	void resumeKeyboardMove();
	bool handleKeyboardP2(unsigned int keyCode,bool isPressed);
	void resetKeyboardControlP2();
	void setLocalPvPHudLayers(HudLayer* p1Hud,HudLayer* p2Hud);
	void activatePlayerControl(Hero* player,HudLayer* hud);
	void restoreDefaultControl();
	void syncLocalPvPHuds(float dt);
	

	void JoyStickRelease();
	void JoyStickUpdate(CCPoint direction);

	CC_SYNTHESIZE(bool,_isSkillFinish,SkillFinish);
	void checkPost(float dt);
	void checkTower();
	void checkBackgroundMusic();

	void onPause();
	void onGear();
	void onKaichang(float dt);
	void onGameStart(float dt);
	void onGameOver(bool isWin);

	void setHPLose(float percent);
	void setCKRLose(bool isCRK2);

	void setReport(const char* name1, const char* name2,CCString* killNum);
	void clearDoubleClick();
	void resetStatusBar();
	void setCoin(const char* value);
	void removeOugisMark(int type);
	void setOugis(CCNode* sender);
	void removeOugis();
	CCNode* ougisChar;

	CCNode* controlChar;
	
	CCLayer* blend;

	void onLeft();


	bool _isSurrender;

	bool _isOugis2Game;
	bool _isHardCoreGame;
	bool _isRandomChar;
	bool _isLocalPvP;
	HudLayer* _p1HudLayer;
	HudLayer* _p2HudLayer;


	int zhenying;
	bool _isExiting;

	CREATE_FUNC(GameLayer);
	std::vector<char*> KeyList;
protected:
	void onHttpRequestCompleted(CCNode *sender ,void *data);
	virtual void	onEnter();
	virtual void	onExit();
	void	updateKeyboardMove();
	void	clickKeyboardButton(ActionButton* button);
	void	updateKeyboardMoveP2();
	void	clickKeyboardButtonP2(ActionButton* button);
	void	JoyStickReleaseP2();
	void	JoyStickUpdateP2(CCPoint direction);

	bool	_keyMoveUp;
	bool	_keyMoveDown;
	bool	_keyMoveLeft;
	bool	_keyMoveRight;
	bool	_keyAttack;
	bool	_keySkill1;
	bool	_keySkill2;
	bool	_keySkill3;
	bool	_keySkill4;
	bool	_keySkill5;
	bool	_keyItem1;
	bool	_keyGear06;
	bool	_keyGear00;
	bool	_keyGear03;

	bool	_p2_keyMoveUp;
	bool	_p2_keyMoveDown;
	bool	_p2_keyMoveLeft;
	bool	_p2_keyMoveRight;
	bool	_p2_keyAttack;
	bool	_p2_keySkill1;
	bool	_p2_keySkill2;
	bool	_p2_keySkill3;
	bool	_p2_keySkill4;
	bool	_p2_keySkill5;
	bool	_p2_keyItem1;
	bool	_p2_keyGear06;
	bool	_p2_keyGear00;
	bool	_p2_keyGear03;

};
