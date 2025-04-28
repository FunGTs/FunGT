#include "vector.hpp"

template <typename T>
flib::Vector<T>::Vector(std::size_t size) : size_(size), _data(nullptr) {
    if (size_ > 0) {
        _data = std::make_unique<T[]>(size_);
    }
}
    template <typename T>
flib::Vector<T>::Vector(std::size_t size, T value) : size_(size), _data(nullptr) {
    if (size_ > 0) {
        _data = std::make_unique<T[]>(size_);
        for (std::size_t i = 0; i < size_; ++i) {
            _data[i] = value;
        }
    }
}
template <typename T>
flib::Vector<T>::Vector(const Vector<T>& other) : size_(other.size_), _data(nullptr) {
    if (size_ > 0) {
        _data = std::make_unique<T[]>(size_);
        for (std::size_t i = 0; i < size_; ++i) {
            _data[i] = other._data[i];
        }
    }
}
template <typename T>
flib::Vector<T>::Vector(Vector<T>&& other) noexcept : size_(other.size_), _data(std::move(other._data)) {
    other.size_ = 0;
}
template <typename T>
flib::Vector<T>::~Vector() {
    // Destructor
    // No need to manually delete _data as std::unique_ptr will handle it
}
template <typename T>
std::size_t flib::Vector<T>::size() const {
    return size_;
}
template <typename T>
T& flib::Vector<T>::operator[](std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
    return _data[index];
}
template <typename T>
const T& flib::Vector<T>::operator[](std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
    return _data[index];
}

template <typename T>
inline T flib::Vector<T>::dot(const Vector<T> &other) const
{
    if (size_ != other.size_)
    {
        throw std::invalid_argument("Vectors must be of the same size for dot product");
    }
    T result = 0;
    for (std::size_t i = 0; i < size_; ++i)
    {
        result += _data[i] * other._data[i];
    }
    return result;
}

template <typename T>
void flib::Vector<T>::print()
{
    for (std::size_t i = 0; i < size_; ++i) {
        std::cout << _data[i] << " ";
    }
    std::cout << std::endl;
}
