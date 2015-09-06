#ifndef _SHADER_HPP_
#define _SHADER_HPP_
#include "config.hpp"
#include "Ref.hpp"

class Node;
class Renderer;
class Shader : public Ref {
    public:
        static GLuint loadStrings(const char *vertexString, const char *fragString);
        static GLuint loadFiles(const char *vertexFile, const char *fragFile);

        inline void Use()const{glUseProgram(id_);}
        inline GLint getAttribLocation(const GLchar *name)const{
            return glGetAttribLocation(id_, name);
        }
        inline GLint getUniformLocation(const GLchar *name)const{
            return glGetUniformLocation(id_, name);
        }
        virtual void Render(Node *node)=0;
        virtual void Flush()=0;

        //inline void getUniformfv(GLint location, GLfloat *params) const {
        //  glGetUniformfv(id_, location, params);
        //}
        //inline void getUniformiv(GLint location, GLint *params) const {
        //  glGetUniformiv(id_, location, params);
        //}
        //inline void getUniformdv(GLint location, GLdouble *params) const {
        //glGetUniformdv(id_, location, params);
        //}
        //inline void getUniformuiv(GLint location, GLuint *params) const {
        //  glGetUniformuiv(id_, location, params);
        //}
        //inline void setUniform() const {}

    protected:
        friend class Renderer;
	Shader();
	Shader(GLuint id):id_(id){}
    inline void setRenderer(Renderer *renderer){renderer_=renderer;}
    inline Renderer *getRenderer()const{return renderer_;}
    inline void setProgramId(GLuint id){id_=id;}
    inline GLuint getShaderId() const { return id_; }
    inline void Unuse()const{glUseProgram(0);}
    virtual ~Shader();
    private:
        Renderer *renderer_;
        GLuint id_;
};

#endif
