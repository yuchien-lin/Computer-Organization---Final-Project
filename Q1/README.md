# GEM5 + NVMAIN Build-Up

## 環境設置
#### 使用 Ubuntu 18.04
  
  <img src="https://github.com/user-attachments/assets/62e1494f-4df0-45be-8111-8de7ae8957ed" width="50%" height="auto">
  
#### 安裝必要套件
  
  <img src="https://github.com/user-attachments/assets/46e1b3eb-91b8-4b45-a418-1d7f33ee3e73" width="50%" height="auto">


## 下載並編譯 gem5
#### 下載 gem5  

  > (網址 : https://gem5.googlesource.com/public/gem5/+/525ce650e1a5bbe71c39d4b15598d6c003cc9f9e)
  
  > 點擊tgz下載，解壓縮後放進HOME內
  
  <img src="https://github.com/user-attachments/assets/245dca58-392c-4edf-b47e-a87f23a1b180" width="50%" height="auto">

#### 編譯gem5

  > 在 gem5 目錄底下輸入
  
   ```python
   scons build/X86/gem5.opt -j4  # j4 表示使用四個core加速
   ```
  
   <img src="https://github.com/user-attachments/assets/9ff61b86-d059-4171-8b05-754c88b9891a" width="50%" height="auto">

## 下載並編譯 NVmain
#### 下載 NVmain

  > terminal 輸入
  
   ```python
   git clone https://github.com/SEAL-UCSB/NVmain
   ```
   
  <img src="https://github.com/user-attachments/assets/4d305b8f-289a-4561-afed-0e588a1af142" width="50%" height="auto">
  
#### 將 NVMain 資料夾放在與 gem5 同一層
#### 修改 NVmain 的 SConscript

  >  將 from gem5_scons import Transform 註解掉
  
  <img src="https://github.com/user-attachments/assets/3181aabd-4ea5-47cf-838e-1d548b9702d8" width="50%" height="auto">

## 讓 gem5 能使用 NVMain

#### 修改 gem5/configs/common/Options.py

  > 在 addCommonOption 這個 function 底下多加這段

  ```python
  for arg in sys.argv:
    if arg[:9] == "--nvmain-":
        parser.add_option(arg, type="string", default="NULL", help="Set NVMain configuration value for a parameter")
  ```
   <img src="https://github.com/user-attachments/assets/8aaa862e-e8c2-438a-b6e2-1c748970c620" width="50%" height="auto">


#### 還原 NVMain 的 SConscript

  >  將 from gem5_scons import Transform 這行取消註解
  
  <img src="https://github.com/user-attachments/assets/eb49d7b0-71ec-4b2b-8bb0-5ef79a9cf127" width="50%" height="auto">
  
#### 混合編譯 gem5 + NVMain

  > 在 gem5 目錄底下輸入
  
   ```python
   scons EXTRAS=../NVmain build/X86/gem5.opt -j4 # j4 表示使用四個core加速
   ```
  
  <img src="https://github.com/user-attachments/assets/11e69e31-3326-4249-a14b-874345c52472" width="50%" height="auto">
  
## 測試執行程式 Hello World

#### 執行 Hello World 測試

  > 在 gem5 目錄下輸入

  ```python
  ./build/X86/gem5.opt configs/example/se.py -c tests/test-progs/hello/bin/x86/linux/hello \
  --cpu-type=TimingSimpleCPU --caches --l2cache \
  --mem-type=NVMainMemory --nvmain-config=../NVmain/Config/PCM_ISSCC_2012_4GB.config
  ```

#### 輸出畫面

  <img src="https://github.com/user-attachments/assets/ba9b002a-082c-4f8d-aee3-e6367477b0f7" width="50%" height="auto">

#### gem5 的 m5out 中 stat.txt

  > dcache
  
  <img src="https://github.com/user-attachments/assets/a14ca640-8234-4ba8-9d6a-14f105d57224" width="50%" height="auto">


  > icache
  
  <img src="https://github.com/user-attachments/assets/f61d241a-79e5-4c6d-812e-783e3a956513" width="50%" height="auto">

  > l2

  <img src="https://github.com/user-attachments/assets/4547bb71-bec4-453e-b6c3-46b2af83cdbf" width="50%" height="auto">
 
  
