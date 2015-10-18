TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_INCDIR += include
QMAKE_CXXFLAGS	+= -std=c++11

HEADERS += \
    include/input/input.hpp \
    include/input/input_linux.hpp \
    include/Action.hpp \
    include/ActionInstant.hpp \
    include/ActionInterpolation.hpp \
    include/ActionInterval.hpp \
    include/ActionManager.hpp \
    include/App.hpp \
    include/bits.hpp \
    include/Camera.hpp \
    include/Cerror.hpp \
    include/config.hpp \
    include/Director.hpp \
    include/Event.hpp \
    include/EventListener.hpp \
    include/File.hpp \
    include/G.hpp \
    include/GLView.h \
    include/kvdb.hpp \
    include/Layer.hpp \
    include/Listener.hpp \
    include/ListenerVector.hpp \
    include/Log.hpp \
    include/Main.hpp \
    include/mm.hpp \
    include/Node.hpp \
    include/noncopyable.hpp \
    include/PrimitiveShader.hpp \
    include/ps_hpp.hpp \
    include/rectpacking.hpp \
    include/Ref.hpp \
    include/Renderer.hpp \
    include/ResourceManager.hpp \
    include/Scene.hpp \
    include/Scheduler.hpp \
    include/Shader.hpp \
    include/Sprite.hpp \
    include/SpriteShader.hpp \
    include/StageLayer.hpp \
    include/String.hpp \
    include/Texture.hpp \
    include/TextureLoader.hpp \
    include/TextureManager.hpp \
    include/types.hpp \
    include/VertexUVColor.hpp \
    src/stb_image.h \
    src/stb_image_write.h \
    include/glyph.hpp \
    include/Pen.hpp \
    include/Brush.hpp \
    include/Label.hpp \
    include/BMFont.hpp

SOURCES += \
    src/Action.cpp \
    src/ActionInstant.cpp \
    src/ActionInterpolation.cpp \
    src/ActionInterval.cpp \
    src/ActionManager.cpp \
    src/App.cpp \
    src/App_linux.cpp \
    src/Cerror.cpp \
    src/Director.cpp \
    src/Event.cpp \
    src/EventListener.cpp \
    src/GLView_linux.cpp \
    src/Main.cpp \
    src/Node.cpp \
    src/PrimitiveShader.cpp \
    src/rectpacking.cpp \
    src/Ref.cpp \
    src/Renderer.cpp \
    src/ResourceManager.cpp \
    src/Scene.cpp \
    src/Scheduler.cpp \
    src/Shader.cpp \
    src/Sprite.cpp \
    src/SpriteShader.cpp \
    src/StageLayer.cpp \
    src/Texture.cpp \
    src/TextureLoader.cpp \
    src/TextureManager.cpp \
    src/glyph.cpp \
    src/Pen.cpp \
    src/Brush.cpp \
    src/Label.cpp \
    src/BMFont.cpp \
    src/Log.cpp


unix: LIBS += -lglfw
unix: LIBS += -lGL
unix: LIBS += -lGLEW
