/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 3
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 }
 */

#pragma once

#include <map>
#include <mutex>
#include <utility>

#include "util/class_utils.h"

#include "types.h"

namespace vfrb::concurrent
{
/**
 * @brief Internal container for aircrafts.
 * Allows concurrent access to elements.
 */
template<typename T>
class ConcurrentContainer
{
public:
    ConcurrentContainer()           = default;
    ~ConcurrentContainer() noexcept = default;

    struct Iterator;

    /**
     * @brief Wrapper for Aircraft stored in the container.
     */
    struct ValueType
    {
        NOT_COPYABLE(ValueType)

        ValueType(T&& val);

        ValueType(ValueType&& other);

        ValueType& operator=(ValueType&& other);

        T value;

    protected:
        friend struct ConcurrentContainer::Iterator;
        std::mutex mutable mutex;
    };

    using KeyType       = usize;
    using ContainerType = std::map<KeyType, ValueType>;

    /**
     * @brief Iterator to access elements.
     * Takes care about data locking and concurrent access.
     */
    struct Iterator
    {
        NOT_COPYABLE(Iterator)

        explicit Iterator(ConcurrentContainer& c);

        Iterator(Iterator&& other);

        Iterator(typename ContainerType::iterator iter, ConcurrentContainer const& c);

        Iterator& operator=(Iterator&& other);

        Iterator& operator++();

        ValueType& operator*();

        ValueType* operator->();

        KeyType key() const;

        bool operator==(Iterator const& other) const;

        bool operator!=(Iterator const& other) const;

    protected:
        typename ContainerType::iterator iterator;
        std::unique_lock<std::mutex>     valueLock;
        ConcurrentContainer const&       container;
    };

    Iterator begin();

    Iterator end();

    std::pair<Iterator, bool> insert(KeyType key, T&& value);

    void erase(KeyType key);

private:
    ContainerType m_container;  ///< Underlying container
    std::mutex mutable m_modMutex;
};

template<typename T>
ConcurrentContainer<T>::ValueType::ValueType(T&& val) : value(std::move(val))
{}

template<typename T>
ConcurrentContainer<T>::ValueType::ValueType(ValueType&& other) : value(std::move(other.value))
{}

template<typename T>
auto ConcurrentContainer<T>::ValueType::operator=(ValueType&& other) -> ValueType&
{
    value = std::move(other.value);
    return *this;
}

template<typename T>
ConcurrentContainer<T>::Iterator::Iterator(ConcurrentContainer<T>& c)
    : iterator(c.m_container.end()), container(c)
{}

template<typename T>
ConcurrentContainer<T>::Iterator::Iterator(Iterator&& other)
    : iterator(std::move(other.iterator)), valueLock(std::move(other.valueLock)), container(other.container)
{}

template<typename T>
ConcurrentContainer<T>::Iterator::Iterator(typename ContainerType::iterator iter,
                                           ConcurrentContainer const&       c)
    : iterator(iter), container(c)
{
    if (iterator != container.m_container.end())
    {
        valueLock = std::unique_lock(iterator->second.mutex);
    }
}

template<typename T>
auto ConcurrentContainer<T>::Iterator::operator=(Iterator&& other) -> Iterator&
{
    iterator  = std::move(other.iterator);
    valueLock = std::move(other.valueLock);
    container = other.container;
}

template<typename T>
auto ConcurrentContainer<T>::Iterator::operator++() -> Iterator&
{
    if (iterator != container.m_container.end())
    {
        valueLock.unlock();
        std::lock_guard lk(container.m_modMutex);
        if (++iterator != container.m_container.end())
        {
            valueLock = std::unique_lock(iterator->second.mutex);
        }
    }
    return *this;
}

template<typename T>
auto ConcurrentContainer<T>::Iterator::operator*() -> ValueType&
{
    return iterator->second;
}

template<typename T>
auto ConcurrentContainer<T>::Iterator::operator-> () -> ValueType*
{
    return &iterator->second;
}

template<typename T>
auto ConcurrentContainer<T>::Iterator::key() const -> KeyType
{
    return iterator->first;
}

template<typename T>
bool ConcurrentContainer<T>::Iterator::operator==(Iterator const& other) const
{
    return iterator == other.iterator;
}

template<typename T>
bool ConcurrentContainer<T>::Iterator::operator!=(Iterator const& other) const
{
    return iterator != other.iterator;
}

template<typename T>
auto ConcurrentContainer<T>::begin() -> Iterator
{
    std::lock_guard lk(m_modMutex);
    return Iterator(m_container.begin(), *this);
}

template<typename T>
auto ConcurrentContainer<T>::end() -> Iterator
{
    return Iterator(*this);
}

template<typename T>
auto ConcurrentContainer<T>::insert(KeyType key, T&& value) -> std::pair<Iterator, bool>
{
    std::lock_guard lk(m_modMutex);
    Iterator        iter(m_container.find(key), *this);
    if (iter == end())
    {
        return std::make_pair(Iterator(m_container.emplace(key, ValueType(std::move(value))).first, *this),
                              true);
    }
    return std::make_pair(std::move(iter), false);
}

template<typename T>
void ConcurrentContainer<T>::erase(KeyType key)
{
    std::lock_guard lk(m_modMutex);
    auto            entry = m_container.find(key);
    if (entry != m_container.end())
    {
        Iterator iter(std::move(entry), *this);
        m_container.erase(key);
    }
}
}  // namespace vfrb::concurrent
