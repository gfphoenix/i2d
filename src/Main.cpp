#include <App.hpp>
#include <Director.hpp>
#include <EventListener.hpp>
#include <types.hpp>
#include <stdio.h>
#include <Scene.hpp>
#include <Shape.hpp>
#include <StageLayer.hpp>
#include <Sprite.hpp>
#include <Texture.hpp>
#include <TextureLoader.hpp>
#include <TextureManager.hpp>
#include <mm.hpp>
#include <ActionInterval.hpp>

Vec2 S = Vec2(480, 800);
Vec2 R1;
Vec2 oldPos;
Vec2 R2;
Vec2 oldPos2;
static bool init()
{
    auto scene = MM<Scene>::New(S);
    auto sprite = MM<Sprite>::New();
    auto tm = TextureManager::getInstance();
    auto t = tm->loadTexture("map.png");
    sprite->setTextureRegion(t->getTextureRegion());
    scene->addChild(sprite);
    Director::getInstance()->run(scene);
    auto line = MM<Line>::New();
    line->setP0(Vec2(40, 40));
    line->setP1(Vec2(400, 400));
    sprite->addChild(line);
    sprite->setPosition(S/2.f);
    {
        auto ml = MM<EventMouseListener>::New();
        ml->onPress = [sprite](EventMouse *e){
            auto p = e->getCursorWorld();
            auto local = sprite->toLocal(p);
            if(!sprite->hit(local))
                return false;
            R1 = e->getCursorWorld();
            oldPos = sprite->getPosition();
            printf("R1(%f, %f), oldPos(%f, %f)\n", R1.x, R1.y, oldPos.x, oldPos.y);
            return true;
        };
        ml->onMove = [sprite](EventMouse *e){
            auto w = e->getCursorWorld();
            auto dxy = w - R1;
            sprite->setPosition(oldPos+dxy);
        };
        sprite->addEventListener(ml);
        float duration=0;
        sprite->schedule([duration](float dt)mutable{
                bool out = duration > 10;
        //        printf("dt = %f  T=%f\n", dt, duration);
                duration += dt;
                return out;
                },"foobar", 3, 1, 17);
        auto life = MM<Sprite>::New();
        life->setTextureRegion(tm->loadTexture("N.png")->getTextureRegion());
        life->setColor(Color3(.31, .8, .3));
        sprite->addChild(life);
        life->setPosition(300, 300);
        auto rot = RotateBy::create(3, 180);
        auto rep = Repeat::create(rot, -1);
        life->runAction(rep);
        life->schedule([life](float dt){
                //printf("rot=%f\n", life->getRotation());
                return false;
                }, "printRot", 0, .5f, 1);
        auto l = MM<EventKeyboardListener>::New();
        l->onPress = [life](EventKeyboard *e){
            printf("key press code = %x\n", (int)e->getKeyCode());
            switch(e->getKeyCode()){
            case KeyCode::KEY_W:
                life->setY(life->getY()+1);
                break;
            case KeyCode::KEY_A:
                life->setX(life->getX()-1);
                break;
            case KeyCode::KEY_S:
                life->setY(life->getY()-1);
                break;
            case KeyCode::KEY_D:
                life->setX(life->getX()+1);
                break;
            default:
                break;
            }
            return true;
        };
        l->onRelease = [](EventKeyboard *e){
            printf("key released code = %x\n", (int)e->getKeyCode());
        };
        life->addEventListener(l);

        auto ml2 = MM<EventMouseListener>::New();
        ml2->onPress = [life](EventMouse *e){
            auto const &p = e->getCursorWorld();
            auto local = life->toLocal(p);
            if(!life->hit(local))
                return false;
            printf("click life OK\n");
            R2 = p;
            oldPos2 = life->getPosition();
            return true;
        };
        ml2->onMove = [life](EventMouse *e){
            auto const &pos = e->getCursorWorld();
            auto dxy = pos - R2;
            life->setPosition(oldPos2+dxy);
        };
        ml2->onScroll = [life,scene](EventMouse *e){
            auto p = life->toLocal(e->getCursorWorld());
            if(!life->hit(p))
                return false;
            auto dy = e->getScrollY();
            auto zoom = scene->getCamera().getZoomFactor();
            zoom += dy *.1f;
            printf("dy = %f, zoom=%f\n", (float)dy, zoom);
            scene->getCamera().setZoomFactor(zoom);
            return true;
        };
        life->addEventListener(ml2);
    }
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
