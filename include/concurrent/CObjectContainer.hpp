/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <map>
#include <utility>

#include "util/class_utils.hpp"

#include "CStaticString.hpp"
#include "Mutex.hpp"
#include "types.hpp"

namespace vfrb::concurrent
{
/// An container for objects bundled with NMEA string, that allows concurrent access.
template<typename ObjectT, usize StringSize>
class CObjectContainer
{
public:
    /// A threadsafe iterator to access elements in the container.
    class CIterator;

    /// Value type stored in the container
    class CValueType;

    using KeyType       = usize;
    using ContainerType = std::map<KeyType, CValueType>;

    /**
     * Get an iterator to the first element in container.
     * @return a valid iterator, or the end-iterator
     */
    CIterator Begin() REQUIRES(!m_modMutex);

    /// Get an iterator to the end of the container, thus pointing nowhere.
    CIterator End();

    /**
     * Insert an object at the given key, if not existing yet.
     * @param key_ The key where to insert
     * @param val_ The object to insert
     * @return an iterator to the element at key and true if it was inserted, or false if not
     */
    std::pair<CIterator, bool> Insert(KeyType key_, ObjectT&& val_) REQUIRES(!m_modMutex);

    /// Erase the element at key.
    void Erase(KeyType key_) REQUIRES(!m_modMutex);

private:
    Mutex mutable m_modMutex;
    ContainerType GUARDED_BY(m_modMutex) m_container;  ///< Underlying container
};

template<typename ObjectT, usize StringSize>
class CObjectContainer<ObjectT, StringSize>::CValueType
{
    friend class CObjectContainer::CIterator;
    Mutex mutable m_mutex;  ///< Locked when this element is accessed

public:
    MOVABLE(CValueType)
    NOT_COPYABLE(CValueType)

    CValueType(ObjectT&& val_);
    ~CValueType() noexcept = default;

    ObjectT                   Value;
    CStaticString<StringSize> Nmea;
};

template<typename ObjectT, usize StringSize>
class CObjectContainer<ObjectT, StringSize>::CIterator
{
    typename ContainerType::iterator m_iterator;   ///< The underlying iterator
    UniqueLock                       m_valueLock;  ///< The lock that is hold while iterator is valid
    CObjectContainer const&          m_container;  ///< The container where elements belong to

public:
    MOVABLE(CIterator)
    NOT_COPYABLE(CIterator)

    explicit CIterator(CObjectContainer& c_);
    CIterator(typename ContainerType::iterator iter_, CObjectContainer const& c_);
    ~CIterator() noexcept = default;

    CIterator&  operator++();
    CValueType& operator*();
    CValueType* operator->();

    KeyType Key() const;

    bool operator==(CIterator const& other_) const;
    bool operator!=(CIterator const& other_) const;
};

template<typename ObjectT, usize StringSize>
CObjectContainer<ObjectT, StringSize>::CValueType::CValueType(ObjectT&& val_) : Value(std::move(val_)) {}

template<typename ObjectT, usize StringSize>
CObjectContainer<ObjectT, StringSize>::CValueType::CValueType(CValueType&& other_) noexcept
    : Value(std::move(other_.Value)), Nmea(other_.Nmea) {}

template<typename ObjectT, usize StringSize>
auto CObjectContainer<ObjectT, StringSize>::CValueType::operator=(CValueType&& other_) noexcept
    -> CValueType& {
    Value = std::move(other_.Value);
    Nmea  = other_.Nmea;
    return *this;
}

template<typename ObjectT, usize StringSize>
CObjectContainer<ObjectT, StringSize>::CIterator::CIterator(CObjectContainer<ObjectT, StringSize>& c_)
    : m_iterator(c_.m_container.end()), m_container(c_) {}

template<typename ObjectT, usize StringSize>
CObjectContainer<ObjectT, StringSize>::CIterator::CIterator(CIterator&& other_) noexcept
    : m_iterator(std::move(other_.m_iterator)),
      m_valueLock(std::move(other_.m_valueLock)),
      m_container(other_.m_container) {}

template<typename ObjectT, usize StringSize>
CObjectContainer<ObjectT, StringSize>::CIterator::CIterator(typename ContainerType::iterator iter_,
                                                            CObjectContainer const&          c_)
    : m_iterator(iter_), m_container(c_) {
    if (m_iterator != m_container.m_container.end()) {
        m_valueLock = UniqueLock(m_iterator->second.m_mutex);
    }
}

template<typename ObjectT, usize StringSize>
auto CObjectContainer<ObjectT, StringSize>::CIterator::operator=(CIterator&& other_) noexcept -> CIterator& {
    m_iterator  = std::move(other_.m_iterator);
    m_valueLock = std::move(other_.m_valueLock);
    m_container = other_.m_container;
}

template<typename ObjectT, usize StringSize>
auto CObjectContainer<ObjectT, StringSize>::CIterator::operator++() -> CIterator& {
    if (m_iterator != m_container.m_container.end()) {
        m_valueLock.unlock();
        LockGuard lk(m_container.m_modMutex);
        if (++m_iterator != m_container.m_container.end()) {
            m_valueLock = UniqueLock(m_iterator->second.m_mutex);
        }
    }
    return *this;
}

template<typename ObjectT, usize StringSize>
auto CObjectContainer<ObjectT, StringSize>::CIterator::operator*() -> CValueType& {
    return m_iterator->second;
}

template<typename ObjectT, usize StringSize>
auto CObjectContainer<ObjectT, StringSize>::CIterator::operator->() -> CValueType* {
    return &m_iterator->second;
}

template<typename ObjectT, usize StringSize>
auto CObjectContainer<ObjectT, StringSize>::CIterator::Key() const -> KeyType {
    return m_iterator->first;
}

template<typename ObjectT, usize StringSize>
bool CObjectContainer<ObjectT, StringSize>::CIterator::operator==(CIterator const& other_) const {
    return m_iterator == other_.m_iterator;
}

template<typename ObjectT, usize StringSize>
bool CObjectContainer<ObjectT, StringSize>::CIterator::operator!=(CIterator const& other_) const {
    return m_iterator != other_.m_iterator;
}

template<typename ObjectT, usize StringSize>
typename CObjectContainer<ObjectT, StringSize>::CIterator CObjectContainer<ObjectT, StringSize>::Begin()
    REQUIRES(!m_modMutex) {
    LockGuard lk(m_modMutex);
    return CIterator(m_container.begin(), *this);
}

template<typename ObjectT, usize StringSize>
auto CObjectContainer<ObjectT, StringSize>::End() -> CIterator {
    return CIterator(*this);
}

template<typename ObjectT, usize StringSize>
std::pair<typename CObjectContainer<ObjectT, StringSize>::CIterator, bool>
    CObjectContainer<ObjectT, StringSize>::Insert(KeyType key_, ObjectT&& value_) REQUIRES(!m_modMutex) {
    LockGuard lk(m_modMutex);
    CIterator iter(m_container.find(key_), *this);
    if (iter == End()) {
        return std::make_pair(
            CIterator(m_container.emplace(key_, CValueType(std::move(value_))).first, *this), true);
    }
    return std::make_pair(std::move(iter), false);
}

template<typename ObjectT, usize StringSize>
void CObjectContainer<ObjectT, StringSize>::Erase(KeyType key_) REQUIRES(!m_modMutex) {
    LockGuard lk(m_modMutex);
    auto      entry = m_container.find(key_);
    if (entry != m_container.end()) {
        CIterator iter(std::move(entry), *this);
        m_container.erase(key_);
    }
}
}  // namespace vfrb::concurrent
