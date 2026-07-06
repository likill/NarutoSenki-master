#include "DualGameLayer.h"
#include "Characters.h"
#include "CCEGLView.h"

DualGameLayer::DualGameLayer(void)
{
	tempHeros=NULL;
	_isHardCoreMode=false;
	_gameLayer=NULL;
	_bgLayer=NULL;
	_hudLayer1=NULL;
	_hudLayer2=NULL;
	_divider=NULL;
}

DualGameLayer::~DualGameLayer(void)
{
}

bool DualGameLayer::init()
{
	bool bRet=false;
	do {
		CC_BREAK_IF(!CCLayer::init());
		bRet=true;
	} while(0);
	return bRet;
}

void DualGameLayer::initGame()
{
	_gameLayer=GameLayer::create();
	_gameLayer->Heros=tempHeros;
	_gameLayer->_isHardCoreGame=_isHardCoreMode;
	_gameLayer->_isLocalPvP=true;
	_gameLayer->initHeros();

	_bgLayer=BGLayer::create();
	_bgLayer->setDelegate(_gameLayer);

	_hudLayer1=HudLayer::create();
	_hudLayer1->setDelegate(_gameLayer);
	_hudLayer2=HudLayer::create();
	_hudLayer2->setDelegate(_gameLayer);

	_gameLayer->setHudLayer(_hudLayer1);
	_gameLayer->setLocalPvPHudLayers(_hudLayer1,_hudLayer2);
	_gameLayer->setTotalKills(CCString::create("0"));
	_gameLayer->setTotalTM(CCString::create("0"));

	this->addChild(_bgLayer,BgTag);
	this->addChild(_gameLayer,GlTag);
	this->addChild(_hudLayer1,HudTag);
	this->addChild(_hudLayer2,HudTag+1);

	_divider=CCLayerColor::create(ccc4(255,255,255,180),2,winSize.height);
	_divider->setPosition(ccp(winSize.width/2-1,0));
	this->addChild(_divider,1000);

	this->schedule(schedule_selector(DualGameLayer::syncHud),0.2f);
}

GameLayer* DualGameLayer::getGameLayer()
{
	return _gameLayer;
}

bool DualGameLayer::handleKeyboard(unsigned int keyCode,bool isPressed)
{
	if(!_gameLayer) {
		return false;
	}

	if(_gameLayer->handleKeyboardP2(keyCode,isPressed)) {
		return true;
	}

	return _gameLayer->handleKeyboard(keyCode,isPressed);
}

void DualGameLayer::resetKeyboardControl()
{
	if(!_gameLayer) {
		return;
	}
	_gameLayer->resetKeyboardControl();
	_gameLayer->resetKeyboardControlP2();
}

void DualGameLayer::syncHud(float dt)
{
	if(_gameLayer) {
		_gameLayer->syncLocalPvPHuds(dt);
	}
}

void DualGameLayer::drawGameView(float viewportX,float viewportWidth,Hero* target)
{
	if(!_gameLayer || !_gameLayer->currentMap || !target) {
		return;
	}

	CCDirector::sharedDirector()->getOpenGLView()->setScissorInPoints(viewportX,0,viewportWidth,winSize.height);
	glEnable(GL_SCISSOR_TEST);

	if(_bgLayer) {
		_bgLayer->setPositionX(viewportX);
		_bgLayer->visit();
	}

	CCPoint playerPoint;
	if(_gameLayer->ougisChar) {
		playerPoint=_gameLayer->ougisChar->getPosition();
	} else if(_gameLayer->controlChar) {
		playerPoint=_gameLayer->controlChar->getPosition();
	} else {
		playerPoint=target->getPosition();
	}

	float mapWidth=_gameLayer->currentMap->getMapSize().width*_gameLayer->currentMap->getTileSize().width;
	float mapHeight=_gameLayer->currentMap->getMapSize().height*_gameLayer->currentMap->getTileSize().height;
	float x=MAX(playerPoint.x,viewportWidth/2);
	float y=MAX(playerPoint.y,winSize.height/2);
	x=MIN(x,mapWidth-viewportWidth/2);
	y=MIN(y,mapHeight-winSize.height/2);

	CCPoint actualPoint=ccp(x,y);
	CCPoint centerPoint=ccp(viewportX+viewportWidth/2,y);
	CCPoint viewPoint=ccpSub(centerPoint,actualPoint);

	_gameLayer->setPosition(viewPoint);
	_gameLayer->visit();

	glDisable(GL_SCISSOR_TEST);
}

void DualGameLayer::visit()
{
	if(!m_bVisible) {
		return;
	}

	kmGLPushMatrix();
	this->transform();
	this->draw();

	float halfWidth=winSize.width/2;
	if(_gameLayer) {
		this->drawGameView(0,halfWidth,_gameLayer->player1 ? _gameLayer->player1 : _gameLayer->currentPlayer);
		this->drawGameView(halfWidth,halfWidth,_gameLayer->player2 ? _gameLayer->player2 : _gameLayer->currentPlayer);
	}

	if(_hudLayer1) {
		_hudLayer1->visit();
	}
	if(_hudLayer2) {
		_hudLayer2->visit();
	}
	if(_divider) {
		_divider->visit();
	}

	kmGLPopMatrix();
}
