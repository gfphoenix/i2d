#include <stdio.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/gl.h>
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

GLuint Shader::loadStrings(const char *vertexString, const char *fragString)
{
    GLint Result = GL_FALSE;
    int InfoLogLength;

    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile Vertex Shader
    glShaderSource(VertexShaderID, 1, &vertexString, nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    if(Result != GL_TRUE){
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            //std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
            char VertexShaderErrorMessage[InfoLogLength+2];
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
            printf("vshader error: %s\n", &VertexShaderErrorMessage[0]);
        }
        return 0;
    }

    // Compile Fragment Shader
    glShaderSource(FragmentShaderID, 1, &fragString , nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    if(Result != GL_TRUE){
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            //std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
            char FragmentShaderErrorMessage[InfoLogLength+2];
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
            printf("fshader error: %s\n", &FragmentShaderErrorMessage[0]);
        }
        return 0;
    }

    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    if(Result != GL_TRUE){
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if ( InfoLogLength > 0 ){
            char ProgramErrorMessage[InfoLogLength+2];
            glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }
        return 0;
    }

//    glDeleteShader(VertexShaderID);
//    glDeleteShader(FragmentShaderID);

    // Link the program
    printf("Create shader program ok!\n");
    return ProgramID;
}
Shader::Shader():
    id_(0){}
Shader::~Shader()
{
    glDeleteShader(id_);
}
