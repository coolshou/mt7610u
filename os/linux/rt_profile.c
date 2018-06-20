/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

	Module Name:
	rt_profile.c

 */

#include "rt_config.h"

#if defined (CONFIG_RA_HW_NAT)  || defined (CONFIG_RA_HW_NAT_MODULE)
#include "../../../../../../net/nat/hw_nat/ra_nat.h"
#include "../../../../../../net/nat/hw_nat/frame_engine.h"
#endif

#ifdef SYSTEM_LOG_SUPPORT
/* for wireless system event message */
char const *pWirelessSysEventText[IW_SYS_EVENT_TYPE_NUM] = {    
	/* system status event */
    "had associated successfully",							/* IW_ASSOC_EVENT_FLAG */
    "had disassociated",									/* IW_DISASSOC_EVENT_FLAG */
    "had deauthenticated",									/* IW_DEAUTH_EVENT_FLAG */
    "had been aged-out and disassociated",					/* IW_AGEOUT_EVENT_FLAG */
    "occurred CounterMeasures attack",						/* IW_COUNTER_MEASURES_EVENT_FLAG */	
    "occurred replay counter different in Key Handshaking",	/* IW_REPLAY_COUNTER_DIFF_EVENT_FLAG */
    "occurred RSNIE different in Key Handshaking",			/* IW_RSNIE_DIFF_EVENT_FLAG */
    "occurred MIC different in Key Handshaking",			/* IW_MIC_DIFF_EVENT_FLAG */
    "occurred ICV error in RX",								/* IW_ICV_ERROR_EVENT_FLAG */
    "occurred MIC error in RX",								/* IW_MIC_ERROR_EVENT_FLAG */
	"Group Key Handshaking timeout",						/* IW_GROUP_HS_TIMEOUT_EVENT_FLAG */ 
	"Pairwise Key Handshaking timeout",						/* IW_PAIRWISE_HS_TIMEOUT_EVENT_FLAG */ 
	"RSN IE sanity check failure",							/* IW_RSNIE_SANITY_FAIL_EVENT_FLAG */ 
	"set key done in WPA/WPAPSK",							/* IW_SET_KEY_DONE_WPA1_EVENT_FLAG */ 
	"set key done in WPA2/WPA2PSK",                         /* IW_SET_KEY_DONE_WPA2_EVENT_FLAG */ 
	"connects with our wireless client",                    /* IW_STA_LINKUP_EVENT_FLAG */ 
	"disconnects with our wireless client",                 /* IW_STA_LINKDOWN_EVENT_FLAG */
	"scan completed",										/* IW_SCAN_COMPLETED_EVENT_FLAG */
	"scan terminate!! Busy!! Enqueue fail!!",				/* IW_SCAN_ENQUEUE_FAIL_EVENT_FLAG */
	"channel switch to ",									/* IW_CHANNEL_CHANGE_EVENT_FLAG */
	"wireless mode is not support",							/* IW_STA_MODE_EVENT_FLAG */
	"blacklisted in MAC filter list",						/* IW_MAC_FILTER_LIST_EVENT_FLAG */
	"Authentication rejected because of challenge failure",	/* IW_AUTH_REJECT_CHALLENGE_FAILURE */
	"Scanning",												/* IW_SCANNING_EVENT_FLAG */
	"Start a new IBSS",										/* IW_START_IBSS_FLAG */
	"Join the IBSS",										/* IW_JOIN_IBSS_FLAG */
	"Shared WEP fail",										/* IW_SHARED_WEP_FAIL*/
	};

#ifdef IDS_SUPPORT
/* for wireless IDS_spoof_attack event message */
char const *pWirelessSpoofEventText[IW_SPOOF_EVENT_TYPE_NUM] = {   	
    "detected conflict SSID",								/* IW_CONFLICT_SSID_EVENT_FLAG */
    "detected spoofed association response",				/* IW_SPOOF_ASSOC_RESP_EVENT_FLAG */
    "detected spoofed reassociation responses",				/* IW_SPOOF_REASSOC_RESP_EVENT_FLAG */
    "detected spoofed probe response",						/* IW_SPOOF_PROBE_RESP_EVENT_FLAG */
    "detected spoofed beacon",								/* IW_SPOOF_BEACON_EVENT_FLAG */
    "detected spoofed disassociation",						/* IW_SPOOF_DISASSOC_EVENT_FLAG */
    "detected spoofed authentication",						/* IW_SPOOF_AUTH_EVENT_FLAG */
    "detected spoofed deauthentication",					/* IW_SPOOF_DEAUTH_EVENT_FLAG */
    "detected spoofed unknown management frame",			/* IW_SPOOF_UNKNOWN_MGMT_EVENT_FLAG */
	"detected replay attack"								/* IW_REPLAY_ATTACK_EVENT_FLAG */	
	};

/* for wireless IDS_flooding_attack event message */
char const *pWirelessFloodEventText[IW_FLOOD_EVENT_TYPE_NUM] = {   	
	"detected authentication flooding",						/* IW_FLOOD_AUTH_EVENT_FLAG */
    "detected association request flooding",				/* IW_FLOOD_ASSOC_REQ_EVENT_FLAG */
    "detected reassociation request flooding",				/* IW_FLOOD_REASSOC_REQ_EVENT_FLAG */
    "detected probe request flooding",						/* IW_FLOOD_PROBE_REQ_EVENT_FLAG */
    "detected disassociation flooding",						/* IW_FLOOD_DISASSOC_EVENT_FLAG */
    "detected deauthentication flooding",					/* IW_FLOOD_DEAUTH_EVENT_FLAG */
    "detected 802.1x eap-request flooding"					/* IW_FLOOD_EAP_REQ_EVENT_FLAG */	
	};
#endif /* IDS_SUPPORT */


#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
#endif /* SYSTEM_LOG_SUPPORT */

static UCHAR *get_dev_profile(RTMP_ADAPTER *pAd)
{
	UCHAR *src = NULL;

#ifdef CONFIG_AP_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_AP(pAd) {
#if defined(CONFIG_RT_FIRST_CARD) && defined(CONFIG_RT_SECOND_CARD)
			int card_idx = pAd->dev_idx;

			if (card_idx == 0)
				src = FIRST_AP_PROFILE_PATH;
			else if (card_idx == 1)
				src = SECOND_AP_PROFILE_PATH;
			else
#endif /* defined(CONFIG_RT_FIRST_CARD) || defined(CONFIG_RT_SECOND_CARD) */
				src = AP_PROFILE_PATH;
		}
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
		IF_DEV_CONFIG_OPMODE_ON_STA(pAd) {
			src = STA_PROFILE_PATH;
		}
#endif /* CONFIG_STA_SUPPORT */

#ifdef MULTIPLE_CARD_SUPPORT
	src = (PSTRING)pAd->MC_FileName;
#endif /* MULTIPLE_CARD_SUPPORT */

	return src;
}


NDIS_STATUS RTMPReadParametersHook(RTMP_ADAPTER *pAd)
{
	PSTRING src = NULL;
	RTMP_OS_FD srcf;
	RTMP_OS_FS_INFO osFSInfo;
	NDIS_STATUS retval = NDIS_STATUS_FAILURE;
	ULONG buf_size = MAX_INI_BUFFER_SIZE, fsize;
	PSTRING buffer = NULL;

#ifdef HOSTAPD_SUPPORT
	int i;
#endif /*HOSTAPD_SUPPORT */
	src = get_dev_profile(pAd);
	if (src && *src) {
		RtmpOSFSInfoChange(&osFSInfo, TRUE);
		srcf = RtmpOSFileOpen(src, O_RDONLY, 0);
		if (IS_FILE_OPEN_ERR(srcf)) {
			DBGPRINT(RT_DEBUG_ERROR, ("Open file \"%s\" failed!\n", src));
		} else {
			// TODO: need to roll back when convert into OSABL code
			fsize = (ULONG)file_inode(srcf)->i_size;
			DBGPRINT(RT_DEBUG_INFO, ("buffer size %lu file size %lu\n",
				buf_size, fsize));
				if (buf_size < (fsize + 1))
					buf_size = fsize + 1;

			buffer = os_alloc_mem(buf_size);
			if (buffer) {
				memset(buffer, 0x00, buf_size);
				retval = RtmpOSFileRead(srcf, buffer, buf_size - 1);
				if (retval > 0) {
					RTMPSetProfileParameters(pAd, buffer);
					retval = NDIS_STATUS_SUCCESS;
				} else {
					DBGPRINT(RT_DEBUG_ERROR,
							("Read file \"%s\" failed(errCode=%d)!\n",
							src, retval));
				}
				os_free_mem(buffer);
			} else {
				retval = NDIS_STATUS_FAILURE;
			}

			if (RtmpOSFileClose(srcf) != 0) {
				retval = NDIS_STATUS_FAILURE;
				DBGPRINT(RT_DEBUG_ERROR,
						("Close file \"%s\" failed(errCode=%d)!\n",
						src, retval));
			}
		}

		RtmpOSFSInfoChange(&osFSInfo, FALSE);
	}

#ifdef HOSTAPD_SUPPORT
		for (i = 0; i < pAd->ApCfg.BssidNum; i++) {
			pAd->ApCfg.MBSSID[i].Hostapd=Hostapd_Diable;
			DBGPRINT(RT_DEBUG_TRACE, 
					("Reset ra%d hostapd support=FLASE",
					i));
		}
#endif /*HOSTAPD_SUPPORT */

#ifdef SINGLE_SKU_V2
	RTMPSetSingleSKUParameters(pAd);
#endif /* SINGLE_SKU_V2 */
	return retval;
}


void RTMP_IndicateMediaState(PRTMP_ADAPTER pAd, NDIS_MEDIA_STATE media_state)
{
	pAd->IndicateMediaState = media_state;

#ifdef SYSTEM_LOG_SUPPORT
	if (pAd->IndicateMediaState == NdisMediaStateConnected) {
		RTMPSendWirelessEvent(pAd, IW_STA_LINKUP_EVENT_FLAG, 
				pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0);
	} else {
		RTMPSendWirelessEvent(pAd, IW_STA_LINKDOWN_EVENT_FLAG, 
				pAd->MacTab.Content[BSSID_WCID].Addr, BSS0, 0);
	}
#endif /* SYSTEM_LOG_SUPPORT */
}


void tbtt_tasklet(unsigned long data)
{
#ifdef CONFIG_AP_SUPPORT
#ifdef WORKQUEUE_BH
	struct work_struct *work = (struct work_struct *)data;
	POS_COOKIE pObj = container_of(work, struct os_cookie, tbtt_task);
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)pObj->pAd_va;
#else
	PRTMP_ADAPTER pAd = (RTMP_ADAPTER *)data;
#endif /* WORKQUEUE_BH */

#ifdef RT_CFG80211_P2P_SUPPORT
		if (pAd->cfg80211_ctrl.isCfgInApMode == RT_CMD_80211_IFTYPE_AP)
#else
	if (pAd->OpMode == OPMODE_AP)
#endif /* RT_CFG80211_P2P_SUPPORT */
	{
		/* step 7 - if DTIM, then move backlogged bcast/mcast frames 
		 * from PSQ to TXQ whenever DtimCount==0 */
#ifdef RTMP_MAC_USB
		if ((pAd->ApCfg.DtimCount + 1) == pAd->ApCfg.DtimPeriod)
#endif /* RTMP_MAC_USB */
		{
			QUEUE_ENTRY *pEntry;
			BOOLEAN bPS = FALSE;
			UINT count = 0;
			unsigned long IrqFlags;

			RTMP_IRQ_LOCK(&pAd->irq_lock, IrqFlags);
			while (pAd->MacTab.McastPsQueue.Head) {
				bPS = TRUE;
				if (pAd->TxSwQueue[QID_AC_BE].Number <= (pAd->TxSwQMaxLen + MAX_PACKETS_IN_MCAST_PS_QUEUE)) {
					pEntry = RemoveHeadQueue(&pAd->MacTab.McastPsQueue);
					/*if (pAd->MacTab.McastPsQueue.Number) */
					if (count) {
						RTMP_SET_PACKET_MOREDATA(pEntry, TRUE);
					}
					InsertHeadQueue(&pAd->TxSwQueue[QID_AC_BE], pEntry);
					count++;
				} else {
					break;
				}
			}
			RTMP_IRQ_UNLOCK(&pAd->irq_lock, IrqFlags);

			if (pAd->MacTab.McastPsQueue.Number == 0) {
				UINT bss_index;

				/* clear MCAST/BCAST backlog bit for all BSS */
				for (bss_index = BSS0; bss_index < pAd->ApCfg.BssidNum; bss_index++)
					WLAN_MR_TIM_BCMC_CLEAR(bss_index);
			}
			pAd->MacTab.PsQIdleCount = 0;

			if (bPS == TRUE) {
				RTMPDeQueuePacket(pAd, FALSE, NUM_OF_TX_RING, 
						/*MAX_TX_IN_TBTT*/MAX_PACKETS_IN_MCAST_PS_QUEUE);
			}
		}
	}
#endif /* CONFIG_AP_SUPPORT */
}

/*
========================================================================
Routine Description:
	Assign protocol to the packet.

Arguments:
	pPkt		- the packet

Return Value:
	None

Note:
========================================================================
*/
static void RtmpOsPktProtocolAssign(PNDIS_PACKET pNetPkt)
{
	struct sk_buff *pRxPkt = RTPKT_TO_OSPKT(pNetPkt);
	pRxPkt->protocol = eth_type_trans(pRxPkt, pRxPkt->dev);
}

#ifdef SYSTEM_LOG_SUPPORT
/*
	========================================================================
	
	Routine Description:
		Send log message through wireless event

		Support standard iw_event with IWEVCUSTOM. It is used below.

		iwreq_data.data.flags is used to store event_flag that is defined by user. 
		iwreq_data.data.length is the length of the event log.

		The format of the event log is composed of the entry's MAC address and
		the desired log message (refer to pWirelessEventText).

			ex: 11:22:33:44:55:66 has associated successfully

		p.s. The requirement of Wireless Extension is v15 or newer. 

	========================================================================
*/
VOID RtmpDrvSendWirelessEvent(
	IN	VOID					*pAdSrc,
	IN	USHORT					Event_flag,
	IN	PUCHAR 					pAddr,
	IN  UCHAR					BssIdx,
	IN	CHAR					Rssi)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
	PSTRING	pBuf = NULL, pBufPtr = NULL;
	USHORT	event, type, BufLen;	
	UCHAR	event_table_len = 0;

	if (pAd->CommonCfg.bWirelessEvent == FALSE)
		return;

	type = Event_flag & 0xFF00;	
	event = Event_flag & 0x00FF;

	switch (type)
	{
		case IW_SYS_EVENT_FLAG_START:
			event_table_len = IW_SYS_EVENT_TYPE_NUM;
			break;
#ifdef IDS_SUPPORT
		case IW_SPOOF_EVENT_FLAG_START:
			event_table_len = IW_SPOOF_EVENT_TYPE_NUM;
			break;

		case IW_FLOOD_EVENT_FLAG_START:
			event_table_len = IW_FLOOD_EVENT_TYPE_NUM;
			break;
#endif /* IDS_SUPPORT */ 			
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
	}
	
	if (event_table_len == 0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : The type(%0x02x) is not valid.\n", __FUNCTION__, type));			       		       		
		return;
	}
	
	if (event >= event_table_len)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("%s : The event(%0x02x) is not valid.\n", __FUNCTION__, event));			       		       		
		return;
	}	
 
	/*Allocate memory and copy the msg. */
/*	if((pBuf = kmalloc(IW_CUSTOM_MAX_LEN, GFP_ATOMIC)) != NULL) */
	pBuf = os_alloc_mem(IW_CUSTOM_MAX_LEN);
	if(pBuf != NULL)
	{
		/*Prepare the payload */
		memset(pBuf, 0, IW_CUSTOM_MAX_LEN);

		pBufPtr = pBuf;		

		if (pAddr)
			pBufPtr += sprintf(pBufPtr, "(RT2860) STA(%02x:%02x:%02x:%02x:%02x:%02x) ", PRINT_MAC(pAddr));				
		else if (BssIdx < MAX_MBSSID_NUM(pAd))
			pBufPtr += sprintf(pBufPtr, "(RT2860) BSS(ra%d) ", BssIdx);
		else
			pBufPtr += sprintf(pBufPtr, "(RT2860) ");

		if (type == IW_SYS_EVENT_FLAG_START)
        {
			pBufPtr += sprintf(pBufPtr, "%s", pWirelessSysEventText[event]);
		    
            if (Event_flag == IW_CHANNEL_CHANGE_EVENT_FLAG)
		  	{
			 	pBufPtr += sprintf(pBufPtr, "%3d", Rssi);
			}			
		}
#ifdef IDS_SUPPORT		
		else if (type == IW_SPOOF_EVENT_FLAG_START)
			pBufPtr += sprintf(pBufPtr, "%s (RSSI=%d)", pWirelessSpoofEventText[event], Rssi);
		else if (type == IW_FLOOD_EVENT_FLAG_START)
			pBufPtr += sprintf(pBufPtr, "%s", pWirelessFloodEventText[event]);
#endif /* IDS_SUPPORT */		
#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */
		else
			pBufPtr += sprintf(pBufPtr, "%s", "unknown event");
		
		pBufPtr[pBufPtr - pBuf] = '\0';
		BufLen = pBufPtr - pBuf;
		
		RtmpOSWrielessEventSend(pAd->net_dev, RT_WLAN_EVENT_CUSTOM, Event_flag, NULL, (PUCHAR)pBuf, BufLen);
		/*DBGPRINT(RT_DEBUG_TRACE, ("%s : %s\n", __FUNCTION__, pBuf)); */
	
/*		kfree(pBuf); */
		os_free_mem(pBuf);
	}
	else
		DBGPRINT(RT_DEBUG_ERROR, ("%s : Can't allocate memory for wireless event.\n", __FUNCTION__));			       		       				
}
#endif /* SYSTEM_LOG_SUPPORT */

void announce_802_3_packet(void *pAdSrc, PNDIS_PACKET pPacket, UCHAR OpMode)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)pAdSrc;
	PNDIS_PACKET pRxPkt = pPacket;

	ASSERT(pPacket);
	MEM_DBG_PKT_FREE_INC(pPacket);

#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_AP(pAd) {
	}
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	/* Push up the protocol stack */
#ifdef CONFIG_AP_SUPPORT
#if defined(PLATFORM_BL2348) || defined(PLATFORM_BL23570)
	{
		extern int (*pToUpperLayerPktSent)(PNDIS_PACKET *pSkb);
		RtmpOsPktProtocolAssign(pRxPkt);
		pToUpperLayerPktSent(pRxPkt);
		return;
	}
#endif /* defined(PLATFORM_BL2348) || defined(PLATFORM_BL23570) */
#endif /* CONFIG_AP_SUPPORT */

#ifdef CONFIG_STA_SUPPORT
#endif /* CONFIG_STA_SUPPORT */

    /* Push up the protocol stack */

#ifdef IKANOS_VX_1X0
{
	IKANOS_DataFrameRx(pAd, pRxPkt);
	return;
}
#endif /* IKANOS_VX_1X0 */

#ifdef INF_PPA_SUPPORT
	if (ppa_hook_directpath_send_fn && pAd->PPAEnable==TRUE ) 
	{
		RtmpOsPktInfPpaSend(pRxPkt);
		pRxPkt=NULL;
		return;
	}	  	
#endif /* INF_PPA_SUPPORT */

	{
#ifdef CONFIG_RT2880_BRIDGING_ONLY
		PACKET_CB_ASSIGN(pRxPkt, 22) = 0xa8;
#endif

#if defined(CONFIG_RA_CLASSIFIER) || defined(CONFIG_RA_CLASSIFIER_MODULE)
		if (ra_classifier_hook_rx!= NULL) {
			unsigned int flags;

			RTMP_IRQ_LOCK(&pAd->page_lock, flags);
			ra_classifier_hook_rx(pRxPkt, classifier_cur_cycle);
			RTMP_IRQ_UNLOCK(&pAd->page_lock, flags);
		}
#endif /* CONFIG_RA_CLASSIFIER */

#if !defined(CONFIG_RA_NAT_NONE)
#if defined (CONFIG_RA_HW_NAT) || defined (CONFIG_RA_HW_NAT_MODULE)
		RtmpOsPktNatMagicTag(pRxPkt);
#endif

#ifdef RA_NAT_SUPPORT
		/* bruce+
			ra_sw_nat_hook_rx return 1 --> continue
			ra_sw_nat_hook_rx return 0 --> FWD & without netif_rx
		*/
		if (ra_sw_nat_hook_rx!= NULL) {
			unsigned int flags;

			RtmpOsPktProtocolAssign(pRxPkt);

			RTMP_IRQ_LOCK(&pAd->page_lock, flags);
			if (ra_sw_nat_hook_rx(pRxPkt)) {
				RtmpOsPktRcvHandle(pRxPkt);
			}
			RTMP_IRQ_UNLOCK(&pAd->page_lock, flags);
			return;
		}
#endif /* RA_NAT_SUPPORT */
#else
		{
#if defined (CONFIG_RA_HW_NAT)  || defined (CONFIG_RA_HW_NAT_MODULE)
			RtmpOsPktNatNone(pRxPkt);
#endif /* CONFIG_RA_HW_NAT */
		}
#endif /* CONFIG_RA_NAT_NONE */
	}

#ifdef CONFIG_AP_SUPPORT
#ifdef BG_FT_SUPPORT
	if (BG_FTPH_PacketFromApHandle(pRxPkt) == 0)
		return;
#endif /* BG_FT_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

	RtmpOsPktProtocolAssign(pRxPkt);

#if defined (CONFIG_WIFI_PKT_FWD)
	if (wf_fwd_rx_hook!= NULL) {
		unsigned int flags;
		RTMP_IRQ_LOCK(&pAd->page_lock, flags);

		if (wf_fwd_rx_hook(pRxPkt) == 0) {
			RTMP_IRQ_UNLOCK(&pAd->page_lock, flags);
			return;
		}

		RTMP_IRQ_UNLOCK(&pAd->page_lock, flags);
	}
#endif /* CONFIG_WIFI_PKT_FWD */

	RtmpOsPktRcvHandle(pRxPkt);
}


#ifdef CONFIG_STA_SUPPORT
void STA_MonPktSend(
	IN RTMP_ADAPTER *pAd,
	IN RX_BLK *pRxBlk)
{
	PNET_DEV pNetDev;
	PNDIS_PACKET pRxPacket;
	PHEADER_802_11 pHeader;
	USHORT DataSize;
	UINT32 MaxRssi;
	UCHAR L2PAD, PHYMODE, BW, ShortGI, MCS, AMPDU, STBC, RSSI1;
	UCHAR BssMonitorFlag11n, Channel, CentralChannel;
	UCHAR *pData, *pDevName;


	/* sanity check */
    ASSERT(pRxBlk->pRxPacket);
    if (pRxBlk->DataSize < 10)
    {
        DBGPRINT(RT_DEBUG_ERROR, ("%s : Size is too small! (%d)\n", __FUNCTION__, pRxBlk->DataSize));
		goto err_free_sk_buff;
    }

    if (pRxBlk->DataSize + sizeof(wlan_ng_prism2_header) > RX_BUFFER_AGGRESIZE)
    {
        DBGPRINT(RT_DEBUG_ERROR, ("%s : Size is too large! (%ld)\n", __FUNCTION__, pRxBlk->DataSize + sizeof(wlan_ng_prism2_header)));
		goto err_free_sk_buff;
    }

	/* init */
	MaxRssi = RTMPMaxRssi(pAd,
						ConvertToRssi(pAd, pRxBlk->pRxWI->RxWIRSSI0, RSSI_0),
						ConvertToRssi(pAd, pRxBlk->pRxWI->RxWIRSSI1, RSSI_1),
						ConvertToRssi(pAd, pRxBlk->pRxWI->RxWIRSSI2, RSSI_2));

	pNetDev = get_netdev_from_bssid(pAd, BSS0); 
	pRxPacket = pRxBlk->pRxPacket;
	pHeader = pRxBlk->pHeader;
	pData = pRxBlk->pData;
	DataSize = pRxBlk->DataSize;
	L2PAD = pRxBlk->pRxInfo->L2PAD;
	PHYMODE = pRxBlk->pRxWI->RxWIPhyMode;
	BW = pRxBlk->pRxWI->RxWIBW;
	ShortGI = pRxBlk->pRxWI->RxWISGI;
	MCS = pRxBlk->pRxWI->RxWIMCS;
	AMPDU = pRxBlk->pRxInfo->AMPDU;
	STBC = pRxBlk->pRxWI->RxWISTBC;
	RSSI1 = pRxBlk->pRxWI->RxWIRSSI1;
	BssMonitorFlag11n = 0;
#ifdef MONITOR_FLAG_11N_SNIFFER_SUPPORT
	BssMonitorFlag11n = (pAd->StaCfg.BssMonitorFlag & MONITOR_FLAG_11N_SNIFFER);
#endif /* MONITOR_FLAG_11N_SNIFFER_SUPPORT */
	pDevName = (UCHAR *)RtmpOsGetNetDevName(pAd->net_dev);
	Channel = pAd->CommonCfg.Channel;
	CentralChannel = pAd->CommonCfg.CentralChannel;

	/* pass the packet */
	send_monitor_packets(pNetDev, pRxPacket, pHeader, pData, DataSize,
						L2PAD, PHYMODE, BW, ShortGI, MCS, AMPDU, STBC, RSSI1,
						BssMonitorFlag11n, pDevName, Channel, CentralChannel,
						MaxRssi);
	return;

err_free_sk_buff:
	RELEASE_NDIS_PACKET(pAd, pRxBlk->pRxPacket, NDIS_STATUS_FAILURE);	
	return;
}
#endif /* CONFIG_STA_SUPPORT */


extern NDIS_SPIN_LOCK TimerSemLock;

void RTMPFreeAdapter(void *pAdSrc)
{
	PRTMP_ADAPTER pAd = (PRTMP_ADAPTER)pAdSrc;
	POS_COOKIE os_cookie;
	int index;

	os_cookie = (POS_COOKIE)pAd->OS_Cookie;

	if (pAd->BeaconBuf)
		os_free_mem(pAd->BeaconBuf);

#ifdef MULTIPLE_CARD_SUPPORT
#ifdef RTMP_FLASH_SUPPORT
	if (pAd->eebuf && (pAd->eebuf != pAd->chipCap.EEPROM_DEFAULT_BIN)) {
		os_free_mem(pAd->eebuf);
		pAd->eebuf = NULL;
	}
#endif /* RTMP_FLASH_SUPPORT */
#endif /* MULTIPLE_CARD_SUPPORT */

	NdisFreeSpinLock(&pAd->MgmtRingLock);

#if defined(RT3290) || defined(RT65xx)
//	NdisFreeSpinLock(&pAd->WlanEnLock);
#endif /* defined(RT3290) || defined(RT65xx) */

	for (index = 0 ; index < NUM_OF_TX_RING; index++) {
		NdisFreeSpinLock(&pAd->TxSwQueueLock[index]);
		NdisFreeSpinLock(&pAd->DeQueueLock[index]);
		pAd->DeQueueRunning[index] = FALSE;
	}

	NdisFreeSpinLock(&pAd->irq_lock);

#ifdef UAPSD_SUPPORT
	NdisFreeSpinLock(&pAd->UAPSDEOSPLock); /* OS_ABL_SUPPORT */
#endif /* UAPSD_SUPPORT */

#ifdef DOT11_N_SUPPORT
	NdisFreeSpinLock(&pAd->mpdu_blk_pool.lock);
#endif /* DOT11_N_SUPPORT */

	if (pAd->iw_stats) {
		os_free_mem(pAd->iw_stats);
		pAd->iw_stats = NULL;
	}

	if (pAd->stats) {
		os_free_mem(pAd->stats);
		pAd->stats = NULL;
	}

	NdisFreeSpinLock(&TimerSemLock);

#ifdef RALINK_ATE
#ifdef RTMP_MAC_USB
	RTMP_OS_ATMOIC_DESTROY(&pAd->BulkOutRemained);
	RTMP_OS_ATMOIC_DESTROY(&pAd->BulkInRemained);
#endif /* RTMP_MAC_USB */
#endif /* RALINK_ATE */

	RTMP_OS_FREE_TIMER(pAd);
	RTMP_OS_FREE_LOCK(pAd);
	RTMP_OS_FREE_TASKLET(pAd);
	RTMP_OS_FREE_TASK(pAd);
	RTMP_OS_FREE_SEM(pAd);
	RTMP_OS_FREE_ATOMIC(pAd);

	RtmpOsVfree(pAd); /* pci_free_consistent(os_cookie->pci_dev,sizeof(RTMP_ADAPTER),pAd,os_cookie->pAd_pa); */
	if (os_cookie)
		os_free_mem(os_cookie);
}


int RTMPSendPackets(NDIS_HANDLE dev_hnd, PPNDIS_PACKET ppPacketArray, UINT NumberOfPackets,
	UINT32 PktTotalLen, RTMP_NET_ETH_CONVERT_DEV_SEARCH Func)
{
	RTMP_ADAPTER *pAd = (RTMP_ADAPTER *)dev_hnd;
	PNDIS_PACKET pPacket = ppPacketArray[0];


	INC_COUNTER64(pAd->WlanCounters.TransmitCountFrmOs);

	if (pPacket == NULL)
		goto done;

	/* RT2870STA does this in RTMPSendPackets() */
#ifdef RALINK_ATE
	if (ATE_ON(pAd)) {
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_RESOURCES);
		return 0;
	}
#endif /* RALINK_ATE */

#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd) {
		/* Drop send request since we are in monitor mode */
		if (MONITOR_ON(pAd)) {
			RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
			return 0;
		}
	}
#endif /* CONFIG_STA_SUPPORT */

        /* EapolStart size is 18 */
	if (PktTotalLen < 14) {
		/*printk("bad packet size: %d\n", pkt->len); */
		hex_dump("bad packet", GET_OS_PKT_DATAPTR(pPacket), PktTotalLen);
		RELEASE_NDIS_PACKET(pAd, pPacket, NDIS_STATUS_FAILURE);
		return 0;
	}



	RTMP_SET_PACKET_5VT(pPacket, 0);
/*	MiniportMMRequest(pAd, pkt->data, pkt->len); */
#ifdef CONFIG_5VT_ENHANCE
    if (*(int*)(GET_OS_PKT_CB(pPacket)) == BRIDGE_TAG) {
		RTMP_SET_PACKET_5VT(pPacket, 1);
    }
#endif


#ifdef CONFIG_STA_SUPPORT
	IF_DEV_CONFIG_OPMODE_ON_STA(pAd)
	{

		STASendPackets((NDIS_HANDLE)pAd, (PPNDIS_PACKET) &pPacket, 1);
	}

#endif /* CONFIG_STA_SUPPORT */

done:
	return 0;
}


PNET_DEV get_netdev_from_bssid(RTMP_ADAPTER *pAd, UCHAR FromWhichBSSID)
{
	PNET_DEV dev_p = NULL;
#ifdef CONFIG_AP_SUPPORT
	UCHAR infRealIdx;
#endif

	do {
#ifdef CONFIG_STA_SUPPORT
#ifdef RT_CFG80211_P2P_CONCURRENT_DEVICE
		if (FromWhichBSSID >= MIN_NET_DEVICE_FOR_CFG80211_VIF_P2P_GO) {
			dev_p = pAd->ApCfg.MBSSID[BSS0].wdev.if_dev;
			break;
		} else if (FromWhichBSSID >= MIN_NET_DEVICE_FOR_CFG80211_VIF_P2P_CLI) {
			//CFG_TODO
			break;
		}
#endif /* RT_CFG80211_P2P_CONCURRENT_DEVICE */
#endif /* CONFIG_STA_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
		infRealIdx = FromWhichBSSID & (NET_DEVICE_REAL_IDX_MASK);
#ifdef APCLI_SUPPORT
		if (FromWhichBSSID >= MIN_NET_DEVICE_FOR_APCLI) {
			dev_p = (infRealIdx >= MAX_APCLI_NUM ? NULL : pAd->ApCfg.ApCliTab[infRealIdx].wdev.if_dev);
			break;
		}
#endif /* APCLI_SUPPORT */

		if ((FromWhichBSSID > 0) &&
			(FromWhichBSSID < pAd->ApCfg.BssidNum) &&
			(FromWhichBSSID < MAX_MBSSID_NUM(pAd)) &&
			(FromWhichBSSID < HW_BEACON_MAX_NUM)) {
			dev_p = pAd->ApCfg.MBSSID[FromWhichBSSID].wdev.if_dev;
		} else
#endif /* CONFIG_AP_SUPPORT */
		{
			dev_p = pAd->net_dev;
		}
	} while (FALSE);

	ASSERT(dev_p);
	return dev_p; /* return one of MBSS */
}


#ifdef CONFIG_AP_SUPPORT
/*
========================================================================
Routine Description:
	Driver pre-Ioctl for AP.

Arguments:
	pAdSrc		- WLAN control block pointer
	pCB		- the IOCTL parameters

Return Value:
	0		No error
	Otherwise	-errno

Note:
========================================================================
*/
int RTMP_AP_IoctlPrepare(RTMP_ADAPTER *pAd, void *pCB)
{
	RT_CMD_AP_IOCTL_CONFIG *pConfig = (RT_CMD_AP_IOCTL_CONFIG *)pCB;
	POS_COOKIE pObj;
	USHORT index;
#ifdef CONFIG_APSTA_MIXED_SUPPORT
	int cmd = 0xff;
#endif /* CONFIG_APSTA_MIXED_SUPPORT */

	pObj = (POS_COOKIE) pAd->OS_Cookie;

	if ((pConfig->priv_flags == INT_MAIN) &&
			!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) {
		if (pConfig->pCmdData == NULL)
			return 0;

		if (RtPrivIoctlSetVal() == pConfig->CmdId_RTPRIV_IOCTL_SET) {
			if (TRUE
#ifdef CONFIG_APSTA_MIXED_SUPPORT
				&& (strstr(pConfig->pCmdData, "OpMode") == NULL)
#endif /* CONFIG_APSTA_MIXED_SUPPORT */
#ifdef SINGLE_SKU
				&& (strstr(pConfig->pCmdData, "ModuleTxpower") == NULL)
#endif /* SINGLE_SKU */
			) {
				return -ENETDOWN;
			}
		} else {
			return -ENETDOWN;
		}
	}

	/* determine this ioctl command is comming from which interface. */
	if (pConfig->priv_flags == INT_MAIN) {
		pObj->ioctl_if_type = INT_MAIN;
		pObj->ioctl_if = MAIN_MBSSID;
	} else if (pConfig->priv_flags == INT_MBSSID) {
		pObj->ioctl_if_type = INT_MBSSID;
		/* if (!RTMPEqualMemory(net_dev->name, pAd->net_dev->name, 3))  // for multi-physical card, no MBSSID */
		if (strcmp(pConfig->name, RtmpOsGetNetDevName(pAd->net_dev)) != 0) {
			/* sample */
			for (index = 1; index < pAd->ApCfg.BssidNum; index++) {
				if (pAd->ApCfg.MBSSID[index].wdev.if_dev == pConfig->net_dev) {
					pObj->ioctl_if = index;
					break;
				}
			}
			/* Interface not found! */
			if (index == pAd->ApCfg.BssidNum)
				return -ENETDOWN;
		} else {
			   /* ioctl command from I/F(ra0) */
			pObj->ioctl_if = MAIN_MBSSID;
		}
		MBSS_MR_APIDX_SANITY_CHECK(pAd, pObj->ioctl_if);
	}
#ifdef APCLI_SUPPORT
	else if (pConfig->priv_flags == INT_APCLI) {
		pObj->ioctl_if_type = INT_APCLI;
		for (index = 0; index < MAX_APCLI_NUM; index++) {
			if (pAd->ApCfg.ApCliTab[index].wdev.if_dev == pConfig->net_dev) {
				pObj->ioctl_if = index;
				break;
			}

			if (index == MAX_APCLI_NUM) {
				DBGPRINT(RT_DEBUG_ERROR, 
						("%s(): can not find Apcli I/F\n", 
						__FUNCTION__));
				return -ENETDOWN;
			}
		}
		APCLI_MR_APIDX_SANITY_CHECK(pObj->ioctl_if);
	}
#endif /* APCLI_SUPPORT */
	else {
	/* DBGPRINT(RT_DEBUG_WARN, ("IOCTL is not supported in WDS interface\n")); */
		return -EOPNOTSUPP;
	}

	pConfig->apidx = pObj->ioctl_if;
	return 0;
}


static void AP_E2PROM_IOCTL_PostCtrl(RTMP_IOCTL_INPUT_STRUCT *wrq, PSTRING msg)
{
	wrq->u.data.length = strlen(msg);
	if (copy_to_user(wrq->u.data.pointer, msg, wrq->u.data.length)) {
		DBGPRINT(RT_DEBUG_TRACE, ("%s: copy_to_user() fail\n", __FUNCTION__));
	}
}


static void IAPP_L2_UpdatePostCtrl(PRTMP_ADAPTER pAd, IN UINT8 *mac_p, int bssid)
{
}
#endif /* CONFIG_AP_SUPPORT */

#ifdef WDS_SUPPORT
VOID AP_WDS_KeyNameMakeUp(
	IN	STRING						*pKey,
	IN	UINT32						KeyMaxSize,
	IN	INT							KeyId)
{
	snprintf(pKey, KeyMaxSize, "Wds%dKey", KeyId);
}
#endif /* WDS_SUPPORT */

