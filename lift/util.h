/*
 * Lift
 *
 * Copyright (c) 2014-2015, NVIDIA CORPORATION
 * Copyright (c) 2015, Nuno Subtil <subtil@gmail.com>
 * Copyright (c) 2015, Roche Molecular Systems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <type_traits>
#include <assert.h>

#include <lift/decorators.h>

namespace lift {

// swap two variables
// use unique name so STL doesn't get confused with std version
template <typename T>
static inline LIFT_HOST_DEVICE void exchange(T& a, T& b)
{
    T temp;
    temp = a;
    a = b;
    b = temp;
}

// get the lane ID for a GPU thread
static inline LIFT_DEVICE unsigned int lane_id(void)
{
#if LIFT_DEVICE_COMPILATION
    unsigned int ret;
    asm("mov.u32 %0, %laneid;" : "=r"(ret));
    return ret;
#else
    assert(!"lane_id not supported on CPU");
    return 0;
#endif
}

// perform integer division, rounding up
// result = (a + b - 1) / b
template <typename T>
static inline LIFT_HOST_DEVICE T idivup(const T a, const T b)
{
    // compile-time type checking: make sure we don't get floats passed in
    static_assert(std::is_integral<T>::value && !std::is_same<T, bool>::value,
                  "idivup requires integral numeric type");

    // runtime checks: both a and b must be positive, avoid wraparound
    assert(a > 0 && b > 0);
    assert((a + b - 1) >= a);

    return (a + (b - 1)) / b;
}

// Retrieve the warp size for this device
static inline uint32 get_warp_size(void)
{
    int active_device;
    cudaGetDevice(&active_device);

    struct cudaDeviceProp dp;
    cudaGetDeviceProperties(&dp, active_device);

    return dp.warpSize;
}

} // namespace lift
