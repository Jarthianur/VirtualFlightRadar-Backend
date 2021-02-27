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

#include "Mutex.hpp"
#include "Types.hpp"

namespace vfrb::concurrent
{
template<typename ObjectT>
class CObjectContainer
{
public:
    class CIterator;
    class CValueType;

    using KeyType       = usize;
    using ContainerType = TreeMap<KeyType, CValueType>;

    CIterator
    Begin() REQUIRES(!m_modMutex);

    auto
    End() -> CIterator;

    Pair<CIterator, bool>
    Insert(KeyType key_, ObjectT&& value_) REQUIRES(!m_modMutex);

    void
    Erase(KeyType key_) REQUIRES(!m_modMutex);

    usize
    Size() const REQUIRES(!m_modMutex);

private:
    Mutex mutable m_modMutex;
    ContainerType GUARDED_BY(m_modMutex) m_container;
};

template<typename ObjectT>
class CObjectContainer<ObjectT>::CValueType
{
    friend class CObjectContainer::CIterator;
    Mutex mutable m_mutex;

public:
    MOVABLE(CValueType)
    NOT_COPYABLE(CValueType)

    CValueType(ObjectT&& val_);
    ~CValueType() noexcept = default;

    ObjectT Value;
};

template<typename ObjectT>
class CObjectContainer<ObjectT>::CIterator
{
    typename ContainerType::iterator m_iterator;
    UniqueLock                       m_valueLock;
    CObjectContainer const&          m_container;

public:
    MOVABLE(CIterator)
    NOT_COPYABLE(CIterator)

    explicit CIterator(CObjectContainer& c_);
    CIterator(typename ContainerType::iterator iter_, CObjectContainer const& c_);
    ~CIterator() noexcept = default;

    auto
    operator++() -> CIterator&;
    auto
    operator*() -> CValueType&;
    auto
    operator->() -> CValueType*;

    [[nodiscard]] auto
    Key() const -> KeyType;

    auto
    operator==(CIterator const& other_) const -> bool;
    auto
    operator!=(CIterator const& other_) const -> bool;
};

template<typename ObjectT>
CObjectContainer<ObjectT>::CValueType::CValueType(ObjectT&& val_) : Value(std::move(val_)) {}

template<typename ObjectT>
CObjectContainer<ObjectT>::CValueType::CValueType(CValueType&& other_) noexcept
    : Value(std::move(other_.Value)) {}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CValueType::operator=(CValueType&& other_) noexcept -> CValueType& {
    Value = std::move(other_.Value);
    return *this;
}

template<typename ObjectT>
CObjectContainer<ObjectT>::CIterator::CIterator(CObjectContainer<ObjectT>& c_)
    : m_iterator(c_.m_container.end()), m_container(c_) {}

template<typename ObjectT>
CObjectContainer<ObjectT>::CIterator::CIterator(CIterator&& other_) noexcept
    : m_iterator(std::move(other_.m_iterator)),
      m_valueLock(std::move(other_.m_valueLock)),
      m_container(other_.m_container) {}

template<typename ObjectT>
CObjectContainer<ObjectT>::CIterator::CIterator(typename ContainerType::iterator iter_,
                                                CObjectContainer const&          c_)
    : m_iterator(iter_), m_container(c_) {
    if (m_iterator != m_container.m_container.end()) {
        m_valueLock = UniqueLock(m_iterator->second.m_mutex);
    }
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CIterator::operator=(CIterator&& other_) noexcept -> CIterator& {
    m_iterator  = std::move(other_.m_iterator);
    m_valueLock = std::move(other_.m_valueLock);
    m_container = other_.m_container;
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CIterator::operator++() -> CIterator& {
    if (m_iterator != m_container.m_container.end()) {
        m_valueLock.unlock();
        LockGuard lk(m_container.m_modMutex);
        if (++m_iterator != m_container.m_container.end()) {
            m_valueLock = UniqueLock(m_iterator->second.m_mutex);
        }
    }
    return *this;
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CIterator::operator*() -> CValueType& {
    return m_iterator->second;
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CIterator::operator->() -> CValueType* {
    return &m_iterator->second;
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CIterator::Key() const -> KeyType {
    return m_iterator->first;
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CIterator::operator==(CIterator const& other_) const -> bool {
    return m_iterator == other_.m_iterator;
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::CIterator::operator!=(CIterator const& other_) const -> bool {
    return m_iterator != other_.m_iterator;
}

template<typename ObjectT>
typename CObjectContainer<ObjectT>::CIterator
CObjectContainer<ObjectT>::Begin() REQUIRES(!m_modMutex) {
    LockGuard lk(m_modMutex);
    return CIterator(m_container.begin(), *this);
}

template<typename ObjectT>
auto
CObjectContainer<ObjectT>::End() -> CIterator {
    return CIterator(*this);
}

template<typename ObjectT>
Pair<typename CObjectContainer<ObjectT>::CIterator, bool>
CObjectContainer<ObjectT>::Insert(KeyType key_, ObjectT&& value_) REQUIRES(!m_modMutex) {
    LockGuard lk(m_modMutex);
    CIterator iter(m_container.find(key_), *this);
    if (iter == End()) {
        return {CIterator(m_container.emplace(key_, CValueType(std::move(value_))).first, *this), true};
    }
    return {std::move(iter), false};
}

template<typename ObjectT>
void
CObjectContainer<ObjectT>::Erase(KeyType key_) REQUIRES(!m_modMutex) {
    LockGuard lk(m_modMutex);
    auto      entry = m_container.find(key_);
    if (entry != m_container.end()) {
        CIterator iter(std::move(entry), *this);
        m_container.erase(key_);
    }
}

template<typename ObjectT>
usize
CObjectContainer<ObjectT>::Size() const REQUIRES(!m_modMutex) {
    LockGuard lk(m_modMutex);
    return m_container.size();
}
}  // namespace vfrb::concurrent
