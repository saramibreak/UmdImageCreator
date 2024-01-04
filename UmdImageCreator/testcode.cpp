/**
 * Copyright 2018-2024 sarami
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <pspsdk.h>
#include <pspkernel.h>

#include "define.h"
#include "execScsiCmdforFileSystem.h"
#include "output.h"
#include "get.h"
#include "_external/pspUmdMan_driver.h"

#if 0
int ExitCallback(int arg1, int arg2, void* arg)
{
	return 0;
}
#endif
void searchCommand()
{
	pspPrintf(
		"\nsearchCommand mode\n"
		"The result is outputted to ms0:/cmdtest.txt\n");
	const char cmdtest[] = "ms0:/cmdtest.txt";
	SceUID uidcmd = sceIoOpen(cmdtest, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (uidcmd < 0) {
		OutputPspError("sceIoOpen", cmdtest, 0, uidcmd);
		return;
	}
	PrepareOpeningDisc();
//	char* file0 = DEVICE_DISC"/UMD_DATA.BIN";
	const char* file1 = DEVICE_DISC"/PSP_GAME/SYSDIR/BOOT.BIN";
	SceUID uidDisc = sceIoOpen(file1, PSP_O_RDONLY, 0);
	if (uidDisc < 0) {
		OutputPspError("sceIoOpen", file1, 0, uidDisc);
		return;
	}
	SceUID uidUmd = sceIoOpen(DEVICE_UMD, PSP_O_RDONLY, 0);
	if (uidUmd < 0) {
		OutputPspError("sceIoOpen", DEVICE_UMD, 0, uidUmd);
		return;
	}
#if 0
	int callbackId = sceKernelCreateCallback("ExitCallback", ExitCallback, NULL);
	if (callbackId < 0) {
		pspPrintf("Cannot execute CreateCallback: result=%#08x\n", callbackId);
	}

	unsigned int result = sceIoSetAsyncCallback(uid, callbackId, NULL);
	if (result < 0) {
		pspPrintf("Cannot execute sceIoSetAsyncCallback: result=%#08x\n", result);
	}
#endif
	unsigned char src[2048] = { 0 };
	unsigned char dst[2048] = { 0 };
	char ret[64] = { 0 };
	unsigned int result = 0;

	// dst null, src null
	for (int i = 0x01000000; i < 0x02000000; i += 0x100000) {
		for (int j = 0; j < 0x10000; j++) {
			if (sceKernelDevkitVersion() <= 0x01050001 && i + j == 0x01f000a3) {
				// FW 1.50 crash
				continue;
			}
			result = sceIoIoctl(uidDisc, i + j, NULL, 0, NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (disc0): 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
			result = sceIoIoctl(uidUmd, i + j, NULL, 0, NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (umd0) : 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#if 0
			result = sceIoIoctlAsync(uid, i + j, NULL, 0, NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoIoctlAsync: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#endif
			result = sceIoDevctl(DEVICE_UMD, i + j, NULL, 0, NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "     sceIoDevctl: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#ifdef PBP
			pspPrintf("\rexecute ioctl %#x", i + j);
#else
			pspDebugScreenSetXY(0, pspDebugScreenGetY());
			pspPrintf("execute ioctl %#x", i + j);
#endif
		}
	}
	pspPrintf("\n");
	// dst null, src not null
	for (int i = 0x01000000; i < 0x02000000; i += 0x100000) {
		for (int j = 0x10000; j < 0x20000; j++) {
			result = sceIoIoctl(uidDisc, i + j, src, sizeof(src), NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (disc0): 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
			result = sceIoIoctl(uidUmd, i + j, src, sizeof(src), NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (umd0) : 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#if 0
			result = sceIoIoctlAsync(uid, i + j, src, sizeof(src), NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoIoctlAsync: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#endif
			result = sceIoDevctl(DEVICE_UMD, i + j, src, sizeof(src), NULL, 0);
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "     sceIoDevctl: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#ifdef PBP
			pspPrintf("\rexecute ioctl %#x", i + j);
#else
			pspDebugScreenSetXY(0, pspDebugScreenGetY());
			pspPrintf("execute ioctl %#x", i + j);
#endif
		}
	}
	pspPrintf("\n");
	// dst not null, src null
	for (int i = 0x01000000; i < 0x02000000; i += 0x100000) {
		for (int j = 0x20000; j < 0x30000; j++) {
			result = sceIoIoctl(uidDisc, i + j, NULL, 0, dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (disc0): 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
			result = sceIoIoctl(uidUmd, i + j, NULL, 0, dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (umd0) : 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#if 0
			result = sceIoIoctlAsync(uid, i + j, NULL, 0, dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoIoctlAsync: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#endif
			result = sceIoDevctl(DEVICE_UMD, i + j, NULL, 0, dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "     sceIoDevctl: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#ifdef PBP
			pspPrintf("\rexecute ioctl %#x", i + j);
#else
			pspDebugScreenSetXY(0, pspDebugScreenGetY());
			pspPrintf("execute ioctl %#x", i + j);
#endif
		}
	}
	pspPrintf("\n");
	// dst not null, src not null
	for (int i = 0x01000000; i < 0x02000000; i += 0x100000) {
		for (int j = 0x30000; j < 0x40000; j++) {
			result = sceIoIoctl(uidDisc, i + j, src, sizeof(src), dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (disc0): 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
			result = sceIoIoctl(uidUmd, i + j, src, sizeof(src), dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "sceIoctl (umd0) : 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#if 0
			result = sceIoIoctlAsync(uid, i + j, src, sizeof(src), dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "   sceIoIoctlAsync: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#endif
			result = sceIoDevctl(DEVICE_UMD, i + j, src, sizeof(src), dst, sizeof(dst));
			if (result != 0x8001b000 && result != 0x80020321 && result != 0x80020325 && result != 0x800200d1) {
				sprintf(ret, "     sceIoDevctl: 0x%08x result = 0x%08x\n", i + j, result);
				sceIoWrite(uidcmd, ret, strlen(ret));
			}
#ifdef PBP
			pspPrintf("\rexecute ioctl %#x", i + j);
#else
			pspDebugScreenSetXY(0, pspDebugScreenGetY());
			pspPrintf("execute ioctl %#x", i + j);
#endif
		}
	}
	pspPrintf("\nDone\n");
	sceIoClose(uidDisc);
	sceIoClose(uidUmd);
	sceIoClose(uidcmd);
}

void testCommand()
{
	pspPrintf(
		"\ntestCommand mode\n"
		"The result is outputted to ms0:/disc_0x[8 digit].bin\n");

	PrepareOpeningDisc();
	const char umddata[] = DEVICE_DISC"/UMD_DATA.BIN";
	SceUID uid = sceIoOpen(umddata, PSP_O_RDONLY, 0);
	if (uid < 0) {
		OutputPspError("sceIoOpen", umddata, 0, uid);
		return;
	}

	SceUID fd = 0;
	unsigned int cmd[] = { 0x01020001, 0x01020002, 0x01020003, 0x01020004, 0x01020006, 0x01020007, 0 };
	unsigned char dst[0x800] = { 0 };
	for (int n = 0; cmd[n] != 0; n++) {
		unsigned int ret = sceIoIoctl(uid, cmd[n], NULL, 0, dst, sizeof(dst));
		if (0x80000000 <= ret) {
			OutputPspError("sceIoIoctl", umddata, cmd[n], ret);
		}
		else {
			char str[32] = {};
			sprintf(str, "ms0:/disc_0x%08x.bin", cmd[n]);
			fd = sceIoOpen(str, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
			sceIoWrite(fd, &dst, sizeof(dst));
			sceIoClose(fd);
		}
		memset(dst, 0, sizeof(dst));
#ifdef PBP
		pspPrintf("\rexecute ioctl 0x%08x\n", cmd[n]);
#else
		pspDebugScreenSetXY(0, pspDebugScreenGetY());
		pspPrintf("execute ioctl 0x%08x", cmd[n]);
#endif
	}
	sceIoClose(uid);
	pspPrintf("\n");

	unsigned int devcmd[] = { 0x01f200a0, 0x01f200a1, 0x01f200a2, 0x01f200a4, 0x01f200a5, 0x01f21011, 0 };
	unsigned char buf[0x60] = { 0 };
	for (int n = 0; devcmd[n] != 0; n++) {
		unsigned int ret = sceIoDevctl(DEVICE_UMD, devcmd[n], NULL, 0, buf, sizeof(buf));
		if (0x80000000 <= ret) {
			OutputPspError("sceIoDevctl", DEVICE_UMD, devcmd[n], ret);
		}
		else {
			char devstr[23] = {};
			sprintf(devstr, "ms0:/umd_%#08x.bin", devcmd[n]);
			fd = sceIoOpen(devstr, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
			sceIoWrite(fd, &buf, sizeof(buf));
			sceIoClose(fd);
		}
		memset(buf, 0, sizeof(buf));
#ifdef PBP
		pspPrintf("\rexecute ioDevctl 0x%08x\n", devcmd[n]);
#else
		pspDebugScreenSetXY(0, pspDebugScreenGetY());
		pspPrintf("execute ioDevctl 0x%08x", devcmd[n]);
#endif
	}
	pspPrintf("\nDone\n");
}

void testNewCommand()
{
	unsigned int ret = 0;
	int nReady = 0;
	int res = 0;

	void* pUmdDrive = _sceUmdManGetUmdDrive(0);
	pspPrintf("_sceUmdManGetUmdDrive(0): %p\n", pUmdDrive);
	void* pUmdInfo = _sceUmdManGetUmdDiscInfo();
	pspPrintf("_sceUmdManGetUmdDiscInfo(): %p\n", pUmdInfo);

	_sceUmdManWaitSema();
	ret = _sceUmdManCheckDeviceReady(pUmdDrive, DEVICE_UMD);
	_sceUmdManSignalSema();
	if (0x80000000 <= ret) {
		OutputPspError("_sceUmdManCheckDeviceReady", " ", 0, ret);
		sceKernelDelayThread(5 * 1000000);
	}
	else if (ret == 0) {
		pspPrintf("Device is ready\n");
		nReady = TRUE;
	}

	if (!nReady) {
		while (1) {
			ret = _sceUmdManActivate();
			if (ret == 0x80210003) {
				pspPrintf("\rNo media. Please insert UMD");
				continue;
			}
			if (0x80000000 <= ret) {
				OutputPspError("_sceUmdManActivate", " ", 0, ret);
				sceKernelDelayThread(5 * 1000000);
			}
			pspPrintf(" -> Media is inserted\n");
			break;
		}
	}
	SceUID uid = 0;
#define TEST_DISC_INFO 1
#if TEST_DISC_INFO
	typedef struct SceUmdDiscInfo {
		unsigned int uiSize;
		unsigned int uiMediaType;
	} SceUmdDiscInfo;
	SceUmdDiscInfo discinfo = { 0, 0 };

	res = _sceUmdManGetDiscInfo(&discinfo);
	uid = sceIoOpen("ms0:_sceUmdManGetDiscInfo.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
	sceIoWrite(uid, &discinfo, sizeof(discinfo));
	sceIoClose(uid);
	pspPrintf("_sceUmdManGetDiscInfo.bin is generated\n");

	memset(&discinfo, 0, sizeof(discinfo));

	res = _sceUmdManGetDiscInfo4VSH(&discinfo);
	if (res < 0) {
		OutputPspError("_sceUmdManGetDiscInfo4VSH", " ", 0, res);
		sceKernelDelayThread(5 * 1000000);
	}
	else {
		uid = sceIoOpen("ms0:_sceUmdManGetDiscInfo4VSH.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, &discinfo, sizeof(discinfo));
		sceIoClose(uid);
		pspPrintf("_sceUmdManGetDiscInfo4VSH.bin is generated\n");
	}
#endif
#define TEST_GET_INQUIRY 1
#if TEST_GET_INQUIRY
	unsigned char buf[56] = {};
	// Same as sceIoDevctl(DEVICE_UMD, IOCTL_UMD_GET_INQUIRY_DATA
	// sceUmdExecInquiryCmd is wrapped by sceUmdManGetInquiry
	res = _sceUmdManGetInquiry(pUmdDrive, 56, buf);
	if (res < 0) {
		OutputPspError("_sceUmdManGetInquiry", " ", 0, res);
	}
	else {
		uid = sceIoOpen("ms0:/_sceUmdManGetInquiry.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, buf, sizeof(buf));
		sceIoClose(uid);
		pspPrintf("_sceUmdManGetInquiry.bin is generated\n");
	}
	memset(buf, 0, sizeof(buf));
#endif
#define TEST_READ_CAPA 1
#if TEST_READ_CAPA
	unsigned char bufCapa[8] = {};
	res = _sceUmdExecReadCapacityCmd(0x25, pUmdDrive, 8, bufCapa);
	if (res < 0) {
		OutputPspError("_sceUmdExecReadCapacityCmd", " ", 0, res);
		sceKernelDelayThread(5 * 1000000);
	}
	else {
		uid = sceIoOpen("ms0:/_sceUmdExecReadCapacityCmd.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, bufCapa, sizeof(bufCapa));
		sceIoClose(uid);
		pspPrintf("_sceUmdExecReadCapacityCmd.bin is generated\n");
	}
#endif
#define TEST_UMD_STRUCTURE 1
#if TEST_UMD_STRUCTURE
	unsigned char bufStruct[2064] = {};
	bufStruct[9] = 8;
	res = _sceUmdExecReadUMDStructureCmd(pUmdDrive, bufStruct, &bufStruct[12]);
	if (res < 0) {
		OutputPspError("_sceUmdExecReadUMDStructureCmd", " ", 0, res);
		sceKernelDelayThread(5 * 1000000);
	}
	else {
		uid = sceIoOpen("ms0:/_sceUmdExecReadUMDStructureCmd.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, bufStruct, sizeof(bufStruct));
		sceIoClose(uid);
		pspPrintf("_sceUmdExecReadUMDStructureCmd.bin is generated\n");
	}
#endif
#define TEST_MKI 1
#if TEST_MKI
	unsigned char bufMKI[96] = {};
	unsigned char bufSPK[0x8000] = {};
	bufMKI[2] = 8;
	
	sceKernelDcacheInvalidateRange(bufSPK, 0x4000);
	
	res = _sceUmdExecReadMKICmd(pUmdDrive, bufMKI, 8, bufSPK);
	if (res < 0) {
		OutputPspError("_sceUmdExecReadMKICmd", " ", 0, res);
		sceKernelDelayThread(5 * 1000000);
	}
	else {
		uid = sceIoOpen("ms0:/_sceUmdExecReadMKICmd.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, bufMKI, sizeof(bufMKI));
		sceIoClose(uid);
		uid = sceIoOpen("ms0:/_sceUmdExecReadMKICmd2.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, bufSPK, sizeof(bufSPK));
		sceIoClose(uid);
		pspPrintf("_sceUmdExecReadMKICmd.bin is generated\n");
	}
#endif
#define TEST_READ_MECHA_STAT 0
#if TEST_READ_MECHA_STAT
	unsigned char bufMecha[16] = {};

	res = _sceUmdExecMechaStatCmd(pUmdDrive, 16, bufMecha);
	if (res < 0) {
		OutputPspError("_sceUmdExecMechaStatCmd", " ", 0, res);
		sceKernelDelayThread(5 * 1000000);
	}
	else {
		uid = sceIoOpen("ms0:/_sceUmdExecMechaStatCmd.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, bufMecha, sizeof(bufMecha));
		sceIoClose(uid);
		pspPrintf("_sceUmdExecMechaStatCmd.bin is generated\n");
	}
#endif
#define TEST_MEDIA_INFO 0
#if TEST_MEDIA_INFO
	// Same as sceIoDevctl(DEVICE_UMD, IOCTL_UMD_GET_MEDIA_INFO
	unsigned char bufMedia[2] = {};
	res = _sceUmdExecGetMediaInfoCmd(pUmdDrive, 2, bufMedia);
	if (res < 0) {
		OutputPspError("_sceUmdExecGetMediaInfoCmd", " ", 0, res);
		sceKernelDelayThread(5 * 1000000);
	}
	else {
		// GAME is "00 01", AUDIO is "00 02" GAME+VIDEO is "00 03"
		uid = sceIoOpen("ms0:/_sceUmdExecGetMediaInfoCmd.bin", PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
		sceIoWrite(uid, bufMedia, sizeof(bufMedia));
		sceIoClose(uid);
		pspPrintf("_sceUmdExecGetMediaInfoCmd.bin is generated\n");
	}
#endif
}
