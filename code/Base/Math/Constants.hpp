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
  } null COMPILER_USED;

  static struct TrueTy {
    COMPILER_FORCEINLINE operator bool( ) const { return true; }
  } True COMPILER_USED;

  static struct FalseTy {
    COMPILER_FORCEINLINE operator bool( ) const { return false; }
  } False COMPILER_USED;

  static struct ZeroTy
  {
    COMPILER_FORCEINLINE operator          double   ( ) const { return 0; }
    COMPILER_FORCEINLINE operator          float    ( ) const { return 0; }
    COMPILER_FORCEINLINE operator          long long( ) const { return 0; }
    COMPILER_FORCEINLINE operator unsigned long long( ) const { return 0; }
    COMPILER_FORCEINLINE operator          long     ( ) const { return 0; }
    COMPILER_FORCEINLINE operator unsigned long     ( ) const { return 0; }
    COMPILER_FORCEINLINE operator          int      ( ) const { return 0; }
    COMPILER_FORCEINLINE operator unsigned int      ( ) const { return 0; }
    COMPILER_FORCEINLINE operator          short    ( ) const { return 0; }
    COMPILER_FORCEINLINE operator unsigned short    ( ) const { return 0; }
    COMPILER_FORCEINLINE operator          char     ( ) const { return 0; }
    COMPILER_FORCEINLINE operator unsigned char     ( ) const { return 0; }
  } zero COMPILER_USED;

  static struct OneTy
  {
    COMPILER_FORCEINLINE operator          double   ( ) const { return 1; }
    COMPILER_FORCEINLINE operator          float    ( ) const { return 1; }
    COMPILER_FORCEINLINE operator          long long( ) const { return 1; }
    COMPILER_FORCEINLINE operator unsigned long long( ) const { return 1; }
    COMPILER_FORCEINLINE operator          long     ( ) const { return 1; }
    COMPILER_FORCEINLINE operator unsigned long     ( ) const { return 1; }
    COMPILER_FORCEINLINE operator          int      ( ) const { return 1; }
    COMPILER_FORCEINLINE operator unsigned int      ( ) const { return 1; }
    COMPILER_FORCEINLINE operator          short    ( ) const { return 1; }
    COMPILER_FORCEINLINE operator unsigned short    ( ) const { return 1; }
    COMPILER_FORCEINLINE operator          char     ( ) const { return 1; }
    COMPILER_FORCEINLINE operator unsigned char     ( ) const { return 1; }
  } one COMPILER_USED;

  static struct NegInfTy
  {
    COMPILER_FORCEINLINE operator          double   ( ) const { return -std::numeric_limits<double>::infinity(); }
    COMPILER_FORCEINLINE operator          float    ( ) const { return -std::numeric_limits<float>::infinity(); }
    COMPILER_FORCEINLINE operator          long long( ) const { return std::numeric_limits<long long>::min(); }
    COMPILER_FORCEINLINE operator unsigned long long( ) const { return std::numeric_limits<unsigned long long>::min(); }
    COMPILER_FORCEINLINE operator          long     ( ) const { return std::numeric_limits<long>::min(); }
    COMPILER_FORCEINLINE operator unsigned long     ( ) const { return std::numeric_limits<unsigned long>::min(); }
    COMPILER_FORCEINLINE operator          int      ( ) const { return std::numeric_limits<int>::min(); }
    COMPILER_FORCEINLINE operator unsigned int      ( ) const { return std::numeric_limits<unsigned int>::min(); }
    COMPILER_FORCEINLINE operator          short    ( ) const { return std::numeric_limits<short>::min(); }
    COMPILER_FORCEINLINE operator unsigned short    ( ) const { return std::numeric_limits<unsigned short>::min(); }
    COMPILER_FORCEINLINE operator          char     ( ) const { return std::numeric_limits<char>::min(); }
    COMPILER_FORCEINLINE operator unsigned char     ( ) const { return std::numeric_limits<unsigned char>::min(); }

  } neg_inf COMPILER_USED;

  static struct PosInfTy
  {
    COMPILER_FORCEINLINE operator          double   ( ) const { return std::numeric_limits<double>::infinity(); }
    COMPILER_FORCEINLINE operator          float    ( ) const { return std::numeric_limits<float>::infinity(); }
    COMPILER_FORCEINLINE operator          long long( ) const { return std::numeric_limits<long long>::max(); }
    COMPILER_FORCEINLINE operator unsigned long long( ) const { return std::numeric_limits<unsigned long long>::max(); }
    COMPILER_FORCEINLINE operator          long     ( ) const { return std::numeric_limits<long>::max(); }
    COMPILER_FORCEINLINE operator unsigned long     ( ) const { return std::numeric_limits<unsigned long>::max(); }
    COMPILER_FORCEINLINE operator          int      ( ) const { return std::numeric_limits<int>::max(); }
    COMPILER_FORCEINLINE operator unsigned int      ( ) const { return std::numeric_limits<unsigned int>::max(); }
    COMPILER_FORCEINLINE operator          short    ( ) const { return std::numeric_limits<short>::max(); }
    COMPILER_FORCEINLINE operator unsigned short    ( ) const { return std::numeric_limits<unsigned short>::max(); }
    COMPILER_FORCEINLINE operator          char     ( ) const { return std::numeric_limits<char>::max(); }
    COMPILER_FORCEINLINE operator unsigned char     ( ) const { return std::numeric_limits<unsigned char>::max(); }
  } inf COMPILER_USED, pos_inf COMPILER_USED;

  static struct NaNTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return std::numeric_limits<double>::quiet_NaN(); }
    COMPILER_FORCEINLINE operator float ( ) const { return std::numeric_limits<float>::quiet_NaN(); }
  } nan COMPILER_USED;

  static struct UlpTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return std::numeric_limits<double>::epsilon(); }
    COMPILER_FORCEINLINE operator float ( ) const { return std::numeric_limits<float>::epsilon(); }
  } ulp COMPILER_USED;

  static struct PiTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return 3.14159265358979323846; }
    COMPILER_FORCEINLINE operator float ( ) const { return 3.14159265358979323846f; }
  } pi COMPILER_USED;

  static struct OneOverPiTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return 0.31830988618379069122; }
    COMPILER_FORCEINLINE operator float ( ) const { return 0.31830988618379069122f; }
  } one_over_pi COMPILER_USED;

  static struct TwoPiTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return 6.283185307179586232; }
    COMPILER_FORCEINLINE operator float ( ) const { return 6.283185307179586232f; }
  } two_pi COMPILER_USED;

  static struct OneOverTwoPiTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return 0.15915494309189534561; }
    COMPILER_FORCEINLINE operator float ( ) const { return 0.15915494309189534561f; }
  } one_over_two_pi COMPILER_USED;

  static struct FourPiTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return 12.566370614359172464; } 
    COMPILER_FORCEINLINE operator float ( ) const { return 12.566370614359172464f; }
  } four_pi COMPILER_USED;

  static struct OneOverFourPiTy
  {
    COMPILER_FORCEINLINE operator double( ) const { return 0.079577471545947672804; }
    COMPILER_FORCEINLINE operator float ( ) const { return 0.079577471545947672804f; }
  } one_over_four_pi COMPILER_USED;

  static struct StepTy {
  } step COMPILER_USED;

  static struct EmptyTy {
  } empty COMPILER_USED;

  static struct FullTy {
  } full COMPILER_USED;
}
