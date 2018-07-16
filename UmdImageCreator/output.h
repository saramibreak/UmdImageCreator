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

#define BOOLEAN_TO_STRING_TRUE_FALSE_W(_b_)		((_b_) ? _T("True") : _T("False"))
#define BOOLEAN_TO_STRING_TRUE_FALSE_A(_b_)		((_b_) ? "True" : "False")
#define BOOLEAN_TO_STRING_YES_NO_W(_b_)			((_b_) ? _T("Yes") : _T("No"))
#define BOOLEAN_TO_STRING_YES_NO_A(_b_)			((_b_) ? "Yes" : "No")

#define STR_DOUBLE_HYPHEN_B		"========== "
#define STR_DOUBLE_HYPHEN_E		" ==========\n"
#define STR_LBA					"LBA[%06d, %#07x]: "
#define STR_OPCODE				"OpCode[%#02x]: "
#define STR_C2FLAG				"C2flag[%x]: "
#define STR_SUBCODE				"SubCode[%x]: "
#define STR_TRACK				"Track[%02u]: "
#define STR_SUB					"Sub"
#define STR_NO_SUPPORT			" doesn't support on this drive\n"

#define OUTPUT_DHYPHEN_PLUS_STR(str)				STR_DOUBLE_HYPHEN_B #str STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(str)		STR_DOUBLE_HYPHEN_B STR_LBA #str STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_SUBCH_F(str)	STR_DOUBLE_HYPHEN_B STR_OPCODE STR_SUBCODE #str STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_C2_SUBCH_F(str)	STR_DOUBLE_HYPHEN_B STR_OPCODE STR_C2FLAG STR_SUBCODE #str STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_TRACK_F(str)	STR_DOUBLE_HYPHEN_B STR_OPCODE STR_SUBCODE STR_TRACK #str STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA			STR_DOUBLE_HYPHEN_B STR_LBA "%s" STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_SUBCH			STR_DOUBLE_HYPHEN_B STR_SUBCODE "%s" STR_DOUBLE_HYPHEN_E
#define OUTPUT_DHYPHEN_PLUS_STR_WITH_TRACK			STR_DOUBLE_HYPHEN_B STR_TRACK "%s" STR_DOUBLE_HYPHEN_E
#define OUTPUT_STR_NO_SUPPORT(str)					#str STR_NO_SUPPORT

#ifdef RUN_FROM_GAME
#define pspPrintf pspDebugScreenPrintf
#else
#define pspPrintf pspDebugScreenKprintf
#endif

// http://www.katsuster.net/index.php?arg_act=cmd_show_diary&arg_date=20160108
#define OutputStringW(str, ...)		_tprintf(str, ##__VA_ARGS__);
#define OutputStringA(str, ...)		pspPrintf(str, ##__VA_ARGS__);

 // If it uses g_LogFile, call InitLogFile()
extern _LOG_FILE g_LogFile;
#define FlushLog() \
{ \
	fflush(g_LogFile.fpVolDesc); \
	fflush(g_LogFile.fpMainInfo); \
	fflush(g_LogFile.fpMainError); \
}

#define OutputErrorStringW(str, ...)	fwprintf(stderr, str, ##__VA_ARGS__);
#define OutputErrorStringA(str, ...)	pspPrintf(str, ##__VA_ARGS__);

#define OutputDiscLogW(str, ...)		fwprintf(g_LogFile.fpDisc, str, ##__VA_ARGS__);
#define OutputDiscLogA(str, ...)		fprintf(g_LogFile.fpDisc, str, ##__VA_ARGS__);
#define OutputDiscWithLBALogA(str, nLBA, ...) \
	fprintf(g_LogFile.fpDisc, STR_LBA str, nLBA, nLBA, ##__VA_ARGS__);

#define OutputVolDescLogW(str, ...)		fwprintf(g_LogFile.fpVolDesc, str, ##__VA_ARGS__);
#define OutputVolDescLogA(str, ...)		fprintf(g_LogFile.fpVolDesc, str, ##__VA_ARGS__);
#define OutputVolDescWithLBALogA(str1, str2, nLBA, ...) \
	fprintf(g_LogFile.fpVolDesc, OUTPUT_DHYPHEN_PLUS_STR_WITH_LBA_F(str1) str2, nLBA, nLBA, ##__VA_ARGS__);

#define OutputMainInfoLogW(str, ...)	fwprintf(g_LogFile.fpMainInfo, str, ##__VA_ARGS__);
#define OutputMainInfoLogA(str, ...)	fprintf(g_LogFile.fpMainInfo, str, ##__VA_ARGS__);
#define OutputMainInfoWithLBALogA(str, nLBA, track, ...) \
	fprintf(g_LogFile.fpMainInfo, STR_LBA STR_TRACK str, nLBA, nLBA, track, ##__VA_ARGS__);

#define OutputMainErrorLogW(str, ...)	if (g_LogFile.fpMainError) fwprintf(g_LogFile.fpMainError, str, ##__VA_ARGS__);
#define OutputMainErrorLogA(str, ...)	if (g_LogFile.fpMainError) fprintf(g_LogFile.fpMainError, str, ##__VA_ARGS__);
#define OutputMainErrorWithLBALogA(str, nLBA, track, ...) \
	if (g_LogFile.fpMainError) fprintf(g_LogFile.fpMainError, STR_LBA STR_TRACK str, nLBA, nLBA, track, ##__VA_ARGS__);

#define OutputLogW(type, str, ...) \
{ \
	INT t = type; \
	if ((t & standardOut) == standardOut) { \
		OutputStringW(str, ##__VA_ARGS__); \
	} \
	if ((t & standardError) == standardError) { \
		OutputErrorStringW(str, ##__VA_ARGS__); \
	} \
	if ((t & fileDisc) == fileDisc) { \
		OutputDiscLogW(str, ##__VA_ARGS__); \
	} \
	if ((t & fileVolDesc) == fileVolDesc) { \
		OutputVolDescLogW(str, ##__VA_ARGS__); \
	} \
	if ((t & fileMainInfo) == fileMainInfo) { \
		OutputMainInfoLogW(str, ##__VA_ARGS__); \
	} \
	if ((t & fileMainError) == fileMainError) { \
		OutputMainErrorLogW(str, ##__VA_ARGS__); \
	} \
}
#define OutputLogA(type, str, ...) \
{ \
	INT t = type; \
	if ((t & standardOut) == standardOut) { \
		OutputStringA(str, ##__VA_ARGS__); \
	} \
	if ((t & standardError) == standardError) { \
		OutputErrorStringA(str, ##__VA_ARGS__); \
	} \
	if ((t & fileDisc) == fileDisc) { \
		OutputDiscLogA(str, ##__VA_ARGS__); \
	} \
	if ((t & fileVolDesc) == fileVolDesc) { \
		OutputVolDescLogA(str, ##__VA_ARGS__); \
	} \
	if ((t & fileMainInfo) == fileMainInfo) { \
		OutputMainInfoLogA(str, ##__VA_ARGS__); \
	} \
	if ((t & fileMainError) == fileMainError) { \
		OutputMainErrorLogA(str, ##__VA_ARGS__); \
	} \
}
#ifdef UNICODE
#define WFLAG "w, ccs=UTF-8"
#define AFLAG "a, ccs=UTF-8"
#define BOOLEAN_TO_STRING_TRUE_FALSE	BOOLEAN_TO_STRING_TRUE_FALSE_W
#define BOOLEAN_TO_STRING_YES_NO		BOOLEAN_TO_STRING_YES_NO_W
#define OutputString			OutputStringW
#define OutputErrorString		OutputErrorStringW
#define OutputVolDescLog		OutputVolDescLogW
#define OutputMainInfoLog		OutputMainInfoLogW
#define OutputMainErrorLog		OutputMainErrorLogW
#define OutputLog				OutputLogW
#else
#define WFLAG "w"
#define AFLAG "a"
#define BOOLEAN_TO_STRING_TRUE_FALSE	BOOLEAN_TO_STRING_TRUE_FALSE_A
#define BOOLEAN_TO_STRING_YES_NO		BOOLEAN_TO_STRING_YES_NO_A
#define OutputString			OutputStringA
#define OutputErrorString		OutputErrorStringA
#define OutputVolDescLog		OutputVolDescLogA
#define OutputMainInfoLog		OutputMainInfoLogA
#define OutputMainErrorLog		OutputMainErrorLogA
#define OutputLog				OutputLogA
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

int CreateFile(char* id, const char* filename, FILE** fp, const char* type);

void DumpIso(char* id, int nDump);