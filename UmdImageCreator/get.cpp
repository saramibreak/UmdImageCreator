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
#include "get.h"
#include "output.h"

#include <pspkernel.h>

extern int nWriteToX;
extern int nWriteToY;

WORD  GetSizeOrWordForVolDesc(
	LPBYTE lpBuf
) {
	WORD val = MAKEWORD(lpBuf[0], lpBuf[1]);
	if (val == 0) {
		val = MAKEWORD(lpBuf[3], lpBuf[2]);
	}
	return val;
}

DWORD  GetSizeOrDwordForVolDesc(
	LPBYTE lpBuf,
	DWORD dwMax
) {
	DWORD val = MAKEDWORD(MAKEWORD(lpBuf[0], lpBuf[1]),
		MAKEWORD(lpBuf[2], lpBuf[3]));
	if (val == 0 || val >= dwMax) {
		val = MAKEDWORD(MAKEWORD(lpBuf[7], lpBuf[6]),
			MAKEWORD(lpBuf[5], lpBuf[4]));
	}
	return val;
}

int GetDiscInfoToConsole(pspUmdInfo* pDiscInfo)
{
	int ret = sceIoDevctl("umd0:", 0x01F20001, NULL, 0, pDiscInfo, sizeof(pspUmdInfo));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F20001\n");
		return FALSE;
	}
	BOOL bMulti = FALSE;
#ifdef PRX
	nWriteToY = 5;
	pspDebugScreenSetXY(0, nWriteToY);
	nWriteToX = 19;
#endif
	pspPrintf("pspUmdTypes: 0x%02x (", pDiscInfo->type);
	if ((pDiscInfo->type & PSP_UMD_TYPE_GAME) == PSP_UMD_TYPE_GAME) {
#ifdef PRX
		pspDebugScreenSetXY(nWriteToX, nWriteToY);
		nWriteToX += 4;
#endif
		pspPrintf("GAME");
		bMulti = TRUE;
	}
	if ((pDiscInfo->type & PSP_UMD_TYPE_VIDEO) == PSP_UMD_TYPE_VIDEO) {
		if (bMulti) {
#ifdef PRX
			pspDebugScreenSetXY(nWriteToX, nWriteToY);
			nWriteToX += 1;
#endif
			pspPrintf(" ");
		}
		bMulti = TRUE;
#ifdef PRX
		pspDebugScreenSetXY(nWriteToX, nWriteToY);
		nWriteToX += 5;
#endif
		pspPrintf("VIDEO");
	}
	if ((pDiscInfo->type & PSP_UMD_TYPE_AUDIO) == PSP_UMD_TYPE_AUDIO) {
		if (bMulti) {
#ifdef PRX
			pspDebugScreenSetXY(nWriteToX, nWriteToY);
			nWriteToX += 1;
#endif
			pspPrintf(" ");
		}
#ifdef PRX
		pspDebugScreenSetXY(nWriteToX, nWriteToY);
		nWriteToX += 5;
#endif
		pspPrintf("AUDIO");
	}
#ifdef PRX
	pspDebugScreenSetXY(nWriteToX, nWriteToY);
	nWriteToX += 1;
#endif
	pspPrintf(")\n\n");

	int nL0L1LBA = 0;
	ret = sceIoDevctl("umd0:", 0x01F20002, NULL, 0, &nL0L1LBA, sizeof(int));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F20002\n");
		return FALSE;
	}
	nL0L1LBA += 1;
	int nL0LBA = 0;
	ret = sceIoDevctl("umd0:", 0x01F20003, NULL, 0, &nL0LBA, sizeof(int));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F20003\n");
		return FALSE;
	}
	nL0LBA += 1;
	if (nL0LBA == nL0L1LBA) {
#ifdef PRX
		pspDebugScreenSetXY(0, ++nWriteToY);
#endif
		pspPrintf("L0 length: %6d (0x%05x)\n", nL0LBA, nL0LBA);
	}
	else {
#ifdef PRX
		pspDebugScreenSetXY(0, ++nWriteToY);
#endif
		pspPrintf("L0 length: %6d (0x%05x)\n", nL0LBA, nL0LBA);
#ifdef PRX
		pspDebugScreenSetXY(0, ++nWriteToY);
#endif
		pspPrintf("L1 length: %6d (0x%05x)\n", nL0L1LBA - nL0LBA, nL0L1LBA - nL0LBA);
#ifdef PRX
		pspDebugScreenSetXY(0, ++nWriteToY);
#endif
		pspPrintf("---------------------------\n");
#ifdef PRX
		pspDebugScreenSetXY(0, ++nWriteToY);
#endif
		pspPrintf("    Total: %6d (0x%05x)\n\n", nL0L1LBA, nL0L1LBA);
	}

	unsigned char unknown[4] = { 0 };
	ret = sceIoDevctl("umd0:", 0x01F20014, NULL, 0, unknown, sizeof(unknown));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F20014\n");
		return FALSE;
	}
#ifdef PRX
	pspDebugScreenSetXY(0, ++nWriteToY);
#endif
	pspPrintf("0x01F20014 is unknown: %02x %02x %02x %02x\n", unknown[0], unknown[1], unknown[2], unknown[3]);

	int unknown2 = 0;
	ret = sceIoDevctl("umd0:", 0x01F200A0, NULL, 0, &unknown2, sizeof(unknown2));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F200A0\n");
		return FALSE;
	}
#ifdef PRX
	pspDebugScreenSetXY(0, ++nWriteToY);
#endif
	pspPrintf("0x01F200A0 is unknown: %02x\n", unknown2);
	return TRUE;
}

int PrepareOpeningDisc()
{
	int ret = 0;
	int stat = 0;
	while (1) {
		ret = sceUmdCheckMedium();
		if (ret == 0 && stat == 0) {
			pspPrintf("Insert UMD\n");
			stat++;
		}
		else if (ret != 0) {
			break;
		}
	}
	ret = sceUmdActivate(1, "disc0:");
	if (ret < 0) {
		pspPrintf("Cannot activate UMD: result=0x%08X\n", ret);
		return FALSE;
	}

	pspPrintf("Wait until UMD is ready ");
	ret = sceUmdWaitDriveStatWithTimer(PSP_UMD_READY, 20000000);
	if (ret < 0) {
		pspPrintf(" => NG: result=0x%08X\n", ret);
		return FALSE;
	}
	pspPrintf(" => OK\n");
	return TRUE;
}

int CloseOpeningDisc()
{
	int ret = sceUmdDeactivate(1, "disc0:");
	if (ret < 0) {
		pspPrintf("Cannot deactivate UMD: result=0x%08X\n", ret);
		return FALSE;
	}
	return TRUE;
}

int GetDiscID(char* id)
{
	SceUID uidData = sceIoOpen("disc0:/UMD_DATA.BIN", PSP_O_RDONLY, 0777);
	if (uidData < 0) {
		pspPrintf("Cannot open UMD_DATA.BIN: result=0x%08X\n", uidData);
		return FALSE;
	}
	sceIoRead(uidData, id, 10);

	int ret = sceIoClose(uidData);
	if (ret < 0) {
		pspPrintf("Cannot close UMD_DATA.BIN: result=0x%08X\n", ret);
		return FALSE;
	}

	return TRUE;
}

int GetParamSfo(unsigned int disctype)
{
	int ret = 0;
	if ((disctype & PSP_UMD_TYPE_GAME) == PSP_UMD_TYPE_GAME) {
		ret = OutputParamSfo("disc0:/PSP_GAME/PARAM.SFO");
	}

	if ((disctype & PSP_UMD_TYPE_VIDEO) == PSP_UMD_TYPE_VIDEO) {
		ret = OutputParamSfo("disc0:/UMD_VIDEO/PARAM.SFO");
	}

	if ((disctype & PSP_UMD_TYPE_AUDIO) == PSP_UMD_TYPE_AUDIO) {
		ret = OutputParamSfo("disc0:/UMD_AUDIO/PARAM.SFO");
	}
	ret = OutputParamSfo("disc0:/PSP_GAME/SYSDIR/UPDATE/PARAM.SFO");
	return ret;
}

int GetDiscInfoToLog(char* id, unsigned int disctype)
{
	PrepareOpeningDisc();
	int ret = GetDiscID(id);
	if (!ret) {
		return FALSE;
	}
	if (!CreateFile(id, disctype, "_disc.txt", &g_LogFile.fpDisc, "w")) {
		return FALSE;
	}

	ret = GetParamSfo(disctype);
	if (!ret) {
		return FALSE;
	}
	CloseOpeningDisc();

	BOOL bMulti = FALSE;
	OutputDiscLogA("\npspUmdTypes: 0x%02x (", disctype);
	if ((disctype & PSP_UMD_TYPE_GAME) == PSP_UMD_TYPE_GAME) {
		OutputDiscLogA("GAME");
		bMulti = TRUE;
	}
	if ((disctype & PSP_UMD_TYPE_VIDEO) == PSP_UMD_TYPE_VIDEO) {
		if (bMulti) {
			OutputDiscLogA(" ");
		}
		bMulti = TRUE;
		OutputDiscLogA("VIDEO");
	}
	if ((disctype & PSP_UMD_TYPE_AUDIO) == PSP_UMD_TYPE_AUDIO) {
		if (bMulti) {
			OutputDiscLogA(" ");
		}
		OutputDiscLogA("AUDIO");
	}
	OutputDiscLogA(")\n\n");

	int nL0L1LBA = 0;
	ret = sceIoDevctl("umd0:", 0x01F20002, NULL, 0, &nL0L1LBA, sizeof(int));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F20002\n");
		return FALSE;
	}
	nL0L1LBA += 1;
	int nL0LBA = 0;
	ret = sceIoDevctl("umd0:", 0x01F20003, NULL, 0, &nL0LBA, sizeof(int));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F20003\n");
		return FALSE;
	}
	nL0LBA += 1;

	if (nL0LBA == nL0L1LBA) {
		OutputDiscLogA("L0 length: %6d (0x%05x)\n", nL0LBA, nL0LBA);
	}
	else {
		OutputDiscLogA("L0 length: %6d (0x%05x)\n", nL0LBA, nL0LBA);
		OutputDiscLogA("L1 length: %6d (0x%05x)\n", nL0L1LBA - nL0LBA, nL0L1LBA - nL0LBA);
		OutputDiscLogA("---------------------------\n");
		OutputDiscLogA("    Total: %6d (0x%05x)\n\n", nL0L1LBA, nL0L1LBA);
	}

	unsigned char unknown[4] = { 0 };
	ret = sceIoDevctl("umd0:", 0x01F20014, NULL, 0, unknown, sizeof(unknown));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F20014\n");
		return FALSE;
	}
	OutputDiscLogA("0x01F20014 is unknown: %02x %02x %02x %02x\n", unknown[0], unknown[1], unknown[2], unknown[3]);

	int unknown2 = 0;
	ret = sceIoDevctl("umd0:", 0x01F200A0, NULL, 0, &unknown2, sizeof(unknown2));
	if (ret < 0) {
		pspPrintf("Cannot run sceIoDevctl: 0x01F200A0\n");
		return FALSE;
	}
	OutputDiscLogA("0x01F200A0 is unknown: %02x\n", unknown2);
	FcloseAndNull(g_LogFile.fpDisc);
	return TRUE;
}
