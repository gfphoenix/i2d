# i2d
a simple and clean designed 2d game engine

# Design Phylosophy:
1. a simple and clean API to use, no noise
2. excellent performance
3. easy to extend

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
central manager of action and scheduler

# Event-Listener:
all input handler is registered by attaching a specific event listener to an existing node

# SOME EIXSTING PROBLEAMS:
1. mouse scroll is handled not correctly!
2. there is a "jumping" when moving an object by mouse. I don't know why

# RE-DESIGN OR OPTIMIZE ???
I want the ActionManager and Scheduler are owned by Scene, not globally.
1. ActionManager : a new design or just optimized
2. Scheduler : a new design or just optimized

 
# TO DO NEXT:
1. point transformation between nodes (done)
2. re-design Sprite
3. defaultShader : should design a new api to easily draw more objects in a single node
4. multi-thread and async loading files
5. font.
6. more...
