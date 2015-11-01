#include <Buffer.hpp>
#include <File.hpp>
#include <Log.hpp>
#include <gl>
#include "Shader.hpp"

// make it to use an abstract file apis
GLuint Shader::loadFiles(const char * vertex_file_path,const char * fragment_file_path){
    auto fv = FileUtils::Open(vertex_file_path);
    auto ff = FileUtils::Open(fragment_file_path);
    if(!fv ||!ff)
        return 0;
    auto lenv = fv->Length();
    auto lenf = ff->Length();

    BufferA bv(lenv+1);
    BufferA bf(lenf+1);

    auto nv = fv->Read(bv.Addr(), bv.Size());
    auto nf = ff->Read(bf.Addr(), bf.Size());
    if(nv!=(int)lenv || nf!=(int)lenf){
        Log::d("read all data of shader file failed");
        return 0;
    }
    bv.Addr()[nv] = bf.Addr()[nf] = '\0';
    return loadStrings((char*)bv.Addr(), (char*)bf.Addr());
}

GLuint Shader::compileShader(GLenum type, const char *src)
{
    GLuint id = glCreateShader(type);
    GLint Result = GL_FALSE;
    int InfoLogLength;
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Check Shader
    glGetShaderiv(id, GL_COMPILE_STATUS, &Result);

    if(Result == GL_TRUE)
        return id;

    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0){
        char errMsg[InfoLogLength+2];
        glGetShaderInfoLog(id, InfoLogLength, nullptr, &errMsg[0]);
        Log::e("shader(type=%u) error: %s\n", (unsigned)type, &errMsg[0]);
    }
    return 0;
}
GLuint Shader::linkShader(GLuint s1, GLuint s2)
{
    GLuint id=0;
    GLint Result = GL_FALSE;
    if(s1==0 || s2==0)
        goto out;
    id = glCreateProgram();
    glAttachShader(id, s1);
    glAttachShader(id, s2);
    glLinkProgram(id);

    // Check the program
    glGetProgramiv(id, GL_LINK_STATUS, &Result);
    if(Result != GL_TRUE){
        int InfoLogLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            char errMsg[InfoLogLength+2];
            glGetProgramInfoLog(id, InfoLogLength, nullptr, &errMsg[0]);
            Log::e("%s\n", &errMsg[0]);
        }
        id=0;
        goto out;
    }
    Log::i("Create shader program ok!\n");
out:
    return id;
}

GLuint Shader::loadStrings(const char *vertexString, const char *fragString)
{
    GLuint ProgramID=0;
    GLuint VertexShaderID = compileShader(GL_VERTEX_SHADER, vertexString);
    GLuint FragmentShaderID = compileShader(GL_FRAGMENT_SHADER, fragString);
    ProgramID = linkShader(VertexShaderID, FragmentShaderID);

    if(VertexShaderID)
        glDeleteShader(VertexShaderID);
    if(FragmentShaderID)
        glDeleteShader(FragmentShaderID);
    return ProgramID;
}
Shader::Shader():
    id_(0){}
Shader::~Shader()
{
    glDeleteShader(id_);
}
