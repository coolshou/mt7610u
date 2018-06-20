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
	wpa.h

*/

#ifndef __WPA_H__
#define __WPA_H__

#ifndef ROUND_UP
#define ROUND_UP(__x, __y) \
	(((ULONG)((__x)+((__y)-1))) & ((ULONG)~((__y)-1)))
#endif

#define SET_UINT16_TO_ARRARY(_V, _LEN)		\
{						\
	_V[0] = ((UINT16)_LEN) >> 8;		\
	_V[1] = ((UINT16)_LEN & 0xFF);		\
}

#define INC_UINT16_TO_ARRARY(_V, _LEN)		\
{						\
	UINT16	var_len;			\
						\
	var_len = (_V[0]<<8) | (_V[1]);		\
	var_len += _LEN;			\
						\
	_V[0] = (var_len & 0xFF00) >> 8;	\
	_V[1] = (var_len & 0xFF);		\
}

#define CONV_ARRARY_TO_UINT16(_V)	((_V[0]<<8) | (_V[1]))

#define ADD_ONE_To_64BIT_VAR(_V)		\
{						\
	UCHAR cnt = LEN_KEY_DESC_REPLAY;	\
	do					\
	{					\
		cnt--;				\
		_V[cnt]++;			\
		if (cnt == 0)			\
			break;			\
	}while (_V[cnt] == 0);			\
}

#define INC_TX_TSC(_tsc, _cnt)			\
{						\
	int i = 0;				\
	while (++_tsc[i] == 0x0) {		\
		i++;				\
		if (i == (_cnt))		\
			break;			\
	}					\
}

#define IS_WPA_CAPABILITY(a)	(((a) >= Ndis802_11AuthModeWPA) && ((a) <= Ndis802_11AuthModeWPA1PSKWPA2PSK))

/*
	WFA recommend to restrict the encryption type in 11n-HT mode.
 	So, the WEP and TKIP shall not be allowed to use HT rate.
 */
#define IS_INVALID_HT_SECURITY(_mode)			\
	(((_mode) == Ndis802_11Encryption1Enabled) || 	\
	 ((_mode) == Ndis802_11Encryption2Enabled))

#define MIX_CIPHER_WPA_TKIP_ON(x)       (((x) & 0x08) != 0)
#define MIX_CIPHER_WPA_AES_ON(x)        (((x) & 0x04) != 0)
#define MIX_CIPHER_WPA2_TKIP_ON(x)      (((x) & 0x02) != 0)
#define MIX_CIPHER_WPA2_AES_ON(x)       (((x) & 0x01) != 0)

/* Some definition are different between Keneral mode and Daemon mode */
#ifdef WPA_DAEMON_MODE
/* The definition for Daemon mode */
#define WPA_GET_BSS_NUM(_pAd)		(_pAd)->mbss_num

#define WPA_GET_PMK(_pAd, _pEntry, _pmk)					\
{															\
	_pmk = _pAd->MBSS[_pEntry->apidx].PMK;					\
}

#define WPA_GET_GTK(_pAd, _pEntry, _gtk)					\
{															\
	_gtk = _pAd->MBSS[_pEntry->apidx].GTK;					\
}

#define WPA_GET_GROUP_CIPHER(_pAd, _pEntry, _cipher)		\
{															\
	_cipher = (_pAd)->MBSS[_pEntry->apidx].GroupEncrypType;	\
}

#define WPA_GET_DEFAULT_KEY_ID(_pAd, _pEntry, _idx)			\
{															\
	_idx = (_pAd)->MBSS[_pEntry->apidx].DefaultKeyId;		\
}

#define WPA_GET_BMCST_TSC(_pAd, _pEntry, _tsc)				\
{															\
	_tsc = 1;												\
}

#define WPA_BSSID(_pAd, _apidx)		(_pAd)->MBSS[_apidx].wlan_addr

#define WPA_OS_MALLOC(_p, _s)		\
{									\
	_p = os_malloc(_s);			\
}

#define WPA_OS_FREE(_p)		\
{								\
	os_free(_p);				\
}

#define WPA_GET_CURRENT_TIME(_time)		\
{										\
	struct timeval tv;					\
	gettimeofday(&tv, NULL);			\
	*(_time) = tv.tv_sec;					\
}

#else
/* The definition for Driver mode */

#if defined(CONFIG_AP_SUPPORT) && defined(CONFIG_STA_SUPPORT)
#define WPA_GET_BSS_NUM(_pAd)		(((_pAd)->OpMode == OPMODE_AP) ? (_pAd)->ApCfg.BssidNum : 1)
#define WPA_GET_GROUP_CIPHER(_pAd, _pEntry, _cipher)					\
	{																	\
	_cipher = Ndis802_11WEPDisabled;								\
		if ((_pAd)->OpMode == OPMODE_AP)								\
		{																\
		if (IS_ENTRY_APCLI(_pEntry) && 								\
			((_pEntry)->MatchAPCLITabIdx < MAX_APCLI_NUM))			\
			_cipher = (_pAd)->ApCfg.ApCliTab[(_pEntry)->MatchAPCLITabIdx].GroupCipher;	\
			else if ((_pEntry)->apidx < (_pAd)->ApCfg.BssidNum)			\
				_cipher = (_pAd)->ApCfg.MBSSID[_pEntry->apidx].GroupKeyWepStatus;\
		}																\
		else															\
			_cipher = (_pAd)->StaCfg.GroupCipher;						\
	}

#define WPA_BSSID(_pAd, _apidx) 	(((_pAd)->OpMode == OPMODE_AP) ?\
									(_pAd)->ApCfg.MBSSID[_apidx].Bssid :\
									(_pAd)->CommonCfg.Bssid)
#elif defined(CONFIG_AP_SUPPORT)
#define WPA_GET_BSS_NUM(_pAd)		(_pAd)->ApCfg.BssidNum
#define WPA_GET_GROUP_CIPHER(_pAd, _pEntry, _cipher)				\
	{																\
	_cipher = Ndis802_11WEPDisabled;							\
	if (IS_ENTRY_APCLI(_pEntry) && 								\
		((_pEntry)->MatchAPCLITabIdx < MAX_APCLI_NUM))			\
		_cipher = (_pAd)->ApCfg.ApCliTab[(_pEntry)->MatchAPCLITabIdx].GroupCipher;	\
		else if ((_pEntry)->apidx < (_pAd)->ApCfg.BssidNum)			\
			_cipher = (_pAd)->ApCfg.MBSSID[_pEntry->apidx].GroupKeyWepStatus;\
	}

#define WPA_BSSID(_pAd, _apidx) 	(_pAd)->ApCfg.MBSSID[_apidx].Bssid

#elif defined(CONFIG_STA_SUPPORT)
#define WPA_GET_BSS_NUM(_pAd)		1
#define WPA_GET_GROUP_CIPHER(_pAd, _pEntry, _cipher)				\
	{																\
		_cipher = (_pAd)->StaCfg.GroupCipher;						\
	}
#define WPA_BSSID(_pAd, _apidx) 	(_pAd)->CommonCfg.Bssid
#endif /* defined(CONFIG_STA_SUPPORT) */

#define WPA_OS_MALLOC(_p, _s)		\
{									\
	os_alloc_mem(NULL, (PUCHAR *)&_p, _s);		\
}

#define WPA_OS_FREE(_p)		\
{							\
	os_free_mem(NULL, _p);	\
}

#define WPA_GET_CURRENT_TIME(_time)		NdisGetSystemUpTime(_time);

#endif /* End of Driver Mode */


/*========================================
	The prototype is defined in cmm_wpa.c
  ========================================*/
void inc_iv_byte(UCHAR *iv, UINT len, UINT cnt);

BOOLEAN WpaMsgTypeSubst(UCHAR EAPType, int *MsgType);

VOID PRF(
	IN UCHAR *key,
	IN INT key_len,
	IN UCHAR *prefix,
	IN INT prefix_len,
	IN UCHAR *data,
	IN INT data_len,
	OUT UCHAR *output,
	IN INT len);

int RtmpPasswordHash(char *password, unsigned char *ssid, int ssidlength,
	unsigned char *output);

void KDF(
	PUINT8 key,
	int key_len,
	PUINT8 label,
	int label_len,
	PUINT8 data,
	int data_len,
	PUINT8 output,
	USHORT len);

PUINT8 WPA_ExtractSuiteFromRSNIE(
	PUINT8 rsnie,
	UINT rsnie_len,
	UINT8 type,
	UINT8 *count);

VOID WpaShowAllsuite(
	IN PUINT8 rsnie,
	IN UINT rsnie_len);

VOID RTMPInsertRSNIE(
	IN PUCHAR pFrameBuf,
	OUT PULONG pFrameLen,
	IN PUINT8 rsnie_ptr,
	IN UINT8 rsnie_len,
	IN PUINT8 pmkid_ptr,
	IN UINT8 pmkid_len);

/*
 =====================================
 	function prototype in cmm_wpa.c
 =====================================
*/
void RTMPToWirelessSta(
	PRTMP_ADAPTER pAd,
	PMAC_TABLE_ENTRY pEntry,
	PUCHAR pHeader802_3,
	UINT HdrLen,
	PUCHAR pData,
	UINT DataLen,
	BOOLEAN bClearFrame);

void WpaDerivePTK(
	IN PRTMP_ADAPTER pAd,
	IN UCHAR *PMK,
	IN UCHAR *ANonce,
	IN UCHAR *AA,
	IN UCHAR *SNonce,
	IN UCHAR *SA,
	OUT UCHAR *output,
	IN UINT len);

void WpaDeriveGTK(
	UCHAR *PMK,
	UCHAR *GNonce,
	UCHAR *AA,
	UCHAR *output,
	UINT len);

void GenRandom(
	PRTMP_ADAPTER pAd,
	UCHAR *macAddr,
	UCHAR *random);

BOOLEAN RTMPCheckWPAframe(
	PRTMP_ADAPTER pAd,
	PMAC_TABLE_ENTRY pEntry,
	PUCHAR pData,
	ULONG DataByteCount,
	UCHAR FromWhichBSSID);

#ifdef HDR_TRANS_SUPPORT
BOOLEAN RTMPCheckWPAframe_Hdr_Trns(
	PRTMP_ADAPTER pAd,
	PMAC_TABLE_ENTRY pEntry,
	PUCHAR pData,
	ULONG DataByteCount,
	UCHAR FromWhichBSSID);
#endif /* HDR_TRANS_SUPPORT */

BOOLEAN RTMPParseEapolKeyData(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pKeyData,
	IN UCHAR KeyDataLen,
	IN UCHAR GroupKeyIndex,
	IN UCHAR MsgType,
	IN BOOLEAN bWPA2,
	IN MAC_TABLE_ENTRY *pEntry);

void WPA_ConstructKdeHdr(
	UINT8 data_type,
	UINT8 data_len,
	PUCHAR pBuf);

void ConstructEapolMsg(
	IN PMAC_TABLE_ENTRY pEntry,
	IN UCHAR GroupKeyWepStatus,
	IN UCHAR MsgType,
	IN UCHAR DefaultKeyIdx,
	IN UCHAR *KeyNonce,
	IN UCHAR *TxRSC,
	IN UCHAR *GTK,
	IN UCHAR *RSNIE,
	IN UCHAR RSNIE_Len,
	OUT PEAPOL_PACKET pMsg);

PCIPHER_KEY RTMPSwCipherKeySelection(
	PRTMP_ADAPTER pAd,
	PUCHAR pIV,
	RX_BLK *pRxBlk,
	PMAC_TABLE_ENTRY pEntry);

NDIS_STATUS RTMPSoftDecryptionAction(
	PRTMP_ADAPTER pAd,
	PUCHAR pHdr,
	UCHAR UserPriority,
	PCIPHER_KEY pKey,
	PUCHAR pData,
	UINT16 *DataByteCnt);

void RTMPSoftConstructIVHdr(
	UCHAR CipherAlg,
	UCHAR key_id,
	PUCHAR pTxIv,
	PUCHAR pHdrIv,
	UINT8 *hdr_iv_len);

void RTMPSoftEncryptionAction(
	PRTMP_ADAPTER pAd,
	UCHAR CipherAlg,
	PUCHAR pHdr,
	PUCHAR pSrcBufData,
	UINT32 SrcBufLen,
	UCHAR KeyIdx,
	PCIPHER_KEY pKey,
	UINT8 *ext_len);

void RTMPMakeRSNIE(
	PRTMP_ADAPTER pAd,
	UINT AuthMode,
	UINT WepStatus,
	UCHAR apidx);

void WPAInstallPairwiseKey(
	PRTMP_ADAPTER pAd,
	UINT8 BssIdx,
	PMAC_TABLE_ENTRY pEntry,
	BOOLEAN bAE);

void WPAInstallSharedKey(
	PRTMP_ADAPTER pAd,
	UINT8 GroupCipher,
	UINT8 BssIdx,
	UINT8 KeyIdx,
	UINT8 Wcid,
	BOOLEAN bAE,
	PUINT8 pGtk,
	UINT8 GtkLen);

void RTMPSetWcidSecurityInfo(
	PRTMP_ADAPTER pAd,
	UINT8 BssIdx,
	UINT8 KeyIdx,
	UINT8 CipherAlg,
	UINT8 Wcid,
	UINT8 KeyTabFlag);

void CalculateMIC(
	UCHAR KeyDescVer,
	UCHAR *PTK,
	PEAPOL_PACKET pMsg);

PSTRING GetEapolMsgType(
	CHAR msg);

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

/*
 =====================================
 	function prototype in cmm_wep.c
 =====================================
*/
UINT RTMP_CALC_FCS32(
	UINT Fcs,
	PUCHAR Cp,
	int Len);

void RTMPConstructWEPIVHdr(
	UINT8 key_idx,
	UCHAR *pn,
	UCHAR *iv_hdr);

BOOLEAN RTMPSoftEncryptWEP(
	PRTMP_ADAPTER pAd,
	PUCHAR pIvHdr,
	PCIPHER_KEY pKey,
	PUCHAR pData,
	ULONG DataByteCnt);

BOOLEAN RTMPSoftDecryptWEP(
	PRTMP_ADAPTER pAd,
	PCIPHER_KEY pKey,
	PUCHAR pData,
	UINT16 *DataByteCnt);

/*
 =====================================
 	function prototype in cmm_tkip.c
 =====================================
*/
BOOLEAN RTMPSoftDecryptTKIP(
	PRTMP_ADAPTER pAd,
	PUCHAR pHdr,
	UCHAR UserPriority,
	PCIPHER_KEY pKey,
	PUCHAR pData,
	UINT16 *DataByteCnt);

void TKIP_GTK_KEY_WRAP(
	UCHAR *key,
	UCHAR *iv,
	UCHAR *input_text,
	UINT32 input_len,
	UCHAR *output_text);

void TKIP_GTK_KEY_UNWRAP(
	UCHAR *key,
	UCHAR *iv,
	UCHAR *input_text,
	UINT32 input_len,
	UCHAR *output_text);

/*
 =====================================
 	function prototype in cmm_aes.c
 =====================================
*/
BOOLEAN RTMPSoftDecryptAES(
	IN PRTMP_ADAPTER pAd,
	IN PUCHAR pData,
	IN ULONG DataByteCnt,
	IN PCIPHER_KEY pWpaKey);

void RTMPConstructCCMPHdr(
	UINT8 key_idx,
	UCHAR *pn,
	UCHAR *ccmp_hdr);

BOOLEAN RTMPSoftEncryptCCMP(
	PRTMP_ADAPTER pAd,
	PUCHAR pHdr,
	PUCHAR pIV,
	PUCHAR pKey,
	PUCHAR pData,
	UINT32 DataLen);

BOOLEAN RTMPSoftDecryptCCMP(
	PRTMP_ADAPTER pAd,
	PUCHAR pHdr,
	PCIPHER_KEY pKey,
	PUCHAR pData,
	UINT16 *DataLen);

VOID CCMP_test_vector(
	IN PRTMP_ADAPTER pAd,
	IN INT input);

#endif
