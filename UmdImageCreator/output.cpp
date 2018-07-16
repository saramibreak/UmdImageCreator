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
#include "output.h"
#include "execScsiCmdforFileSystem.h"

#include <stdlib.h>
#include <string.h>

extern int nWriteToY;

VOID OutputLastErrorNumAndString(
	INT errnum,
	LPCTSTR pszFuncName,
	LONG lLineNum
) {
	OutputMainErrorLogA("[F:%s][L:%lu] GetLastError: %d, %s\n"
		, pszFuncName, lLineNum, errnum, strerror(errnum));
	pspPrintf("[F:%s][L:%lu] GetLastError: %d, %s\n"
		, pszFuncName, lLineNum, errnum, strerror(errnum));
}

int CreateFile(char* id, const char* filename, FILE** fp, const char* type)
{
	char dir[] = "ms0:/PSP/GAME/UmdImageCreator/";
	if (sizeof(dir) + strlen(id) + strlen(filename) > 80) {
		pspPrintf("path is too long\n");
		return 0;
	}
	char path[80] = { 0 };
	strncpy(path, dir, sizeof(dir));
	strncat(path, id, strlen(id));
	strncat(path, filename, strlen(filename));

	*fp = fopen(path, type);
	if (*fp == NULL) {
		pspPrintf("Cannot create %s\n", path);
		return 0;
	}
	return  1;
}

void DumpIso(char* id, int nDump)
{
	SceUID uid = sceIoOpen("umd0:", PSP_O_RDONLY, 0);
	if (uid < 0) {
		pspPrintf("Cannot open UMD: result=0x%08X\n", uid);
		return;
	}
	if (!CreateFile(id, "_mainInfo.txt", &g_LogFile.fpMainInfo, "w")) {
		return;
	}
	if (!CreateFile(id, "_mainError.txt", &g_LogFile.fpMainError, "w")) {
		return;
	}
	if (!CreateFile(id, "_volDesc.txt", &g_LogFile.fpVolDesc, "w")) {
		return;
	}

	int nVolumeSpaceSize = 0;
	if (!ReadDVDForFileSystem(uid, &nVolumeSpaceSize)) {
		return;
	}

	if (nDump) {
		FILE* fpIso = NULL;
		if (!CreateFile(id, ".iso", &fpIso, "wb")) {
			return;
		}
#ifdef PRX
		size_t allocSize = DISC_RAW_READ_SIZE * 240;
		LPBYTE lpBuf = (LPBYTE)calloc(allocSize, sizeof(BYTE));
		if (!lpBuf) {
			pspPrintf("Failed to calloc\n");
			return;
		}
		nWriteToY++;
#else
		size_t allocSize = DISC_RAW_READ_SIZE * 128;
		SceUID mem = sceKernelAllocPartitionMemory(2, "Memory1", 0, allocSize, NULL);
		if (mem < 0) {
			pspPrintf("Failed to run sceKernelAllocPartitionMemory\n");
			return;
		}
		LPBYTE lpBuf = (LPBYTE)sceKernelGetBlockHeadAddr(mem);
#endif
		int nTransferLen = allocSize / DISC_RAW_READ_SIZE;

		for (int nLBA = 0; nLBA < nVolumeSpaceSize; nLBA += nTransferLen) {
			if (nTransferLen > nVolumeSpaceSize - nLBA) {
				nTransferLen = nVolumeSpaceSize - nLBA;
				allocSize = DISC_RAW_READ_SIZE * nTransferLen;
			}
			if (!ExecReadDisc(uid, nLBA, lpBuf, nTransferLen)) {
				break;
			}
			fwrite(lpBuf, sizeof(BYTE), allocSize, fpIso);
#ifdef PBP
			pspPrintf("\rCreating %s.iso (LBA) %6d/%6d", id, nLBA + nTransferLen, nVolumeSpaceSize);
#else
			pspDebugScreenSetXY(0, nWriteToY);
			pspPrintf("Creating %s.iso (LBA) %6d/%6d", id, nLBA + nTransferLen, nVolumeSpaceSize);
#endif
		}
#ifdef PRX
		FreeAndNull(lpBuf);
#else
		sceKernelFreePartitionMemory(mem);
#endif
		FcloseAndNull(fpIso);
	}
	pspPrintf("\nDone\n");
	sceIoClose(uid);
	FcloseAndNull(g_LogFile.fpMainInfo);
	FcloseAndNull(g_LogFile.fpMainError);
	FcloseAndNull(g_LogFile.fpVolDesc);
}
