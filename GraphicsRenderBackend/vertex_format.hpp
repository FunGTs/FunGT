#if !defined(_VERTEX_FORMAT_HPP_)
#define _VERTEX_FORMAT_HPP_

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>
#include "../include/prerequisites.hpp"
#include "../vendor/glm/include/glm/glm.hpp"

// Backend-agnostic base type — backends translate this to their own enum
enum class AttrBaseType { Float, Int, UInt };

// Attribute descriptor — one entry per vertex field
struct VertexAttribute {
    std::string  name;
    uint32_t     location;   // shader binding location
    uint32_t     count;      // number of components
    AttrBaseType baseType;   // backend-agnostic type
    bool         isInt;      // true -> integer path (IPointer on GL, int format on VK)
    uint32_t     offset;     // offsetof(Vertex, field)
};

// Full format for a vertex type
struct VertexFormat {
    uint32_t                     stride = 0;
    std::vector<VertexAttribute> attributes;

    VertexFormat& add(std::string name, uint32_t location, uint32_t count,
                      AttrBaseType baseType, bool isInt, uint32_t offset)
    {
        attributes.push_back({ std::move(name), location, count, baseType, isInt, offset });
        return *this;
    }
};

// Type traits map C++ types to (baseType, count, isInt)
// Add specialisations here for every type used in a vertex struct.
template<typename T> struct AttrTraits;

template<> struct AttrTraits<float>     { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 1; static constexpr bool isInt = false; };
template<> struct AttrTraits<int>       { static constexpr AttrBaseType baseType = AttrBaseType::Int;   static constexpr uint32_t count = 1; static constexpr bool isInt = true;  };
template<> struct AttrTraits<uint32_t>  { static constexpr AttrBaseType baseType = AttrBaseType::UInt;  static constexpr uint32_t count = 1; static constexpr bool isInt = true;  };
template<> struct AttrTraits<glm::vec2> { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 2; static constexpr bool isInt = false; };
template<> struct AttrTraits<glm::vec3> { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 3; static constexpr bool isInt = false; };
template<> struct AttrTraits<glm::vec4> { static constexpr AttrBaseType baseType = AttrBaseType::Float; static constexpr uint32_t count = 4; static constexpr bool isInt = false; };

// Fixed-size arrays — e.g. int m_BoneIDs[4], float m_Weights[4]
template<typename T, std::size_t N> struct AttrTraits<T[N]> {
    static constexpr AttrBaseType baseType = AttrTraits<T>::baseType;
    static constexpr uint32_t     count    = static_cast<uint32_t>(N);
    static constexpr bool         isInt    = AttrTraits<T>::isInt;
};

// Per-attribute add helper — used by the macro
template<typename VertT, typename AttrT>
inline void _fungt_add_attr(VertexFormat& fmt, uint32_t& location,
                            const char* name, uint32_t offset)
{
    using T = AttrTraits<AttrT>;
    fmt.add(name, location++, T::count, T::baseType, T::isInt, offset);
}

#define _FUNGT_ADD_ATTR(VertType, field) \
    _fungt_add_attr<VertType, decltype(VertType::field)>( \
        fmt, loc, #field, static_cast<uint32_t>(offsetof(VertType, field)));

// Variadic expander — up to 6 attributes (extend if needed)
#define _FUNGT_EXPAND1(V,a)           _FUNGT_ADD_ATTR(V,a)
#define _FUNGT_EXPAND2(V,a,b)         _FUNGT_EXPAND1(V,a)         _FUNGT_ADD_ATTR(V,b)
#define _FUNGT_EXPAND3(V,a,b,c)       _FUNGT_EXPAND2(V,a,b)       _FUNGT_ADD_ATTR(V,c)
#define _FUNGT_EXPAND4(V,a,b,c,d)     _FUNGT_EXPAND3(V,a,b,c)     _FUNGT_ADD_ATTR(V,d)
#define _FUNGT_EXPAND5(V,a,b,c,d,e)   _FUNGT_EXPAND4(V,a,b,c,d)   _FUNGT_ADD_ATTR(V,e)
#define _FUNGT_EXPAND6(V,a,b,c,d,e,f) _FUNGT_EXPAND5(V,a,b,c,d,e) _FUNGT_ADD_ATTR(V,f)

#define _FUNGT_GET_EXPAND(_1,_2,_3,_4,_5,_6,NAME,...) NAME
#define _FUNGT_EXPAND_ATTRS(V,...) \
    _FUNGT_GET_EXPAND(__VA_ARGS__, \
        _FUNGT_EXPAND6, _FUNGT_EXPAND5, _FUNGT_EXPAND4, \
        _FUNGT_EXPAND3, _FUNGT_EXPAND2, _FUNGT_EXPAND1)(V,__VA_ARGS__)

// Main macro — place inside the vertex struct.
// Generates a static getFormat() method. Locations assigned in declaration order starting at 0.
// Usage: FUNGT_VERTEX_FORMAT(MyVertex, position, normal, texcoord, m_BoneIDs, m_Weights)
#define FUNGT_VERTEX_FORMAT(VertType, ...)             \
    static const VertexFormat& getFormat() {           \
        static VertexFormat fmt;                       \
        if (!fmt.attributes.empty()) return fmt;       \
        fmt.stride = sizeof(VertType);                 \
        uint32_t loc = 0;                              \
        _FUNGT_EXPAND_ATTRS(VertType, __VA_ARGS__)     \
        return fmt;                                    \
    }


#endif // _VERTEX_FORMAT_HPP_
