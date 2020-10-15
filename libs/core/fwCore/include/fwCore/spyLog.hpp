/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

/**
 * @file spyLog.hpp
 * @brief This file defines SpyLog macros.
 * These macros are used to log messages to a file or to the console during
 * application execution.
 *
 * Six log levels are defined :
 * -# Fatal
 * -# Error
 * -# Warning
 * -# Info
 * -# Debug
 * -# Trace
 *
 * Log level is set by defining SPYLOG_LEVEL to N where 0 < N <= 6. If log
 * level is set to N, every log level lesser than N will be enabled, the other
 * macro will be define but won't have any effect.
 *
 * Each log level macro can accept strings or stringstreams:
 *   - Example : SLM_INFO( "Count : " << i );
 *
 * FATAL macros have a particular behavior : the application is aborted after
 * the message was logged.
 *
 *
 */

#pragma once

# define SPYLOG_ABORT() std::abort()

# ifdef _DEBUG

#  ifdef WIN32
#   define DEBUG_BREAK() __debugbreak()
#  else
#   include <csignal>
#   define DEBUG_BREAK() std::raise(SIGTRAP)
#  endif

#  ifdef SPYLOG_ABORT
#   undef SPYLOG_ABORT
#  endif
#  define SPYLOG_ABORT() DEBUG_BREAK()

# endif

# include <cassert>
# include <sstream>

#include <boost/preprocessor/comparison/greater_equal.hpp>
#include <boost/preprocessor/control/expr_iif.hpp>

# include "fwCore/log/SpyLogger.hpp"
# include "fwCore/log/ScopedMessage.hpp"

// -----------------------------------------------------------------------------

/**
 * @cond
 */

// -----------------------------------------------------------------------------

# ifndef __FWCORE_EXPR_BLOCK
#  define __FWCORE_EXPR_BLOCK(expr) do { expr } while (0)
# endif

# define __FWCORE_IF(cond, code) if ( cond ) { code }

# define SL_LOG(log, loglevel, message) __FWCORE_EXPR_BLOCK(   \
        std::stringstream oslStr;                              \
        oslStr << message;                                     \
        log.loglevel(oslStr.str(), __FILE__, __LINE__);      \
        )

#ifdef SPYLOG_LEVEL

#define __FWCORE_IF_ENABLED( level, expr ) \
    BOOST_PP_EXPR_IIF( BOOST_PP_GREATER_EQUAL(SPYLOG_LEVEL, level), expr)

#define __FWCORE_IF_ELSE_ENABLED( level, expr1, expr2 ) \
    BOOST_PP_IIF( BOOST_PP_GREATER_EQUAL(SPYLOG_LEVEL, level), expr1, expr2)

#endif

// -----------------------------------------------------------------------------

#  define SL_TRACE(log, message) __FWCORE_IF_ENABLED( 6,                              \
                                                      SL_LOG(log, trace, message);    \
                                                      )

#  define SL_TRACE_IF(log, message, cond) __FWCORE_IF_ENABLED( 6,                                                 \
                                                               __FWCORE_IF(cond, SL_LOG(log, trace, message); )   \
                                                               )

#  define SL_DEBUG(log, message) __FWCORE_IF_ENABLED( 5,                            \
                                                      SL_LOG(log, debug, message);  \
                                                      )
#define SL_DEBUG_IF(log, message, cond) __FWCORE_IF_ENABLED( 5,                                                   \
                                                             __FWCORE_IF(cond, SL_LOG(log, debug, message); )     \
                                                             )

#define SL_INFO(log, message) __FWCORE_IF_ENABLED( 4,                             \
                                                   SL_LOG(log, info, message);    \
                                                   )
#define SL_INFO_IF(log, message, cond) __FWCORE_IF_ENABLED( 4,                                               \
                                                            __FWCORE_IF(cond, SL_LOG(log, info, message); )  \
                                                            )

#define SL_WARN(log, message) __FWCORE_IF_ENABLED( 3,                              \
                                                   SL_LOG(log, warn, message);    \
                                                   )
#define SL_WARN_IF(log, message, cond) __FWCORE_IF_ENABLED( 3,                                                \
                                                            __FWCORE_IF(cond, SL_LOG(log, warn, message); )   \
                                                            )

#define SL_ERROR(log, message) __FWCORE_IF_ENABLED( 2,                            \
                                                    SL_LOG(log, error, message);  \
                                                    )
#define SL_ERROR_IF(log, message, cond) __FWCORE_IF_ENABLED( 2,                                               \
                                                             __FWCORE_IF(cond, SL_LOG(log, error, message); ) \
                                                             )

#define SL_FATAL(log, message) __FWCORE_IF_ENABLED( 1,                             \
                                                    SL_LOG(log, fatal, message);   \
                                                    SPYLOG_ABORT();                 \
                                                    )
#define SL_FATAL_IF(log, message, cond) __FWCORE_IF_ENABLED( 1,                                             \
                                                             __FWCORE_IF(cond, SL_FATAL(log, message); )    \
                                                             )

// -----------------------------------------------------------------------------

# ifdef _DEBUG
#  ifdef WIN32
#  define __SL_ASSERT(log, message, cond) __FWCORE_EXPR_BLOCK(                                          \
        __FWCORE_IF(!(cond),                                                                            \
                    std::stringstream oslStr1;                                                          \
                    oslStr1 << "Assertion '" <<                                                         \
                    #cond << "' failed.\n" << message;                                                  \
                    log.fatal(oslStr1.str(), __FILE__, __LINE__);                                       \
                    _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", oslStr1.str().c_str());  \
                    __debugbreak();                                                                     \
                    ))
#  else
#  define __SL_ASSERT(log, message, cond) __FWCORE_EXPR_BLOCK(          \
        __FWCORE_IF(!(cond),                                            \
                    std::stringstream oslStr1;                          \
                    oslStr1 << "Assertion '" <<                         \
                    #cond << "' failed: " << message;                   \
                    log.fatal(oslStr1.str(), __FILE__, __LINE__);       \
                    SPYLOG_ABORT();                                     \
                    ))
#  endif

#  define SL_ASSERT(log, message, cond) __FWCORE_EXPR_BLOCK(   \
        __FWCORE_IF(!(cond),                                   \
                    std::stringstream oslStr;                  \
                    oslStr << message;                         \
                    __SL_ASSERT(log, oslStr.str(), cond);      \
                    ))
# else
#  define SL_ASSERT(log, message, cond) // empty
# endif

// -----------------------------------------------------------------------------

#  define _SPYLOG_SPYLOGGER_            \
    ::fwCore::log::SpyLogger::getSpyLogger()

// -----------------------------------------------------------------------------

/**
 * @endcond
 */

/** @{ */
/** Trace message macros.  */
# define SLM_TRACE(message) SL_TRACE(_SPYLOG_SPYLOGGER_, message)
# define OSLM_TRACE(message) SL_TRACE(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal trace message macros.  */
# define SLM_TRACE_IF(message, cond) SL_TRACE_IF(_SPYLOG_SPYLOGGER_, message, cond)
# define OSLM_TRACE_IF(message, cond) SL_TRACE_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Debug message macros.  */
# define SLM_DEBUG(message) SL_DEBUG(_SPYLOG_SPYLOGGER_, message)
/** @deprecated @sight22, use SLM_DEBUG instead. */
# define OSLM_DEBUG(message) SL_DEBUG(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal debug message macros.  */
# define SLM_DEBUG_IF(message, cond) SL_DEBUG_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** @deprecated @sight22, use SLM_DEBUG_IF instead. */
# define OSLM_DEBUG_IF(message, cond) SL_DEBUG_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Info message macros.  */
# define SLM_INFO(message) SL_INFO(_SPYLOG_SPYLOGGER_, message)
/** @deprecated @sight22, use SLM_INFO instead. */
# define OSLM_INFO(message) SL_INFO(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal info message macros.  */
# define SLM_INFO_IF(message, cond) SL_INFO_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** @deprecated @sight22, use SLM_INFO_IF instead. */
# define OSLM_INFO_IF(message, cond) SL_INFO_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Warning message macros.  */
# define SLM_WARN(message) SL_WARN(_SPYLOG_SPYLOGGER_, message)
/** @deprecated @sight22, use SLM_WARN instead.  */
# define OSLM_WARN(message) SL_WARN(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal warning message macros.  */
# define SLM_WARN_IF(message, cond) SL_WARN_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** @deprecated @sight22, use SLM_WARN_IF instead. */
# define OSLM_WARN_IF(message, cond) SL_WARN_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Error message macros.  */
# define SLM_ERROR(message) SL_ERROR(_SPYLOG_SPYLOGGER_, message)
/** @deprecated @sight22, use SLM_ERROR instead.  */
# define OSLM_ERROR(message) SL_ERROR(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal error message macros.  */
# define SLM_ERROR_IF(message, cond) SL_ERROR_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** @deprecated @sight22, use SLM_ERROR_IF instead.  */
# define OSLM_ERROR_IF(message, cond) SL_ERROR_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Fatal message macros.  */
# define SLM_FATAL(message) SL_FATAL(_SPYLOG_SPYLOGGER_, message)
/** @deprecated @sight22, use SLM_FATAL instead.  */
# define OSLM_FATAL(message) SL_FATAL(_SPYLOG_SPYLOGGER_, message)
/** Conditionnal fatal message macros.  */
# define SLM_FATAL_IF(message, cond) SL_FATAL_IF(_SPYLOG_SPYLOGGER_, message, cond)
/** @deprecated @sight22, use SLM_FATAL_IF instead.  */
# define OSLM_FATAL_IF(message, cond) SL_FATAL_IF(_SPYLOG_SPYLOGGER_, message, cond)
/**  @} */

/** @{ */
/** Log message macros.  */
#define SLM_LOG(message)                                          \
    __FWCORE_EXPR_BLOCK(                                      \
        std::stringstream stream;                                  \
        stream << message;                                         \
        _SPYLOG_SPYLOGGER_.log(stream.str(), __FILE__, __LINE__);  \
        )
#define OSLM_LOG(message) SLM_LOG(message)
/**  @} */

/**
 * @name Special macros
 * @{ */

/** @brief work like 'assert' from 'cassert', with in addition a message logged by
 * spylog (with FATAL loglevel)  */
# define SLM_ASSERT(message, cond)                      \
    SL_ASSERT(_SPYLOG_SPYLOGGER_, message, cond)
# define OSLM_ASSERT(message, cond)                     \
    SL_ASSERT(_SPYLOG_SPYLOGGER_, message, cond)

// -----------------------------------------------------------------------------

# ifdef __GNUC__                // with GCC
#  define SLM_PRETTY_FUNC() __PRETTY_FUNCTION__
# elif defined(_MSC_VER)        // with MSC
#  define SLM_PRETTY_FUNC() __FUNCSIG__
# else
#  define SLM_PRETTY_FUNC() __func__
# endif

/**
 * @brief Trace contextual function signature.
 *
 * Generate a log trace message with the (contextual) function signature.
 */
# ifndef SPYLOG_TIMER
#  define SLM_TRACE_FUNC() SLM_TRACE(SLM_PRETTY_FUNC())
# else

# define SLM_TRACE_FUNC() __FWCORE_EXPR_BLOCK(                  \
        ::fwCore::log::ScopedMessage __spylog__scoped__msg__(        \
            __FILE__, __LINE__, SLM_PRETTY_FUNC());             \
        __spylog__scoped__msg__.use();                          \
        )

# endif
/**  @} */

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED(oldFnName, newFnName, version) \
    SLM_ERROR(  "[DEPRECATED] '" << oldFnName << "' is deprecated and will be removed in '" << version << "', use '" \
                                 << newFnName << "' instead. It is still used by '" + this->getClassname() + "'." \
                );

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED_IF(oldFnName, newFnName, version, condition) \
    SLM_ERROR_IF("[DEPRECATED] '" << oldFnName << "' is deprecated and will be removed in '" << version << "', use '" \
                                  << newFnName << "' instead. It is still used by '" + this->getClassname() + "'.", \
                 ondition);

/**
 * @brief Use this macro when deprecating a function to warn the developer.
 */
#define FW_DEPRECATED_MSG(message, version) \
    SLM_ERROR(  "[DEPRECATED] " << message << " It will be removed in '" << version << "'");

/**
 * @brief Use this macro when deprecating a service key to warn the developer.
 */
#define FW_DEPRECATED_KEY(newKey, access, version) \
    SLM_ERROR(  "[DEPRECATED] The key '" << newKey << "' is not correctly set. Please correct the configuration to " \
                "set an '" << access << "' key named '" << newKey << "'. The support of the old key will be removed " \
                "in '" << version << "'.");

/** Preprocessor define that can be tested to know if trace log level is active */
#define SLM_TRACE_ENABLED __FWCORE_IF_ELSE_ENABLED( 6, 1, 0 )

/** Preprocessor define that can be tested to know if debug log level is active */
#define SLM_DEBUG_ENABLED __FWCORE_IF_ELSE_ENABLED( 5, 1, 0 )

/** Preprocessor define that can be tested to know if info log level is active */
#define SLM_INFO_ENABLED  __FWCORE_IF_ELSE_ENABLED( 4, 1, 0 )

/** Preprocessor define that can be tested to know if warning log level is active */
#define SLM_WARN_ENABLED  __FWCORE_IF_ELSE_ENABLED( 3, 1, 0 )

/** Preprocessor define that can be tested to know if error log level is active */
#define SLM_ERROR_ENABLED __FWCORE_IF_ELSE_ENABLED( 2, 1, 0 )

/** Preprocessor define that can be tested to know if fatal log level is active */
#define SLM_FATAL_ENABLED __FWCORE_IF_ELSE_ENABLED( 1, 1, 0 )
