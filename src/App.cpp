#include <App.hpp>
#include <debug>
#include <Director.hpp>
#include <Layer.hpp>
#include <Scene.hpp>
#include <Sprite.hpp>
#include <TextureManager.hpp>
#include <mm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <random>
#include <ActionInterval.hpp>
#include <ActionInstant.hpp>
#include <Texture.hpp>
#include <sys/time.h>

bool App::main()
{
    //Director::getInstance()->getGLView()->setDesignResolution(480, 800, ResolutionPolicy::KEEP);
    Director::getInstance()->getGLView()->setDesignResolution(480, 800);
    Debug("App::main, sizeof Node = %d\n", sizeof(Node));
    auto tm = TextureManager::getInstance();
    auto map = tm->loadTexture("pic/map.png");
    auto end = tm->loadTexture("pic/frog.png");
    auto r = map->getTextureRegion();
    Debug("map res=%p\n", (void*)map.get());
    auto subR = r->getSubRegion(0, 0, 50, 50);
    auto sp = MM<Sprite>::New();
    sp->setName("map0");
    sp->setTextureRegion(Ref_ptr<TextureRegion2D>(r));
    //sp->setPosition(40, 40);
    sp->setAnchor(0,0);
    auto scene = MM<Scene>::New();
    scene->setName("scene0");
    auto camera = MM<OrthCamera>::New();
    {
        auto &v = Director::getInstance()->getGLView();
        auto &size = v->getViewSize();
        Debug("view size= %d x %d\n", size[0], size[1]);
    //    camera->init(size[0], size[1]);
        camera->init(480, 800);
       // camera->setZoomFactor(1.2);
    }
    scene->setCamera(camera);
    //scene->getCamera().Orth2d(0, 480, 0, 800);
    scene->addChild(sp);

    auto frog = MM<Sprite>::New();
    frog->setName("S2");
    auto region = end->getTextureRegion(0,0,0,0, RegionDirection::BOTTOM);
    //auto region = end->getTextureRegion(0,10,0,0, RegionDirection::LEFT);
    //auto region = end->getTextureRegion(0,0,0,0, RegionDirection::TOP);
    frog->setTextureRegion(region);
    frog->setPosition(180, 180);
    //s2->setRotation(45);
    frog->setColor(Color4(1.5, .7, .8, .7));
    frog->setZindex(1);
    auto child = MM<Sprite>::New();
    child->setTextureRegion(region);
    child->setPosition(child->getWidth()*2, child->getHeight()*2);
    {
        auto rot = RotateBy::create(40, 3600);
        auto s1 = ScaleTo::create(1, 1.4f, 1.4f);
        auto s2 = ScaleTo::create(1, .6f, .6f);
        auto seq = Sequence::create({s1, s2});
        auto r = Repeat::create(seq, -1);
        auto rr = Repeat::create(rot, -1);
        child->runAction(rr);
        //frog->runAction(r);
        auto mask = MM<Sprite>::New();
        mask->setTextureRegion(subR);
        auto &&c = mask->getColorCp();
        c.r = 1000;
        // c.g = 10;
        c.a = 1000;
        mask->setColor(c);
        child->addChild(mask);
        mask->setZindex(-1);
        mask->setAnchor(0,0);
        mask->setSize(child->getSize());
        mask->setPosition(40, 40);
    }
    //frog->setSkewX(45);
    //frog->setSkewY(30);
    //child->setSkewY(-60);
    //frog->addChild(child);
    Vec2 min(0.0f,0.0f);
    Vec2 max(480.0f, 800.0f);
    auto N = 1 * 0;
    for(auto i=N; i>0; i--){
        auto n = MM<Sprite>::New();
        n->setTextureRegion(region);
        sp->addChild(n);
        auto && pos = glm::linearRand(min, max);
        n->setPosition(pos);
        if(i>50){
            n->runAction(Repeat::create(RotateBy::create(2, 360), 4));
        }
        continue;
        n->schedule([n](float d)->bool{
                auto r=n->getRotation();
                r += glm::linearRand(-1.0f, 2.0f);
                n->setRotation(r);
                return false;
                }, "rot");

    }
    for(auto i=1000*N; i>0; i--){
        //sp->addChild(MM<Layer>::New());
    }
    //    s2->setFlipX();
    //auto layer = MM<Layer>::New();
    //layer->addChild(s2);
    //sp->addChild(layer);
    sp->addChild(frog);
    //sp->addChild(child);
    frog->addChild(child);
    frog->setColor(255,0,0,255);
    Director::getInstance()->run(scene);
    Debug("Run here, default shader = %p\n", sp->getShader());


    //auto s0 = Scheduler::create();
    auto s0 = scene->getScheduler();
    //s0->pause();
    float ca=.05f;
    float sa=1.0f;

    float g=.5f;
    float sg=1.0f;
    int count=0;
    frog->schedule([frog,g, sg,count, scene](float)mutable{
            g += 0.01f * sg;
            if(g>=4.0f){
            sg = -1.0f;
            }else if(g<0.3f){
            sg = 1.0f;
            }
            //        scene->getCamera()->getMat()[3][3] = g;
            auto c4 = frog->getColorCp();
            c4.g = g;
            frog->setColor(c4);
            // printf("Green - %d\n", ++count);
            return false;
            }, "Green");
#if 0
    s0->scheduleNode(frog, "Green", [frog,g, sg,count](float)mutable{
            g += 0.01f * sg;
            if(g>=4.0f){
            sg = -1.0f;
            }else if(g<0.3f){
            sg = 1.0f;
            }
            auto c4 = frog->getColorCp();
            c4.g = g;
            frog->setColor(c4);
            // printf("Green - %d\n", ++count);
            },0, 0.03f, 1000000);
#endif
    auto tmpp = (void*)2000;
#if 0
    s0->schedulePtr(tmpp, "tmp", [s0,frog](float){
            // printf("500 loops ok...\n");
            auto bb = s0->isScheduled(frog, "barz");
            // printf("bars is running = %d\n", bb);
            s0->unscheduleNodeOne(frog, "barz");
            bb = s0->isScheduled(frog, "barz");
            // printf("bars is running = %d\n", bb);
            // printf("safe exit..\n");
            },15.0f, 0.0f, 0);

    s0->schedulePtr(tmpp, "tmpx", [tmpp,s0,frog](float){
            auto bb = s0->isScheduled(frog, "barz");
            //  printf("bars is running = %d\n", bb);
            s0->unscheduleNodeOne(frog, "barz");
            bb = s0->isScheduled(tmpp, "tmp");
            // printf("tmpp's tmp is running = %d\n", bb);
            bb = s0->isScheduled(frog, "Green");
            // printf("Green is running = %d\n", bb);
            s0->unscheduleNodeOne(frog, "Green");
            bb = s0->isScheduled(frog, "Green");
            // printf("Green is running = %d\n", bb);
            //s2->remove();

            float alpha=0;
            s0->scheduleNode(frog, "alpha", [frog,alpha](float dt)mutable{
                    alpha += dt;
                    if(alpha>5.0f)
                    alpha -= 5.0f;
                    float a = alpha/5.0f;
                    auto c = frog->getColorCp();
                    c.a = a;
                    frog->setColor(c);
                    });
    },20.0f, 0.0f, 0);
#endif
    frog->schedule([frog,ca,sa](float)mutable{
            //printf("S2 scheduled, delta = %f, pfs=%.1f\n", dt, 1.0f/dt);
            //            auto y=s2->getY();
            //            auto x=s2->getX();
            //            x += glm::linearRand(-.20f, .20f);
            //            x = glm::clamp(x, 0.0f, 480.0f);
            //            s2->setX(x);

            //            y += glm::linearRand(-1.0f, 1.0f);
            //            y = glm::clamp(y, 200.0f, 600.0f);
            //            s2->setY(y);
            auto r = frog->getRotation();
            frog->setRotation(r+1);

            ca += 0.01 * sa;
            if(ca>=4.0f){
            sa = -1;
            }else if(ca<0.01f){
            sa = 1;
            }
            auto c4 = frog->getColorCp();
            c4.r = ca;
            frog->setColor(c4);
            auto tm = time(nullptr);
            //  printf("ONCE scheduled...%s\n", ctime(&tm));
            return false;
    }, "once", 1, 0, -1);
    auto seq = MM<Sequence>::New();
    seq->actions_.push_back(Show::create());
    seq->actions_.push_back(Delay::create(1));
    seq->actions_.push_back(Hide::create());
    seq->actions_.push_back(Delay::create(1));
    // frog->runAction(seq);
    return true;
}
void App::onEnterBackground()
{
}
void App::onEnterForeground()
{
}
