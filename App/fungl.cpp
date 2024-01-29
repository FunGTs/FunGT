#include "fungl.hpp"
bool firstMouse = true; 
float yaw = -90.f; 
float pitch = 0.0;
float lastXmouse = 1000/2.0; 
float lastYmouse = 1000/2.0;  
float mouseXin; 
float mouseYin;
glm::vec2 mouseInput(0.f); 
glm::vec3 cameraFront  = glm::vec3(0.0f,0.0f,-1.0f);
FunGL::FunGL(){

}
FunGL::FunGL(int _width, int _height, std::string _name)
:   width{_width}, height{_height}, Windowname{_name}{
        std::cout<<"FunGL constructor\n";
 
}
FunGL::~FunGL(){
    std::cout<<"FunGL destructor\n";
    // Delete window before ending the program
    glfwDestroyWindow(window);
    // Terminate GLFW before ending the program
    glfwTerminate();
}
void FunGL::setBackground(float red, float green, float blue, float alfa){
    colors[0] = red; colors[1] = green; colors[2] = blue; colors[3] = alfa; 
}

void FunGL::processInput(glm::vec3 &cameraUp, glm::vec3 &cameraFront, glm::vec3 &cameraPos,float &deltaTime)
{
  

    const float cameraSpeed = 2.5f*deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
    

}
void mouse_callback(GLFWwindow *window, double xpos,double ypos){
    // if(firstMouse){
    //     lastXmouse = xpos; 
    //     lastYmouse = ypos; 
    //     firstMouse = false; 
    // }
    // float xoffset = xpos - lastXmouse; 
    // float yoffset = lastYmouse - ypos; 
    // lastXmouse = xpos; 
    // lastYmouse = ypos; 
    // float sens = 0.1f; 
    // xoffset *= sens; 
    // yoffset *= sens; 
    // yaw += xoffset;
    // pitch += yoffset;

    // if(pitch>89.0f){
    //     pitch = 89.0f;
    // }
    // if(pitch<-89.0f){
    //     pitch = -89.0f;
    // }
    // glm::vec3 dir; 
    // dir.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
    // dir.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));
    // dir.y = sin(glm::radians(pitch));
    // cameraFront = glm::normalize(dir);
    mouseInput.x = xpos; 
    mouseInput.y = ypos; 
}
int FunGL::set(){
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
    
    glViewport(0,0,frameBufferWidth,frameBufferHeight);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, mouse_callback);

    glClearColor(colors[0], colors[1],colors[2],colors[3]);
    glfwSwapBuffers(window);
    // Clean the back buffer and assign the new color to it
    //OPENGL OPTIONS
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); //GL_LINE for just lines //GL_FILL for fill color
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

    #ifdef __APPLE__
        #define GLFW_INCLUDE_GLCOREARB
            Shader core_program{"../resources/vertex_core_OSX.glsl","../resources/fragment_core_OSX.glsl"};
        #else
            Shader core_program{"../resources/shaders_course/vertex_canvas.glsl","../resources/shaders_course/ex01_shader.glsl"};
        #endif

    // /* Here it starts to show an image*/
    Square square{};
    // Square cube{};
    // //Cube cube{"../img/box.jpg"};
    // //Pyramid pyramid{"../img/stone2.jpg"};
    // //VAO, hold data and send to graphics card 
    // GLuint VAO;
    // glCreateVertexArrays(1, &VAO);
    // glBindVertexArray(VAO); //Bind;
    // // //TEXTURE
    // // Texture texture{"../img/darkbrown.png"};
    // //  texture.bind();

    // //Generate VBO: sends data to the GPU
    // GLuint VBO;
    // glGenBuffers(1 /* One buffer*/, &VBO);
    // glBindBuffer(GL_ARRAY_BUFFER/*Target*/, VBO);
    // glBufferData(GL_ARRAY_BUFFER /*Target*/,cube.sizeOfVertices(),cube.getVertices(),GL_STATIC_DRAW /*Does not change once is sents*/); //data we are sending to the graphics card
    // // GEN EBO adn BIND and SEND DATA

    // GLuint EBO; //For indexing;
    // glGenBuffers(1,&EBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.sizeOfIndices(),cube.getIndices(),GL_STATIC_DRAW);

    // //Set Vertex Attributes pointers and enable n
    // //glVertexAttribPointer(0 /*First element: positions*/,3 /* 3 floats*/, GL_FLOAT/*Type*/,GL_FALSE, 3*sizeof(GLfloat)/*how much steps to the next vertex pos*/, (GLvoid*)0);
    // //glEnableVertexAttribArray(0); 
    // //SET VERTEXATTRIBPOINTERS AND ENABLE (INPUT ASSEMBLY)
    //     //POSITION 
    //     glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,position));
    //     glEnableVertexAttribArray(0);
    //     //COLOR
    //     glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,color));
    //     glEnableVertexAttribArray(1);
    //     //TEXTURE COORDS
    //     //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
    //     glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,texcoord));
    //     glEnableVertexAttribArray(2);
    // //Bind VAO

    // glBindVertexArray(0);

    //Shader init
    // UNIFORMS
    float timeValue = glfwGetTime();
    // float greenValue = (sin(timeValue)/2.0f)+0.5f;
    // core_program.setUniform4f(0.f,greenValue,0.f,"change_color");
    // core_program.Bind();
     
    // Setup Dear ImGui context
    // ctx1 = ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.Fonts->AddFontDefault();
    // ImFont* font1 = io.Fonts->AddFontFromFileTTF("../Fonts/Autobusbold-1ynL.ttf",20.0f);
    //
 
    //BALL 
    float frameSizeX = 1600.f; 
    float frameSizeY = 1000.f; 
   

    //Creating model matrix to perform movements 
     //INIT MATRICES
        glm::vec3 position(0.f);
        position.z = 1.0;
        glm::vec3 rotation(0.f);
        glm::vec3 scale(0.5f);

        //Projection matrix 
        float fov = 45.f; 
        float nearPlane = 0.1f; 
        float farPlane = 100.f; 
        glm::mat4 ProjectionMatrix(1.f);
        //ProjectionMatrix = glm::ortho(-2.f, 2.f,-2.f, 2.f,-1.f, 1.f);
        ProjectionMatrix = glm::perspective(glm::radians(fov),static_cast<float>(frameBufferWidth)/frameBufferHeight, nearPlane, farPlane);
        //Model Matrix
        glm::mat4 ModelMatrix(1.f);
        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        ModelMatrix = glm::scale(ModelMatrix, scale); 

        //Camera:
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f,0.0f,0.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos-cameraTarget);
        glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);
        glm::vec3 CameraRight = glm::normalize(glm::cross(up,cameraDirection));
        glm::vec3 cameraUP = glm::cross(cameraDirection,CameraRight);
        

        glm::mat4 ViewMatrix(1.f);
        ViewMatrix= glm::lookAt(cameraPos,cameraPos+cameraFront, cameraUP);

    //once the matrices have been created, let's send them to the shader
     core_program.Bind(); //Important to bind shader
        core_program.setUniformMat4fv("ModelMatrix",ModelMatrix);
        core_program.setUniformMat4fv("ViewMatrix",ViewMatrix);
        core_program.setUniformMat4fv("ProjectionMatrix",ProjectionMatrix);
        core_program.setUniform1f("frameSizeX",frameSizeX); 
        core_program.setUniform1f("frameSizeY",frameSizeY);
        core_program.setUniform1f("time",timeValue);
        core_program.setUniformVec2f(mouseInput,"mouseInput");
    core_program.unBind();
         float deltaTime = 0.0f; 
        float lastFrame = 0.0f;
    int keydraw = 1; 
    while (!glfwWindowShouldClose(window))
    {
         /* Render here */
        
         /*IMGUI*/
    
        
        /*END IMGUI*/
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
         /* Poll for and process events */
        glfwPollEvents();
       
        //texture.bind();
     
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //UPDATE
        //updateInput(window);
        //USE A PROGRAM

        core_program.Bind();
  
        rotation.y = (float)glfwGetTime()*10.0;
        rotation.x = (float)glfwGetTime()*10.0;
        rotation.z = (float)glfwGetTime()*10.0;
        ModelMatrix = glm::mat4(1.f);
        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)); 
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        ModelMatrix = glm::scale(ModelMatrix, scale); 

        core_program.setUniformMat4fv("ModelMatrix",ModelMatrix);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame-lastFrame; 
        lastFrame = currentFrame; 
        processInput(cameraUP,cameraFront,cameraPos,deltaTime);
        glm::mat4 ViewMatrix(1.f);
        ViewMatrix = glm::lookAt(cameraPos,cameraPos+cameraFront, cameraUP);
        core_program.setUniformMat4fv("ViewMatrix",ViewMatrix);
        core_program.setUniformVec2f(mouseInput,"mouseInput");
        core_program.setUniform1f("time",currentFrame);
        // if(glfwGetKey(window, GLFW_KEY_1)){
        //     keydraw = 1; 
            
        // }
        // if(glfwGetKey(window, GLFW_KEY_2)){
        //     keydraw=2; 
            
        // }
        // if(keydraw==1){
        //     cube.draw();
        // }
        // else{
        //     pyramid.draw();
        // }
        //Bind vertex array object
        // glBindVertexArray(VAO);
        square.draw();
       
        
        // //DRAW
        // glDrawElements(GL_TRIANGLES,cube.getNumOfIndices(), GL_UNSIGNED_INT,0);
        // //imguiRender();
       
        glFlush();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 1; 
}