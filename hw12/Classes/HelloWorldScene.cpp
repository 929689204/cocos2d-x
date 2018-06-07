#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

	/* ���ݿ�sqlite */
	char * errMsg = NULL;	//������Ϣ   
	std::string sqlstr;		//SQLָ��   
	int result;				//sqlite3_exec����ֵ   
	//��һ�����ݿ⣬��������ݿⲻ���ڣ��򴴽�һ�����ݿ��ļ�   
	result = sqlite3_open("simpleGame.db", &db);
	//����������IDΪ���������Զ�����   
	result = sqlite3_exec(db, "create table if not exists Scores( ID integer primary key autoincrement, score nvarchar(32) ) ", NULL, NULL, &errMsg);

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();


	//�����ļ�·�����ٵ�����Ƭ��ͼ
	TMXTiledMap* tmx = TMXTiledMap::create("map.tmx");

	//����λ��
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	tmx->setScale(Director::getInstance()->getContentScaleFactor());

	//����ê��
	tmx->setAnchorPoint(Vec2(0.5, 0.5));

	//��ӵ���Ϸͼ���У�����0����Z�ᣨZ��͵Ļᱻ�ߵ��ڵ���  
	this->addChild(tmx,0);


	//	����һ����ͼ
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//	����ͼ�������ص�λ�и�����ؼ�֡
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//	ʹ�õ�һ֡��������
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	/* hp�� */
	//	hp����Ѫ��
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	//	hp������
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//	ʹ��hp������progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);//	����Ϊ��������
	pT->setBarChangeRate(Point(1, 0));	//	���÷��������ң�
	pT->setMidpoint(Point(0, 1));		//	���ÿ�ʼ�ط������Ͻǣ�
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);

	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);

	// ��̬����
	idle.reserve(1);
	idle.pushBack(frame0);

	// ��������
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.08f);
	attackAnimation->setRestoreOriginalFrame(true);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attackAnimation");

	// ��������(֡����22֡���ߣ�90����79��
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.08f);
	deadAnimation->setRestoreOriginalFrame(false);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "deadAnimation");

	// �˶�����(֡����8֡���ߣ�101����68��
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.04f);
	AnimationCache::getInstance()->addAnimation(runAnimation, "runAnimation");

	/* �ƶ���ť */
	//	up
	auto upLabel = MenuItemLabel::create(Label::createWithTTF("W", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::MoveUp, this));
	//	down
	auto downLabel = MenuItemLabel::create(Label::createWithTTF("S", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::MoveDown, this));
	//	left
	auto leftLabel = MenuItemLabel::create(Label::createWithTTF("A", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::MoveLeft, this));
	//	right
	auto rightLabel = MenuItemLabel::create(Label::createWithTTF("D", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::MoveRight, this));

	upLabel->setPosition(Point(100, 100));
	downLabel->setPosition(Point(100, 50));
	leftLabel->setPosition(Point(50, 50));
	rightLabel->setPosition(Point(150, 50));

	Menu * moveMenu = Menu::create(upLabel, downLabel, leftLabel, rightLabel, NULL);
	moveMenu->setPosition(Vec2::ZERO);
	this->addChild(moveMenu, 0, "moveMenu");

	acting = false;
	endGame = false;

	/* X��Y���ܰ�ť */
	//auto XLabel = MenuItemLabel::create(Label::createWithTTF("X", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::Xfun, this));
	auto YLabel = MenuItemLabel::create(Label::createWithTTF("Y", "fonts/arial.ttf", 50), CC_CALLBACK_1(HelloWorld::attackMoster, this));

	//XLabel->setPosition(Vec2(visibleSize.width - origin.x - 50, 100));
	YLabel->setPosition(Vec2(visibleSize.width - origin.x - 60, 75));

	//Menu * funMenu = Menu::create(XLabel, YLabel, NULL);
	Menu * funMenu = Menu::create(YLabel, NULL);
	funMenu->setPosition(Vec2::ZERO);
	this->addChild(funMenu, 0, "funMenu");

	//����ʱ
	time = Label::createWithTTF("60", "fonts/arial.ttf", 30);
	time->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 200));
	this->addChild(time);

	//��ɱ��
	attackNum = Label::createWithTTF("0", "fonts/arial.ttf", 50);
	attackNum->setPosition(Vec2(visibleSize.width - 50, visibleSize.height / 2 + 200));
	attackNum->setColor(Color3B(255, 0, 0));
	this->addChild(attackNum);

	//��ѷ���
	std::string BestScore = Value(get()).asString();
	std::string show = "Your best score: "+ BestScore;
	Label* bestScore = Label::createWithTTF(show, "fonts/arial.ttf", 20);
	bestScore->setPosition(Vec2(visibleSize.width - 90, visibleSize.height / 2 + 150));
	bestScore->setColor(Color3B(0, 0, 0));
	this->addChild(bestScore);

	//����ʱ������
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);

	//���ɹ��������
	schedule(schedule_selector(HelloWorld::createAndMoveMonster), 2.5f, kRepeatForever, 0);

	//���﹥����ҵ�����
	schedule(schedule_selector(HelloWorld::hitByMonster), 2.0f, kRepeatForever, 0);

	return true;
}

//ʱ�䵹��ʱ
void HelloWorld::updateTime(float dt) {
	int leftTime = std::atoi(time->getString().c_str());
	if (leftTime > 0) {
		--leftTime;
		std::string tmp = Value(leftTime).asString();
		time->setString(tmp);
	}
	else {
		endGame = true;
		time->setString("TIME OUT");
		store();
		unschedule(schedule_selector(HelloWorld::updateTime));
		unschedule(schedule_selector(HelloWorld::createAndMoveMonster));
		unschedule(schedule_selector(HelloWorld::hitByMonster));
	}
}

//����
void HelloWorld::MoveUp(cocos2d::Ref* pSender) {
	if (!endGame) {
		Animate* runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation"));
		/*��ֹ�߳���Ļ�ϱ߽�*/
		if (player->getPositionY() + 30 + player->getContentSize().height / 2 < visibleSize.height) {
			Point p = Point(player->getPositionX(), player->getPositionY() + 30);
			auto move = MoveTo::create(0.4f, p);
			auto mySpawn = Spawn::createWithTwoActions(move, runAnimate);
			player->runAction(mySpawn);
		}
		else {
			player->runAction(runAnimate);
		}
	}
}

//����
void HelloWorld::MoveDown(cocos2d::Ref * pSender) {
	if (!endGame) {
		Animate* runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation"));
		/*��ֹ�߳���Ļ�±߽�*/
		if (player->getPositionY() - 30 - player->getContentSize().height / 2 > 0) {
			Point p = Point(player->getPositionX(), player->getPositionY() - 30);
			auto move = MoveTo::create(0.4f, p);
			auto mySpawn = Spawn::createWithTwoActions(move, runAnimate);
			player->runAction(mySpawn);
		}
		else {
			player->runAction(runAnimate);
		}
	}
}

//����
void HelloWorld::MoveRight(cocos2d::Ref * pSender) {
	if (!endGame) {
		Animate* runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation"));

		player->setFlippedX(false);
		/*��ֹ�߳���Ļ�ұ߽�*/
		if (player->getPositionX() + 30 + player->getContentSize().width / 2 < visibleSize.width) {
			Point p = Point(player->getPositionX() + 30, player->getPositionY());
			auto move = MoveTo::create(0.4f, p);
			auto mySpawn = Spawn::createWithTwoActions(move, runAnimate);
			player->runAction(mySpawn);
		}
		else {
			player->runAction(runAnimate);
		}
	}
}

//����
void HelloWorld::MoveLeft(cocos2d::Ref * pSender) {
	if (!endGame) {
		Animate* runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("runAnimation"));

		player->setFlippedX(true);
		/*��ֹ�߳���Ļ��߽�*/
		if (player->getPositionX() - 30 - player->getContentSize().width / 2 > 0) {
			Point p = Point(player->getPositionX() - 30, player->getPositionY());
			auto move = MoveTo::create(0.4f, p);
			auto mySpawn = Spawn::createWithTwoActions(move, runAnimate);
			player->runAction(mySpawn);
		}
		else {
			player->runAction(runAnimate);
		}
	}
}

//���ɹ��ﲢ�������ƶ�
void HelloWorld::createAndMoveMonster(float dt) {
	//���ɹ���
	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	m->setPosition(x, y);
	addChild(m, 3);

	//�ƶ�����
	fac->moveMonster(player->getPosition(), 2.0f);
}

//����������ɫ����ɫ��Ѫ����ɫѪ��Ϊ���򲥷���������
void HelloWorld::hitByMonster(float dt) {
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(player->getBoundingBox());
	if (collision != NULL) {
		fac->removeMonster(collision);

		//	��Ѫ
		int num = pT->getPercentage();
		if (num > 0) {
			ProgressTo* progressTo = ProgressTo::create(1.5f, num - 20);
			pT->runAction(progressTo);
		}
		if(num - 20 <= 0) {
			//������Ϸ
			endGame = true;
			time->setString("Game Over");
			unschedule(schedule_selector(HelloWorld::updateTime));
			unschedule(schedule_selector(HelloWorld::createAndMoveMonster));
			unschedule(schedule_selector(HelloWorld::hitByMonster));

			store();

			Animate* deadAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("deadAnimation"));
			player->runAction(deadAnimate);
		}
	}
}

//��ɫ��������򵽻�Ѫ
void HelloWorld::attackMoster(cocos2d::Ref * pSpender) {
	Rect playerRect = player->getBoundingBox();
	//param: x, y, width, height
	Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY() - 20,
		playerRect.getMaxX() - playerRect.getMinX() + 80,
		playerRect.getMaxY() - playerRect.getMinY() + 40);

	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(attackRect);
	if (collision != NULL && !acting) {
		
		fac->removeMonster(collision);

		acting = true;
		Animate* attackAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("attackAnimation"));
		auto set = CallFunc::create(([this]() {
			log("attack");
			acting = false;
		}));

		//���ӻ�ɱ��
		auto add = CallFunc::create(([this]() {
			int kill = std::atoi(attackNum->getString().c_str());
			++kill;
			std::string tmp = Value(kill).asString();
			attackNum->setString(tmp);
		}));

		Sequence* attackSeq = Sequence::create(attackAnimate, set, add, NULL);
		player->runAction(attackSeq);


		//	��Ѫ
		int num = pT->getPercentage();
		if (num < 100) {
			ProgressTo* progressTo = ProgressTo::create(1.5f, num + 20);
			pT->runAction(progressTo);
		}
	}
}

//��
void HelloWorld::store() {
	
	/* sqlite */
	char * errMsg = NULL;	//������Ϣ   
	std::string sqlstr = " insert into Scores( score ) values ( '" + attackNum->getString() + "' ) ";
	sqlite3_exec(db, sqlstr.c_str(), NULL, NULL, &errMsg);
	//�ر����ݿ�   
	sqlite3_close(db);

	/* userdefault */
	//���xml�ļ��Ƿ���ڣ��Ǳ��룩
	if(!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
	}

	//��
	int kill = std::atoi(attackNum->getString().c_str());
	if(kill > database->getIntegerForKey("bestScore", 0))
		database->setIntegerForKey("bestScore", kill);
	
	int round = database->getIntegerForKey("round", 0) + 1;
	database->setIntegerForKey("round", round);

	std::string newKey = "round" + Value(round).asString();
	database->setIntegerForKey(newKey.c_str(), kill);
}

//ȡ
int HelloWorld::get() {
	/* sqlite */

	/* userdefault */
	log("%s", FileUtils::getInstance()->getWritablePath().c_str());

	//���xml�ļ��Ƿ���ڣ��Ǳ��룩
	if (!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
	}

	//ȡ
	int bestScore = database->getIntegerForKey("bestScore", 0);

	return bestScore;
}