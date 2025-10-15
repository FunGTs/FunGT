#if !defined(_DISPLAY_GRAPHICS_H_)
#define _DISPLAY_GRAPHICS_H_
enum class Backend {
    OpenGL,
    Vulkan,
    Metal
};
class DisplayGraphics {
public:
    static void SetBackend(Backend api) { s_API = api; }
    static Backend GetBackend() { return s_API; }

private:
    static Backend s_API;
};




#endif // _DISPLAY_GRAPHICS_H_
