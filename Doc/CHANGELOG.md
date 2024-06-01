# Change Log
## v1.0 (2018-07-17)
- Initial release

## v1.1 (2018-07-30)
- added: Output PARAM.SFO into _disc.txt

## v1.2 (2018-09-09)
- added: disc size to log
- added: checking free space of MemoryStick

## v1.3 (2021-02-03)
- changed: use the latest src of DiscImageCreator
- changed: prx doesn't use checking the memory stick
- changed: prx exits automatically when completes dumping the log or iso.
- fixed: app hangs if there is not disc0:/PSP_GAME/SYSDIR/UPDATE/PARAM.SFO

## v1.4 (2022-02-22)
- changed: Replaced ':' to '_' (Disc ID)

## v1.5 (2022-04-01)
- changed: refactoring
- added: output inquiry data to _drive.txt
- added: pspumdman.prx (this module is used only for the purpose of some tests)
- added: document (IoctlCommandList.txt, PrxAnalysis.txt)

## v1.6 (2023-12-01)
- added: support to dump PFI.bin
- changed: Visual Studio 2019 to 2022

## v1.7 (2024-06-01)
- added: creating iso directory automatically
- changed: PATH_TABLE_RECORD_SIZE
- fixed: the memory stick size is overflowed
