#if !defined(_INFO_WINDOW_H_)
#define _INFO_WINDOW_H_
#include<string>
#include "../GUI/gui.hpp"
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
