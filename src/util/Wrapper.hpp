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

#ifndef SRC_UTIL_WRAPPER_HPP_
#define SRC_UTIL_WRAPPER_HPP_

#include <boost/thread/mutex.hpp>
#include <cstdint>
#include <string>

namespace util
{

/**
 * @class Wrapper
 * @brief Store any value and provide per Wrapper meta-data and mutex.
 * @tparam T The type of value to hold
 */
template<typename T>
struct Wrapper
{
	/// Mutex to enable threadsafety per Wrapper.
	boost::mutex mutex;

	/**
	 * @fn trySetValue
	 * @brief Try to set the value, fails if attempts priority is less.
	 * @note Succeeds always if attempt is valid.
	 * @param cr_nv The new value
	 * @param prio  The attempts priority
	 */
	bool trySetValue(const T& crNewValue, std::uint32_t vPriority,
	        std::uint32_t& rAttempts)
	{
		mUpdated = (vPriority >= mLastPriority)
		        || (!mUpdated && vPriority * ++rAttempts >= mLastPriority);
		if (mUpdated)
		{
			mValue = crNewValue;
			mLastPriority = vPriority;
		}
		return mUpdated;
	}

	/**
	 * @fn getValue
	 * @brief Get the value.
	 * @return the value
	 */
	inline const T& getValue()
	{
		return mValue;
	}

private:
	/// The value
	T mValue;

	/// Last written priority
	std::uint32_t mLastPriority = 0;

	///
	bool mUpdated = false;
};

/**
 * @class TmpWrapper
 * @brief Pseudo temporary Wrapper; value is invalid after read and valid after write.
 * @tparam T the type of value to hold
 */
template<typename T>
struct TmpWrapper
{
	/// Is the value valid?
	bool isValueValid;

	/// Mutex to enable threadsafety per TmpWrapper.
	boost::mutex mutex;

	/**
	 * @fn trySetValue
	 * @brief Try to set the value, fails if attempts priority is less.
	 * @note Succeeds always if attempt is valid.
	 * @param cr_nv The new value
	 * @param prio  The attempts priority
	 */
	bool trySetValue(const T& crNewValue, std::uint32_t vPriority,
	        std::uint32_t& rAttempts)
	{
		mUpdated = (vPriority >= mLastPriority)
		        || (!mUpdated && vPriority * ++rAttempts >= mLastPriority);
		if (mUpdated)
		{
			mValue = crNewValue;
			mLastPriority = vPriority;
			isValueValid = true;
		}
		return mUpdated;
	}

	/**
	 * @fn getValue
	 * @brief Get the value and invalidate it.
	 * @return the value
	 */
	inline const T& getValue()
	{
		isValueValid = false;
		return mValue;
	}

private:
	/// The value
	T mValue;

	/// Last written priority
	std::uint32_t mLastPriority;

	///
	bool mUpdated = false;
};

}  // namespace util

#endif /* SRC_UTIL_WRAPPER_HPP_ */
