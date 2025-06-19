# Modify last level cache policy based on frequency based replacement policy
## 先執行一次原始的版本(LRU)
#### 原始為 LRU，可在 gem5/src/mem/cache/Cache.py 中看到

<img src="https://github.com/user-attachments/assets/9ae412b0-e074-45f2-bf30-f46cfd9909ff" width="50%" height="auto">

#### 執行原始版本
> 使用 quicksort array 大小 1000000
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/quicksort \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=2 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
<img src="https://github.com/user-attachments/assets/c81fa6f7-f0a7-41fc-af03-bf8fa4bcb092" width="50%" height="auto">

#### 執行結果

<img src="https://github.com/user-attachments/assets/79cf05a3-258a-49ac-b563-252e29dbc3bf" width="50%" height="auto">

#### gem5/m5out/stats.txt 看 log

<img src="https://github.com/user-attachments/assets/52900be9-5bb8-47bf-9b51-33ea4be6f773" width="50%" height="auto">

## 修改為frequency based replacement 
#### 新增兩個檔案在 gem5/src/mem/cache/replacement_policies 
> 透過修改 lru_rp.cc 及 lru_rp.hh 達到 LFU-Oldest 所需

> 先複製這兩個檔案，在 replacement_policies y 資料夾底下 terminal 輸入
```python
cp lru_rp.hh fb_rp.hh
cp lru_rp.cc fb_rp.cc
```
<img src="https://github.com/user-attachments/assets/592d51ac-99cf-45a8-a862-d54974298fa6" width="45%" height="auto">

<img src="https://github.com/user-attachments/assets/ab0097b7-395e-4e15-8d30-000e4bb0321a" width="45%" height="auto">

#### 修改 fb_rp.hh

> 將全部 LRU/lru 都換成 FB/fb

> 修改 struct FBReplData : ReplacementData 內容
```python
struct FBReplData : ReplacementData
{
  /** Number of references to this entry since it was reset. */
  unsigned refCount;
  /** Tick on which the entry was last touched. */
  Tick lastTouchTick;

  /**
  * Default constructor. Invalidate data.
  */
  FBReplData() : refCount(0),lastTouchTick(0) {}
};
```

<img src="https://github.com/user-attachments/assets/6632b625-a813-482b-abf4-997bf93d2924" width="50%" height="auto">

#### 修改 fb_rp.cc

> 將全部 LRU/lru 都換成 FB/fb

> FBRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)，在裡面新增 Reset reference count

```python
// Reset reference count	
std::static_pointer_cast<FBReplData>(replacement_data)->refCount = 0;
```

<img src="https://github.com/user-attachments/assets/5845c896-d82b-4247-abcb-c5a051495d18" width="50%" height="auto">

>FBRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const，在裡面新增 Update reference count

```python
// Update reference count
std::static_pointer_cast<FBReplData>(replacement_data)->refCount++;
```

<img src="https://github.com/user-attachments/assets/b839b098-aebb-481a-9c2e-8604373d251a" width="50%" height="auto">

>FBRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const，在裡面新增 Reset reference count

```python
// Reset reference count
std::static_pointer_cast<FBReplData>(replacement_data)->refCount = 1;
```

<img src="https://github.com/user-attachments/assets/652c56b9-37c5-4e36-aa59-2cf2d8e1f583" width="50%" height="auto">

> FBRP::getVictim(const ReplacementCandidates& candidates) const，for 迴圈裡面 修改 if-else if
> 比較 refCount，refCount 相同時，比較 lastTouchTick
```python
if (std::static_pointer_cast<FBReplData>(
    candidate->replacementData)->refCount <
    std::static_pointer_cast<FBReplData>(
        victim->replacementData)->refCount) {
    victim = candidate;
}
else if (std::static_pointer_cast<FBReplData>(
    candidate->replacementData)->refCount ==
    std::static_pointer_cast<FBReplData>(
        victim->replacementData)->refCount &&
    std::static_pointer_cast<FBReplData>(
    candidate->replacementData)->lastTouchTick <
    std::static_pointer_cast<FBReplData>(
        victim->replacementData)->lastTouchTick) {
    victim = candidate;
}
```

<img src="https://github.com/user-attachments/assets/a17ec397-67fb-4a44-bc36-b5ee4d2054e6" width="50%" height="auto">

#### 修改 SConscript

> gem5/src/mem/cache/replacement_policies/SConscript 在底下新增一行
```python
Source('fb_rp.cc')
```

<img src="https://github.com/user-attachments/assets/f4eed4fb-65d0-4c19-9eef-15982fe180dc" width="50%" height="auto">

#### 修改 ReplacementPolicies.py

> gem5/src/mem/cache/replacement_policies/ReplacementPolicies.py 新增 FBRP class

```python
class FBRP(BaseReplacementPolicy):
    type = 'FBRP'
    cxx_class = 'FBRP'
    cxx_header = "mem/cache/replacement_policies/fb_rp.hh"
```

<img src="https://github.com/user-attachments/assets/f4eed4fb-65d0-4c19-9eef-15982fe180dc" width="50%" height="auto">

#### 修改 Caches.py

> gem5/config/common/Caches.py L3Cache 更改取代的 policy

```python
replacement_policy = Param.BaseReplacementPolicy(FBRP(), "Replacement policy")
```

<img src="https://github.com/user-attachments/assets/cbef132d-3afc-4d43-aa96-d68c1f7f1fdc" width="50%" height="auto">

#### 重新混和編譯 NVmain 和 gem5

> 在 gem5 資料夾底下 terminal 輸入

``` python
scons EXTRAS=../NVmain build/X86/gem5.opt -j4   # j4 表示使用四個core加速
```

<img src="https://github.com/user-attachments/assets/68f7945a-0b71-4ba8-9686-322455096427" width="50%" height="auto">

#### 執行frequency based版本
> 使用 quicksort array 大小 1000000
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/quicksort \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=2 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
<img src="https://github.com/user-attachments/assets/394e1aac-3226-4d63-9e40-681265beb603" width="50%" height="auto">


#### 執行結果

<img src="https://github.com/user-attachments/assets/e52da620-c173-49cf-84e3-f062d45d811e" width="50%" height="auto">

#### gem5/m5out/stats.txt 看 log

<img src="https://github.com/user-attachments/assets/5765ab7b-d90e-4e1e-8b91-a8acbcf28921" width="50%" height="auto">





