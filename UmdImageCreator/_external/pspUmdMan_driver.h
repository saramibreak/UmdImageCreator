#ifdef __cplusplus
extern "C" {
#endif

// These function can be used by All FW
void* _sceUmdManGetUmdDrive(int driveNum);
int _sceUmdManGetInquiry(void* drive, unsigned char* param, unsigned char* buf);
int _sceUmdExecInquiryCmd(void* drive, unsigned char* param, unsigned char* buf);

// These function can be used only by FW 1.5x to 3.5x
int _sceUmdManActivate(int unit, const char* drive);
int _sceUmdManCheckDeviceReady(void* drive, const char* dev);
int _sceUmdManGetDiscInfo(void* buf);
void* _sceUmdManGetUmdDiscInfo(int driveNum);

#ifdef __cplusplus
}
#endif
