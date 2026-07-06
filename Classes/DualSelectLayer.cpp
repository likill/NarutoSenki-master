#include "DualSelectLayer.h"
#include "StartMenu.h"
#include <time.h>

USING_NS_CC_EXT;
using namespace CocosDenshion;

DualSelectLayer::DualSelectLayer(void)
{
	selectArray=NULL;
	_selectList=NULL;
	_pageLayer1=NULL;
	_pageLayer2=NULL;
	_pageLayer3=NULL;
	page1_btn=NULL;
	page2_btn=NULL;
	page3_btn=NULL;
	_p1HeroHalf=NULL;
	_p1HeroName=NULL;
	_p2HeroHalf=NULL;
	_p2HeroName=NULL;
	_selectImg=NULL;
	_phaseLabel=NULL;
	_p1SelectHero=NULL;
	_p2SelectHero=NULL;
	_phase=PHASE_P1_SELECT;
	isStart=false;
}

DualSelectLayer::~DualSelectLayer(void)
{
	CC_SAFE_RELEASE(selectArray);
	CC_SAFE_RELEASE(_selectList);
}

bool DualSelectLayer::init()
{
	bool bRet=false;
	do {
		CC_BREAK_IF(!CCLayer::init());

		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Select.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Menu.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Record.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Record2.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("UI.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Report.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Ougis.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Ougis2.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Map.plist");
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Gears.plist");

		CCSprite* bgSprite=CCSprite::create("red_bg.png");
		bgSprite->setAnchorPoint(ccp(0,0));
		bgSprite->setPosition(ccp(0,0));
		this->addChild(bgSprite,-5);

		CCSprite* gold_left=CCSprite::createWithSpriteFrameName("gold_left.png");
		gold_left->setAnchorPoint(ccp(0,0));
		gold_left->setPosition(ccp(0,20));
		this->addChild(gold_left,1);

		CCSprite* gold_right=CCSprite::createWithSpriteFrameName("gold_right.png");
		gold_right->setAnchorPoint(ccp(0,1));
		gold_right->setPosition(ccp(winSize.width-gold_right->getContentSize().width-20,winSize.height-20));
		this->addChild(gold_right,1);

		CCSprite* cloud_left=CCSprite::createWithSpriteFrameName("cloud.png");
		cloud_left->setPosition(ccp(0,15));
		cloud_left->setFlipX(true);
		cloud_left->setFlipY(true);
		cloud_left->setAnchorPoint(ccp(0,0));
		this->addChild(cloud_left,2);
		CCActionInterval* cmv1=CCMoveBy::create(1,ccp(-15,0));
		cloud_left->runAction(CCRepeatForever::create(CCSequence::create(cmv1,cmv1->reverse(),NULL)));

		CCSprite* cloud_right=CCSprite::createWithSpriteFrameName("cloud.png");
		cloud_right->setPosition(ccp(winSize.width-cloud_right->getContentSize().width,15));
		cloud_right->setFlipY(true);
		cloud_right->setAnchorPoint(ccp(0,0));
		this->addChild(cloud_right,2);
		CCActionInterval* cmv2=CCMoveBy::create(1,ccp(15,0));
		cloud_right->runAction(CCRepeatForever::create(CCSequence::create(cmv2,cmv2->reverse(),NULL)));

		CCSprite* cloud_top=CCSprite::createWithSpriteFrameName("cloud_top.png");
		cloud_top->setPosition(ccp(winSize.width/2,winSize.height-(cloud_top->getContentSize().height+12)));
		cloud_top->setAnchorPoint(ccp(0.5f,0));
		this->addChild(cloud_top,1);

		CCSprite* menu_bar_b=CCSprite::create("menu_bar2.png");
		menu_bar_b->setAnchorPoint(ccp(0,0));
		this->addChild(menu_bar_b,2);

		CCSprite* menu_bar_t=CCSprite::create("menu_bar3.png");
		menu_bar_t->setAnchorPoint(ccp(0,0));
		menu_bar_t->setPosition(ccp(0,winSize.height-menu_bar_t->getContentSize().height));
		this->addChild(menu_bar_t,2);

		CCLabelTTF* select_title=CCLabelTTF::create("LOCAL PVP",FONT_TYPE,18);
		select_title->setAnchorPoint(ccp(0,1));
		select_title->setPosition(ccp(8,winSize.height-6));
		this->addChild(select_title,3);

		_selectList=CCArray::create();
		selectArray=CCArray::create();
		_pageLayer1=CCLayer::create();
		_pageLayer2=CCLayer::create();
		_pageLayer3=CCLayer::create();

		char* selectList[]={
			"Naruto","Sakura","Sai","Kakashi","None","None","None",
			"Shikamaru","Ino","Choji","Asuma","Kiba","Hinata","Shino",
			"Neji","Tenten","Lee","None","None","None","None",
			"None","Tobirama","Hiruzen","Minato","Jiraiya","Tsunade","Orochimaru",
			"None","Deidara","Kakuzu","Hidan","Tobi","Konan","Pain",
			"Itachi","Kisame","None","None","None","None","None",
			"Sasuke","Karin","Suigetsu","Jugo","None","None","None",
			"Gaara","None","Kankuro","Chiyo","None","None","None",
			"None","None","None","None","None","None","None"
		};

		int num=sizeof(selectList)/sizeof(char*);
		for(int i=0;i<num;i++) {
			int Column=i%7;
			int Row=i/7;
			int Page=int(i/21);
			const char* charName=selectList[i];
			_selectList->addObject(CCString::createWithFormat("%s",charName));

			CCString* path=CCString::createWithFormat("%s_select.png",charName);
			SelectButton* select_btn=SelectButton::create(path->getCString());
			select_btn->setDelegate4(this);
			select_btn->setCharName(CCString::createWithFormat("%s",charName));

			CCPoint pos=ccp(winSize.width/2-36+(Column-1)*27+Column/4*10,winSize.height-112-(72*(Row-Page*3)));
			select_btn->setPosition(pos);
			if(Page==0) {
				_pageLayer1->addChild(select_btn,-Column);
			} else if(Page==1) {
				_pageLayer2->addChild(select_btn,-Column);
			} else {
				_pageLayer3->addChild(select_btn,-Column);
			}
			selectArray->addObject(select_btn);
		}

		this->addChild(_pageLayer1,5);
		this->addChild(_pageLayer2,5);
		this->addChild(_pageLayer3,5);
		_pageLayer2->setVisible(false);
		_pageLayer3->setVisible(false);
		_pageLayer2->setPositionY(2000);
		_pageLayer3->setPositionY(2000);

		page1_btn=CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("page1_off.png"),CCSprite::createWithSpriteFrameName("page1_on.png"),this,menu_selector(DualSelectLayer::onPage1Btn));
		page2_btn=CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("page2_off.png"),CCSprite::createWithSpriteFrameName("page2_on.png"),this,menu_selector(DualSelectLayer::onPage2Btn));
		page3_btn=CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("page3_off.png"),CCSprite::createWithSpriteFrameName("page3_on.png"),this,menu_selector(DualSelectLayer::onPage3Btn));
		CCMenu* page_menu=CCMenu::create(page1_btn,page2_btn,page3_btn,NULL);
		page_menu->alignItemsHorizontallyWithPadding(20);
		page_menu->setPosition(ccp(winSize.width/2+48,42));
		this->addChild(page_menu,10);
		page1_btn->selected();

		selectArray->retain();
		_selectList->retain();

		SelectButton* firstBtn=(SelectButton*)selectArray->objectAtIndex(0);
		_selectImg=CCSprite::createWithSpriteFrameName("1p.png");
		_selectImg->setAnchorPoint(ccp(0,0));
		_selectImg->setPosition(ccp(firstBtn->getPositionX()-2,firstBtn->getPositionY()-2));
		this->addChild(_selectImg,500);
		_selectImg->runAction(CCRepeatForever::create(CCBlink::create(0.6f,1)));

		const char* firstHero=firstBtn->getCharName()->getCString();
		_p1HeroHalf=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_half.png",firstHero)->getCString());
		_p1HeroHalf->setAnchorPoint(ccp(0,0));
		_p1HeroHalf->setPosition(ccp(10,10));
		this->addChild(_p1HeroHalf,1);

		_p1HeroName=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_font.png",firstHero)->getCString());
		_p1HeroName->setAnchorPoint(ccp(0.5f,0));
		_p1HeroName->setPosition(ccp(100,20));
		this->addChild(_p1HeroName,5);

		_phaseLabel=CCLabelTTF::create("P1 SELECT",FONT_TYPE,14);
		_phaseLabel->setPosition(ccp(winSize.width/2,winSize.height-28));
		this->addChild(_phaseLabel,20);

		if(CCUserDefault::sharedUserDefault()->getBoolForKey("isBGM")!=false) {
			SimpleAudioEngine::sharedEngine()->playBackgroundMusic("Audio/Music/select_music.mp3",true);
		}

		if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) {
			this->setKeypadEnabled(true);
		}

		bRet=true;
	} while(0);

	return bRet;
}

void DualSelectLayer::setSelected(CCObject* sender)
{
	if(_phase==PHASE_ALL_DONE) {
		return;
	}

	SelectButton* btn=(SelectButton*)sender;
	const char* heroName=btn->getCharName()->getCString();
	_selectImg->setPosition(ccp(btn->getPositionX()-2,btn->getPositionY()-2));

	if(_phase==PHASE_P1_SELECT) {
		_p1SelectHero=heroName;
		if(_p1HeroHalf) {
			_p1HeroHalf->removeFromParent();
		}
		_p1HeroHalf=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_half.png",heroName)->getCString());
		_p1HeroHalf->setAnchorPoint(ccp(0,0));
		_p1HeroHalf->setPosition(ccp(10,10));
		this->addChild(_p1HeroHalf,1);

		if(_p1HeroName) {
			_p1HeroName->removeFromParent();
		}
		_p1HeroName=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_font.png",heroName)->getCString());
		_p1HeroName->setAnchorPoint(ccp(0.5f,0));
		_p1HeroName->setPosition(ccp(100,20));
		this->addChild(_p1HeroName,5);

		CCSprite* mark=CCSprite::createWithSpriteFrameName("com_label2.png");
		mark->setPosition(ccp(70,140));
		this->addChild(mark,10);

		_selectImg->setColor(ccc3(255,80,80));
		_phase=PHASE_P2_SELECT;
		this->updatePhaseDisplay();
	} else if(_phase==PHASE_P2_SELECT) {
		_p2SelectHero=heroName;

		if(_p2HeroHalf) {
			_p2HeroHalf->removeFromParent();
		}
		_p2HeroHalf=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_half.png",heroName)->getCString());
		_p2HeroHalf->setAnchorPoint(ccp(1,0));
		_p2HeroHalf->setFlipX(true);
		_p2HeroHalf->setPosition(ccp(winSize.width-10,10));
		this->addChild(_p2HeroHalf,1);

		if(_p2HeroName) {
			_p2HeroName->removeFromParent();
		}
		_p2HeroName=CCSprite::createWithSpriteFrameName(CCString::createWithFormat("%s_font.png",heroName)->getCString());
		_p2HeroName->setAnchorPoint(ccp(0.5f,0));
		_p2HeroName->setPosition(ccp(winSize.width-100,20));
		this->addChild(_p2HeroName,5);

		CCSprite* mark=CCSprite::createWithSpriteFrameName("com_label2.png");
		mark->setPosition(ccp(winSize.width-70,140));
		this->addChild(mark,10);

		_phase=PHASE_ALL_DONE;
		this->updatePhaseDisplay();
		this->scheduleOnce(schedule_selector(DualSelectLayer::onGameStart),0.6f);
	}
}

void DualSelectLayer::updatePhaseDisplay()
{
	if(!_phaseLabel) {
		return;
	}
	if(_phase==PHASE_P1_SELECT) {
		_phaseLabel->setString("P1 SELECT");
	} else if(_phase==PHASE_P2_SELECT) {
		_phaseLabel->setString("P2 SELECT");
	} else {
		_phaseLabel->setString("START");
	}
}

void DualSelectLayer::addHero(CCArray* heros, const char* character, const char* role, const char* group)
{
	CCDictionary* dic=CCDictionary::create();
	dic->setObject(CCString::create(character),"character");
	dic->setObject(CCString::create(role),"role");
	dic->setObject(CCString::create(group),"group");
	heros->addObject(dic);
}

CCString* DualSelectLayer::takeRandomHero(CCArray* heros)
{
	int index=random(heros->count());
	if(index==(int)heros->count()) {
		index=heros->count()-1;
	}
	CCString* hero=(CCString*)heros->objectAtIndex(index);
	CCString* result=CCString::createWithFormat("%s",hero->getCString());
	heros->removeObjectAtIndex(index);
	return result;
}

void DualSelectLayer::onGameStart(float dt)
{
	if(isStart || !_p1SelectHero || !_p2SelectHero) {
		return;
	}

	isStart=true;
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
	SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");

	char* heroList[]={
		"Konan","Sakura","Naruto","Sai","Deidara",
		"Kakashi","Itachi","Tenten","Jiraiya","Suigetsu",
		"Tsunade","Tobirama","Neji","Ino","Asuma","Gaara",
		"Karin","Sasuke","Hidan","Choji","Kankuro",
		"Shino","Minato","Tobi","Kakuzu","Hinata",
		"Shikamaru","Chiyo","Kisame",
		"Hiruzen","Kiba","Jugo","Lee"
	};

	int num=sizeof(heroList)/sizeof(char*);
	CCArray* realHero=CCArray::create();
	for(int i=0;i<num;i++) {
		if(strcmp(_p1SelectHero,heroList[i])==0) {
			continue;
		}
		if(strcmp(_p2SelectHero,heroList[i])==0) {
			continue;
		}
		realHero->addObject(CCString::create(heroList[i]));
	}

	srand((int)time(0));
	CCArray* tempHeros=CCArray::create();
	this->addHero(tempHeros,_p1SelectHero,"Player","Konoha");

	for(int i=0;i<2;i++) {
		CCString* hero=this->takeRandomHero(realHero);
		this->addHero(tempHeros,hero->getCString(),"Com","Konoha");
	}

	this->addHero(tempHeros,_p2SelectHero,"Player","Akatsuki");

	for(int i=0;i<2;i++) {
		CCString* hero=this->takeRandomHero(realHero);
		this->addHero(tempHeros,hero->getCString(),"Com","Akatsuki");
	}

	CCScene* loadScene=CCScene::create();
	LoadLayer* loadLayer=LoadLayer::create();
	loadLayer->tempHeros=tempHeros;
	loadLayer->_isHardCoreMode=true;
	loadLayer->_isLocalPvP=true;
	loadScene->addChild(loadLayer);
	loadLayer->preloadAudio();
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.5f,loadScene));
}

void DualSelectLayer::onPage1Btn(CCObject* sender)
{
	page1_btn->selected();
	page2_btn->unselected();
	page3_btn->unselected();
	_pageLayer1->setVisible(true);
	_pageLayer2->setVisible(false);
	_pageLayer3->setVisible(false);
	_pageLayer1->setPositionY(0);
	_pageLayer2->setPositionY(2000);
	_pageLayer3->setPositionY(2000);
}

void DualSelectLayer::onPage2Btn(CCObject* sender)
{
	page1_btn->unselected();
	page2_btn->selected();
	page3_btn->unselected();
	_pageLayer1->setVisible(false);
	_pageLayer2->setVisible(true);
	_pageLayer3->setVisible(false);
	_pageLayer1->setPositionY(2000);
	_pageLayer2->setPositionY(0);
	_pageLayer3->setPositionY(2000);
}

void DualSelectLayer::onPage3Btn(CCObject* sender)
{
	page1_btn->unselected();
	page2_btn->unselected();
	page3_btn->selected();
	_pageLayer1->setVisible(false);
	_pageLayer2->setVisible(false);
	_pageLayer3->setVisible(true);
	_pageLayer1->setPositionY(2000);
	_pageLayer2->setPositionY(2000);
	_pageLayer3->setPositionY(0);
}

void DualSelectLayer::keyBackClicked()
{
	SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/cancel.ogg");
	CCScene* menuScene=CCScene::create();
	CCLayer* menuLayer=StartMenu::create();
	menuScene->addChild(menuLayer);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.5f,menuScene));
}
