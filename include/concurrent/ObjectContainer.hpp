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

#include "Mutex.hpp"
#include "types.h"

namespace vfrb::concurrent
{
/**
 * @brief Internal container for aircrafts.
 * Allows concurrent access to elements.
 */
template<typename T, usize CstrSz>
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

        ValueType(T&& val_);

        ValueType(ValueType&& other_);

        ValueType& operator=(ValueType&& other_);

        T                     value;
        util::CString<CstrSz> nmea;

    private:
        friend struct ObjectContainer::Iterator;
        Mutex mutable m_mutex;
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

        explicit Iterator(ObjectContainer& c_);

        Iterator(Iterator&& other_);

        Iterator(typename ContainerType::iterator iter_, ObjectContainer const& c_);

        Iterator& operator=(Iterator&& other_);

        Iterator& operator++();

        ValueType& operator*();

        ValueType* operator->();

        KeyType Key() const;

        bool operator==(Iterator const& other_) const;

        bool operator!=(Iterator const& other_) const;

    private:
        typename ContainerType::iterator m_iterator;
        UniqueLock                       m_valueLock;
        ObjectContainer const&           m_container;
    };

    Iterator Begin() REQUIRES(!m_modMutex);

    Iterator End();

    std::pair<Iterator, bool> Insert(KeyType key_, T&& value_) REQUIRES(!m_modMutex);

    void Erase(KeyType key_) REQUIRES(!m_modMutex);

private:
    Mutex mutable m_modMutex;
    ContainerType GUARDED_BY(m_modMutex) m_container;  ///< Underlying container
};

template<typename T, usize CstrSz>
ObjectContainer<T, CstrSz>::ValueType::ValueType(T&& val_) : value(std::move(val_))
{}

template<typename T, usize CstrSz>
ObjectContainer<T, CstrSz>::ValueType::ValueType(ValueType&& other_)
    : value(std::move(other_.value)), nmea(other_.nmea)
{}

template<typename T, usize CstrSz>
auto ObjectContainer<T, CstrSz>::ValueType::operator=(ValueType&& other_) -> ValueType&
{
    value = std::move(other_.value);
    nmea  = other_.nmea;
    return *this;
}

template<typename T, usize CstrSz>
ObjectContainer<T, CstrSz>::Iterator::Iterator(ObjectContainer<T, CstrSz>& c_)
    : m_iterator(c_.m_container.end()), m_container(c_)
{}

template<typename T, usize CstrSz>
ObjectContainer<T, CstrSz>::Iterator::Iterator(Iterator&& other_)
    : m_iterator(std::move(other_.m_iterator)),
      m_valueLock(std::move(other_.m_valueLock)),
      m_container(other_.m_container)
{}

template<typename T, usize CstrSz>
ObjectContainer<T, CstrSz>::Iterator::Iterator(typename ContainerType::iterator iter_,
                                               ObjectContainer const&           c_)
    : m_iterator(iter_), m_container(c_)
{
    if (m_iterator != m_container.m_container.end())
    {
        m_valueLock = UniqueLock(m_iterator->second.m_mutex);
    }
}

template<typename T, usize CstrSz>
auto ObjectContainer<T, CstrSz>::Iterator::operator=(Iterator&& other_) -> Iterator&
{
    m_iterator  = std::move(other_.m_iterator);
    m_valueLock = std::move(other_.m_valueLock);
    m_container = other_.m_container;
}

template<typename T, usize CstrSz>
auto ObjectContainer<T, CstrSz>::Iterator::operator++() -> Iterator&
{
    if (m_iterator != m_container.m_container.end())
    {
        m_valueLock.unlock();
        LockGuard lk(m_container.m_modMutex);
        if (++m_iterator != m_container.m_container.end())
        {
            m_valueLock = UniqueLock(m_iterator->second.m_mutex);
        }
    }
    return *this;
}

template<typename T, usize CstrSz>
auto ObjectContainer<T, CstrSz>::Iterator::operator*() -> ValueType&
{
    return m_iterator->second;
}

template<typename T, usize CstrSz>
auto ObjectContainer<T, CstrSz>::Iterator::operator-> () -> ValueType*
{
    return &m_iterator->second;
}

template<typename T, usize CstrSz>
auto ObjectContainer<T, CstrSz>::Iterator::Key() const -> KeyType
{
    return m_iterator->first;
}

template<typename T, usize CstrSz>
bool ObjectContainer<T, CstrSz>::Iterator::operator==(Iterator const& other_) const
{
    return m_iterator == other_.m_iterator;
}

template<typename T, usize CstrSz>
bool ObjectContainer<T, CstrSz>::Iterator::operator!=(Iterator const& other_) const
{
    return m_iterator != other_.m_iterator;
}

template<typename T, usize CstrSz>
typename ObjectContainer<T, CstrSz>::Iterator ObjectContainer<T, CstrSz>::Begin() REQUIRES(!m_modMutex)
{
    LockGuard lk(m_modMutex);
    return Iterator(m_container.begin(), *this);
}

template<typename T, usize CstrSz>
auto ObjectContainer<T, CstrSz>::End() -> Iterator
{
    return Iterator(*this);
}

template<typename T, usize CstrSz>
std::pair<typename ObjectContainer<T, CstrSz>::Iterator, bool>
    ObjectContainer<T, CstrSz>::Insert(KeyType key_, T&& value_) REQUIRES(!m_modMutex)
{
    LockGuard lk(m_modMutex);
    Iterator  iter(m_container.find(key_), *this);
    if (iter == End())
    {
        return std::make_pair(Iterator(m_container.emplace(key_, ValueType(std::move(value_))).first, *this),
                              true);
    }
    return std::make_pair(std::move(iter), false);
}

template<typename T, usize CstrSz>
void ObjectContainer<T, CstrSz>::Erase(KeyType key_) REQUIRES(!m_modMutex)
{
    LockGuard lk(m_modMutex);
    auto      entry = m_container.find(key_);
    if (entry != m_container.end())
    {
        Iterator iter(std::move(entry), *this);
        m_container.erase(key_);
    }
}
}  // namespace vfrb::concurrent
