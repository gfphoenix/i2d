#ifndef _EVENTDISPATCHER_HPP_
#define _EVENTDISPATCHER_HPP_
#include "Ref.hpp"
#include "Event.hpp"
#include "EventListener.hpp"
#include "GLView_linux.hpp"

#include <algorithm>
#include <vector>
#include <unordered_map>
class EventListener;
class Node;
class EventDispatcher : public Ref
{
    private:
        EventKeyboard eventKey[5];
    public:
        EventDispatcher();
        ~EventDispatcher(){}
        void addEventListenerWithScenePriority(EventListener *listener, Node *);
        void addEventListenerWithFixedPriority(EventListener *listener, int priority, Node *node);
        void removeEventListener(EventListener *listener);
        void removeEventListenerForNode(Node *node);
        void removeEventListenerForType(EventListener::Type type);
        void removeAllEventListeners();
        inline void pauseEventListener(EventListener *listener){listener->setPaused(true);}
        inline void resumeEventListener(EventListener *listener){listener->setPaused(false);}
        inline void pauseEventListenersForNode(Node *node){pause_resumeForNode(node, true);}
        inline void resumeEventListenersForNode(Node *node){pause_resumeForNode(node,false);}

        void dispatchEvent(Event *Event);
        void handleEventKey(KeyCode code, KeyAction action, KeyMods);
        inline bool isEnabled()const{return enabled_;}
        inline void setEnabled(bool enable){enabled_=enable;}
    private:
        class ListenerBundle
        {
            std::vector<Ref_ptr<EventListener>> *fixedPriorityListeners_;
            std::vector<Ref_ptr<EventListener>> *scenePriorityListeners_;
            //std::vector<EventListener*> *fixedPriorityListeners_;
            //std::vector<EventListener*> *scenePriorityListeners_;
            private:
            ListenerBundle();
            ~ListenerBundle();
            inline void addListener(std::vector<Ref_ptr<EventListener>> *&listeners, const Ref_ptr<EventListener> &l)
            {
                if(listeners==nullptr)
                    listeners = new std::vector<Ref_ptr<EventListener>>();
                listeners->push_back(l);
            }
            inline void addSceneListener(const Ref_ptr<EventListener> &l)
            {
                addListener(scenePriorityListeners_, l);
            }
            inline void addFixedListener(const Ref_ptr<EventListener> &l)
            {
                addListener(fixedPriorityListeners_, l);
            }
            void cleanupFixedZombieListeners();
            void cleanupSceneZombieListeners();
            inline void removeBundleListener(EventListener *l, std::vector<Ref_ptr<EventListener>> *pListeners)
            {
                if(l == nullptr || pListeners == nullptr)
                    return;
                std::remove_if(pListeners->begin(), pListeners->end(),
                        [l](const Ref_ptr<EventListener> &x){
                        return x==l;
                        });
            }
            inline void removeSceneListener(EventListener *l)
            {
                removeBundleListener(l, scenePriorityListeners_);
            }
            inline void removeFixedListener(EventListener *l)
            {
                removeBundleListener(l, fixedPriorityListeners_);
            }

            inline void markRemove(std::vector<Ref_ptr<EventListener>> *ls)
            {
                if(ls != nullptr)
                    for(auto &p : *ls)
                        p->markRemove();
            }
            inline void markRemoveScene(){markRemove(scenePriorityListeners_);}
            inline void markRemoveFixed(){markRemove(fixedPriorityListeners_);}
            inline void markRemoveAll(){markRemoveScene();markRemoveFixed();}
            inline void clearSceneListeners(){if(scenePriorityListeners_){scenePriorityListeners_->clear();}}
            inline void clearFixedListeners(){if(fixedPriorityListeners_){fixedPriorityListeners_->clear();}}
            inline void clearAllListeners(){clearFixedListeners();clearSceneListeners();}
            inline void sortFixedListeners()
            {
                if(fixedPriorityListeners_)
                    std::sort(fixedPriorityListeners_->begin(),
                            fixedPriorityListeners_->end(),
                            [](const Ref_ptr<EventListener> &l, const Ref_ptr<EventListener> &r){return l->getFixedPriority()<r->getFixedPriority();}
                            );
            }

            friend class EventDispatcher;
        };
        // pause : true for pause ; false for resume
        void pause_resumeForNode(Node *node, bool pause);
        inline void markRemove(){hasZombies_=true;}
        inline bool shouldRemove()const{return hasZombies_;}
        // check add and zombie + sort
        void update();
        void addEventListener(Ref_ptr<EventListener> &&l);

        // core
        //void checkAndAddListeners__();
        void sortListeners(const Node *root);
        void setDirty(bool dirtyOrder){dirty_sceneOrder_=dirtyOrder;}
        inline ListenerBundle &getEventListenerForType(EventListener::Type type){return listeners_[(int)type];}


        // data
        // Event-Listeners divided by listener type, each type has a slot.
        // and the slots have a uniform struct to store listeners which have scene graph based priority
        // and fixed priority
        ListenerBundle listeners_[(int)(EventListener::Type::Type_Number)];
        //std::vector<Ref_ptr<EventListener>> listeners_[(int)(EventListener::Type::Type_Number)];
        std::unordered_map<const Node *, std::vector<Ref_ptr<EventListener>> > nodeListenersMap_;
        std::vector<Ref_ptr<EventListener>> toBeAdd_;
        bool enabled_;
        bool dirty_sceneOrder_;
        bool hasZombies_;
};

#endif
