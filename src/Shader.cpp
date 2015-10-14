#include <stdio.h>
#include <iostream>
#include <fstream>
#include <gl>
#include "Shader.hpp"

// make it to use an abstract file apis
GLuint Shader::loadFiles(const char * vertex_file_path,const char * fragment_file_path){

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open()){
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }else{
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", fragment_file_path);
        return 0;
    }

    //printf("vert:\n%s\n", VertexShaderCode.c_str());
    //printf("frag:\n%s\n", FragmentShaderCode.c_str());
    return loadStrings(VertexShaderCode.c_str(), FragmentShaderCode.c_str());
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
        printf("shader(type=%u) error: %s\n", (unsigned)type, &errMsg[0]);
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
            fprintf(stderr, "%s\n", &errMsg[0]);
        }
        id=0;
        goto out;
    }
    printf("Create shader program ok!\n");
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
