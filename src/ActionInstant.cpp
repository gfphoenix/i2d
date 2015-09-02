#include <ActionInstant.hpp>
#include <mm.hpp>

Show * Show::clone() const{return create();}
ActionInstant * Show::reverse() const{return Hide::create();}
Hide * Hide::clone() const{return create();}
ActionInstant * Hide::reverse() const{return Show::create();}

