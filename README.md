# 便携CW训练器 CW-Pocker-Trainer

## 仓库简介
  - **3D-Models**: 包含可编辑和打印的STEP模型文件。PLA文件夹内的模型建议使用PLA或其他刚性耗材打印，TPU文件夹内的模型建议使用TPU 95A或其他柔性耗材打印。
  - **Firmware**: 包含可以直接下载的固件，所有固件均可使用WCHISPTool下载， WCHISPTool的安装包也在此文件夹中，CH549G-WCHLINK-Firmware是调试芯片的固件，只需在第一次烧录即可（对于从工房买到焊好的PCB板,此固件已经烧录完成），CH32F103C8T6-Main-Firmware是主控芯片的固件，下载不同固件可以实现不同的功能，每个固件具体的介绍在该文件夹中。
  - **Keil-Project**: CW训练器的源码工程,Keil中设置调试器为CMSIS-DAP Debugger可直接编译下载到训练器。
  - 具体介绍参考项目中的 “便携CW训练器使用说明.pdf”

本项目完全开源，欢迎贡献代码。
## 交流群
QQ交流群：798617844   进群答案为：**ReadAir**

![alt text](CWPT.png)
