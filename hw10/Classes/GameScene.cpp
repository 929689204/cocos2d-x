#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
	return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	//���ñ���
	auto gameBG = Sprite::create("level-background-0.jpg");
	gameBG->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(gameBG, 0);

	//StoneLayer
	auto stoneLayer = Layer::create();
	stoneLayer->setAnchorPoint(Vec2::ZERO);
	stoneLayer->setPosition(Vec2::ZERO);
	this->addChild(stoneLayer, 0, "stoneLayer");

	//ʯͷ����
	auto stone = Sprite::create("stone.png");
	stone->setPosition(Point(560, 480));
	stoneLayer->addChild(stone, 0 , "stone");
	
	//MouseLayer
	auto mouseLayer = Layer::create();
	mouseLayer->setAnchorPoint(Vec2::ZERO);
	mouseLayer->setPosition(Vec2(0, visibleSize.height / 2));
	this->addChild(mouseLayer, 0, "mouseLayer");
	
	//������
	auto mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* mouseAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("mouseAnimation"));
	mouse->runAction(RepeatForever::create(mouseAnimate));
	mouse->setPosition(visibleSize.width / 2, 0);
	mouseLayer->addChild(mouse, 0, "mouse");

	//shoot��ť
	auto shootLabel = MenuItemLabel::create(Label::createWithTTF("SHOOT", "fonts/Marker Felt.ttf", 50), CC_CALLBACK_1(GameScene::clickShoot, this));
	shootLabel->setPosition(Point(visibleSize.width / 2 + 250, 500));
	Menu *menu = Menu::create(shootLabel, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 0, "menu");

	//add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

bool GameScene::onTouchBegan(Touch *touch, Event *unused_event) {

	auto location = touch->getLocation();
	auto mouseLayer = this->getChildByName("mouseLayer");
	auto mouse = mouseLayer->getChildByName("mouse");

	// ��ȡ��ǰ�����������԰�ť��λ�����꣬�������������������ת��
	auto targetP = mouseLayer->convertToNodeSpaceAR(location);
	//log("p1:%f,%f", targetP.x, targetP.y);

	//��������
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese, 1);

	//��������ʧЧ��ʵ��
	auto fadeOut = FadeOut::create(2.0f);
	cheese->runAction(fadeOut);

	//�����ƶ�������λ��
	auto moveTo = MoveTo::create(1, targetP);
	mouse->runAction(moveTo);

	return true;
}

void GameScene::clickShoot(cocos2d::Ref* pSender) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto menu = this->getChildByName("menu");

	auto stoneLayer = this->getChildByName("stoneLayer");
	auto stone = Sprite::create("stone.png");
	stone->setPosition(Point(560, 480));
	stoneLayer->addChild(stone);

	auto mouseLayer = this->getChildByName("mouseLayer");
	auto mouse = mouseLayer->getChildByName("mouse");
	auto mouseLocation = mouse->getPosition();

	//����ת��
	auto stoneWorldPosition = mouseLayer->convertToWorldSpace(mouseLocation);
	auto targetP = stoneLayer->convertToNodeSpace(stoneWorldPosition);

	//ʯͷ���������ƶ�,Ȼ����ʧ
	auto moveTo = MoveTo::create(1.0f, targetP);
	auto fadeOut = FadeOut::create(1.0f);
	auto seq = Sequence::create(moveTo, fadeOut, nullptr);
	stone->runAction(seq);

	//��������שʯ���ƶ������λ��
	auto dia = Sprite::create("diamond.png");
	dia->setPosition(targetP);
	this->addChild(dia);
	float x = mouseLocation.x, y = mouseLocation.y;
	while (x == mouseLocation.x && y == mouseLocation.y) {
		auto rand = RandomHelper();
		x = rand.random_real(0.0f , visibleSize.width);
		y = rand.random_real(-visibleSize.height / 2, visibleSize.height / 2);
	}
	auto run = MoveTo::create(1.f, Point(x, y));
	mouse->runAction(run);
}