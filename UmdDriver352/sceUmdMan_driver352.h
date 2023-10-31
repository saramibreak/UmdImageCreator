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
#ifdef __cplusplus
extern "C" {
#endif

int sceUmdExecAdjustDataCmd(void* arg1, void* arg2, void* arg3, void* arg4);
int sceUmdExecAllocateFromReadCmd(void* arg1, void* arg2, void* arg3);
int sceUmdExecGetEventStatusCmd(void* arg1, void* arg2, void* arg3);
int sceUmdExecInquiryCmd(void* drive, unsigned char* param, unsigned char* buf);
int sceUmdExecModSelectCmd(void* arg1, void* arg2, void* arg3);
int sceUmdExecModSenseCmd(void* arg1, void* arg2, void* arg3);
int sceUmdExecPrefetch10Cmd(void* arg1, void* arg2);
int sceUmdExecPreventAllowMediaCmd(void* arg);
int sceUmdExecRead10Cmd(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, void* arg7);
int sceUmdExecReadCapacityCmd(unsigned int cmd, void* drive, unsigned int size, unsigned char* buf);
int sceUmdExecReadDiscInfoCmd(void* arg1, unsigned int arg2, void* arg3);
int sceUmdExecReadMKICmd(void* arg1, void* arg2, unsigned int arg3, void* arg4);
int sceUmdExecReadUMDStructureCmd(void* arg1, unsigned char* arg2, unsigned char* arg3);
int sceUmdExecReportCacheCmd(void* arg1, void* arg2, void* arg3);
int sceUmdExecReqSenseCmd(void* arg1, void* arg2);
int sceUmdExecSeekCmd(unsigned int pos);
int sceUmdExecSetAreaLimitCmd(void* arg1, void* arg2);
int sceUmdExecSetCDSpeedCmd(void* arg1, void* arg2);
int sceUmdExecSetLockLengthCmd(unsigned int val);
int sceUmdExecStartStopUnitCmd(void* arg1, void* arg2);
int sceUmdExecTestCmd(void* arg1, void* arg2, void* arg3, void* arg4);
int sceUmdManActivate();
int sceUmdManCancelAlarm();
int sceUmdManChangePowerMode(void* arg1, void* arg2);
int sceUmdManCheckDeviceReady(void* drive, const char* dev);
int sceUmdManExecAtaCmd(void* arg1, void* arg2, void* arg3);
int sceUmdManGetBootFrom();
int sceUmdManGetCommandTimerFlag();
int sceUmdManGetDiscInfo(void* buf);
int sceUmdManGetDiscInfo4VSH(void* buf);
int sceUmdManGetErrorStatus();
int sceUmdManGetInquiry(void* drive, unsigned int size, unsigned char* buf);
int sceUmdManGetIntrStateFlag();
int sceUmdManGetPowerStat(int driveNum);
int sceUmdManGetReadyFlag();
void* sceUmdManGetUmdDiscInfo();
void* sceUmdManGetUmdDrive(int driveNum);
int sceUmdManInit();
int sceUmdManIsUmdDrive();
int sceUmdManLeptonReset();
int sceUmdManMediaPresent();
int sceUmdManPollSema();
int sceUmdManRegisterImposeCallBack(void* arg1, void* arg2);
int sceUmdManRegisterInsertEjectUMDCallBack(void* arg1, void* arg2, void* arg3);
int sceUmdManSetAlarm(unsigned int val1);
int sceUmdManSetDisableReadAhead(void* arg1);
int sceUmdManSetEnableReadAhead(void* arg1);
int sceUmdManSetReadAheadSize(void* arg1, void* arg2, void* arg3, void* arg4);
int sceUmdManSignalSema();
int sceUmdManSoftReset();
void* sceUmdManSPKGetMKI();
void* sceUmdManStart(void* arg1);
int sceUmdManStop(unsigned int val);
int sceUmdManTerm();
int sceUmdManUnRegisterImposeCallback();
int sceUmdManUnRegisterInsertEjectUMDCallBack(void* arg1);
int sceUmdManValidateUMD(void* arg1, void* arg2);
int sceUmdManWaitSema();

int sceUmdExecClearCacheInfoCmd(void* arg1, void* arg2);
int sceUmdExecGetConfigurationCmd(void* arg1, void* arg2, void* arg3);
int sceUmdExecGetErrorLogCmd(void* arg1, void* arg2, void* arg3);
int sceUmdExecGetMediaInfoCmd(void* arg1, unsigned int arg2, void* arg3);
int sceUmdExecMechaStatCmd(void* arg1, unsigned int arg2, void* arg3);
int sceUmdExecSetAccessLimitCmd(void* arg1, void* arg2);
int sceUmdExecSetStreamingCmd(void* arg1, void* arg2, void* arg3);

/* These are commands that have been successfully called as functions
sceUmdExecInquiryCmd     		-> 12h	INQUIRY
sceUmdExecReadCapacityCmd		-> 25h	READ CAPACITY
sceUmdExecReadUMDStructureCmd   -> ADh	READ DVD STRUCTURE
*/

/* These are commands that have not been successfully called as functions
sceUmdExecTestCmd               -> 00h	TEST UNIT READY
sceUmdExecReqSenseCmd			-> 03h	REQUEST SENSE
sceUmdExecStartStopUnitCmd		-> 1Bh	START STOP UNIT
sceUmdExecPreventAllowMediaCmd	-> 1Eh	PREVENT ALLOW MEDIUM REMOVAL
sceUmdExecRead10Cmd				-> 28h	READ(10)
sceUmdExecSeekCmd				-> 2Bh	SEEK(10)
sceUmdExecPrefetch10Cmd			-> 34h	PRE-FETCH(10)
sceUmdExecGetEventStatusCmd		-> 4Ah	GET EVENT STATUS NOTIFICATION
sceUmdExecModSenseCmd			-> 5Ah	MODE SENSE(10)
sceUmdExecModSelectCmd			-> 55h	MODE SELECT(10)
sceUmdExecSetStreamingCmd       -> B6h	SET_STREAMING
sceUmdExecSetCDSpeedCmd         -> BBh	SET CD SPEED
sceUmdExecMechaStatCmd          -> BDh	MECHANISM_STATUS
sceUmdExecAdjustDataCmd         -> UNKNOWN
sceUmdExecAllocateFromReadCmd   -> UNKNOWN
sceUmdExecClearCacheInfoCmd     -> UNKNOWN
sceUmdExecGetErrorLogCmd        -> UNKNOWN
sceUmdExecReportCacheCmd        -> UNKNOWN
sceUmdExecSetAccessLimitCmd     -> UNKNOWN
sceUmdExecSetAreaLimitCmd       -> UNKNOWN
sceUmdExecSetLockLengthCmd      -> UNKNOWN
sceUmdExecReadMKICmd            -> UNKNOWN
*/

/* Others
sceUmdExecGetConfigurationCmd	-> 46h	GET CONFIGURATION --> It seems devkit only
sceUmdExecReadDiscInfoCmd		-> 51h	READ DISC INFORMATION --> It seems devkit only

sceUmdExecGetMediaInfoCmd       -> It's UMD specific
*/

#ifdef __cplusplus
}
#endif

