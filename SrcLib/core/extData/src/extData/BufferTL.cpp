/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "extData/BufferTL.hpp"

#include <fwCore/base.hpp>

#include <fwData/registry/macros.hpp>
#include <fwData/Exception.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/pool/pool.hpp>

#include <cmath>

namespace extData
{

const size_t BufferTL::s_DEFAULT_TIMELINE_MAX_SIZE = 1000;

//------------------------------------------------------------------------------

BufferTL::BufferTL ( ::fwData::Object::Key key ) :
    TimeLine(key),
    m_maximumSize(s_DEFAULT_TIMELINE_MAX_SIZE)
{
}

//------------------------------------------------------------------------------

BufferTL::~BufferTL ()
{
    this->clearTimeline();
}

//------------------------------------------------------------------------------

void BufferTL::allocPoolSize(std::size_t size)
{
    this->clearTimeline();
    ::fwCore::mt::WriteLock lock(m_tlMutex);
    m_pool = std::make_shared< PoolType >(size);
}

//------------------------------------------------------------------------------

void BufferTL::pushObject(const SPTR(::extData::timeline::Object) &obj)
{
    // This check is important for inherited classes
    SLM_ASSERT("Trying to push not compatible Object in the BufferTL.", isObjectValid(obj));

    ::fwCore::mt::WriteLock writeLock(m_tlMutex);
    if(m_timeline.size() >= m_maximumSize)
    {
        TimelineType::iterator begin = m_timeline.begin();
        m_timeline.erase(begin);
    }

    SPTR(::extData::timeline::Buffer) srcObj = std::dynamic_pointer_cast< ::extData::timeline::Buffer >(obj);
    m_timeline.insert(TimelineType::value_type(obj->getTimestamp(), srcObj));
}

//------------------------------------------------------------------------------

SPTR(::extData::timeline::Object) BufferTL::popObject(TimestampType timestamp)
{
    const auto itFind = m_timeline.find(timestamp);

    // Check if timestamp exists
    SLM_ASSERT("Trying to erase not existing timestamp", itFind != m_timeline.end());

    SPTR(::extData::timeline::Object) object = itFind->second;

    ::fwCore::mt::WriteLock writeLock(m_tlMutex);

    m_timeline.erase(itFind);

    return object;
}

//------------------------------------------------------------------------------

void BufferTL::modifyTime(TimestampType timestamp, TimestampType newTimestamp)
{
    const auto itFind = m_timeline.find(timestamp);

    // Check if timestamp exists
    SLM_ASSERT("Trying to swap at non-existing timestamp", itFind != m_timeline.end());

    // Check if newTimestamp is not already used
    SLM_ASSERT("New timestamp already used by an other object", m_timeline.find(newTimestamp) == m_timeline.end());

    ::fwCore::mt::WriteLock writeLock(m_tlMutex);

    m_timeline.insert(TimelineType::value_type(newTimestamp, itFind->second));
    m_timeline.erase(itFind);
}

//------------------------------------------------------------------------------

void BufferTL::setObject(TimestampType timestamp, const SPTR(::extData::timeline::Object) &obj)
{
    // Check if timestamp exists
    SLM_ASSERT("Trying to set an object at non-existing timestamp", m_timeline.find(timestamp) != m_timeline.end());

    ::fwCore::mt::WriteLock writeLock(m_tlMutex);

    SPTR(::extData::timeline::Buffer) srcObj = std::dynamic_pointer_cast< ::extData::timeline::Buffer >(obj);
    m_timeline[timestamp]                    = srcObj;
}

//------------------------------------------------------------------------------

CSPTR(::extData::timeline::Object) BufferTL::getClosestObject(::fwCore::HiResClock::HiResClockType timestamp,
                                                              DirectionType direction) const
{
    ::fwCore::mt::ReadLock readLock(m_tlMutex);
    SPTR(::extData::timeline::Buffer) result;
    if(m_timeline.empty())
    {
        return result;
    }

    TimelineType::const_iterator iter = m_timeline.lower_bound(timestamp);
    if (iter != m_timeline.begin())
    {
        if( iter == m_timeline.end() )
        {
            if (direction != FUTURE)
            {
                SPTR(::extData::timeline::Buffer) previousObj = (--iter)->second;
                result                                        = previousObj;
            }
        }
        else
        {
            ::fwCore::HiResClock::HiResClockType nextTS = iter->first;
            SPTR(::extData::timeline::Buffer) nextObj   = iter->second;

            switch ( direction )
            {
                case PAST:
                    result = (--iter)->second;
                    break;
                case BOTH:
                {
                    ::fwCore::HiResClock::HiResClockType previousTS = (--iter)->first;
                    SPTR(::extData::timeline::Buffer) previousObj   = iter->second;
                    result                                          =
                        ((nextTS - timestamp) > (timestamp - previousTS)) ? previousObj : nextObj;
                }
                break;
                case FUTURE:
                    result = nextObj;
                    break;
            }
        }
    }
    else
    {
        if (direction != PAST)
        {
            SPTR(::extData::timeline::Buffer) nextObj = iter->second;
            result                                    = nextObj;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

CSPTR(::extData::timeline::Object) BufferTL::getObject(::fwCore::HiResClock::HiResClockType timestamp) const
{
    ::fwCore::mt::ReadLock readLock(m_tlMutex);
    SPTR(::extData::timeline::Buffer) result;
    TimelineType::const_iterator iter = m_timeline.find(timestamp);

    if(iter != m_timeline.end())
    {
        result = iter->second;
    }

    OSLM_WARN_IF("There is no object in the timeline matching the timestamp: " << timestamp << ".",
                 iter == m_timeline.end());

    return result;
}

//------------------------------------------------------------------------------

CSPTR(::extData::timeline::Object) BufferTL::getNewerObject() const
{
    SPTR(::extData::timeline::Object) result;

    ::fwCore::mt::ReadLock readLock(m_tlMutex);
    if (!m_timeline.empty())
    {
        result = m_timeline.rbegin()->second;
    }
    return result;
}

//------------------------------------------------------------------------------

::fwCore::HiResClock::HiResClockType BufferTL::getNewerTimestamp() const
{
    ::fwCore::HiResClock::HiResClockType result = 0;

    ::fwCore::mt::ReadLock readLock(m_tlMutex);
    if (!m_timeline.empty())
    {
        result = m_timeline.rbegin()->first;
    }
    return result;
}

//------------------------------------------------------------------------------

void BufferTL::clearTimeline()
{
    ::fwCore::mt::WriteLock writeLock(m_tlMutex);
    m_timeline.clear();
}

} // namespace extData
