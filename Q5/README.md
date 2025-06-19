# Test the performance of write back and write through policy based on 4-way associative cache with isscc_pcm

## 產生 multiply 執行檔

> 在 gem5/benchmark 資料夾底下 terminal輸入

```python
gcc --static multiply.c -o multiply
```
<img src="" width="50%" height="auto">

## 用原始的跑為 write back

#### 用 4-way 執行
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/multiply \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=4 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
<img src="" width="50%" height="auto">

#### 執行結果

<img src="" width="50%" height="auto">

#### gem5/m5out/stats.txt 看 log

<img src="" width="50%" height="auto">

## 修改為用 write through

#### 修改 base.cc
> 在 gem5/src/mem/cache/base.cc 在 elif內 多加一段
```python
if (blk ->isWritable()){
    PacketPtr  writeCleanPkt = writecleanBlk(blk, pkt->req->getDest(), pkt->id);
    writebacks.push_back(writeCleanPkt);
}
```

<img src="" width="50%" height="auto">

#### 重新混和編譯 NVmain 和 gem5

> 在 gem5 資料夾底下 terminal 輸入

```python
scons EXTRAS=../NVmain build/X86/gem5.opt -j4   # j4 表示使用四個core加速
```

<img src="" width="50%" height="auto">

#### 執行 write through 版本
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/multiply \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=4 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

<img src="" width="50%" height="auto">

#### 執行結果

<img src="" width="50%" height="auto">

#### gem5/m5out/stats.txt 看 log

<img src="" width="50%" height="auto">

