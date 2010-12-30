/******************************************************************************
 * 
 *     (c) Copyright  2008, RealTEK Technologies Inc. All Rights Reserved.
 * 
 * Module:	HalRf6052.c	( Source C File)
 * 
 * Note:	Provide RF 6052 series relative API.	 
 *
 * Function:	
 * 		 
 * Export:	
 * 
 * Abbrev:	
 * 
 * History:
 * Data			Who		Remark
 * 
 * 09/25/2008	MHC		Create initial version.
 * 11/05/2008 	MHC		Add API for tw power setting.
 * 
 * 	
******************************************************************************/

#define _HAL_RF6052_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <rtl871x_byteorder.h>

#include <hal_init.h>

#include "Hal8192CPhyReg.h"
#include "Hal8192CPhyCfg.h"
#include "HalRf.h"



/*---------------------------Define Local Constant---------------------------*/
// Define local structure for debug!!!!!
typedef struct RF_Shadow_Compare_Map {
	// Shadow register value
	u32		Value;
	// Compare or not flag
	u8		Compare;
	// Record If it had ever modified unpredicted
	u8		ErrorOrNot;
	// Recorver Flag
	u8		Recorver;
	//
	u8		Driver_Write;
}RF_SHADOW_T;
/*---------------------------Define Local Constant---------------------------*/


/*------------------------Define global variable-----------------------------*/
/*------------------------Define global variable-----------------------------*/


/*------------------------Define local variable------------------------------*/
// 2008/11/20 MH For Debug only, RF
//static	RF_SHADOW_T	RF_Shadow[RF6052_MAX_PATH][RF6052_MAX_REG] = {0};
static	RF_SHADOW_T	RF_Shadow[RF6052_MAX_PATH][RF6052_MAX_REG];
/*------------------------Define local variable------------------------------*/


/*---------------------Define local function prototype-----------------------*/
VOID
phy_RF6052_Config_HardCode(
	IN	PADAPTER		Adapter
	);

int
phy_RF6052_Config_ParaFile(
	IN	PADAPTER		Adapter
	);
/*---------------------Define local function prototype-----------------------*/

/*------------------------Define function prototype--------------------------*/
extern	void		RF_ChangeTxPath(	IN	PADAPTER	Adapter, 
										IN	u16		DataRate);

/*------------------------Define function prototype--------------------------*/


/*------------------------Define function prototype--------------------------*/
/*-----------------------------------------------------------------------------
 * Function:	RF_ChangeTxPath
 *
 * Overview:	For RL6052, we must change some RF settign for 1T or 2T.
 *
 * Input:		u2Byte DataRate		// 0x80-8f, 0x90-9f
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 09/25/2008 	MHC		Create Version 0.
 *						Firmwaer support the utility later.
 *
 *---------------------------------------------------------------------------*/
extern	void		RF_ChangeTxPath(	IN	PADAPTER	Adapter, 
										IN	u16		DataRate)
{
// We do not support gain table change inACUT now !!!! Delete later !!!
#if 0//(RTL92SE_FPGA_VERIFY == 0)
	static	u1Byte	RF_Path_Type = 2;	// 1 = 1T 2= 2T			
	static	u4Byte	tx_gain_tbl1[6] 
			= {0x17f50, 0x11f40, 0x0cf30, 0x08720, 0x04310, 0x00100};
	static	u4Byte	tx_gain_tbl2[6] 
			= {0x15ea0, 0x10e90, 0x0c680, 0x08250, 0x04040, 0x00030};
	u1Byte	i;
	
	if (RF_Path_Type == 2 && (DataRate&0xF) <= 0x7)
	{
		// Set TX SYNC power G2G3 loop filter
		PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
					RF_TXPA_G2, bRFRegOffsetMask, 0x0f000);
		PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
					RF_TXPA_G3, bRFRegOffsetMask, 0xeacf1);

		// Change TX AGC gain table
		for (i = 0; i < 6; i++)					
			PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
						RF_TX_AGC, bRFRegOffsetMask, tx_gain_tbl1[i]);

		// Set PA to high value
		PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
					RF_TXPA_G2, bRFRegOffsetMask, 0x01e39);
	}
	else if (RF_Path_Type == 1 && (DataRate&0xF) >= 0x8)
	{
		// Set TX SYNC power G2G3 loop filter
		PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
					RF_TXPA_G2, bRFRegOffsetMask, 0x04440);
		PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
					RF_TXPA_G3, bRFRegOffsetMask, 0xea4f1);

		// Change TX AGC gain table
		for (i = 0; i < 6; i++)
			PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
						RF_TX_AGC, bRFRegOffsetMask, tx_gain_tbl2[i]);

		// Set PA low gain
		PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)RF90_PATH_A, 
					RF_TXPA_G2, bRFRegOffsetMask, 0x01e19);
	}
#endif	
	
}	/* RF_ChangeTxPath */


/*-----------------------------------------------------------------------------
 * Function:    PHY_RF6052SetBandwidth()
 *
 * Overview:    This function is called by SetBWModeCallback8190Pci() only
 *
 * Input:       PADAPTER				Adapter
 *			WIRELESS_BANDWIDTH_E	Bandwidth	//20M or 40M
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Note:		For RF type 0222D
 *---------------------------------------------------------------------------*/
VOID
PHY_RF6052SetBandwidth(
	IN	PADAPTER				Adapter,
	IN	HT_CHANNEL_WIDTH		Bandwidth)	//20M or 40M
{	
	u8			eRFPath;	
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	switch(Bandwidth)
	{
		case HT_CHANNEL_WIDTH_20:
			for(eRFPath=0;eRFPath<pHalData->NumTotalRFPath;eRFPath++)
			{
				pHalData->RfRegChnlVal[eRFPath] = ((pHalData->RfRegChnlVal[eRFPath] & 0xfffff3ff) | 0x0400);
				PHY_SetRFReg(Adapter, eRFPath, RF_CHNLBW, 0xff, pHalData->RfRegChnlVal[eRFPath]);
			}
			break;
				
		case HT_CHANNEL_WIDTH_40:
			for(eRFPath=0;eRFPath<pHalData->NumTotalRFPath;eRFPath++)
			{
				pHalData->RfRegChnlVal[eRFPath] = ((pHalData->RfRegChnlVal[eRFPath] & 0xfffff3ff));
				PHY_SetRFReg(Adapter, eRFPath, RF_CHNLBW, 0xcff, pHalData->RfRegChnlVal[eRFPath]);
			}
			break;
				
		default:
			//RT_TRACE(COMP_DBG, DBG_LOUD, ("PHY_SetRF8225Bandwidth(): unknown Bandwidth: %#X\n",Bandwidth ));
			break;			
	}
	
}


/*-----------------------------------------------------------------------------
 * Function:	PHY_RF6052SetCckTxPower
 *
 * Overview:	
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/05/2008 	MHC		Simulate 8192series..
 *
 *---------------------------------------------------------------------------*/
#define		TxHighPwrLevel_Normal		0	
#define		TxHighPwrLevel_Level1		1
#define		TxHighPwrLevel_Level2		2

extern	VOID
PHY_RF6052SetCckTxPower(
	IN	PADAPTER		Adapter,
	IN	u8*			pPowerlevel)
{
	EEPROM_EFUSE_PRIV *pEEPROM = GET_EEPROM_EFUSE_PRIV(Adapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct mlme_ext_priv *pmlmeext = &Adapter->mlmeextpriv;
	struct dm_priv *pdmpriv = &Adapter->dmpriv;
	//PMGNT_INFO		pMgntInfo=&Adapter->MgntInfo;	
	u32			TxAGC[2]={0, 0}, tmpval=0;
	BOOLEAN			TurboScanOff = _FALSE;
	u8			idx1, idx2;
	u8*			ptr;
	
	if(pEEPROM->EEPROMRegulatory != 0)
		TurboScanOff = _TRUE;

		
	if( pmlmeext->sitesurvey_res.state == _TRUE)
	{
		TxAGC[RF90_PATH_A] = 0x3f3f3f3f;
		TxAGC[RF90_PATH_B] = 0x3f3f3f3f;
		if(TurboScanOff)
		{
			for(idx1=RF90_PATH_A; idx1<=RF90_PATH_B; idx1++)
			{
				TxAGC[idx1] = 
					pPowerlevel[idx1] | (pPowerlevel[idx1]<<8) |
					(pPowerlevel[idx1]<<16) | (pPowerlevel[idx1]<<24);
			}
		}
	}
	else
	{
		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
		{	
			TxAGC[RF90_PATH_A] = 0x10101010;
			TxAGC[RF90_PATH_B] = 0x10101010;
		}
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
		{	
			TxAGC[RF90_PATH_A] = 0x00000000;
			TxAGC[RF90_PATH_B] = 0x00000000;
		}
		else
		{
			for(idx1=RF90_PATH_A; idx1<=RF90_PATH_B; idx1++)
			{
				TxAGC[idx1] = 
					pPowerlevel[idx1] | (pPowerlevel[idx1]<<8) |
					(pPowerlevel[idx1]<<16) | (pPowerlevel[idx1]<<24);
			}

			if(pEEPROM->EEPROMRegulatory==0)
			{
				tmpval = (pHalData->MCSTxPowerLevelOriginalOffset[0][6]) + 
						(pHalData->MCSTxPowerLevelOriginalOffset[0][7]<<8);
				TxAGC[RF90_PATH_A] += tmpval;
				
				tmpval = (pHalData->MCSTxPowerLevelOriginalOffset[0][14]) + 
						(pHalData->MCSTxPowerLevelOriginalOffset[0][15]<<24);
				TxAGC[RF90_PATH_B] += tmpval;
			}
		}
	}

	for(idx1=RF90_PATH_A; idx1<=RF90_PATH_B; idx1++)
	{
		ptr = (u8*)(&(TxAGC[idx1]));
		for(idx2=0; idx2<4; idx2++)
		{
			if(*ptr > RF6052_MAX_TX_PWR)
				*ptr = RF6052_MAX_TX_PWR;
			ptr++;
		}
	}

	// rf-A cck tx power
	tmpval = TxAGC[RF90_PATH_A]&0xff;
	PHY_SetBBReg(Adapter, rTxAGC_A_CCK1_Mcs32, bMaskByte1, tmpval);
	//RTPRINT(FPHY, PHY_TXPWR, ("CCK PWR 1M (rf-A) = 0x%x (reg 0x%x)\n", tmpval, rTxAGC_A_CCK1_Mcs32));
	tmpval = TxAGC[RF90_PATH_A]>>8;
	PHY_SetBBReg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, 0xffffff00, tmpval);
	//RTPRINT(FPHY, PHY_TXPWR, ("CCK PWR 2~11M (rf-A) = 0x%x (reg 0x%x)\n", tmpval, rTxAGC_B_CCK11_A_CCK2_11));

	// rf-B cck tx power
	tmpval = TxAGC[RF90_PATH_B]>>24;
	PHY_SetBBReg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte0, tmpval);
	//RTPRINT(FPHY, PHY_TXPWR, ("CCK PWR 11M (rf-B) = 0x%x (reg 0x%x)\n", tmpval, rTxAGC_B_CCK11_A_CCK2_11));
	tmpval = TxAGC[RF90_PATH_B]&0x00ffffff;
	PHY_SetBBReg(Adapter, rTxAGC_B_CCK1_55_Mcs32, 0xffffff00, tmpval);
	//RTPRINT(FPHY, PHY_TXPWR, ("CCK PWR 1~5.5M (rf-B) = 0x%x (reg 0x%x)\n", 
	//	tmpval, rTxAGC_B_CCK1_55_Mcs32));
	
}	/* PHY_RF6052SetCckTxPower */

//
// powerbase0 for OFDM rates
// powerbase1 for HT MCS rates
//
void getPowerBase(
	IN	PADAPTER	Adapter,
	IN	u8*		pPowerLevel,
	IN	u8		Channel,
	IN OUT u32*	OfdmBase,
	IN OUT u32*	MCSBase
	)
{
	EEPROM_EFUSE_PRIV *pEEPROM = GET_EEPROM_EFUSE_PRIV(Adapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32			powerBase0, powerBase1;
	u8			Legacy_pwrdiff=0, HT20_pwrdiff=0;
	u8			i, powerlevel[2];

	for(i=0; i<2; i++)
	{
		powerlevel[i] = pPowerLevel[i];
		Legacy_pwrdiff = pEEPROM->TxPwrLegacyHtDiff[i][Channel-1];			
		powerBase0 = powerlevel[i] + Legacy_pwrdiff; 

		powerBase0 = (powerBase0<<24) | (powerBase0<<16) |(powerBase0<<8) |powerBase0;
		*(OfdmBase+i) = powerBase0;
		//RTPRINT(FPHY, PHY_TXPWR, (" [OFDM power base index rf(%c) = 0x%x]\n", ((i==0)?'A':'B'), *(OfdmBase+i)));
	}

	for(i=0; i<2; i++)
	{
		//Check HT20 to HT40 diff
		if(pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
		{
			HT20_pwrdiff = pEEPROM->TxPwrHt20Diff[i][Channel-1];
			powerlevel[i] += HT20_pwrdiff;
		}
		powerBase1 = powerlevel[i];
		powerBase1 = (powerBase1<<24) | (powerBase1<<16) |(powerBase1<<8) |powerBase1;
		*(MCSBase+i) = powerBase1;
		//RTPRINT(FPHY, PHY_TXPWR, (" [MCS power base index rf(%c) = 0x%x]\n", ((i==0)?'A':'B'), *(MCSBase+i)));
	}
}

void getTxPowerWriteValByRegulatory(
	IN		PADAPTER	Adapter,
	IN		u8		Channel,
	IN		u8		index,
	IN		u32*		powerBase0,
	IN		u32*		powerBase1,
	OUT		u32*		pOutWriteVal
	)
{
	EEPROM_EFUSE_PRIV *pEEPROM = GET_EEPROM_EFUSE_PRIV(Adapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv *pdmpriv = &Adapter->dmpriv;
	u8	i, chnlGroup, pwr_diff_limit[CHANNEL_GROUP_MAX+1];
	u32 	writeVal, customer_limit, rf;
	
	//
	// Index 0 & 1= legacy OFDM, 2-5=HT_MCS rate
	//
	for(rf=0; rf<2; rf++)
	{
		switch(pEEPROM->EEPROMRegulatory)
		{
			case 0:	// Realtek better performance
					// increase power diff defined by Realtek for large power
				chnlGroup = 0;
				//RTPRINT(FPHY, PHY_TXPWR, ("MCSTxPowerLevelOriginalOffset[%d][%d] = 0x%x\n", 
				//	chnlGroup, index, pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)]));
				writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] + 
					((index<2)?powerBase0[rf]:powerBase1[rf]);
				//RTPRINT(FPHY, PHY_TXPWR, ("RTK better performance, writeVal(%c) = 0x%x\n", ((rf==0)?'A':'B'), writeVal));
				break;
			case 1:	// Realtek regulatory
					// increase power diff defined by Realtek for regulatory
				if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_40)
				{
					writeVal = ((index<2)?powerBase0[rf]:powerBase1[rf]);
					//RTPRINT(FPHY, PHY_TXPWR, ("Realtek regulatory, 40MHz, writeVal(%c) = 0x%x\n", ((rf==0)?'A':'B'), writeVal));
				}
				else
				{
					if(pHalData->pwrGroupCnt == 1)
						chnlGroup = 0;
					if(pHalData->pwrGroupCnt >= CHANNEL_GROUP_MAX)
					{
						chnlGroup = getChnlGroupfromArray(Channel-1);
						if(pHalData->pwrGroupCnt == CHANNEL_GROUP_MAX+1)
							chnlGroup++;
					}
					//RTPRINT(FPHY, PHY_TXPWR, ("MCSTxPowerLevelOriginalOffset[%d][%d] = 0x%x\n", 
					//chnlGroup, index, pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)]));
					writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] + 
							((index<2)?powerBase0[rf]:powerBase1[rf]);
					//RTPRINT(FPHY, PHY_TXPWR, ("Realtek regulatory, 20MHz, writeVal(%c) = 0x%x\n", ((rf==0)?'A':'B'), writeVal));
				}
				break;
			case 2:	// Better regulatory
					// don't increase any power diff
				writeVal = ((index<2)?powerBase0[rf]:powerBase1[rf]);
				//RTPRINT(FPHY, PHY_TXPWR, ("Better regulatory, writeVal(%c) = 0x%x\n", ((rf==0)?'A':'B'), writeVal));
				break;
			case 3:	// Customer defined power diff.
					// increase power diff defined by customer.
				chnlGroup = 0;
				//RTPRINT(FPHY, PHY_TXPWR, ("MCSTxPowerLevelOriginalOffset[%d][%d] = 0x%x\n", 
				//	chnlGroup, index, pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)]));

				if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_40)
				{
					//RTPRINT(FPHY, PHY_TXPWR, ("customer's limit, 40MHz rf(%c) = 0x%x\n", 
					//	((rf==0)?'A':'B'), pHalData->PwrGroupHT40[rf][Channel-1]));
				}
				else
				{
					//RTPRINT(FPHY, PHY_TXPWR, ("customer's limit, 20MHz rf(%c) = 0x%x\n", 
					//	((rf==0)?'A':'B'), pHalData->PwrGroupHT20[rf][Channel-1]));
				}
				for (i=0; i<(CHANNEL_GROUP_MAX+1); i++)
				{
					pwr_diff_limit[i] = (u8)((pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)]&(0x7f<<(i*8)))>>(i*8));
					if (pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_40)
					{
						if(pwr_diff_limit[i] > pEEPROM->PwrGroupHT40[rf][Channel-1])
							pwr_diff_limit[i] = pEEPROM->PwrGroupHT40[rf][Channel-1];
					}
					else
					{
						if(pwr_diff_limit[i] > pEEPROM->PwrGroupHT20[rf][Channel-1])
							pwr_diff_limit[i] = pEEPROM->PwrGroupHT20[rf][Channel-1];
					}
				}
				customer_limit = (pwr_diff_limit[3]<<24) | (pwr_diff_limit[2]<<16) |
								(pwr_diff_limit[1]<<8) | (pwr_diff_limit[0]);
				//RTPRINT(FPHY, PHY_TXPWR, ("Customer's limit rf(%c) = 0x%x\n", ((rf==0)?'A':'B'), customer_limit));

				writeVal = customer_limit + ((index<2)?powerBase0[rf]:powerBase1[rf]);
				//RTPRINT(FPHY, PHY_TXPWR, ("Customer, writeVal rf(%c)= 0x%x\n", ((rf==0)?'A':'B'), writeVal));
				break;
			default:
				chnlGroup = 0;
				writeVal = pHalData->MCSTxPowerLevelOriginalOffset[chnlGroup][index+(rf?8:0)] + 
						((index<2)?powerBase0[rf]:powerBase1[rf]);
				//RTPRINT(FPHY, PHY_TXPWR, ("RTK better performance, writeVal rf(%c) = 0x%x\n", ((rf==0)?'A':'B'), writeVal));
				break;
		}
#if 1
		if(!IS_NORMAL_CHIP(pHalData->VersionID))
		{
			if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
				writeVal = 0x12121212;
			else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
				writeVal = 0x12121212;
		}
#endif

		*(pOutWriteVal+rf) = writeVal;
	}
}

void writeOFDMPowerReg(
	IN		PADAPTER	Adapter,
	IN		u8		index,
	IN 		u32*		pValue
	)
{
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u16 RegOffset_A[6] = {	rTxAGC_A_Rate18_06, rTxAGC_A_Rate54_24, 
							rTxAGC_A_Mcs03_Mcs00, rTxAGC_A_Mcs07_Mcs04, 
							rTxAGC_A_Mcs11_Mcs08, rTxAGC_A_Mcs15_Mcs12};
	u16 RegOffset_B[6] = {	rTxAGC_B_Rate18_06, rTxAGC_B_Rate54_24, 
							rTxAGC_B_Mcs03_Mcs00, rTxAGC_B_Mcs07_Mcs04,
							rTxAGC_B_Mcs11_Mcs08, rTxAGC_B_Mcs15_Mcs12};
	u8 i, rf, pwr_val[4];
	u8 rfa_lower_bound, rfa_upper_bound, rf_pwr_diff;
	u32 writeVal;
	u16 RegOffset;

	for(rf=0; rf<2; rf++)
	{
		writeVal = pValue[rf];
		for(i=0; i<4; i++)
		{
			pwr_val[i] = (u8)((writeVal & (0x7f<<(i*8)))>>(i*8));
			if (pwr_val[i]  > RF6052_MAX_TX_PWR)
				pwr_val[i]  = RF6052_MAX_TX_PWR;
		}
		writeVal = (pwr_val[3]<<24) | (pwr_val[2]<<16) |(pwr_val[1]<<8) |pwr_val[0];

		if(rf == 0)
			RegOffset = RegOffset_A[index];
		else
			RegOffset = RegOffset_B[index];
		
		PHY_SetBBReg(Adapter, RegOffset, bMaskDWord, writeVal);
		//RTPRINT(FPHY, PHY_TXPWR, ("Set 0x%x = %08x\n", RegOffset, writeVal));	
	}
}
/*-----------------------------------------------------------------------------
 * Function:	PHY_RF6052SetOFDMTxPower
 *
 * Overview:	For legacy and HY OFDM, we must read EEPROM TX power index for 
 *			different channel and read original value in TX power register area from
 *			0xe00. We increase offset and original value to be correct tx pwr.
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/05/2008 	MHC		Simulate 8192 series method.
 * 01/06/2009	MHC		1. Prevent Path B tx power overflow or underflow dure to
 *						A/B pwr difference or legacy/HT pwr diff.
 *						2. We concern with path B legacy/HT OFDM difference.
 * 01/22/2009	MHC		Support new EPRO format from SD3.
 *
 *---------------------------------------------------------------------------*/
extern	VOID 
PHY_RF6052SetOFDMTxPower(
	IN	PADAPTER	Adapter,
	IN	u8*		pPowerLevel,
	IN	u8		Channel)
{
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32 writeVal[2], powerBase0[2], powerBase1[2];
	u8 index = 0;
	u8 i;


	getPowerBase(Adapter, pPowerLevel, Channel, &powerBase0[0], &powerBase1[0]);

	for(index=0; index<6; index++)
	{
		getTxPowerWriteValByRegulatory(Adapter, Channel, index, 
			&powerBase0[0], &powerBase1[0], &writeVal[0]);

		writeOFDMPowerReg(Adapter, index, &writeVal[0]);
	}
	
}


int
PHY_RF6052_Config(
	IN	PADAPTER		Adapter)
{
	HAL_DATA_TYPE				*pHalData = GET_HAL_DATA(Adapter);
	int					rtStatus = _SUCCESS;	
	
	//
	// Initialize general global value
	//
	// TODO: Extend RF_PATH_C and RF_PATH_D in the future
	if(pHalData->rf_type == RF_1T1R)
		pHalData->NumTotalRFPath = 1;
	else
		pHalData->NumTotalRFPath = 2;

	//
	// Config BB and RF
	//
	rtStatus = phy_RF6052_Config_ParaFile(Adapter);
#if 0	
	switch( Adapter->MgntInfo.bRegHwParaFile )
	{
		case 0:
			phy_RF6052_Config_HardCode(Adapter);
			break;

		case 1:
			rtStatus = phy_RF6052_Config_ParaFile(Adapter);
			break;

		case 2:
			// Partial Modify. 
			phy_RF6052_Config_HardCode(Adapter);
			phy_RF6052_Config_ParaFile(Adapter);
			break;

		default:
			phy_RF6052_Config_HardCode(Adapter);
			break;
	}
#endif	
	return rtStatus;
		
}

VOID
phy_RF6052_Config_HardCode(
	IN	PADAPTER		Adapter
	)
{
	
	// Set Default Bandwidth to 20M
	//Adapter->HalFunc	.SetBWModeHandler(Adapter, HT_CHANNEL_WIDTH_20);

	// TODO: Set Default Channel to channel one for RTL8225
	
}

int
phy_RF6052_Config_ParaFile(
	IN	PADAPTER		Adapter
	)
{
	u32					u4RegValue;
	u8					eRFPath;		
	BB_REGISTER_DEFINITION_T	*pPhyReg;	

	int				rtStatus = _SUCCESS;
	HAL_DATA_TYPE			*pHalData = GET_HAL_DATA(Adapter);
	static char				sz88CRadioAFile[] = RTL8188C_PHY_RADIO_A;	
	static char				sz88CRadioBFile[] = RTL8188C_PHY_RADIO_B;
	static char				sz92CRadioAFile[] = RTL8192C_PHY_RADIO_A;
	static char				sz92CRadioBFile[] = RTL8192C_PHY_RADIO_B;
	static char				sz88CTestRadioAFile[] = RTL8188C_PHY_RADIO_A;	
	static char				sz88CTestRadioBFile[] = RTL8188C_PHY_RADIO_B;
	static char				sz92CTestRadioAFile[] = RTL8192C_PHY_RADIO_A;	
	static char				sz92CTestRadioBFile[] = RTL8192C_PHY_RADIO_B;

	static char				sz92DRadioAFile[] = RTL8192D_PHY_RADIO_A;	
	static char				sz92DRadioBFile[] = RTL8192D_PHY_RADIO_B;
	static char				sz92DTestRadioAFile[] = RTL8192D_PHY_RADIO_A;	
	static char				sz92DTestRadioBFile[] = RTL8192D_PHY_RADIO_B;
	
	u8					*pszRadioAFile, *pszRadioBFile;
	u8			u1bTmp;
	BOOLEAN		bMac1NeedInitRadioAFirst = _FALSE;	
	BOOLEAN		bNeedPowerDownRadioA = _FALSE;

      // 92D RF config  zhiyuan 2010/04/07
      // Single phy mode: use radio_a radio_b config path_A path_B seperately by MAC0, and MAC1 needn't configure RF;
      // Dual PHY mode:MAC0 use radio_a config 1st phy path_A, MAC1 use radio_b config 2nd PHY path_A.
	if(IS_HARDWARE_TYPE_8192D(pHalData)){
		if(IS_NORMAL_CHIP(pHalData->VersionID))
		{
			pszRadioAFile = sz92DRadioAFile;
			pszRadioBFile = sz92DRadioBFile;

			if(pHalData->interfaceIndex==1)
			{
				if(pHalData->MacPhyMode92D==DUALMAC_DUALPHY)
					pszRadioAFile = sz92DRadioBFile;
				else
					return rtStatus;
			}
		}
		else
		{
			pszRadioAFile = sz92DTestRadioAFile;
			pszRadioBFile = sz92DTestRadioBFile;
			if(pHalData->interfaceIndex==1)
			{
				//
				// when 92D test chip dual mac dual phy mode, if enable MAC1 first, before init RF radio B,
				// also init RF radio A, and then let radio A go to power down mode.
				// Note: normal chip need do this or not will be considerred later.
				//
				if(pHalData->MacPhyMode92D==DUALMAC_DUALPHY)
				{
					u1bTmp = read8(Adapter, REG_MAC0);

					if (!(u1bTmp&MAC0_ON))
					{
						// MAC0 not enabled, also init radio A before init radio B.

						// Enable BB and RF
#if (DEV_BUS_TYPE == PCI_INTERFACE)                                
						//PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0xE0);
#if  0
						MpWritePCIDwordDBI8192C(Adapter, 
											(REG_SYS_FUNC_EN - 2), 
											MpReadPCIDwordDBI8192C(Adapter, (REG_SYS_FUNC_EN - 2), BIT3)&0xFFFCFFFF,
											BIT3);
#endif
						//u2bTmp = PlatformEFIORead2Byte(Adapter, REG_SYS_FUNC_EN);
						//PlatformEFIOWrite2Byte(Adapter, REG_SYS_FUNC_EN, u2bTmp|BIT13|BIT0|BIT1);     
						MpWritePCIDwordDBI8192C(Adapter, 
											(REG_SYS_FUNC_EN - 2), 
											MpReadPCIDwordDBI8192C(Adapter, (REG_SYS_FUNC_EN - 2), BIT3)|BIT29|BIT16|BIT17,
											BIT3);
#elif (DEV_BUS_TYPE == USB_INTERFACE)
						pHalData->bDuringMac1InitRadioA = _TRUE;
						write16(Adapter, REG_SYS_FUNC_EN, read16(Adapter, REG_SYS_FUNC_EN)&0xFFFC); 
						write16(Adapter, REG_SYS_FUNC_EN, read16(Adapter, REG_SYS_FUNC_EN)|BIT13|BIT0|BIT1); 
						pHalData->bDuringMac1InitRadioA = _FALSE;
#endif
                                
						pHalData->NumTotalRFPath = 2;
						bMac1NeedInitRadioAFirst = _TRUE;
					}
					else
					{
						// MAC0 enabled, only init radia B.
						pszRadioAFile = sz92DTestRadioBFile;
					}
				}
				else
				{
					return rtStatus;
				}
			}
		}
	}
	else{
		if(IS_92C_SERIAL( pHalData->VersionID))// 88c's IPA  is different from 92c's
		{
			if(IS_NORMAL_CHIP(pHalData->VersionID))
			{
				pszRadioAFile = sz92CRadioAFile;
				pszRadioBFile = sz92CRadioBFile;
			}
			else
			{
				pszRadioAFile = sz92CTestRadioAFile;
				pszRadioBFile = sz92CTestRadioBFile;
			}
		}
		else
		{
			if(IS_NORMAL_CHIP(pHalData->VersionID))
			{
				pszRadioAFile = sz88CRadioAFile;
				pszRadioBFile = sz88CRadioBFile;
			}
			else
			{
				pszRadioAFile = sz88CTestRadioAFile;
				pszRadioBFile = sz88CTestRadioBFile;
			}
		}
	}

	//3//-----------------------------------------------------------------
	//3// <2> Initialize RF
	//3//-----------------------------------------------------------------
	//for(eRFPath = RF90_PATH_A; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
	for(eRFPath = 0; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
	{
		if (IS_HARDWARE_TYPE_8192D(pHalData) && bMac1NeedInitRadioAFirst)
		{
			if (eRFPath == RF90_PATH_A)
			{
				pHalData->bDuringMac1InitRadioA = _TRUE;
				bNeedPowerDownRadioA = _TRUE;
			}

			if (eRFPath == RF90_PATH_B)
			{
				pHalData->bDuringMac1InitRadioA = _FALSE;
				bMac1NeedInitRadioAFirst = _FALSE;
				eRFPath = RF90_PATH_A;
				pszRadioAFile = sz92DTestRadioBFile;
				pHalData->NumTotalRFPath = 1;                    
			}
		}

		pPhyReg = &pHalData->PHYRegDef[eRFPath];
		
		/*----Store original RFENV control type----*/		
		switch(eRFPath)
		{
		case RF90_PATH_A:
		case RF90_PATH_C:
			u4RegValue = PHY_QueryBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV);
			break;
		case RF90_PATH_B :
		case RF90_PATH_D:
			u4RegValue = PHY_QueryBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV<<16);
			break;
		}

		/*----Set RF_ENV enable----*/		
		PHY_SetBBReg(Adapter, pPhyReg->rfintfe, bRFSI_RFENV<<16, 0x1);
		udelay_os(1);//PlatformStallExecution(1);
		
		/*----Set RF_ENV output high----*/
		PHY_SetBBReg(Adapter, pPhyReg->rfintfo, bRFSI_RFENV, 0x1);
		udelay_os(1);//PlatformStallExecution(1);

		/* Set bit number of Address and Data for RF register */
		PHY_SetBBReg(Adapter, pPhyReg->rfHSSIPara2, b3WireAddressLength, 0x0); 	// Set 1 to 4 bits for 8255
		udelay_os(1);//PlatformStallExecution(1);

		PHY_SetBBReg(Adapter, pPhyReg->rfHSSIPara2, b3WireDataLength, 0x0);	// Set 0 to 12  bits for 8255
		udelay_os(1);//PlatformStallExecution(1);

		/*----Initialize RF fom connfiguration file----*/
		switch(eRFPath)
		{
		case RF90_PATH_A:
#ifdef CONFIG_EMBEDDED_FWIMG
			rtStatus= PHY_ConfigRFWithHeaderFile(Adapter,(RF90_RADIO_PATH_E)eRFPath);
#else
			rtStatus = PHY_ConfigRFWithParaFile(Adapter, pszRadioAFile, (RF90_RADIO_PATH_E)eRFPath);
#endif
			break;
		case RF90_PATH_B:
#ifdef CONFIG_EMBEDDED_FWIMG
			rtStatus= PHY_ConfigRFWithHeaderFile(Adapter,(RF90_RADIO_PATH_E)eRFPath);
#else			
			rtStatus = PHY_ConfigRFWithParaFile(Adapter, pszRadioBFile, (RF90_RADIO_PATH_E)eRFPath);
#endif
			break;
		case RF90_PATH_C:
			break;
		case RF90_PATH_D:
			break;
		}

		/*----Restore RFENV control type----*/;
		switch(eRFPath)
		{
		case RF90_PATH_A:
		case RF90_PATH_C:
			PHY_SetBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV, u4RegValue);
			break;
		case RF90_PATH_B :
		case RF90_PATH_D:
			PHY_SetBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV<<16, u4RegValue);
			break;
		}

		if(rtStatus != _SUCCESS){
			//RT_TRACE(COMP_FPGA, DBG_LOUD, ("phy_RF6052_Config_ParaFile():Radio[%d] Fail!!", eRFPath));
			goto phy_RF6052_Config_ParaFile_Fail;
		}

	}

	if (IS_HARDWARE_TYPE_8192D(pHalData) && bNeedPowerDownRadioA)
	{
		// check MAC0 enable or not again now, if enabled, not power down radio A.
		u1bTmp = read8(Adapter, REG_MAC0);

		if (!(u1bTmp&MAC0_ON))
		{
			// power down RF radio A according to YuNan's advice.
#if (DEV_BUS_TYPE == PCI_INTERFACE) 	        
			MpWritePCIDwordDBI8192C(Adapter, 
   						rFPGA0_XA_LSSIParameter, 
   						0x00000000,
   						BIT3);
#elif (DEV_BUS_TYPE == USB_INTERFACE)
			pHalData->bDuringMac1InitRadioA = _TRUE;
			write32(Adapter, rFPGA0_XA_LSSIParameter, 0x00000000); 
			pHalData->bDuringMac1InitRadioA = _FALSE;
#endif
		}
		bNeedPowerDownRadioA = _FALSE;
      }

	//RT_TRACE(COMP_INIT, DBG_LOUD, ("<---phy_RF6052_Config_ParaFile()\n"));
	return rtStatus;
	
phy_RF6052_Config_ParaFile_Fail:	
	return rtStatus;
}


//
// ==> RF shadow Operation API Code Section!!!
//
/*-----------------------------------------------------------------------------
 * Function:	PHY_RFShadowRead
 *				PHY_RFShadowWrite
 *				PHY_RFShadowCompare
 *				PHY_RFShadowRecorver
 *				PHY_RFShadowCompareAll
 *				PHY_RFShadowRecorverAll
 *				PHY_RFShadowCompareFlagSet
 *				PHY_RFShadowRecorverFlagSet
 *
 * Overview:	When we set RF register, we must write shadow at first.
 *			When we are running, we must compare shadow abd locate error addr.
 *			Decide to recorver or not.
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/20/2008 	MHC		Create Version 0.
 *
 *---------------------------------------------------------------------------*/
extern	u32
PHY_RFShadowRead(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset)
{
	return	RF_Shadow[eRFPath][Offset].Value;
	
}	/* PHY_RFShadowRead */


extern	VOID
PHY_RFShadowWrite(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u32				Data)
{
	RF_Shadow[eRFPath][Offset].Value = (Data & bRFRegOffsetMask);
	RF_Shadow[eRFPath][Offset].Driver_Write = _TRUE;
	
}	/* PHY_RFShadowWrite */


extern	BOOLEAN
PHY_RFShadowCompare(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset)
{
	u32	reg;
	// Check if we need to check the register
	if (RF_Shadow[eRFPath][Offset].Compare == _TRUE)
	{
		reg = PHY_QueryRFReg(Adapter, eRFPath, Offset, bRFRegOffsetMask);
		// Compare shadow and real rf register for 20bits!!
		if (RF_Shadow[eRFPath][Offset].Value != reg)
		{
			// Locate error position.
			RF_Shadow[eRFPath][Offset].ErrorOrNot = _TRUE;
			//RT_TRACE(COMP_INIT, DBG_LOUD, 
			//("PHY_RFShadowCompare RF-%d Addr%02lx Err = %05lx\n", 
			//eRFPath, Offset, reg));
		}
		return RF_Shadow[eRFPath][Offset].ErrorOrNot ;
	}
	return _FALSE;
}	/* PHY_RFShadowCompare */


extern	VOID
PHY_RFShadowRecorver(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset)
{
	// Check if the address is error
	if (RF_Shadow[eRFPath][Offset].ErrorOrNot == _TRUE)
	{
		// Check if we need to recorver the register.
		if (RF_Shadow[eRFPath][Offset].Recorver == _TRUE)
		{
			PHY_SetRFReg(Adapter, eRFPath, Offset, bRFRegOffsetMask, 
							RF_Shadow[eRFPath][Offset].Value);
			//RT_TRACE(COMP_INIT, DBG_LOUD, 
			//("PHY_RFShadowRecorver RF-%d Addr%02lx=%05lx", 
			//eRFPath, Offset, RF_Shadow[eRFPath][Offset].Value));
		}
	}
	
}	/* PHY_RFShadowRecorver */


extern	VOID
PHY_RFShadowCompareAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			PHY_RFShadowCompare(Adapter, (RF90_RADIO_PATH_E)eRFPath, Offset);
		}
	}
	
}	/* PHY_RFShadowCompareAll */


extern	VOID
PHY_RFShadowRecorverAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			PHY_RFShadowRecorver(Adapter, (RF90_RADIO_PATH_E)eRFPath, Offset);
		}
	}
	
}	/* PHY_RFShadowRecorverAll */


extern	VOID
PHY_RFShadowCompareFlagSet(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u8				Type)
{
	// Set True or False!!!
	RF_Shadow[eRFPath][Offset].Compare = Type;
		
}	/* PHY_RFShadowCompareFlagSet */


extern	VOID
PHY_RFShadowRecorverFlagSet(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u8				Type)
{
	// Set True or False!!!
	RF_Shadow[eRFPath][Offset].Recorver= Type;
		
}	/* PHY_RFShadowRecorverFlagSet */


extern	VOID
PHY_RFShadowCompareFlagSetAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			// 2008/11/20 MH For S3S4 test, we only check reg 26/27 now!!!!
			if (Offset != 0x26 && Offset != 0x27)
				PHY_RFShadowCompareFlagSet(Adapter, (RF90_RADIO_PATH_E)eRFPath, Offset, _FALSE);
			else
				PHY_RFShadowCompareFlagSet(Adapter, (RF90_RADIO_PATH_E)eRFPath, Offset, _TRUE);
		}
	}
		
}	/* PHY_RFShadowCompareFlagSetAll */


extern	VOID
PHY_RFShadowRecorverFlagSetAll(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			// 2008/11/20 MH For S3S4 test, we only check reg 26/27 now!!!!
			if (Offset != 0x26 && Offset != 0x27)
				PHY_RFShadowRecorverFlagSet(Adapter, (RF90_RADIO_PATH_E)eRFPath, Offset, _FALSE);
			else
				PHY_RFShadowRecorverFlagSet(Adapter, (RF90_RADIO_PATH_E)eRFPath, Offset, _TRUE);
		}
	}
		
}	/* PHY_RFShadowCompareFlagSetAll */

extern	VOID
PHY_RFShadowRefresh(
	IN	PADAPTER			Adapter)
{
	u32		eRFPath;
	u32		Offset;

	for (eRFPath = 0; eRFPath < RF6052_MAX_PATH; eRFPath++)
	{
		for (Offset = 0; Offset <= RF6052_MAX_REG; Offset++)
		{
			RF_Shadow[eRFPath][Offset].Value = 0;
			RF_Shadow[eRFPath][Offset].Compare = _FALSE;
			RF_Shadow[eRFPath][Offset].Recorver  = _FALSE;
			RF_Shadow[eRFPath][Offset].ErrorOrNot = _FALSE;
			RF_Shadow[eRFPath][Offset].Driver_Write = _FALSE;
		}
	}
	
}	/* PHY_RFShadowRead */

/* End of HalRf6052.c */

