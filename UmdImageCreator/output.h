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
#pragma once
#include "windef.h"
#include "struct.h"
#include "forwardDeclaration.h"
#include "define.h"

#include <stdio.h>

#define BOOLEAN_TO_STRING_TRUE_FALSE(_b_)		((_b_) ? _T("True") : _T("False"))
#define BOOLEAN_TO_STRING_YES_NO(_b_)			((_b_) ? _T("Yes") : _T("No"))

#define STR_DOUBLE_HYPHEN_B	"========== "
#define STR_DOUBLE_HYPHEN_E	" ==========\n"
#define STR_LBA				"LBA[%06d, %#07x]: "
#define STR_OPCODE			"OpCode[%#02x]: "
#define STR_C2FLAG			"C2flag[%d]: "
#define STR_SUBCODE			"SubCode[%x]: "
#define STR_TRACK			"Track[%02d]: "
#define STR_SUB				"Sub"
#define STR_NO_SUPPORT		" doesn't support on this drive\n"

#define OUTPUT_DHYPHEN_PLUS_STR(str)					STR_DOUBLE_HYPHEN_B str STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(str)			STR_DOUBLE_HYPHEN_B STR_LBA str STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA				STR_DOUBLE_HYPHEN_B STR_LBA "%" CHARWIDTH "s" STR_DOUBLE_HYPHEN_E

#ifdef RUN_FROM_GAME
#define pspPrintf pspDebugScreenPrintf
#else
#define pspPrintf pspDebugScreenKprintf
#endif

// http://www.katsuster.net/index.php?arg_act=cmd_show_diary&arg_date=20160108
#define OutputString(str, ...)		pspPrintf(str, ##__VA_ARGS__);

 // If it uses g_LogFile, call InitLogFile()
extern _LOG_FILE g_LogFile;
#define FlushLog() \
{ \
	fflush(g_LogFile.fpVolDesc); \
	fflush(g_LogFile.fpMainInfo); \
	fflush(g_LogFile.fpMainError); \
}

#define OutputErrorString(str, ...)	pspPrintf(str, ##__VA_ARGS__);

#define OutputDiscLog(str, ...)		fprintf(g_LogFile.fpDisc, str, ##__VA_ARGS__);
#define OutputDiscWithLBALog(str, nLBA, ...) \
	fprintf(g_LogFile.fpDisc, STR_LBA str, nLBA, nLBA, ##__VA_ARGS__);

#define OutputVolDescLog(str, ...)		fprintf(g_LogFile.fpVolDesc, str, ##__VA_ARGS__);
#define OutputVolDescWithLBALog1(str1, nLBA, ...) \
	fprintf(g_LogFile.fpVolDesc, OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(str1), nLBA, (UINT)nLBA, ##__VA_ARGS__);
#define OutputVolDescWithLBALog2(str1, str2, nLBA, ...) \
	fprintf(g_LogFile.fpVolDesc, OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(str1) str2, nLBA, (UINT)nLBA, ##__VA_ARGS__);

#define OutputMainInfoLog(str, ...)	fprintf(g_LogFile.fpMainInfo, str, ##__VA_ARGS__);
#define OutputMainInfoWithLBALog(str, nLBA, track, ...) \
	fprintf(g_LogFile.fpMainInfo, STR_LBA STR_TRACK str, nLBA, nLBA, track, ##__VA_ARGS__);

#define OutputMainErrorLog(str, ...)	if (g_LogFile.fpMainError) fprintf(g_LogFile.fpMainError, str, ##__VA_ARGS__);
#define OutputMainErrorWithLBALog(str, nLBA, track, ...) \
	if (g_LogFile.fpMainError) fprintf(g_LogFile.fpMainError, STR_LBA STR_TRACK str, nLBA, nLBA, track, ##__VA_ARGS__);

#define OutputLog(type, str, ...) \
{ \
	INT t = type; \
	if ((t & standardOut) == standardOut) { \
		OutputString(str, ##__VA_ARGS__); \
	} \
	if ((t & standardError) == standardError) { \
		OutputErrorString(str, ##__VA_ARGS__); \
	} \
	if ((t & fileDisc) == fileDisc) { \
		OutputDiscLog(str, ##__VA_ARGS__); \
	} \
	if ((t & fileVolDesc) == fileVolDesc) { \
		OutputVolDescLog(str, ##__VA_ARGS__); \
	} \
	if ((t & fileMainInfo) == fileMainInfo) { \
		OutputMainInfoLog(str, ##__VA_ARGS__); \
	} \
	if ((t & fileMainError) == fileMainError) { \
		OutputMainErrorLog(str, ##__VA_ARGS__); \
	} \
}
#ifdef UNICODE
#define WFLAG "w, ccs=UTF-8"
#define AFLAG "a, ccs=UTF-8"
#else
#define WFLAG "w"
#define AFLAG "a"
#endif

#define FcloseAndNull(fp) \
{ \
	if (fp) { \
		fclose(fp); \
		fp = NULL; \
	} \
}

#define FreeAndNull(lpBuf) \
{ \
	if (lpBuf) { \
		free(lpBuf); \
		lpBuf = NULL; \
	} \
}

VOID OutputLastErrorNumAndString(
	INT errnum,
	LPCTSTR pszFuncName,
	LONG lLineNum
);

int OutputParamSfo(const char* paramsfo);

int CreateFile(char* id, unsigned int discType, const char* filename, FILE** fp, const char* type);

void DumpIso(char* id, unsigned int discType, unsigned int discSize, int nDump);
