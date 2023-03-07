// Copyright 2021 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_TYPE_TRAITS_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_TYPE_TRAITS_H_

#include <type_traits>

namespace cpp17 {
namespace internal {

template <typename T>
static constexpr bool is_reference_wrapper = false;
template <typename T>
static constexpr bool is_reference_wrapper<std::reference_wrapper<T>> = true;

// These are from [func.require] ¶ 1.1-7
template <typename MemFn, typename Class, typename T>
static constexpr bool invoke_pmf_base = std::is_member_function_pointer<MemFn Class::*>::value &&
                                        std::is_base_of<Class, std::remove_reference_t<T>>::value;

template <typename MemFn, typename Class, typename T>
static constexpr bool invoke_pmf_refwrap =
    std::is_member_function_pointer<MemFn Class::*>::value &&
    is_reference_wrapper<std::remove_cv_t<std::remove_reference_t<T>>>;

template <typename MemFn, typename Class, typename T>
static constexpr bool invoke_pmf_other =
    std::is_member_function_pointer<MemFn Class::*>::value && !invoke_pmf_base<MemFn, Class, T> &&
    !invoke_pmf_refwrap<MemFn, Class, T>;

template <typename MemObj, typename Class, typename T>
static constexpr bool invoke_pmd_base = std::is_member_object_pointer<MemObj Class::*>::value &&
                                        std::is_base_of<Class, std::remove_reference_t<T>>::value;

template <typename MemObj, typename Class, typename T>
static constexpr bool invoke_pmd_refwrap =
    std::is_member_object_pointer<MemObj Class::*>::value &&
    is_reference_wrapper<std::remove_cv_t<std::remove_reference_t<T>>>;

template <typename MemObj, typename Class, typename T>
static constexpr bool invoke_pmd_other =
    std::is_member_object_pointer<MemObj Class::*>::value && !invoke_pmd_base<MemObj, Class, T> &&
    !invoke_pmd_refwrap<MemObj, Class, T>;

// ¶ 1.7 says to just return f(t1, t2, ..., tn) in all other cases

// Just internal forward declarations for SFINAE; cpp20::invoke is defined in
// lib/stdcompat/functional.h
template <typename MemFn, typename Class, typename T, typename... Args>
constexpr auto invoke(MemFn Class::*f, T&& obj, Args&&... args)
    -> std::enable_if_t<invoke_pmf_base<MemFn, Class, T>,
                        decltype((std::forward<T>(obj).*f)(std::forward<Args>(args)...))>;

template <typename MemFn, typename Class, typename T, typename... Args>
constexpr auto invoke(MemFn Class::*f, T&& obj, Args&&... args)
    -> std::enable_if_t<invoke_pmf_refwrap<MemFn, Class, T>,
                        decltype((obj.get().*f)(std::forward<Args>(args)...))>;

template <typename MemFn, typename Class, typename T, typename... Args>
constexpr auto invoke(MemFn Class::*f, T&& obj, Args&&... args)
    -> std::enable_if_t<invoke_pmf_other<MemFn, Class, T>,
                        decltype(((*std::forward<T>(obj)).*f)(std::forward<Args>(args)...))>;

template <typename MemObj, typename Class, typename T>
constexpr auto invoke(MemObj Class::*f, T&& obj)
    -> std::enable_if_t<invoke_pmd_base<MemObj, Class, T>, decltype(std::forward<T>(obj).*f)>;

template <typename MemObj, typename Class, typename T>
constexpr auto invoke(MemObj Class::*f, T&& obj)
    -> std::enable_if_t<invoke_pmd_refwrap<MemObj, Class, T>, decltype(obj.get().*f)>;

template <typename MemObj, typename Class, typename T>
constexpr auto invoke(MemObj Class::*f, T&& obj)
    -> std::enable_if_t<invoke_pmd_other<MemObj, Class, T>, decltype((*std::forward<T>(obj)).*f)>;

template <typename F, typename... Args>
constexpr auto invoke(F&& f, Args&&... args)
    -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));

template <typename R, typename F, typename... Args,
          typename = std::enable_if_t<std::is_void<R>::value>>
constexpr auto invoke_r(F&& f, Args&&... args)
    -> decltype(static_cast<void>(::cpp17::internal::invoke(std::forward<F>(f),
                                                            std::forward<Args>(args)...)));

template <typename R, typename F, typename... Args,
          typename = std::enable_if_t<!std::is_void<R>::value>>
constexpr auto invoke_r(F&& f, Args&&... args)
    -> std::enable_if_t<std::is_convertible<decltype(::cpp17::internal::invoke(
                                                std::forward<F>(f), std::forward<Args>(args)...)),
                                            R>::value,
                        R>;

template <typename R, typename F, typename... Args>
constexpr auto is_valid_invoke(std::nullptr_t)
    -> decltype(invoke_r<R>(std::declval<F>(), std::declval<Args>()...), std::true_type());

template <typename R, typename F, typename... Args>
constexpr std::false_type is_valid_invoke(...);

template <bool Enable, typename F, typename... Args>
struct invoke_result {};

template <typename F, typename... Args>
struct invoke_result<true, F, Args...> {
  using type = decltype(::cpp17::internal::invoke(std::declval<F>(), std::declval<Args>()...));
};

}  // namespace internal
}  // namespace cpp17

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_TYPE_TRAITS_H_
