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
#include <pspumd.h>

WORD GetSizeOrWordForVolDesc(
	LPBYTE lpBuf
);

DWORD GetSizeOrDwordForVolDesc(
	LPBYTE lpBuf,
	DWORD dwMax
);

int GetDiscInfoToConsole(pspUmdInfo* pDiscInfo);

int GetDiscInfoToLog(char* id, unsigned int discType, unsigned int* pDiscSize);

int GetDiscID(char* id);

int GetMSInfo(MS_INFO *info);
