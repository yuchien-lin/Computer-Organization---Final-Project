# GEM5 + NVMAIN Build-Up

## 環境設置
- 使用 Ubuntu 18.04
  
  <img src="https://github.com/user-attachments/assets/62e1494f-4df0-45be-8111-8de7ae8957ed" width="50%" height="auto">
- 安裝必要套件
  
  <img src="https://github.com/user-attachments/assets/46e1b3eb-91b8-4b45-a418-1d7f33ee3e73" width="50%" height="auto">


## 下載並編譯 gem5
- 下載 gem5  

  (網址 : https://gem5.googlesource.com/public/gem5/+/525ce650e1a5bbe71c39d4b15598d6c003cc9f9e)
  
  點擊tgz下載，解壓縮後放進HOME內
  
  <img src="https://github.com/user-attachments/assets/245dca58-392c-4edf-b47e-a87f23a1b180" width="50%" height="auto">

- 進入 GEM5 資料夾後編譯

  輸入 : scons build/X86/gem5.opt -j4  # j4 表示使用四個core加速

   <img src="https://github.com/user-attachments/assets/9ff61b86-d059-4171-8b05-754c88b9891a" width="50%" height="auto">

## 下載並編譯 NVmain
- 下載 NVmain

  terminal 輸入 : git clone https://github.com/SEAL-UCSB/NVmain

  <img src="https://github.com/user-attachments/assets/4d305b8f-289a-4561-afed-0e588a1af142" width="50%" height="auto">
  
- 將 NVMain 資料夾放在與 gem5 同一層
- 修改 NVmain 的 SConscript

  將 from gem5_scons import Transform 註解掉
  
  <img src="https://github.com/user-attachments/assets/3181aabd-4ea5-47cf-838e-1d548b9702d8" width="50%" height="auto">

  
