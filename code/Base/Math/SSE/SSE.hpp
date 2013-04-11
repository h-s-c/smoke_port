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

/* http://store.steampowered.com/hwsurvey */
#include <xmmintrin.h> /* SSE */
#include <emmintrin.h> /* SSE2 */
#include <pmmintrin.h> /* SSE3 */
#include <tmmintrin.h> /* SSSE3 */
#include <smmintrin.h> /* SSE4.1/SSE4.2 */

const __m128 _mm_lookupmask_ps[16] = {
  _mm_castsi128_ps(_mm_set_epi32( 0, 0, 0, 0)),
  _mm_castsi128_ps(_mm_set_epi32( 0, 0, 0,-1)),
  _mm_castsi128_ps(_mm_set_epi32( 0, 0,-1, 0)),
  _mm_castsi128_ps(_mm_set_epi32( 0, 0,-1,-1)),
  _mm_castsi128_ps(_mm_set_epi32( 0,-1, 0, 0)),
  _mm_castsi128_ps(_mm_set_epi32( 0,-1, 0,-1)),
  _mm_castsi128_ps(_mm_set_epi32( 0,-1,-1, 0)),
  _mm_castsi128_ps(_mm_set_epi32( 0,-1,-1,-1)),
  _mm_castsi128_ps(_mm_set_epi32(-1, 0, 0, 0)),
  _mm_castsi128_ps(_mm_set_epi32(-1, 0, 0,-1)),
  _mm_castsi128_ps(_mm_set_epi32(-1, 0,-1, 0)),
  _mm_castsi128_ps(_mm_set_epi32(-1, 0,-1,-1)),
  _mm_castsi128_ps(_mm_set_epi32(-1,-1, 0, 0)),
  _mm_castsi128_ps(_mm_set_epi32(-1,-1, 0,-1)),
  _mm_castsi128_ps(_mm_set_epi32(-1,-1,-1, 0)),
  _mm_castsi128_ps(_mm_set_epi32(-1,-1,-1,-1))
};
