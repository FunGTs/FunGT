#include "infowindow.hpp"



InfoWindow::InfoWindow()
:GUI(){
    std::cout<<"InfoWindow constructor"<<std::endl;
  
}
void InfoWindow::showInfoWindow(){
    
    if (!ImGui::GetCurrentContext()) {
       
        return;  // Avoid crash
    }
    else{
        
    }
     glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
     glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
     glRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    {
            ImGui::Begin("OpenGL Info");
            ImGui::Text("Running on: %s", OS_NAME);  // Display OS name
            ImGui::Text("OpenGL Version: %s", glVersion.c_str());
            ImGui::Text("Vendor: %s", glVendor.c_str());
            ImGui::Text("Renderer: %s", glRenderer.c_str());
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
    }
   
}

void InfoWindow::renderGUI()
{
    this->newFrame();
    //std::cout<<"InfoWindow::renderGUI"<<std::endl;
    this->showInfoWindow();
    //std::cout<<"ending InfoWindow::renderGUI"<<std::endl;
    this->render();
}

InfoWindow::~InfoWindow()
{
    std::cout<<"InfoWindow destructor"<<std::endl; 
}
