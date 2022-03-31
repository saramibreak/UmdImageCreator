#ifdef __cplusplus
extern "C" {
#endif

void* sceUmdManGetUmdDrive(int driveNum);
int sceUmdManGetInquiry(void* drive, u8* size, u8* buf);

// sceUmdExecXXXCmd looks like the SCSI COMMAND
// See -> https://en.wikipedia.org/wiki/SCSI_command
int sceUmdExecInquiryCmd(void* drive, u8* param, u8* buf); // same as "12	INQUIRY" of SCSI command
#if 0
// These function can use only by FW 1.5x to 3.5x
int sceUmdManGetDiscInfo(void* drive, u8* param, u8* buf);
int sceUmdManGetUmdDiscInfo(void* drive, u8* param, u8* buf);
int sceUmdExecReadCapacityCmd(void* drive, u8* buf);
//int sceUmdExecReadCapacityCmd(void* drive, u8* param, u8* buf);
int sceUmdExecRead10Cmd(void* drive, u8* param, u8* buf);
int sceUmdExecModSenseCmd(void* drive, u8* param, u8* buf);
int sceUmdExecReadUMDStructureCmd(void* drive, u8* param, u8* buf);
int sceUmdExecSetCDSpeedCmd(void* drive, u8* param, u8* buf);
int sceUmdExecMechaStatCmd(void* drive, u8* param, u8* buf);
int sceUmdExecGetMediaInfoCmd(void* drive, u8* param, u8* buf);
int sceUmdExecReadMKICmd(void* drive, u8* param, u8* buf);

// These function can use only by FW 3.5x
int sceUmdExecGetConfigurationCmd(void* drive, u8* param, u8* buf);
int sceUmdExecReadDiscInfoCmd(void* drive, u8* param, u8* buf);
#endif
#if 0
// Not yet
sceUmdExecTestCmd               -> 00	TEST UNIT READY ?
sceUmdExecReqSenseCmd			-> 03	REQUEST SENSE ?
sceUmdExecStartStopUnitCmd		-> 1B	START STOP UNIT ?
sceUmdExecPreventAllowMediaCmd	-> 1E	PREVENT ALLOW MEDIUM REMOVAL ?
sceUmdExecReadCapacityCmd		-> 25	READ CAPACITY(10) ?
sceUmdExecRead10Cmd				-> 28	READ(10) ?
sceUmdExecSeekCmd				-> 2B	SEEK(10) ?
sceUmdExecPrefetch10Cmd			-> 34	PRE-FETCH(10) ?
sceUmdExecGetConfigurationCmd	-> 46	GET CONFIGURATION ?
sceUmdExecGetEventStatusCmd		-> 4A	GET EVENT STATUS NOTIFICATION ?
sceUmdExecReadDiscInfoCmd		-> 51	READ DISC INFORMATION ?
sceUmdExecModSenseCmd			-> 5A	MODE SENSE(10) ?
sceUmdExecModSelectCmd			-> 55	MODE SELECT(10) ?
sceUmdExecReadUMDStructureCmd   -> AD	READ DVD STRUCTURE ?
sceUmdExecSetStreamingCmd       -> B6	SET_STREAMING ?
sceUmdExecSetCDSpeedCmd         -> BB	SET_CD_SPEED ?
sceUmdExecMechaStatCmd          -> BD	MECHANISM_STATUS ?
sceUmdExecGetMediaInfoCmd       -> UNKNOWN
sceUmdExecSetAreaLimitCmd       -> UNKNOWN
sceUmdExecGetErrorLogCmd        -> UNKNOWN
sceUmdExecAllocateFromReadCmd   -> UNKNOWN
sceUmdExecSetAccessLimitCmd     -> UNKNOWN
sceUmdExecClearCacheInfoCmd     -> UNKNOWN
sceUmdExecAdjustDataCmd         -> UNKNOWN
sceUmdExecReportCacheCmd        -> UNKNOWN
sceUmdExecSetLockLengthCmd      -> UNKNOWN
sceUmdExecReadMKICmd            -> UNKNOWN
#endif
#ifdef __cplusplus
}
#endif

