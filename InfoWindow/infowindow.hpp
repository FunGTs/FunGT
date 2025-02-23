#if !defined(_INFO_WINDOW_H_)
#define _INFO_WINDOW_H_
#include<string>
#include "../GUI/gui.hpp"
#if defined(_WIN32) || defined(_WIN64)
    #define OS_NAME "Windows"
#elif defined(__APPLE__) || defined(__MACH__)
    #define OS_NAME "macOS"
#elif defined(__linux__)
    #define OS_NAME "Linux"
#elif defined(__unix__)
    #define OS_NAME "Unix"
#elif defined(__FreeBSD__)
    #define OS_NAME "FreeBSD"
#else
    #define OS_NAME "Unknown OS"
#endif
class InfoWindow : public GUI{

    private:
        std::string glVersion;
        std::string glVendor;
        std::string glRenderer;

        public:
            InfoWindow();
            void showInfoWindow();
            void renderGUI() override;
            ~InfoWindow();


};

#endif // _INFO_WINDOW_H_
