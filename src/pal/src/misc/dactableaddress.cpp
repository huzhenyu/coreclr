//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information. 
//

/*++



Module Name:

    dactableaddress.cpp

Abstract:

    Functions to write and read DAC global pointer table address between the coreclr
    and DAC/debugger processes.

NOTE:

   These functions are temporary until a better way to plumb the DAC table
   address from the debuggee to debugger processes is implemented.

Revision History:


--*/

#include "pal/palinternal.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

/*++
Function
    PAL_PublishDacTableAddress

Parameters

    baseAddress
       [in] base address of CLR module
    tableAddress
       [in] address of dac table
    tableSize
       [in] size of dac table

Return Values
   pal errors

--*/
PALIMPORT
DWORD
PALAPI
PAL_PublishDacTableAddress(
    IN PVOID baseAddress,
    IN PVOID tableAddress,
    IN ULONG tableSize)
{
    DWORD ret = NO_ERROR;

    char fileName[100]; 
    snprintf(fileName, sizeof(fileName), "/tmp/%p_dacTable", baseAddress);

    FILE *file = fopen(fileName, "w+");
    if (file != nullptr) 
    {
        char dacTableInfo[100]; 
        snprintf(dacTableInfo, sizeof(dacTableInfo), "%p %d\n", tableAddress, tableSize);

        if (fputs(dacTableInfo, file) < 0)
        {
            ret = ERROR_INVALID_DATA;
        }

        fclose(file);
    }
    else
    {
        ret = ERROR_FILE_NOT_FOUND;
    }

    return ret;
}


/*++
Function
    PAL_GetDacTableAddress

Parameters

    baseAddress
       [in] base address of CLR module
    tableAddress
       [out] pointer to put DAC table address
    tableSize
       [out] pointer to put DAC table size

Return Values
   pal errors

--*/
PALIMPORT
DWORD
PALAPI
PAL_GetDacTableAddress(
    IN PVOID baseAddress,
    OUT PVOID *tableAddress,
    OUT PULONG tableSize)
{
    DWORD ret = NO_ERROR;

    char fileName[100]; 
    snprintf(fileName, sizeof(fileName), "/tmp/%p_dacTable", baseAddress);

    FILE *file = fopen(fileName, "r");
    if (file != nullptr) 
    {
        char data[100]; 
        if (fgets(data, sizeof(data), file) != nullptr)
        {
            if (sscanf(data, "%p %d\n", tableAddress, tableSize) != 2)
            {
                ret = ERROR_INVALID_DATA;
            }
        }
        else
        {
            ret = ERROR_INVALID_DATA;
        }

        fclose(file);
    }
    else
    {
        ret = ERROR_FILE_NOT_FOUND;
    }
    return ret;
}

/*++
Function
    PAL_CleanupDacTableAddress

Parameters
    None

Return Values
   None

--*/
PALIMPORT
VOID
PALAPI
PAL_CleanupDacTableAddress(
    IN PVOID baseAddress)
{
    char fileName[100]; 
    snprintf(fileName, sizeof(fileName), "/tmp/%p_dacTable", baseAddress);
    remove(fileName);
}
