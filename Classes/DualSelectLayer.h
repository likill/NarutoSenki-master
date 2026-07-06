#pragma once

#include "cocos2d.h"
#include "Defines.h"
#include "SimpleAudioEngine.h"
#include "cocos-ext.h"
#include "LoadLayer.h"
#include "SkillLayer.h"

USING_NS_CC;

class DualSelectLayer : public CCLayer {
public:
	DualSelectLayer(void);
	~DualSelectLayer(void);

	bool init();
	CREATE_FUNC(DualSelectLayer);

	void setSelected(CCObject* sender);
	void keyBackClicked();

protected:
	enum SelectPhase {
		PHASE_P1_SELECT,
		PHASE_P2_SELECT,
		PHASE_ALL_DONE
	};

	void onPage1Btn(CCObject* sender);
	void onPage2Btn(CCObject* sender);
	void onPage3Btn(CCObject* sender);
	void onGameStart(float dt);
	void updatePhaseDisplay();
	void addHero(CCArray* heros, const char* character, const char* role, const char* group);
	CCString* takeRandomHero(CCArray* heros);

	CCArray* selectArray;
	CCArray* _selectList;
	CCLayer* _pageLayer1;
	CCLayer* _pageLayer2;
	CCLayer* _pageLayer3;
	CCMenuItemSprite* page1_btn;
	CCMenuItemSprite* page2_btn;
	CCMenuItemSprite* page3_btn;

	CCSprite* _p1HeroHalf;
	CCSprite* _p1HeroName;
	CCSprite* _p2HeroHalf;
	CCSprite* _p2HeroName;
	CCSprite* _selectImg;
	CCLabelTTF* _phaseLabel;

	const char* _p1SelectHero;
	const char* _p2SelectHero;
	SelectPhase _phase;
	bool isStart;
};
