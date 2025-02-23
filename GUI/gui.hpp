#if !defined(_GUI_H_)
#define _GUI_H_
#include "../include/prerequisites.hpp"
#include "../include/imgui_headers.hpp"

class FunGT; // Forward declaration
class GUI {
    friend class FunGT; // Declare FunGT as a friend class
    private:
        GLFWwindow *m_window;

    private:
        void cleanUp();
    public:
        GUI();
        ~GUI();
        virtual void renderGUI() = 0;
    protected:
        void setup(GLFWwindow &window);
        void newFrame();
        void render();


};

#endif // _GUI_H_
