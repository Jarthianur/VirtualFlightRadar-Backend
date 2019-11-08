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
class CObjectContainer
{
public:
    CObjectContainer()           = default;
    ~CObjectContainer() noexcept = default;

    class CIterator;

    /**
     * @brief Wrapper for Aircraft stored in the container.
     */
    class CValueType
    {
        NOT_COPYABLE(CValueType)

        friend class CObjectContainer::CIterator;
        Mutex mutable m_mutex;

    public:
        CValueType(T&& val_);

        CValueType(CValueType&& other_);

        CValueType& operator=(CValueType&& other_);

        T                     value;
        util::CString<CstrSz> nmea;
    };

    using KeyType       = usize;
    using ContainerType = std::map<KeyType, CValueType>;

    /**
     * @brief Iterator to access elements.
     * Takes care about data locking and concurrent access.
     */
    class CIterator
    {
        NOT_COPYABLE(CIterator)
        typename ContainerType::iterator m_iterator;
        UniqueLock                       m_valueLock;
        CObjectContainer const&          m_container;

    public:
        explicit CIterator(CObjectContainer& c_);

        CIterator(CIterator&& other_);

        CIterator(typename ContainerType::iterator iter_, CObjectContainer const& c_);

        CIterator& operator=(CIterator&& other_);

        CIterator& operator++();

        CValueType& operator*();

        CValueType* operator->();

        KeyType Key() const;

        bool operator==(CIterator const& other_) const;

        bool operator!=(CIterator const& other_) const;
    };

    CIterator begin() REQUIRES(!m_modMutex);

    CIterator end();

    std::pair<CIterator, bool> insert(KeyType key_, T&& value_) REQUIRES(!m_modMutex);

    void erase(KeyType key_) REQUIRES(!m_modMutex);

    FUNCTION_ALIAS(Begin, begin)
    FUNCTION_ALIAS(End, end)
    FUNCTION_ALIAS(Insert, insert)
    FUNCTION_ALIAS(Erase, erase)

private:
    Mutex mutable m_modMutex;
    ContainerType GUARDED_BY(m_modMutex) m_container;  ///< Underlying container
};

template<typename T, usize CstrSz>
CObjectContainer<T, CstrSz>::CValueType::CValueType(T&& val_) : value(std::move(val_))
{}

template<typename T, usize CstrSz>
CObjectContainer<T, CstrSz>::CValueType::CValueType(CValueType&& other_)
    : value(std::move(other_.value)), nmea(other_.nmea)
{}

template<typename T, usize CstrSz>
auto CObjectContainer<T, CstrSz>::CValueType::operator=(CValueType&& other_) -> CValueType&
{
    value = std::move(other_.value);
    nmea  = other_.nmea;
    return *this;
}

template<typename T, usize CstrSz>
CObjectContainer<T, CstrSz>::CIterator::CIterator(CObjectContainer<T, CstrSz>& c_)
    : m_iterator(c_.m_container.end()), m_container(c_)
{}

template<typename T, usize CstrSz>
CObjectContainer<T, CstrSz>::CIterator::CIterator(CIterator&& other_)
    : m_iterator(std::move(other_.m_iterator)),
      m_valueLock(std::move(other_.m_valueLock)),
      m_container(other_.m_container)
{}

template<typename T, usize CstrSz>
CObjectContainer<T, CstrSz>::CIterator::CIterator(typename ContainerType::iterator iter_,
                                                  CObjectContainer const&          c_)
    : m_iterator(iter_), m_container(c_)
{
    if (m_iterator != m_container.m_container.end())
    {
        m_valueLock = UniqueLock(m_iterator->second.m_mutex);
    }
}

template<typename T, usize CstrSz>
auto CObjectContainer<T, CstrSz>::CIterator::operator=(CIterator&& other_) -> CIterator&
{
    m_iterator  = std::move(other_.m_iterator);
    m_valueLock = std::move(other_.m_valueLock);
    m_container = other_.m_container;
}

template<typename T, usize CstrSz>
auto CObjectContainer<T, CstrSz>::CIterator::operator++() -> CIterator&
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
auto CObjectContainer<T, CstrSz>::CIterator::operator*() -> CValueType&
{
    return m_iterator->second;
}

template<typename T, usize CstrSz>
auto CObjectContainer<T, CstrSz>::CIterator::operator-> () -> CValueType*
{
    return &m_iterator->second;
}

template<typename T, usize CstrSz>
auto CObjectContainer<T, CstrSz>::CIterator::Key() const -> KeyType
{
    return m_iterator->first;
}

template<typename T, usize CstrSz>
bool CObjectContainer<T, CstrSz>::CIterator::operator==(CIterator const& other_) const
{
    return m_iterator == other_.m_iterator;
}

template<typename T, usize CstrSz>
bool CObjectContainer<T, CstrSz>::CIterator::operator!=(CIterator const& other_) const
{
    return m_iterator != other_.m_iterator;
}

template<typename T, usize CstrSz>
typename CObjectContainer<T, CstrSz>::CIterator CObjectContainer<T, CstrSz>::begin() REQUIRES(!m_modMutex)
{
    LockGuard lk(m_modMutex);
    return Iterator(m_container.begin(), *this);
}

template<typename T, usize CstrSz>
auto CObjectContainer<T, CstrSz>::end() -> CIterator
{
    return CIterator(*this);
}

template<typename T, usize CstrSz>
std::pair<typename CObjectContainer<T, CstrSz>::CIterator, bool>
    CObjectContainer<T, CstrSz>::insert(KeyType key_, T&& value_) REQUIRES(!m_modMutex)
{
    LockGuard lk(m_modMutex);
    CIterator iter(m_container.find(key_), *this);
    if (iter == End())
    {
        return std::make_pair(Iterator(m_container.emplace(key_, ValueType(std::move(value_))).first, *this),
                              true);
    }
    return std::make_pair(std::move(iter), false);
}

template<typename T, usize CstrSz>
void CObjectContainer<T, CstrSz>::erase(KeyType key_) REQUIRES(!m_modMutex)
{
    LockGuard lk(m_modMutex);
    auto      entry = m_container.find(key_);
    if (entry != m_container.end())
    {
        CIterator iter(std::move(entry), *this);
        m_container.erase(key_);
    }
}
}  // namespace vfrb::concurrent
