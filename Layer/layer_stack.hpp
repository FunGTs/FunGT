#if !defined(_LAYER_STACK_H_)
#define _LAYER_STACK_H_

#include <vector>
#include <algorithm>
#include <memory>
#include "layer.hpp"


class LayerStack {
    public:
        LayerStack() = default;
        ~LayerStack() = default;

        void PushLayer(std::unique_ptr<Layer> layer) {
            layer->onAttach();
            m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, std::move(layer));
            m_LayerInsertIndex++;
            
        }

        void PushOverlay(std::unique_ptr<Layer> overlay) {
            overlay->onAttach();
            m_Layers.emplace_back(std::move(overlay));
           
        }

        void PopLayer(std::unique_ptr<Layer> layer) {
            auto it = std::find_if(m_Layers.begin(), m_Layers.end(), 
                [&] (const std::unique_ptr<Layer>& currLayer) {return currLayer == layer; });
            if (it != m_Layers.end()) {
                (*it)->onDetach();
                m_Layers.erase(it);
                m_LayerInsertIndex--;
            }
        }

        std::vector<std::unique_ptr<Layer>>::iterator begin() { return m_Layers.begin(); }
        std::vector<std::unique_ptr<Layer>>::iterator end() { return m_Layers.end(); }

    private:
        std::vector<std::unique_ptr<Layer>> m_Layers;
        unsigned int m_LayerInsertIndex = 0;
};

#endif // _LAYER_STACK_H_
