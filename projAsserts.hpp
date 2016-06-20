#ifndef MAIN_HPP
  #error projAsserts.hpp should be included *last*. You tried to include it before main.hpp
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
  #pragma once
  #pragma warning(disable : 4482) // do not warn on MyEnum::MyConstant (which is legal in C++11)
#endif

#ifndef PROJ_ASSERTS_HPP
#define PROJ_ASSERTS_HPP 1

// This *MUST* be the *LAST* file included in every of our cpp and (other) hpp files

// boost/assert.hpp is not idempotent (has no include guard), so
// including this file last is the only way to ensure
// that we get the macro definitions that we want

#define ENABLE_RUNTIME_ASSERTS 1
#define ENABLE_STATIC_ASSERTS 1

#ifdef ENABLE_RUNTIME_ASSERTS
  #define BOOST_ENABLE_ASSERT_HANDLER 1
  // http://www.boost.org/doc/libs/1_57_0/libs/assert/assert.html#BOOST_ASSERT
  // see errorHandling.hpp and errorHandling.cpp
#else
  #define BOOST_DISABLE_ASSERTS 1
  // causes Boost to turn BOOST_ASSERT into ((void)0) ignoring NDEBUG
#endif

#include <boost/assert.hpp>

#ifdef ENABLE_STATIC_ASSERTS
  #include <boost/static_assert.hpp>
#endif

#ifndef ENABLE_RUNTIME_ASSERTS // we must ensure BOOST_STATIC_ASSERT is an empty macro
  #undef  BOOST_STATIC_ASSERT      /* avoid macro redefinition compiler warnings */
  #define BOOST_STATIC_ASSERT(ARG) /* empty */
#endif

#endif // #ifndef PROJ_ASSERTS_HPP
