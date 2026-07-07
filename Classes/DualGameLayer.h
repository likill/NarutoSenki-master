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

	CCArray* tempHeros;
	bool _isHardCoreMode;
	bool _isLocalCoop;

	void initGame();
	bool handleKeyboard(unsigned int keyCode,bool isPressed);
	void resetKeyboardControl();
	GameLayer* getGameLayer();

	virtual void visit();

protected:
	void drawGameView(float viewportX,float viewportWidth,Hero* target);
	void syncHud(float dt);

	GameLayer* _gameLayer;
	BGLayer* _bgLayer;
	HudLayer* _hudLayer1;
	HudLayer* _hudLayer2;
	CCLayerColor* _divider;
};
