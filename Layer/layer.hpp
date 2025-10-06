#if !defined(_LAYER_H_)
#define _LAYER_H_
#include<string>

class Layer{
    protected:
        std::string m_debugName;
    public:
        Layer(const std::string& name = "Layer") : m_debugName(name) {}
        virtual ~Layer() = default;

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate() {}
        virtual void onImGuiRender() {}

        inline const std::string& getName() const { return m_debugName; }

};


#endif // _LAYER_H_
