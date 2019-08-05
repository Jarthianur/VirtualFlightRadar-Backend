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

#include <cstddef>
#include <map>
#include <mutex>
#include <utility>

#include "util/defines.h"

namespace util
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

        ValueType(T&& val) : value(std::move(val)) {}

        ValueType(ValueType&& other) : value(std::move(other.value)) {}

        ValueType& operator=(ValueType&& other)
        {
            value = std::move(other.value);
            return *this;
        }

        T value;

    protected:
        friend struct ConcurrentContainer::Iterator;
        mutable std::mutex mutex;
    };

    using KeyType       = std::size_t;
    using ContainerType = std::map<KeyType, ValueType>;

    /**
     * @brief Iterator to access elements.
     * Takes care about data locking and concurrent access.
     */
    struct Iterator
    {
        NOT_COPYABLE(Iterator)

        explicit Iterator(ConcurrentContainer& c) : iterator(c.m_container.end()), container(c) {}

        Iterator(Iterator&& other)
            : iterator(std::move(other.iterator)),
              valueLock(std::move(other.valueLock)),
              container(other.container)
        {}

        Iterator(typename ContainerType::iterator iter, const ConcurrentContainer& c)
            : iterator(iter), container(c)
        {
            if (iterator != container.m_container.end())
            {
                valueLock = std::unique_lock<std::mutex>(iterator->second.mutex);
            }
        }

        Iterator& operator=(Iterator&& other)
        {
            iterator  = std::move(other.iterator);
            valueLock = std::move(other.valueLock);
            container = other.container;
        }

        Iterator& operator++()
        {
            if (iterator != container.m_container.end())
            {
                valueLock.unlock();
                std::lock_guard<std::mutex> lk(container.m_modMutex);
                if (++iterator != container.m_container.end())
                {
                    valueLock = std::unique_lock<std::mutex>(iterator->second.mutex);
                }
            }
            return *this;
        }

        ValueType& operator*()
        {
            return iterator->second;
        }

        ValueType* operator->()
        {
            return &iterator->second;
        }

        KeyType getKey() const
        {
            return iterator->first;
        }

        bool operator==(const Iterator& other) const
        {
            return iterator == other.iterator;
        }

        bool operator!=(const Iterator& other) const
        {
            return iterator != other.iterator;
        }

    protected:
        typename ContainerType::iterator iterator;
        std::unique_lock<std::mutex>     valueLock;
        const ConcurrentContainer&       container;
    };

    Iterator begin()
    {
        std::lock_guard<std::mutex> lock(m_modMutex);
        return Iterator(m_container.begin(), *this);
    }

    Iterator end()
    {
        return Iterator(*this);
    }

    std::pair<Iterator, bool> insert(KeyType key, T&& value)
    {
        std::lock_guard<std::mutex> lock(m_modMutex);
        Iterator                    iter(m_container.find(key), *this);
        if (iter == end())
        {
            return std::make_pair(
                Iterator(m_container.emplace(key, ValueType(std::move(value))).first, *this), true);
        }
        return std::make_pair(std::move(iter), false);
    }

    void erase(KeyType key)
    {
        std::lock_guard<std::mutex> lock(m_modMutex);
        auto                        entry = m_container.find(key);
        if (entry != m_container.end())
        {
            Iterator iter(std::move(entry), *this);
            m_container.erase(key);
        }
    }

private:
    ContainerType      m_container;  ///< Underlying container
    mutable std::mutex m_modMutex;
};

}  // namespace util
