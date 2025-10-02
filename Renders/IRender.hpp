#if !defined(_IRENDER_H_)
#define _IRENDER_H_

enum class RenderAPI{

    None   = 0,
    OpenGL = 1,
    Vulkan = 2

};



class IRender{
    public:
        virtual void render() = 0; //pure virtual function
        virtual void onKey() = 0; //pure virtual function
        virtual ~IRender() = default; //virtual destructor
};


#endif // _IRENDER_H_
