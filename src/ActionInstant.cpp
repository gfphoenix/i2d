#include <ActionInstant.hpp>
#include <mm.hpp>

Show * Show::clone() const{return MM<Show>::New();}
ActionInstant * Show::reverse() const{return MM<Hide>::New();}
Hide * Hide::clone() const{return MM<Hide>::New();}
ActionInstant * Hide::reverse() const{return MM<Show>::New();}

