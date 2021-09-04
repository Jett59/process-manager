#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <string.h>
#include <stdlib.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

typedef struct {
    DWORD pid;
    PROCESS_MEMORY_COUNTERS memInfo;
    TCHAR name [MAX_PATH];
} processInfo;

processInfo getProcessInfo( DWORD processID )
{
    processInfo result;
    memset(&result, 0, sizeof(processInfo));
    result.pid = processID;

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, result.name, 
                               sizeof(result.name)/sizeof(TCHAR) );
        }
        GetProcessMemoryInfo(hProcess, &result.memInfo, sizeof(result.memInfo));
    }
    CloseHandle( hProcess );
    return result;
}

int main( void )
{
    // Get the list of process identifiers.

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 1;
    }


    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            getProcessInfo( aProcesses[i] );
        }
    }

    return 0;
}
