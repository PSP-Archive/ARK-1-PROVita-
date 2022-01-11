
// sceReboot.prx
#define REBOOT_MAIN 0x0
#define REBOOT_ICACHE_FLUSH_ALL 0x18AC
#define REBOOT_DCACHE_FLUSH_ALL 0x1E40
#define REBOOT_FILESYSTEM_REPLACE_PATCH_1 0x206C
#define REBOOT_FILESYSTEM_REPLACE_PATCH_2 0x2078
#define REBOOT_PSP_HEADER_CHECK_PATCH 0x2DB0
#define REBOOT_MODULE_BUFFER_SIZE_PATCH 0x2DB4
#define REBOOT_LOADCORE_MOVE_ADDRESS_INTO_ARG3 0x2424
#define REBOOT_LOADCORE_MODULE_START_JUMP 0x242C
#define REBOOT_LFAT_OPEN 0x20BC
#define REBOOT_LFAT_OPEN_CALL 0x22FC

// sysmem.prx
#define SYSMEM_LIBC_TIME 0xF718
#define SYSMEM_LIBC_TIME_JALR_NOP 0xF768
#define SYSMEM_SYSMEMUSERFORUSER_91DE343C_NOP 0x9CE8
#define SYSMEM_ICACHE_INVALIDATE_ALL 0xE98
#define SYSMEM_DCACHE_WRITEBACK_INVALIDATE_ALL 0x744
#define SYSMEM_DCACHE_WRITEBACK_INVALIDATE_RANGE 0x930
#define SYSMEM_GZIP_DECOMPRESS 0xF804
#define SYSMEM_BRANCH_CHECK_1 0x98F0
#define SYSMEM_BRANCH_CHECK_2 0x9A10
#define SYSMEM_BRANCH_CHECK_3 0x9AA8
#define SYSMEM_BRANCH_CHECK_4 0x9B58
#define SYSMEM_BRANCH_CHECK_5 0x9C2C
#define SYSMEM_BRANCH_CHECK_6 0x9CD0
#define SYSMEM_BRANCH_CHECK_7 0x9D74
#define SYSMEM_BRANCH_CHECK_8 0x9E0C
#define SYSMEM_BRANCH_CHECK_9 0x9EBC
#define SYSMEM_BRANCH_CHECK_10 0x9F6C
#define SYSMEM_DEVKIT_HIGH_16BIT 0x118A0
#define SYSMEM_DEVKIT_LOW_16BIT (SYSMEM_DEVKIT_HIGH_16BIT + 8)

// libhttp.prx
#define HTTP_LOAD_CERT_FROM_FLASH 0x170D0

// np_matching2.prx
#define NP2_GET_OPEN_PSID 0x2CF88
#define NP2_SIGNATURE_STRING 0x2D25C

// pspnet_adhoc_matching.prx
#define WLAN_GET_ETHER_ADDR_STUB 0x46D4

// init.prx
#define INIT_MODULE_BOOTSTART 0x1A54
#define INIT_START_MODULE_STUB 0x1C5C

// loadcore.prx
#define LOADCORE_TEXT_RELATIVE 0x16F00
#define LOADCORE_TEXT (KERNEL_BASE + LOADCORE_TEXT_RELATIVE)
#define LOADCORE_MODULE_BOOTSTART 0xAF8
#define LOADCORE_FIND_MODULE_BY_NAME 0x6DE4
#define LOADCORE_MEMLMD_DECRYPT 0x766C
#define LOADCORE_MEMLMD_DECRYPT_CALL_1 0x3DE4
#define LOADCORE_MEMLMD_DECRYPT_CALL_2 0x58AC
#define LOADCORE_CHECK_EXEC_REFERENCE 0x798C
#define LOADCORE_CHECK_EXEC_CALL_1 0x11F0
#define LOADCORE_CHECK_EXEC_CALL_2 0x1240
#define LOADCORE_CHECK_EXEC_CALL_3 0x4828
#define LOADCORE_PROBE_EXEC_1 0x62B4
#define LOADCORE_PROBE_EXEC_2 0x620C
#define LOADCORE_RELOCATION_HANDLER_0 0x7DBC
#define LOADCORE_RELOCATION_HANDLER_7 0x7DD8
#define LOADCORE_PROBE_EXEC_1_CALL 0x4424
#define LOADCORE_PROBE_EXEC_2_CALL_1 0x4624
#define LOADCORE_PROBE_EXEC_2_CALL_2 0x6520
#define LOADCORE_SYSCALL_CHECK 0x3CE4
#define LOADCORE_PRX_TYPE_CHECK 0x71F0
#define LOADCORE_POPS_CHECK 0x69F0
#define LOADCORE_BOOTSTART_START_CALL 0x199C
#define LOADCORE_MOVE_START_FUNCTION_INTO_ARG1 0x19A0
#define LOADCORE_NID_FILLER 0xEA8
#define LOADCORE_NID_FILLER_CALL 0x39B8
#define LOADCORE_NID_SYSCALL15_PART_1 0x3B20
#define LOADCORE_NID_SYSCALL15_PART_2 0x3B28

// loadexec_01g.prx
#define LOADEXEC_LOAD_EXEC_FOR_USER 0xECC
#define LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE 0x23D0
#define LOADEXEC_LOAD_REBOOT 0x0
#define LOADEXEC_LOAD_REBOOT_CALL 0x2DE0
#define LOADEXEC_REBOOT_BUFFER_ADDRESS_SETTER 0x2E2C
#define LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_K1_CHECK_1 0x241E
#define LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_K1_CHECK_2 0x16A6
#define LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_USER_LEVEL_CHECK_1 0x2460
#define LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_USER_LEVEL_CHECK_2 0x16D8
#define LOADEXEC_LOAD_EXEC_VSH_WITH_APITYPE_USER_LEVEL_CHECK_3 0x2668
#define LOADEXEC_LOAD_EXEC_FOR_USER_362A956B_FIX 0x9B4

// iofilemgr.prx
#define IOFILEMGR_IO_OPEN 0x3F88
#define IOFILEMGR_IO_READ 0x4098
#define IOFILEMGR_IO_WRITE 0x40D0
#define IOFILEMGR_IO_CLOSE 0x3F48
#define IOFILEMGR_IO_DOPEN 0x13FC
#define IOFILEMGR_IO_DREAD 0x1570
#define IOFILEMGR_IO_DCLOSE 0x1620
#define IOFILEMGR_IO_RENAME 0x17E4
#define IOFILEMGR_IO_REMOVE 0x16C4
#define IOFILEMGR_IO_ADD_DRIVER_REFERENCE 0x6314
#define IOFILEMGR_IO_FIND_DRIVER 0x2A04
#define IOFILEMGR_IO_GET_STAT 0x4234

// modulemgr.prx
#define MODULEMGR_PARTITION_CHECK 0x811C
#define MODULEMGR_PROLOGUE_MODULE 0x8270
#define MODULEMGR_INIT_APITYPE_FIELD 0x9C1C
#define MODULEMGR_INIT_FILENAME_FIELD 0x9C20
#define MODULEMGR_INIT_APPLICATION_TYPE_FIELD 0x9C78
#define MODULEMGR_DEVICE_CHECK_1 0x790
#define MODULEMGR_DEVICE_CHECK_2 0x7EC
#define MODULEMGR_DEVICE_CHECK_3 0x31C0
#define MODULEMGR_DEVICE_CHECK_4 0x321C
#define MODULEMGR_DEVICE_CHECK_5 0x324A
#define MODULEMGR_DEVICE_CHECK_6 0x3554
#define MODULEMGR_DEVICE_CHECK_7 0x35AC
#define MODULEMGR_DEVICE_CHECK_8 0x35DA
#define MODULEMGR_CHECK_EXEC_STUB 0x8A48
#define MODULEMGR_PARTITION_CHECK_CALL_1 0x662C
#define MODULEMGR_PARTITION_CHECK_CALL_2 0x69A8
#define MODULEMGR_PROLOGUE_MODULE_CALL 0x7158
#define MODULEMGR_PROBE_EXEC_3 0x000089E8
#define MODULEMGR_PROBE_EXEC_3_CALL 0x00007DA8

// interruptman.prx
#define INTERRUPTMAN_BREAKING_LOOP_OF_DEATH 0xE98
#define INTERRUPTMAN_HARDWARE_REGISTER_WRITE_BC000000 0xDEC
#define INTERRUPTMAN_HARDWARE_REGISTER_WRITE_BC000004 0xDF0

// mesg_led_01g.prx
#define MESGLED_DECRYPT 0xE0
#define MESGLED_DECRYPT_CALL_1 0x3F1C
#define MESGLED_DECRYPT_CALL_2 0x5454
#define MESGLED_DECRYPT_CALL_3 0x5788
#define MESGLED_DECRYPT_CALL_4 0x56F8
#define MESGLED_DECRYPT_CALL_5 0x1F68
#define MESGLED_DECRYPT_CALL_6 0x237C

// memlmd_01g.prx
#define MEMLMD_DECRYPT 0x20C
#define MEMLMD_DECRYPT_CALL_1 0xF1C
#define MEMLMD_DECRYPT_CALL_2 0xF80

// mediasync.prx
// 6.60 mediasync offsets
#define MEDIASYNC_MS_CHECK_MEDIA 0x744
#define MEDIASYNC_DISC_MEDIA_CHECK_1 0x3C4
#define MEDIASYNC_DISC_MEDIA_CHECK_2 0xDC8
#define MEDIASYNC_MS_SYSTEM_FILE_CHECK 0x10B4
#define MEDIASYNC_DISC_ID_CHECK_1 0xFC0
#define MEDIASYNC_DISC_ID_CHECK_2 0xFDC
/*
//2.10 mediasync offsets
#define MEDIASYNC_MS_CHECK_MEDIA 0x6A8
#define MEDIASYNC_DISC_MEDIA_CHECK_1 0x328
#define MEDIASYNC_DISC_MEDIA_CHECK_2 0xD2C
#define MEDIASYNC_MS_SYSTEM_FILE_CHECK 0x1018
#define MEDIASYNC_DISC_ID_CHECK_1 0xF24
#define MEDIASYNC_DISC_ID_CHECK_2 0xF40
*/

// threadman.prx
#define THREADMAN_USERLEVEL_STRUCTURE 0x19F40
#define THREADMAN_CREATE_THREAD_REFERENCE 0x19264
#define THREADMAN_START_THREAD_REFERENCE 0x19408
#define THREADMAN_CREATE_THREAD 0x12CF8
#define THREADMAN_START_THREAD 0x1351C
#define THREADMAN_DELAY_THREAD 0x3D78

// np9660.prx
#define NP9660_INIT_FOR_KERNEL_CALL 0x3C40
#define NP9660_INIT_ISOFS_CALL 0x3C5C
#define NP9660_READ_DISC_SECTOR_CALL_1 0x43F8
#define NP9660_READ_DISC_SECTOR_CALL_2 0x59D4
#define NP9660_IO_CLOSE_STUB 0x7DD0
#define NP9660_INIT 0x368C
// These need code upgrade to point to BSS properly, right now they are relative to Text
// See 0x00003394
// NP9660_ISO_FD at the start of BSS
#define NP9660_DATA_1 (0x5BB4 - 0x5BA4 + 0x148 + 0x8A00)
#define NP9660_DATA_2 (0x5BBC - 0x5BA4 + 0x148 + 0x8A00)
#define NP9660_DATA_3 (0x5BD0 - 0x5BA4 + 0x148 + 0x8A00)
#define NP9660_DATA_4 (0x5BD8 - 0x5BA4 + 0x148 + 0x8A00)
#define NP9660_DATA_5 (0xD4 + 0x8A00)
#define NP9660_ISO_FD (0x148 + 0x8A00)
#define NP9660_READ_DISC_SECTOR 0x5054
#define NP9660_READ_SECTOR_FLUSH 0x50C4

// isofs.prx
#define ISOFS_WAIT_SEMA_CALL     0x00003FEC
#define ISOFS_SIGNAL_SEMA_CALL_1 0x00004024
#define ISOFS_SIGNAL_SEMA_CALL_2 0x000040D8
#define ISOFS_SIGNAL_SEMA_CALL_3 0x000042B4

// openpsid.prx
#define OPENPSID_GETOPENPSID 0x2B0

// kermit_ctrl.prx
#define CTRL_SET_SAMPLING_CYCLE 0xA6C
#define CTRL_SET_SAMPLING_MODE 0x9C8
#define CTRL_READ_BUFFER_POSITIVE 0xB90
#define CTRL_PEEK_BUFFER_POSITIVE 0x1328

// pspnet_apctl.prx
#define WLANDRV_LIB_B5E7B187 0x00010970

// pspnet_inet.prx
#define WLANDRV_LIB_51B0BBB8 0x00030FE4

// kermit.prx
#define KERMIT_4F75AA05 0x0000036C

// kermit_peripheral.prx
#define KERMIT_PERIPHERAL_CMD_SEND 0x000007CC

