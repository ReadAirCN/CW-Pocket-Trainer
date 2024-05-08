#include "flash.h"

 
// �����嶨��
typedef union
{
	System_Setting_Info_TypeDef System_Setting_Info;
	u8 buf_u8 [128];
	u32 buf_u32 [32];
} Cvt_Info_u8_u32;



System_Setting_Info_TypeDef System_Setting_Info_default={"10",0,"BD4XUW",SendSpeed_Slow,0x36,};
System_Setting_Info_TypeDef System_Setting_Info;

//// �������������ת����ʽ����
//Cvt_Info_u8_u32 Tmp_Cvt;

u8 MakeValid_SendSpeed(u8 data)
{
	return (data>SendSpeed_Super)?SendSpeed_Slow:data ;
}

u8 MakeValid_Volume(u8 data)
{
    u8 speaker = data&0x0F;
    if (speaker > 9) {
        speaker = 6;
    }
    u8 ear_phone = data >> 4;
    if (ear_phone > 9) {
        ear_phone = 3;
    }

    return speaker + (ear_phone << 4);
}

void BSP_Flash_Read_Info(void)
{
    // ȡ��flash������
		System_Setting_Info = *(System_Setting_Info_TypeDef *)(SETTING_START_ADDR);
		// У��flash���Ƿ������ã����û�У������Ĭ������
		if (System_Setting_Info.software_version[0]!='1')
		{
			System_Setting_Info = System_Setting_Info_default;
		}
		// �������м��ز���
		GLOBAL_SendSpeed = MakeValid_SendSpeed(System_Setting_Info.user_speed);
		u8 volume = MakeValid_Volume(System_Setting_Info.user_volume);
        VolumeList_index[0] = volume&0x0F;
        VolumeList_index[1] = volume>>4;
}

void Update_System_Setting(void)
{
	// ���뵱ǰ����
	System_Setting_Info.user_speed = GLOBAL_SendSpeed;
	System_Setting_Info.user_volume = VolumeList_index[0] + (VolumeList_index[1] << 4);
}
void BSP_Flash_Write_Info(void)
{	
		Cvt_Info_u8_u32 data;
		// Ҫд��Flash�ĵ�ַ
		u32 ADDR = SETTING_START_ADDR;
			// д�������1
		System_Setting_Info.flash_write_time += 1;
		// ��ǰ���ô���data
		data.System_Setting_Info = System_Setting_Info;
		// ����ģʽ����
		FLASH_Unlock_Fast();                      
	
    FLASH_ErasePage_Fast(ADDR); // ���ٲ��� 128 �ֽ� = 32 ����
    FLASH_BufReset();                         // �建��
    FLASH_BufLoad(ADDR			 , data.buf_u32[0] , data.buf_u32[1],  data.buf_u32[2],  data.buf_u32[3]);
    FLASH_BufLoad(ADDR + 0x10, data.buf_u32[4] , data.buf_u32[5],  data.buf_u32[6],  data.buf_u32[7]);
    FLASH_BufLoad(ADDR + 0x20, data.buf_u32[8] , data.buf_u32[9],  data.buf_u32[10], data.buf_u32[11]);
    FLASH_BufLoad(ADDR + 0x30, data.buf_u32[12], data.buf_u32[13], data.buf_u32[14], data.buf_u32[15]);
    FLASH_BufLoad(ADDR + 0x40, data.buf_u32[16], data.buf_u32[17], data.buf_u32[18], data.buf_u32[19]);
    FLASH_BufLoad(ADDR + 0x50, data.buf_u32[20], data.buf_u32[21], data.buf_u32[22], data.buf_u32[23]);
    FLASH_BufLoad(ADDR + 0x60, data.buf_u32[24], data.buf_u32[25], data.buf_u32[26], data.buf_u32[27]);
    FLASH_BufLoad(ADDR + 0x70, data.buf_u32[28], data.buf_u32[29], data.buf_u32[30], data.buf_u32[31]);
    FLASH_ProgramPage_Fast(ADDR); 							// д����
		FLASH_Lock_Fast();                          // ��
}

//void BSP_Flash_Read_Setting(void)
//{
//		ScreenCvt = *(u32 *)(SETTING_START_ADDR);
//}

//void BSP_Flash_Write_Setting(void)
//{
//		Tmp_Cvt.buf_u32[0] = ScreenCvt;

//		FLASH_Unlock_Fast();                      // ����ģʽ����
//	
//    FLASH_ErasePage_Fast(SETTING_START_ADDR); // ���ٲ��� 128 �ֽ� = 32 ����
//    FLASH_BufReset();                         // �建��
//    FLASH_BufLoad(SETTING_START_ADDR, Tmp_Cvt.buf_u32[0], Tmp_Cvt.buf_u32[1], Tmp_Cvt.buf_u32[2], Tmp_Cvt.buf_u32[3]);
//    FLASH_BufLoad(SETTING_START_ADDR + 0x10, Tmp_Cvt.buf_u32[4], Tmp_Cvt.buf_u32[5], Tmp_Cvt.buf_u32[6], Tmp_Cvt.buf_u32[7]);
//    FLASH_BufLoad(SETTING_START_ADDR + 0x20, Tmp_Cvt.buf_u32[8], Tmp_Cvt.buf_u32[9], Tmp_Cvt.buf_u32[10], Tmp_Cvt.buf_u32[11]);
//    FLASH_BufLoad(SETTING_START_ADDR + 0x30, Tmp_Cvt.buf_u32[12], Tmp_Cvt.buf_u32[13], Tmp_Cvt.buf_u32[14], Tmp_Cvt.buf_u32[15]);
//    FLASH_BufLoad(SETTING_START_ADDR + 0x40, Tmp_Cvt.buf_u32[16], Tmp_Cvt.buf_u32[17], Tmp_Cvt.buf_u32[18], Tmp_Cvt.buf_u32[19]);
//    FLASH_BufLoad(SETTING_START_ADDR + 0x50, Tmp_Cvt.buf_u32[20], Tmp_Cvt.buf_u32[21], Tmp_Cvt.buf_u32[22], Tmp_Cvt.buf_u32[23]);
//    FLASH_BufLoad(SETTING_START_ADDR + 0x60, Tmp_Cvt.buf_u32[24], Tmp_Cvt.buf_u32[25], Tmp_Cvt.buf_u32[26], Tmp_Cvt.buf_u32[27]);
//    FLASH_BufLoad(SETTING_START_ADDR + 0x70, Tmp_Cvt.buf_u32[28], Tmp_Cvt.buf_u32[29], Tmp_Cvt.buf_u32[30], Tmp_Cvt.buf_u32[31]);
//    FLASH_ProgramPage_Fast(SETTING_START_ADDR); // д����
//		FLASH_Lock_Fast();                          // ��
//}

