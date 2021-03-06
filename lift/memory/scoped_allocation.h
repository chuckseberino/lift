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

#include "../types.h"
#include "../backends.h"
#include "../decorators.h"

#include "persistent_allocation.h"

namespace lift {

// an allocation that releases memory upon destruction
template <target_system system,
          typename T,
          typename index_type = uint32,
          typename allocator = typename default_memory_allocator<system>::allocator_type>
struct scoped_allocation : public persistent_allocation<system, T, index_type, allocator>
{
    typedef persistent_allocation<system, T, index_type, allocator> base;
    using base::base;

    scoped_allocation() = default;

    // Allow move construction, but ensure other gets cleared.
    scoped_allocation(scoped_allocation&& other)
        : base(other) // copy
    {
         other.storage = nullptr;
         other.storage_size = 0;
    }

    // Allow move assignment, but ensure other gets cleared.
    scoped_allocation& operator=(scoped_allocation&& other)
    {
        base::operator=(other);
        other.storage = nullptr;
        other.storage_size = 0;
        return *this;
    }

    ~scoped_allocation()
    {
        base::free();
    }

    // Disallow copy construction, which is inherently a thin copy.
    scoped_allocation(const scoped_allocation& other) = delete;
    // Disallow general assignment.
    scoped_allocation& operator=(const scoped_allocation&) = delete;
};

template <target_system system,
          typename T,
          typename index_type = uint32,
          typename allocator = typename default_memory_allocator<system>::suballocator_type>
using scoped_suballocation = scoped_allocation<system, T, index_type, allocator>;

} // namespace lift
