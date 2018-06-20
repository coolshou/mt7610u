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
 *************************************************************************/
/****************************************************************************

    Module Name:
	rt_os_util.h

	Abstract:
	All function prototypes are provided from UTIL modules.

	Note:
	But can not use any OS key word and compile option here.
	All functions are provided from UTIL modules.


***************************************************************************/

#ifndef __RT_OS_UTIL_H__
#define __RT_OS_UTIL_H__

/* ============================ rt_linux.c ================================== */
/* General */
void RtmpUtilInit(void);

/* OS Time */
void RtmpusecDelay(ULONG usec);
void RtmpOsUsDelay(ULONG value);
void RtmpOsMsDelay(ULONG msec);
void RTMP_GetCurrentSystemTime(LARGE_INTEGER *time);
ULONG RTMPMsecsToJiffies(UINT msec);

void RTMP_GetCurrentSystemTick(ULONG *pNow);
void RtmpOsWait(UINT32 Time);
UINT32 RtmpOsTimerAfter(ULONG a, ULONG b);
UINT32 RtmpOsTimerBefore(ULONG a, ULONG b);
void RtmpOsGetSystemUpTime(ULONG *pTime);
UINT32 RtmpOsTickUnitGet(void);
void *os_alloc_mem(size_t size);
NDIS_STATUS os_alloc_mem_suspend(void *pReserved, UCHAR **mem, ULONG size);
void os_free_mem(void *mem);

NDIS_STATUS AdapterBlockAllocateMemory(
	IN	PVOID 	handle,
	OUT	PVOID 	*ppAd,
	IN	UINT32 	SizeOfpAd);

void *RtmpOsVmalloc(
	IN	ULONG 	Size);

void RtmpOsVfree(void *pMem);
ULONG RtmpOsCopyFromUser(void *to, const void *from, ULONG n);
ULONG RtmpOsCopyToUser(void *to, const void *from, ULONG n);
BOOLEAN RtmpOsStatsAlloc(void **ppStats, void **ppIwStats);

/* OS Packet */

PNDIS_PACKET RtmpOSNetPktAlloc(
	IN	void 	*pReserved,
	IN	int 		size);

PNDIS_PACKET RTMP_AllocateFragPacketBuffer(void *pReserved, ULONG Length);

NDIS_STATUS RTMPAllocateNdisPacket(void *pReserved, PNDIS_PACKET *ppPacket,
	PUCHAR pHeader, UINT HeaderLen, PUCHAR pData, UINT DataLen);

void RTMPFreeNdisPacket(
	IN	void 	*pReserved,
	IN	PNDIS_PACKET			pPacket);

NDIS_STATUS Sniff2BytesFromNdisBuffer(
	IN  PNDIS_BUFFER			pFirstBuffer,
	IN  UCHAR           		DesiredOffset,
	OUT PUCHAR          		pByte0,
	OUT PUCHAR          		pByte1);

void RTMP_QueryPacketInfo(PNDIS_PACKET pPacket, PACKET_INFO *pPacketInfo,
	PUCHAR *pSrcBufVA, UINT *pSrcBufLen);

PNDIS_PACKET DuplicatePacket(PNET_DEV pNetDev, PNDIS_PACKET pPacket,
	UCHAR FromWhichBSSID);

PNDIS_PACKET duplicate_pkt(PNET_DEV pNetDev, PUCHAR pHeader802_3, UINT HdrLen,
	PUCHAR pData, ULONG DataSize, UCHAR FromWhichBSSID);

PNDIS_PACKET duplicate_pkt_with_TKIP_MIC(void *pReserved, PNDIS_PACKET pOldPkt);

PNDIS_PACKET duplicate_pkt_with_VLAN(PNET_DEV pNetDev, USHORT VLAN_VID,
	USHORT VLAN_Priority, PUCHAR pHeader802_3, UINT HdrLen, PUCHAR pData,
	ULONG DataSize, UCHAR FromWhichBSSID, UCHAR *TPID);

typedef void (*RTMP_CB_8023_PACKET_ANNOUNCE)(void *pCtrlBkPtr, PNDIS_PACKET pPacket,
	UCHAR OpMode);

BOOLEAN RTMPL2FrameTxAction(void *pCtrlBkPtr, PNET_DEV pNetDev,
	RTMP_CB_8023_PACKET_ANNOUNCE _announce_802_3_packet, UCHAR apidx,
	PUCHAR pData, UINT32 data_len, UCHAR OpMode);

PNDIS_PACKET ExpandPacket(void *pReserved, PNDIS_PACKET pPacket, UINT32 ext_head_len,
	UINT32 ext_tail_len);

PNDIS_PACKET ClonePacket(void *pReserved, PNDIS_PACKET pPacket, PUCHAR pData,
	ULONG DataSize);

void wlan_802_11_to_802_3_packet(PNET_DEV pNetDev, UCHAR OpMode, USHORT VLAN_VID,
	USHORT VLAN_Priority, PNDIS_PACKET pRxPacket, UCHAR *pData, ULONG DataSize,
	PUCHAR pHeader802_3, UCHAR FromWhichBSSID, UCHAR *TPID);

void send_monitor_packets(
	IN	PNET_DEV pNetDev,
	IN	PNDIS_PACKET			pRxPacket,
	IN	PHEADER_802_11			pHeader,
	IN	UCHAR 	*pData,
	IN	USHORT 	DataSize,
	IN	UCHAR 	L2PAD,
	IN	UCHAR 	PHYMODE,
	IN	UCHAR 	BW,
	IN	UCHAR 	ShortGI,
	IN	UCHAR 	MCS,
	IN	UCHAR 	AMPDU,
	IN	UCHAR 	STBC,
	IN	UCHAR 	RSSI1,
	IN	UCHAR 	BssMonitorFlag11n,
	IN	UCHAR 	*pDevName,
	IN	UCHAR 	Channel,
	IN	UCHAR 	CentralChannel,
	IN	UINT32 	MaxRssi);

UCHAR VLAN_8023_Header_Copy(USHORT VLAN_VID, USHORT VLAN_Priority, PUCHAR pHeader802_3,
	UINT HdrLen, PUCHAR pData, UCHAR FromWhichBSSID, UCHAR *TPID);

void RtmpOsPktBodyCopy(
	IN	PNET_DEV pNetDev,
	IN	PNDIS_PACKET			pNetPkt,
	IN	ULONG 	ThisFrameLen,
	IN	PUCHAR 	pData);

INT RtmpOsIsPktCloned(
	IN	PNDIS_PACKET			pNetPkt);

PNDIS_PACKET RtmpOsPktCopy(
	IN	PNDIS_PACKET			pNetPkt);

PNDIS_PACKET RtmpOsPktClone(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktDataPtrAssign(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UCHAR 	*pData);

void RtmpOsPktLenAssign(
	IN	PNDIS_PACKET			pNetPkt,
	IN	LONG 	Len);

void RtmpOsPktTailAdjust(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT 	removedTagLen);

PUCHAR RtmpOsPktTailBufExtend(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT 	Len);

PUCHAR RtmpOsPktHeadBufExtend(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT 	Len);

void RtmpOsPktReserve(
	IN	PNDIS_PACKET			pNetPkt,
	IN	UINT 	Len);

//void RtmpOsPktProtocolAssign(
//	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktInfPpaSend(
	IN	PNDIS_PACKET			pNetPkt);


void RtmpOsPktNatMagicTag(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktNatNone(
	IN	PNDIS_PACKET			pNetPkt);

void RtmpOsPktRcvHandle(PNDIS_PACKET pNetPkt);
void RtmpOsPktInit(PNDIS_PACKET pNetPkt, PNET_DEV pNetDev, UCHAR *pData, USHORT DataSize);

PNDIS_PACKET RtmpOsPktIappMakeUp(
	IN	PNET_DEV pNetDev,
	IN	UINT8 	*pMac);

BOOLEAN RtmpOsPktOffsetInit(void);

/*
========================================================================
Routine Description:
	Initialize the OS atomic_t.
*/

UINT16 RtmpOsNtohs(
	IN	UINT16 	Value);

UINT16 RtmpOsHtons(
	IN	UINT16 	Value);

UINT32 RtmpOsNtohl(
	IN	UINT32 	Value);

UINT32 RtmpOsHtonl(
	IN	UINT32 	Value);

/* OS File */
RTMP_OS_FD RtmpOSFileOpen(char *pPath,  int flag, int mode);
int RtmpOSFileClose(RTMP_OS_FD osfd);
void RtmpOSFileSeek(RTMP_OS_FD osfd, int offset);
int RtmpOSFileRead(RTMP_OS_FD osfd, char *pDataPtr, int readLen);
int RtmpOSFileWrite(RTMP_OS_FD osfd, char *pDataPtr, int writeLen);
void RtmpOSFSInfoChange(RTMP_OS_FS_INFO *pOSFSInfoOrg, BOOLEAN bSet);

INT32 RtmpOsFileIsErr(
	IN	void *pFile);


/* OS Network Interface */
int RtmpOSNetDevAddrSet(UCHAR OpMode, PNET_DEV pNetDev, PUCHAR pMacAddr,
	PUCHAR dev_name);

void RtmpOSNetDevFree(PNET_DEV pNetDev);

void RtmpOSNetDevClose(
	IN PNET_DEV 	pNetDev);


INT RtmpOSNetDevAlloc(
	IN	PNET_DEV *new_dev_p,
	IN	UINT32 	privDataSize);

INT RtmpOSNetDevOpsAlloc(
	IN	PVOID 	*pNetDevOps);


PNET_DEV RtmpOSNetDevGetByName(
	IN	PNET_DEV pNetDev,
	IN	PSTRING 	pDevName);

void RtmpOSNetDeviceRefPut(
	IN	PNET_DEV pNetDev);

INT RtmpOSNetDevDestory(
	IN	void 	*pReserved,
	IN	PNET_DEV pNetDev);

void RtmpOSNetDevDetach(PNET_DEV pNetDev);
int RtmpOSNetDevAttach(UCHAR OpMode, PNET_DEV pNetDev, RTMP_OS_NETDEV_OP_HOOK *pDevOpHook);

void RtmpOSNetDevProtect(
	IN BOOLEAN lock_it);

PNET_DEV RtmpOSNetDevCreate(INT32 MC_RowID, UINT32 *pIoctlIF, int devType,
	int devNum, int privMemSize, PSTRING pNamePrefix);

BOOLEAN RtmpOSNetDevIsUp(void *pDev);

unsigned char *RtmpOsNetDevGetPhyAddr(void *pDev);
char *RtmpOsGetNetDevName(void *pDev);

void RtmpOsNetQueueStart(PNET_DEV pDev);

void RtmpOsNetQueueStop(PNET_DEV pDev);

void RtmpOsNetQueueWake(PNET_DEV pDev);

void RtmpOsSetPktNetDev(void *pPkt, void *pDev);

PNET_DEV RtmpOsPktNetDevGet(void *pPkt);

void RtmpOsSetNetDevPriv(void *pDev, void *pPriv);
void *RtmpOsGetNetDevPriv(void *pDev);

USHORT RtmpDevPrivFlagsGet(void *pDev);
void RtmpDevPrivFlagsSet(void *pDev, USHORT PrivFlags);

void RtmpOsSetNetDevType(void *pDev, USHORT Type);

void RtmpOsSetNetDevTypeMonitor(void *pDev);


/* OS Semaphore */
void RtmpOsCmdUp(RTMP_OS_TASK *pCmdQTask);
void RtmpOsMlmeUp(RTMP_OS_TASK *pMlmeQTask);
BOOLEAN RtmpOsSemaInitLocked(RTMP_OS_SEM *pSemOrg, LIST_HEADER *pSemList);
BOOLEAN RtmpOsSemaInit(RTMP_OS_SEM *pSemOrg, LIST_HEADER *pSemList);
BOOLEAN RtmpOsSemaDestroy(RTMP_OS_SEM *pSemOrg);
INT RtmpOsSemaWaitInterruptible(RTMP_OS_SEM *pSemOrg);
void RtmpOsSemaWakeUp(RTMP_OS_SEM *pSemOrg);


void RtmpOsInitCompletion(RTMP_OS_COMPLETION *pCompletion);
void RtmpOsExitCompletion(RTMP_OS_COMPLETION *pCompletion);
void RtmpOsComplete(RTMP_OS_COMPLETION *pCompletion);
ULONG RtmpOsWaitForCompletionTimeout(RTMP_OS_COMPLETION *pCompletion, ULONG Timeout); 

/* OS Task */
BOOLEAN RtmpOsTaskletSche(RTMP_NET_TASK_STRUCT *pTasklet);

BOOLEAN RtmpOsTaskletInit(
	RTMP_NET_TASK_STRUCT *pTasklet,
	void (*pFunc)(unsigned long data),
	ULONG Data,
	LIST_HEADER *pTaskletList);

BOOLEAN RtmpOsTaskletKill(RTMP_NET_TASK_STRUCT *pTasklet);

void RtmpOsTaskletDataAssign(
	RTMP_NET_TASK_STRUCT *pTasklet,
	ULONG Data);

void RtmpOsTaskWakeUp(RTMP_OS_TASK *pTaskOrg);

INT32 RtmpOsTaskIsKilled(RTMP_OS_TASK *pTaskOrg);

BOOLEAN RtmpOsCheckTaskLegality(RTMP_OS_TASK *pTaskOrg);

BOOLEAN RtmpOSTaskAlloc(
	IN	RTMP_OS_TASK			*pTask,
	IN	LIST_HEADER *pTaskList);

void RtmpOSTaskFree(
	IN	RTMP_OS_TASK			*pTask);

NDIS_STATUS RtmpOSTaskKill(
	IN	RTMP_OS_TASK			*pTaskOrg);

INT RtmpOSTaskNotifyToExit(
	IN	RTMP_OS_TASK			*pTaskOrg);

void RtmpOSTaskCustomize(
	IN	RTMP_OS_TASK			*pTaskOrg);

NDIS_STATUS RtmpOSTaskAttach(
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	RTMP_OS_TASK_CALLBACK	fn,
	IN	ULONG 	arg);

NDIS_STATUS RtmpOSTaskInit(
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	PSTRING 	pTaskName,
	IN	void 	*pPriv,
	IN	LIST_HEADER *pTaskList,
	IN	LIST_HEADER *pSemList);

BOOLEAN RtmpOSTaskWait(
	IN	void 	*pReserved,
	IN	RTMP_OS_TASK			*pTaskOrg,
	IN	INT32 	*pStatus);

void *RtmpOsTaskDataGet(RTMP_OS_TASK *pTaskOrg);

INT32 RtmpThreadPidKill(RTMP_OS_PID	 PID);

/* OS Cache */
void RtmpOsDCacheFlush(ULONG AddrStart, ULONG Size);

/* OS Timer */
void RTMP_SetPeriodicTimer(NDIS_MINIPORT_TIMER *pTimerOrg, unsigned long timeout);

void RTMP_OS_Init_Timer(void *pReserved, NDIS_MINIPORT_TIMER *pTimerOrg,
	TIMER_FUNCTION function, PVOID data, LIST_HEADER *pTimerList);

void RTMP_OS_Add_Timer(NDIS_MINIPORT_TIMER *pTimerOrg, unsigned long timeout);
void RTMP_OS_Mod_Timer(NDIS_MINIPORT_TIMER *pTimerOrg, unsigned long timeout);
void RTMP_OS_Del_Timer(NDIS_MINIPORT_TIMER *pTimerOrg, BOOLEAN *pCancelled);

void RTMP_OS_Release_Timer(NDIS_MINIPORT_TIMER *pTimerOrg);

BOOLEAN RTMP_OS_Alloc_Rsc(
	IN	LIST_HEADER *pRscList,
	IN	void  	*pRsc,
	IN	UINT32 	RscLen);

void RTMP_OS_Free_Rscs(
	IN	LIST_HEADER *pRscList);

/* OS Lock */
BOOLEAN RtmpOsAllocateLock(
	IN	NDIS_SPIN_LOCK			*pLock,
	IN	LIST_HEADER *pLockList);

void RtmpOsFreeSpinLock(
	IN	NDIS_SPIN_LOCK			*pLockOrg);

void RtmpOsSpinLockBh(
	IN	NDIS_SPIN_LOCK			*pLockOrg);

void RtmpOsSpinUnLockBh(NDIS_SPIN_LOCK *pLockOrg);
void RtmpOsIntLock(NDIS_SPIN_LOCK *pLockOrg, ULONG *pIrqFlags);
void RtmpOsIntUnLock(NDIS_SPIN_LOCK *pLockOrg, ULONG IrqFlags);

/* OS PID */
void RtmpOsGetPid(ULONG *pDst, ULONG PID);
void RtmpOsTaskPidInit(RTMP_OS_PID *pPid);

/* OS I/O */
void RTMP_PCI_Writel(ULONG Value, void *pAddr);
void RTMP_PCI_Writew(ULONG Value, void *pAddr);
void RTMP_PCI_Writeb(ULONG Value, void *pAddr);
ULONG RTMP_PCI_Readl(void *pAddr);
ULONG RTMP_PCI_Readw(void *pAddr);
ULONG RTMP_PCI_Readb(void *pAddr);

int RtmpOsPciConfigReadWord(
	IN	void 	*pDev,
	IN	UINT32 	Offset,
	OUT UINT16 	*pValue);

int RtmpOsPciConfigWriteWord(
	IN	void 	*pDev,
	IN	UINT32 	Offset,
	IN	UINT16 	Value);

int RtmpOsPciConfigReadDWord(
	IN	void 	*pDev,
	IN	UINT32 	Offset,
	OUT UINT32 	*pValue);

int RtmpOsPciConfigWriteDWord(
	IN	void 	*pDev,
	IN	UINT32 	Offset,
	IN	UINT32 	Value);

int RtmpOsPciFindCapability(
	IN	void 	*pDev,
	IN	int 		Cap);

void *RTMPFindHostPCIDev(void *pPciDevSrc);

int RtmpOsPciMsiEnable(void *pDev);
void RtmpOsPciMsiDisable(void *pDev);

/* OS Wireless */
ULONG RtmpOsMaxScanDataGet(void);

/* OS Interrutp */
INT32 RtmpOsIsInInterrupt(void);

/* OS USB */
void *RtmpOsUsbUrbDataGet(void *pUrb);
NTSTATUS RtmpOsUsbUrbStatusGet(void *pUrb);
ULONG RtmpOsUsbUrbLenGet(void *pUrb);

/* OS Atomic */
BOOLEAN RtmpOsAtomicInit(RTMP_OS_ATOMIC *pAtomic, LIST_HEADER *pAtomicList);
void RtmpOsAtomicDestroy(RTMP_OS_ATOMIC *pAtomic);
LONG RtmpOsAtomicRead(RTMP_OS_ATOMIC *pAtomic);
void RtmpOsAtomicDec(RTMP_OS_ATOMIC *pAtomic);
void RtmpOsAtomicInterlockedExchange(RTMP_OS_ATOMIC *pAtomicSrc, LONG Value);

/* OS Utility */
void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);

typedef void (*RTMP_OS_SEND_WLAN_EVENT)(void *pAdSrc, USHORT Event_flag,
	PUCHAR pAddr, UCHAR BssIdx, CHAR Rssi);

void RtmpOsSendWirelessEvent(void *pAd, USHORT Event_flag, PUCHAR pAddr,
	UCHAR BssIdx, CHAR Rssi, RTMP_OS_SEND_WLAN_EVENT pFunc);

int RtmpOSWrielessEventSend(PNET_DEV pNetDev, UINT32 eventType, int flags,
	PUCHAR pSrcMac, PUCHAR pData, UINT32 dataLen);

int RtmpOSWrielessEventSendExt(PNET_DEV pNetDev, UINT32 eventType, int flags,
	PUCHAR pSrcMac, PUCHAR pData, UINT32 dataLen, UINT32 family);

UINT RtmpOsWirelessExtVerGet(void);

void RtmpDrvAllMacPrint(void *pReserved, UINT32 *pBufMac, UINT32 AddrStart,
	UINT32 AddrEnd, UINT32 AddrStep);

void RtmpDrvAllE2PPrint(void *pReserved, USHORT *pMacContent, UINT32 AddrEnd,
	UINT32 AddrStep);

void RtmpDrvAllRFPrint(void *pReserved, UCHAR *pBuf, UINT32 BufLen);
void RtmpOsWlanEventSet(void *pReserved, BOOLEAN *pCfgWEnt, BOOLEAN FlgIsWEntSup);

int RtmpOSIRQRelease(
	IN	PNET_DEV pNetDev,
	IN	UINT32 	infType,
	IN	PPCI_DEV pci_dev,
	IN	BOOLEAN 	*pHaveMsi);


UINT16 RtmpOsGetUnaligned(UINT16 *pWord);
UINT32 RtmpOsGetUnaligned32(UINT32 *pWord);
ULONG RtmpOsGetUnalignedlong(ULONG *pWord);
long RtmpOsSimpleStrtol(const char *cp, char **endp, unsigned int base);

void RtmpOsOpsInit(RTMP_OS_ABL_OPS *pOps);

/* ============================ rt_os_util.c ================================ */
void RtmpDrvRateGet(void *pReserved, UINT8 MODE, UINT8 ShortGI, UINT8 BW,
	UINT8 MCS, UINT8 Antenna, ULONG *pRate);

char * rtstrchr(const char * s, int c);

PSTRING   WscGetAuthTypeStr(USHORT authFlag);

PSTRING   WscGetEncryTypeStr(USHORT encryFlag);

USHORT WscGetAuthTypeFromStr(PSTRING arg);

USHORT WscGetEncrypTypeFromStr(PSTRING arg);

void RtmpMeshDown(
	IN void *pDrvCtrlBK,
	IN BOOLEAN WaitFlag,
	IN BOOLEAN (*RtmpMeshLinkCheck)(IN void *pAd));

USHORT RtmpOsNetPrivGet(PNET_DEV pDev);

BOOLEAN RtmpOsCmdDisplayLenCheck(
	IN	UINT32 	LenSrc,
	IN	UINT32 	Offset);

void WpaSendMicFailureToWpaSupplicant(PNET_DEV pNetDev, const PUCHAR src_addr,
	BOOLEAN bUnicast, int key_id, const PUCHAR tsc);

int wext_notify_event_assoc(PNET_DEV pNetDev, UCHAR *ReqVarIEs, UINT32 ReqVarIELen);

void SendAssocIEsToWpaSupplicant(PNET_DEV pNetDev, UCHAR *ReqVarIEs, UINT32 ReqVarIELen);

/* ============================ rt_rbus_pci_util.c ========================== */
void RtmpAllocDescBuf(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT void **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RtmpFreeDescBuf(
	IN PPCI_DEV pPciDev,
	IN ULONG Length,
	IN void *VirtualAddress,
	IN NDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RTMP_AllocateFirstTxBuffer(
	IN PPCI_DEV pPciDev,
	IN UINT Index,
	IN ULONG Length,
	IN BOOLEAN Cached,
	OUT void **VirtualAddress,
	OUT PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

void RTMP_FreeFirstTxBuffer(
	IN	PPCI_DEV pPciDev,
	IN	ULONG 	Length,
	IN	BOOLEAN 	Cached,
	IN	PVOID 	VirtualAddress,
	IN	NDIS_PHYSICAL_ADDRESS	PhysicalAddress);

PNDIS_PACKET RTMP_AllocateRxPacketBuffer(
	IN	void 	*pReserved,
	IN	void 	*pPciDev,
	IN	ULONG 	Length,
	IN	BOOLEAN 	Cached,
	OUT	PVOID 	*VirtualAddress,
	OUT	PNDIS_PHYSICAL_ADDRESS	PhysicalAddress);

#ifdef CONFIG_STA_SUPPORT
#ifdef CONFIG_PM
#ifdef USB_SUPPORT_SELECTIVE_SUSPEND

int RTMP_Usb_AutoPM_Put_Interface(void *pUsb_Dev, void *intf);
int RTMP_Usb_AutoPM_Get_Interface(void *pUsb_Dev, void *intf);

#endif /* USB_SUPPORT_SELECTIVE_SUSPEND */
#endif /* CONFIG_PM */
#endif /* CONFIG_STA_SUPPORT */



ra_dma_addr_t linux_pci_map_single(void *pPciDev, void *ptr, size_t size, int sd_idx, int direction);

void linux_pci_unmap_single(void *pPciDev, ra_dma_addr_t dma_addr, size_t size, int direction);

/* ============================ rt_usb_util.c =============================== */
#ifdef RTMP_MAC_USB
typedef void (*USB_COMPLETE_HANDLER)(void *);

void dump_urb(void *purb);

int rausb_register(void * new_driver);

void rausb_deregister(void * driver);

/*struct urb *rausb_alloc_urb(int iso_packets); */

void rausb_free_urb(void *urb);

void rausb_put_dev(void *dev);

struct usb_device *rausb_get_dev(void *dev);

int rausb_submit_urb(void *urb);

void *rausb_buffer_alloc(void *dev,
 			size_t size,
 			ra_dma_addr_t *dma);

void rausb_buffer_free(void *dev,
 			size_t size,
 			void *addr,
 			ra_dma_addr_t dma);

int rausb_control_msg(void *dev,
 		unsigned int pipe,
 		__u8 request,
 		__u8 requesttype,
 		__u16 value,
 		__u16 index,
 		void *data,
 		__u16 size,
 		int timeout);

void rausb_fill_bulk_urb(void *urb,
 		 void *dev,
 		 unsigned int pipe,
 		 void *transfer_buffer,
 		 int buffer_length,
 		 USB_COMPLETE_HANDLER complete_fn,
 		 void *context);

unsigned int rausb_sndctrlpipe(void *dev, ULONG address);

unsigned int rausb_rcvctrlpipe(void *dev, ULONG address);


unsigned int rausb_sndbulkpipe(void *dev, ULONG address);
unsigned int rausb_rcvbulkpipe(void *dev, ULONG address);

void rausb_kill_urb(void *urb);

void RtmpOsUsbEmptyUrbCheck(
	IN	void **ppWait,
	IN	NDIS_SPIN_LOCK		*pBulkInLock,
	IN	UCHAR *pPendingRx);


void	RtmpOsUsbInitHTTxDesc(
	IN	void			*pUrbSrc,
	IN	void			*pUsb_Dev,
	IN	UINT			BulkOutEpAddr,
	IN	PUCHAR			pSrc,
	IN	ULONG			BulkOutSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	void			*pTxContext,
	IN	ra_dma_addr_t		TransferDma);

void	RtmpOsUsbInitRxDesc(
	IN	void			*pUrbSrc,
	IN	void			*pUsb_Dev,
	IN	UINT			BulkInEpAddr,
	IN	UCHAR			*pTransferBuffer,
	IN	UINT32			BufSize,
	IN	USB_COMPLETE_HANDLER	Func,
	IN	void			*pRxContext,
	IN	ra_dma_addr_t		TransferDma);

void *RtmpOsUsbContextGet(
	IN	void			*pUrb);

NTSTATUS RtmpOsUsbStatusGet(
	IN	void			*pUrb);

void RtmpOsUsbDmaMapping(
	IN	void			*pUrb);
#endif /* RTMP_MAC_USB */

#if defined(RTMP_RBUS_SUPPORT) || defined(RTMP_FLASH_SUPPORT)
void RtmpFlashRead(UCHAR *p, ULONG a, ULONG b);
void RtmpFlashWrite(UCHAR *p, ULONG a, ULONG b);
#endif /* defined(RTMP_RBUS_SUPPORT) || defined(RTMP_FLASH_SUPPORT) */

UINT32 RtmpOsGetUsbDevVendorID(void *pUsbDev);
UINT32 RtmpOsGetUsbDevProductID(void *pUsbDev);

/* CFG80211 */
#ifdef RT_CFG80211_SUPPORT
typedef struct __CFG80211_BAND {
	UINT8 RFICType;
	UINT8 MpduDensity;
	UINT8 TxStream;
	UINT8 RxStream;
	UINT32 MaxTxPwr;
	UINT32 MaxBssTable;
	UINT16 RtsThreshold;
	UINT16 FragmentThreshold;
	UINT32 RetryMaxCnt; /* bit0~7: short; bit8 ~ 15: long */
	BOOLEAN FlgIsBMode;
} CFG80211_BAND;

void CFG80211OS_UnRegister(void *pCB, void *pNetDev);

BOOLEAN CFG80211_SupBandInit(void *pCB, CFG80211_BAND *pBandInfo, void *pWiphyOrg,
	void *pChannelsOrg, void *pRatesOrg);

BOOLEAN CFG80211OS_SupBandReInit(void *pCB, CFG80211_BAND *pBandInfo);
void CFG80211OS_RegHint(void *pCB, UCHAR *pCountryIe, ULONG CountryIeLen);
void CFG80211OS_RegHint11D(void *pCB, UCHAR *pCountryIe, ULONG CountryIeLen);

BOOLEAN CFG80211OS_BandInfoGet(void *pCB, void *pWiphyOrg, void **ppBand24,
	void **ppBand5);

UINT32 CFG80211OS_ChanNumGet(void *pCB, void *pWiphyOrg, UINT32 IdBand);

BOOLEAN CFG80211OS_ChanInfoGet(void *pCB, void *pWiphyOrg, UINT32 IdBand,
	UINT32 IdChan, UINT32 *pChanId, UINT32 *pPower, BOOLEAN *pFlgIsRadar);

BOOLEAN CFG80211OS_ChanInfoInit(void *pCB, UINT32 InfoIndex, UCHAR ChanId,
	UCHAR MaxTxPwr, BOOLEAN FlgIsNMode, BOOLEAN FlgIsBW20M);

void CFG80211OS_Scaning(void *pCB, UINT32 ChanId, UCHAR *pFrame, UINT32 FrameLen,
	INT32 RSSI, BOOLEAN FlgIsNMode, UINT8 BW);

void CFG80211OS_ScanEnd(void *pCB, BOOLEAN FlgIsAborted);

void CFG80211OS_ConnectResultInform(void *pCB, UCHAR *pBSSID, UCHAR *pReqIe,
	UINT32 ReqIeLen, UCHAR *pRspIe, UINT32 RspIeLen, UCHAR FlgIsSuccess);

#endif /* RT_CFG80211_SUPPORT */




/* ================================ MACRO =================================== */
#define RTMP_UTIL_DCACHE_FLUSH(__AddrStart, __Size)

/* ================================ EXTERN ================================== */
extern UCHAR SNAP_802_1H[6];
extern UCHAR SNAP_BRIDGE_TUNNEL[6];
extern UCHAR EAPOL[2];
extern UCHAR TPID[];
extern UCHAR IPX[2];
extern UCHAR APPLE_TALK[2];
extern UCHAR NUM_BIT8[8];
extern ULONG RTPktOffsetData, RTPktOffsetLen, RTPktOffsetCB;

extern ULONG OS_NumOfMemAlloc, OS_NumOfMemFree;

extern INT32 ralinkrate[];
extern UINT32 RT_RateSize;

#ifdef PLATFORM_UBM_IPX8
#include "vrut_ubm.h"
#endif /* PLATFORM_UBM_IPX8 */

INT32  RtPrivIoctlSetVal(void);

void OS_SPIN_LOCK(spinlock_t *lock);
void OS_SPIN_UNLOCK(spinlock_t *lock);
void OS_SPIN_LOCK_IRQSAVE(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void OS_SPIN_UNLOCK_IRQRESTORE(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void OS_SPIN_LOCK_IRQ(NDIS_SPIN_LOCK *lock);
void OS_SPIN_UNLOCK_IRQ(NDIS_SPIN_LOCK *lock);
void RtmpOsSpinLockIrqSave(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void RtmpOsSpinUnlockIrqRestore(NDIS_SPIN_LOCK *lock, unsigned long *flags);
void RtmpOsSpinLockIrq(NDIS_SPIN_LOCK *lock);
void RtmpOsSpinUnlockIrq(NDIS_SPIN_LOCK *lock);
int  OS_TEST_BIT(int bit, unsigned long *flags);
void OS_SET_BIT(int bit, unsigned long *flags);
void OS_CLEAR_BIT(int bit, unsigned long *flags);

#endif /* __RT_OS_UTIL_H__ */
