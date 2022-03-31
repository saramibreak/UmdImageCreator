/**
 * Copyright 2018-2022 sarami
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

#define DISC_MAIN_DATA_SIZE		(2048)

#define PATH_TABLE_RECORD_SIZE	(400)

// 情報交換の水準
// 水準	1	2	3
// ファイル識別子	8+3文字	30文字	30文字
// ディレクトリ識別子	8文字	31文字	31文字
// Romeo Extensions
// 1バイト文字で128文字までの長いファイル名を記録できる。
// Joliet Extensions
// Unicodeで64文字までの長いファイル名を記録できる。
#define MAX_FNAME_FOR_VOLUME (64)
#define MIN_LEN_DR (34)

#define MAKEUINT(a, b)      ((UINT)(((WORD)(((UINT_PTR)(a)) & 0xffff)) | ((UINT)((WORD)(((UINT_PTR)(b)) & 0xffff))) << 16))
#define MAKEDWORD(a, b)      ((DWORD)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))

#define CTL_CODE(IoctlType, DeviceType, Dst, Src, Priv, Cache, ThWait, Function) (\
    ((IoctlType) << 24) | ((DeviceType) << 20) | ((Dst) << 17) | ((Src) << 16) | ((Priv) << 15) | ((Cache) << 14) | ((ThWait) << 12) | (Function) \
)

#define IOCTL_UMD		0x1
#define DEVICE_TYPE_UMD	0xf
#define IOCTL_MS		0x2
#define DEVICE_TYPE_MS	0x4

#define DST_BLK_USED              0x1
#define DST_BLK_UNUSED            0x0
#define SRC_BLK_USED              0x1
#define SRC_BLK_UNUSED            0x0
#define PRIV_REQUIRED             0x1
#define PRIV_NOT_REQUIRED         0x0
#define CACHE_NOT_COHERENCY       0x1
#define CACHE_COHERENCY           0x0
#define NOT_THREAD_WAIT_3         0x3
#define NOT_THREAD_WAIT_2         0x2
#define THREAD_WAIT               0x1
#define THREAD_WAIT_WITH_CALLBACK 0x0

#define IOCTL_UMD_SEEK               CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_UNUSED, SRC_BLK_USED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x0a6)

#define IOCTL_UMD_GET_MEDIA_INFO     CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x001)
#define IOCTL_UMD_GET_LAST_LBA       CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x002)
#define IOCTL_UMD_GET_LAST_LBA_OF_L0 CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x003)
#define IOCTL_UMD_GET_INQUIRY_DATA   CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x014)
#define IOCTL_UMD_0xA0               CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x0a0)
#define IOCTL_UMD_0xA1               CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x0a1)
#define IOCTL_UMD_0xA2               CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x0a2)
#define IOCTL_UMD_0xA4               CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x0a4)
#define IOCTL_UMD_0xA5               CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x0a5)
#define IOCTL_UMD_0x011              CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT, 0x011)

#define IOCTL_UMD_GET_MAIN_CHANNEL   CTL_CODE(IOCTL_UMD, DEVICE_TYPE_UMD, DST_BLK_USED, SRC_BLK_USED, PRIV_NOT_REQUIRED, CACHE_COHERENCY, THREAD_WAIT_WITH_CALLBACK, 0x003)

#define IOCTL_MS_GET_MS_INFO         CTL_CODE(IOCTL_MS, DEVICE_TYPE_MS, DST_BLK_USED, SRC_BLK_UNUSED, PRIV_NOT_REQUIRED, CACHE_NOT_COHERENCY, THREAD_WAIT, 0x818)

#define DEVICE_UMD "umd0:"
#define DEVICE_DISC "disc0:"
#define DEVICE_MS "ms0:"

/* from scsi.h */
//
// Inquiry defines. Used to interpret data returned from target as result
// of inquiry command.
//
// DeviceType field
//

#define DIRECT_ACCESS_DEVICE            0x00    // disks
#define SEQUENTIAL_ACCESS_DEVICE        0x01    // tapes
#define PRINTER_DEVICE                  0x02    // printers
#define PROCESSOR_DEVICE                0x03    // scanners, printers, etc
#define WRITE_ONCE_READ_MULTIPLE_DEVICE 0x04    // worms
#define READ_ONLY_DIRECT_ACCESS_DEVICE  0x05    // cdroms
#define SCANNER_DEVICE                  0x06    // scanners
#define OPTICAL_DEVICE                  0x07    // optical disks
#define MEDIUM_CHANGER                  0x08    // jukebox
#define COMMUNICATION_DEVICE            0x09    // network
// 0xA and 0xB are obsolete
#define ARRAY_CONTROLLER_DEVICE         0x0C
#define SCSI_ENCLOSURE_DEVICE           0x0D
#define REDUCED_BLOCK_DEVICE            0x0E    // e.g., 1394 disk
#define OPTICAL_CARD_READER_WRITER_DEVICE 0x0F
#define BRIDGE_CONTROLLER_DEVICE        0x10
#define OBJECT_BASED_STORAGE_DEVICE     0x11    // OSD
#define HOST_MANAGED_ZONED_BLOCK_DEVICE 0x14    // Host managed zoned block device
#define UNKNOWN_OR_NO_DEVICE            0x1F    // Unknown or no device type
#define LOGICAL_UNIT_NOT_PRESENT_DEVICE 0x7F
#define DEVICE_QUALIFIER_ACTIVE         0x00
#define DEVICE_QUALIFIER_NOT_ACTIVE     0x01
#define DEVICE_QUALIFIER_NOT_SUPPORTED  0x03
