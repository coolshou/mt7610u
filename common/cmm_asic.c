/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	cmm_asic.c

	Abstract:
	Functions used to communicate with ASIC

*/

#include "rt_config.h"

#ifdef CONFIG_STA_SUPPORT
void AsicUpdateAutoFallBackTable(PRTMP_ADAPTER pAd, PUCHAR pRateTable)
{
	UCHAR i;
	HT_FBK_CFG0_STRUC HtCfg0;
	HT_FBK_CFG1_STRUC HtCfg1;
	LG_FBK_CFG0_STRUC LgCfg0;
	LG_FBK_CFG1_STRUC LgCfg1;
#ifdef DOT11N_SS3_SUPPORT
	TX_FBK_CFG_3S_0_STRUC Ht3SSCfg0;
	TX_FBK_CFG_3S_1_STRUC Ht3SSCfg1;
#endif /* DOT11N_SS3_SUPPORT */
	RTMP_RA_LEGACY_TB *pCurrTxRate, *pNextTxRate;

#ifdef AGS_SUPPORT
	RTMP_RA_AGS_TB *pCurrTxRate_AGS, *pNextTxRate_AGS;
	BOOLEAN bUseAGS = FALSE;

	if (AGS_IS_USING(pAd, pRateTable)) {
		DBGPRINT(RT_DEBUG_TRACE, ("%s: Use AGS\n", __FUNCTION__));

		bUseAGS = TRUE;

		Ht3SSCfg0.word = 0x1211100f;
		Ht3SSCfg1.word = 0x16151413;
	}
#endif /* AGS_SUPPORT */

#ifdef DOT11N_SS3_SUPPORT
	if (IS_RT3883(pAd)) {
		Ht3SSCfg0.word = 0x12111008;
		Ht3SSCfg1.word = 0x16151413;
	}
#endif /* DOT11N_SS3_SUPPORT */

	/* set to initial value*/
	HtCfg0.word = 0x65432100;
	HtCfg1.word = 0xedcba980;
	LgCfg0.word = 0xedcba988;
	LgCfg1.word = 0x00002100;

#if	defined(MT76x0)
	/*
		0x1360
		[23:20] - Auto fall back MCS as VHT 1SS MCS=9
		[19:16] - Auto fall back MCS as VHT 1SS MCS=8
	*/
	if (IS_MT76x0(pAd)
		LgCfg1.word = 0x00872100;
#endif /* MT76x0 */
#ifdef MT76x2
	if (IS_MT76x2(pAd))
		LgCfg1.word = 0x87872100;
#endif

#ifdef NEW_RATE_ADAPT_SUPPORT
	/* Use standard fallback if using new rate table */
	if (ADAPT_RATE_TABLE(pRateTable))
		goto skipUpdate;
#endif /* NEW_RATE_ADAPT_SUPPORT */

#ifdef AGS_SUPPORT
	if (bUseAGS) {
		pNextTxRate_AGS = (RTMP_RA_AGS_TB *)pRateTable+1;
		pNextTxRate = (RTMP_RA_LEGACY_TB *)pNextTxRate_AGS;
	} else
#endif /* AGS_SUPPORT */
		pNextTxRate = (RTMP_RA_LEGACY_TB *)pRateTable+1;

	for (i = 1; i < *((PUCHAR) pRateTable); i++) {
#ifdef AGS_SUPPORT
		if (bUseAGS) {
			pCurrTxRate_AGS = (RTMP_RA_AGS_TB *)pRateTable+1+i;
			pCurrTxRate = (RTMP_RA_LEGACY_TB *)pCurrTxRate_AGS;
		} else
#endif /* AGS_SUPPORT */
			pCurrTxRate = (RTMP_RA_LEGACY_TB *)pRateTable+1+i;

		switch (pCurrTxRate->Mode) {
		case 0:		/* CCK */
			break;
		case 1:		/* OFDM */
			{
			switch(pCurrTxRate->CurrMCS) {
			case 0:
				LgCfg0.field.OFDMMCS0FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			case 1:
				LgCfg0.field.OFDMMCS1FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			case 2:
				LgCfg0.field.OFDMMCS2FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			case 3:
				LgCfg0.field.OFDMMCS3FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			case 4:
				LgCfg0.field.OFDMMCS4FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			case 5:
				LgCfg0.field.OFDMMCS5FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			case 6:
				LgCfg0.field.OFDMMCS6FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			case 7:
				LgCfg0.field.OFDMMCS7FBK = (pNextTxRate->Mode == MODE_OFDM) ? (pNextTxRate->CurrMCS+8): pNextTxRate->CurrMCS;
				break;
			}
			}
		break;
#ifdef DOT11_N_SUPPORT
		case 2:		/* HT-MIX */
		case 3:		/* HT-GF */
			{
			if ((pNextTxRate->Mode >= MODE_HTMIX) && (pCurrTxRate->CurrMCS != pNextTxRate->CurrMCS)) {
				if (pCurrTxRate->CurrMCS <= 15) {
					switch (pCurrTxRate->CurrMCS) {
					case 0:
						HtCfg0.field.HTMCS0FBK = pNextTxRate->CurrMCS;
						break;
					case 1:
						HtCfg0.field.HTMCS1FBK = pNextTxRate->CurrMCS;
						break;
					case 2:
						HtCfg0.field.HTMCS2FBK = pNextTxRate->CurrMCS;
						break;
					case 3:
						HtCfg0.field.HTMCS3FBK = pNextTxRate->CurrMCS;
						break;
					case 4:
						HtCfg0.field.HTMCS4FBK = pNextTxRate->CurrMCS;
						break;
					case 5:
						HtCfg0.field.HTMCS5FBK = pNextTxRate->CurrMCS;
						break;
					case 6:
						HtCfg0.field.HTMCS6FBK = pNextTxRate->CurrMCS;
						break;
					case 7:
						HtCfg0.field.HTMCS7FBK = pNextTxRate->CurrMCS;
						break;
					case 8:
						HtCfg1.field.HTMCS8FBK = 0;//pNextTxRate->CurrMCS;
						break;
					case 9:
						HtCfg1.field.HTMCS9FBK = pNextTxRate->CurrMCS;
						break;
					case 10:
						HtCfg1.field.HTMCS10FBK = pNextTxRate->CurrMCS;
						break;
					case 11:
						HtCfg1.field.HTMCS11FBK = pNextTxRate->CurrMCS;
						break;
					case 12:
						HtCfg1.field.HTMCS12FBK = pNextTxRate->CurrMCS;
						break;
					case 13:
						HtCfg1.field.HTMCS13FBK = pNextTxRate->CurrMCS;
						break;
					case 14:
						HtCfg1.field.HTMCS14FBK = pNextTxRate->CurrMCS;
						break;
					case 15:
						HtCfg1.field.HTMCS15FBK = pNextTxRate->CurrMCS;
						break;
					}
				} else
#ifdef AGS_SUPPORT
				if ((bUseAGS == TRUE) &&
					(pCurrTxRate->CurrMCS >= 16) && (pCurrTxRate->CurrMCS <= 23)) {
					switch(pCurrTxRate->CurrMCS) {
					case 16:
						Ht3SSCfg0.field.HTMCS16FBK = pNextTxRate->CurrMCS;
						break;
					case 17:
						Ht3SSCfg0.field.HTMCS17FBK = pNextTxRate->CurrMCS;
						break;
					case 18:
						Ht3SSCfg0.field.HTMCS18FBK = pNextTxRate->CurrMCS;
						break;
					case 19:
						Ht3SSCfg0.field.HTMCS19FBK = pNextTxRate->CurrMCS;
						break;
					case 20:
						Ht3SSCfg1.field.HTMCS20FBK = pNextTxRate->CurrMCS;
						break;
					case 21:
						Ht3SSCfg1.field.HTMCS21FBK = pNextTxRate->CurrMCS;
						break;
					case 22:
						Ht3SSCfg1.field.HTMCS22FBK = pNextTxRate->CurrMCS;
						break;
					case 23:
						Ht3SSCfg1.field.HTMCS23FBK = pNextTxRate->CurrMCS;
						break;
					}
				} else
#endif /* AGS_SUPPORT */
					DBGPRINT(RT_DEBUG_ERROR,
							("AsicUpdateAutoFallBackTable: not support CurrMCS=%d\n",
							pCurrTxRate->CurrMCS));
				}
			}
			break;
#endif /* DOT11_N_SUPPORT */
		}

		pNextTxRate = pCurrTxRate;
	}

#ifdef AGS_SUPPORT
	if (bUseAGS == TRUE) {
		Ht3SSCfg0.field.HTMCS16FBK = 0x8; // MCS 16 -> MCS 8
		HtCfg1.field.HTMCS8FBK = 0x0; // MCS 8 -> MCS 0

		LgCfg0.field.OFDMMCS2FBK = 0x3; // OFDM 12 -> CCK 11
		LgCfg0.field.OFDMMCS1FBK = 0x2; // OFDM 9 -> CCK 5.5
		LgCfg0.field.OFDMMCS0FBK = 0x2; // OFDM 6 -> CCK 5.5
	}
#endif /* AGS_SUPPORT */

#ifdef NEW_RATE_ADAPT_SUPPORT
skipUpdate:
#endif /* NEW_RATE_ADAPT_SUPPORT */

	RTMP_IO_WRITE32(pAd, HT_FBK_CFG0, HtCfg0.word);
	RTMP_IO_WRITE32(pAd, HT_FBK_CFG1, HtCfg1.word);
	RTMP_IO_WRITE32(pAd, LG_FBK_CFG0, LgCfg0.word);
	RTMP_IO_WRITE32(pAd, LG_FBK_CFG1, LgCfg1.word);

#ifdef DOT11N_SS3_SUPPORT
	if (IS_RT2883(pAd) || IS_RT3883(pAd)
#ifdef AGS_SUPPORT
		|| (bUseAGS == TRUE)
#endif /* AGS_SUPPORT */
	) {
		RTMP_IO_WRITE32(pAd, TX_FBK_CFG_3S_0, Ht3SSCfg0.word);
		RTMP_IO_WRITE32(pAd, TX_FBK_CFG_3S_1, Ht3SSCfg1.word);
		DBGPRINT(RT_DEBUG_TRACE,
				("AsicUpdateAutoFallBackTable: Ht3SSCfg0=0x%x, Ht3SSCfg1=0x%x\n",
				Ht3SSCfg0.word, Ht3SSCfg1.word));
	}
#endif /* DOT11N_SS3_SUPPORT */

}
#endif /* CONFIG_STA_SUPPORT */


BOOLEAN AsicSetAutoFallBack(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
	TX_RTY_CFG_STRUC tx_rty_cfg = {.word = 0};

	RTMP_IO_READ32(pAd, TX_RTY_CFG, &tx_rty_cfg.word);
	tx_rty_cfg.field.TxautoFBEnable = ((enable == TRUE) ? 1 : 0);
	RTMP_IO_WRITE32(pAd, TX_RTY_CFG, tx_rty_cfg.word);

	return TRUE;
}


/*
	========================================================================

	Routine Description:
		Set MAC register value according operation mode.
		OperationMode AND bNonGFExist are for MM and GF Proteciton.
		If MM or GF mask is not set, those passing argument doesn't not take effect.

		Operation mode meaning:
		= 0 : Pure HT, no preotection.
		= 0x01; there may be non-HT devices in both the control and extension channel, protection is optional in BSS.
		= 0x10: No Transmission in 40M is protected.
		= 0x11: Transmission in both 40M and 20M shall be protected
		if (bNonGFExist)
			we should choose not to use GF. But still set correct ASIC registers.
	========================================================================
*/
typedef enum _PROT_REG_IDX_{
	REG_IDX_CCK = 0,	/* 0x1364 */
	REG_IDX_OFDM = 1,	/* 0x1368 */
	REG_IDX_MM20 = 2,	/* 0x136c */
	REG_IDX_MM40 = 3,	/* 0x1370 */
	REG_IDX_GF20 = 4,	/* 0x1374 */
	REG_IDX_GF40 = 5,	/* 0x1378 */
}PROT_REG_IDX;


void AsicUpdateProtect(PRTMP_ADAPTER pAd, USHORT OperationMode, UCHAR SetMask,
	BOOLEAN bDisableBGProtect, BOOLEAN bNonGFExist)
{
	PROT_CFG_STRUC	ProtCfg, ProtCfg4;
	UINT32 Protect[6];
	USHORT offset;
	UCHAR i, PhyMode = 0x4000;
	UINT32 MacReg = 0;
#ifdef DOT11_VHT_AC
#ifdef RT65xx
	PROT_CFG_STRUC vht_port_cfg = {.word = 0};
	UINT16 protect_rate = 0;
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */

#ifdef RALINK_ATE
	if (ATE_ON(pAd))
		return;
#endif /* RALINK_ATE */

#ifdef DOT11_N_SUPPORT
	if (!(pAd->CommonCfg.bHTProtect) && (OperationMode != 8))
		return;

	if (pAd->BATable.numDoneOriginator) {
		/* enable the RTS/CTS to avoid channel collision*/
		SetMask |= ALLN_SETPROTECT;
		OperationMode = 8;
	}
#endif /* DOT11_N_SUPPORT */

	/* Config ASIC RTS threshold register*/
	RTMP_IO_READ32(pAd, TX_RTS_CFG, &MacReg);
	MacReg &= 0xFF0000FF;
	/* If the user want disable RtsThreshold and enbale Amsdu/Ralink-Aggregation,
	 *  set the RtsThreshold as 4096*/
	if ((
#ifdef DOT11_N_SUPPORT
		(pAd->CommonCfg.BACapability.field.AmsduEnable) ||
#endif /* DOT11_N_SUPPORT */
		(pAd->CommonCfg.bAggregationCapable == TRUE))
		&& pAd->CommonCfg.RtsThreshold == MAX_RTS_THRESHOLD) {
		MacReg |= (0x1000 << 8);
	} else {
		MacReg |= (pAd->CommonCfg.RtsThreshold << 8);
	}

	RTMP_IO_WRITE32(pAd, TX_RTS_CFG, MacReg);

	/* Initial common protection settings*/
	RTMPZeroMemory(Protect, sizeof(Protect));
	ProtCfg4.word = 0;
	ProtCfg.word = 0;
	ProtCfg.field.TxopAllowGF40 = 1;
	ProtCfg.field.TxopAllowGF20 = 1;
	ProtCfg.field.TxopAllowMM40 = 1;
	ProtCfg.field.TxopAllowMM20 = 1;
	ProtCfg.field.TxopAllowOfdm = 1;
	ProtCfg.field.TxopAllowCck = 1;
	ProtCfg.field.RTSThEn = 1;
	ProtCfg.field.ProtectNav = ASIC_SHORTNAV;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
	// TODO: shiang, is that a correct way to set 0x2000 here??
	if (IS_RT65XX(pAd))
		/* Bit 15:13, 0:Legacy CCK, 1: Legacy OFDM, 2: HT mix mode,
		 * 3: HT green field, 4: VHT mode, 5-7: Reserved */
		PhyMode = 0x2000;
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */

	/* update PHY mode and rate*/
	if (pAd->OpMode == OPMODE_AP) {
		/* update PHY mode and rate*/
		if (pAd->CommonCfg.Channel > 14)
			ProtCfg.field.ProtectRate = PhyMode;
		ProtCfg.field.ProtectRate |= pAd->CommonCfg.RtsRate;
	} else if (pAd->OpMode == OPMODE_STA) {
		// Decide Protect Rate for Legacy packet
		if (pAd->CommonCfg.Channel > 14) {
			ProtCfg.field.ProtectRate = PhyMode; // OFDM 6Mbps
		} else {
			ProtCfg.field.ProtectRate = 0x0000; // CCK 1Mbps
			if (pAd->CommonCfg.MinTxRate > RATE_11)
				ProtCfg.field.ProtectRate |= PhyMode; // OFDM 6Mbps
		}
	}

#ifdef DOT11_VHT_AC
#ifdef RT65xx
	if (IS_RT65XX(pAd))
		protect_rate = ProtCfg.field.ProtectRate;
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */

#ifdef CONFIG_FPGA_MODE
//+++Add by shiang for debug
	if (pAd->fpga_ctl.fpga_on & 0x8)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s():RtsRate=%d\n",
					__FUNCTION__, pAd->CommonCfg.RtsRate));
	}
//---Add by shiang for debug
#endif /* CONFIG_FPGA_MODE */

	/* Handle legacy(B/G) protection*/
	if (bDisableBGProtect) {
		/*ProtCfg.field.ProtectRate = pAd->CommonCfg.RtsRate;*/
		ProtCfg.field.ProtectCtrl = 0;
		Protect[REG_IDX_CCK] = ProtCfg.word;
		Protect[REG_IDX_OFDM] = ProtCfg.word;
		pAd->FlgCtsEnabled = 0; /* CTS-self is not used */
	} else {
		if (pAd->CommonCfg.Channel <= 14) {
			/*ProtCfg.field.ProtectRate = pAd->CommonCfg.RtsRate;*/
			/* CCK do not need to be protected*/
			ProtCfg.field.ProtectCtrl = 0;
			Protect[REG_IDX_CCK] = ProtCfg.word;
			/* OFDM needs using CCK to protect*/
			ProtCfg.field.ProtectCtrl = ASIC_CTS;
			Protect[REG_IDX_OFDM] = ProtCfg.word;
			pAd->FlgCtsEnabled = 1; /* CTS-self is used */
		} else {
			ProtCfg.field.ProtectCtrl = 0;/* CCK do not need to be protected*/
			Protect[REG_IDX_CCK] = ProtCfg.word;
			Protect[REG_IDX_OFDM] = ProtCfg.word;
			pAd->FlgCtsEnabled = 0; /* CTS-self is not used */
		}
	}

#ifdef DOT11_N_SUPPORT
	/* Decide HT frame protection.*/
	if ((SetMask & ALLN_SETPROTECT) != 0) {
		switch(OperationMode) {
		case 0x0:
			/* NO PROTECT */
			/* 1.All STAs in the BSS are 20/40 MHz HT*/
			/* 2. in ai 20/40MHz BSS*/
			/* 3. all STAs are 20MHz in a 20MHz BSS*/
			/* Pure HT. no protection.*/

			/* MM20_PROT_CFG*/
			/*	Reserved (31:27)*/
			/* 	PROT_TXOP(25:20) -- 010111*/
			/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
			/*  PROT_CTRL(17:16) -- 00 (None)*/
#ifdef DOT11_VHT_AC
			/* 	PROT_RATE(15:0)  -- 0x2004 (OFDM 24M)*/
			Protect[2] = 0x01742004;
#else /* DOT11_VHT_AC */
			/* 	PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)*/
			Protect[2] = 0x01744004;
#endif /* !DOT11_VHT_AC */

			/* MM40_PROT_CFG*/
			/*	Reserved (31:27)*/
			/* 	PROT_TXOP(25:20) -- 111111*/
			/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
			/*  PROT_CTRL(17:16) -- 00 (None) */
#ifdef DOT11_VHT_AC
			/* 	PROT_RATE(15:0)  -- 0x2084 (duplicate OFDM 24M)*/
			Protect[3] = 0x03f42084;
#else
			/* 	PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)*/
			Protect[3] = 0x03f44084;
#endif

			/* CF20_PROT_CFG*/
			/*	Reserved (31:27)*/
			/* 	PROT_TXOP(25:20) -- 010111*/
			/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
			/*  PROT_CTRL(17:16) -- 00 (None)*/
#ifdef DOT11_VHT_AC
			/* 	PROT_RATE(15:0)  -- 0x2004 (OFDM 24M)*/
			Protect[4] = 0x01742004;
#else
			/* 	PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)*/
			Protect[4] = 0x01744004;
#endif

			/* CF40_PROT_CFG*/
			/*	Reserved (31:27)*/
			/* 	PROT_TXOP(25:20) -- 111111*/
			/*	PROT_NAV(19:18)  -- 01 (Short NAV protection)*/
			/*  PROT_CTRL(17:16) -- 00 (None)*/
#ifdef DOT11_VHT_AC
			/* 	PROT_RATE(15:0)  -- 0x2084 (duplicate OFDM 24M)*/
			Protect[5] = 0x03f42084;
#else
			/* 	PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)*/
			Protect[5] = 0x03f44084;
#endif

			if (bNonGFExist) {
				/* PROT_NAV(19:18)  -- 01 (Short NAV protectiion)*/
				/* PROT_CTRL(17:16) -- 01 (RTS/CTS)*/
#ifdef DOT11_VHT_AC
				Protect[REG_IDX_GF20] = 0x01752004;
				Protect[REG_IDX_GF40] = 0x03f52084;
#else
				Protect[REG_IDX_GF20] = 0x01754004;
				Protect[REG_IDX_GF40] = 0x03f54084;
#endif
			}
			pAd->CommonCfg.IOTestParm.bRTSLongProtOn = FALSE;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
			// TODO: shiang-6590, fix me for this protection mechanism
			if (IS_RT65XX(pAd)) {
				PROT_CFG_STRUC vht_port_cfg;

				RTMP_IO_READ32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = 0;
				RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);

				RTMP_IO_READ32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = 0;


				RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);

				RTMP_IO_READ32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = 0;


				RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);
			}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
			break;

		case 1:
			/* This is "HT non-member protection mode."*/
			/* If there may be non-HT STAs my BSS*/
#ifdef DOT11_VHT_AC
			ProtCfg.word = 0x01742004;	/* PROT_CTRL(17:16) : 0 (None)*/
			ProtCfg4.word = 0x03f42084; /* duplicate legacy 24M. BW set 1.*/
#else
			ProtCfg.word = 0x01744004;	/* PROT_CTRL(17:16) : 0 (None)*/
			ProtCfg4.word = 0x03f44084;	/* duplicae legacy 24M. BW set 1.*/
#endif
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED)) {
				ProtCfg.word = 0x01740003; /*ERP use Protection bit is set, use protection rate at Clause 18..*/
				ProtCfg4.word = 0x03f40003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083; */
			}
			/*Assign Protection method for 20&40 MHz packets*/
			ProtCfg.field.ProtectCtrl = ASIC_RTS;
			ProtCfg.field.ProtectNav = ASIC_SHORTNAV;
			ProtCfg4.field.ProtectCtrl = ASIC_RTS;
			ProtCfg4.field.ProtectNav = ASIC_SHORTNAV;
			Protect[REG_IDX_MM20] = ProtCfg.word;
			Protect[REG_IDX_MM40] = ProtCfg4.word;
			Protect[REG_IDX_GF20] = ProtCfg.word;
			Protect[REG_IDX_GF40] = ProtCfg4.word;
			pAd->CommonCfg.IOTestParm.bRTSLongProtOn = TRUE;
#ifdef DOT11_VHT_AC
#ifdef RT65xx
			// TODO: shiang-6590, fix me for this protection mechanism
			if (IS_RT65XX(pAd)) {
				// Temporary tuen on RTS in VHT, MAC: TX_PROT_CFG6, TX_PROT_CFG7, TX_PROT_CFG8
				PROT_CFG_STRUC vht_port_cfg;

				RTMP_IO_READ32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;

				RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);

				RTMP_IO_READ32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;

				RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);

				RTMP_IO_READ32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;

				RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);
			}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */

			break;
				
		case 2:
			/* If only HT STAs are in BSS. at least one is 20MHz. Only protect 40MHz packets*/
#ifdef DOT11_VHT_AC
			ProtCfg.word = 0x01742004;  /* PROT_CTRL(17:16) : 0 (None)*/
			ProtCfg4.word = 0x03f42084; /* duplicaet legacy 24M. BW set 1.*/
#else
			ProtCfg.word = 0x01744004;  /* PROT_CTRL(17:16) : 0 (None)*/
			ProtCfg4.word = 0x03f44084; /* duplicaet legacy 24M. BW set 1.*/
#endif
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
			{
				ProtCfg.word = 0x01740003;	/*ERP use Protection bit is set, use protection rate at Clause 18..*/
				ProtCfg4.word = 0x03f40003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083; */
			} 
			/*Assign Protection method for 40MHz packets*/
			ProtCfg4.field.ProtectCtrl = ASIC_RTS;
			ProtCfg4.field.ProtectNav = ASIC_SHORTNAV;
			Protect[REG_IDX_MM20] = ProtCfg.word;
			Protect[REG_IDX_MM40] = ProtCfg4.word;
			if (bNonGFExist)
			{
				ProtCfg.field.ProtectCtrl = ASIC_RTS;
				ProtCfg.field.ProtectNav = ASIC_SHORTNAV;
			}
			Protect[REG_IDX_GF20] = ProtCfg.word;
			Protect[REG_IDX_GF40] = ProtCfg4.word;

			pAd->CommonCfg.IOTestParm.bRTSLongProtOn = FALSE;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
			// TODO: shiang-6590, fix me for this protection mechanism
			if (IS_RT65XX(pAd))
			{
				PROT_CFG_STRUC vht_port_cfg;

				RTMP_IO_READ32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = 0;
				RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);  

				RTMP_IO_READ32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
				RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);  

				RTMP_IO_READ32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
				RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);  
			}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
			break;
				
		case 3:
			/* HT mixed mode.	 PROTECT ALL!*/
			/* Assign Rate*/
#ifdef DOT11_VHT_AC
			ProtCfg.word = 0x01742004;	/*duplicaet legacy 24M. BW set 1.*/
			ProtCfg4.word = 0x03f42084;
#else
			ProtCfg.word = 0x01744004;	/*duplicaet legacy 24M. BW set 1.*/
			ProtCfg4.word = 0x03f44084;
#endif
			/* both 20MHz and 40MHz are protected. Whether use RTS or CTS-to-self depends on the*/
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
			{
				ProtCfg.word = 0x01740003;	/*ERP use Protection bit is set, use protection rate at Clause 18..*/
				ProtCfg4.word = 0x03f40003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083*/
			}
			/*Assign Protection method for 20&40 MHz packets*/
			ProtCfg.field.ProtectCtrl = ASIC_RTS;
			ProtCfg.field.ProtectNav = ASIC_SHORTNAV;
			ProtCfg4.field.ProtectCtrl = ASIC_RTS;
			ProtCfg4.field.ProtectNav = ASIC_SHORTNAV;
			Protect[REG_IDX_MM20] = ProtCfg.word;
			Protect[REG_IDX_MM40] = ProtCfg4.word;
			Protect[REG_IDX_GF20] = ProtCfg.word;
			Protect[REG_IDX_GF40] = ProtCfg4.word;
			pAd->CommonCfg.IOTestParm.bRTSLongProtOn = TRUE;

#ifdef DOT11_VHT_AC
#ifdef RT65xx
			// TODO: shiang-6590, fix me for this protection mechanism
			if (IS_RT65XX(pAd))
			{
				// Temporary tuen on RTS in VHT, MAC: TX_PROT_CFG6, TX_PROT_CFG7, TX_PROT_CFG8
				PROT_CFG_STRUC vht_port_cfg;

				RTMP_IO_READ32(pAd, TX_PROT_CFG6, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
				RTMP_IO_WRITE32(pAd, TX_PROT_CFG6, vht_port_cfg.word);  

				RTMP_IO_READ32(pAd, TX_PROT_CFG7, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
				RTMP_IO_WRITE32(pAd, TX_PROT_CFG7, vht_port_cfg.word);  

				RTMP_IO_READ32(pAd, TX_PROT_CFG8, &vht_port_cfg.word);
				vht_port_cfg.field.ProtectCtrl = ASIC_RTS;
				vht_port_cfg.field.ProtectNav = ASIC_SHORTNAV;
				RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, vht_port_cfg.word);  
			}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
			break;
				
		case 8:
			/* Special on for Atheros problem n chip.*/
#ifdef DOT11_VHT_AC
			ProtCfg.word = 0x01752004;	/*duplicaet legacy 24M. BW set 1.*/
			ProtCfg4.word = 0x03f52084;
#else
			ProtCfg.word = 0x01754004;	/*duplicaet legacy 24M. BW set 1.*/
			ProtCfg4.word = 0x03f54084;
#endif
			if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_BG_PROTECTION_INUSED))
			{
				ProtCfg.word = 0x01750003;	/*ERP use Protection bit is set, use protection rate at Clause 18..*/
				ProtCfg4.word = 0x03f50003; /* Don't duplicate RTS/CTS in CCK mode. 0x03f40083*/
			}
			
			Protect[REG_IDX_MM20] = ProtCfg.word; 	/*0x01754004;*/
			Protect[REG_IDX_MM40] = ProtCfg4.word; /*0x03f54084;*/
			Protect[REG_IDX_GF20] = ProtCfg.word; 	/*0x01754004;*/
			Protect[REG_IDX_GF40] = ProtCfg4.word; /*0x03f54084;*/
			pAd->CommonCfg.IOTestParm.bRTSLongProtOn = TRUE;
			break;
		}
	}
#endif /* DOT11_N_SUPPORT */

	offset = CCK_PROT_CFG;
	for (i = 0;i < 6;i++) {
		if ((SetMask & (1<< i))) {
			RTMP_IO_WRITE32(pAd, offset + i*4, Protect[i]);
		}
	}

#ifdef DOT11_VHT_AC
#ifdef RT65xx
	if (IS_RT65XX(pAd)) {
		RTMP_IO_READ32(pAd, TX_PROT_CFG8, &MacReg);
		MacReg &= (~0x18000000);
		if (pAd->CommonCfg.vht_bw_signal) {
			if (pAd->CommonCfg.vht_bw_signal == 1) /* static */
				MacReg |= 0x08000000;
			else if (pAd->CommonCfg.vht_bw_signal == 2)/* dynamic */
				MacReg |= 0x18000000;
		}
		RTMP_IO_WRITE32(pAd, TX_PROT_CFG8, MacReg);
	}
#endif /* RT65xx */
#endif /* DOT11_VHT_AC */
}


void AsicBBPAdjust(RTMP_ADAPTER *pAd)
{
	// TODO: shiang-6590, now this function only used for AP mode, why we need this differentation?
	if (pAd->chipOps.ChipBBPAdjust != NULL)
		pAd->chipOps.ChipBBPAdjust(pAd);
}


/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSwitchChannel(RTMP_ADAPTER *pAd, UCHAR Channel, BOOLEAN bScan)
{
	UCHAR bw;
#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND
	POS_COOKIE  pObj = (POS_COOKIE) pAd->OS_Cookie;
#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))
		return;

	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);
#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND

			if( (RTMP_Usb_AutoPM_Get_Interface(pObj->pUsb_Dev,pObj->intf)) == 1)
			{
				DBGPRINT(RT_DEBUG_TRACE, ("%s(): autopm_resume success\n", __FUNCTION__));
				RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);
			}
			else if ((RTMP_Usb_AutoPM_Get_Interface(pObj->pUsb_Dev,pObj->intf)) == (-1))
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s(): autopm_resume fail\n", __FUNCTION__));
				RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_SUSPEND);
				return;
			} else {
				DBGPRINT(RT_DEBUG_TRACE, ("%s(): autopm_resume do nothing\n", __FUNCTION__));
			}
#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */



#ifdef DOT11_VHT_AC
	if (pAd->CommonCfg.BBPCurrentBW == BW_80)
		pAd->hw_cfg.cent_ch = pAd->CommonCfg.vht_cent_ch;
	else
#endif /* DOT11_VHT_AC */
		pAd->hw_cfg.cent_ch = Channel;

	if (pAd->chipOps.ChipSwitchChannel)
		pAd->chipOps.ChipSwitchChannel(pAd, Channel, bScan);
	else
		DBGPRINT(RT_DEBUG_ERROR,
				("For this chip, no specified channel switch function!\n"));

	/* R66 should be set according to Channel and use 20MHz when scanning*/
	if (bScan) {
		bw = BW_20;
	} else {
		bw = pAd->CommonCfg.BBPCurrentBW;
	}
	RTMPSetAGCInitValue(pAd, bw);

#ifdef TXBF_SUPPORT
	/* FW will initialize TxBf HW status. Re-calling this AP could
	 * recover previous status */
	rtmp_asic_set_bf(pAd);

#ifdef MT76x2
	if (IS_MT76x2(pAd)) {
		// Disable BF HW to apply profile to packets when nSS == 2.
		/* Maybe it can be initialized at chip init but removing the same
		 * CR initialization from FW will be better */
		RTMP_IO_READ32(pAd, TXO_R4, &value32);
		value32 |= 0x2000000;
		RTMP_IO_WRITE32(pAd, TXO_R4, value32);

		// Enable SIG-B CRC check
		RTMP_IO_READ32(pAd, RXO_R13, &value32);
		value32 |= 0x100;
		RTMP_IO_WRITE32(pAd, RXO_R13, value32);
	}
#endif /* MT76x2 */
#endif /* TXBF_SUPPORT */
}


/*
	==========================================================================
	Description:
		This function is required for 2421 only, and should not be used during
		site survey. It's only required after NIC decided to stay at a channel
		for a longer period.
		When this function is called, it's always after AsicSwitchChannel().

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicLockChannel(PRTMP_ADAPTER pAd, UCHAR Channel)
{
}

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */


#ifdef RTMP_TEMPERATURE_COMPENSATION
VOID InitLookupTable(
	IN PRTMP_ADAPTER pAd)
{
	int Idx, IdxTmp;
	int i;
	enum IEEE80211_BAND band;
	int band_nums = 1;
	const int Offset = 7;
	EEPROM_WORD_STRUC WordStruct = {{0}};
	UCHAR PlusStepNum[IEEE80211_BAND_NUMS][8] = {{0, 1, 3, 2, 3, 3, 3, 2}, {0, 1, 3, 2, 3, 3, 3, 2}};
	UCHAR MinusStepNum[IEEE80211_BAND_NUMS][8] = {{1, 1, 1, 1, 1, 1, 0, 1}, {1, 1, 1, 1, 1, 1, 0, 1}};
	UCHAR Step[IEEE80211_BAND_NUMS] = {10, 10};
	UCHAR RFValue = 0, BbpValue = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("==> InitLookupTable\n"));
	
	/* Read from EEPROM, as parameters for lookup table for G band */
	RT28xx_EEPROM_READ16(pAd, 0x6e, WordStruct.word);
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4G] EEPROM 6e = %x\n", WordStruct.word));
	PlusStepNum[IEEE80211_BAND_2G][0] = (WordStruct.field.Byte0 & 0x0F);
	PlusStepNum[IEEE80211_BAND_2G][1] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
	PlusStepNum[IEEE80211_BAND_2G][2] = (WordStruct.field.Byte1 & 0x0F);
	PlusStepNum[IEEE80211_BAND_2G][3] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

	RT28xx_EEPROM_READ16(pAd, 0x70, WordStruct.word);
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4G] EEPROM 70 = %x\n", WordStruct.word));
	PlusStepNum[IEEE80211_BAND_2G][4] = (WordStruct.field.Byte0 & 0x0F);
	PlusStepNum[IEEE80211_BAND_2G][5] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
	PlusStepNum[IEEE80211_BAND_2G][6] = (WordStruct.field.Byte1 & 0x0F);
	PlusStepNum[IEEE80211_BAND_2G][7] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

	RT28xx_EEPROM_READ16(pAd, 0x72, WordStruct.word);
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4G] EEPROM 72 = %x\n", WordStruct.word));
	MinusStepNum[IEEE80211_BAND_2G][0] = (WordStruct.field.Byte0 & 0x0F);
	MinusStepNum[IEEE80211_BAND_2G][1] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
	MinusStepNum[IEEE80211_BAND_2G][2] = (WordStruct.field.Byte1 & 0x0F);
	MinusStepNum[IEEE80211_BAND_2G][3] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

	RT28xx_EEPROM_READ16(pAd, 0x74, WordStruct.word);
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4] EEPROM 74 = %x\n", WordStruct.word));
	MinusStepNum[IEEE80211_BAND_2G][4] = (WordStruct.field.Byte0 & 0x0F);
	MinusStepNum[IEEE80211_BAND_2G][5] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
	MinusStepNum[IEEE80211_BAND_2G][6] = (WordStruct.field.Byte1 & 0x0F);
	MinusStepNum[IEEE80211_BAND_2G][7] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

	RT28xx_EEPROM_READ16(pAd, 0x76, WordStruct.word);
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4G] EEPROM 76 = %x\n", WordStruct.word));
	pAd->TxPowerCtrl.TssiGain[IEEE80211_BAND_2G] = (WordStruct.field.Byte0 & 0x0F);
	Step[IEEE80211_BAND_2G] = (WordStruct.field.Byte0 >> 4);
	pAd->TxPowerCtrl.RefTemp[IEEE80211_BAND_2G] = (CHAR)WordStruct.field.Byte1;

	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4G] Plus = %u %u %u %u %u %u %u %u\n",
		PlusStepNum[IEEE80211_BAND_2G][0],
		PlusStepNum[IEEE80211_BAND_2G][1],
		PlusStepNum[IEEE80211_BAND_2G][2],
		PlusStepNum[IEEE80211_BAND_2G][3],
		PlusStepNum[IEEE80211_BAND_2G][4],
		PlusStepNum[IEEE80211_BAND_2G][5],
		PlusStepNum[IEEE80211_BAND_2G][6],
		PlusStepNum[IEEE80211_BAND_2G][7]
		));
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4G] Minus = %u %u %u %u %u %u %u %u\n",
		MinusStepNum[IEEE80211_BAND_2G][0],
		MinusStepNum[IEEE80211_BAND_2G][1],
		MinusStepNum[IEEE80211_BAND_2G][2],
		MinusStepNum[IEEE80211_BAND_2G][3],
		MinusStepNum[IEEE80211_BAND_2G][4],
		MinusStepNum[IEEE80211_BAND_2G][5],
		MinusStepNum[IEEE80211_BAND_2G][6],
		MinusStepNum[IEEE80211_BAND_2G][7]
		));
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4G] tssi gain/step = %u\n", pAd->TxPowerCtrl.TssiGain[IEEE80211_BAND_2G]));
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4] Step = %u\n", Step[IEEE80211_BAND_2G]));
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 2.4] RefTemp_2G = %d\n", pAd->TxPowerCtrl.RefTemp[IEEE80211_BAND_2G]));

#ifdef A_BAND_SUPPORT
	if (RFIC_IS_5G_BAND(pAd))
	{
		/* Read from EEPROM, as parameters for lookup table for A band */
		RT28xx_EEPROM_READ16(pAd, 0xd4, WordStruct.word);
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] EEPROM d4 = %x\n", WordStruct.word));
		PlusStepNum[IEEE80211_BAND_5G][0] = (WordStruct.field.Byte0 & 0x0F);
		PlusStepNum[IEEE80211_BAND_5G][1] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
		PlusStepNum[IEEE80211_BAND_5G][2] = (WordStruct.field.Byte1 & 0x0F);
		PlusStepNum[IEEE80211_BAND_5G][3] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

		RT28xx_EEPROM_READ16(pAd, 0xd6, WordStruct.word);
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] EEPROM d6 = %x\n", WordStruct.word));
		PlusStepNum[IEEE80211_BAND_5G][4] = (WordStruct.field.Byte0 & 0x0F);
		PlusStepNum[IEEE80211_BAND_5G][5] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
		PlusStepNum[IEEE80211_BAND_5G][6] = (WordStruct.field.Byte1 & 0x0F);
		PlusStepNum[IEEE80211_BAND_5G][7] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

		RT28xx_EEPROM_READ16(pAd, 0xd8, WordStruct.word);
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] EEPROM d8 = %x\n", WordStruct.word));
		MinusStepNum[IEEE80211_BAND_5G][0] = (WordStruct.field.Byte0 & 0x0F);
		MinusStepNum[IEEE80211_BAND_5G][1] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
		MinusStepNum[IEEE80211_BAND_5G][2] = (WordStruct.field.Byte1 & 0x0F);
		MinusStepNum[IEEE80211_BAND_5G][3] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

		RT28xx_EEPROM_READ16(pAd, 0xda, WordStruct.word);
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] EEPROM da = %x\n", WordStruct.word));
		MinusStepNum[IEEE80211_BAND_5G][4] = (WordStruct.field.Byte0 & 0x0F);
		MinusStepNum[IEEE80211_BAND_5G][5] = (((WordStruct.field.Byte0 & 0xF0) >> 4) & 0x0F);
		MinusStepNum[IEEE80211_BAND_5G][6] = (WordStruct.field.Byte1 & 0x0F);
		MinusStepNum[IEEE80211_BAND_5G][7] = (((WordStruct.field.Byte1 & 0xF0) >> 4) & 0x0F);

		RT28xx_EEPROM_READ16(pAd, 0xdc, WordStruct.word);
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] EEPROM dc = %x\n", WordStruct.word));
		pAd->TxPowerCtrl.TssiGain[IEEE80211_BAND_5G] = (WordStruct.field.Byte0 & 0x0F);
		Step[IEEE80211_BAND_5G] = (WordStruct.field.Byte0 >> 4);
		pAd->TxPowerCtrl.RefTemp[IEEE80211_BAND_5G] = (CHAR)WordStruct.field.Byte1;

		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] Plus = %u %u %u %u %u %u %u %u\n",
			PlusStepNum[IEEE80211_BAND_5G][0],
			PlusStepNum[IEEE80211_BAND_5G][1],
			PlusStepNum[IEEE80211_BAND_5G][2],
			PlusStepNum[IEEE80211_BAND_5G][3],
			PlusStepNum[IEEE80211_BAND_5G][4],
			PlusStepNum[IEEE80211_BAND_5G][5],
			PlusStepNum[IEEE80211_BAND_5G][6],
			PlusStepNum[IEEE80211_BAND_5G][7]
			));
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] Minus = %u %u %u %u %u %u %u %u\n",
			MinusStepNum[IEEE80211_BAND_5G][0],
			MinusStepNum[IEEE80211_BAND_5G][1],
			MinusStepNum[IEEE80211_BAND_5G][2],
			MinusStepNum[IEEE80211_BAND_5G][3],
			MinusStepNum[IEEE80211_BAND_5G][4],
			MinusStepNum[IEEE80211_BAND_5G][5],
			MinusStepNum[IEEE80211_BAND_5G][6],
			MinusStepNum[IEEE80211_BAND_5G][7]
			));
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] tssi gain/step = %u\n", pAd->TxPowerCtrl.TssiGain[IEEE80211_BAND_5G]));
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] Step = %u\n", Step[IEEE80211_BAND_5G]));
		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation 5G] RefTemp_2G = %d\n", pAd->TxPowerCtrl.RefTemp[IEEE80211_BAND_5G]));

		band_nums = IEEE80211_BAND_NUMS;
	}
#endif /* A_BAND_SUPPORT */


	for (band = IEEE80211_BAND_2G; band < band_nums; band++)
	{
		/* positive */
		i = 0;
		IdxTmp = 1;

		pAd->TxPowerCtrl.LookupTable[band][1 + Offset] = Step[band] / 2;
		pAd->TxPowerCtrl.LookupTable[band][0 + Offset] = pAd->TxPowerCtrl.LookupTable[band][1 + Offset] - Step[band];
		for (Idx = 2; Idx < 26;)/* Idx++ )*/
		{
			if (PlusStepNum[band][i] != 0 || i >= 8)
			{
				if (Idx >= IdxTmp + PlusStepNum[band][i] && i < 8)
				{
					pAd->TxPowerCtrl.LookupTable[band][Idx + Offset] = pAd->TxPowerCtrl.LookupTable[band][Idx - 1 + Offset] + (Step[band] - (i+1) + 1);
					IdxTmp = IdxTmp + PlusStepNum[band][i];
					i += 1;
				}
				else
				{
					pAd->TxPowerCtrl.LookupTable[band][Idx + Offset] = pAd->TxPowerCtrl.LookupTable[band][Idx - 1 + Offset] + (Step[band] - (i+1) + 1);
				}
				Idx++;
			}
			else
			{
				i += 1;
			}
		}

		/* negative */
		i = 0;
		IdxTmp = 1;
		for (Idx = 1; Idx < 8;)/* Idx++ )*/
		{
			if (MinusStepNum[band][i] != 0 || i >= 8)
			{
				if ((Idx + 1) >= IdxTmp + MinusStepNum[band][i] && i < 8)
				{
					pAd->TxPowerCtrl.LookupTable[band][-Idx + Offset] = pAd->TxPowerCtrl.LookupTable[band][-Idx + 1 + Offset] - (Step[band] + (i+1) - 1);
					IdxTmp = IdxTmp + MinusStepNum[band][i];
					i += 1;
				}
				else
				{
					pAd->TxPowerCtrl.LookupTable[band][-Idx + Offset] = pAd->TxPowerCtrl.LookupTable[band][-Idx + 1 + Offset] - (Step[band] + (i+1) - 1);
				}
				Idx++;
			}
			else
			{
				i += 1;
			}
		}

		DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation] Lookup table as below:\n"));
		for (Idx = 0; Idx < 33; Idx++)
		{
			DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation band(%d)] %d, %d\n", band, Idx - Offset, pAd->TxPowerCtrl.LookupTable[band][Idx]));
		}
	}
	
	/* Set BBP_R47 */
	RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R47, &BbpValue);
	BbpValue = (BbpValue & 0xf7);	/* bit3 = 0 */
	BbpValue = (BbpValue | 0x80);	/* bit7 = 1, bit4 = 0 */
	RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R47, BbpValue);
	
	/*  Set RF_R27 */
	RT30xxReadRFRegister(pAd, RF_R27, &RFValue);
	/* Set [7:6] to 01. For method 2, it is set at initialization. */
	RFValue = ((RFValue & 0x7f) | 0x40);
	RT30xxWriteRFRegister(pAd, RF_R27, RFValue);
	DBGPRINT(RT_DEBUG_TRACE, ("[temp. compensation] Set RF_R27 to 0x%x\n", RFValue));
}


VOID AsicGetAutoAgcOffsetForTemperatureSensor(
	IN PRTMP_ADAPTER 		pAd,
	IN PCHAR				pDeltaPwr,
	IN PCHAR				pTotalDeltaPwr,
	IN PCHAR				pAgcCompensate,
	IN PCHAR 				pDeltaPowerByBbpR1)
{
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;
	const TX_POWER_TUNING_ENTRY_STRUCT *TxPowerTuningTable;
	TX_POWER_TUNING_ENTRY_STRUCT *TxPowerTuningTableEntry0 = NULL; /* Ant0 */
	TX_POWER_TUNING_ENTRY_STRUCT *TxPowerTuningTableEntry1 = NULL; /* Ant1 */
	BBP_R49_STRUC	BbpR49;
	BOOLEAN			bAutoTxAgc = FALSE;
	PCHAR			pTxAgcCompensate = NULL;
	UCHAR 			RFValue = 0;
	CHAR			TuningTableUpperBound = 0, TuningTableIndex0 = 0, TuningTableIndex1 = 0;
	INT 				CurrentTemp = 0;
	INT RefTemp;
	INT *LookupTable;
	INT	LookupTableIndex = pAd->TxPowerCtrl.LookupTableIndex + TEMPERATURE_COMPENSATION_LOOKUP_TABLE_OFFSET;

	DBGPRINT(RT_DEBUG_INFO, ("-->%s\n", __FUNCTION__));
	
	BbpR49.byte = 0;
	*pTotalDeltaPwr = 0;

#ifdef A_BAND_SUPPORT
	if (pAd->CommonCfg.Channel > 14)
	{
		/* a band channel */
		bAutoTxAgc = pAd->bAutoTxAgcA;
		pTxAgcCompensate = &pAd->TxAgcCompensateA;
		TxPowerTuningTable = pChipCap->TxPowerTuningTable_5G;
		RefTemp = pAd->TxPowerCtrl.RefTemp[IEEE80211_BAND_5G];
		LookupTable = &pAd->TxPowerCtrl.LookupTable[IEEE80211_BAND_5G][0];
		TuningTableUpperBound = pChipCap->TxAlcTxPowerUpperBound_5G;
	}
	else
#endif /* A_BAND_SUPPORT */
	{
		/* bg band channel */
		bAutoTxAgc = pAd->bAutoTxAgcG;
		pTxAgcCompensate = &pAd->TxAgcCompensateG;
		TxPowerTuningTable = pChipCap->TxPowerTuningTable_2G;
		RefTemp = pAd->TxPowerCtrl.RefTemp[IEEE80211_BAND_2G];
		LookupTable = &pAd->TxPowerCtrl.LookupTable[IEEE80211_BAND_2G][0];
		TuningTableUpperBound = pChipCap->TxAlcTxPowerUpperBound_2G;
	}

	/* AutoTxAgc in EEPROM means temperature compensation enabled/diablded. */
	if (bAutoTxAgc)
	{ 
		/* Current temperature */
#ifdef RT65xx
		if (IS_RT65XX(pAd))
		{
			UINT32 bbp_val;	
			RTMP_BBP_IO_READ32(pAd, CORE_R35, &bbp_val);
			CurrentTemp = (UCHAR)(bbp_val & 0xff);
		}
		else
#endif /* RT65xx */
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpR49.byte);
			CurrentTemp = (CHAR)BbpR49.byte;
		}
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] BBP_R49 = %02x, current temp = %d\n", BbpR49.byte, CurrentTemp));
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] RefTemp = %d\n", RefTemp));
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] index = %d\n", pAd->TxPowerCtrl.LookupTableIndex));
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] f(%d)= %d\n", pAd->TxPowerCtrl.LookupTableIndex - 1, LookupTable[LookupTableIndex - 1]));
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] f(%d)= %d\n", pAd->TxPowerCtrl.LookupTableIndex, LookupTable[LookupTableIndex]));
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] f(%d)= %d\n", pAd->TxPowerCtrl.LookupTableIndex + 1, LookupTable[LookupTableIndex + 1]));
		if (CurrentTemp > RefTemp + LookupTable[LookupTableIndex + 1] + ((LookupTable[LookupTableIndex + 1] - LookupTable[LookupTableIndex]) >> 2) &&
			LookupTableIndex < 32)
		{
			DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] ++\n"));
			LookupTableIndex++;
			pAd->TxPowerCtrl.LookupTableIndex++;
		}
		else if (CurrentTemp < RefTemp + LookupTable[LookupTableIndex] - ((LookupTable[LookupTableIndex] - LookupTable[LookupTableIndex - 1]) >> 2) &&
			LookupTableIndex > 0)
		{
			DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] --\n"));
			LookupTableIndex--;
			pAd->TxPowerCtrl.LookupTableIndex--;
		}
		else
		{
			DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] ==\n"));
		}

		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] idxTxPowerTable=%d, idxTxPowerTable2=%d, TuningTableUpperBound=%d\n",
			pAd->TxPowerCtrl.idxTxPowerTable + pAd->TxPowerCtrl.LookupTableIndex,
			pAd->TxPowerCtrl.idxTxPowerTable2 + pAd->TxPowerCtrl.LookupTableIndex,
			TuningTableUpperBound));

		TuningTableIndex0 = pAd->TxPowerCtrl.idxTxPowerTable 
									+ pAd->TxPowerCtrl.LookupTableIndex 
#ifdef DOT11_N_SUPPORT
									+ pAd->TxPower[pAd->CommonCfg.CentralChannel-1].Power;
#else
									+ pAd->TxPower[pAd->CommonCfg.Channel-1].Power;
#endif /* DOT11_N_SUPPORT */
		/* The boundary verification */ 
		TuningTableIndex0 = (TuningTableIndex0 > TuningTableUpperBound) ? TuningTableUpperBound : TuningTableIndex0;
		TuningTableIndex0 = (TuningTableIndex0 < LOWERBOUND_TX_POWER_TUNING_ENTRY) ? 
							LOWERBOUND_TX_POWER_TUNING_ENTRY : TuningTableIndex0;
		TxPowerTuningTableEntry0 = &TxPowerTuningTable[TuningTableIndex0 + TX_POWER_TUNING_ENTRY_OFFSET];
		
		TuningTableIndex1 = pAd->TxPowerCtrl.idxTxPowerTable2 
									+ pAd->TxPowerCtrl.LookupTableIndex 
#ifdef DOT11_N_SUPPORT				
									+ pAd->TxPower[pAd->CommonCfg.CentralChannel-1].Power2;
#else
									+ pAd->TxPower[pAd->CommonCfg.Channel-1].Power2;
#endif /* DOT11_N_SUPPORT */
		/* The boundary verification */
		TuningTableIndex1 = (TuningTableIndex1 > TuningTableUpperBound) ? TuningTableUpperBound : TuningTableIndex1;
		TuningTableIndex1 = (TuningTableIndex1 < LOWERBOUND_TX_POWER_TUNING_ENTRY) ? 
							LOWERBOUND_TX_POWER_TUNING_ENTRY : TuningTableIndex1;
		TxPowerTuningTableEntry1 = &TxPowerTuningTable[TuningTableIndex1 + TX_POWER_TUNING_ENTRY_OFFSET];
			
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] (tx0)RF_TX_ALC = %x, MAC_PowerDelta = %d, TuningTableIndex = %d\n",
			TxPowerTuningTableEntry0->RF_TX_ALC, TxPowerTuningTableEntry0->MAC_PowerDelta, TuningTableIndex0));
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] (tx1)RF_TX_ALC = %x, MAC_PowerDelta = %d, TuningTableIndex = %d\n",
			TxPowerTuningTableEntry1->RF_TX_ALC, TxPowerTuningTableEntry1->MAC_PowerDelta, TuningTableIndex1));

		/* Update RF_R49 [0:5] */
		RT30xxReadRFRegister(pAd, RF_R49, &RFValue);
		RFValue = ((RFValue & ~0x3F) | TxPowerTuningTableEntry0->RF_TX_ALC);
		if ((RFValue & 0x3F) > 0x27) /* The valid range of the RF R49 (<5:0>tx0_alc<5:0>) is 0x00~0x27 */
		{
			RFValue = ((RFValue & ~0x3F) | 0x27);
		}
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] Update RF_R49[0:5] to 0x%x\n", TxPowerTuningTableEntry0->RF_TX_ALC));
		RT30xxWriteRFRegister(pAd, RF_R49, RFValue);

		/* Update RF_R50 [0:5] */
		RT30xxReadRFRegister(pAd, RF_R50, &RFValue);
		RFValue = ((RFValue & ~0x3F) | TxPowerTuningTableEntry1->RF_TX_ALC);
		if ((RFValue & 0x3F) > 0x27) /* The valid range of the RF R49 (<5:0>tx0_alc<5:0>) is 0x00~0x27 */
		{
			RFValue = ((RFValue & ~0x3F) | 0x27);
		}
		DBGPRINT(RT_DEBUG_INFO, ("[temp. compensation] Update RF_R50[0:5] to 0x%x\n", TxPowerTuningTableEntry1->RF_TX_ALC));
		RT30xxWriteRFRegister(pAd, RF_R50, RFValue);
		
		*pTotalDeltaPwr = TxPowerTuningTableEntry0->MAC_PowerDelta;
		
	}

	*pAgcCompensate = *pTxAgcCompensate;
	DBGPRINT(RT_DEBUG_INFO, ("<--%s\n", __FUNCTION__));
}
#endif /* RTMP_TEMPERATURE_COMPENSATION */


void AsicResetBBPAgent(RTMP_ADAPTER *pAd)
{
	BBP_CSR_CFG_STRUC BbpCsr;

	/* Still need to find why BBP agent keeps busy, but in fact, hardware
	 * still function ok. Now clear busy first. */
	/* IF chipOps.AsicResetBbpAgent == NULL, run "else" part */
	if (pAd->chipOps.AsicResetBbpAgent != NULL) {
		pAd->chipOps.AsicResetBbpAgent(pAd);
	} else if (pAd->chipCap.MCUType != ANDES) {
		DBGPRINT(RT_DEBUG_INFO, ("Reset BBP Agent busy bit.!! \n"));
		RTMP_IO_READ32(pAd, H2M_BBP_AGENT, &BbpCsr.word);
		BbpCsr.field.Busy = 0;
		RTMP_IO_WRITE32(pAd, H2M_BBP_AGENT, BbpCsr.word);
	}
	//RTMP_CHIP_ASIC_RESET_BBP_AGENT(pAd);
}


#ifdef CONFIG_STA_SUPPORT
/*
	==========================================================================
	Description:
		put PHY to sleep here, and set next wakeup timer. PHY doesn't not wakeup
		automatically. Instead, MCU will issue a TwakeUpInterrupt to host after
		the wakeup timer timeout. Driver has to issue a separate command to wake
		PHY up.

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSleepThenAutoWakeup(PRTMP_ADAPTER pAd, USHORT TbttNumToNextWakeUp)
{
	RTMP_STA_SLEEP_THEN_AUTO_WAKEUP(pAd, TbttNumToNextWakeUp);
}

/*
	==========================================================================
	Description:
		AsicForceWakeup() is used whenever manual wakeup is required
		AsicForceSleep() should only be used when not in INFRA BSS. When
		in INFRA BSS, we should use AsicSleepThenAutoWakeup() instead.
	==========================================================================
 */
#if 0 //JB removed
static void AsicForceSleep(PRTMP_ADAPTER pAd)
{
}
#endif

/*
	==========================================================================
	Description:
		AsicForceWakeup() is used whenever Twakeup timer (set via AsicSleepThenAutoWakeup)
		expired.

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	==========================================================================
 */
void AsicForceWakeup(PRTMP_ADAPTER pAd, BOOLEAN bFromTx)
{
	DBGPRINT(RT_DEBUG_INFO, ("--> AsicForceWakeup \n"));
	RTMP_STA_FORCE_WAKEUP(pAd, bFromTx);
}
#endif /* CONFIG_STA_SUPPORT */


/*
	==========================================================================
	Description:
		Set My BSSID

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
 /* CFG_TODO */
void AsicSetBssid(RTMP_ADAPTER *pAd, UCHAR *pBssid)
{
	ULONG		  Addr4;

	DBGPRINT(RT_DEBUG_TRACE, ("===> AsicSetBssid %x:%x:%x:%x:%x:%x\n",
				PRINT_MAC(pBssid)));
	
	Addr4 = (ULONG)(pBssid[0]) | 
			(ULONG)(pBssid[1] << 8)  | 
			(ULONG)(pBssid[2] << 16) |
			(ULONG)(pBssid[3] << 24);
	RTMP_IO_WRITE32(pAd, MAC_BSSID_DW0, Addr4);

#ifdef HDR_TRANS_SUPPORT
	RTMP_IO_WRITE32(pAd, HT_MAC_BSSID_DW0, Addr4);
#endif /* HDR_TRANS_SUPPORT */

	Addr4 = 0;
	/* always one BSSID in STA mode*/
	Addr4 = (ULONG)(pBssid[4]) | (ULONG)(pBssid[5] << 8);


	RTMP_IO_WRITE32(pAd, MAC_BSSID_DW1, Addr4);

#ifdef HDR_TRANS_SUPPORT
	/*
		point WCID MAC table to 0x1800
		This is for debug.
		But HDR_TRANS doesn't work if you remove it.
		Check after IC formal release.
	*/
	Addr4 |= 0x18000000;
	RTMP_IO_WRITE32(pAd, HT_MAC_BSSID_DW1, Addr4);
#endif /* HDR_TRANS_SUPPORT */

}


#ifdef DOT11_N_SUPPORT
/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */
BOOLEAN AsicSetRDG(RTMP_ADAPTER *pAd, BOOLEAN bEnable)
{
	TX_LINK_CFG_STRUC TxLinkCfg;
	UINT32 Data = 0;

	RTMP_IO_READ32(pAd, TX_LINK_CFG, &TxLinkCfg.word);
	TxLinkCfg.field.TxRDGEn =  (bEnable ? 1 : 0);
	RTMP_IO_WRITE32(pAd, TX_LINK_CFG, TxLinkCfg.word);

	RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);
	Data &= 0xFFFFFF00;
	if (bEnable) {
		Data |= 0x80;
	} else {
		/* For CWC test, change txop from 0x30 to 0x20 in TxBurst mode*/
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE)
			&& (pAd->CommonCfg.bEnableTxBurst == TRUE)
#ifdef DOT11_N_SUPPORT
			&& (pAd->MacTab.fAnyStationMIMOPSDynamic == FALSE)
#endif /* DOT11_N_SUPPORT */
		)
			Data |= 0x20;
	}
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);


	if (bEnable)
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);
	else
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE);

	return TRUE;
}

VOID AsicEnableRDG(
	IN PRTMP_ADAPTER pAd) 
{
	TX_LINK_CFG_STRUC	TxLinkCfg;
	UINT32				Data = 0;

	RTMP_IO_READ32(pAd, TX_LINK_CFG, &TxLinkCfg.word);
	TxLinkCfg.field.TxRDGEn = 1;
	RTMP_IO_WRITE32(pAd, TX_LINK_CFG, TxLinkCfg.word);

	RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);
	Data  &= 0xFFFFFF00;
	Data  |= 0x80;
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);
}

/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */
VOID AsicDisableRDG(
	IN PRTMP_ADAPTER pAd) 
{
	TX_LINK_CFG_STRUC	TxLinkCfg;
	UINT32				Data = 0;



	RTMP_IO_READ32(pAd, TX_LINK_CFG, &TxLinkCfg.word);
	TxLinkCfg.field.TxRDGEn = 0;
	RTMP_IO_WRITE32(pAd, TX_LINK_CFG, TxLinkCfg.word);

	RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);
	
	Data  &= 0xFFFFFF00;
	/*Data  |= 0x20;*/
#ifndef WIFI_TEST
	/*if ( pAd->CommonCfg.bEnableTxBurst )	*/
	/*	Data |= 0x60;  for performance issue not set the TXOP to 0*/
#endif
	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE) 
#ifdef DOT11_N_SUPPORT
		&& (pAd->MacTab.fAnyStationMIMOPSDynamic == FALSE)
#endif /* DOT11_N_SUPPORT */
	)
	{
		/* For CWC test, change txop from 0x30 to 0x20 in TxBurst mode*/
		if (pAd->CommonCfg.bEnableTxBurst)
		Data |= 0x20;
	}
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);

}
#endif /* DOT11_N_SUPPORT */

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicDisableSync(PRTMP_ADAPTER pAd)
{
	BCN_TIME_CFG_STRUC csr;

	DBGPRINT(RT_DEBUG_TRACE, ("--->Disable TSF synchronization\n"));

	/* 2003-12-20 disable TSF and TBTT while NIC in power-saving have side effect*/
	/*			  that NIC will never wakes up because TSF stops and no more */
	/*			  TBTT interrupts*/
	pAd->TbttTickCount = 0;
	RTMP_IO_READ32(pAd, BCN_TIME_CFG, &csr.word);
	csr.field.bBeaconGen = 0;
	csr.field.bTBTTEnable = 0;
	csr.field.TsfSyncMode = 0;
	csr.field.bTsfTicking = 0;
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr.word);
}

/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicEnableBssSync(PRTMP_ADAPTER pAd)
{
	BCN_TIME_CFG_STRUC csr;

	DBGPRINT(RT_DEBUG_TRACE, ("--->AsicEnableBssSync(INFRA mode)\n"));

	RTMP_IO_READ32(pAd, BCN_TIME_CFG, &csr.word);
/*	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, 0x00000000);*/
#ifdef CONFIG_AP_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd) {
		/* ASIC register in units of 1/16 TU*/
		csr.field.BeaconInterval = pAd->CommonCfg.BeaconPeriod << 4;
		csr.field.bTsfTicking = 1;
		csr.field.TsfSyncMode = 3; /* sync TSF similar as in ADHOC mode?*/
		csr.field.bBeaconGen  = 1; /* AP should generate BEACON*/
		csr.field.bTBTTEnable = 1;
	}
#endif /* CONFIG_AP_SUPPORT */
#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd) {
		/* ASIC register in units of 1/16 TU*/
		csr.field.BeaconInterval = pAd->CommonCfg.BeaconPeriod << 4; /* ASIC register in units of 1/16 TU*/
		csr.field.bTsfTicking = 1;
		csr.field.TsfSyncMode = 1; /* sync TSF in INFRASTRUCTURE mode*/
		csr.field.bBeaconGen  = 0; /* do NOT generate BEACON*/
		csr.field.bTBTTEnable = 1;
	}
#endif /* CONFIG_STA_SUPPORT */
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr.word);
}

/*CFG_TODO*/
void AsicEnableApBssSync(PRTMP_ADAPTER pAd)
{
	BCN_TIME_CFG_STRUC csr;

	DBGPRINT(RT_DEBUG_TRACE, ("--->AsicEnableBssSync(INFRA mode)\n"));

	RTMP_IO_READ32(pAd, BCN_TIME_CFG, &csr.word);

	/* ASIC register in units of 1/16 TU*/
	csr.field.BeaconInterval = pAd->CommonCfg.BeaconPeriod << 4;
	csr.field.bTsfTicking = 1;
	csr.field.TsfSyncMode = 3; /* sync TSF similar as in ADHOC mode?*/
	csr.field.bBeaconGen  = 1; /* AP should generate BEACON*/
	csr.field.bTBTTEnable = 1;

	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr.word);
}


/*
	==========================================================================
	Description:
	Note:
		BEACON frame in shared memory should be built ok before this routine
		can be called. Otherwise, a garbage frame maybe transmitted out every
		Beacon period.

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicEnableIbssSync(RTMP_ADAPTER *pAd)
{
	BCN_TIME_CFG_STRUC csr9;
	PUCHAR ptr;
	UINT i;
	ULONG beaconBaseLocation = 0;
	USHORT beaconLen = (USHORT) pAd->BeaconTxWI.TxWIMPDUByteCnt;
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	UINT32 longptr;

#ifdef RT_BIG_ENDIAN
	{
		TXWI_STRUC localTxWI;

		NdisMoveMemory((PUCHAR)&localTxWI, (PUCHAR)&pAd->BeaconTxWI, TXWISize);
		RTMPWIEndianChange(pAd, (PUCHAR)&localTxWI, TYPE_TXWI);
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
				beaconLen = (USHORT) localTxWI.TxWIMPDUByteCnt;
		}
	}
#endif /* RT_BIG_ENDIAN */

	DBGPRINT(RT_DEBUG_TRACE, ("--->AsicEnableIbssSync(TxWIMPDUByteCnt=%d, beaconLen=%d)\n", pAd->BeaconTxWI.TxWIMPDUByteCnt, beaconLen));


	DBGPRINT(RT_DEBUG_TRACE, ("--->AsicEnableIbssSync(ADHOC mode. TxWIMPDUByteCnt = %d)\n", pAd->BeaconTxWI.TxWIMPDUByteCnt));

	RTMP_IO_READ32(pAd, BCN_TIME_CFG, &csr9.word);
	csr9.field.bBeaconGen = 0;
	csr9.field.bTBTTEnable = 0;
	csr9.field.bTsfTicking = 0;
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr9.word);
	beaconBaseLocation = HW_BEACON_BASE0(pAd);


#ifdef RTMP_MAC_USB
	/* move BEACON TXD and frame content to on-chip memory*/
	ptr = (PUCHAR)&pAd->BeaconTxWI;
	for (i=0; i < TXWISize; i+=2)
	{
		longptr =  *ptr + (*(ptr+1)<<8);
		RTMP_CHIP_UPDATE_BEACON(pAd, HW_BEACON_BASE0(pAd) + i, longptr, 2);
		ptr += 2;
	}

	/* start right after the 16-byte TXWI field*/
	ptr = pAd->BeaconBuf;
	/*for (i=0; i< pAd->BeaconTxWI.TxWIMPDUByteCnt; i+=2)*/
	for (i=0; i< beaconLen; i+=2)
	{
		longptr =  *ptr + (*(ptr+1)<<8);
		RTMP_CHIP_UPDATE_BEACON(pAd, HW_BEACON_BASE0(pAd) + TXWISize + i, longptr, 2);
		ptr +=2;
	}
#endif /* RTMP_MAC_USB */


	
	/* For Wi-Fi faily generated beacons between participating stations. */
	/* Set TBTT phase adaptive adjustment step to 8us (default 16us)*/
	/* don't change settings 2006-5- by Jerry*/
	/*RTMP_IO_WRITE32(pAd, TBTT_SYNC_CFG, 0x00001010);*/
	
	/* start sending BEACON*/
	csr9.field.BeaconInterval = pAd->CommonCfg.BeaconPeriod << 4; /* ASIC register in units of 1/16 TU*/
	csr9.field.bTsfTicking = 1;
	/*
		(STA ad-hoc mode) Upon the reception of BEACON frame from associated BSS, 
		local TSF is updated with remote TSF only if the remote TSF is greater than local TSF
	*/
	csr9.field.TsfSyncMode = 2; /* sync TSF in IBSS mode*/
	csr9.field.bTBTTEnable = 1;
	csr9.field.bBeaconGen = 1;
	RTMP_IO_WRITE32(pAd, BCN_TIME_CFG, csr9.word);
}

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */
VOID AsicSetEdcaParm(
	IN PRTMP_ADAPTER pAd,
	IN PEDCA_PARM	 pEdcaParm)
{
	EDCA_AC_CFG_STRUC   Ac0Cfg, Ac1Cfg, Ac2Cfg, Ac3Cfg;
	AC_TXOP_CSR0_STRUC csr0;
	AC_TXOP_CSR1_STRUC csr1;
	AIFSN_CSR_STRUC    AifsnCsr;
	CWMIN_CSR_STRUC    CwminCsr;
	CWMAX_CSR_STRUC    CwmaxCsr;
	int i;

	Ac0Cfg.word = 0;
	Ac1Cfg.word = 0;
	Ac2Cfg.word = 0;
	Ac3Cfg.word = 0;
	if ((pEdcaParm == NULL) || (pEdcaParm->bValid == FALSE))
	{
		DBGPRINT(RT_DEBUG_TRACE,("AsicSetEdcaParm\n"));
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_WMM_INUSED);
		for (i=0; i<MAX_LEN_OF_MAC_TABLE; i++)
		{
			if (IS_ENTRY_CLIENT(&pAd->MacTab.Content[i]) || IS_ENTRY_APCLI(&pAd->MacTab.Content[i]))
				CLIENT_STATUS_CLEAR_FLAG(&pAd->MacTab.Content[i], fCLIENT_STATUS_WMM_CAPABLE);
		}

		/*========================================================*/
		/*      MAC Register has a copy .*/
		/*========================================================*/
/*#ifndef WIFI_TEST*/
		if( pAd->CommonCfg.bEnableTxBurst )		
		{
			/* For CWC test, change txop from 0x30 to 0x20 in TxBurst mode*/
			Ac0Cfg.field.AcTxop = 0x20; /* Suggest by John for TxBurst in HT Mode*/
		}
		else
			Ac0Cfg.field.AcTxop = 0;	/* QID_AC_BE*/
/*#else*/
/*		Ac0Cfg.field.AcTxop = 0;	 QID_AC_BE*/
/*#endif					*/
		Ac0Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac0Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac0Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Ac0Cfg.word);

		Ac1Cfg.field.AcTxop = 0;	/* QID_AC_BK*/
		Ac1Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac1Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac1Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, Ac1Cfg.word);

		if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B))
		{
			Ac2Cfg.field.AcTxop = 192;	/* AC_VI: 192*32us ~= 6ms*/
			Ac3Cfg.field.AcTxop = 96;	/* AC_VO: 96*32us  ~= 3ms*/
		}
		else
		{
			Ac2Cfg.field.AcTxop = 96;	/* AC_VI: 96*32us ~= 3ms*/
			Ac3Cfg.field.AcTxop = 48;	/* AC_VO: 48*32us ~= 1.5ms*/
		}
		Ac2Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac2Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac2Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, Ac2Cfg.word);
		Ac3Cfg.field.Cwmin = CW_MIN_IN_BITS;
		Ac3Cfg.field.Cwmax = CW_MAX_IN_BITS;
		Ac3Cfg.field.Aifsn = 2;
		RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, Ac3Cfg.word);

		/*========================================================*/
		/*      DMA Register has a copy too.*/
		/*========================================================*/
		csr0.field.Ac0Txop = 0;		/* QID_AC_BE*/
		csr0.field.Ac1Txop = 0;		/* QID_AC_BK*/
		RTMP_IO_WRITE32(pAd, WMM_TXOP0_CFG, csr0.word);
		if (WMODE_EQUAL(pAd->CommonCfg.PhyMode, WMODE_B))
		{
			csr1.field.Ac2Txop = 192;		/* AC_VI: 192*32us ~= 6ms*/
			csr1.field.Ac3Txop = 96;		/* AC_VO: 96*32us  ~= 3ms*/
		}
		else
		{
			csr1.field.Ac2Txop = 96;		/* AC_VI: 96*32us ~= 3ms*/
			csr1.field.Ac3Txop = 48;		/* AC_VO: 48*32us ~= 1.5ms*/
		}
		RTMP_IO_WRITE32(pAd, WMM_TXOP1_CFG, csr1.word);

		CwminCsr.word = 0;
		CwminCsr.field.Cwmin0 = CW_MIN_IN_BITS;
		CwminCsr.field.Cwmin1 = CW_MIN_IN_BITS;
		CwminCsr.field.Cwmin2 = CW_MIN_IN_BITS;
		CwminCsr.field.Cwmin3 = CW_MIN_IN_BITS;
		RTMP_IO_WRITE32(pAd, WMM_CWMIN_CFG, CwminCsr.word);

		CwmaxCsr.word = 0;
		CwmaxCsr.field.Cwmax0 = CW_MAX_IN_BITS;
		CwmaxCsr.field.Cwmax1 = CW_MAX_IN_BITS;
		CwmaxCsr.field.Cwmax2 = CW_MAX_IN_BITS;
		CwmaxCsr.field.Cwmax3 = CW_MAX_IN_BITS;
		RTMP_IO_WRITE32(pAd, WMM_CWMAX_CFG, CwmaxCsr.word);

		RTMP_IO_WRITE32(pAd, WMM_AIFSN_CFG, 0x00002222);

		NdisZeroMemory(&pAd->CommonCfg.APEdcaParm, sizeof(EDCA_PARM));

	}
	else
	{
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_WMM_INUSED);
		/*========================================================*/
		/*      MAC Register has a copy.*/
		/*========================================================*/
		
		/* Modify Cwmin/Cwmax/Txop on queue[QID_AC_VI], Recommend by Jerry 2005/07/27*/
		/* To degrade our VIDO Queue's throughput for WiFi WMM S3T07 Issue.*/
		
		/*pEdcaParm->Txop[QID_AC_VI] = pEdcaParm->Txop[QID_AC_VI] * 7 / 10;  rt2860c need this		*/

		Ac0Cfg.field.AcTxop =  pEdcaParm->Txop[QID_AC_BE];
		Ac0Cfg.field.Cwmin= pEdcaParm->Cwmin[QID_AC_BE];
		Ac0Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_BE];
		Ac0Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_BE]; /*+1;*/

		Ac1Cfg.field.AcTxop =  pEdcaParm->Txop[QID_AC_BK];
		Ac1Cfg.field.Cwmin = pEdcaParm->Cwmin[QID_AC_BK]; /*+2; */
		Ac1Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_BK];
		Ac1Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_BK]; /*+1;*/


		Ac2Cfg.field.AcTxop = (pEdcaParm->Txop[QID_AC_VI] * 6) / 10;
		{
		Ac2Cfg.field.Cwmin = pEdcaParm->Cwmin[QID_AC_VI];
		Ac2Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_VI];
		}
		/*sync with window 20110524*/
		Ac2Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_VI] + 1; /* 5.2.27 T6 Pass Tx VI+BE, but will impack 5.2.27/28 T7. Tx VI*/
		
#ifdef INF_AMAZON_SE
#endif /* INF_AMAZON_SE */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			/* Tuning for Wi-Fi WMM S06*/
			if (pAd->CommonCfg.bWiFiTest && 
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
				Ac2Cfg.field.Aifsn -= 1; 

			/* Tuning for TGn Wi-Fi 5.2.32*/
			/* STA TestBed changes in this item: conexant legacy sta ==> broadcom 11n sta*/
			if (STA_TGN_WIFI_ON(pAd) && 
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
			{
				Ac0Cfg.field.Aifsn = 3;
				Ac2Cfg.field.AcTxop = 5;
			}
			
		}
#endif /* CONFIG_STA_SUPPORT */

		Ac3Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_VO];
		Ac3Cfg.field.Cwmin = pEdcaParm->Cwmin[QID_AC_VO];
		Ac3Cfg.field.Cwmax = pEdcaParm->Cwmax[QID_AC_VO];
		Ac3Cfg.field.Aifsn = pEdcaParm->Aifsn[QID_AC_VO];

/*#ifdef WIFI_TEST*/
		if (pAd->CommonCfg.bWiFiTest)
		{
			if (Ac3Cfg.field.AcTxop == 102)
			{
			Ac0Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_BE] ? pEdcaParm->Txop[QID_AC_BE] : 10;
				Ac0Cfg.field.Aifsn  = pEdcaParm->Aifsn[QID_AC_BE]-1; /* AIFSN must >= 1 */
			Ac1Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_BK];
				Ac1Cfg.field.Aifsn  = pEdcaParm->Aifsn[QID_AC_BK];
			Ac2Cfg.field.AcTxop = pEdcaParm->Txop[QID_AC_VI];
			}
		}
/*#endif  WIFI_TEST */
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */


		RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Ac0Cfg.word);
		RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, Ac1Cfg.word);
		RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, Ac2Cfg.word);
		RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, Ac3Cfg.word);


		/*========================================================*/
		/*      DMA Register has a copy too.*/
		/*========================================================*/
		csr0.field.Ac0Txop = Ac0Cfg.field.AcTxop;
		csr0.field.Ac1Txop = Ac1Cfg.field.AcTxop;
		RTMP_IO_WRITE32(pAd, WMM_TXOP0_CFG, csr0.word);

		csr1.field.Ac2Txop = Ac2Cfg.field.AcTxop;
		csr1.field.Ac3Txop = Ac3Cfg.field.AcTxop;
		RTMP_IO_WRITE32(pAd, WMM_TXOP1_CFG, csr1.word);

		CwminCsr.word = 0;
		CwminCsr.field.Cwmin0 = pEdcaParm->Cwmin[QID_AC_BE];
		CwminCsr.field.Cwmin1 = pEdcaParm->Cwmin[QID_AC_BK];
		CwminCsr.field.Cwmin2 = pEdcaParm->Cwmin[QID_AC_VI];
#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
			CwminCsr.field.Cwmin3 = pEdcaParm->Cwmin[QID_AC_VO] - 1; /*for TGn wifi test*/
#endif /* CONFIG_STA_SUPPORT */
		RTMP_IO_WRITE32(pAd, WMM_CWMIN_CFG, CwminCsr.word);

		CwmaxCsr.word = 0;
		CwmaxCsr.field.Cwmax0 = pEdcaParm->Cwmax[QID_AC_BE];
		CwmaxCsr.field.Cwmax1 = pEdcaParm->Cwmax[QID_AC_BK];
		CwmaxCsr.field.Cwmax2 = pEdcaParm->Cwmax[QID_AC_VI];
		CwmaxCsr.field.Cwmax3 = pEdcaParm->Cwmax[QID_AC_VO];
		RTMP_IO_WRITE32(pAd, WMM_CWMAX_CFG, CwmaxCsr.word);

		AifsnCsr.word = 0;
		AifsnCsr.field.Aifsn0 = Ac0Cfg.field.Aifsn; /*pEdcaParm->Aifsn[QID_AC_BE];*/
		AifsnCsr.field.Aifsn1 = Ac1Cfg.field.Aifsn; /*pEdcaParm->Aifsn[QID_AC_BK];*/
#ifdef CONFIG_STA_SUPPORT
#ifdef RTMP_MAC_USB
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			if(pAd->Antenna.field.TxPath == 1)
				AifsnCsr.field.Aifsn1 = Ac1Cfg.field.Aifsn + 2; 	/*5.2.27 T7 Pass*/
		}
#endif /* RTMP_MAC_USB */
#endif /* CONFIG_STA_SUPPORT */
		AifsnCsr.field.Aifsn2 = Ac2Cfg.field.Aifsn; /*pEdcaParm->Aifsn[QID_AC_VI];*/
#ifdef INF_AMAZON_SE
#endif /* INF_AMAZON_SE */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			/* Tuning for Wi-Fi WMM S06*/
			if (pAd->CommonCfg.bWiFiTest &&
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
				AifsnCsr.field.Aifsn2 = Ac2Cfg.field.Aifsn - 4;

			/* Tuning for TGn Wi-Fi 5.2.32*/
			/* STA TestBed changes in this item: connexant legacy sta ==> broadcom 11n sta*/
			if (STA_TGN_WIFI_ON(pAd) && 
				pEdcaParm->Aifsn[QID_AC_VI] == 10)
			{
				AifsnCsr.field.Aifsn0 = 3;
				AifsnCsr.field.Aifsn2 = 7;
			}

			if (INFRA_ON(pAd))
				CLIENT_STATUS_SET_FLAG(&pAd->MacTab.Content[BSSID_WCID], fCLIENT_STATUS_WMM_CAPABLE);
		}
#endif /* CONFIG_STA_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
		{
			AifsnCsr.field.Aifsn3 = Ac3Cfg.field.Aifsn - 1; /*pEdcaParm->Aifsn[QID_AC_VO]; for TGn wifi test*/
		}
#endif /* CONFIG_STA_SUPPORT */
		RTMP_IO_WRITE32(pAd, WMM_AIFSN_CFG, AifsnCsr.word);

		NdisMoveMemory(&pAd->CommonCfg.APEdcaParm, pEdcaParm, sizeof(EDCA_PARM));
		if (!ADHOC_ON(pAd))
		{
			DBGPRINT(RT_DEBUG_TRACE,("EDCA [#%d]: AIFSN CWmin CWmax  TXOP(us)  ACM\n", pEdcaParm->EdcaUpdateCount));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_BE      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[0],
									 pEdcaParm->Cwmin[0],
									 pEdcaParm->Cwmax[0],
									 pEdcaParm->Txop[0]<<5,
									 pEdcaParm->bACM[0]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_BK      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[1],
									 pEdcaParm->Cwmin[1],
									 pEdcaParm->Cwmax[1],
									 pEdcaParm->Txop[1]<<5,
									 pEdcaParm->bACM[1]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_VI      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[2],
									 pEdcaParm->Cwmin[2],
									 pEdcaParm->Cwmax[2],
									 pEdcaParm->Txop[2]<<5,
									 pEdcaParm->bACM[2]));
			DBGPRINT(RT_DEBUG_TRACE,("     AC_VO      %2d     %2d     %2d      %4d     %d\n",
									 pEdcaParm->Aifsn[3],
									 pEdcaParm->Cwmin[3],
									 pEdcaParm->Cwmax[3],
									 pEdcaParm->Txop[3]<<5,
									 pEdcaParm->bACM[3]));
		}

	}

	pAd->CommonCfg.RestoreBurstMode = Ac0Cfg.word;
}

/*
	==========================================================================
	Description:

	IRQL = PASSIVE_LEVEL
	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */
VOID 	AsicSetSlotTime(
	IN PRTMP_ADAPTER pAd,
	IN BOOLEAN bUseShortSlotTime) 
{
	ULONG	SlotTime;
	UINT32	RegValue = 0;

#ifdef CONFIG_STA_SUPPORT
	if (pAd->CommonCfg.Channel > 14)
		bUseShortSlotTime = TRUE;
#endif /* CONFIG_STA_SUPPORT */

	if (bUseShortSlotTime && OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED))
		return;
	else if ((!bUseShortSlotTime) && (!OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED)))
		return;

	if (bUseShortSlotTime)
		OPSTATUS_SET_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
	else
		OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);

	SlotTime = (bUseShortSlotTime)? 9 : 20;

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		/* force using short SLOT time for FAE to demo performance when TxBurst is ON*/
		if (((pAd->StaActive.SupportedPhyInfo.bHtEnable == FALSE) && (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_WMM_INUSED)))
#ifdef DOT11_N_SUPPORT
			|| ((pAd->StaActive.SupportedPhyInfo.bHtEnable == TRUE) && (pAd->CommonCfg.BACapability.field.Policy == BA_NOTUSE))
#endif /* DOT11_N_SUPPORT */
			)
		{
			/* In this case, we will think it is doing Wi-Fi test*/
			/* And we will not set to short slot when bEnableTxBurst is TRUE.*/
		}
		else if (pAd->CommonCfg.bEnableTxBurst)
		{
			OPSTATUS_SET_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
			SlotTime = 9;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	
	/* For some reasons, always set it to short slot time.*/
	/* ToDo: Should consider capability with 11B*/
#ifdef CONFIG_STA_SUPPORT 
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{
		if (pAd->StaCfg.BssType == BSS_ADHOC)	
		{
			OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_SLOT_INUSED);
			SlotTime = 20;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

	RTMP_IO_READ32(pAd, BKOFF_SLOT_CFG, &RegValue);
	RegValue = RegValue & 0xFFFFFF00;

	RegValue |= SlotTime;

	RTMP_IO_WRITE32(pAd, BKOFF_SLOT_CFG, RegValue);
}

/*
	========================================================================
	Description:
		Add Shared key information into ASIC.
		Update shared key, TxMic and RxMic to Asic Shared key table
		Update its cipherAlg to Asic Shared key Mode.

	Return:
	========================================================================
*/
void AsicAddSharedKeyEntry(PRTMP_ADAPTER pAd, UCHAR BssIndex, UCHAR KeyIdx,
	PCIPHER_KEY pCipherKey)
{
	ULONG offset;
	SHAREDKEY_MODE_STRUC csr1;
	UCHAR org_bssindex;

	PUCHAR pKey = pCipherKey->Key;
	PUCHAR pTxMic = pCipherKey->TxMic;
	PUCHAR pRxMic = pCipherKey->RxMic;
	UCHAR CipherAlg = pCipherKey->CipherAlg;

	DBGPRINT(RT_DEBUG_TRACE,
			("AsicAddSharedKeyEntry BssIndex=%d, KeyIdx=%d\n",
			BssIndex,KeyIdx));

	DBGPRINT(RT_DEBUG_TRACE,("AsicAddSharedKeyEntry: %s key #%d\n",
			CipherName[CipherAlg], BssIndex*4 + KeyIdx));
	DBGPRINT(RT_DEBUG_TRACE,
			("  Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pKey[0], pKey[1], pKey[2], pKey[3], pKey[4], pKey[5],
			pKey[6], pKey[7], pKey[8], pKey[9], pKey[10], pKey[11],
			pKey[12], pKey[13], pKey[14], pKey[15]));
	if (pRxMic) {
		DBGPRINT(RT_DEBUG_TRACE,
				("  Rx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				pRxMic[0], pRxMic[1], pRxMic[2], pRxMic[3],
				pRxMic[4], pRxMic[5], pRxMic[6], pRxMic[7]));
	}
	if (pTxMic) {
		DBGPRINT(RT_DEBUG_TRACE,
				("  Tx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				pTxMic[0], pTxMic[1], pTxMic[2], pTxMic[3],
				pTxMic[4], pTxMic[5], pTxMic[6], pTxMic[7]));
	}

	org_bssindex = BssIndex;
	if (BssIndex >= 8)
	{
		SharedKeyTableBase = SHARED_KEY_TABLE_BASE_EXT;
		SharedKeyModeBase = SHARED_KEY_MODE_BASE_EXT;
		BssIndex -= 8;

	{
		/* fill key material - key + TX MIC + RX MIC*/
		UINT32 share_key_base = 0, share_key_size = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			if (org_bssindex >= 8)
				share_key_base = RLT_SHARED_KEY_TABLE_BASE_EXT;
			else
			share_key_base = RLT_SHARED_KEY_TABLE_BASE;
			share_key_size = RLT_HW_KEY_ENTRY_SIZE;
		}
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			if (org_bssindex >= 8)
				share_key_base = SHARED_KEY_TABLE_BASE_EXT;
			else
			share_key_base = SHARED_KEY_TABLE_BASE;
			share_key_size = HW_KEY_ENTRY_SIZE;
		}
#endif /* RTMP_MAC */

		offset = share_key_base + (4*BssIndex + KeyIdx)*share_key_size;

#ifdef RTMP_MAC_USB
		{
			RTUSBMultiWrite(pAd, offset, pKey, MAX_LEN_OF_SHARE_KEY, FALSE);

			offset += MAX_LEN_OF_SHARE_KEY;
			if (pTxMic) {
				RTUSBMultiWrite(pAd, offset, pTxMic, 8, FALSE);
			}

			offset += 8;
			if (pRxMic) {
				RTUSBMultiWrite(pAd, offset, pRxMic, 8, FALSE);
			}
		}
#endif /* RTMP_MAC_USB */
	}

	{
		UINT32 share_key_mode_base = 0;
#ifdef RLT_MAC
		if (pAd->chipCap.hif_type == HIF_RLT) {
			if (org_bssindex >= 8)
				share_key_mode_base = RLT_SHARED_KEY_MODE_BASE_EXT;
			else
			share_key_mode_base= RLT_SHARED_KEY_MODE_BASE;
		}
#endif /* RLT_MAC */
#ifdef RTMP_MAC
		if (pAd->chipCap.hif_type == HIF_RTMP) {
			if (org_bssindex >= 8)
				share_key_mode_base = SHARED_KEY_MODE_BASE_EXT;
			else
			share_key_mode_base = SHARED_KEY_MODE_BASE;
		}
#endif /* RTMP_MAC */

		/* Update cipher algorithm. WSTA always use BSS0*/
		RTMP_IO_READ32(pAd, share_key_mode_base + 4 * (BssIndex/2), &csr1.word);
		DBGPRINT(RT_DEBUG_TRACE,
				("Read: SHARED_KEY_MODE_BASE at this Bss[%d] KeyIdx[%d]= 0x%x \n",
				BssIndex,KeyIdx, csr1.word));
		if ((BssIndex%2) == 0) {
			if (KeyIdx == 0)
				csr1.field.Bss0Key0CipherAlg = CipherAlg;
			else if (KeyIdx == 1)
				csr1.field.Bss0Key1CipherAlg = CipherAlg;
			else if (KeyIdx == 2)
				csr1.field.Bss0Key2CipherAlg = CipherAlg;
			else
				csr1.field.Bss0Key3CipherAlg = CipherAlg;
		} else {
			if (KeyIdx == 0)
				csr1.field.Bss1Key0CipherAlg = CipherAlg;
			else if (KeyIdx == 1)
				csr1.field.Bss1Key1CipherAlg = CipherAlg;
			else if (KeyIdx == 2)
				csr1.field.Bss1Key2CipherAlg = CipherAlg;
			else
				csr1.field.Bss1Key3CipherAlg = CipherAlg;
		}
		DBGPRINT(RT_DEBUG_TRACE,
				("Write: SHARED_KEY_MODE_BASE at this Bss[%d] = 0x%x \n",
				BssIndex, csr1.word));
		RTMP_IO_WRITE32(pAd, share_key_mode_base+ 4 * (BssIndex/2), csr1.word);
	}
}


/* IRQL = DISPATCH_LEVEL*/
void AsicRemoveSharedKeyEntry(PRTMP_ADAPTER pAd, UCHAR BssIndex, UCHAR KeyIdx)
{
	/*ULONG SecCsr0;*/
	SHAREDKEY_MODE_STRUC csr1;
	UINT32 share_key_mode_base = 0;

	DBGPRINT(RT_DEBUG_TRACE,("AsicRemoveSharedKeyEntry: #%d \n",
			BssIndex*4 + KeyIdx));

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		share_key_mode_base= RLT_SHARED_KEY_MODE_BASE;
#endif
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		share_key_mode_base = SHARED_KEY_MODE_BASE;
#endif

	RTMP_IO_READ32(pAd, share_key_mode_base+4*(BssIndex/2), &csr1.word);
	if ((BssIndex%2) == 0) {
		if (KeyIdx == 0)
			csr1.field.Bss0Key0CipherAlg = 0;
		else if (KeyIdx == 1)
			csr1.field.Bss0Key1CipherAlg = 0;
		else if (KeyIdx == 2)
			csr1.field.Bss0Key2CipherAlg = 0;
		else
			csr1.field.Bss0Key3CipherAlg = 0;
	} else {
		if (KeyIdx == 0)
			csr1.field.Bss1Key0CipherAlg = 0;
		else if (KeyIdx == 1)
			csr1.field.Bss1Key1CipherAlg = 0;
		else if (KeyIdx == 2)
			csr1.field.Bss1Key2CipherAlg = 0;
		else
			csr1.field.Bss1Key3CipherAlg = 0;
	}
	DBGPRINT(RT_DEBUG_TRACE,
			("Write: SHARED_KEY_MODE_BASE at this Bss[%d] = 0x%x \n",
			BssIndex, csr1.word));
	RTMP_IO_WRITE32(pAd, share_key_mode_base+4*(BssIndex/2), csr1.word);

	ASSERT(BssIndex < 4);
	ASSERT(KeyIdx < 4);
}


void AsicUpdateWCIDIVEIV(PRTMP_ADAPTER pAd, USHORT WCID, ULONG uIV, ULONG uEIV)
{
	ULONG offset;
	UINT32 iveiv_tb_base = 0, iveiv_tb_size = 0;

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		iveiv_tb_base = RLT_MAC_IVEIV_TABLE_BASE;
		iveiv_tb_size = RLT_HW_IVEIV_ENTRY_SIZE;
	}
#endif /* RLT_MAC */

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		iveiv_tb_base = MAC_IVEIV_TABLE_BASE;
		iveiv_tb_size = HW_IVEIV_ENTRY_SIZE;
	}
#endif /* RTMP_MAC */

	offset = iveiv_tb_base + (WCID * iveiv_tb_size);

	RTMP_IO_WRITE32(pAd, offset, uIV);
	RTMP_IO_WRITE32(pAd, offset + 4, uEIV);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: wcid(%d) 0x%08lx, 0x%08lx \n",
			__FUNCTION__, WCID, uIV, uEIV));
}


void AsicUpdateRxWCIDTable(PRTMP_ADAPTER pAd, USHORT WCID, PUCHAR pAddr)
{
	ULONG offset;
	ULONG Addr;

	offset = MAC_WCID_BASE + (WCID * HW_WCID_ENTRY_SIZE);
	Addr = pAddr[0] + (pAddr[1] << 8) +(pAddr[2] << 16) +(pAddr[3] << 24);
	RTMP_IO_WRITE32(pAd, offset, Addr);
	Addr = pAddr[4] + (pAddr[5] << 8);
	RTMP_IO_WRITE32(pAd, offset + 4, Addr);
}


/*
	========================================================================
	Description:
		Add Client security information into ASIC WCID table and IVEIV table.
	Return:

	Note :
		The key table selection rule :
		1.	Wds-links and Mesh-links always use Pair-wise key table.
		2. 	When the CipherAlg is TKIP, AES, SMS4 or the dynamic WEP is enabled,
			it needs to set key into Pair-wise Key Table.
		3.	The pair-wise key security mode is set NONE, it means as no security.
		4.	In STA Adhoc mode, it always use shared key table.
		5.	Otherwise, use shared key table

	========================================================================
*/
void AsicUpdateWcidAttributeEntry(PRTMP_ADAPTER pAd, UCHAR BssIdx, UCHAR KeyIdx,
	UCHAR CipherAlg, UINT8 Wcid, UINT8 KeyTabFlag)
{
	WCID_ATTRIBUTE_STRUC WCIDAttri;
	USHORT offset;
	UINT32 wcid_attr_base = 0, wcid_attr_size = 0;
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		wcid_attr_base = RLT_MAC_WCID_ATTRIBUTE_BASE;
		wcid_attr_size = RLT_HW_WCID_ATTRI_SIZE;
	}
#endif
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		wcid_attr_base = MAC_WCID_ATTRIBUTE_BASE;
		wcid_attr_size = HW_WCID_ATTRI_SIZE;
	}
#endif
	/* Initialize the content of WCID Attribue  */
	WCIDAttri.word = 0;

	/* The limitation of HW WCID table */
	if (Wcid > 254) {
		DBGPRINT(RT_DEBUG_WARN, ("%s:Invalid wcid(%d)\n", __FUNCTION__,
				Wcid));
		return;
	}

	/* Update the pairwise key security mode.
	   Use bit10 and bit3~1 to indicate the pairwise cipher mode */
	WCIDAttri.field.PairKeyModeExt = ((CipherAlg & 0x08) >> 3);
	WCIDAttri.field.PairKeyMode = (CipherAlg & 0x07);

	/* Update the MBSS index.
	   Use bit11 and bit6~4 to indicate the BSS index */
	WCIDAttri.field.BSSIdxExt = ((BssIdx & 0x08) >> 3);
	WCIDAttri.field.BSSIdx = (BssIdx & 0x07);

	/* Assign Key Table selection */
	WCIDAttri.field.KeyTab = KeyTabFlag;

	/* Update related information to ASIC */
	offset = wcid_attr_base + (Wcid * wcid_attr_size);
	RTMP_IO_WRITE32(pAd, offset, WCIDAttri.word);

	DBGPRINT(RT_DEBUG_TRACE,
			("%s:WCID #%d, KeyIdx #%d, WCIDAttri=0x%x, Alg=%s\n",
			__FUNCTION__, Wcid, KeyIdx, WCIDAttri.word,
			CipherName[CipherAlg]));
}


/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicDelWcidTab(RTMP_ADAPTER *pAd, UCHAR wcid_idx)
{
	UINT32 offset;
	UCHAR cnt, cnt_s, cnt_e;
#ifdef MCS_LUT_SUPPORT
	UINT32 mcs_tb_offset = 0;
#endif /* MCS_LUT_SUPPORT */

	DBGPRINT(RT_DEBUG_TRACE, ("AsicDelWcidTab==>wcid_idx = 0x%x\n",wcid_idx));
	if (wcid_idx == WCID_ALL) {
		cnt_s = 0;
		cnt_e = (WCID_ALL - 1);
	} else {
#ifdef MCS_LUT_SUPPORT
		if (RTMP_TEST_MORE_FLAG(pAd, fASIC_CAP_MCS_LUT))
			mcs_tb_offset = 0x400;
#endif /* MCS_LUT_SUPPORT */
		cnt_s = cnt_e = wcid_idx;
	}

	for (cnt = cnt_s; cnt_s <= cnt_e; cnt_s++) {
		offset = MAC_WCID_BASE + cnt * HW_WCID_ENTRY_SIZE;
		RTMP_IO_WRITE32(pAd, offset, 0x0);
		RTMP_IO_WRITE32(pAd, offset + 4, 0x0);
#ifdef MCS_LUT_SUPPORT
		if (mcs_tb_offset) {
			offset += mcs_tb_offset;
			RTMP_IO_WRITE32(pAd, offset, 0x0);
			RTMP_IO_WRITE32(pAd, offset + 4, 0x0);
		}
#endif /* MCS_LUT_SUPPORT */
	}
}


/*
	========================================================================
	Description:
		Add Pair-wise key material into ASIC.
		Update pairwise key, TxMic and RxMic to Asic Pair-wise key table

	Return:
	========================================================================
*/
void AsicAddPairwiseKeyEntry(PRTMP_ADAPTER pAd, UCHAR WCID, PCIPHER_KEY pCipherKey)
{
	int i;
	ULONG offset;
	UINT32 pairwise_key_base = 0, pairwise_key_len = 0;
	UCHAR *pTxMic = pCipherKey->TxMic;
	UCHAR *pRxMic = pCipherKey->RxMic;
#ifdef DBG
	UCHAR *pKey = pCipherKey->Key;
	UCHAR CipherAlg = pCipherKey->CipherAlg;
#endif

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		pairwise_key_base = RLT_PAIRWISE_KEY_TABLE_BASE;
		pairwise_key_len = RLT_HW_KEY_ENTRY_SIZE;
	}
#endif
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP) {
		pairwise_key_base = PAIRWISE_KEY_TABLE_BASE;
		pairwise_key_len = HW_KEY_ENTRY_SIZE;
	}
#endif

	/* EKEY */
	offset = pairwise_key_base + (WCID * pairwise_key_len);
#ifdef RTMP_MAC_USB
	RTUSBMultiWrite(pAd, offset, &pCipherKey->Key[0], MAX_LEN_OF_PEER_KEY, FALSE);
#endif /* RTMP_MAC_USB */
	for (i = 0; i < MAX_LEN_OF_PEER_KEY; i += 4) {
		UINT32 Value;
		RTMP_IO_READ32(pAd, offset + i, &Value);
	}
	offset += MAX_LEN_OF_PEER_KEY;

	/*  MIC KEY */
	if (pTxMic) {
#ifdef RTMP_MAC_USB
		RTUSBMultiWrite(pAd, offset, &pCipherKey->TxMic[0], 8, FALSE);
#endif /* RTMP_MAC_USB */
	}
	offset += 8;

	if (pRxMic) {
#ifdef RTMP_MAC_USB
		RTUSBMultiWrite(pAd, offset, &pCipherKey->RxMic[0], 8, FALSE);
#endif /* RTMP_MAC_USB */
	}

	DBGPRINT(RT_DEBUG_TRACE,("AsicAddPairwiseKeyEntry: WCID #%d Alg=%s\n",
			WCID, CipherName[CipherAlg]));
	DBGPRINT(RT_DEBUG_TRACE,
			(" Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
			pKey[0], pKey[1], pKey[2], pKey[3], pKey[4], pKey[5],
			pKey[6], pKey[7], pKey[8], pKey[9], pKey[10], pKey[11],
			pKey[12], pKey[13], pKey[14], pKey[15]));
	if (pRxMic) {
		DBGPRINT(RT_DEBUG_TRACE,
				(" Rx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				pRxMic[0], pRxMic[1], pRxMic[2], pRxMic[3],
				pRxMic[4], pRxMic[5], pRxMic[6], pRxMic[7]));
	}
	if (pTxMic) {
		DBGPRINT(RT_DEBUG_TRACE,
				(" Tx MIC Key = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
				pTxMic[0], pTxMic[1], pTxMic[2], pTxMic[3],
				pTxMic[4], pTxMic[5], pTxMic[6], pTxMic[7]));
	}
}


/*
	========================================================================
	Description:
		Remove Pair-wise key material from ASIC.

	Return:
	========================================================================
*/
void AsicRemovePairwiseKeyEntry(PRTMP_ADAPTER pAd, UCHAR Wcid)
{
	/* Set the specific WCID attribute entry as OPEN-NONE */
	AsicUpdateWcidAttributeEntry(pAd, BSS0, 0, CIPHER_NONE, Wcid,
			PAIRWISEKEYTABLE);
}


BOOLEAN AsicSendCommandToMcu(RTMP_ADAPTER *pAd, UCHAR Command, UCHAR Token,
	UCHAR Arg0, UCHAR Arg1, BOOLEAN in_atomic)
{
#ifdef RT65xx
	// TODO: shiang-6590, fix me, currently firmware is not ready yet, so ignore it!
	if (IS_RT65XX(pAd))
		return TRUE;
#endif /* RT65xx */

	if (pAd->chipOps.sendCommandToMcu)
		return pAd->chipOps.sendCommandToMcu(pAd, Command, Token, Arg0,
				Arg1, in_atomic);
	else
		return FALSE;
}


BOOLEAN AsicSendCmdToMcuAndWait(RTMP_ADAPTER *pAd, UCHAR Command, UCHAR Token,
	UCHAR Arg0, UCHAR Arg1, BOOLEAN in_atomic)
{
	AsicSendCommandToMcu(pAd, Command, Token, Arg0, Arg1, in_atomic);
	return TRUE;
}

#if 0 //JB removed
static BOOLEAN AsicSendCommandToMcuBBP(PRTMP_ADAPTER pAd, UCHAR Command,
	UCHAR Token, UCHAR Arg0, UCHAR Arg1, BOOLEAN FlgIsNeedLocked)
{
#ifdef RT65xx
	// TODO: shiang-6590, fix me, currently firmware is not ready yet, so ignore it!
	if (IS_RT65XX(pAd))
		return TRUE;
#endif /* RT65xx */

	if (pAd->chipOps.sendCommandToMcu)
		return pAd->chipOps.sendCommandToMcu(pAd, Command, Token, Arg0,
				Arg1, FlgIsNeedLocked);
	else
		return FALSE;
}
#endif

/*
	========================================================================
	Description:
		For 1x1 chipset : 2070 / 3070 / 3090 / 3370 / 3390 / 5370 / 5390
		Usage :	1. Set Default Antenna as initialize
				2. Antenna Diversity switching used
				3. iwpriv command switch Antenna

	Return:
	========================================================================
 */
void AsicSetRxAnt(PRTMP_ADAPTER pAd, UCHAR Ant)
{
	if (pAd->chipOps.SetRxAnt)
		pAd->chipOps.SetRxAnt(pAd, Ant);
}


void AsicTurnOffRFClk(PRTMP_ADAPTER pAd, UCHAR Channel)
{
	if (pAd->chipOps.AsicRfTurnOff) {
		pAd->chipOps.AsicRfTurnOff(pAd);
	} else {
#if defined(RT28xx) || defined(RT2880) || defined(RT2883)
		/* RF R2 bit 18 = 0*/
		UINT32 R1 = 0, R2 = 0, R3 = 0;
		UCHAR index;
		RTMP_RF_REGS *RFRegTable;

		RFRegTable = RF2850RegTable;
#endif /* defined(RT28xx) || defined(RT2880) || defined(RT2883) */

		switch (pAd->RfIcType) {
#if defined(RT28xx) || defined(RT2880) || defined(RT2883)
#if defined(RT28xx) || defined(RT2880)
		case RFIC_2820:
		case RFIC_2850:
		case RFIC_2720:
		case RFIC_2750:
#endif /* defined(RT28xx) || defined(RT2880) */
			for (index = 0; index < NUM_OF_2850_CHNL; index++) {
				if (Channel == RFRegTable[index].Channel){
					R1 = RFRegTable[index].R1 & 0xffffdfff;
					R2 = RFRegTable[index].R2 & 0xfffbffff;
					R3 = RFRegTable[index].R3 & 0xfff3ffff;

					RTMP_RF_IO_WRITE32(pAd, R1);
					RTMP_RF_IO_WRITE32(pAd, R2);

					/* Program R1b13 to 1, R3/b18,19 to 0, R2b18 to 0. */
					/* Set RF R2 bit18=0, R3 bit[18:19]=0*/
					/*if (pAd->StaCfg.bRadio == FALSE)*/
					if (1) {
						RTMP_RF_IO_WRITE32(pAd, R3);

						DBGPRINT(RT_DEBUG_TRACE,
								("AsicTurnOffRFClk#%d(RF=%d, ) , R2=0x%08x,  R3 = 0x%08x \n",
								Channel,
								pAd->RfIcType, R2, R3));
					} else {
						DBGPRINT(RT_DEBUG_TRACE,
								("AsicTurnOffRFClk#%d(RF=%d, ) , R2=0x%08x \n",
								Channel,
								pAd->RfIcType, R2));
					}
					break;
				}
			}
			break;
#endif /* defined(RT28xx) || defined(RT2880) || defined(RT2883) */
		default:
			DBGPRINT(RT_DEBUG_TRACE, ("AsicTurnOffRFClk#%d : Unkonwn RFIC=%d\n",
					Channel, pAd->RfIcType));
			break;
		}
	}
}

#ifdef STREAM_MODE_SUPPORT
// StreamModeRegVal - return MAC reg value for StreamMode setting
UINT32 StreamModeRegVal(RTMP_ADAPTER *pAd)
{
	UINT32 streamWord;

	switch (pAd->CommonCfg.StreamMode) {
	case 1:
		streamWord = 0x030000;
		break;
	case 2:
		streamWord = 0x0c0000;
		break;
	case 3:
		streamWord = 0x0f0000;
		break;
	default:
		streamWord = 0x0;
		break;
	}

	return streamWord;
}


/*
	========================================================================
	Description:
		configure the stream mode of specific MAC or all MAC and set to ASIC.

	Prameters:
		pAd		 ---
		pMacAddr ---
		bClear	 --- disable the stream mode for specific macAddr when
				(pMacAddr!=NULL)

	Return:
	========================================================================
*/
void AsicSetStreamMode(RTMP_ADAPTER *pAd, PUCHAR pMacAddr, int chainIdx,
	BOOLEAN bEnabled)
{
	UINT32 streamWord;
	UINT32 regAddr, regVal;

	if (!pAd->chipCap.FlgHwStreamMode)
		return;

	streamWord = StreamModeRegVal(pAd);

	if (!bEnabled)
		streamWord = 0;

	regAddr = TX_CHAIN_ADDR0_L + chainIdx * 8;
	RTMP_IO_WRITE32(pAd, regAddr,
			(UINT32)(pMacAddr[0]) |
			(UINT32)(pMacAddr[1] << 8)  |
			(UINT32)(pMacAddr[2] << 16) |
			(UINT32)(pMacAddr[3] << 24));

	RTMP_IO_READ32(pAd, regAddr + 4, &regVal);
	regVal &= (~0x000f0000);
	RTMP_IO_WRITE32(pAd, regAddr + 4,
			(regVal | streamWord) |
			(UINT32)(pMacAddr[4]) |
			(UINT32)(pMacAddr[5] << 8));
}


void RtmpStreamModeInit(RTMP_ADAPTER *pAd)
{
	int chainIdx;
	UCHAR *pMacAddr;

	if (pAd->chipCap.FlgHwStreamMode == FALSE)
		return;

	for (chainIdx = 0; chainIdx < STREAM_MODE_STA_NUM; chainIdx++) {
		pMacAddr = &pAd->CommonCfg.StreamModeMac[chainIdx][0];
		AsicSetStreamMode(pAd, pMacAddr, chainIdx, TRUE);
	}
}
#endif // STREAM_MODE_SUPPORT //


#ifdef DOT11_N_SUPPORT
BOOLEAN AsicSetRalinkBurstMode(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
	UINT32 Data = 0;

	RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);
	if (enable) {
		pAd->CommonCfg.RestoreBurstMode = Data;
		Data  &= 0xFFF00000;
		Data  |= 0x86380;
	} else {
		Data = pAd->CommonCfg.RestoreBurstMode;
		Data &= 0xFFFFFF00;

		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE)
#ifdef DOT11_N_SUPPORT
			&& (pAd->MacTab.fAnyStationMIMOPSDynamic == FALSE)
#endif // DOT11_N_SUPPORT //
		)
		{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE))
			Data |= 0x80;
		else if (pAd->CommonCfg.bEnableTxBurst)
			Data |= 0x20;
		}
	}
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);

	if (enable)
		RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_RALINK_BURST_MODE);
	else
		RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_RALINK_BURST_MODE);

	return TRUE;
}
/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */
VOID AsicEnableRalinkBurstMode(
	IN PRTMP_ADAPTER pAd) 
{
	UINT32				Data = 0;

	RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);
	pAd->CommonCfg.RestoreBurstMode = Data;
	Data  &= 0xFFF00000;
	Data  |= 0x86380;
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);
}

/*
	==========================================================================
	Description:

	IRQL = DISPATCH_LEVEL
	
	==========================================================================
 */

VOID AsicDisableRalinkBurstMode(
	IN PRTMP_ADAPTER pAd) 
{
	UINT32				Data = 0;

	RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &Data);

	Data = pAd->CommonCfg.RestoreBurstMode;
	Data &= 0xFFFFFF00;

	if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_DYNAMIC_BE_TXOP_ACTIVE) 
#ifdef DOT11_N_SUPPORT
		&& (pAd->MacTab.fAnyStationMIMOPSDynamic == FALSE)
#endif // DOT11_N_SUPPORT //
	)
	{
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RDG_ACTIVE))
			Data |= 0x80;
		else if (pAd->CommonCfg.bEnableTxBurst)
			Data |= 0x20;
	}
	RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, Data);
}
#endif // DOT11_N_SUPPORT //


#ifdef WOW_SUPPORT
#ifdef RTMP_MAC_USB

/* switch firmware
   a) before into WOW mode, switch firmware to WOW-enable firmware
   b) exit from WOW mode, switch firmware to normal firmware
*/
void AsicLoadWOWFirmware(PRTMP_ADAPTER pAd, BOOLEAN WOW)
{
	if (WOW)
		pAd->WOW_Cfg.bWOWFirmware = TRUE;
	else
		pAd->WOW_Cfg.bWOWFirmware = FALSE;

	RtmpAsicLoadFirmware(pAd);
}

/* In WOW mode, 8051 mcu will send null frame, and pick data from 0x7780
 * the null frame includes TxWI and 802.11 header */
void AsicWOWSendNullFrame(PRTMP_ADAPTER pAd, UCHAR TxRate, BOOLEAN bQosNull)
{
	TXWI_STRUC *TxWI;
	PUCHAR NullFrame;
	UINT8  packet_len;
	PUCHAR ptr;
	USHORT offset;
	UINT32 cipher = pAd->StaCfg.GroupCipher;
	UINT32 Value;
	UINT8 TXWISize = pAd->chipCap.TXWISize;
	UINT32 share_key_mode_base = 0;

	ComposeNullFrame(pAd);
	TxWI = (TXWI_STRUC *)&pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE];
	NullFrame = (PUCHAR)&pAd->NullFrame;
#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT) {
		packet_len = TxWI->TXWI_N.MPDUtotalByteCnt;
	}
#endif

#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		packet_len = TxWI->TXWI_O.MPDUtotalByteCnt;
#endif

	DBGPRINT(RT_DEBUG_OFF, ("TxWI:\n"));
	/* copy TxWI to MCU memory */
	ptr = (PUCHAR)TxWI;
	for (offset = 0; offset < TXWISize; offset += 4) {
		RTMPMoveMemory(&Value, ptr+offset, 4);
		DBGPRINT(RT_DEBUG_OFF, ("offset: %02d %08x\n", offset, Value));
		RTMP_IO_WRITE32(pAd, HW_NULL2_BASE + offset, Value);
	}

	DBGPRINT(RT_DEBUG_OFF, ("802.11 header:\n"));
	/* copy 802.11 header to memory */
	ptr = (PUCHAR)NullFrame;
	for (offset = 0; offset < packet_len; offset += 4) {
		RTMPMoveMemory(&Value, ptr+offset, 4);
		DBGPRINT(RT_DEBUG_OFF, ("offset: %02d %08x\n", offset, Value));
		RTMP_IO_WRITE32(pAd, HW_NULL2_BASE + TXWISize + offset, Value);
	}

	DBGPRINT(RT_DEBUG_OFF, ("Write GroupCipher Mode: %d\n",
			pAd->StaCfg.GroupCipher));

#ifdef RLT_MAC
	if (pAd->chipCap.hif_type == HIF_RLT)
		share_key_mode_base = RLT_SHARED_KEY_MODE_BASE;
#endif /* RLT_MAC */
#ifdef RTMP_MAC
	if (pAd->chipCap.hif_type == HIF_RTMP)
		share_key_mode_base = SHARED_KEY_MODE_BASE;
#endif /* RTMP_MAC */

	RTMP_IO_READ32(pAd, share_key_mode_base, &Value);
	switch (cipher) {
		 /* don't care WEP, because it dosen't have re-key issue */
	case Ndis802_11TKIPEnable: /* TKIP */
		Value |= 0x0330;
		break;
	case Ndis802_11AESEnable: /* AES */
		Value |= 0x0440;
		break;
	}
	RTMP_IO_WRITE32(pAd, share_key_mode_base, Value);
}
#endif /* RTMP_MAC_USB */
#endif /* WOW_SUPPORT */


BOOLEAN AsicSetPreTbttInt(RTMP_ADAPTER *pAd, BOOLEAN enable)
{
	UINT32 val;

	RTMP_IO_READ32(pAd, INT_TIMER_CFG, &val);
	val &= 0xffff0000;
	val |= 6 << 4; /* Pre-TBTT is 6ms before TBTT interrupt. 1~10 ms is reasonable. */
	RTMP_IO_WRITE32(pAd, INT_TIMER_CFG, val);
	/* Enable pre-tbtt interrupt */
	RTMP_IO_READ32(pAd, INT_TIMER_EN, &val);
	val |= 0x1;
	RTMP_IO_WRITE32(pAd, INT_TIMER_EN, val);

	return TRUE;
}


BOOLEAN AsicWaitPDMAIdle(struct _RTMP_ADAPTER *pAd, int round, int wait_us)
{
	int i = 0;
	WPDMA_GLO_CFG_STRUC GloCfg;

	do {
		RTMP_IO_READ32(pAd, WPDMA_GLO_CFG, &GloCfg.word);
		if ((GloCfg.field.TxDMABusy == 0) && (GloCfg.field.RxDMABusy == 0)) {
			DBGPRINT(RT_DEBUG_TRACE, ("==>  DMAIdle, GloCfg=0x%x\n",
					GloCfg.word));
			return TRUE;
		}
		if (RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST))
			return FALSE;
		RTMPusecDelay(wait_us);
	}while ((i++) < round);

	DBGPRINT(RT_DEBUG_TRACE, ("==>  DMABusy, GloCfg=0x%x\n", GloCfg.word));

	return FALSE;
}


#ifdef DOT11_N_SUPPORT
#ifdef MT76x2
#define MAX_AGG_CNT	48
#elif defined(RT65xx) || defined(MT7601)
#define MAX_AGG_CNT	32
#elif defined(RT2883) || defined(RT3883)
#define MAX_AGG_CNT	16
#else
#define MAX_AGG_CNT	8
#endif
BOOLEAN AsicReadAggCnt(RTMP_ADAPTER *pAd, ULONG *aggCnt, int cnt_len)
{
	UINT32 reg_addr;
	TX_AGG_CNT_STRUC reg_val;
	int i, cnt, seg;
	static USHORT aggReg[] = {
		TX_AGG_CNT, TX_AGG_CNT3,
#if MAX_AGG_CNT > 8
		TX_AGG_CNT4, TX_AGG_CNT7,
#endif
#if MAX_AGG_CNT > 16
		TX_AGG_CNT8, TX_AGG_CNT15,
#endif
#if MAX_AGG_CNT > 32
		TX_AGG_CNT16, TX_AGG_CNT23,
#endif
	};

	NdisZeroMemory(aggCnt, cnt_len * sizeof(ULONG));
	seg = (sizeof(aggReg) /sizeof(USHORT));

	cnt = 0;
	for (i = 0; i < seg; i += 2) {
		for (reg_addr = aggReg[i] ; reg_addr <= aggReg[i+1] ; reg_addr += 4) {
			RTMP_IO_READ32(pAd, reg_addr, &reg_val.word);
			if (cnt < (cnt_len -1)) {
				aggCnt[cnt] = reg_val.field.AggCnt_x;
				aggCnt[cnt+1] = reg_val.field.AggCnt_y;
				DBGPRINT(RT_DEBUG_TRACE,
						("%s():Get AggSize at Reg(0x%x) with val(0x%08x) [AGG_%d=>%ld, AGG_%d=>%ld]\n",
						__FUNCTION__, reg_addr,
						reg_val.word, cnt, aggCnt[cnt],
						cnt+1, aggCnt[cnt+1]));
				cnt += 2;
			} else {
				DBGPRINT(RT_DEBUG_TRACE,
						("%s():Get AggSize at Reg(0x%x) failed, no enough buffer(cnt_len=%d, cnt=%d)\n",
						__FUNCTION__, reg_addr, cnt_len,
						cnt));
			}
		}
	}

	return TRUE;
}

#endif /* DOT11_N_SUPPORT */


int AsicSetChannel(RTMP_ADAPTER *pAd, UCHAR ch, UINT8 bw, UINT8 ext_ch, BOOLEAN bScan)
{
	rtmp_bbp_set_bw(pAd, bw);

	/*  Tx/RX : control channel setting */
	rtmp_bbp_set_ctrlch(pAd, ext_ch);
	rtmp_mac_set_ctrlch(pAd, ext_ch);

	/* Let BBP register at 20MHz to do scan */
	AsicSwitchChannel(pAd, ch, bScan);
	AsicLockChannel(pAd, ch);

#ifdef RT28xx
	RT28xx_ch_tunning(pAd, bw);
#endif /* RT28xx */

	return 0;
}

#ifdef RLT_MAC
/*
	==========================================================================
	Description:
		Set BSSID of Root AP

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
void AsicSetApCliBssid(PRTMP_ADAPTER pAd, PUCHAR pBssid, UCHAR index)
{
	UINT32 Addr4 = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("%s():%x:%x:%x:%x:%x:%x\n", __FUNCTION__,
			PRINT_MAC(pBssid)));

	Addr4 = (UINT32)(pBssid[0]) |
			(UINT32)(pBssid[1] << 8)  |
			(UINT32)(pBssid[2] << 16) |
			(UINT32)(pBssid[3] << 24);
	RTMP_IO_WRITE32(pAd, MAC_APCLI_BSSID_DW0, Addr4);

	Addr4 = 0;
	Addr4 = (ULONG)(pBssid[4]) | (ULONG)(pBssid[5] << 8);
	/* Enable APCLI mode */
	Addr4 |= 0x10000;

	RTMP_IO_WRITE32(pAd, MAC_APCLI_BSSID_DW1, Addr4);
}

#ifdef MAC_REPEATER_SUPPORT
/*
	==========================================================================
	Description:
		Set Extended MAC Address

	IRQL = DISPATCH_LEVEL

	==========================================================================
 */
VOID AsicSetExtendedMacAddr(
	IN PRTMP_ADAPTER pAd, 
	IN PUCHAR pMacAddr,
	IN UINT32 Idx) 
{
	UINT32		  Addr4 = 0;
	
	DBGPRINT(RT_DEBUG_TRACE, ("===> AsicSetExtendedMacAddr %x:%x:%x:%x:%x:%x\n",
				PRINT_MAC(pMacAddr)));
	
	Addr4 = (UINT32)(pMacAddr[0]) | 
			(UINT32)(pMacAddr[1] << 8)  | 
			(UINT32)(pMacAddr[2] << 16) |
			(UINT32)(pMacAddr[3] << 24);
	RTMP_IO_WRITE32(pAd, MAC_ADDR_EXT0_31_0 + (Idx*8), Addr4);

	Addr4 = 0;
	Addr4 = (ULONG)(pMacAddr[4]) | (ULONG)(pMacAddr[5] << 8);
	/* Enable APCLI mode */
	Addr4 |= 0x10000;

	RTMP_IO_WRITE32(pAd, MAC_ADDR_EXT0_47_32 + (Idx*8), Addr4);
}
#endif /* MAC_REPEATER_SUPPORT */
#endif /* RLT_MAC */

#ifdef NEW_WOW_SUPPORT
void RT28xxAndesWOWEnable(PRTMP_ADAPTER pAd)
{
	NEW_WOW_MASK_CFG_STRUCT mask_cfg;
	NEW_WOW_SEC_CFG_STRUCT sec_cfg;
	NEW_WOW_INFRA_CFG_STRUCT infra_cfg;
	NEW_WOW_P2P_CFG_STRUCT p2p_cfg;
	NEW_WOW_PARAM_STRUCT wow_param;
	struct CMD_UNIT CmdUnit;
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;
	INT32 Ret;
	MAC_TABLE_ENTRY *pEntry = NULL;

	NdisZeroMemory(&CmdUnit, sizeof(CmdUnit));

	/* WOW enable */
	NdisZeroMemory(&wow_param, sizeof(wow_param));

	wow_param.Parameter = WOW_ENABLE; /* WOW enable */
	wow_param.Value = TRUE;

	CmdUnit.u.ANDES.Type = CMD_WOW_FEATURE; /* feature enable */
	CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_PARAM_STRUCT);
	CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&wow_param;

	Ret = AsicSendCmdToAndes(pAd, &CmdUnit);

	if (Ret != NDIS_STATUS_SUCCESS) {
		printk("\x1b[31m%s: send WOW config command failed(%d/%d)!!\x1b[m\n",
				__FUNCTION__, CmdUnit.u.ANDES.Type, wow_param.Parameter);
		return;
	}

	RtmpOsMsDelay(1);
	/* mask configuration */
	NdisZeroMemory(&mask_cfg, sizeof(mask_cfg));

	mask_cfg.Config_Type = WOW_MASK_CFG; 	/* detect mask config */
	mask_cfg.Function_Enable = TRUE;
	mask_cfg.Detect_Mask = 1UL << WOW_MAGIC_PKT;	/* magic packet */
	mask_cfg.Event_Mask = 0;

	CmdUnit.u.ANDES.Type = CMD_WOW_CONFIG; /* WOW config */
	CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_MASK_CFG_STRUCT);
	CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&mask_cfg;

	Ret = AsicSendCmdToAndes(pAd, &CmdUnit);

	if (Ret != NDIS_STATUS_SUCCESS) {
		printk("\x1b[31m%s: send WOW config command failed!!(%d/%d)\x1b[m\n",
				__FUNCTION__, CmdUnit.u.ANDES.Type,
				mask_cfg.Config_Type);
		return;
	}

	RtmpOsMsDelay(1);

	/* security configuration */
	if (pAd->StaCfg.AuthMode >= Ndis802_11AuthModeWPAPSK) {
		NdisZeroMemory(&sec_cfg, sizeof(sec_cfg));
		sec_cfg.Config_Type = WOW_SEC_CFG; 	/* security config */

		if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPAPSK)
			sec_cfg.WPA_Ver = 0;
		else if (pAd->StaCfg.AuthMode == Ndis802_11AuthModeWPA2PSK)
			sec_cfg.WPA_Ver = 1;

		pEntry = &pAd->MacTab.Content[BSSID_WCID];

		NdisCopyMemory(sec_cfg.PTK, pEntry->PTK, 64);
		NdisCopyMemory(sec_cfg.R_COUNTER, pEntry->R_Counter, LEN_KEY_DESC_REPLAY);

		sec_cfg.Key_Id = pAd->StaCfg.DefaultKeyId;
		sec_cfg.Cipher_Alg = pEntry->WepStatus;
		printk("\x1b[31m%s: wep status %d\x1b[m\n", __FUNCTION__, pEntry->WepStatus);
		sec_cfg.Group_Cipher = pAd->StaCfg.GroupCipher;
		printk("\x1b[31m%s: group status %d\x1b[m\n", __FUNCTION__, sec_cfg.Group_Cipher);
		printk("\x1b[31m%s: aid %d\x1b[m\n", __FUNCTION__, pEntry->Aid);
		sec_cfg.WCID = BSSID_WCID;

		CmdUnit.u.ANDES.Type = CMD_WOW_CONFIG; /* WOW config */
		CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_SEC_CFG_STRUCT);
		CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&sec_cfg;

		Ret = AsicSendCmdToAndes(pAd, &CmdUnit);

		if (Ret != NDIS_STATUS_SUCCESS) {
			printk("\x1b[31m%s: send WOW config command failed(%d/%d)!!\x1b[m\n",
					__FUNCTION__, CmdUnit.u.ANDES.Type,
					sec_cfg.Config_Type);
			return;
		}

		RtmpOsMsDelay(1);
	}

	/* Infra configuration */

	NdisZeroMemory(&infra_cfg, sizeof(infra_cfg));

	infra_cfg.Config_Type = WOW_INFRA_CFG; 	/* infra config */

	COPY_MAC_ADDR(infra_cfg.STA_MAC, pAd->CurrentAddress);
	COPY_MAC_ADDR(infra_cfg.AP_MAC, pAd->CommonCfg.Bssid);

	CmdUnit.u.ANDES.Type = CMD_WOW_CONFIG; /* WOW config */
	CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_INFRA_CFG_STRUCT);
	CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&infra_cfg;

	if (OPSTATUS_TEST_FLAG(pAd, fOP_STATUS_MEDIA_STATE_CONNECTED))
		infra_cfg.AP_Status = TRUE;
	else
		infra_cfg.AP_Status = FALSE;

	Ret = AsicSendCmdToAndes(pAd, &CmdUnit);

	if (Ret != NDIS_STATUS_SUCCESS) {
		printk("\x1b[31m%s: send WOW config command failed(%d/%d)!!\x1b[m\n",
				__FUNCTION__, CmdUnit.u.ANDES.Type,
				infra_cfg.Config_Type);
		return;
	}

	RtmpOsMsDelay(1);


	/* P2P configuration */

	/* Wakeup Option */
	NdisZeroMemory(&wow_param, sizeof(wow_param));

	wow_param.Parameter = WOW_WAKEUP; /* Wakeup Option */
	if (pAd->WOW_Cfg.bInBand) {
		wow_param.Value = WOW_WAKEUP_BY_USB;
	} else {
		INT32 Value;

		wow_param.Value = WOW_WAKEUP_BY_GPIO;

		RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &Value);
		printk("\x1b[31m%s: 0x80 = %x\x1b[m\n", __FUNCTION__, Value);
		Value &= ~0x01010000; /* GPIO0(ouput) --> 0(data) */
		RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, Value);
	}

	CmdUnit.u.ANDES.Type = CMD_WOW_FEATURE; /* feature enable */
	CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_PARAM_STRUCT);
	CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&wow_param;

	Ret = AsicSendCmdToAndes(pAd, &CmdUnit);

	if (Ret != NDIS_STATUS_SUCCESS) {
		printk("\x1b[31m%s: send WOW config command failed(%d/%d)!!\x1b[m\n",
				__FUNCTION__, CmdUnit.u.ANDES.Type, wow_param.Parameter);
		return;
	}

	RtmpOsMsDelay(1);

	/* traffic to Andes */
	NdisZeroMemory(&wow_param, sizeof(wow_param));
	wow_param.Parameter = WOW_TRAFFIC; /* Traffic switch */
	wow_param.Value = WOW_PKT_TO_ANDES;	/* incoming packet to FW */

	CmdUnit.u.ANDES.Type = CMD_WOW_FEATURE; /* feature enable */
	CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_PARAM_STRUCT);
	CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&wow_param.Parameter;

	Ret = AsicSendCmdToAndes(pAd, &CmdUnit);

	if (Ret != NDIS_STATUS_SUCCESS) {
		printk("\x1b[31m%s: send WOW config command failed(%d/%d)!!\x1b[m\n",
				__FUNCTION__, CmdUnit.u.ANDES.Type,
				wow_param.Parameter);
		return;
	}

	RtmpOsMsDelay(1);

	RTMP_SET_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);
}

void RT28xxAndesWOWDisable(PRTMP_ADAPTER pAd)
{
	NEW_WOW_PARAM_STRUCT param;
	struct CMD_UNIT CmdUnit;
	RTMP_CHIP_CAP *pChipCap = &pAd->chipCap;
	INT32 Ret;
	UINT32 Value;
	MAC_TABLE_ENTRY *pEntry = NULL;

	printk("\x1b[31m%s: ...\x1b[m", __FUNCTION__);

	/* clean BulkIn Reset flag */
	//pAd->Flags &= ~0x80000;
	RTMP_CLEAR_FLAG(pAd, fRTMP_ADAPTER_IDLE_RADIO_OFF);

	NdisZeroMemory(&CmdUnit, sizeof(CmdUnit));

	/* WOW disable */
	NdisZeroMemory(&param, sizeof(param));
	param.Parameter = WOW_ENABLE;
	param.Value = FALSE;

	CmdUnit.u.ANDES.Type = CMD_WOW_FEATURE; /* WOW enable */
	CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_PARAM_STRUCT);
	CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&param;

	Ret = AsicSendCmdToAndes(pAd, &CmdUnit);

	if (Ret != NDIS_STATUS_SUCCESS) {
		printk("\x1b[31m%s: send WOW config command failed!!\x1b[m\n",
				__FUNCTION__);
		return;
	}

	RtmpOsMsDelay(1);

	/* traffic to Host */
	NdisZeroMemory(&param, sizeof(param));
	param.Parameter = WOW_TRAFFIC;
	param.Value = WOW_PKT_TO_HOST;

	CmdUnit.u.ANDES.Type = CMD_WOW_FEATURE;
	CmdUnit.u.ANDES.CmdPayloadLen = sizeof(NEW_WOW_PARAM_STRUCT);
	CmdUnit.u.ANDES.CmdPayload = (PUCHAR)&param;

	Ret = AsicSendCmdToAndes(pAd, &CmdUnit);
	if (Ret != NDIS_STATUS_SUCCESS) {
		printk("\x1b[31m%s: send WOW config command failed!!\x1b[m\n",
				__FUNCTION__);
		return;
	}

	RtmpOsMsDelay(1);

	/* Restore MAC TX/RX */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &Value);
	Value |= 0xC;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, Value);

	RTUSBBulkReceive(pAd);
	RTUSBBulkCmdRspEventReceive(pAd);

	//printk("\x1b[31m%s: pendingRx %d\x1b[m\n", __FUNCTION__, pAd->PendingRx);
	//printk("\x1b[31m%s: BulkInReq %d\x1b[m\n", __FUNCTION__, pAd->BulkInReq);

	/* restore hardware remote wakeup flag */
	RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &Value);
	printk("\x1b[31m%s: 0x80 %08x\x1b[m\n", __FUNCTION__, Value);
	Value &= ~0x80;
	RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, Value);

	if (pAd->WOW_Cfg.bInBand == FALSE) {
		INT32 Value;

		RTMP_IO_READ32(pAd, WLAN_FUN_CTRL, &Value);
		printk("\x1b[31m%s: 0x80 = %x\x1b[m\n", __FUNCTION__, Value);
		Value &= ~0x01010000; /* GPIO0(ouput) --> 0(data) */
		RTMP_IO_WRITE32(pAd, WLAN_FUN_CTRL, Value);
	}
}

#endif /* NEW_WOW_SUPPORT */

#ifdef THERMAL_PROTECT_SUPPORT
void thermal_protection(RTMP_ADAPTER *pAd)
{
	RTMP_CHIP_OP *pChipOps = &pAd->chipOps;
	INT32 temp_diff = 0, current_temp = 0;

	if (pAd->chipCap.ThermalProtectSup == FALSE)
		return;

	if (pAd->chipOps.ChipGetCurrentTemp != NULL) \
		pCurrentTemp = pAd->chipOps.ChipGetCurrentTemp(pAd);

	temp_diff = current_temp - pAd->last_thermal_pro_temp;
	pAd->last_thermal_pro_temp = current_temp;

	DBGPRINT(RT_DEBUG_INFO, ("%s - current temp=%d, temp diff=%d\n",
			__FUNCTION__, current_temp, temp_diff));

	if (temp_diff > 0) {
		if (current_temp > (pAd->thermal_pro_criteria + 10) /* 90 */)
			RTMP_CHIP_THERMAL_PRO_2nd_COND(pAd);
		else if (current_temp > pAd->thermal_pro_criteria /* 80 */)
			RTMP_CHIP_THERMAL_PRO_1st_COND(pAd);
		else
			RTMP_CHIP_THERMAL_PRO_DEFAULT_COND(pAd);
	} else if (temp_diff < 0) {
		if (current_temp < (pAd->thermal_pro_criteria - 5) /* 75 */)
			RTMP_CHIP_THERMAL_PRO_DEFAULT_COND(pAd);
		else if (current_temp < (pAd->thermal_pro_criteria + 5) /* 85 */)
			RTMP_CHIP_THERMAL_PRO_1st_COND(pAd);
		else
			RTMP_CHIP_THERMAL_PRO_2nd_COND(pAd);
	}
}
#endif /* THERMAL_PROTECT_SUPPORT */

#ifdef DROP_MASK_SUPPORT
void asic_set_drop_mask(PRTMP_ADAPTER ad, USHORT wcid, BOOLEAN enable)
{
	UINT32 mac_reg = 0, reg_id, group_index;
	UINT32 drop_mask = (1 << (wcid % 32));

	/* each group has 32 entries */
	group_index = (wcid - (wcid % 32)) >> 5 /* divided by 32 */;
	reg_id = (TX_WCID_DROP_MASK0 + 4*group_index);

	RTMP_IO_READ32(ad, reg_id, &mac_reg);

	mac_reg = (enable ? \
				(mac_reg | drop_mask):(mac_reg & ~drop_mask));
	RTMP_IO_WRITE32(ad, reg_id, mac_reg);
	DBGPRINT(RT_DEBUG_TRACE,
			("%s(%u):, wcid = %u, reg_id = 0x%08x, mac_reg = 0x%08x,"
			" group_index = %u, drop_mask = 0x%08x\n",
			__FUNCTION__, enable, wcid, reg_id, mac_reg,
			group_index, drop_mask));
}


void asic_drop_mask_reset(PRTMP_ADAPTER ad)
{
	UINT32 i, reg_id;

	for (i = 0; i < 8 /* num of drop mask group */; i++) {
		reg_id = (TX_WCID_DROP_MASK0 + i*4);
		RTMP_IO_WRITE32(ad, reg_id, 0);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("%s()\n", __FUNCTION__));
}
#endif /* DROP_MASK_SUPPORT */

#ifdef MULTI_CLIENT_SUPPORT
void asic_change_tx_retry(PRTMP_ADAPTER pAd, USHORT num)
{
	UINT32	TxRtyCfg, MacReg = 0;

	if (pAd->CommonCfg.txRetryCfg == 0) {
		/* txRetryCfg is invalid, should not be 0 */
		DBGPRINT(RT_DEBUG_TRACE, ("txRetryCfg=%x\n",
				pAd->CommonCfg.txRetryCfg));
		return ;
	}

	if (num < 3) {
		/* Tx date retry default 15 */
		RTMP_IO_READ32(pAd, TX_RTY_CFG, &TxRtyCfg);
		TxRtyCfg = ((TxRtyCfg & 0xffff0000) |
				(pAd->CommonCfg.txRetryCfg & 0x0000ffff));
		RTMP_IO_WRITE32(pAd, TX_RTY_CFG, TxRtyCfg);

		/* Tx RTS retry default 32 */
		RTMP_IO_READ32(pAd, TX_RTS_CFG, &MacReg);
		MacReg &= 0xFEFFFF00;
		MacReg |= 0x20;
		RTMP_IO_WRITE32(pAd, TX_RTS_CFG, MacReg);
	} else {
		/* Tx date retry 10 */
		TxRtyCfg = 0x4100080A;
		RTMP_IO_WRITE32(pAd, TX_RTY_CFG, TxRtyCfg);

		/* Tx RTS retry 3 */
		RTMP_IO_READ32(pAd, TX_RTS_CFG, &MacReg);
		MacReg &= 0xFEFFFF00;
		MacReg |= 0x01000003;
		RTMP_IO_WRITE32(pAd, TX_RTS_CFG, MacReg);

		/* enable fallback legacy */
		if (pAd->CommonCfg.Channel > 14)
			RTMP_IO_WRITE32(pAd, HT_FBK_TO_LEGACY, 0x1818);
		else
			RTMP_IO_WRITE32(pAd, HT_FBK_TO_LEGACY, 0x1010);
	}
}

void pkt_aggr_num_change(PRTMP_ADAPTER pAd, USHORT num)
{
	if (IS_RT6352(pAd)) {
		if (num < 5) {
			/* use default */
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_20M1S, 0x77777777);
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_20M2S, 0x77777777);
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_40M1S, 0x77777777);
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_40M2S, 0x77777777);
		} else {
			/* modify by MCS */
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_20M1S, 0x77754433);
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_20M2S, 0x77765543);
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_40M1S, 0x77765544);
			RTMP_IO_WRITE32(pAd, AMPDU_MAX_LEN_40M2S, 0x77765544);
		}
	}
}

void asic_tune_be_wmm(PRTMP_ADAPTER pAd, USHORT num)
{
	UCHAR  bssCwmin = 4, apCwmin = 4, apCwmax = 6;

	if (num <= 4) {
		/* use profile cwmin */
		if (pAd->CommonCfg.APCwmin > 0 && pAd->CommonCfg.BSSCwmin > 0
			&& pAd->CommonCfg.APCwmax > 0) {
			apCwmin = pAd->CommonCfg.APCwmin;
			apCwmax = pAd->CommonCfg.APCwmax;
			bssCwmin = pAd->CommonCfg.BSSCwmin;
		}
	} else if (num > 4 && num <= 8) {
		apCwmin = 4;
		apCwmax = 6;
		bssCwmin = 5;
	} else if (num > 8 && num <= 16) {
		apCwmin = 4;
		apCwmax = 6;
		bssCwmin = 6;
	} else if (num > 16 && num <= 64) {
		apCwmin = 4;
		apCwmax = 6;
		bssCwmin = 7;
	} else if (num > 64 && num <= 128) {
		apCwmin = 4;
		apCwmax = 6;
		bssCwmin = 8;
	}

	pAd->CommonCfg.APEdcaParm.Cwmin[0] = apCwmin;
	pAd->CommonCfg.APEdcaParm.Cwmax[0] = apCwmax;
	pAd->ApCfg.BssEdcaParm.Cwmin[0] = bssCwmin;

	AsicSetEdcaParm(pAd, &pAd->CommonCfg.APEdcaParm);
}
#endif /* MULTI_CLIENT_SUPPORT */