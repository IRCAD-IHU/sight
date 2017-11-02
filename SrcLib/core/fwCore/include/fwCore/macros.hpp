/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCORE_MACROS_HPP__
#define __FWCORE_MACROS_HPP__

/**
 * @file fwCore/macros.hpp
 * @brief This file defines fwCore base macros.
 */

#include "fwCore/Demangler.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/control/expr_if.hpp>
#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/logical/and.hpp>
#include <boost/preprocessor/logical/not.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/seq/fold_right.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/transform.hpp>
#include <boost/preprocessor/tuple/elem.hpp>

#include <memory>
#include <string>
#include <type_traits>

#ifdef _WIN32
#pragma warning(disable: 4003)
#endif //_WIN32

#define FwCoreNotUsedMacro(x) ((void) x )

/** @cond
 */

#define __FWCORE_TYPEDEF_SELF_NAME               SelfType
#define __FWCORE_TYPEDEF_SHARED_PTR_NAME         sptr
#define __FWCORE_TYPEDEF_WEAK_PTR_NAME           wptr
#define __FWCORE_TYPEDEF_UNIQUE_PTR_NAME         uptr
#define __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME   csptr
#define __FWCORE_TYPEDEF_WEAK_PTR_CONST_NAME     cwptr
#define __FWCORE_TYPEDEF_UNIQUE_PTR_CONST_NAME   cuptr
#define __FWCORE_FACTORY_NAME                    New
#define __FWCORE_TYPEDEF_SUPERCLASS_NAME         BaseClass
#define __FWCORE_TYPEDEF_ROOTCLASS_NAME          RootClass
#define __FWCORE_DYNAMIC_CAST_FUNC_NAME          dynamicCast
#define __FWCORE_DYNAMIC_CONST_CAST_FUNC_NAME    dynamicConstCast
#define __FWCORE_CONST_CAST_FUNC_NAME            constCast
#define __FWCORE_SPTR_FROM_THIS_FUNC_NAME        getSptr
#define __FWCORE_CONST_SPTR_FROM_THIS_FUNC_NAME  getConstSptr
#define __FWCORE_ARG_NAME                        __var
/** @endcond */

/**
 * @name Smart pointers macro
 * @{ */

/** Expand to shared_ptr < _cls_ > */
#define SPTR(_cls_)  std::shared_ptr < _cls_ >
/** Expand to shared_ptr < const _cls_ > */
#define CSPTR(_cls_) std::shared_ptr < const _cls_ >
/** Expand to weak_ptr < _cls_ > */
#define WPTR(_cls_)  std::weak_ptr < _cls_ >
/** Expand to weak_ptr < const _cls_ > */
#define CWPTR(_cls_) std::weak_ptr < const _cls_ >
/** Expand to unique_ptr < _cls_ > */
#define UPTR(_cls_)  std::unique_ptr < _cls_ >
/** Expand to unique_ptr < const _cls_ > */
#define CUPTR(_cls_) std::unique_ptr < const _cls_ >
/**  @} */

// @cond
#define __FWCORE_NOTHING( _a_ )
#define __FWCORE_NOTHING_2( _a_, _b_ )

#define __FWCORE_GET_NTH_ELEM_III(_s_, _n_, _elem_) BOOST_PP_SEQ_ELEM(_n_, _elem_)
#define __FWCORE_GET_ARGS_TYPES(_args_) BOOST_PP_SEQ_TRANSFORM(__FWCORE_GET_NTH_ELEM_III, 0, _args_)
#define __FWCORE_GET_ARG_DEFAULT_IF_HAS_ONE(arg)             \
    BOOST_PP_EXPR_IF(                                        \
        BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(arg), 2 ),     \
            =                                                \
            )                                                \
    BOOST_PP_EXPAND(                                         \
        BOOST_PP_IF(                                     \
            BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(arg), 2 ), \
            BOOST_PP_SEQ_ELEM, __FWCORE_NOTHING_2        \
            )                                            \
        (1, arg)                                      \
        )

#define __FWCORE_SEQ_IS_EMPTY_SINGLE(_seq_) BOOST_PP_AND( BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(_seq_), 1), \
                                                          BOOST_PP_EQUAL(BOOST_PP_SEQ_SIZE(BOOST_PP_SEQ_ELEM(0, _seq_)), \
                                                                         0) )
#define __FWCORE_SEQ_IS_NOT_EMPTY_SINGLE(_seq_) BOOST_PP_NOT(__FWCORE_SEQ_IS_EMPTY_SINGLE(_seq_))

/*
 * __FWCORE_GENERATE_ARGS( ((a)) ((b)) ((c)(default)) ) expands to  (a __var, ) (b __var, ) (c __var, = default) where
 *__var is defined by __FWCORE_ARG_NAME
 */
#define __FWCORE_ADD_ARG_NAME_AND_VALUE_III(s, _data_, _elem_)                                                                 \
    BOOST_PP_EXPR_IF(_data_, \
                     (BOOST_PP_SEQ_ELEM(0, _elem_) __FWCORE_ARG_NAME,  __FWCORE_GET_ARG_DEFAULT_IF_HAS_ONE(_elem_) ) )
#define __FWCORE_GENERATE_ARGS(_args_)                                                                                                                                   \
    BOOST_PP_IF(                                                                                                                                                         \
        __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                                                                                                        \
        (),                                                                                                                                                          \
        BOOST_PP_EXPR_IF(__FWCORE_SEQ_IS_NOT_EMPTY_SINGLE(_args_), \
                         BOOST_PP_SEQ_TRANSFORM)  (__FWCORE_ADD_ARG_NAME_AND_VALUE_III, BOOST_PP_SEQ_SIZE(_args_), \
                                                   _args_ ) \
        )

/*
 * __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE( ((a)) ((b)) ((c)(default)) ) expands to  (a __var0, b __var1, c __var2 =
 * default)  or to () if _args_ is an empty boost_pp sequence
 */
#define __FWCORE_ADD_ID(_r_, _data_, _i_, _elem_) BOOST_PP_IF(_data_, \
                                                              ( BOOST_PP_CAT( BOOST_PP_TUPLE_ELEM(2, 0, _elem_), \
                                                                              _i_) BOOST_PP_TUPLE_ELEM(2, 1, \
                                                                                                       _elem_)), ())
#define __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE(_args_)                                                                            \
    BOOST_PP_IF(                                                                                                                  \
        __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                                                                     \
        (),                                                                                                                       \
        BOOST_PP_SEQ_TO_TUPLE(BOOST_PP_SEQ_FOR_EACH_I(__FWCORE_ADD_ID, BOOST_PP_SEQ_SIZE(_args_), \
                                                      __FWCORE_GENERATE_ARGS(_args_))) \
        )

/*
 * __FWCORE_GENERATE_NUMBERED_TUPLE((a)(b)(c)) expands to  ( __var0, __var1, __var2)  or to () if _args_ is an empty
 * boost_pp sequence
 */
#define __FWCORE_GENERATE_NUMBERED_TUPLE(_args_)                                 \
    BOOST_PP_IF(                                                                 \
        __FWCORE_SEQ_IS_EMPTY_SINGLE(_args_),                                \
        (),                                                                  \
        (BOOST_PP_ENUM_PARAMS(BOOST_PP_SEQ_SIZE(_args_), __FWCORE_ARG_NAME)) \
        )

/*
 * Utils for Factories (obj::New(...)), work like constructors utils
 */
#define __FWCORE_GENERATE_FACTORY( _factory_, _args_ )                                                           \
    static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_FACTORY_NAME __FWCORE_GENERATE_TYPED_NUMBERED_TUPLE(_args_) \
    {                                                                                                        \
        return __FWCORE_TYPEDEF_SHARED_PTR_NAME(_factory_ __FWCORE_GENERATE_NUMBERED_TUPLE(_args_));       \
    }

#define __FWCORE_GENERATE_ONE_FACTORY(_r_, _data_, _args_)             \
    __FWCORE_GENERATE_FACTORY( BOOST_PP_SEQ_ELEM(0, _data_), _args_);

#define __FWCORE_GENERATE_FACTORIES( _factory_, _args_ )                      \
    BOOST_PP_SEQ_FOR_EACH(__FWCORE_GENERATE_ONE_FACTORY, (_factory_), _args_)

#define __FWCORE_GENERATE_ONE_FACTORY_WITH_N_FACTORIES(_r_, _data_, _factory_args_)                                \
    __FWCORE_GENERATE_FACTORY( BOOST_PP_TUPLE_ELEM(2, 0, _factory_args_), BOOST_PP_TUPLE_ELEM(2, 1, _factory_args_));

#define __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES_I( _factories_args_ )                                    \
    BOOST_PP_SEQ_FOR_EACH(__FWCORE_GENERATE_ONE_FACTORY_WITH_N_FACTORIES, (BOOST_PP_EMPTY), _factories_args_)

#define __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY( _factory_, _args_ ) \
    __FWCORE_GENERATE_FACTORIES( _factory_, _args_ )

#define __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES( _factories_args_ ) \
    __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES_I( _factories_args_ )

/*
 * __FWCORE_GET_CLASSNAME and __FWCORE_GET_SUPERCLASSNAME work with a sequence like "(classname)(baseclassname)" or
 *"(classname)"
 * and return respectively 1st and 2nd element
 *
 * if the sequence containt only 1 element, __FWCORE_GET_SUPERCLASSNAME returns the 1st
 */
#define __FWCORE_GET_CLASSNAME( _seq_ ) BOOST_PP_SEQ_ELEM(0, _seq_)
#define __FWCORE_GET_SUPERCLASSNAME( _seq_ ) BOOST_PP_SEQ_ELEM( BOOST_PP_IF( BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE(_seq_), \
                                                                                             2 ), 1, 0), _seq_)

/*
 * __FWCORE_CLASS_TYPEDEFS define several typdefs for classes (sptr, wptr, ...)
 *
 * BaseClass is a typedef to the superclass
 * RootClass is a typedef to the toplevel base class
 */
#define __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                               \
    /** Self type  */                                                                                      \
    typedef __FWCORE_GET_CLASSNAME (_classinfo_) __FWCORE_TYPEDEF_SELF_NAME;                                \
    /** Type of base class  */                                                                             \
    typedef  __FWCORE_GET_SUPERCLASSNAME (_classinfo_) __FWCORE_TYPEDEF_SUPERCLASS_NAME;                   \
                                                                                                           \
    typedef  BOOST_PP_IF ( BOOST_PP_EQUAL ( BOOST_PP_SEQ_SIZE(_classinfo_), 2 ),                             \
                           __FWCORE_TYPEDEF_SUPERCLASS_NAME::__FWCORE_TYPEDEF_ROOTCLASS_NAME, \
                           __FWCORE_TYPEDEF_SELF_NAME                                         \
                           ) __FWCORE_TYPEDEF_ROOTCLASS_NAME;                                \
    /** Shared pointer type  */                                                                            \
    typedef SPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_SHARED_PTR_NAME;                          \
    /** Weak pointer type  */                                                                              \
    typedef WPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_WEAK_PTR_NAME;                            \
    /** Unique pointer type  */                                                                            \
    typedef UPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_UNIQUE_PTR_NAME;                          \
    /** Const shared pointer type  */                                                                      \
    typedef CSPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME;                   \
    /** Const weak pointer type  */                                                                        \
    typedef CWPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_WEAK_PTR_CONST_NAME;                     \
    /** Const unique pointer type  */                                                                      \
    typedef CUPTR ( __FWCORE_TYPEDEF_SELF_NAME ) __FWCORE_TYPEDEF_UNIQUE_PTR_CONST_NAME;

/*
 * Cast definition for casting from baseclassname and derived to _classname_
 */
#define __FWCORE_GENERATE_CAST(_classname_, _baseclassname_)                                                    \
    /** @brief Cast to dynamic shared pointer   */                                                              \
    template< class BASETYPE, typename = typename std::enable_if < std::is_const<BASETYPE>::value >::type >     \
    static std::shared_ptr< const _classname_> __FWCORE_DYNAMIC_CAST_FUNC_NAME(                                \
        const std::shared_ptr<BASETYPE> &p )                                                                    \
    {                                                                                                           \
        return std::dynamic_pointer_cast< const _classname_ >(p);                                               \
    }                                                                                                          \
    template< class BASETYPE, typename = typename std::enable_if < !std::is_const<BASETYPE>::value >::type >    \
    static std::shared_ptr<_classname_> __FWCORE_DYNAMIC_CAST_FUNC_NAME(                                       \
        const std::shared_ptr<BASETYPE> &p )                                                                    \
    {                                                                                                           \
        return std::dynamic_pointer_cast< _classname_ >(p);                                                     \
    }                                                                                                          \
    /** @brief Const shared pointer cast to dynamic pointer \
     *  @deprecated simply use dynamicCast instead, now it handles const or not const arguments. \
     */                                                                                                         \
    template< class BASETYPE > static __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME __FWCORE_DYNAMIC_CONST_CAST_FUNC_NAME( \
        BASETYPE const &p ) \
    {                                                                                                                                    \
        return std::dynamic_pointer_cast< const _classname_ >(p);                                                                    \
    }                                                                                                                                   \
    /** @brief Cast to const shared pointer */                                                                                           \
    static __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_CONST_CAST_FUNC_NAME( \
        __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME const &p )            \
    {                                                                                                                                    \
        return std::const_pointer_cast< _classname_ >(p);                                                                            \
    }

#define __FWCORE_PREDECLARE(_s_, _state_, _elem_) \
    BOOST_PP_IF(                                 \
        BOOST_PP_EQUAL( _s_, 2 ),         \
        class _elem_; ,                   \
        namespace _elem_ { _state_ }      \
        )

// @endcond

/**
 * @name Class declaration helpers
 * @{ */

/**
 * @brief Generate getSptr and getConstSptr methods
 *
 * These methods use 'shared_from_this' to get a shared pointer and cast it to
 * required type
 *
 */
#define fwCoreAllowSharedFromThis()                                                                   \
    /** @brief return a casted const shared ptr from this object */                                   \
    __FWCORE_TYPEDEF_SHARED_PTR_CONST_NAME __FWCORE_CONST_SPTR_FROM_THIS_FUNC_NAME() const {          \
        return __FWCORE_DYNAMIC_CONST_CAST_FUNC_NAME(this->::fwCore::BaseObject::shared_from_this()); \
    }                                                                                                 \
    /** @brief return a casted shared ptr from this object */                                         \
    __FWCORE_TYPEDEF_SHARED_PTR_NAME __FWCORE_SPTR_FROM_THIS_FUNC_NAME(){                             \
        return __FWCORE_DYNAMIC_CAST_FUNC_NAME(this->::fwCore::BaseObject::shared_from_this());       \
    }

/**
 * @brief Generate predeclarations
 *
 * @param \_cls_ Class to predeclare, in the form (some)(namespace)(class)
 *
 *   Example :
 *   fwCorePredeclare( (fwData)(Image) ) expands to :
 *   namespace fwData {
 *     class Image;
 *     }
 *
 */
#define fwCorePredeclare( _cls_ )                                           \
    BOOST_PP_SEQ_FOLD_RIGHT( __FWCORE_PREDECLARE, BOOST_PP_SEQ_NIL, _cls_)

#define __FWCORE_STATIC_CACHE( _type_, value )        \
    static const _type_ __cache__(value); return __cache__;

/**
 * @brief Generate virtual methods that return classname/namespace strings
 *
 * Example: for ::fwData::Object,
 * - Classname is ::fwData::Object
 * - LeafClassname is Object
 */
#define __FWCORECLASSNAMEMACRO(_qualifier)                                                        \
    /** @name Demangling methods */                                                               \
    /** @{ */                                                                                     \
    /** @brief return object's classnam without its namespacee, i.e. BaseObject */                \
    virtual const std::string& getLeafClassname() const _qualifier                                \
    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::Demangler(*this).getLeafClassname()); }       \
    static const std::string& leafClassname()                                                     \
    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getLeafClassname<SelfType>()); }              \
    /** @brief return full object's classname with its namespace, i.e. ::fwCore::BaseObject */    \
    virtual const std::string& getClassname() const _qualifier                                    \
    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getClassname<SelfType>()); }                  \
    static const std::string& classname()                                                         \
    { __FWCORE_STATIC_CACHE(std::string,  ::fwCore::getClassname<SelfType>()); }                  \
    /** @} */

#define fwCoreInterfaceMacro() __FWCORECLASSNAMEMACRO()
#define fwCoreClassnameMacro() __FWCORECLASSNAMEMACRO(override)

/**
 * @brief Generate virtual methods that check if passed type is same type of
 * (or a topclass of) 'this' type
 *
 * Example:
 * ::fwData::Image::IsTypeOf("::fwData::Object") is true
 * image->isA("::fwData::Object") is true
 *
 */
#define __FWCOREISTYPEOFMACRO(_classinfo_)                                                  \
    static bool isTypeOf(const std::string &type)                                           \
    {                                                                                       \
        if (__FWCORE_TYPEDEF_SELF_NAME::classname() == type)                                \
        {                                                                                   \
            return true;                                                                    \
        }                                                                                   \
        return BOOST_PP_IF( BOOST_PP_EQUAL( BOOST_PP_SEQ_SIZE( _classinfo_), 2 ),           \
                            __FWCORE_TYPEDEF_SUPERCLASS_NAME::isTypeOf( type ), false);     \
    }

#define __FWCOREISAMACRO(_qualifier)                                                        \
    virtual bool isA(const std::string &type) const _qualifier                              \
    {                                                                                       \
        return this->__FWCORE_TYPEDEF_SELF_NAME::isTypeOf(type);                            \
    }

#define fwCoreIsTypeOfMacro(_classinfo_) \
    __FWCOREISTYPEOFMACRO(_classinfo_) \
    __FWCOREISAMACRO(override)
#define fwCoreInterfaceIsTypeOfMacro(_classinfo_) \
    __FWCOREISTYPEOFMACRO(_classinfo_) \
    __FWCOREISAMACRO()

namespace fwTools
{
template<class, class, class>
class ClassFactory;
class Factory;
}

namespace boost
{
namespace serialization
{
class access;
}
}

namespace boost
{
namespace python
{
namespace objects
{
template <class, class>
struct pointer_holder;
}
}
}

/**
 * @brief Generate common code for friend class Factory
 */
#define fwCoreFriendClassFactoryMacro()                                      \
    friend class ::boost::serialization::access;                             \
    template<class, class>                                                   \
    friend struct ::boost::python::objects::pointer_holder;   \
    template<class, class, class>                                            \
    friend class ::fwTools::ClassFactory;                                    \
    friend class ::fwTools::Factory;

/**
 * @brief Generate methods for classes with one factory
 *
 * For this macro parameters, each bracket is significant.
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 *
 * @param \_parameters_ ( types_param_factory0 ) (...) ( types_param_factoryN )
 *                      - where types_param_factoryX can be :
 *                        - for a 0-arg factory : ()
 *                        - for a 1-arg of T type factory : ((T)) or ((T)(d))
 *                          where d is a default value
 *                        - for a N-parameters factory : ((type0)) ((type1)) ... ((typeN)(default_value))
 *                      - Several types_param_factory can be declared if a factory have several signatures
 *                        - Example for several signatures : () ( ((int)) ) ( ((int)) ((std::string)("default")) )
 *
 * @param \_factory_ A factory that can take arguments as defined in _parameters_ arguments
 */
#define fwCoreClassFactoryMacro(_classinfo_, _parameters_, _factory_)                                    \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                 \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) \
     * with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */                                  \
    /* @cond */                                                                                          \
    /* @endcond */                                                                                       \
    __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY(_factory_, _parameters_)                               \
    fwCoreFriendClassFactoryMacro()                                                                      \

/**
 * @brief Generate common construction methods for classes with one factory
 *
 * For this macro parameters, each bracket is significant.
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 *
 * @param \_parameters_ ( types_param_factory0 ) (...) ( types_param_factoryN )
 *                      - where types_param_factoryX can be :
 *                        - for a 0-arg factory : ()
 *                        - for a 1-arg of T type factory : ((T)) or ((T)(d))
 *                          where d is a default value
 *                        - for a N-parameters factory : ((type0)) ((type1)) ... ((typeN)(default_value))
 *                      - Several types_param_factory can be declared if a factory have several signatures
 *                        - Example for several signatures : () ( ((int)) ) ( ((int)) ((std::string)("default")) )
 *
 * @param \_factory_ A factory that can take arguments as defined in _parameters_ arguments
 */
#define fwCoreClassDefinitionsWithFactoryMacro(_classinfo_, _parameters_, _factory_)                     \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                 \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) \
     * with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */                                  \
    /* @cond */                                                                                          \
    /* @endcond */                                                                                       \
    __FWCORE_GENERATE_FACTORIES_WITH_ONE_FACTORY(_factory_, _parameters_)                               \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME)         \
    fwCoreFriendClassFactoryMacro()                                                                      \
    fwCoreClassnameMacro()                                                                               \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common construction methods for classes with several factories
 *
 * For this macro parameters, each bracket is significant.
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 *
 * @param \_factories_args_ ((factory0, types_param_factory0)) ((factory1, types_param_factory1) ... ((factoryN,
 * types_param_factoryN))
 *                          - where types_param_factoryX can be :
 *                            - for a 0-arg factory : ()
 *                            - for a 1-arg of T type factory : ((T)) or ((T)(d))
 *                              where d is a default value
 *                            - for a N-parameters factory : ((type0)) ((type1)) ... ((typeN)(default_value))
 *
 */
#define fwCoreClassDefinitionsWithNFactoriesMacro(_classinfo_, _factories_args_)                                    \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                                            \
    /** Specialized version of shared_ptr (alias to shared_ptr< __FWCORE_GET_CLASSNAME(_classinfo_) > ) \
     * with embeded factory for __FWCORE_GET_CLASSNAME(_classinfo_). */                                             \
    /* @cond */                                                                                                     \
                                                                                                                    \
    __FWCORE_GENERATE_FACTORIES_WITH_N_FACTORIES(_factories_args_)                                                 \
    /* @endcond */                                                                                                  \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME)                     \
    fwCoreFriendClassFactoryMacro()                                                                                 \
    fwCoreClassnameMacro()                                                                                          \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common code for services classes
 *
 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreServiceClassDefinitionsMacro(_classinfo_)                                           \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                          \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME)  \
    fwCoreFriendClassFactoryMacro()                                                               \
    fwCoreClassnameMacro()                                                                        \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common code for Non Instanciable classes (Interfaces, Abstract classes, ...)
 *

 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreNonInstanciableClassDefinitionsMacro(_classinfo_)                                   \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                          \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME)  \
    fwCoreClassnameMacro()                                                                        \
    fwCoreIsTypeOfMacro(_classinfo_)

/**
 * @brief Generate common code for a base class (Interfaces, Abstract classes, ...)
 *

 * @param \_classinfo_ Class information in the form : (classname)(baseclassname) or (classname).
 *                  baseclassname is only required for a non-base class, and should not
 *                  be used if baseclassname == classname
 */
#define fwCoreBaseClassDefinitionsMacro(_classinfo_)                                              \
    __FWCORE_CLASS_TYPEDEFS(_classinfo_)                                                          \
    __FWCORE_GENERATE_CAST(__FWCORE_GET_CLASSNAME(_classinfo_), __FWCORE_TYPEDEF_ROOTCLASS_NAME)  \
    fwCoreInterfaceMacro()                                                                        \
    fwCoreInterfaceIsTypeOfMacro(_classinfo_)

/**  @} */
#endif // __FWCORE_MACROS_HPP__

