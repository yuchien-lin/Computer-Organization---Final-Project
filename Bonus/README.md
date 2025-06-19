# Design last level cache policy to reduce the energy consumption of pcm_based main memory Baseline:LRU

## 原始的LRU

#### 執行hello 4-way
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/x86/linux/hello \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=4 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

<img src="https://github.com/user-attachments/assets/2237265a-4027-4013-ba59-d72bef2d68d5" width="50%" height="auto">

#### 執行結果 Active energy

<img src="https://github.com/user-attachments/assets/01017495-c579-41a7-87b2-fa548184ee3f" width="50%" height="auto">


## 修改 replacement policy

#### 優先淘汰 clean block
> clean block不需要寫回主記憶體
> 當一組內全部都是 dirty block 時，才選最久未用的 dirty block

#### 新增一個 replacement way 
> 新增兩個檔案在 gem5/src/mem/cache/replacement_policies，如Q4一樣
> 透過修改 lru_rp.cc 及 lru_rp.hh 達到所需
> 先複製這兩個檔案，在 replacement_policies 資料夾底下 terminal 輸入
```python
cp lru_rp.hh da_rp.hh
cp lru_rp.cc da_rp.cc
```

<img src="https://github.com/user-attachments/assets/ae1fe7b4-eb8f-4f0d-8a71-266dd4145d85" width="45%" height="auto"> 
<img src="https://github.com/user-attachments/assets/89e0d6af-3e90-49b9-99d3-8e8acac0962a" width="45%" height="auto">

#### 修改 da_rp.hh
> 將全部 LRU/lru 都換成 DA/da
> 其他不用動
<img src="https://github.com/user-attachments/assets/ed3b1ec1-e166-480e-bb4b-61be42a23667" width="50%" height="auto">

#### 修改 da_rp.cc

> 將全部 LRU/lru 都換成 DA/da
> 修改 DARP::getVictim(const ReplacementCandidates& candidates) const
```python
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
```

<img src="https://github.com/user-attachments/assets/c1da267a-c452-423c-89ad-5cd5ab6b09d0" width="50%" height="auto">

