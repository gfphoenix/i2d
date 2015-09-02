#ifndef ACTIONINSTANT_HPP
#define ACTIONINSTANT_HPP
#include "Action.hpp"
#include "mm.hpp"
#include "Node.hpp"

class Show : public ActionInstant
{
    public:
        inline static Show * create(){return MM<Show>::New();}
    protected:
        Show * clone() const override;
        ActionInstant * reverse() const override;
        virtual void update()override{node_->setVisible(true);}
};
class Hide : public ActionInstant
{
    public:
        inline static Hide * create(){return MM<Hide>::New();}
    protected:
        Hide * clone() const override;
        ActionInstant * reverse() const override;
        virtual void update()override{node_->setVisible(false);}
};
class ToggleVisibility : public ActionInstant
{
    public:
        inline static ToggleVisibility * create(){return MM<ToggleVisibility>::New();}
    protected:
        ToggleVisibility * clone() const override{return create();}
        ToggleVisibility * reverse() const override{return create();}
        virtual void update()override{node_->setVisible(!node_->isVisible());}
};
#endif // ACTIONINSTANT_HPP
