# i2d
a simple and clean designed 2d game engine
(REFACTORING NOW)

# Design Phylosophy:
1. a simple and clean API to use, no noise
2. excellent performance
3. easy to extend

# Dependency:
glfw, GL, GLEW, glm and a c++11 compatible compiler

# Resource-Manager:
1. simple base class for resource manager
2. existing texture manager that can automatically load and unload texture resource based on ref-count
3. only sync loading exists, lack of async loading.

# Render:
Node based render tree has three core components:

1. Node : base tree node, has basic infomation about the node
2. StageLayer: a Stage which has its own camera and resolution policy
3. Scene : a scene manager which manages one or more StageLayer, input event, scheduler, etc.

# ActionManager & Scheduler:
ActionManager and Scheduler are owned by a Scene, not globally.

# Event-Listener:
all input handler is registered by attaching a specific event listener to an existing node


# TO DO NEXT:
1. Pen and Brush to help primitive drawing
2. Abstract File API
3. Enhance event handler
4. Review code and fix bugs
5. more...
