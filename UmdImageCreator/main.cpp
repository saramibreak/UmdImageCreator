/**
 * Copyright 2018 sarami
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
#include <errno.h>
#include <string.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <psploadexec_kernel.h>
#include <psppower.h>
#include <pspumd.h>

#include "define.h"
#include "struct.h"
#include "execScsiCmdforFileSystem.h"
#include "get.h"
#include "outputScsiCmdLogforCD.h"
#include "output.h"

#ifdef PBP
PSP_MODULE_INFO("UmdImageCreator", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(-1);
#endif

#ifdef PRX
PSP_MODULE_INFO("UmdImageCreator", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(-1);
#endif

int nWriteToX = 0;
int nWriteToY = 0;
_LOG_FILE g_LogFile = { NULL, NULL, NULL, NULL };

void searchCommand()
{
	SceUID uid = sceIoOpen("umd0:", PSP_O_RDONLY, 0);
	if (uid < 0) {
		pspPrintf("Cannot open UMD disc: result=0x%08X\n", uid);
		return;
	}
	SceUID uidcmd = sceIoOpen(
		"ms0:/PSP/GAME/UmdImageCreator/cmdtest.txt", PSP_O_WRONLY | PSP_O_CREAT, 0777);
	if (uidcmd < 0) {
		pspPrintf("Cannot create cmdtest.txt\n");
		return;
	}
	/*E
	* PSP IOCTL structure
	*
	*  31      28 27    20 19 18  17  16  15   14   13  12 11       0
	* +----------+--------+-----+---+---+----+-----+------+----------+
	* | Reserved | DEVICE | Rsv |Dst|Src|Priv|Cache|ThWait| Function |
	* +----------+--------+-----+---+---+----+-----+------+----------+
	*
	*  bit 31-28: Reserved
	*      0000= Always zero
	*
	*  bit 27-20: DeviceType(DEVICE)
	*      0x00=     Generic file I/O
	*      0x01=     Generic TTY
	*      0x02`0F= reserved for generic file I/O, TTY
	*      0x10`1F= UMD specific IOCTL
	*      0x20`2F= MemoryStick specific OCTL
	*      0x30`3F= USB specific IOCTL
	*      0x40`4F= CPHIO specific IOCTL
	*      0x50`5F= IrDA/SIRCS specific IOCTL
	*
	*  bit 19,18: Reserved
	*      00= Always zero
	*
	*  bit 17: output block use(Dst)
	*      1= Use    (output data exits)
	*      0= No use (no output data)
	*
	*  bit 16: input block use(Src)
	*      1= Use    (input data exits)
	*      0= No use (no input data)
	*
	*  bit 15: privilege access(Priv)
	*      1= Privilege is required
	*      0= Privilege is not required
	*
	*  bit 14: input/output cache operation in drvier(Cache)
	*      1= Driver NOT guarantees cache coherency.
	*      0= Driver guarantees cache coherency.
	*
	*  bit 13,12: Thread WAIT condition(ThWait)
	*      11= Driver does not make thread to WAIT condition, can execute in CPUDI
	*      10= Driver does not make thread to WAIT condition
	*      01= Driver may make thread to WAIT condition
	*      00= Driver may make thread to WAIT condition with callback
	*         :
	*
	*  bit 11-0: Function code(Function)
	*      Concrete meanings differ for every device code.
	*      If device codes differ, with the same value of an function code part
	*      must be treated as a different meanings.
	*/
	// Test to get the PFI
	//  27-20 |17-16 |11-0|       
	// 0x1[0-f][2-3]0[0-f][0-f][0-f]
	char buffer[2052] = { 0 };
	char ret[41] = { 0 };
	int cmdRange[] = {
		0x1010000, 0x1110000, 0x1210000, 0x1310000,
		0x1410000, 0x1510000, 0x1610000, 0x1710000,
		0x1810000, 0x1910000, 0x1a10000, 0x1b10000,
		0x1c10000, 0x1d10000, 0x1e10000, 0x1f10000
	};

	for (int i = 0; i < 16; i++) {
		for (int cmd = cmdRange[i]; cmd < cmdRange[i] + 0x1000; cmd++) {
			int result = sceIoIoctl(uid, cmd, buffer, sizeof(buffer), NULL, 0);
			sprintf(ret, "sceIoIoctl: 0x%08X result=0x%08X\n", cmd, result);
			sceIoWrite(uidcmd, ret, sizeof(ret));
			pspPrintf("\rsceIoIoctl: 0x%08X result=0x%08X", cmd, result);

			result = sceIoDevctl("umd0:", cmd, buffer, sizeof(buffer), NULL, 0);
			sprintf(ret, "sceIoDvctl: 0x%08X result=0x%08X\n", cmd, result);
			sceIoWrite(uidcmd, ret, sizeof(ret));
		}
	}
	pspPrintf("\n");

	int cmdRange2[] = {
		0x1020000, 0x1120000, 0x1220000, 0x1320000, 
		0x1420000, 0x1520000, 0x1620000, 0x1720000,
		0x1820000, 0x1920000, 0x1a20000, 0x1b20000, 
		0x1c20000, 0x1d20000, 0x1e20000, 0x1f20000
	};
	
	for (int i = 0; i < 16; i++) {
		for (int cmd = cmdRange2[i]; cmd < cmdRange2[i] + 0x1000; cmd++) {
			int result = sceIoIoctl(uid, cmd, NULL, 0, buffer, sizeof(buffer));
			sprintf(ret, "sceIoIoctl: 0x%08X result=0x%08X\n", cmd, result);
			sceIoWrite(uidcmd, ret, sizeof(ret));
			pspPrintf("\rsceIoIoctl: 0x%08X result=0x%08X", cmd, result);

			result = sceIoDevctl("umd0:", cmd, NULL, 0, buffer, sizeof(buffer));
			sprintf(ret, "sceIoDvctl: 0x%08X result=0x%08X\n", cmd, result);
			sceIoWrite(uidcmd, ret, sizeof(ret));
		}
	}
	pspPrintf("\n");

	int cmdRange3[] = {
		0x1030000, 0x1130000, 0x1230000, 0x1330000,
		0x1430000, 0x1530000, 0x1630000, 0x1730000,
		0x1830000, 0x1930000, 0x1a30000, 0x1b30000,
		0x1c30000, 0x1d30000, 0x1e30000, 0x1f30000
	};
	for (int i = 0; i < 16; i++) {
		for (int cmd = cmdRange3[i]; cmd < cmdRange3[i] + 0x1000; cmd++) {
			int result = sceIoIoctl(uid, cmd, buffer, sizeof(buffer), buffer, sizeof(buffer));
			sprintf(ret, "sceIoIoctl: 0x%08X result=0x%08X\n", cmd, result);
			sceIoWrite(uidcmd, ret, sizeof(ret));
			pspPrintf("\rsceIoIoctl: 0x%08X result=0x%08X", cmd, result);

			result = sceIoDevctl("umd0:", cmd, buffer, sizeof(buffer), buffer, sizeof(buffer));
			sprintf(ret, "sceIoDvctl: 0x%08X result=0x%08X\n", cmd, result);
			sceIoWrite(uidcmd, ret, sizeof(ret));
		}
	}
	pspPrintf("\nDone\n");
	sceIoClose(uid);
	sceIoClose(uidcmd);
}

void testDevctl()
{
	g_LogFile.fpMainInfo = fopen("ms0:/PSP/GAME/UmdImageCreator/devctlTest.txt", "w");
	if (g_LogFile.fpMainInfo == NULL) {
		pspPrintf("Cannot create devctlTest.txt\n");
		return;
	}
	BYTE buffer[2052] = { 0 };
	/*
	int cmd[] = {
		0x01F100A3, 0x01F100A4, 0x01F100A6, 0x01F100A8, 0x01F100A9
	};
	for (size_t i = 0; i < sizeof(cmd) / sizeof(int); i++) {
		int result = sceIoDevctl("umd0:", cmd[i], buffer, sizeof(buffer), NULL, 0);
		OutputCDMain(fileMainInfo, buffer, cmd[i], 2048);
		memset(buffer, 0, sizeof(buffer));
	}
	*/
	int cmd2[] = {
		0x01F20001, 0x01F20002, 0x01F20003, 0x01F20014, 0x01F200A0
	};
	for (size_t i = 0; i < sizeof(cmd2) / sizeof(int); i++) {
		sceIoDevctl("umd0:", cmd2[i], NULL, 0, buffer, sizeof(buffer));
		OutputCDMain(fileMainInfo, buffer, cmd2[i], 2048);
		memset(buffer, 0, sizeof(buffer));
	}

	SceUID uid = sceIoOpen("umd0:", PSP_O_RDONLY, 0);
	if (uid < 0) {
		pspPrintf("Cannot open UMD disc: result=0x%08X\n", uid);
		return;
	}

	INT seek[4] = { 0 };
	seek[0] = 16;
	INT result = sceIoIoctl(uid, 0x01F100A6, seek, sizeof(seek), NULL, 0);
	if (result < 0) {
		OutputLastErrorNumAndString(errno, _T(__FUNCTION__), __LINE__);
		return;
	}

	int cmd3[] = {
		0x01F300A5, 0x01F300A7
	};
	int tmp = 1;
	for (size_t i = 0; i < sizeof(cmd3) / sizeof(int); i++) {
		sceIoDevctl("umd0:", cmd3[i], &tmp, sizeof(int), buffer, tmp);
		OutputCDMain(fileMainInfo, buffer, cmd3[i], 2048);
		memset(buffer, 0, sizeof(buffer));
	}
	pspPrintf("\nDone\n");
	FcloseAndNull(g_LogFile.fpMainInfo);
}

SceUID threadlist[66], st_thlist_first[66], st_thnum_first;
int threadnumber;

void threadschanger(int stat, SceUID threadlist[], int threadnumber)
{
	int(*request_stat_func)(SceUID) = NULL;
	int i, j;
	SceUID selfid = sceKernelGetThreadId();

	if (stat == 1)
		request_stat_func = sceKernelResumeThread;

	else if (stat == 0)
		request_stat_func = sceKernelSuspendThread;


	SceKernelThreadInfo status;

	for (i = 0; i < threadnumber; i++) {
		int no_target = 0;
		for (j = 0; j < st_thnum_first; j++) {
			if (threadlist[i] == st_thlist_first[j] || selfid == threadlist[i]) {
				no_target = 1;
				break;
			}
		}

		sceKernelReferThreadStatus(threadlist[i], &status);
		if (!no_target) (*request_stat_func)(threadlist[i]);
	}
}

void printInitMessage() {
	pspDebugScreenClear();
	pspPrintf("Press Circle to start the dumping iso\n");
	pspPrintf("Press Cross to start the dumping log\n");
#if 0
	pspPrintf("Press Square to search all umd commands and run some umd commands\n");
#endif
	pspPrintf("Press Triangle to exit\n");
}

#define RGB(r, g, b) ((r)|((g)<<8)|((b)<<16))
SceCtrlData pad;
int done = 0;

void run() {
	pspDebugScreenInit();
#ifdef PRX
	pspDebugScreenSetBackColor(RGB(0, 0, 0));
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, threadlist, 66, &threadnumber);
	threadschanger(0, threadlist, threadnumber);
#endif
	printInitMessage();
	pspUmdInfo discInfo = { 0, 0 };
	unsigned int nDiscSize = 0;

	scePowerSetClockFrequency(333, 333, 166);
	while (!done) {
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_CIRCLE) {
			printInitMessage();
			if (GetDiscInfoToConsole(&discInfo)) {
				char id[11] = { 0 };
				if (GetDiscInfoToLog(id, discInfo.type, &nDiscSize)) {
					DumpIso(id, discInfo.type, nDiscSize, TRUE);
#ifdef PRX
					pspPrintf("Automatically exit\n");
					sceKernelDelayThread(5 * 1000000);
					threadschanger(1, threadlist, threadnumber);
					done = 1;
#endif
				}
			}
		}

		if (pad.Buttons & PSP_CTRL_CROSS) {
			printInitMessage();
			if (GetDiscInfoToConsole(&discInfo)) {
				char id[11] = { 0 };
				if (GetDiscInfoToLog(id, discInfo.type, &nDiscSize)) {
					DumpIso(id, discInfo.type, nDiscSize, FALSE);
#ifdef PRX
					pspPrintf("Automatically exit\n");
					sceKernelDelayThread(5 * 1000000);
					threadschanger(1, threadlist, threadnumber);
					done = 1;
#endif
				}
			}
		}
#if 0
		if (pad.Buttons & PSP_CTRL_SQUARE) {
			searchCommand();
			testDevctl();
		}
#endif
		if (pad.Buttons & PSP_CTRL_TRIANGLE) {
#ifdef PRX
			threadschanger(1, threadlist, threadnumber);
#endif
			done = 1;
		}
		sceKernelDelayThreadCB(5000);
	}
	scePowerSetClockFrequency(222, 222, 111);

	sceGuTerm();
#ifdef PBP
	sceKernelExitGame();
#else
	sceKernelExitVSHVSH(NULL);
#endif
}

#ifdef PBP
int main(int argc, char *argv[])
{
	run();
	return 0;
}
#endif

#ifdef PRX
#ifdef __cplusplus
extern "C" {
#endif
int main_thread(SceSize args, void *argp)
{
	while (!done) {
		sceKernelDelayThread(50000);
		sceCtrlPeekBufferPositive(&pad, 1);

		if ((pad.Buttons & PSP_CTRL_NOTE)) {
			run();
		}
	}
	return 0;
}

int module_start(SceSize args, void *argp)
{
	int thid = sceKernelCreateThread("main_thread",	main_thread, 32, 0x6000, 0, NULL);

	if (thid >= 0) {
		sceKernelStartThread(thid, args, argp);
	}
	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
#ifdef __cplusplus
}
#endif
#endif
