#ifndef REDNERER
#define REDNERER
class Shader;
class StageLayer;
class Renderer
{
    Shader *last_;
public:
    inline void Use(Shader *shader, StageLayer *stage);
    void Flush(StageLayer *stage);

};

#endif // REDNERER

