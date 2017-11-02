/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTRUN_HXX__
#define __FWCOM_SLOTRUN_HXX__

#ifndef __FWCOM_SLOTRUN_HPP__
#error fwCom/SlotRun.hpp not included
#endif

#include "fwCom/exception/NoWorker.hpp"
#include "fwCom/SlotBase.hxx"
#include "fwCom/util/WeakCall.hpp"

#include <fwCore/mt/types.hpp>

#include <fwThread/TaskHandler.hpp>
#include <fwThread/Worker.hpp>

#include <future>

namespace fwCom
{

//------------------------------------------------------------------------------

template< typename ... A >
inline std::function< void() > SlotRun< void (A ...) >::bindRun( A ... args  ) const
{
    return std::bind( ( void (SelfType::*)( A ... ) const ) &SelfType::run, this, args ... );
}

//-----------------------------------------------------------------------------

template< typename ... A >
inline SlotBase::VoidSharedFutureType SlotRun< void (A ...) >::asyncRun(
    const ::fwThread::Worker::sptr& worker, A ... args ) const
{
    if(!worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("No valid worker.") );
    }

    return postWeakCall< void >(
        worker,
        ::fwCom::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindRun( args ... )
            )
        );
}

//-----------------------------------------------------------------------------

template< typename ... A >
inline SlotBase::VoidSharedFutureType SlotRun< void (A ...) >::asyncRun(A ... args) const
{
    ::fwCore::mt::ReadLock lock(this->m_workerMutex);

    if(!this->m_worker)
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::NoWorker("Slot has no worker set.") );
    }

    return postWeakCall< void >(
        m_worker,
        ::fwCom::util::weakcall(
            std::dynamic_pointer_cast< const SlotBase >(this->shared_from_this()),
            this->bindRun( args ... ),
            this->m_worker
            )
        );
}

//-----------------------------------------------------------------------------

// Copied from fwThread::Worker because of issues with gcc 4.2 and template
// keyword
template< typename ... A >
template< typename R, typename WEAKCALL >
std::shared_future< R > SlotRun< void (A ...) >::postWeakCall( const ::fwThread::Worker::sptr& worker, WEAKCALL f )
{
    std::packaged_task< R() > task( f );
    std::future< R > ufuture = task.get_future();

    std::function< void() > ftask = ::fwThread::moveTaskIntoFunction(task);

    worker->post(ftask);

    return std::move(ufuture);
}

//-----------------------------------------------------------------------------

} // namespace fwCom

#endif /* __FWCOM_SLOTRUN_HXX__ */

