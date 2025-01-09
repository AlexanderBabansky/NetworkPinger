#include <Windows.h>
#include <strsafe.h>
#include "service_utils.h"

extern LPCSTR SVCNAME;
DWORD ServiceMain(HANDLE stopEvent);

namespace {
SERVICE_STATUS_HANDLE gSvcStatusHandle = NULL;
SERVICE_STATUS gSvcStatus{};
HANDLE ghSvcStopEvent = NULL;

VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else
        gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        gSvcStatus.dwCheckPoint = 0;
    else
        gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    // Handle the requested control code.

    switch (dwCtrl) {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

        // Signal the service to stop.

        SetEvent(ghSvcStopEvent);
        ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

        return;

    case SERVICE_CONTROL_INTERROGATE: break;

    default: break;
    }
}

VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    DWORD retCode = ServiceMain(ghSvcStopEvent);
    ReportSvcStatus(SERVICE_STOPPED, retCode, 0);
    return;
}

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);
    if (!gSvcStatusHandle) {
        service_utils::SvcReportEvent(TEXT("RegisterServiceCtrlHandler"), SVCNAME);
        return;
    }
    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
    SvcInit(dwArgc, lpszArgv);
}

} // namespace

int main(int argc, CHAR *argv[])
{
    ghSvcStopEvent = CreateEvent(NULL,  // default security attributes
                                 TRUE,  // manual reset event
                                 FALSE, // not signaled
                                 NULL); // no name

    if (ghSvcStopEvent == NULL) {
        ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
        return 1;
    }

    if (argc == 2) {
        if (lstrcmpi(argv[1], TEXT("local")) == 0) {
            ServiceMain(ghSvcStopEvent);
            return 0;
        }
    }

    std::string SVCNAME_nonconst = SVCNAME;

    SERVICE_TABLE_ENTRY DispatchTable[] = {{SVCNAME_nonconst.data(),
                                            (LPSERVICE_MAIN_FUNCTION)SvcMain},
                                           {NULL, NULL}};
    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        service_utils::SvcReportEvent(TEXT("StartServiceCtrlDispatcher"), SVCNAME);
    }
    return 0;
}
