#if !defined(_GRAPHICS_RENDER_BACKEND_NEXT_VERTEX_FORMAT_HPP_)
#define _GRAPHICS_RENDER_BACKEND_NEXT_VERTEX_FORMAT_HPP_

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "../vendor/glm/include/glm/glm.hpp"

namespace GraphicsRenderBackendNext {

enum class AttrBaseType {
    Float,
    Int,
    UInt
};

struct VertexAttribute {
    std::string m_name;
    uint32_t m_count;
    AttrBaseType m_baseType;
    bool m_isInt;
    uint32_t m_offset;
};

struct VertexFormat {
    uint32_t m_stride = 0;
    std::vector<VertexAttribute> m_attributes;

    VertexFormat& add(std::string name, uint32_t count, AttrBaseType baseType,
                      bool isInt, uint32_t offset)
    {
        m_attributes.push_back({ std::move(name), count, baseType, isInt, offset });
        return *this;
    }
};

template<typename T> struct AttrTraits;

template<> struct AttrTraits<float>      { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 1; static constexpr bool isInt = false; };
template<> struct AttrTraits<int>        { static constexpr AttrBaseType baseType = AttrBaseType::Int;   static constexpr uint32_t count = 1; static constexpr bool isInt = true;  };
template<> struct AttrTraits<uint32_t>   { static constexpr AttrBaseType baseType = AttrBaseType::UInt;  static constexpr uint32_t count = 1; static constexpr bool isInt = true;  };
template<> struct AttrTraits<glm::vec2>  { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 2; static constexpr bool isInt = false; };
template<> struct AttrTraits<glm::vec3>  { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 3; static constexpr bool isInt = false; };
template<> struct AttrTraits<glm::vec4>  { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 4; static constexpr bool isInt = false; };
template<> struct AttrTraits<glm::uvec4> { static constexpr AttrBaseType baseType = AttrBaseType::UInt;  static constexpr uint32_t count = 4; static constexpr bool isInt = true;  };

template<typename T, std::size_t N> struct AttrTraits<T[N]> {
    static constexpr AttrBaseType baseType = AttrTraits<T>::baseType;
    static constexpr uint32_t count = static_cast<uint32_t>(N);
    static constexpr bool isInt = AttrTraits<T>::isInt;
};

template<typename VertexType, typename AttributeType>
inline void addVertexAttribute(VertexFormat& format, const char* name, uint32_t offset)
{
    using T = AttrTraits<AttributeType>;
    format.add(name, T::count, T::baseType, T::isInt, offset);
}

}

#define _FUNGT_NEXT_ADD_ATTR(VertexType, field) \
    GraphicsRenderBackendNext::addVertexAttribute<VertexType, decltype(VertexType::field)>( \
        format, #field, static_cast<uint32_t>(offsetof(VertexType, field)));

#define _FUNGT_NEXT_EXPAND1(V, a) _FUNGT_NEXT_ADD_ATTR(V, a)
#define _FUNGT_NEXT_EXPAND2(V, a, b) _FUNGT_NEXT_EXPAND1(V, a) _FUNGT_NEXT_ADD_ATTR(V, b)
#define _FUNGT_NEXT_EXPAND3(V, a, b, c) _FUNGT_NEXT_EXPAND2(V, a, b) _FUNGT_NEXT_ADD_ATTR(V, c)
#define _FUNGT_NEXT_EXPAND4(V, a, b, c, d) _FUNGT_NEXT_EXPAND3(V, a, b, c) _FUNGT_NEXT_ADD_ATTR(V, d)
#define _FUNGT_NEXT_EXPAND5(V, a, b, c, d, e) _FUNGT_NEXT_EXPAND4(V, a, b, c, d) _FUNGT_NEXT_ADD_ATTR(V, e)
#define _FUNGT_NEXT_EXPAND6(V, a, b, c, d, e, f) _FUNGT_NEXT_EXPAND5(V, a, b, c, d, e) _FUNGT_NEXT_ADD_ATTR(V, f)

#define _FUNGT_NEXT_GET_EXPAND(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define _FUNGT_NEXT_EXPAND_ATTRS(V, ...) \
    _FUNGT_NEXT_GET_EXPAND(__VA_ARGS__, \
        _FUNGT_NEXT_EXPAND6, _FUNGT_NEXT_EXPAND5, _FUNGT_NEXT_EXPAND4, \
        _FUNGT_NEXT_EXPAND3, _FUNGT_NEXT_EXPAND2, _FUNGT_NEXT_EXPAND1)(V, __VA_ARGS__)

#define FUNGT_NEXT_VERTEX_FORMAT(VertexType, ...) \
    static const GraphicsRenderBackendNext::VertexFormat& getNextFormat() { \
        static GraphicsRenderBackendNext::VertexFormat format; \
        if (!format.m_attributes.empty()) return format; \
        format.m_stride = sizeof(VertexType); \
        _FUNGT_NEXT_EXPAND_ATTRS(VertexType, __VA_ARGS__) \
        return format; \
    }

#endif // _GRAPHICS_RENDER_BACKEND_NEXT_VERTEX_FORMAT_HPP_
