/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWTOOLS_RANDOM_GENERATOR_HPP__
#define __FWTOOLS_RANDOM_GENERATOR_HPP__

#include <fwCore/base.hpp>

#include <boost/concept_check.hpp>
#include <boost/mpl/if.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include <algorithm>
#include <ctime>
#include <type_traits>

namespace fwTools
{
namespace random
{

/**
 * @brief On each invocation, it returns a random value uniformly distributed in the range [min..max).
 * @param min      The "min" parameter of the distribution.
 * @param max      The "max" parameter of the distribution.
 * @param seedVal  Seed for generator.
 * @pre   min <= max
 *
 * @return returns a random value uniformly distributed in the range [min..max)
 */
template <typename T>
T getValue(T min, T max, std::uint32_t seedVal = std::time(NULL))
{
    SLM_ASSERT("Wrong min/max value", min <= max);
    typedef typename ::boost::mpl::if_<
            std::is_floating_point<T>,
            ::boost::uniform_real<>,
            ::boost::uniform_int<>
            >::type DistroType;

    ::boost::mt19937 seed(seedVal);
    DistroType dist(min, max);
    ::boost::variate_generator< ::boost::mt19937&, DistroType > random(seed, dist);
    return random();
}

/**
 * @brief On each invocation, this method fills specified container with random values uniformly distributed in the
 * range [min..max).
 * @param[in] min            The "min" parameter of the distribution.
 * @param[in] max            The "max" parameter of the distribution.
 * @param[out] randContainer Container to fill with random values.
 * @param[in] seedVal        Seed for generator.
 * @pre  min <= max
 * @pre  CONTAINER type same as template T
 */
template <typename T, typename CONTAINER>
void fillContainer(T min, T max, CONTAINER& randContainer, std::uint32_t seedVal = std::time(NULL))
{
    SLM_ASSERT("Wrong min/max value", min <= max);
    SLM_ASSERT("Container type not same as T", (std::is_same< T, typename CONTAINER::value_type>::value) );
    typedef typename ::boost::mpl::if_<
            std::is_floating_point<T>,
            ::boost::uniform_real<>,
            ::boost::uniform_int<>
            >::type DistroType;

    ::boost::mt19937 seed(seedVal);
    DistroType dist(min, max);
    ::boost::variate_generator< ::boost::mt19937&, DistroType > random(seed, dist);
    std::generate(randContainer.begin(), randContainer.end(), random);
}

} // namespace random
} // namespace fwTools

#endif //__FWTOOLS_RANDOM_GENERATOR_HPP__
