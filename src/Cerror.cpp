#include <Cerror.hpp>

Cerror Cerror::Eexist = Cerror("@not exist");
Cerror Cerror::Eperm = Cerror("@perm denied");
Cerror Cerror::Enospace = Cerror("@no space");
Cerror Cerror::Eio = Cerror("@Eio");
