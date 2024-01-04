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
#include <pspkernel.h>

#include "_external/pspUmdMan_driver.h"
#include "get.h"
#include "output.h"
#include "outputScsiCmdLogforCD.h"

WORD  GetSizeOrWordForVolDesc(
	LPBYTE lpBuf
) {
	WORD val = MAKEWORD(lpBuf[0], lpBuf[1]);
	if (val == 0) {
		val = MAKEWORD(lpBuf[3], lpBuf[2]);
	}
	return val;
}

UINT GetSizeOrUintForVolDesc(
	LPBYTE lpBuf,
	UINT uiMax
) {
	UINT val = MAKEUINT(MAKEWORD(lpBuf[0], lpBuf[1]), MAKEWORD(lpBuf[2], lpBuf[3]));
	if (val == 0 || val >= uiMax) {
		val = MAKEUINT(MAKEWORD(lpBuf[7], lpBuf[6]), MAKEWORD(lpBuf[5], lpBuf[4]));
	}
	return val;
}

int GetDriveInfo(char* id, unsigned int discType)
{
	pspPrintf("\nOutput drive information\n");
	INQUIRYDATA inquiry;
	// 0x01F20014
	int ret = sceIoDevctl(DEVICE_UMD, IOCTL_UMD_GET_INQUIRY_DATA, NULL, 0, &inquiry, sizeof(inquiry));
	if (ret < 0) {
		OutputPspError("sceIoDevctl", DEVICE_UMD, IOCTL_UMD_GET_INQUIRY_DATA, ret);
		return FALSE;
	}

	if (!CreateFile(id, discType, "_drive.txt", &g_LogFile.fpDrive, "w")) {
		return FALSE;
	}
	OutputInquiry(&inquiry);
	FcloseAndNull(g_LogFile.fpDrive);
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
	ret = sceUmdActivate(1, DEVICE_DISC);
	if (ret < 0) {
		OutputPspError("sceUmdActivate", " ", NULL, ret);
		return FALSE;
	}

	pspPrintf("Wait until UMD is ready => ");
	ret = sceUmdWaitDriveStatWithTimer(PSP_UMD_READY, 20000000);
	if (ret < 0) {
		OutputPspError("sceUmdWaitDriveStatWithTimer", " ", NULL, ret);
		return FALSE;
	}
	pspPrintf("OK\n");
	return TRUE;
}

int CloseOpeningDisc()
{
	int ret = sceUmdDeactivate(1, DEVICE_DISC);
	if (ret < 0) {
		OutputPspError("sceUmdDeactivate", " ", NULL, ret);
		return FALSE;
	}
	return TRUE;
}

int GetDiscID(char* id)
{
	const char umddata[] = "disc0:/UMD_DATA.BIN";
	SceUID uidData = sceIoOpen(umddata, PSP_O_RDONLY, 0777);
	if (uidData < 0) {
		OutputPspError("sceIoOpen", umddata, NULL, uidData);
		return FALSE;
	}
	sceIoRead(uidData, id, 10);

	int ret = sceIoClose(uidData);
	if (ret < 0) {
		OutputPspError("sceIoClose", umddata, NULL, ret);
		return FALSE;
	}

	return TRUE;
}

int GetParamSfo(unsigned int discType)
{
	int ret = 0;
	if ((discType & PSP_UMD_TYPE_GAME) == PSP_UMD_TYPE_GAME) {
		ret = OutputParamSfo("disc0:/PSP_GAME/PARAM.SFO");
	}

	if ((discType & PSP_UMD_TYPE_VIDEO) == PSP_UMD_TYPE_VIDEO) {
		ret = OutputParamSfo("disc0:/UMD_VIDEO/PARAM.SFO");
	}

	if ((discType & PSP_UMD_TYPE_AUDIO) == PSP_UMD_TYPE_AUDIO) {
		ret = OutputParamSfo("disc0:/UMD_AUDIO/PARAM.SFO");
	}
	// some discs don't have it
	OutputParamSfo("disc0:/PSP_GAME/SYSDIR/UPDATE/PARAM.SFO");
	return ret;
}

int GetPfi(char* id, pspUmdInfo* pUmdInfo)
{
	unsigned char bufStruct[2064] = {};
	bufStruct[9] = 8;
	void* pUmdDrive = _sceUmdManGetUmdDrive(0);
	int res = _sceUmdExecReadUMDStructureCmd(pUmdDrive, bufStruct, &bufStruct[12]);
	if (res < 0) {
		OutputPspError("_sceUmdExecReadUMDStructureCmd", " ", 0, res);
		sceKernelDelayThread(5 * 1000000);
	}
	else {
		FILE* fp = NULL;
		if (!CreateFile(id, pUmdInfo->type, "_PFI.bin", &fp, "w")) {
			return FALSE;
		}
		fwrite(&bufStruct[16], sizeof(BYTE), 2048, fp);
		fclose(fp);

		LPCTSTR lpDiskCategory[] = {
			_T("Reserved1"), _T("Reserved2"), _T("Reserved3"), _T("Reserved4"), 
			_T("Reserved5"), _T("Reserved6"), _T("Reserved7"), _T("Reserved8"),
			_T("Read-Only disc"), _T("Reserved10"), _T("Reserved11"), _T("Reserved12"),
			_T("Reserved13"), _T("Reserved14"), _T("Reserved15"), _T("Reserved16")
		};

		LPCTSTR lpTrackDensity[] = {
			_T("0.70 um/track"), _T("Reserved2"), _T("Reserved3"), _T("Reserved4"),
			_T("Reserved5"), _T("Reserved6"), _T("Reserved7"), _T("Reserved8"),
			_T("Reserved9"), _T("Reserved10"), _T("Reserved11"), _T("Reserved12"),
			_T("Reserved13"), _T("Reserved14"), _T("Reserved15"), _T("Reserved16")
		};

		LPCTSTR lpLinearDensity[] = {
			_T("Reserved1"), _T("Reserved2"), _T("Reserved3"), _T("Reserved4"),
			_T("Reserved5"), _T("Reserved6"), _T("Reserved7"), _T("Reserved8"),
			_T("Reserved9"), _T("Reserved10"), _T("Reserved11"), _T("Reserved12"),
			_T("Reserved13"), _T("Reserved14"), _T("0.139 um/bit"), _T("Reserved16")
		};

		UMD_FULL_LAYER_DESCRIPTOR umdLayer;
		memcpy(&umdLayer, &bufStruct[16], 2048);

		DWORD dwStartingDataSector = umdLayer.commonHeader.StartingDataSector;
		DWORD dwEndDataSector = umdLayer.commonHeader.EndDataSector;
		DWORD dwEndLayerZeroSector = umdLayer.commonHeader.EndLayerZeroSector;
		REVERSE_LONG(&dwStartingDataSector);
		REVERSE_LONG(&dwEndDataSector);
		REVERSE_LONG(&dwEndLayerZeroSector);

		OutputLog(fileDisc,
			OUTPUT_DHYPHEN_PLUS_STR("PhysicalFormatInformation")
			"\t     VersionNumber: %d\n"
			"\t      DiskCategory: %s\n"
			"\t       MaximumRate: %d\n"
			"\t          DiskSize: %d\n"
			"\t         LayerType: %d\n"
			"\t         TrackPath: %s\n"
			"\t          DiskType: %s\n"
			"\t        TrackPitch: %s\n"
			"\t  ChannelBitLength: %s\n"
			"\tStartingDataSector: %8lu (%#lx)\n"
			"\t     EndDataSector: %8lu (%#lx)\n"
			"\tEndLayerZeroSector: %8lu (%#lx)\n"
			"\t    MediaAttribute: %04u\n"
//			"\t     MediaSpecific: \n"
			, umdLayer.commonHeader.VersionNumber
			, lpDiskCategory[umdLayer.commonHeader.DiskCategory]
			, umdLayer.commonHeader.MaximumRate
			, umdLayer.commonHeader.DiskSize
			, umdLayer.commonHeader.LayerType
			, umdLayer.commonHeader.TrackPath == 0 ? _T("Parallel Track Path") : _T("Opposite Track Path")
			, umdLayer.commonHeader.DiskType == 0 ? _T("Single Layer") : _T("Double Layer")
			, lpTrackDensity[umdLayer.commonHeader.TrackPitch]
			, lpLinearDensity[umdLayer.commonHeader.ChannelBitLength]
			, dwStartingDataSector, dwStartingDataSector
			, dwEndDataSector, dwEndDataSector
			, dwEndLayerZeroSector, dwEndLayerZeroSector
			, umdLayer.commonHeader.MediaAttribute
		);
//		OutputMainChannel(fileDisc, umdLayer.MediaSpecific, 0, sizeof(umdLayer.MediaSpecific));
		OutputLog(standardOut, "PFI.bin was dumped\n");
	}
	return TRUE;
}

int GetDiscInfo(char* id, pspUmdInfo* pUmdInfo, unsigned int* pDiscSize)
{
	int ret = sceIoDevctl(DEVICE_UMD, IOCTL_UMD_GET_MEDIA_INFO, NULL, 0, pUmdInfo, sizeof(pspUmdInfo));
	if (ret < 0) {
		OutputPspError("sceIoDevctl", DEVICE_UMD, IOCTL_UMD_GET_MEDIA_INFO, ret);
		return FALSE;
	}
	if (pUmdInfo->type == 0) {
		pspDebugScreenSetXY(0, pspDebugScreenGetY());
		pspPrintf("Please insert a disc before push the button");
		return FALSE;
	}
	pspPrintf("\n");

	if (!PrepareOpeningDisc()) {
		return FALSE;
	}
	if (!GetDiscID(id)) {
		return FALSE;
	}
	char* p = strchr(id, ':');
	if (p != NULL) {
		*p = '_';
	}
	if (!CreateFile(id, pUmdInfo->type, "_disc.txt", &g_LogFile.fpDisc, "w")) {
		return FALSE;
	}
	if (!GetParamSfo(pUmdInfo->type)) {
		return FALSE;
	}
	CloseOpeningDisc();

	BOOL bMulti = FALSE;
	OutputLog(standardOut | fileDisc, "\npspUmdTypes: 0x%02x (", pUmdInfo->type);
	if ((pUmdInfo->type & PSP_UMD_TYPE_GAME) == PSP_UMD_TYPE_GAME) {
		OutputLog(standardOut | fileDisc, "GAME");
		bMulti = TRUE;
	}
	if ((pUmdInfo->type & PSP_UMD_TYPE_VIDEO) == PSP_UMD_TYPE_VIDEO) {
		if (bMulti) {
			OutputLog(standardOut | fileDisc, " ");
		}
		bMulti = TRUE;
		OutputLog(standardOut | fileDisc, "VIDEO");
	}
	if ((pUmdInfo->type & PSP_UMD_TYPE_AUDIO) == PSP_UMD_TYPE_AUDIO) {
		if (bMulti) {
			OutputLog(standardOut | fileDisc, " ");
		}
		OutputLog(standardOut | fileDisc, "AUDIO");
	}
	OutputLog(standardOut | fileDisc, ")\n");

	int nLastLBA = 0;
	ret = sceIoDevctl(DEVICE_UMD, IOCTL_UMD_GET_LAST_LBA, NULL, 0, &nLastLBA, sizeof(int));
	if (ret != 0) {
		OutputPspError("sceIoDevctl", DEVICE_UMD, IOCTL_UMD_GET_LAST_LBA, ret);
		return FALSE;
	}
	int nAllLength = nLastLBA + 1;

	int nL0LBA = 0;
	ret = sceIoDevctl(DEVICE_UMD, IOCTL_UMD_GET_LAST_LBA_OF_L0, NULL, 0, &nL0LBA, sizeof(int));
	if (ret != 0) {
		OutputPspError("sceIoDevctl", DEVICE_UMD, IOCTL_UMD_GET_LAST_LBA_OF_L0, ret);
		return FALSE;
	}
	int nL0Length = nL0LBA + 1;

	if (nL0Length == nAllLength) {
		OutputLog(standardOut | fileDisc, "  L0 length: %6d (0x%05x)\n", nL0Length, nL0Length);
	}
	else {
		OutputLog(standardOut | fileDisc, 
			"  L0 length: %6d (0x%05x)\n"
			"  L1 length: %6d (0x%05x)\n"
			"  ---------------------------\n"
			"      Total: %6d (0x%05x)\n"
			, nL0Length, nL0Length, nAllLength - nL0Length, nAllLength - nL0Length, nAllLength, nAllLength);
	}

	*pDiscSize = nAllLength * DISC_MAIN_DATA_SIZE;
	OutputLog(standardOut | fileDisc, "   FileSize: %d (0x%08x)\n", *pDiscSize, *pDiscSize);

#ifdef PBP
	GetPfi(id, pUmdInfo);
#endif
	FcloseAndNull(g_LogFile.fpDisc);
	return TRUE;
}

int GetMSInfo(MS_INFO* info)
{
	int ret = sceIoDevctl(DEVICE_MS, IOCTL_MS_GET_MS_INFO, &info, 4, NULL, 0);
	if (ret == 0) {
		info->smax = (info->cluster_max  * info->sector_count) * info->sector_size;
		info->sfree = (info->cluster_free * info->sector_count) * info->sector_size;
		info->sused = info->smax - info->sfree;
		return TRUE;
	}
	OutputPspError("sceIoDevctl", DEVICE_MS, IOCTL_MS_GET_MS_INFO, ret);
	return FALSE;
}
