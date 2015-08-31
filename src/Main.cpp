#include <App.hpp>
#include <Director.hpp>
#include <EventListener.hpp>
#include <types.hpp>
#include <stdio.h>
#include <Scene.hpp>
#include <StageLayer.hpp>
#include <Sprite.hpp>
#include <Texture.hpp>
#include <TextureLoader.hpp>
#include <TextureManager.hpp>
#include <mm.hpp>
#include <ActionInterval.hpp>

Vec2 S = Vec2(480, 800);
static bool init()
{
    CheckGL();
    auto scene = MM<Scene>::New(S);
    CheckGL();

    auto sprite = MM<Sprite>::New();
    CheckGL();
    auto tm = TextureManager::getInstance();
    CheckGL();
    auto t = tm->loadTexture("map.png");
    sprite->setTextureRegion(t->getTextureRegion());
    scene->addChild(sprite);
    Director::getInstance()->run(scene);

    //sprite->setAnchor(0, 0);
    sprite->setPosition(S/2.f);
    {
        auto l = MM<EventKeyboardListener>::New();
        l->onPress = [sprite](EventKeyboard *e){
            printf("key press code = %x\n", (int)e->getKeyCode());
            switch(e->getKeyCode()){
            case KeyCode::KEY_W:
                sprite->setY(sprite->getY()+1);
                break;
            case KeyCode::KEY_A:
                sprite->setX(sprite->getX()-1);
                break;
            case KeyCode::KEY_S:
                sprite->setY(sprite->getY()-1);
                break;
            case KeyCode::KEY_D:
                sprite->setX(sprite->getX()+1);
                break;
            default:
                break;
            }

            return true;
        };
        l->onRelease = [](EventKeyboard *e){
            printf("key released code = %x\n", (int)e->getKeyCode());
        };
        sprite->addEventListener(l);
        auto ml = MM<EventMouseListener>::New();
        ml->onPress = [](EventMouse *){
            return true;
        };
        auto ip = sprite->getPosition();
        ml->onMove = [ip,sprite](EventMouse *e){
            auto const &start = e->getStartPos();
            auto const &pos = e->getCursorPos();
            auto dxy = pos - start;
            sprite->setPosition(ip+dxy);
        };
        sprite->addEventListener(ml);
        auto rot = RotateBy::create(4, 180);
        auto rep = Repeat::create(rot, -1);
        if(sprite->am_!=nullptr)
            Director::getInstance()->getIdleContainer().getIdleActionManager()->addAction(sprite, rep);
        else
            sprite->am_->addAction(sprite, rep);
    }
    //sprite->setRotation(30);
    //sprite->setPosition(80, 40);
    printf("size = %f, %f", sprite->getWidth(), sprite->getHeight());
    auto const &oxy = scene->getViewOrigin();
    auto const &size = scene->getViewSize();
    printf("view rect = (%f, %f), %f x %f\n",
           oxy.x, oxy.y, size.x, size.y);
    printf("anchor %f, %f  pos %f, %f  scale %f, %f, rotation %f \n",
           sprite->getAnchorX(), sprite->getAnchorY(),
           sprite->getX(), sprite->getY(),
           sprite->getScaleX(), sprite->getScaleY(),
           sprite->getRotation()
           );
    auto winsize = Director::getInstance()->getWinSize();
    printf("winsize = %f x %f\n", winsize.x, winsize.y);

    auto eye = scene->getCamera().getEye();
    printf("eye = %f, %f\n", eye.x, eye.y);
    return true;
}

int main()
{
    auto app = App::getInstance();
    Hooks h;
    h.main = init;
    app->go(h);

    return 0;
}
