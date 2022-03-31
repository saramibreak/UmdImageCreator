	.set noreorder

#include "pspstub.s"

#	STUB_START	"sceUmdMan_driver",0x00010011,0x00450005
	STUB_START	"sceUmdMan_driver",0x40090000,0x00030005
	STUB_FUNC	0x47E2B6D8,sceUmdManGetUmdDrive
#	STUB_FUNC	0xB2368381,sceUmdManCheckDeviceReady
	STUB_FUNC	0xE779ECEF,sceUmdManGetInquiry
#	STUB_FUNC	0x1B1BF9FD,sceUmdExecRead10Cmd
#	STUB_FUNC	0x18DE1880,sceUmdExecPrefetch10Cmd
#	STUB_FUNC	0x3D44BABF,sceUmdExecReadUMDStructureCmd
#	STUB_FUNC	0xE3F448E0,sceUmdExecStartStopUnitCmd
	STUB_FUNC	0x1B19A313,sceUmdExecInquiryCmd
#	STUB_FUNC	0x2CBE959B,sceUmdExecReqSenseCmd
#	STUB_FUNC	0x2A39569B,sceUmdExecModSenseCmd
#	STUB_FUNC	0xCEE55E3E,sceUmdExecModSelectCmd
#	STUB_FUNC	0xE5B7EDC5,sceUmdExecMechaStatCmd
#	STUB_FUNC	0x5AA96415,sceUmdExecReadCapacityCmd
#	STUB_FUNC	0x250E6975,sceUmdExecSeekCmd
#	STUB_FUNC	0xF819E17C,sceUmdExecReadMKICmd
#	STUB_FUNC	0x108B2322,sceUmdExecGetMediaInfoCmd
#	STUB_FUNC	0xBF88476F,sceUmdExecSetCDSpeedCmd
#	STUB_FUNC	0x485D4925,sceUmdExecSetStreamingCmd
#	STUB_FUNC	0x8609D1E4,sceUmdManGetDiscInfo
#	STUB_FUNC	0xAAA4ED91,sceUmdManGetDiscInfo4VSH
#	STUB_FUNC	0xE192C10A,sceUmdManGetUmdDiscInfo
	STUB_END
