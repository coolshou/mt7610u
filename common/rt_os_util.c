/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program; if not, write to the                         *
 * Free Software Foundation, Inc.,                                       *
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                       *
 *************************************************************************

	Module Name:
	rt_os_util.c

	Abstract:
	All functions provided from UTIL module are put here (OS independent).


***************************************************************************/

#define RTMP_MODULE_OS
#define RTMP_MODULE_OS_UTIL

/*#include "rt_config.h"
*/
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rtmp_osabl.h"


 UINT32 RalinkRate[256] = {
	2,  4, 11, 22, 
	12, 18, 24, 36, 48, 72, 96, 108, 109, 110, 111, 112, /* CCK and OFDM */
	13, 26, 39, 52, 78, 104, 117, 130, 26, 52, 78, 104, 156, 208, 234, 260,
	39, 78, 117, 156, 234, 312, 351, 390, /* BW 20, 800ns GI, MCS 0~23 */
	27, 54, 81, 108, 162, 216, 243, 270, 54, 108, 162, 216, 324, 432, 486, 540,
	81, 162, 243, 324, 486, 648, 729, 810, /* BW 40, 800ns GI, MCS 0~23 */
	14, 29, 43, 57, 87, 115, 130, 144, 29, 59, 87, 115, 173, 230, 260, 288,
	43, 87, 130, 173, 260, 317, 390, 433, /* BW 20, 400ns GI, MCS 0~23 */
	30, 60, 90, 120, 180, 240, 270, 300, 60, 120, 180, 240, 360, 480, 540, 600,
	90, 180, 270, 360, 540, 720, 810, 900, /* BW 40, 400ns GI, MCS 0~23 */
	13, 26, 39, 52, 78, 104, 117, 130, 156, /* AC: 20Mhz, 800ns GI, MCS: 0~8 */
	27, 54, 81, 108, 162, 216, 243, 270, 324, 360, /* AC: 40Mhz, 800ns GI, MCS: 0~9 */
	59, 117, 176, 234, 351, 468, 527, 585, 702, 780, /* AC: 80Mhz, 800ns GI, MCS: 0~9 */
	14, 29, 43, 57, 87, 115, 130, 144, 173, /* AC: 20Mhz, 400ns GI, MCS: 0~8 */
	30, 60, 90, 120, 180, 240, 270, 300, 360, 400, /* AC: 40Mhz, 400ns GI, MCS: 0~9 */
	65, 130, 195, 260, 390, 520, 585, 650, 780, 867, /* AC: 80Mhz, 400ns GI, MCS: 0~9 */
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
	20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47 /* 3*3 */
}; 

void RtmpDrvRateGet(void *pReserved, UINT8 MODE, UINT8 ShortGI, UINT8 BW,
	UINT8 MCS, UINT8 Antenna, ULONG *pRate)
{
	UINT32 MCS_1NSS = (UINT32) MCS;
	*pRate = 0;

	DBGPRINT(RT_DEBUG_TRACE,
			("<==== %s \nMODE: %x shortGI: %x BW: %x MCS: %x Antenna: %x \n",
			__FUNCTION__,MODE,ShortGI,BW,MCS,Antenna));
	if ((BW >= Rate_BW_MAX) || (ShortGI >= Rate_GI_MAX) ||
		(BW >= Rate_BW_MAX)) {
		DBGPRINT(RT_DEBUG_ERROR,
				("<==== %s MODE: %x shortGI: %x BW: %x MCS: %x Antenna: %x , param error\n",
				__FUNCTION__,MODE,ShortGI,BW,MCS,Antenna));
		return;
	}

#ifdef DOT11_VHT_AC
	if (MODE >= MODE_VHT) {
		if (MCS_1NSS > 9) {
			Antenna = (MCS / 10)+1;
			MCS_1NSS %= 10;
		}
		*pRate = RalinkRate_VHT_1NSS[BW][ShortGI][MCS_1NSS];
	} else
#endif /* DOT11_VHT_AC */

#ifdef DOT11_N_SUPPORT
	if ((MODE >= MODE_HTMIX) && (MODE < MODE_VHT)) {
		if (MCS_1NSS > 7) {
			Antenna = (MCS / 8)+1;
			MCS_1NSS %= 8;
		}
		*pRate = RalinkRate_HT_1NSS[BW][ShortGI][MCS_1NSS];
	} else
#endif /* DOT11_N_SUPPORT */
	if (MODE == MODE_OFDM)
		*pRate = RalinkRate_Legacy[MCS_1NSS+4];
	else
		*pRate = RalinkRate_Legacy[MCS_1NSS];

	*pRate *= 500000;
#if defined(DOT11_VHT_AC) || defined(DOT11_N_SUPPORT)
	if (MODE >= MODE_HTMIX)
		*pRate *= Antenna;
#endif /* DOT11_VHT_AC */

	DBGPRINT(RT_DEBUG_TRACE,
			("=====> %s \nMODE: %x shortGI: %x BW: %x MCS: %x Antenna: %x  Rate = %lu\n",
			__FUNCTION__,MODE,ShortGI,BW,MCS_1NSS,Antenna,
			(*pRate)/1000000));
}


char *rtstrchr(const char * s, int c)
{
	for (; *s != (char) c; ++s)
		if (*s == '\0')
			return NULL;
	return (char *) s;
}


VOID RtmpMeshDown(
	IN VOID *pDrvCtrlBK,
	IN BOOLEAN WaitFlag,
	IN BOOLEAN	 (*RtmpMeshLinkCheck)(IN VOID *pAd))
{
}



	
BOOLEAN RtmpOsCmdDisplayLenCheck(
	IN UINT32 LenSrc,
	IN UINT32 Offset)
{
	if (LenSrc > (IW_PRIV_SIZE_MASK - Offset))
		return FALSE;

	return TRUE;
}


#if defined(WPA_SUPPLICANT_SUPPORT) || defined(APCLI_WPA_SUPPLICANT_SUPPORT)
void WpaSendMicFailureToWpaSupplicant(PNET_DEV pNetDev, const PUCHAR src_addr,
	BOOLEAN bUnicast, int key_id, const PUCHAR tsc)
{
#ifdef RT_CFG80211_SUPPORT
	CFG80211OS_MICFailReport(pNetDev, src_addr, bUnicast, key_id, tsc);
#else
	char custom[IW_CUSTOM_MAX] = {0};

	snprintf(custom, sizeof(custom), "MLME-MICHAELMICFAILURE.indication");
	if (bUnicast)
		sprintf(custom, "%s unicast", custom);

	RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_CUSTOM, -1, NULL,
			(PUCHAR)custom, strlen(custom));
#endif /* RT_CFG80211_SUPPORT */

	return;
}
#endif /* defined(WPA_SUPPLICANT_SUPPORT) || defined(APCLI_WPA_SUPPLICANT_SUPPORT) */


#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
int wext_notify_event_assoc(PNET_DEV pNetDev, UCHAR *ReqVarIEs, UINT32 ReqVarIELen)
{
	char custom[IW_CUSTOM_MAX] = {0};

#if WIRELESS_EXT > 17
	if (ReqVarIELen <= IW_CUSTOM_MAX) {
		NdisMoveMemory(custom, ReqVarIEs, ReqVarIELen);
		RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_ASSOC_REQ_IE, -1,
				NULL, (UCHAR *)custom, ReqVarIELen);
	} else {
		DBGPRINT(RT_DEBUG_TRACE,
				("pAd->StaCfg.ReqVarIELen > MAX_CUSTOM_LEN\n"));
	}
#else
	int len;

	len = (ReqVarIELen*2) + 17;
	if (len <= IW_CUSTOM_MAX) {
		UCHAR   idx;
		snprintf(custom, sizeof(custom), "ASSOCINFO(ReqIEs=");

		for (idx = 0; idx < ReqVarIELen; idx++)
			sprintf(custom, "%s%02x", custom, ReqVarIEs[idx]);

		RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_CUSTOM, -1,
				NULL, custom, len);
	} else {
		DBGPRINT(RT_DEBUG_TRACE, ("len(%d) > MAX_CUSTOM_LEN\n", len));
	}
#endif

	return 0;
}
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#ifdef WPA_SUPPLICANT_SUPPORT
#ifndef NATIVE_WPA_SUPPLICANT_SUPPORT
void SendAssocIEsToWpaSupplicant(PNET_DEV pNetDev, UCHAR *ReqVarIEs,
	UINT32 ReqVarIELen)
{
	STRING custom[IW_CUSTOM_MAX] = {0};

	if ((ReqVarIELen + 17) <= IW_CUSTOM_MAX) {
		snprintf(custom, sizeof(custom), "ASSOCINFO_ReqIEs=");
		NdisMoveMemory(custom+17, ReqVarIEs, ReqVarIELen);
		RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_CUSTOM,
				RT_REQIE_EVENT_FLAG, NULL, (PUCHAR)custom,
				ReqVarIELen + 17);
		RtmpOSWrielessEventSend(pNetDev, RT_WLAN_EVENT_CUSTOM,
				RT_ASSOCINFO_EVENT_FLAG, NULL, NULL, 0);
	} else {
		DBGPRINT(RT_DEBUG_TRACE,
				("pAd->StaCfg.ReqVarIELen + 17 > MAX_CUSTOM_LEN\n"));
	}
}
#endif /* NATIVE_WPA_SUPPLICANT_SUPPORT */
#endif /* WPA_SUPPLICANT_SUPPORT */
#endif /*CONFIG_STA_SUPPORT*/

INT32 RtPrivIoctlSetVal(void)
{
	return (INT32)RTPRIV_IOCTL_SET;
}

