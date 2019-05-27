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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <string>

#include "object/Aircraft.h"
#include "processor/AircraftProcessor.h"
#include "util/defines.h"

#include "Data.hpp"

namespace data
{
/**
 * @brief Store aircrafts.
 */
class AircraftData : public Data
{
    /// Times until FLARM status is removed
    static constexpr const std::uint32_t NO_FLARM_THRESHOLD = object::Object::OUTDATED;
    /// Times until aircraft gets deleted
    static constexpr const std::uint32_t DELETE_THRESHOLD = 120;

    class Container
    {
    public:
        DEFAULT_CTOR(Container)
        DEFAULT_DTOR(Container)

        struct Iterator;
        struct ValueType
        {
            ValueType(object::Aircraft&& a) : aircraft(std::move(a)) {}

            ValueType& operator=(ValueType&& other)
            {
                aircraft.tryUpdate(std::move(other.aircraft));
                return *this;
            }

            object::Aircraft aircraft;

        protected:
            friend struct Container::Iterator;
            mutable std::mutex dataMutex;
        };

        using KeyType       = std::size_t;
        using ContainerType = std::map<KeyType, ValueType>;

        struct Iterator
        {
            explicit Iterator(Container& c) : iterator(c.m_container.end()), container(c) {}

            Iterator(ContainerType::iterator&& iter, const Container& c)
                : iterator(std::move(iter)), container(c)
            {
                if (iterator != container.m_container.end())
                {
                    dataLock = std::unique_lock<std::mutex>(iterator->second.dataMutex);
                }
            }

            Iterator& operator++()
            {
                if (iterator != container.m_container.end())
                {
                    dataLock.unlock();
                    std::lock_guard<std::mutex> lk(container.m_modMutex);
                    if (++iterator != container.m_container.end())
                    {
                        dataLock = std::unique_lock<std::mutex>(iterator->second.dataMutex);
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
            ContainerType::iterator      iterator;
            std::unique_lock<std::mutex> dataLock;
            const Container&             container;
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

        std::pair<Iterator, bool> insert(object::Aircraft&& aircraft)
        {
            KeyType key =
                std::hash<std::string>()(*aircraft.get_id());  // TODO: provide char* based hashing
            std::lock_guard<std::mutex> lock(m_modMutex);
            Iterator                    iter(m_container.find(key), *this);
            if (iter == end())
            {
                return std::make_pair(
                    Iterator(m_container.emplace(key, ValueType{std::move(aircraft)}).first, *this),
                    true);
            }
            return std::make_pair(iter, false);
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

    protected:
        ContainerType      m_container;
        mutable std::mutex m_modMutex;
    };

public:
    AircraftData();
    DEFAULT_CHILD_DTOR(AircraftData)

    /**
     * @brief Constructor
     * @param maxDist The max distance filter
     */
    explicit AircraftData(std::int32_t maxDist);

    /**
     * @brief Insert or update an Aircraft.
     * @param aircraft The update
     * @return true on success, else false
     * @threadsafe
     */
    bool update(object::Object&& aircraft) override;

    void set_environment(const object::Position& position, double atmPress);

    /**
     * @brief Process all aircrafts.
     * @param position The refered position
     * @param atmPress The atmospheric pressure
     * @threadsafe
     */
    void access(const accessor_fn& func) override;

private:
    Container m_container;

    /// Processor for aircrafts
    processor::AircraftProcessor m_processor;
};

}  // namespace data
