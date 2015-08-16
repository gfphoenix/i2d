#include <EventDispatcher.hpp>
#include <debug>
#include <vector>

    EventDispatcher::ListenerBundle::ListenerBundle()
    :scenePriorityListeners_(nullptr)
     , fixedPriorityListeners_(nullptr)
{}
EventDispatcher::ListenerBundle::~ListenerBundle()
{
    delete scenePriorityListeners_;
    delete fixedPriorityListeners_;
}
static void cleanup(std::vector<Ref_ptr<EventListener>> *l)
{
    if(l == nullptr)
        return;
    auto p = l->data();
    auto n = l->size();
    int i=0;
    for(int j=0; j<n; j++){
        if(!p[j]->isZombie()){
            if(i!=j)
                p[i] = std::move(p[j]);
            i++;
        }
    }
    l->resize(i);
}
void EventDispatcher::ListenerBundle::cleanupFixedZombieListeners()
{
    cleanup(fixedPriorityListeners_);
}
void EventDispatcher::ListenerBundle::cleanupSceneZombieListeners()
{
    cleanup(scenePriorityListeners_);
}

EventDispatcher::EventDispatcher()
    : enabled_(true)
    , dirty_sceneOrder_(false)
    , hasZombies_(false)
{
}
void EventDispatcher::addEventListenerWithScenePriority(EventListener *listener, Node *node)
{
    Assert(listener && node, "listener and node must not be null");
    Assert(!listener->isRegistered(), "listener has already registered");

    listener->setRegistered();
    listener->setNode(node);
    listener->setFixedPriority(0);

    toBeAdd_.push_back(listener);
}
void EventDispatcher::addEventListenerWithFixedPriority(EventListener *listener, int priority, Node *node)
{
    Assert(listener, "listener must not be null");
    Assert(!listener->isRegistered(), "listener must not be registered");
    Assert(priority!=0, "fixed priority must not be 0");

    listener->setRegistered();
    listener->setNode(node);
    listener->setFixedPriority(priority);

    toBeAdd_.push_back(listener);
}
void EventDispatcher::removeEventListener(EventListener *listener)
{
    listener->markRemove();
    markRemove();
}
void EventDispatcher::removeEventListenerForNode(Node *node)
{
    if(node==nullptr)
        return ;
    for(auto &p : toBeAdd_)
        if(p->getNode()==node)
            p->markRemove();
    auto it = nodeListenersMap_.find(node);
    if(it == nodeListenersMap_.end())
        return;
    auto &v = it->second;
    for(auto &p : v){
        p->markRemove();
    }
    // this method maybe called in event-handler loop, so just mark it
    // does cleaning-up in the next update
    //nodeListenersMap_.erase(it);

    // mark remove in toBeAdd_
    markRemove();
}
void EventDispatcher::removeEventListenerForType(EventListener::Type type)
{
    auto &bundle = getEventListenerForType(type);
    // first mark it, and remove all
    bundle.markRemoveAll();
    //bundle.clearAllListeners();
    for(auto &p : toBeAdd_)
        if(p->getType()==type)
            p->markRemove();
    markRemove();
}
void EventDispatcher::removeAllEventListeners()
{
    nodeListenersMap_.clear();
    for(auto &bundle : listeners_){
        bundle.markRemoveAll();
    }
    toBeAdd_.clear();
    hasZombies_ = false;
    setDirty(true);
}
void EventDispatcher::pause_resumeForNode(Node *node, bool pause)
{
    if(node==nullptr)
        return;
    for(auto &p : toBeAdd_)
        if(p->getNode()==node)
            p->setPaused(pause);

    auto v = nodeListenersMap_.find(node);
    if(v==nodeListenersMap_.end())
        return;
    auto &vec = v->second;
    for(auto &p : vec){
        p->setPaused(pause);
    }
}
//void EventDispatcher::checkAndAddListeners__()
//{
//    for(auto it=toBeAdd_.begin(); it!=toBeAdd_.end(); it++){
//        const auto &l = it->get();
//        if(l->isZombie())
//            continue;
//        const auto type = l->getType();
//        auto &bundle = getEventListenerForType(type);
//        if(l->getFixedPriority()){
//            bundle.addFixedListener(l);
//        }else{
//            auto node = l->getNode();
//            bundle.addSceneListener(l);
//            addSceneListener__(l, node);
//        }
//    }
//    toBeAdd_.clear();
//    setDirty(true);
//}
//void EventDispatcher::addSceneListener__(EventListener *listener, Node *node)
//{
//    auto it = nodeListenersMap_.find(node);
//    if(it==nodeListenersMap_.end()){
//        std::vector<Ref_ptr<EventListener>> listeners;
//        listeners.push_back(listener);
//        nodeListenersMap_.insert(std::pair<Node*,std::vector<Ref_ptr<EventListener>>>(node, std::move(listeners)));
//    }else{
//        auto &v = it->second;
//        v.push_back(listener);
//    }
//}
//void EventDispatcher::updateListeners__()
//{
//}
void EventDispatcher::addEventListener(Ref_ptr<EventListener> &&l)
{
    auto node = l->getNode();
    auto v = nodeListenersMap_.find(node);
    if(v==nodeListenersMap_.end()){
        nodeListenersMap_.insert({node, {std::move(l)}});
    }else{
        v->second.push_back(std::move(l));
    }
}
void EventDispatcher::update()
{
    auto dirty = shouldRemove();
    if(toBeAdd_.size()>0){
        auto p = toBeAdd_.data();
        int n = toBeAdd_.size();
        for(int i=0; i<n; i++){
            if(!p[i]->isZombie()){
                addEventListener(std::move(p[i]));
            }
        }
        toBeAdd_.clear();
        dirty = true;
    }
    if(shouldRemove()){
        for(auto x=nodeListenersMap_.begin(); x!=nodeListenersMap_.end();){
            auto &vec = x->second;
            const auto n = vec.size();
            auto p = vec.data();
            int i=0;
            for(int j=0; j<n; j++){
                if(!p[j]->isZombie()){
                    if(i!=j)
                        p[i] = std::move(p[j]);
                    i++;
                }
            }
            if(i!=n)
                vec.resize(i);

            if(i==0)
                x = nodeListenersMap_.erase(x);
            else
                ++x;
        }
    }

    // sort scene based listeners

}
void visitScene(const Node *root, std::vector<const Node *> &v, const std::unordered_map<const Node*, std::vector<Ref_ptr<EventListener>>> &map)
{
    const auto &children = root->getChildren();
    int i=children.size()-1;
    for(; i>=0; i--){
        const auto &x = children[i].get();
        if(x->getZindex()<0)
            break;
        visitScene(x, v, map);
    }
    if(map.find(root) != map.end())
        v.push_back(root);
    for(; i>=0; i--){
        const auto &x = children[i].get();
        visitScene(x, v, map);
    }
}

void EventDispatcher::sortListeners(const Node *root)
{
    for(auto &l : listeners_)
        l.clearAllListeners();
    std::vector<const Node *> sceneNodes;
    // collect sceneNodes;
    //
    visitScene(root, sceneNodes, nodeListenersMap_);

    for(auto &node : sceneNodes){
        auto &vec = nodeListenersMap_[node];
        for(auto &listener : vec){
            auto type = listener->getType();
            auto &bundle = getEventListenerForType(type);
            if(listener->getFixedPriority() == 0){
                bundle.addSceneListener(listener);
            }else{
                bundle.addFixedListener(listener);
            }
        }
    }
    // fixed listener will node==nullptr
    auto s0 = nodeListenersMap_.find(nullptr);
    if(s0!=nodeListenersMap_.end()){
        auto &v = s0->second;
        for(auto &l : v){
            auto type = l->getType();
            auto &bundle = getEventListenerForType(type);
            bundle.addFixedListener(l);
        }
    }
    // sort fixed
    for(auto &b : listeners_){
        b.sortFixedListeners();
    }
}
