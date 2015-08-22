#ifndef _SHADER_HPP_
#define _SHADER_HPP_
#include "config.hpp"
#include "Ref.hpp"


class Node;
class Renderer;
class Scene;
class StageLayer;
class Shader : public Ref {
    public:
        static GLuint loadStrings(const char *vertexString, const char *fragString);
        static GLuint loadFiles(const char *vertexFile, const char *fragFile);
        static Shader *getDefaultShader();
        inline void use()const{glUseProgram(id_);}
        virtual const std::string getInfo()const{return "Shader ";}

//	inline void addRenderCall(Scene *scene){scene->addRenderCall();}
        inline GLint getAttribLocation(const GLchar *name)const{
            return glGetAttribLocation(id_, name);
        }
        inline GLint getUniformLocation(const GLchar *name)const{
            return glGetUniformLocation(id_, name);
        }
        inline GLint getAttribLocation(const GLchar *name){
            return glGetAttribLocation(id_, name);
        }
        inline GLint getUniformLocation(const GLchar *name){
            return glGetUniformLocation(id_, name);
        }
        //virtual void LimitBatchNumber(int min, int max)=0;
        virtual void Use(Renderer *renderer)=0;
        virtual void Render(Renderer *renderer, Node *node)=0;
        virtual void Flush(Renderer *renderer)=0;

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
	Shader();
	Shader(GLuint id):id_(id){}
	inline void setProgramId(GLuint name){id_=name;}
    inline GLuint getShaderId() const { return id_; }

    inline void Use()const{glUseProgram(id_);}
    inline void Unuse()const{glUseProgram(0);}
    virtual ~Shader();
    private:
        GLuint id_;
};

#endif
