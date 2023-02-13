#pragma once

#include <vector>
#include <memory>
#include <cassert>

namespace Chaotirender
{   
    using buffer_id = int;

    template <typename T> 
    class BufferList
    {
    public:
        buffer_id add(const T& data); // copy

        buffer_id add(std::unique_ptr<T>&& data); // move ownership

        void remove(buffer_id id); // reset ptr to null

        T* get(buffer_id id) const; // reference

        void clear(); // clear all data

    private:
        std::vector<std::unique_ptr<T>> m_buffer_list;
    };

    template <typename T>
    buffer_id BufferList<T>::add(const T& data)
    {
        buffer_id id = m_buffer_list.size();
        m_buffer_list.push_back(std::make_unique<T>(data));
        return id;
    }

    template <typename T>
    buffer_id BufferList<T>::add(std::unique_ptr<T>&& data)
    {
        buffer_id id = m_buffer_list.size();
        m_buffer_list.push_back(std::move(data));
        return id;
    }

    template <typename T>
    void BufferList<T>::remove(buffer_id id)
    {   
        assert(id >= 0 && id < m_buffer_list.size() && "buffer index out of range!!!");
        m_buffer_list[id].reset();
    }

    template <typename T>
    T* BufferList<T>::get(buffer_id id) const 
    { 
        assert(id >= 0 && id < m_buffer_list.size() && "buffer index out of range!!!");
        return m_buffer_list[id].get();
    }

    template <typename T>
    void BufferList<T>::clear() 
    { 
        m_buffer_list.clear(); // memory automatically destroyed with unique_ptr
    }
}