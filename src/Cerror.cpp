#include <Cerror.hpp>

const Cerror Cerror::Eexist = Cerror("@not exist");
const Cerror Cerror::Eperm = Cerror("@perm denied");
const Cerror Cerror::Enospace = Cerror("@no space");
const Cerror Cerror::Eio = Cerror("@Eio");
