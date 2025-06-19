# Test the performance of write back and write through policy based on 4-way associative cache with isscc_pcm

## 產生 multiply 執行檔

> 在 gem5/benchmark 資料夾底下 terminal輸入

```python
gcc --static multiply.c -o multiply
```
<img src="https://github.com/user-attachments/assets/308be316-be92-4e28-87d8-ccaa2c6cf52f" width="50%" height="auto">

## 用原始的跑為 write back

#### 用 4-way 執行
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/multiply \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=4 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```
<img src="https://github.com/user-attachments/assets/dfe548ad-d23c-40ca-82c9-3b9f0964bd22" width="50%" height="auto">

#### 執行結果

<img src="https://github.com/user-attachments/assets/52ef9f04-4bfc-43e6-9598-af316198e6f6" width="50%" height="auto">

#### gem5/m5out/stats.txt 看 log

<img src="https://github.com/user-attachments/assets/6367634c-a870-4e69-b587-0ba22c892e6f" width="50%" height="auto">

## 修改為用 write through

#### 修改 base.cc
> 在 gem5/src/mem/cache/base.cc
> else if (blk && (pkt->needsWritable() ? blk->isWritable() 內多加一段
> 如果 block 是可寫的，發出 writeclean，產生一個 writeback 封包並 push 進 writebacks。
```python
if (blk ->isWritable()){
    PacketPtr  writeCleanPkt = writecleanBlk(blk, pkt->req->getDest(), pkt->id);
    writebacks.push_back(writeCleanPkt);
}
```

<img src="https://github.com/user-attachments/assets/fe671291-6521-4cce-8b90-5a5fe419ac68" width="50%" height="auto">

#### 重新混和編譯 NVmain 和 gem5

> 在 gem5 資料夾底下 terminal 輸入

```python
scons EXTRAS=../NVmain build/X86/gem5.opt -j4   # j4 表示使用四個core加速
```

<img src="https://github.com/user-attachments/assets/8be5948f-0eba-4919-9e95-6895d2255c39" width="50%" height="auto">

#### 執行 write through 版本
> 在 gem5 資料夾底下 terminal 輸入
```python
./build/X86/gem5.opt configs/example/se.py -c benchmark/multiply \
--cpu-type=TimingSimpleCPU --caches --l2cache --l3cache \
--l1i_size=32kB --l1d_size=32kB --l2_size=128kB --l3_size=1MB --l3_assoc=4 \
--mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
```

<img src="https://github.com/user-attachments/assets/78304a6f-b89c-4069-8934-e2f2daa56c50" width="50%" height="auto">

#### 執行結果

<img src="https://github.com/user-attachments/assets/3650b82e-3f5c-4bfe-8b7c-5b91f0534f81" width="50%" height="auto">

![image](https://github.com/user-attachments/assets/6792ac55-17ef-46a0-8272-339281dc908c)


#### gem5/m5out/stats.txt 看 log

<img src="https://github.com/user-attachments/assets/73a95359-04b9-492a-9000-d3e074ed36f7" width="50%" height="auto">

