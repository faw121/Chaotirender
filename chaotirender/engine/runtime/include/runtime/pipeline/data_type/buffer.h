#pragma once

#include <vector>
#include <cassert>

namespace Chaotirender
{
    template<typename T> 
    class Buffer2D
    {
    public:
        Buffer2D(int w, int h);

        Buffer2D(int w, int h, const T& value);

        T get(int i, int j) const;

        void set(int i, int j, const T& value);

        void clear(const T& value);

        void resizeAndClear(int w, int h, const T& value);

        void resize(int w, int h);

        const T* data() const;

    private:
        int m_w;
        int m_h;
        std::vector<T> m_data;
    };

    template <typename T>
    Buffer2D<T>::Buffer2D(int w, int h): m_w(w), m_h(h), m_data(w * h) {}

    template <typename T>
    Buffer2D<T>::Buffer2D(int w, int h, const T& value): m_w(w), m_h(h), m_data(w * h, value) {}

    template <typename T>
    T Buffer2D<T>::get(int i, int j) const
    {   
        assert(i < m_w && j < m_h);
        return m_data[j * m_w + i];
    }

    template <typename T>
    void Buffer2D<T>::set(int i, int j, const T& value)
    {
        assert(i < m_w && j < m_h);
        m_data[j * m_w + i] = value;
    }

    template <typename T>
    void Buffer2D<T>::clear(const T& value) { std::fill(m_data.begin(), m_data.end(), value); }

    template <typename T>
    void Buffer2D<T>::resizeAndClear(int w, int h, const T& value)
    {
        m_w = w;
        m_h = h;
        m_data.resize(w * h);
        std::fill(m_data.begin(), m_data.end(), value);
    }

    template <typename T>
    void Buffer2D<T>::resize(int w, int h)
    {
        m_w = w;
        m_h = h;
        m_data.resize(w * h);
    }

    template <typename T>
    const T* Buffer2D<T>::data() const { return m_data.data(); }
}