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
#include "enum.h"

#define DISC_RAW_READ_SIZE		(2048)

#define DIRECTORY_RECORD_SIZE	(400)

// �������̐���
// ����	1	2	3
// �t�@�C�����ʎq	8+3����	30����	30����
// �f�B���N�g�����ʎq	8����	31����	31����
// Romeo Extensions
// 1�o�C�g������128�����܂ł̒����t�@�C�������L�^�ł���B
// Joliet Extensions
// Unicode��64�����܂ł̒����t�@�C�������L�^�ł���B
#define MAX_FNAME_FOR_VOLUME (64)
#define MIN_LEN_DR (34)

#define MAKEDWORD(a, b)      ((DWORD)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))

