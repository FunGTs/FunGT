#include "infowindow.hpp"



InfoWindow::InfoWindow()
:GUI(){
    std::cout<<"InfoWindow constructor"<<std::endl;
  
}
void InfoWindow::showInfoWindow(){
    std::cout<<"InfoWindow::showInfoWindow"<<std::endl;
    if (!ImGui::GetCurrentContext()) {
        std::cerr << "Error: ImGui context is not created!" << std::endl;
        return;  // Avoid crash
    }
    else{
        std::cout<<"NO ERROR: InfoWindow::showInfoWindow"<<std::endl;
    }
     glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
     glVendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
     glRenderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    {
            ImGui::Begin("OpenGL Info");
             ImGui::Text("OpenGL Version: %s", glVersion.c_str());
             ImGui::Text("Vendor: %s", glVendor.c_str());
             ImGui::Text("Renderer: %s", glRenderer.c_str());
             ImGui::End();
    }
    std::cout<<"ending InfoWindow::showInfoWindow"<<std::endl;
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
