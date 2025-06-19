/**
 * Copyright (c) 2018 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Carvalho
 */

#include "mem/cache/replacement_policies/da_rp.hh"

#include <cassert>
#include <memory>

#include "params/DARP.hh"

DARP::DARP(const Params *p)
    : BaseReplacementPolicy(p)
{
}

void
DARP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
const
{
    // Reset last touch timestamp
    std::static_pointer_cast<DAReplData>(
        replacement_data)->lastTouchTick = Tick(0);
}

void
DARP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Update last touch timestamp
    std::static_pointer_cast<DAReplData>(
        replacement_data)->lastTouchTick = curTick();
}

void
DARP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Set last touch timestamp
    std::static_pointer_cast<DAReplData>(
        replacement_data)->lastTouchTick = curTick();
}

ReplaceableEntry*
DirtyAwareDARP::getVictim(const ReplacementCandidates& candidates) const
{
    ReplaceableEntry* victim = nullptr;
    Tick min_tick = Tick(-1);

    // select clean block smallest in lastTouchTick 
    for (const auto& candidate : candidates) {
        auto blk = static_cast<CacheBlk*>(candidate);
        if (!blk->isDirty()) {
            auto da_data = std::static_pointer_cast<DAReplData>(candidate->replacementData);
            if (!victim || da_data->lastTouchTick < min_tick) {
                victim = candidate;
                min_tick = da_data->lastTouchTick;
            }
        }
    }
    if (victim)
        return victim;

    // if all dirty block use LRU select one
    victim = candidates[0];
    auto victim_data = std::static_pointer_cast<DAReplData>(victim->replacementData);
    for (const auto& candidate : candidates) {
        auto cand_data = std::static_pointer_cast<DAReplData>(candidate->replacementData);
        if (cand_data->lastTouchTick < victim_data->lastTouchTick) {
            victim = candidate;
            victim_data = cand_data;
        }
    }
    return victim;
}

std::shared_ptr<ReplacementData>
DARP::instantiateEntry()
{
    return std::shared_ptr<ReplacementData>(new DAReplData());
}

DARP*
DARPParams::create()
{
    return new DARP(this);
}
