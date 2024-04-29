/**
 * PROJECT:         ExectOS
 * COPYRIGHT:       See COPYING.md in the top level directory
 * FILE:            xtldr/modules/dummy/dummy.c
 * DESCRIPTION:     XTLDR Dummy Module
 * DEVELOPERS:      Rafal Kupiec <belliash@codingworkshop.eu.org>
 */

#include <dummy.h>


/* Dummy module information */
MODULE_AUTHOR(L"Rafal Kupiec <belliash@codingworkshop.eu.org>");
MODULE_DESCRIPTION(L"XTLDR Dummy Module");
MODULE_LICENSE(L"GPLv3");
MODULE_VERSION(L"0.1");

/**
 * Stub boot routine.
 *
 * @param Parameters
 *        Supplies all parameters associated with the chosen boot menu entry.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
DmBootSystem(IN PXTBL_BOOT_PARAMETERS Parameters)
{
    return STATUS_EFI_SUCCESS;
}

/**
 * This routine is the entry point of the XT EFI boot loader module.
 *
 * @param ImageHandle
 *        Firmware-allocated handle that identifies the image.
 *
 * @param SystemTable
 *        Provides the EFI system table.
 *
 * @return This routine returns a status code.
 *
 * @since XT 1.0
 */
XTCDECL
EFI_STATUS
XtLdrModuleMain(IN EFI_HANDLE ImageHandle,
                IN PEFI_SYSTEM_TABLE SystemTable)
{
    EFI_GUID DummyGuid = XT_XTOS_BOOT_PROTOCOL_GUID;
    EFI_STATUS Status;

    /* Open the XTLDR protocol */
    Status = BlGetXtLdrProtocol(SystemTable, ImageHandle, &XtLdrProtocol);
    if(Status != STATUS_EFI_SUCCESS)
    {
        /* Failed to open the protocol, return error */
        return STATUS_EFI_PROTOCOL_ERROR;
    }

    /* Set boot protocol routines */
    BlpDummyProtocol.BootSystem = DmBootSystem;

    /* Register XTOS boot protocol */
    XtLdrProtocol->Boot.RegisterProtocol(L"DUMMYOS", &DummyGuid);

    /* Register DUMMY protocol as XTOS boot protocol */
    return XtLdrProtocol->Protocol.Install(&BlpDummyProtocol, &DummyGuid);
}
