#ifndef REDNERER
#define REDNERER
#include "Camera.hpp"
#include "StageLayer.hpp"
class Shader;
class StageLayer;
class Renderer
{
protected:
    Shader *last_;
    StageLayer *stage_;
    unsigned long long totalCalls_;
    int lastFrameCalls_;
    int currentCalls_;
    friend class StageLayer;
    inline void setStageLayer(StageLayer *stage){stage_ = stage;}
public:
    inline Renderer():
        last_(nullptr), stage_(nullptr),
        totalCalls_(0), lastFrameCalls_(0),
        currentCalls_(0){}
    inline void startRenderCallCounter(){lastFrameCalls_=currentCalls_;currentCalls_=0;}
    inline int getRenderCalls()const{return lastFrameCalls_;}
    inline unsigned long long getTotalRenderCalls()const{return totalCalls_;}
    inline void addRenderCall(){totalCalls_++;currentCalls_++;}
    inline const Camera &getCamera()const{return stage_->getCamera();}
    // must be called in DrawSelf
    virtual void Use(Shader *shader);
    // only used in stage
    virtual void Flush();
};

#endif // REDNERER

