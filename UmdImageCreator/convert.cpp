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
#include "struct.h"
#include "convert.h"

DWORD PadSizeForVolDesc(
	DWORD dwSize
) {
	INT nPadding = DISC_RAW_READ_SIZE - (INT)dwSize;
	// dwSize isn't 2048 byte
	if (nPadding != 0) {
		// dwSize is smaller than 2048 byte
		if (nPadding > 0){
			// Generally, directory size is per 2048 byte
			// Exception:
			//  Codename - Outbreak (Europe) (Sold Out Software)
			//  Commandos - Behind Enemy Lines (Europe) (Sold Out Software)
			// and more
			dwSize += nPadding;
		}
		// dwSize is larger than 2048 byte
		else {
			nPadding = (INT)dwSize % DISC_RAW_READ_SIZE;
			// dwSize isn't 4096, 6144, 8192 etc byte
			if (nPadding != 0) {
				nPadding = DISC_RAW_READ_SIZE - nPadding;
				dwSize += nPadding;
			}
		}
	}
	return dwSize;
}
