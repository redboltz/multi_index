/* Copyright 2003-2018 Joaquin M Lopez Munoz.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * See http://www.boost.org/libs/multi_index for library home page.
 */

#ifndef BOOST_MULTI_INDEX_KEY_HPP
#define BOOST_MULTI_INDEX_KEY_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>

#if __cplusplus>=201703L

#include <type_traits>

namespace boost{

namespace multi_index{

/* C++17 terse key specification syntax */

namespace detail{

template<auto... Keys>
struct key_impl
{
  static_assert(sizeof...(Keys),"at least one key must be provided");
};

template<typename Class,typename Type,Type Class::*PtrToMember>
struct key_impl<PtrToMember>
{
  using value_type=Class;
  using type=member<Class,Type,PtrToMember>;
};

template<
  typename Class,typename Type,Type (Class::*PtrToMemberFunction)()const
>
struct key_impl<PtrToMemberFunction>
{
  using value_type=Class;
  using type=const_mem_fun<Class,Type,PtrToMemberFunction>;
};

template<typename Class,typename Type,Type (Class::*PtrToMemberFunction)()>
struct key_impl<PtrToMemberFunction>
{
  using value_type=Class;
  using type=mem_fun<Class,Type,PtrToMemberFunction>;
};

template<class Value,typename Type,Type (*PtrToFunction)(Value)>
struct key_impl<PtrToFunction>
{
  using value_type=Value;
  using type=global_fun<Value,Type,PtrToFunction>;
};

template<typename... Ts>
struct least_generic;

template<typename T0,typename... Ts>
struct least_generic<T0,Ts...>
{
  using type=T0;
};

template<typename T0,typename T1,typename... Ts>
struct least_generic<T0,T1,Ts...>
{
  static_assert(
    std::is_convertible_v<const T0&,const T1&>||
    std::is_convertible_v<const T1&,const T0&>,
    "one type should be convertible to the other");
    
  using type=typename least_generic<
    std::conditional_t<std::is_convertible_v<const T0&,const T1&>,T0,T1>,
    Ts...
  >::type;
};

template<auto Key0,auto... Keys>
struct key_impl<Key0,Keys...>
{
  using value_type=typename least_generic<
    std::decay_t<typename key_impl<Key0>::value_type>,
    std::decay_t<typename key_impl<Keys>::value_type>...
  >::type;
  using type=composite_key<
    value_type,
    typename key_impl<Key0>::type,
    typename key_impl<Keys>::type...
  >;
};

template<typename=composite_key<void,void>>
struct composite_key_size;

template<typename... Args>
struct composite_key_size<composite_key<Args...>>
{
  static constexpr auto value=sizeof...(Args)-1;
};

template<auto... Keys>
struct limited_size_key_impl
{
  static_assert(
    sizeof...(Keys)<=composite_key_size<>::value,
    "specified number of keys must meet the limits of "
    "boost::multi_index::composite_key");
  using type=typename key_impl<Keys...>::type;
};

} /* namespace multi_index::detail */

template<auto... Keys>
using key=typename detail::limited_size_key_impl<Keys...>::type;

} /* namespace multi_index */

} /* namespace boost */

#endif
#endif