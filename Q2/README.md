# Enable L3 last level cache in GEM5 + NVMAIN

## 修改 5 個檔案以支援 --l3cache

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
    assoc = 16
    tag_latency = 20
    data_latency = 20
    response_latency = 20
    mshrs = 20
    tgts_per_mshr = 12
    write_buffers = 8
  ```

  <img src="https://github.com/user-attachments/assets/d27aa594-540a-43ff-bb17-962474404755" width="50%" height="auto">

#### XBar.py : gem5/src/mem/XBar.py

  >增加 L3XBar class 在 L2XBar 底下

  ```python
  class L3XBar(CoherentXBar):
    width = 64  # 512 bits
    frontend_latency = 1
    forward_latency = 0
    response_latency = 1
    snoop_response_latency = 1
    snoop_filter = SnoopFilter(lookup_latency=0)
  ```

  <img src="https://github.com/user-attachments/assets/ed251415-9389-422d-8724-8dcc2b750aa0" width="50%" height="auto">

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

>config_cache 的第一行 options.caches or options.l2cache 後面多加上 or options.l3cache

<p></p>  

> 底下 if options.cpu_type == "O3_ARM_v7a_3": 修改

```python
if options.cpu_type == "O3_ARM_v7a_3":
  try:
    from cores.arm.O3_ARM_v7a import *
  except:
    print("O3_ARM_v7a_3 is unavailable. Did you compile the O3 model?")
    sys.exit(1)

  dcache_class, icache_class, l2_cache_class, l3_cache_class, walk_cache_class = \
    O3_ARM_v7a_DCache, O3_ARM_v7a_ICache, O3_ARM_v7aL2, O3_ARM_v7aL3,\
    O3_ARM_v7aWalkCache
else:
  dcache_class, icache_class, l2_cache_class, l3_cache_class, walk_cache_class = \
    L1_DCache, L1_ICache, L2Cache , L3Cache, None
```
  
<img src="https://github.com/user-attachments/assets/4c009b09-6b6e-4b0b-a22d-3f68f81e5312" width="50%" height="auto">

<p></p>

> 修改 if options.l2cache and options.elastic_trace_en 後面的 if options.l2cache，在前面新增 l3cache 判斷建立 L3 cache 與 L3XBar
  
```python
  if options.l3cache:
    system.l3 = l3_cache_class(clk_domain=system.cpu_clk_domain,
                               size=options.l3_size, assoc=options.l3_assoc)
    system.tol3bus = L3XBar(clk_domain=system.cpu_clk_domain)
    system.l3.mem_side = system.membus.slave
  elif options.l2cache:
    # Provide a clock for the L2 and the L1-to-L2 bus here as they
    # are not connected using addTwoLevelCacheHierarchy. Use the
    # same clock as the CPUs.
    system.l2 = l2_cache_class(clk_domain=system.cpu_clk_domain,
                               size=options.l2_size,
                               assoc=options.l2_assoc)
  
    system.tol2bus = L2XBar(clk_domain = system.cpu_clk_domain)
    system.l2.cpu_side = system.tol2bus.master
    system.l2.mem_side = system.membus.slave
```

<img src="https://github.com/user-attachments/assets/aa79fb91-538c-425a-a229-752614bbee3c" width="50%" height="auto">  

<p></p>

> 修改for i in xrange(options.num_cpus) 內的 if options.l2cache ，在前面新增 l3cache 判斷，每顆 CPU 建立 private L2，並連到共用 L3 (L2 mem_side 連到 tol3bus)
      
```python
if options.l3cache:
   system.cpu[i].l2 = l2_cache_class(clk_domain=system.cpu_clk_domain,
                                     size=options.l2_size,
                                     assoc=options.l2_assoc)
   system.cpu[i].tol2bus = L2XBar(clk_domain=system.cpu_clk_domain)

   system.cpu[i].l2.cpu_side = system.cpu[i].tol2bus.master
   system.cpu[i].l2.mem_side = system.tol3bus.slave

   system.cpu[i].connectAllPorts(system.cpu[i].tol2bus, system.membus)

elif options.l2cache:
   system.cpu[i].connectAllPorts(system.tol2bus, system.membus)
```
<img src="https://github.com/user-attachments/assets/80a48c0d-b52f-49b1-8e77-96f23e546fbe" width="50%" height="auto">

<p></p>

> for 迴圈結束後，把 tol3bus 的 master 連到 L3 的 cpu_side
      
```python
if options.l3cache:
  system.l3.cpu_side = system.tol3bus.master
```
<img src="https://github.com/user-attachments/assets/cdb42a2c-32f9-44be-a8ba-4f8e3b336143" width="50%" height="auto">

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
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

#### 輸出畫面

<img src="https://github.com/user-attachments/assets/37fc60d5-9b7d-4af7-b34e-cba508e14552" width="50%" height="auto">

#### gem5/m5out/stat.txt 看log

> dcache

<img src="https://github.com/user-attachments/assets/49ca9c16-6bbe-43b4-b05a-b00c0d3717c5" width="50%" height="auto">

<p></p>

> icache

<img src="https://github.com/user-attachments/assets/4ab9883b-a595-42be-aeca-a19808e8ac26" width="50%" height="auto">

<p></p>

> l2cache

<img src="https://github.com/user-attachments/assets/bc2e1434-65a9-4efb-b2ed-fb7b15b24e18" width="50%" height="auto">

<p></p>

> l3cache

<img src="https://github.com/user-attachments/assets/07c82e9c-b28c-4cdb-930a-8949e916e2b5" width="50%" height="auto">

