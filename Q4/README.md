# Modify last level cache policy based on frequency based replacement policy
## 先執行一次原始的版本(LRU)
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


