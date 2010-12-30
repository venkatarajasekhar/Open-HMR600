#ifndef __INC_HAL8192SE_FW_IMG_H
#define __INC_HAL8192SE_FW_IMG_H

/*Created on  2009/ 4/14, 12:27*/

#include <linux/types.h>
#define ImgArrayLength 70736
extern u8 Rtl8192SEFwImgArray[ImgArrayLength];
#define MainArrayLength 1
extern u8 Rtl8192SEFwMainArray[MainArrayLength];
#define DataArrayLength 1
extern u8 Rtl8192SEFwDataArray[DataArrayLength];
#define PHY_REG_2T2RArrayLength 372
extern u32 Rtl8192SEPHY_REG_2T2RArray[PHY_REG_2T2RArrayLength];
#define PHY_REG_1T2RArrayLength 1
extern u32 Rtl8192SEPHY_REG_1T2RArray[PHY_REG_1T2RArrayLength];
#define PHY_ChangeTo_1T1RArrayLength 48
extern u32 Rtl8192SEPHY_ChangeTo_1T1RArray[PHY_ChangeTo_1T1RArrayLength];
#define PHY_ChangeTo_1T2RArrayLength 45
extern u32 Rtl8192SEPHY_ChangeTo_1T2RArray[PHY_ChangeTo_1T2RArrayLength];
#define PHY_ChangeTo_2T2RArrayLength 45
extern u32 Rtl8192SEPHY_ChangeTo_2T2RArray[PHY_ChangeTo_2T2RArrayLength];
#define PHY_REG_Array_PGLength 84
extern u32 Rtl8192SEPHY_REG_Array_PG[PHY_REG_Array_PGLength];
#define RadioA_1T_ArrayLength 202
extern u32 Rtl8192SERadioA_1T_Array[RadioA_1T_ArrayLength];
#define RadioB_ArrayLength 22
extern u32 Rtl8192SERadioB_Array[RadioB_ArrayLength];
#define RadioA_to1T_ArrayLength 196
extern u32 Rtl8192SERadioA_to1T_Array[RadioA_to1T_ArrayLength];
#define RadioA_to2T_ArrayLength 196
extern u32 Rtl8192SERadioA_to2T_Array[RadioA_to2T_ArrayLength];
#define RadioB_GM_ArrayLength 10
extern u32 Rtl8192SERadioB_GM_Array[RadioB_GM_ArrayLength];
#define MAC_2T_ArrayLength 106
extern u32 Rtl8192SEMAC_2T_Array[MAC_2T_ArrayLength];
#define MACPHY_Array_PGLength 1
extern u32 Rtl8192SEMACPHY_Array_PG[MACPHY_Array_PGLength];
#define AGCTAB_ArrayLength 320
extern u32 Rtl8192SEAGCTAB_Array[AGCTAB_ArrayLength];

#endif //__INC_HAL8192SE_FW_IMG_H
