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
#include <utility>

#include "util/CString.hpp"
#include "util/class_utils.h"

#include "Mutex.h"
#include "types.h"

namespace vfrb::concurrent
{
/**
 * @brief Internal container for aircrafts.
 * Allows concurrent access to elements.
 */
template<typename T, usize CstrS>
class ObjectContainer
{
public:
    ObjectContainer()           = default;
    ~ObjectContainer() noexcept = default;

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

        T value                   GUARDED_BY(mutex);
        util::CString<CstrS> nmea GUARDED_BY(mutex);

    protected:
        friend struct ObjectContainer::Iterator;
        Mutex mutable mutex;
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

        explicit Iterator(ObjectContainer& c);

        Iterator(Iterator&& other);

        Iterator(typename ContainerType::iterator iter, ObjectContainer const& c);

        Iterator& operator=(Iterator&& other);

        Iterator& operator++();

        ValueType& operator*();

        ValueType* operator->();

        KeyType key() const;

        bool operator==(Iterator const& other) const;

        bool operator!=(Iterator const& other) const;

    protected:
        typename ContainerType::iterator iterator;
        UniqueLock                       valueLock;
        ObjectContainer const& container GUARDED_BY(container.m_modMutex);
    };

    Iterator begin() REQUIRES(!m_modMutex);

    Iterator end();

    std::pair<Iterator, bool> insert(KeyType key, T&& value) REQUIRES(!m_modMutex);

    void erase(KeyType key) REQUIRES(!m_modMutex);

private:
    ContainerType GUARDED_BY(m_modMutex) m_container;  ///< Underlying container
    Mutex mutable m_modMutex;
};

template<typename T, usize CstrS>
ObjectContainer<T, CstrS>::ValueType::ValueType(T&& val) : value(std::move(val))
{}

template<typename T, usize CstrS>
ObjectContainer<T, CstrS>::ValueType::ValueType(ValueType&& other)
    : value(std::move(other.value)), nmea(other.nmea)
{}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::ValueType::operator=(ValueType&& other) -> ValueType&
{
    value = std::move(other.value);
    nmea  = other.nmea;
    return *this;
}

template<typename T, usize CstrS>
ObjectContainer<T, CstrS>::Iterator::Iterator(ObjectContainer<T, CstrS>& c)
    : iterator(c.m_container.end()), container(c)
{}

template<typename T, usize CstrS>
ObjectContainer<T, CstrS>::Iterator::Iterator(Iterator&& other)
    : iterator(std::move(other.iterator)), valueLock(std::move(other.valueLock)), container(other.container)
{}

template<typename T, usize CstrS>
ObjectContainer<T, CstrS>::Iterator::Iterator(typename ContainerType::iterator iter, ObjectContainer const& c)
    : iterator(iter), container(c)
{
    if (iterator != container.m_container.end())
    {
        valueLock = UniqueLock(iterator->second.mutex);
    }
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::Iterator::operator=(Iterator&& other) -> Iterator&
{
    iterator  = std::move(other.iterator);
    valueLock = std::move(other.valueLock);
    container = other.container;
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::Iterator::operator++() -> Iterator&
{
    if (iterator != container.m_container.end())
    {
        valueLock.unlock();
        LockGuard lk(container.m_modMutex);
        if (++iterator != container.m_container.end())
        {
            valueLock = UniqueLock(iterator->second.mutex);
        }
    }
    return *this;
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::Iterator::operator*() -> ValueType&
{
    return iterator->second;
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::Iterator::operator-> () -> ValueType*
{
    return &iterator->second;
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::Iterator::key() const -> KeyType
{
    return iterator->first;
}

template<typename T, usize CstrS>
bool ObjectContainer<T, CstrS>::Iterator::operator==(Iterator const& other) const
{
    return iterator == other.iterator;
}

template<typename T, usize CstrS>
bool ObjectContainer<T, CstrS>::Iterator::operator!=(Iterator const& other) const
{
    return iterator != other.iterator;
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::begin() -> Iterator
{
    LockGuard lk(m_modMutex);
    return Iterator(m_container.begin(), *this);
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::end() -> Iterator
{
    return Iterator(*this);
}

template<typename T, usize CstrS>
auto ObjectContainer<T, CstrS>::insert(KeyType key, T&& value) -> std::pair<Iterator, bool>
{
    LockGuard lk(m_modMutex);
    Iterator  iter(m_container.find(key), *this);
    if (iter == end())
    {
        return std::make_pair(Iterator(m_container.emplace(key, ValueType(std::move(value))).first, *this),
                              true);
    }
    return std::make_pair(std::move(iter), false);
}

template<typename T, usize CstrS>
void ObjectContainer<T, CstrS>::erase(KeyType key)
{
    LockGuard lk(m_modMutex);
    auto      entry = m_container.find(key);
    if (entry != m_container.end())
    {
        Iterator iter(std::move(entry), *this);
        m_container.erase(key);
    }
}
}  // namespace vfrb::concurrent
