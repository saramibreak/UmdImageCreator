#include <pspsdk.h>
#include <pspkernel.h>
#include "sceUmdMan_driver.h"

PSP_MODULE_INFO("pspUmdMan_driver", PSP_MODULE_KERNEL, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

void *_sceUmdManGetUmdDrive(int driveNum)
{
	int k1 = pspSdkSetK1(0);

	void *drive = (void*)sceUmdManGetUmdDrive(driveNum);

	pspSdkSetK1(k1);
	return(drive);
}

int _sceUmdManGetInquiry(void* drive, unsigned char* size, unsigned char* buf)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetInquiry(drive, size, buf);

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

/// <summary>
/// These function can use only FW 1.5x to 3.5x
/// </summary>
int _sceUmdManActivate(int unit, const char* drive)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManActivate(unit, drive);

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

int _sceUmdManGetDiscInfo(void* buf)
{
	int k1 = pspSdkSetK1(0);

	int ret = sceUmdManGetDiscInfo(buf);

	pspSdkSetK1(k1);
	return(ret);
}

void* _sceUmdManGetUmdDiscInfo(int driveNum)
{
	int k1 = pspSdkSetK1(0);

	void* ret = sceUmdManGetUmdDiscInfo(driveNum);

	pspSdkSetK1(k1);
	return(ret);
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop()
{
	return 0;
}
