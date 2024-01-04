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
#include <pspsdk.h>
#include <pspkernel.h>
#include "sceUmdMan_driver.h"

PSP_MODULE_INFO("pspUmdMan_driver", PSP_MODULE_KERNEL, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

int _sceUmdExecAdjustDataCmd(void* arg1, void* arg2, void* arg3, void* arg4)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecAdjustDataCmd(arg1, arg2, arg3, arg4);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecAllocateFromReadCmd(void* arg1, void* arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecAllocateFromReadCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecGetEventStatusCmd(void* arg1, void* arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecGetEventStatusCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecInquiryCmd(void* drive, unsigned char* param, unsigned char* buf)
{
	int k1 = pspSdkSetK1(0);

	int res = sceUmdExecInquiryCmd(drive, param, buf);

	pspSdkSetK1(k1);
	return(res);
}

int _sceUmdExecModSelectCmd(void* arg1, void* arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecModSelectCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecModSenseCmd(void* arg1, void* arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecModSenseCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecPrefetch10Cmd(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecPrefetch10Cmd(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecPreventAllowMediaCmd(void* arg)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecPreventAllowMediaCmd(arg);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecRead10Cmd(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, void* arg7)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecRead10Cmd(arg1, arg2, arg3, arg4, arg5, arg6, arg7);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecReadCapacityCmd(unsigned int cmd, void* drive, unsigned int size, unsigned char* buf)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecReadCapacityCmd(cmd, drive, size, buf);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecReadDiscInfoCmd(void* arg1, unsigned int arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecReadDiscInfoCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecReadMKICmd(void* arg1, void* arg2, unsigned int arg3, void* arg4)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecReadMKICmd(arg1, arg2, arg3, arg4);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecReadUMDStructureCmd(void* arg1, unsigned char* arg2, unsigned char* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecReadUMDStructureCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecReportCacheCmd(void* arg1, void* arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecReportCacheCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecReqSenseCmd(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecReqSenseCmd(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecSeekCmd(unsigned int pos)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecSeekCmd(pos);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecSetAreaLimitCmd(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecSetAreaLimitCmd(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecSetCDSpeedCmd(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecSetCDSpeedCmd(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecSetLockLengthCmd(unsigned int val)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecSetLockLengthCmd(val);

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdExecStartStopUnitCmd(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecStartStopUnitCmd(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdExecTestCmd(void* arg1, void* arg2, void* arg3, void* arg4)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdExecTestCmd(arg1, arg2, arg3, arg4);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManActivate()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManActivate();

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManCancelAlarm()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManCancelAlarm();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManChangePowerMode(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManChangePowerMode(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManCheckDeviceReady(void* drive, const char* dev)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManCheckDeviceReady(drive, dev);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManExecAtaCmd(void* arg1, void* arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManExecAtaCmd(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManGetBootFrom()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetBootFrom();

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManGetCommandTimerFlag()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetCommandTimerFlag();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManGetDiscInfo(void* buf)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetDiscInfo(buf);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManGetDiscInfo4VSH(void* buf)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetDiscInfo4VSH(buf);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManGetErrorStatus()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetErrorStatus();

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManGetInquiry(void* drive, unsigned int size, unsigned char* buf)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetInquiry(drive, size, buf);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManGetIntrStateFlag()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetIntrStateFlag();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManGetPowerStat(int driveNum)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetPowerStat(driveNum);

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManGetReadyFlag()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetReadyFlag();

	pspSdkSetK1(k1);
	return(ret);
}

void* _sceUmdManGetUmdDiscInfo()
{
	int k1 = pspSdkSetK1(0);

	void* ret = sceUmdManGetUmdDiscInfo();

	pspSdkSetK1(k1);
	return(ret);
}

void *_sceUmdManGetUmdDrive(int driveNum)
{
	int k1 = pspSdkSetK1(0);

	void *drive = (void*)sceUmdManGetUmdDrive(driveNum);

	pspSdkSetK1(k1);
	return(drive);
}

int _sceUmdManInit()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManInit();

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManIsUmdDrive()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManIsUmdDrive();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManLeptonReset()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManLeptonReset();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManMediaPresent()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManMediaPresent();

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManPollSema()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManPollSema();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManRegisterImposeCallBack(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManRegisterImposeCallBack(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManRegisterInsertEjectUMDCallBack(void* arg1, void* arg2, void* arg3)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManRegisterInsertEjectUMDCallBack(arg1, arg2, arg3);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManSetAlarm(unsigned int val1)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManSetAlarm(val1);

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManSetDisableReadAhead(void* arg1)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManSetDisableReadAhead(arg1);

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManSetEnableReadAhead(void* arg1)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManSetEnableReadAhead(arg1);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManSetReadAheadSize(void* arg1, void* arg2, void* arg3, void* arg4)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManSetReadAheadSize(arg1, arg2, arg3, arg4);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManSignalSema()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManSignalSema();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManSoftReset()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManSoftReset();

	pspSdkSetK1(k1);
	return ret;
}

void* _sceUmdManSPKGetMKI()
{
	int k1 = pspSdkSetK1(0);

	void* ret = sceUmdManSPKGetMKI();

	pspSdkSetK1(k1);
	return ret;
}

void* _sceUmdManStart(void* arg1)
{
	int k1 = pspSdkSetK1(0);

	void* ret = sceUmdManStart(arg1);

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManStop(unsigned int val)
{
	int k1 = pspSdkSetK1(0);

	// 1 or 16 is used
	int ret = sceUmdManStop(val);

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManTerm()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManTerm();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManUnRegisterImposeCallback()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManUnRegisterImposeCallback();

	pspSdkSetK1(k1);
	return ret;
}

int _sceUmdManUnRegisterInsertEjectUMDCallBack(void* arg1)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManUnRegisterInsertEjectUMDCallBack(arg1);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManValidateUMD(void* arg1, void* arg2)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManValidateUMD(arg1, arg2);

	pspSdkSetK1(k1);
	return(ret);
}

int _sceUmdManWaitSema()
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManWaitSema();

	pspSdkSetK1(k1);
	return ret;
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop()
{
	return 0;
}
