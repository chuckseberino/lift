/*
 * Lift
 *
 * Copyright (c) 2014-2015, NVIDIA CORPORATION
 * Copyright (c) 2015, Nuno Subtil <subtil@gmail.com>
 * Copyright (c) 2015, Roche Molecular Systems Inc.
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

namespace lift {

#include <lift/decorators.h>

enum target_system
{
    host,
    cuda,
};

} // namespace lift

#if LIFT_CUDA
#include <thrust/device_vector.h>
#include <thrust/system/cuda/vector.h>
#endif // LIFT_CUDA

#include <thrust/system/tbb/vector.h>
#include <thrust/execution_policy.h>

namespace lift {

template <target_system system>
struct backend_policy
{ };


#if LIFT_CUDA
template <>
struct backend_policy<cuda>
{
    typedef thrust::system::cuda::tag tag;

    static inline decltype(thrust::cuda::par) execution_policy(void)
    {
        return thrust::cuda::par;
    }
};
#endif // LIFT_CUDA

template <>
struct backend_policy<host>
{
    typedef thrust::system::tbb::tag tag;

    static inline decltype(thrust::tbb::par) execution_policy(void)
    {
        return thrust::tbb::par;
    }
};

} // namespace lift

// ugly macro hackery to force arbitrary device function / method instantiation
// note: we intentionally never instantiate device functions for the host system

#if LIFT_CUDA
#define __FUNC_CUDA(fun) void *ptr_cuda = (void *)fun<lift::cuda>;
#define __METHOD_CUDA(base, method) void *ptr_cuda = (void *)&base<lift::cuda>::method;
#else
#define __FUNC_CUDA(fun)
#define __METHOD_CUDA(base, method)
#endif // LIFT_CUDA

#define __FUNC_TBB(fun) auto *ptr_TBB= (void *)fun<lift::host>;
#define __METHOD_TBB(base, method) auto ptr_TBB = (void *)&base<lift::host>::method;

// free function instantiation
#define INSTANTIATE(fun) \
        namespace __ ## fun ## __instantiation {    \
            __FUNC_CUDA(fun);                       \
            __FUNC_TBB(fun);                        \
    }

// method instantiation
#define METHOD_INSTANTIATE(base, method) \
        namespace __ ## base ## __ ## method ## __instantiation {   \
            __METHOD_CUDA(base, method);                            \
            __METHOD_TBB(base, method);                             \
    }
