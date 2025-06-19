# Enable L3 last level cache in GEM5 + NVMAIN

## 修改 5 個檔案以支援 --l3cache

#### 順序
CPU
  │
tol2bus (L2 XBar)
  │
L2 cache 
  │
tol3bus (L3 XBar)
  │
L3 cache 
  │
membus
  │
主記憶體（NVMain/DRAM）

#### Options.py : gem5/configs/common/Options.py

  >增加一行在 l2cache 底下
  ```python
  parser.add_option("--l3cache", action="store_true")
  ```
  
  <img src="https://github.com/user-attachments/assets/1f61695f-e413-4f63-bb36-9c29532a6815" width="50%" height="auto">
    
#### Cache.py : gem5/configs/common/Caches.py

  >增加 L3Cache class 在 L2cache 底下

  ```python
  class L3Cache(Cache):
    assoc = 64
    tag_latency = 32
    data_latency = 32
    response_latency = 32
    mshrs = 32
    tgts_per_mshr = 24
    write_buffers = 16
  ```

  <img src="https://github.com/user-attachments/assets/ab7752c7-c50a-4554-a752-6791220ceefc" width="50%" height="auto">

#### XBar.py : gem5/src/mem/XBar.py

  >增加 L3XBar class 在 L2XBar 底下

  ```python
  class L3XBar(CoherentXBar):
    # 256-bit crossbar by default
    width = 32
    frontend_latency = 1
    forward_latency = 0
    response_latency = 1
    snoop_response_latency = 1
    snoop_filter = SnoopFilter(lookup_latency=0)
  ```

  <img src="https://github.com/user-attachments/assets/760bf039-d7ae-435a-9d81-2d497a062fba" width="50%" height="auto">

#### BaseCPU.py : gem5/src/cpu/BaseCPU.py

  >增加 L3XBar import 在 L2XBar 底下

  ```python
  from XBar import L3XBar
  ```

  <img src="https://github.com/user-attachments/assets/353164fc-0b26-4ec5-a1f5-95262ef895a2" width="50%" height="auto">
  
  <p></p>
  
  >增加 addThreeLevelCacheHierarchy finction 在 addTwoLevelCacheHierarchy 底下

  ```python
  def addThreeLevelCacheHierarchy(self, ic, dc, l3c, iwc=None, dwc=None,
                                    xbar=None):
        self.addPrivateSplitL2Caches(ic, dc, iwc, dwc)
        self.toL3Bus = L3XBar()
        self.connectCachedPorts(self.toL3Bus)
        self.l3cache = l3c
        self.toL3Bus.master = self.l3cache.cpu_side
        self._cached_ports = ['l3cache.mem_side']
  ```
  
  <img src="https://github.com/user-attachments/assets/c1946ad7-de97-431b-a745-55faf5b12851" width="50%" height="auto">

#### CacheConfig.py : gem5/configs/common/CacheConfig.py

> 底下 if options.cpu_type == "O3_ARM_v7a_3": 修改

```python
if options.cpu_type == "O3_ARM_v7a_3":
  try:
    from cores.arm.O3_ARM_v7a import *
  except:
    print("O3_ARM_v7a_3 is unavailable. Did you compile the O3 model?")
    sys.exit(1)

  dcache_class, icache_class, l2_cache_class, walk_cache_class, l3_cache_class = \
    O3_ARM_v7a_DCache, O3_ARM_v7a_ICache, O3_ARM_v7aL2, \
    O3_ARM_v7aWalkCache, O3_ARM_v7aL3
else:
  dcache_class, icache_class, l2_cache_class, walk_cache_class, l3_cache_class = \
    L1_DCache, L1_ICache, L2Cache, None, L3Cache
```
  
<img src="https://github.com/user-attachments/assets/cf94990a-7efb-4d2a-8601-6a6728107177" width="50%" height="auto">

<p></p>

> 修改 if options.l2cache and options.elastic_trace_en 後面的 if options.l2cache，在前面新增 l3cache 判斷建立 L3 cache 與 L3XBar
  
  ```python
  if options.l2cache and  options.l3cache:
     system.l2 = l2_cache_class(clk_domain=system.cpu_clk_domain,
                                size=options.l2_size,
                                assoc=options.l2_assoc)
     system.l3 = l3_cache_class(clk_domain=system.cpu_clk_domain,
                                size=options.l3_size,
                                assoc=options.l3_assoc)

     system.tol2bus = L2XBar(clk_domain = system.cpu_clk_domain)
     system.tol3bus = L3XBar(clk_domain = system.cpu_clk_domain)

     system.l2.cpu_side = system.tol2bus.master
     system.l2.mem_side = system.tol3bus.slave

     system.l3.cpu_side = system.tol3bus.master
     system.l3.mem_side = system.membus.slave
  elif options.l2cache:
  ```

<img src="https://github.com/user-attachments/assets/3331de48-cafa-4a23-9284-11406d24f116" width="50%" height="auto">  

## 測試執行程式 Hello World

#### 重新混合編譯 gem5 + NVMain

> 在 gem5 資料夾底下 terminal輸入

```python
scons EXTRAS=../NVmain build/X86/gem5.opt -j4 # j4 表示使用四個core加速
```

<img src="https://github.com/user-attachments/assets/8a828898-487f-4187-9e3a-8881f7407bc7" width="50%" height="auto">

#### 執行 Hello World 測試

>在 gem5 資料夾底下 terminal輸入

```python
./build/X86/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/x86/linux/hello \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l3_size=1MB --l3_assoc=16 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
<img src="https://github.com/user-attachments/assets/2b433197-fb04-42e7-b5dd-b2c44c376f05" width="50%" height="auto">

#### 輸出畫面

<img src="https://github.com/user-attachments/assets/58db1333-f8a4-4431-9e27-84d8c4e8d87d" width="50%" height="auto">

#### Active energy

<img src="https://github.com/user-attachments/assets/baad52dd-6391-4838-9202-a416e15474eb" width="50%" height="auto">

#### gem5/m5out/stat.txt 看log

> dcache

<img src="https://github.com/user-attachments/assets/d394a6ae-37e6-4042-a33c-53ab673a8e1d" width="50%" height="auto">

<p></p>

> icache

<img src="https://github.com/user-attachments/assets/1fc817ea-5706-4bfa-ab0c-7ba4c8237e6b" width="50%" height="auto">

<p></p>

> l2cache

<img src="https://github.com/user-attachments/assets/0f9871ae-2df9-4a24-93be-77826b1fc34b" width="50%" height="auto">

<p></p>

> l3cache

<img src="https://github.com/user-attachments/assets/b5be644f-7607-411d-9816-d8c89f29eb57" width="50%" height="auto">


