// ======================================================================== //
// Copyright 2009-2012 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "Base/Platform.hpp"

#include <limits>

namespace Base
{
  static struct NullTy {
  } null MAYBE_UNUSED;

  static struct TrueTy {
    FORCEINLINE operator bool( ) const { return true; }
  } True MAYBE_UNUSED;

  static struct FalseTy {
    FORCEINLINE operator bool( ) const { return false; }
  } False MAYBE_UNUSED;

  static struct ZeroTy
  {
    FORCEINLINE operator          double   ( ) const { return 0; }
    FORCEINLINE operator          float    ( ) const { return 0; }
    FORCEINLINE operator          long long( ) const { return 0; }
    FORCEINLINE operator unsigned long long( ) const { return 0; }
    FORCEINLINE operator          long     ( ) const { return 0; }
    FORCEINLINE operator unsigned long     ( ) const { return 0; }
    FORCEINLINE operator          int      ( ) const { return 0; }
    FORCEINLINE operator unsigned int      ( ) const { return 0; }
    FORCEINLINE operator          short    ( ) const { return 0; }
    FORCEINLINE operator unsigned short    ( ) const { return 0; }
    FORCEINLINE operator          char     ( ) const { return 0; }
    FORCEINLINE operator unsigned char     ( ) const { return 0; }
  } zero MAYBE_UNUSED;

  static struct OneTy
  {
    FORCEINLINE operator          double   ( ) const { return 1; }
    FORCEINLINE operator          float    ( ) const { return 1; }
    FORCEINLINE operator          long long( ) const { return 1; }
    FORCEINLINE operator unsigned long long( ) const { return 1; }
    FORCEINLINE operator          long     ( ) const { return 1; }
    FORCEINLINE operator unsigned long     ( ) const { return 1; }
    FORCEINLINE operator          int      ( ) const { return 1; }
    FORCEINLINE operator unsigned int      ( ) const { return 1; }
    FORCEINLINE operator          short    ( ) const { return 1; }
    FORCEINLINE operator unsigned short    ( ) const { return 1; }
    FORCEINLINE operator          char     ( ) const { return 1; }
    FORCEINLINE operator unsigned char     ( ) const { return 1; }
  } one MAYBE_UNUSED;

  static struct NegInfTy
  {
    FORCEINLINE operator          double   ( ) const { return -std::numeric_limits<double>::infinity(); }
    FORCEINLINE operator          float    ( ) const { return -std::numeric_limits<float>::infinity(); }
    FORCEINLINE operator          long long( ) const { return std::numeric_limits<long long>::min(); }
    FORCEINLINE operator unsigned long long( ) const { return std::numeric_limits<unsigned long long>::min(); }
    FORCEINLINE operator          long     ( ) const { return std::numeric_limits<long>::min(); }
    FORCEINLINE operator unsigned long     ( ) const { return std::numeric_limits<unsigned long>::min(); }
    FORCEINLINE operator          int      ( ) const { return std::numeric_limits<int>::min(); }
    FORCEINLINE operator unsigned int      ( ) const { return std::numeric_limits<unsigned int>::min(); }
    FORCEINLINE operator          short    ( ) const { return std::numeric_limits<short>::min(); }
    FORCEINLINE operator unsigned short    ( ) const { return std::numeric_limits<unsigned short>::min(); }
    FORCEINLINE operator          char     ( ) const { return std::numeric_limits<char>::min(); }
    FORCEINLINE operator unsigned char     ( ) const { return std::numeric_limits<unsigned char>::min(); }

  } neg_inf MAYBE_UNUSED;

  static struct PosInfTy
  {
    FORCEINLINE operator          double   ( ) const { return std::numeric_limits<double>::infinity(); }
    FORCEINLINE operator          float    ( ) const { return std::numeric_limits<float>::infinity(); }
    FORCEINLINE operator          long long( ) const { return std::numeric_limits<long long>::max(); }
    FORCEINLINE operator unsigned long long( ) const { return std::numeric_limits<unsigned long long>::max(); }
    FORCEINLINE operator          long     ( ) const { return std::numeric_limits<long>::max(); }
    FORCEINLINE operator unsigned long     ( ) const { return std::numeric_limits<unsigned long>::max(); }
    FORCEINLINE operator          int      ( ) const { return std::numeric_limits<int>::max(); }
    FORCEINLINE operator unsigned int      ( ) const { return std::numeric_limits<unsigned int>::max(); }
    FORCEINLINE operator          short    ( ) const { return std::numeric_limits<short>::max(); }
    FORCEINLINE operator unsigned short    ( ) const { return std::numeric_limits<unsigned short>::max(); }
    FORCEINLINE operator          char     ( ) const { return std::numeric_limits<char>::max(); }
    FORCEINLINE operator unsigned char     ( ) const { return std::numeric_limits<unsigned char>::max(); }
  } inf MAYBE_UNUSED, pos_inf MAYBE_UNUSED;

  static struct NaNTy
  {
    FORCEINLINE operator double( ) const { return std::numeric_limits<double>::quiet_NaN(); }
    FORCEINLINE operator float ( ) const { return std::numeric_limits<float>::quiet_NaN(); }
  } nan MAYBE_UNUSED;

  static struct UlpTy
  {
    FORCEINLINE operator double( ) const { return std::numeric_limits<double>::epsilon(); }
    FORCEINLINE operator float ( ) const { return std::numeric_limits<float>::epsilon(); }
  } ulp MAYBE_UNUSED;

  static struct PiTy
  {
    FORCEINLINE operator double( ) const { return 3.14159265358979323846; }
    FORCEINLINE operator float ( ) const { return 3.14159265358979323846f; }
  } pi MAYBE_UNUSED;

  static struct OneOverPiTy
  {
    FORCEINLINE operator double( ) const { return 0.31830988618379069122; }
    FORCEINLINE operator float ( ) const { return 0.31830988618379069122f; }
  } one_over_pi MAYBE_UNUSED;

  static struct TwoPiTy
  {
    FORCEINLINE operator double( ) const { return 6.283185307179586232; }
    FORCEINLINE operator float ( ) const { return 6.283185307179586232f; }
  } two_pi MAYBE_UNUSED;

  static struct OneOverTwoPiTy
  {
    FORCEINLINE operator double( ) const { return 0.15915494309189534561; }
    FORCEINLINE operator float ( ) const { return 0.15915494309189534561f; }
  } one_over_two_pi MAYBE_UNUSED;

  static struct FourPiTy
  {
    FORCEINLINE operator double( ) const { return 12.566370614359172464; } 
    FORCEINLINE operator float ( ) const { return 12.566370614359172464f; }
  } four_pi MAYBE_UNUSED;

  static struct OneOverFourPiTy
  {
    FORCEINLINE operator double( ) const { return 0.079577471545947672804; }
    FORCEINLINE operator float ( ) const { return 0.079577471545947672804f; }
  } one_over_four_pi MAYBE_UNUSED;

  static struct StepTy {
  } step MAYBE_UNUSED;

  static struct EmptyTy {
  } empty MAYBE_UNUSED;

  static struct FullTy {
  } full MAYBE_UNUSED;
}
