/***************************************************************** (C) COPYRIGHT  *****************************************************************
 * File Name          : BSP_flash.h
 * Author             : lzx
 * Version            : V1.0.0
 * Date               : 2021/11/25
 * Description        : 【宏定义】  存储首地址
 *                      【全局变量】 设备类型和固件版本号 每次修改程序以写入到flash中
 *                      【函数说明】 BSP_Flash_Init          读取历史设定值，并查看是否需要写入新的设备类型和固件版本
 *                                  BSP_Flash_Read_Setting  读取设定
 *                                  BSP_Flash_Write_Setting 写入设定
 ************************************************************************************************************************************************/

#ifndef __FLASH_H
#define __FLASH_H

#include "debug.h"
#include "Timer.h"

// 系统设置基地址
#define SETTING_START_ADDR ((uint32_t)0x0800E000)

	
// 系统设置结构体
typedef struct {
		// 128字节 
		char software_version[2];				 	// 软件版本V[1].[0]
		u16  flash_write_time;						// FLASH写入次数
		char user_name[16];								// 用户姓名/呼号 
		// 上述部分使用了20个字节
		char user_speed;     	        		// 用户拍发速度：1-4档
    char user_volume;									// 用户音量
    char user_future[106];     	      // 未来版本预留
} System_Setting_Info_TypeDef;

extern System_Setting_Info_TypeDef System_Setting_Info;

extern void Update_System_Setting(void);

void BSP_Flash_Read_Info(void);
void BSP_Flash_Write_Info(void);
void Flash_Read_Setting(void);
void Flash_Write_Setting(void);
void BSP_Flash_Init(void);

#endif
