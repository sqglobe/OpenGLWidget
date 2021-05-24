#include "trianglerenderer.h"
#include <epoxy/gl.h>
#include <iostream>

static const char * vShaderStr =
      "#version 410\n"
      "layout (location = 0) in vec3 vPosition;    \n"
      "uniform mat4 OrthoMatrix; \n"
      "uniform mat4 TransformMatrix; \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = TransformMatrix* OrthoMatrix * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);  \n"
      "}                            \n";

   static const char * fShaderStr =
      "#version 410\n"
      "void main()                                  \n"
      "{                                            \n"
      "  gl_FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
      "}                                            \n";

static int getUniformLocation(GLuint program, const char *name)
{
    auto res = glGetUniformLocation(program, name);

    if(res < 0)
    {
        std::cerr << name << " uniform is not found" << std::endl;
    }

    return res;
}

static GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;

   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
    return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, nullptr );

   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled )
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         std::string infoLog(static_cast<std::size_t>(infoLen), '\0');

         glGetShaderInfoLog ( shader, infoLen, nullptr, &infoLog[0] );
         std::cerr <<  "Error compiling shader: " << infoLog << std::endl;

      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;

}

TriangleRenderer::TriangleRenderer(const std::vector<float> &vertices):
    m_VerticiesCount(vertices.size())
{



       GLuint vertexShader;
         GLuint fragmentShader;
         GLint linked;

         // Load the vertex/fragment shaders
         vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
         fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

         // Create the program object
         programID = glCreateProgram ( );

         if ( programID == 0 )
         {
             abort();
         }

         glAttachShader ( programID, vertexShader );
         glAttachShader ( programID, fragmentShader );

         // Link the program
         glLinkProgram ( programID );

         // Check the link status
         glGetProgramiv ( programID, GL_LINK_STATUS, &linked );

         if ( !linked )
         {
            GLint infoLen = 0;

            glGetProgramiv ( programID, GL_INFO_LOG_LENGTH, &infoLen );

            if ( infoLen > 1 )
            {

               std::string infoLog(static_cast<std::size_t>(infoLen), '\0');

               glGetProgramInfoLog ( programID, infoLen, nullptr, &infoLog[0] );
               std::cerr <<  "Error link program: " << infoLog << std::endl;
            }

            glDeleteProgram ( programID );
            abort();
         }
         glDeleteShader(vertexShader);
         glDeleteShader(fragmentShader);
         glClearColor ( 1.0f, 1.0f, 0.0f, 0.0f );

         glGenVertexArrays(1, &VAO);
         glGenBuffers(1, &VBO);

         glUseProgram(programID);
         glBindBuffer(GL_ARRAY_BUFFER, VBO);
         glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float) * vertices.size()), &vertices[0], GL_STATIC_DRAW);

         transformLocation = getUniformLocation(programID, "TransformMatrix");

         auto orthoLocation = getUniformLocation(programID, "OrthoMatrix");
         glm::mat4 orthoMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
         glUniformMatrix4fv(orthoLocation, 1, GL_FALSE, &orthoMatrix[0][0]);

}

TriangleRenderer::~TriangleRenderer()
{

}


void TriangleRenderer::draw()
{
    glUseProgram(programID);
    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);



    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    auto rotate = glm::rotate(glm::mat4(1.0), m_Grad, glm::vec3(0.f,1.f,0.0f));
    m_Grad += .01f;

    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &rotate[0][0]);

    glEnableVertexAttribArray ( 0 );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, nullptr);

    glDrawArrays ( GL_TRIANGLES, 0, static_cast<GLint>(m_VerticiesCount / 3));
    glFlush();

}
