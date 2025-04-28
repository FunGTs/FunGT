#if !defined(_VECTOR_H_)
#define _VECTOR_H_
#include <iostream>
#include <memory>
#include <cstddef>
#include <sycl/sycl.hpp>
namespace flib
{
    template <typename T>
    class Vector
    {
        public:
            std::size_t size_;
            std::unique_ptr<T[]> _data;

            Vector(std::size_t size);
            Vector(std::size_t size, T value);
            Vector(const Vector<T>& other);
            Vector(Vector<T>&& other) noexcept;
            ~Vector();

            std::size_t size() const;
            T& operator[](std::size_t index);
            const T& operator[](std::size_t index) const;
            sycl::buffer<T, 1> to_sycl_buffer() const
            {
                return sycl::buffer<T, 1>(_data.get(), sycl::range<1>(size_));
            }
            T dot(const Vector<T>& other) const; 
            void print();
    };

    // Implementations of the member functions can be done in a separate .cpp file or inline as shown above.
    // The print function can be implemented to display the vector elements.
} // namespace flib

#include "vector_impl.hpp"
#endif // _VECTOR_H_
