/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwTools/attributesmacros.hpp
 * @brief TODO
 *
 * @author IRCAD (Research and Development Team).
 */

#ifndef _FWTOOLS_ATTRIBUTESMACROS_HPP_
#define _FWTOOLS_ATTRIBUTESMACROS_HPP_

#include <boost/ref.hpp>

#include <boost/preprocessor/control/if.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>


/** @cond */
#define __FWTOOLS_MEMBER_PREFIX                   m_
#define __FWTOOLS_ATTRIBUTE_ACCESSOR_PARAM_PREFIX _
#define __FWTOOLS_ATTRIBUTE_MAP_NAME              __m_object_attributes__
#define __FWTOOLS_ATTRIBUTES_REGISTER_FUNC_NAME   __registerAttributes

#define __FWTOOLS_NONE
/** @endcond */


#define fwToolsPrependParamPrefix( _name ) \
    BOOST_PP_CAT (__FWTOOLS_ATTRIBUTE_ACCESSOR_PARAM_PREFIX, _name)

#define fwToolsPrependMemberPrefix( _name ) \
    BOOST_PP_CAT (__FWTOOLS_MEMBER_PREFIX, _name)

#define fwToolsAttributeType( _type ) \
    _type::__FWCORE_TYPEDEF_SHARED_PTR_NAME

//-----------------------------------
// Setter
//-----------------------------------
#define fwToolsSetterMacro(_type, _var) \
    fwToolsSetMacro(_type, _var);       \
    fwToolsSetCRefMacro(_type, _var);

//-----------------------------------
//
#define fwToolsSetMacro(_type, _var)                                                \
    void set##_var (const _type fwToolsPrependParamPrefix(_var))        \
    {                                                                               \
        this->fwToolsPrependMemberPrefix( _var ) = fwToolsPrependParamPrefix(_var); \
    }

//-----------------------------------
//
#define fwToolsSetCRefMacro(_type, _var)                                            \
    void setCRef##_var (const _type & fwToolsPrependParamPrefix(_var))  \
    {                                                                               \
        this->fwToolsPrependMemberPrefix( _var ) = fwToolsPrependParamPrefix(_var); \
    }

//-----------------------------------


//-----------------------------------
// Getter
//-----------------------------------
#define fwToolsGetterMacro(_type, _var) \
    fwToolsGetMacro(_type, _var);       \
    fwToolsGetRefMacro(_type, _var);    \
    fwToolsGetCRefMacro(_type, _var);


//-----------------------------------
//
#define fwToolsGetMacro(_type, _var)               \
    const _type get##_var () const     \
    {                                              \
        return fwToolsPrependMemberPrefix( _var ); \
    }

//-----------------------------------
//
#define fwToolsGetRefMacro(_type, _var)                  \
    _type & getRef##_var ()                  \
    {                                                    \
        return this->fwToolsPrependMemberPrefix( _var ); \
    }

//-----------------------------------
//
#define fwToolsGetCRefMacro(_type, _var)                 \
    const _type & getCRef##_var () const     \
    {                                                    \
        return this->fwToolsPrependMemberPrefix( _var ); \
    }

//-----------------------------------
// Getter/Setter
//-----------------------------------
#define fwGetterSetterMacro( _type, _name, _desc )              \
    /** @name fwToolsPrependMemberPrefix( _var ) accessor */    \
    /** Getter/Setter for _var                            */    \
    /** @{                                                */    \
    fwToolsGetterMacro( fwToolsAttributeType( _type ) , _name); \
    fwToolsSetterMacro( fwToolsAttributeType( _type ) , _name); \
    /**@}                                                 */


#define fwToolsDeclareAttrMacro( _type, _name, _desc )                   \
    /** @desc _desc **/                                                  \
     fwToolsAttributeType( _type )  fwToolsPrependMemberPrefix( _name );


#define fwToolsDeclareAttrTypedefMacro( _type, _name, _desc ) \
    /** @desc _desc **/                                       \
     typedef _type BOOST_PP_CAT( _name, Type );






#define __FWTOOLS_ONE_ATTR_REGISTER_MACRO(_type, _name, _desc )                 \
    /* Register the specified attribute in the map */                           \
      /* Insert pair into map */                                                \
      __FWTOOLS_ATTRIBUTE_MAP_NAME.insert(                                      \
              std::make_pair(                                                   \
                  BOOST_PP_STRINGIZE(_name),                                    \
                  boost::ref( (AttrType&) fwToolsPrependMemberPrefix( _name ) ) \
                  ));

#define fwToolsOneAttrRegisterMacro( r, data, _tuple ) \
    __FWTOOLS_ONE_ATTR_REGISTER_MACRO _tuple


#define fwToolsOneAttrDeclareMacro( r, data, _tuple ) \
    fwToolsDeclareAttrMacro _tuple


#define fwToolsOneAttrTypedefDeclareMacro( r, data, _tuple ) \
    fwToolsDeclareAttrTypedefMacro _tuple


#define fwToolsOneAttrGetterSetterMacro( r, data, _tuple ) \
    fwGetterSetterMacro _tuple


#define fwToolsDeclareAttributesMacro( _attributes )                      \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrDeclareMacro, _, _attributes)

#define fwToolsDeclareAttributesTypedefMacro( _attributes )                      \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrTypedefDeclareMacro, _, _attributes)


#define fwToolsGetterSetterAttributesMacro( _attributes )                      \
    /** @cond **/                                                              \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrGetterSetterMacro, _, _attributes) \
    /** @endcond **/


#define fwToolsRegisterAttributesMacro( _attributes )                      \
    /** @cond **/                                                          \
    void __FWTOOLS_ATTRIBUTES_REGISTER_FUNC_NAME()                         \
    {                                                                      \
        BOOST_PP_SEQ_FOR_EACH(fwToolsOneAttrRegisterMacro, _, _attributes) \
    }                                                                      \
    /** @endcond **/


#define fwToolsAttributesMacro( _attributes )           \
    private:                                            \
    fwToolsDeclareAttributesMacro( _attributes )        \
    fwToolsRegisterAttributesMacro( _attributes )       \
    public:                                             \
    fwToolsDeclareAttributesTypedefMacro( _attributes ) \
    fwToolsGetterSetterAttributesMacro( _attributes )




#endif //_FWTOOLS_ATTRIBUTESMACROS_HPP_
