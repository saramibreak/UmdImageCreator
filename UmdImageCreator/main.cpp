/**
 * Copyright 2018-2023 sarami
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
#include <pspsdk.h>
#include <pspkernel.h>

#include "define.h"
#include "struct.h"
#include "execScsiCmdforFileSystem.h"
#include "get.h"
#include "outputScsiCmdLogforCD.h"
#include "output.h"
#include "testcode.h"

#ifdef PBP
PSP_MODULE_INFO("UmdImageCreator", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(-1);
#endif

#ifdef PRX
PSP_MODULE_INFO("UmdImageCreator", PSP_MODULE_KERNEL, 1, 1);
PSP_MAIN_THREAD_ATTR(0);
PSP_HEAP_SIZE_KB(-1);
#endif

_LOG_FILE g_LogFile = { NULL, NULL, NULL, NULL , NULL };

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

#define RGB(r, g, b) ((r)|((g)<<8)|((b)<<16))
SceCtrlData pad;
int done = 0;

void run()
{
	pspDebugScreenInit();
#ifdef PRX
	pspDebugScreenSetBackColor(RGB(0, 0, 0));
	sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, threadlist, 66, &threadnumber);
	threadschanger(0, threadlist, threadnumber);
#endif
	pspDebugScreenClear();

	scePowerSetClockFrequency(333, 333, 167);
	int nKernelVer = sceKernelDevkitVersion();
	pspPrintf("sceKernelDevkitVersion: 0x%08x\n", nKernelVer);
#ifdef PBP
	char pspumdman[32] = {};
	if (nKernelVer < 0x03070110) {
		memcpy(pspumdman, "ms0:/seplugins/pspumdman352.prx", sizeof(pspumdman));
	}
	else if (nKernelVer < 0x06060010) {
		pspPrintf("This kernel version does not support to load pspumdman.prx\n");
	}
	else if (nKernelVer <= 0x06060110) {
		memcpy(pspumdman, "ms0:/seplugins/pspumdman.prx", sizeof(pspumdman));
	}

	SceUID uid = pspSdkLoadStartModule(pspumdman, PSP_MEMORY_PARTITION_KERNEL);
	if (uid < 0) {
		OutputPspError("pspSdkLoadStartModule", pspumdman, 0, uid);
	}
	else {
		pspPrintf("Loading module of %s: 0x%08x\n", pspumdman, uid);
	}
#endif
	pspPrintf(
		"Press Circle to start the dumping (iso + log)\n"
		"Press Cross to start the dumping (only log)\n"
		"Press Triangle to exit\n\n"
	);

	pspUmdInfo discInfo = { 0, 0 };
	unsigned int nDiscSize = 0;

	while (!done) {
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_CIRCLE || pad.Buttons & PSP_CTRL_CROSS) {
			char id[11] = { 0 };
			if (GetDiscInfo(id, &discInfo, &nDiscSize)) {
				GetDriveInfo(id, discInfo.type);
				int nDump = TRUE;
				if (pad.Buttons & PSP_CTRL_CROSS) {
					nDump = FALSE;
				}
				DumpIso(id, discInfo.type, nDiscSize, nDump);
#ifdef PRX
				pspPrintf("Automatically exit\n");
				sceKernelDelayThread(5 * 1000000);
				threadschanger(1, threadlist, threadnumber);
#endif
				done = 1;
			}
		}
		if (pad.Buttons & PSP_CTRL_TRIANGLE) {
#ifdef PRX
			threadschanger(1, threadlist, threadnumber);
#endif
			done = 1;
		}
#if 1
		if (pad.Buttons & PSP_CTRL_SQUARE) {
			searchCommand();
			sceKernelDelayThread(5 * 1000000);
#ifdef PRX
			threadschanger(1, threadlist, threadnumber);
#endif
			done = 1;
		}
		if (pad.Buttons & PSP_CTRL_RTRIGGER) {
			testCommand();
			sceKernelDelayThread(5 * 1000000);
#ifdef PRX
			threadschanger(1, threadlist, threadnumber);
#endif
			done = 1;
		}
		if (pad.Buttons & PSP_CTRL_LTRIGGER) {
			testNewCommand();
			sceKernelDelayThread(5 * 1000000);
#ifdef PRX
			threadschanger(1, threadlist, threadnumber);
#endif
			done = 1;
		}
#endif
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
