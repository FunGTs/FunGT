#include "display.hpp"

Vertex vertices[] = 
{
    //POSITION                         //COLOR                  //Texcoords            //NORMAL
    glm::vec3(-0.5,0.5f,0.f),      glm::vec3(1.f,0.f,0.f),   glm::vec2(0.f,1.f),    glm::vec3(0.f,0.f,1.f),
    glm::vec3(-0.5f,-0.5f,0.f),    glm::vec3(0.f,1.f,0.f),   glm::vec2(0.f,0.f),    glm::vec3(0.f,0.f,1.f),
    glm::vec3(0.5f,-0.5f,0.f),     glm::vec3(0.f,0.f,1.f),    glm::vec2(1.f,0.f),   glm::vec3(0.f,0.f,1.f),
    glm::vec3(0.5f,0.5f,0.f),      glm::vec3(1.f,1.f,0.f),     glm::vec2(1.f,1.f),  glm::vec3(0.f,0.f,1.f),
};
unsigned nOfvertices = sizeof(vertices)/sizeof(Vertex);
GLuint indices[] = 
{
    0,1,2,
    0,2,3
};
unsigned nOfIndices = sizeof(indices)/sizeof(GLuint);
Display::Display(){

}
Display::Display(int _width, int _height, std::string _name)
:   width{_width}, height{_height}, Windowname{_name}{
        std::cout<<"Display constructor\n";
        
}
Display::~Display(){
    std::cout<<"Display destructor\n";
    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
}
void Display::setBackground(float red, float green, float blue, float alfa){
    colors[0] = red; colors[1] = green; colors[2] = blue; colors[3] = alfa; 
}
int Display::set(){
    if (!glfwInit())
    return -1;
    // Tell GLFW what version of OpenGL we are using 
    // In this case we are using OpenGL 3.3 
    int frameBufferWidth = 0;
    int frameBufferHeight = 0;
     #ifdef __APPLE__
    /* We need to explicitly ask for a 3.2 context on OS X */
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #else
    //OpenGL version string: 4.6.0 NVIDIA 390.141 <--- ACTUAL ON MY PC
    //OpenGL version : 4.4.0 <--- Using in this code (First number is MAJOR, second is the MINOR)
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    #endif
    //FOR MAC USERS:

    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    window = glfwCreateWindow(width,height,Windowname.c_str(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwGetFramebufferSize(window,&frameBufferWidth,&frameBufferHeight);
    
    glViewport(0,0,frameBufferWidth,frameBufferWidth);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glClearColor(colors[0], colors[1],colors[2],colors[3]);
    glfwSwapBuffers(window);
    // Clean the back buffer and assign the new color to it
    //OPENGL OPTIONS

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

    #ifdef __APPLE__
        if(glfwInit()!=GL_TRUE)
        {
            std::cout<<"ERROR"<<std::endl;
        }
    #else
        if(glewInit()!=GLEW_OK)
        {
            std::cout<<"ERROR"<<std::endl;
        }
    #endif
    //OPENGL OPTIONS

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //Change to GL_FILL to actually fill the polygon
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    
    //Shader init

        #ifdef __APPLE__
        #define GLFW_INCLUDE_GLCOREARB
            Shader core_program{"../resources/vertex_core_OSX.glsl","../resources/fragment_core_OSX.glsl"};
        #else
            Shader core_program{"../resources/vertex_core.glsl","../resources/fragment_core.glsl"};
        #endif
    
        //MODEL MESH

        funGT::Mesh mesh{vertices,nOfvertices, indices,nOfIndices};
        // Setup Dear ImGui context


        imguiSetup(window);

        //INIT MATRICES
        glm::vec3 position(0.f);
        glm::vec3 rotation(0.f);
        glm::vec3 scale(1.f);

        //Projection matrix 


        glm::mat4 ModelMatrix(1.f);
        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        ModelMatrix = glm::scale(ModelMatrix, scale); 
 



        glm::vec3 positionCam  = glm::vec3(0.f,0.f,0.8f);
        glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
        glm::vec3 frontCam = glm::vec3(0.f, 0.f, -1.f);
        
        glm::mat4 ViewMatrix(1.f);
        ViewMatrix = glm::lookAt(positionCam, positionCam + frontCam, worldUp);

        float fov = 45.f; 
        float nearPlane = 0.1f; 
        float farPlane = 1000.f; 



        glm::mat4 ProjectionMatrix(1.f);
        ProjectionMatrix = glm::perspective(glm::radians(fov),static_cast<float>(frameBufferWidth)/frameBufferHeight, nearPlane, farPlane);
        //ProjectionMatrix = glm::ortho(-2.f, 2.f,-2.f, 2.f,-1.f, 1.f);





        //VAO, VBO, EBO 
        VAO vertexArrayObject{1};
        Texture texture{"../img/pusheen.png"};
        texture.bind();
        //core_program.setUniform1i("u_Texture",0);

        //Lights:
        glm::vec3 lightPos0(0.f, 0.f, 2.f);
        //MATERIAL
        Material material0(glm::vec3(0.1f), glm::vec3(1.f),glm::vec3(1.f), 0, 0);

        //Send projection matrices to the shader
        core_program.Bind(); //Important to bind shader
        core_program.setUniformMat4fv("ModelMatrix",ModelMatrix);
        core_program.setUniformMat4fv("ViewMatrix",ViewMatrix);
        core_program.setUniformMat4fv("ProjectionMatrix",ProjectionMatrix);
        core_program.setUniformVec3f(lightPos0,"lightPos0");
        core_program.setUniformVec3f(positionCam,"cameraPos");
        core_program.unBind(); //Important to unbind shader


        // generate VBO and bind and send DATA
        VB vertexBuffer{vertices,sizeof(vertices)};


        std::cout<<"Size of: "<<nOfvertices <<" vertices: "<<sizeof(vertices)<<std::endl;

        //GEN EBO and BIND and SEND DATA
        VI vertexIndices{indices,sizeof(indices)};

        std::cout<<"Size of: "<<nOfIndices <<" indices: "<<sizeof(indices)<<std::endl;
        printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));

        //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
        //POSITION 
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,position));
        glEnableVertexAttribArray(0);
        //COLOR
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,color));
        glEnableVertexAttribArray(1);
        //TEXTURE COORDS
        //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glEnableVertexAttribArray(2);
        //NORMAL COORDS
        //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
        glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
        glEnableVertexAttribArray(3);


        glBindVertexArray(0);    
       
    while (!glfwWindowShouldClose(window))
    {
         /* Render here */
        
         /*IMGUI*/
        imguiNewFrame();
        imguiFrameBasic(position, rotation);
        imguiRender();
        /*END IMGUI*/
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
         /* Poll for and process events */
        glfwPollEvents();
       
        
        texture.bind();
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //UPDATE
        //updateInput(window);
        //USE A PROGRAM
        core_program.Bind();
        //Update uniforms. Why? 
        //Uniforms are variables yuou send from the CPU to the GPU
        material0.sendToShader(core_program);
        //Move, rotate and scale
     
        //position.z -= 0.01f;
        ModelMatrix = glm::mat4(1.f);
        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        ModelMatrix = glm::scale(ModelMatrix, scale); 

        core_program.setUniformMat4fv("ModelMatrix",ModelMatrix);
        
        glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
        ProjectionMatrix = glm::mat4(1.0f);
        //Updating the projection matrix at each frame avoids the image stretches when we resize the window
        ProjectionMatrix = glm::perspective(glm::radians(fov),static_cast<float>(frameBufferWidth)/frameBufferHeight, nearPlane, farPlane);
        //ProjectionMatrix = glm::ortho(-2.f, 2.f,-2.f, 2.f,-1.f, 1.f);
        core_program.setUniformMat4fv("ProjectionMatrix",ProjectionMatrix);



        vertexBuffer.build();
        //bind vertex array object
        vertexArrayObject.build();

        //DRAW
        glDrawElements(GL_TRIANGLES,nOfIndices,GL_UNSIGNED_INT, 0);

        imguiRender();
       
        glFlush();
    }

    imguiCleanUp();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0; 
}