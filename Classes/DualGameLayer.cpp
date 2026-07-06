#include "DualGameLayer.h"
#include "Characters.h"
#include "CCEGLView.h"
#include "LocalPvPResolution.h"

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

static bool isCameraNodeOwnedByPlayer(CCNode* node,Hero* player)
{
	if(!node || !player) {
		return false;
	}

	ActionManager* actor=dynamic_cast<ActionManager*>(node);
	if(!actor) {
		return false;
	}

	return actor==player || actor->getMaster()==player || actor->getControler()==player || actor->getSecMaster()==player;
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
	LocalPvPResolution::applyLocalPvP();
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

	_divider=CCLayerColor::create(ccc4(255,255,255,180),2,LocalPvPResolution::kPvPSingleViewHeight);
	_divider->setPosition(ccp(LocalPvPResolution::kPvPSingleViewWidth-1,0));
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

	CCDirector::sharedDirector()->getOpenGLView()->setScissorInPoints(viewportX,0,viewportWidth,LocalPvPResolution::kPvPSingleViewHeight);
	glEnable(GL_SCISSOR_TEST);

	if(_bgLayer) {
		_bgLayer->setPositionX(viewportX);
		_bgLayer->visit();
	}

	CCPoint playerPoint;
	if(isCameraNodeOwnedByPlayer(_gameLayer->ougisChar,target)) {
		playerPoint=_gameLayer->ougisChar->getPosition();
	} else if(isCameraNodeOwnedByPlayer(_gameLayer->controlChar,target)) {
		playerPoint=_gameLayer->controlChar->getPosition();
	} else {
		playerPoint=target->getPosition();
	}

	float mapWidth=_gameLayer->currentMap->getMapSize().width*_gameLayer->currentMap->getTileSize().width;
	float mapHeight=_gameLayer->currentMap->getMapSize().height*_gameLayer->currentMap->getTileSize().height;
	float x=MAX(playerPoint.x,viewportWidth/2);
	float y=MAX(playerPoint.y,LocalPvPResolution::kPvPSingleViewHeight/2);
	x=MIN(x,mapWidth-viewportWidth/2);
	y=MIN(y,mapHeight-LocalPvPResolution::kPvPSingleViewHeight/2);

	CCPoint actualPoint=ccp(x,y);
	CCPoint centerPoint=ccp(viewportX+viewportWidth/2,y);
	CCPoint viewPoint=ccpSub(centerPoint,actualPoint);

	if(_gameLayer->blend) {
		_gameLayer->blend->setPosition(ccp(viewportX-viewPoint.x,0));
	}

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

	float viewWidth=LocalPvPResolution::kPvPSingleViewWidth;
	if(_gameLayer) {
		this->drawGameView(0,viewWidth,_gameLayer->player1 ? _gameLayer->player1 : _gameLayer->currentPlayer);
		this->drawGameView(viewWidth,viewWidth,_gameLayer->player2 ? _gameLayer->player2 : _gameLayer->currentPlayer);
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
