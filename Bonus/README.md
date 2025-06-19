# Design last level cache policy to reduce the energy consumption of pcm_based main memory Baseline:LRU

## 原始的LRU

#### 執行 quicksort 4-way
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/quicksort \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=4 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

<img src="https://github.com/user-attachments/assets/f8413139-0e7c-4232-a142-869e081dd4c5" width="50%" height="auto">


#### 執行結果 Energy

<img src="https://github.com/user-attachments/assets/c3b9af97-c47c-430d-b114-531300afc9c4" width="50%" height="auto">

#### gem5/m5out/stats.txt 看 log

<img src="https://github.com/user-attachments/assets/ab463075-162e-4b95-9dfc-6102ab45a884" width="50%" height="auto">

## 修改 replacement policy

#### 優先淘汰 clean block (dirty-aware policy)
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

#### 修改 da_rp.hh : 
> gem5/src/mem/cache/replacement_policies/da_rp.hh

> 將全部 LRU/lru 都換成 DA/da

> 其他不用動
> 
<img src="https://github.com/user-attachments/assets/ed3b1ec1-e166-480e-bb4b-61be42a23667" width="50%" height="auto">

#### 修改 da_rp.cc : 
> gem5/src/mem/cache/replacement_policies/da_rp.cc

> 將全部 LRU/lru 都換成 DA/da

> 多加一個 include
```python
#include "mem/cache/base.hh"
```

<img src="https://github.com/user-attachments/assets/208e5b45-40fb-4b88-89b9-d7721b1216f2" width="50%" height="auto">

> 修改 DARP::getVictim(const ReplacementCandidates& candidates) const
```python
DARP::getVictim(const ReplacementCandidates& candidates) const
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

<img src="https://github.com/user-attachments/assets/a24446fa-f0d8-4ab7-8ee9-5f58b4965c88" width="50%" height="auto">

#### 修改 ReplacementPolicies.py 
> gem5/src/mem/cache/replacement_policies/ReplacementPolicies.py

> 新增 DARP class
```python
class DARP(BaseReplacementPolicy):
    type = 'DARP'
    cxx_class = 'DARP'
    cxx_header = "mem/cache/replacement_policies/da_rp.hh"
```

<img src="https://github.com/user-attachments/assets/27aab017-94f6-4aa1-9a4f-bd4a9cc763a5" width="50%" height="auto">

#### 修改 SConscript 
> gem5/src/mem/cache/replacement_policies/SConscript

> 在底下新增一行

```python
Source('da_rp.cc')
```
<img src="https://github.com/user-attachments/assets/cc254f6a-7116-496c-81e2-0ce4efc415ea" width="50%" height="auto">

#### 修改 Caches.py
> gem5/config/common/Caches.py L3Cache 更改BaseReplacementPolicy的 policy

```python
replacement_policy = Param.BaseReplacementPolicy(DARP(), "Replacement policy")
```

<img src="https://github.com/user-attachments/assets/2e90e375-c960-4766-9f00-6a13b3f62815" width="50%" height="auto">

#### 重新混和編譯 NVmain 和 gem5

> 在 gem5 資料夾底下 terminal 輸入

```python
scons EXTRAS=../NVmain build/X86/gem5.opt -j4   # j4 表示使用四個core加速
```

<img src="https://github.com/user-attachments/assets/98b21ef2-9f2e-4019-b013-4518c8b7dbe0" width="50%" height="auto">

#### 執行 quicksort 4-way
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/quicksort \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=4 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

<img src="https://github.com/user-attachments/assets/83e4af40-d4c6-471e-a279-394fc617bffd" width="50%" height="auto">

#### 執行結果 Energy

<img src="https://github.com/user-attachments/assets/6405c742-9030-4f3d-bb83-aa65c5fe1266" width="50%" height="auto">

#### gem5/m5out/stats.txt 看 log

<img src="https://github.com/user-attachments/assets/4cedac1e-14a9-4ae8-9d9f-351a9c8cea8e" width="50%" height="auto">






