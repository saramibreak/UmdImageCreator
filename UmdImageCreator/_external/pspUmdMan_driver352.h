#ifdef __cplusplus
extern "C" {
#endif

int _sceUmdExecAdjustDataCmd(void* arg1, void* arg2, void* arg3, void* arg4);
int _sceUmdExecAllocateFromReadCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdExecGetEventStatusCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdExecInquiryCmd(void* drive, unsigned char* param, unsigned char* buf);
int _sceUmdExecModSelectCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdExecModSenseCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdExecPrefetch10Cmd(void* arg1, void* arg2);
int _sceUmdExecPreventAllowMediaCmd(void* arg);
int _sceUmdExecRead10Cmd(void* arg1, void* arg2, void* arg3, void* arg4, void* arg5, void* arg6, void* arg7);
int _sceUmdExecReadCapacityCmd(unsigned int cmd, void* drive, unsigned int size, unsigned char* buf);
int _sceUmdExecReadDiscInfoCmd(void* arg1, unsigned int arg2, void* arg3);
int _sceUmdExecReadMKICmd(void* arg1, void* arg2, void* arg3, void* arg4);
int _sceUmdExecReadUMDStructureCmd(void* arg1, unsigned char* arg2, unsigned char* arg3);
int _sceUmdExecReportCacheCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdExecReqSenseCmd(void* arg1, void* arg2);
int _sceUmdExecSeekCmd(unsigned int pos);
int _sceUmdExecSetAreaLimitCmd(void* arg1, void* arg2);
int _sceUmdExecSetCDSpeedCmd(void* arg1, void* arg2);
int _sceUmdExecSetLockLengthCmd(unsigned int val);
int _sceUmdExecStartStopUnitCmd(void* arg1, void* arg2);
int _sceUmdExecTestCmd(void* arg1, void* arg2, void* arg3, void* arg4);
int _sceUmdManActivate();
int _sceUmdManCancelAlarm();
int _sceUmdManChangePowerMode(void* arg1, void* arg2);
int _sceUmdManCheckDeviceReady(void* drive, const char* dev);
int _sceUmdManExecAtaCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdManGetBootFrom();
int _sceUmdManGetCommandTimerFlag();
int _sceUmdManGetDiscInfo(void* buf);
int _sceUmdManGetDiscInfo4VSH(void* buf);
int _sceUmdManGetErrorStatus();
int _sceUmdManGetInquiry(void* drive, unsigned int size, unsigned char* buf);
int _sceUmdManGetIntrStateFlag();
int _sceUmdManGetPowerStat(int driveNum);
int _sceUmdManGetReadyFlag();
void* _sceUmdManGetUmdDiscInfo();
void* _sceUmdManGetUmdDrive(int driveNum);
int _sceUmdManInit();
int _sceUmdManIsUmdDrive();
int _sceUmdManLeptonReset();
int _sceUmdManMediaPresent();
int _sceUmdManPollSema();
int _sceUmdManRegisterImposeCallBack(void* arg1, void* arg2);
int _sceUmdManRegisterInsertEjectUMDCallBack(void* arg1, void* arg2, void* arg3);
int _sceUmdManSetAlarm(unsigned int val1);
int _sceUmdManSetDisableReadAhead(void* arg1);
int _sceUmdManSetEnableReadAhead(void* arg1);
int _sceUmdManSetReadAheadSize(void* arg1, void* arg2, void* arg3, void* arg4);
int _sceUmdManSignalSema();
int _sceUmdManSoftReset();
int _sceUmdManSPKGetMKI();
void* _sceUmdManStart(void* arg1);
int _sceUmdManStop(unsigned int val);
int _sceUmdManTerm();
int _sceUmdManUnRegisterImposeCallback();
int _sceUmdManUnRegisterInsertEjectUMDCallBack(void* arg1);
int _sceUmdManValidateUMD(void* arg1, void* arg2);
int _sceUmdManWaitSema();

int _sceUmdExecClearCacheInfoCmd(void* arg1, void* arg2);
int _sceUmdExecGetConfigurationCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdExecGetErrorLogCmd(void* arg1, void* arg2, void* arg3);
int _sceUmdExecGetMediaInfoCmd(void* arg1, unsigned int arg2, void* arg3);
int _sceUmdExecMechaStatCmd(void* arg1, unsigned int arg2, void* arg3);
int _sceUmdExecSetAccessLimitCmd(void* arg1, void* arg2);
int _sceUmdExecSetStreamingCmd(void* arg1, void* arg2, void* arg3);

#ifdef __cplusplus
}
#endif
