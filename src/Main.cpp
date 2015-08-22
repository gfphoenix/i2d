#include <App.hpp>
#include <Director.hpp>
#include <types.hpp>
#include <stdio.h>
#include <Scene.hpp>
#include <StageLayer.hpp>
#include <Sprite.hpp>
#include <Texture.hpp>
#include <TextureLoader.hpp>
#include <TextureManager.hpp>
#include <mm.hpp>
Vec2 S = Vec2(480, 800);
static bool init()
{
    CheckGL();
    auto scene = MM<Scene>::New();
    CheckGL();
    auto stage = MM<StageLayer>::New(S, ResolutionPolicy::FULL);
    CheckGL();
    auto sprite = MM<Sprite>::New();
    CheckGL();
    auto tm = TextureManager::getInstance();
    CheckGL();
    auto t = tm->loadTexture("map.png");
    sprite->setTextureRegion(t->getTextureRegion());
    stage->addChild(sprite);
    scene->setMainStage(stage);
    Director::getInstance()->run(scene);

    //sprite->setAnchor(0, 0);
    sprite->setPosition(S/2.f);

    //sprite->setRotation(30);
    //sprite->setPosition(80, 40);
    printf("size = %f, %f", sprite->getWidth(), sprite->getHeight());
    auto const &oxy = stage->getViewOrigin();
    auto const &size = stage->getViewSize();
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
    //stage->getCamera().setEye(0, 0);
    auto eye = stage->getCamera().getEye();
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
