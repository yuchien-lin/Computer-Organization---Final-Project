# Config last level cache to 2-way and full-way associative cache and test performance

## 下載並產生quicksort的執行檔

#### 下載zip檔，接壓縮到gem5

<img src="https://github.com/user-attachments/assets/36674076-9fb9-4c82-927f-78a9c1f84b05" width="50%" height="auto">

#### 產生執行檔
> 在 benchmark 資料夾底下 terminal輸入
```python
gcc --static quicksort.c -o quicksort
```
<img src="https://github.com/user-attachments/assets/390ca45c-9ab6-48d8-b5fa-5110ba2e65ca" width="50%" height="auto">

## 2-way associative
#### 執行quicksort 
> 在 gem5 資料夾底下 terminal輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/quicksort \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=2 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
<img src="https://github.com/user-attachments/assets/f9fe087b-97d8-4cb9-b4ce-86f04a1373cb" width="50%" height="auto">

#### 執行結果

<img src="https://github.com/user-attachments/assets/f9036b5f-bff3-4c16-bdbf-0182a46e2387" width="50%" height="auto">

#### array 大小 100000，gem5/m5out/stats.txt 看 log
<img src="https://github.com/user-attachments/assets/7e609661-4170-40ee-9f8d-1f00e8715e3f" width="50%" height="auto">

#### quicksort 內 array 大小 1000000，gem5/m5out/stats.txt 看 log
> array 大小 100000 看不出差距，修改 quicksort 內 array大小 變為10倍，重新編譯，其他步驟相同

 <img src="https://github.com/user-attachments/assets/c198330d-77fb-4f74-98c4-0df6ef10d791" width="40%" height="auto">

 <img src="https://github.com/user-attachments/assets/b4be86b0-0649-4d4e-b1b9-e6c5795cc56f" width="50%" height="auto">

## full-way associative
#### 執行quicksort
> L3 cache 是 1MB，block size 是 64B，full-way associative 就是 --l3_assoc=16384
> 在 gem5 資料夾底下 terminal輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/quicksort \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=16384 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
<img src="https://github.com/user-attachments/assets/31476d6c-f83c-436f-bd0e-f97d655927d1" width="50%" height="auto">

#### 執行結果
<img src="https://github.com/user-attachments/assets/95704e6f-33cc-4eca-a385-c9685d9dc1a9" width="50%" height="auto">

#### array 大小 100000，gem5/m5out/stats.txt 看 log
<img src="https://github.com/user-attachments/assets/037b539c-069f-458b-8180-22331ec7340d" width="50%" height="auto">

#### quicksort 內 array 大小 1000000，gem5/m5out/stats.txt 看 log
> array 大小 100000 看不出差距，修改 quicksort 內 array大小 變為10倍，重新編譯，其他步驟相同
<img src="https://github.com/user-attachments/assets/c198330d-77fb-4f74-98c4-0df6ef10d791" width="40%" height="auto">
<img src="https://github.com/user-attachments/assets/e0cf7b66-77b9-4bf8-87f1-88849972be67" width="50%" height="auto">















