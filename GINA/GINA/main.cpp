//MSGINA.DLL Ver1.0 API
//なにもしないGINA.DLLのサンプル

#include <windows.h>
#include <stdio.h>
#include <winwlx.h>

//MSGINA.dllのファンクションを呼び出すための関数ポインタ構造体

typedef BOOL	(CALLBACK * PGWLX_Negotiate)(DWORD, DWORD *);
typedef BOOL	(CALLBACK * PGWLX_Initialize)(LPWSTR, HANDLE, PVOID, PVOID, PVOID);
typedef VOID	(CALLBACK * PGWLX_DisplaySASNotice)(PVOID);
typedef int		(CALLBACK * PGWLX_LoggedOutSAS)(PVOID, DWORD, PLUID, PSID, PDWORD, PHANDLE,PWLX_MPR_NOTIFY_INFO, PVOID *);
typedef BOOL	(CALLBACK * PGWLX_ActivateUserShell)(PVOID, PWSTR, PWSTR, PVOID);
typedef int		(CALLBACK * PGWLX_LoggedOnSAS)(PVOID, DWORD, PVOID);
typedef VOID	(CALLBACK * PGWLX_DisplayLockedNotice)(PVOID);
typedef int		(CALLBACK * PGWLX_WkstaLockedSAS)(PVOID, DWORD);
typedef BOOL	(CALLBACK * PGWLX_IsLockOk)(PVOID);
typedef BOOL	(CALLBACK * PGWLX_IsLogoffOk)(PVOID);
typedef VOID	(CALLBACK * PGWLX_Logoff)(PVOID);
typedef VOID	(CALLBACK * PGWLX_Shutdown)(PVOID, DWORD);

//上の関数ポインタ構造体の変数

PGWLX_Negotiate GWlxNegotiate;
PGWLX_Initialize GWlxInitialize;
PGWLX_DisplaySASNotice GWlxDisplaySASNotice;
PGWLX_LoggedOutSAS GWlxLoggedOutSAS;
PGWLX_ActivateUserShell GWlxActivateUserShell;
PGWLX_LoggedOnSAS GWlxLoggedOnSAS;
PGWLX_DisplayLockedNotice GWlxDisplayLockedNotice;
PGWLX_WkstaLockedSAS GWlxWkstaLockedSAS;
PGWLX_IsLockOk GWlxIsLockOk;
PGWLX_IsLogoffOk GWlxIsLogoffOk;
PGWLX_Logoff GWlxLogoff;
PGWLX_Shutdown GWlxShutdown;

//初期化
static BOOL MyInitialize(void){

	HINSTANCE hDll;
	//MSGINA.DLLをロードします
	if (! (hDll = LoadLibrary(TEXT("MSGINA.DLL")))) return FALSE;

	//DLLの関数と関数ポインタ変数を接続します。

	GWlxNegotiate = (PGWLX_Negotiate)GetProcAddress(hDll, "WlxNegotiate");
	if (! GWlxNegotiate) return FALSE;

	GWlxInitialize = (PGWLX_Initialize)GetProcAddress(hDll, "WlxInitialize");
	if (! GWlxInitialize) return FALSE;

	GWlxDisplaySASNotice = (PGWLX_DisplaySASNotice)GetProcAddress(hDll, "WlxDisplaySASNotice");
	if (! GWlxDisplaySASNotice) return FALSE;

	GWlxLoggedOutSAS = (PGWLX_LoggedOutSAS)GetProcAddress(hDll, "WlxLoggedOutSAS");
	if (! GWlxLoggedOutSAS) return FALSE;

	GWlxActivateUserShell = (PGWLX_ActivateUserShell)GetProcAddress(hDll, "WlxActivateUserShell");
	if (! GWlxActivateUserShell) return FALSE;

	GWlxLoggedOnSAS = (PGWLX_LoggedOnSAS)GetProcAddress(hDll, "WlxLoggedOnSAS");
	if (! GWlxLoggedOnSAS) return FALSE;

	GWlxDisplayLockedNotice = (PGWLX_DisplayLockedNotice)GetProcAddress(hDll, "WlxDisplayLockedNotice");
	if (! GWlxDisplayLockedNotice) return FALSE;

	GWlxIsLockOk = (PGWLX_IsLockOk)GetProcAddress(hDll, "WlxIsLockOk");
	if (! GWlxIsLockOk) return FALSE;

	GWlxWkstaLockedSAS = (PGWLX_WkstaLockedSAS)GetProcAddress(hDll, "WlxWkstaLockedSAS");
	if (! GWlxWkstaLockedSAS) return FALSE;

	GWlxIsLogoffOk = (PGWLX_IsLogoffOk)GetProcAddress(hDll, "WlxIsLogoffOk");
	if (! GWlxIsLogoffOk) return FALSE;

	GWlxLogoff = (PGWLX_Logoff)GetProcAddress(hDll, "WlxLogoff");
	if (! GWlxLogoff) return FALSE;

	GWlxShutdown = (PGWLX_Shutdown)GetProcAddress(hDll, "WlxShutdown");
	if (! GWlxShutdown) return FALSE;

	return TRUE;
}

//利用可能なバージョンであるかを返す
BOOL WINAPI WlxNegotiate(DWORD dwWinlogonVersion,DWORD *pdwDllVersion){
    if (! MyInitialize()) return FALSE;
    return (* GWlxNegotiate)(dwWinlogonVersion, pdwDllVersion);
}

//MSGINA.DLLの初期化処理
BOOL WINAPI WlxInitialize(
	LPWSTR lpWinsta,
	HANDLE hWlx,
	PVOID pvReserved,
	PVOID pWinlogonFunctions,
	PVOID *pWlxContext)
{
    return (* GWlxInitialize)(
	lpWinsta,
	hWlx,
	pvReserved,
	pWinlogonFunctions,
	pWlxContext
	);
}

//ディスプレイがロックされたときに呼び出される
VOID WINAPI WlxDisplaySASNotice(PVOID pWlxContext)
{
    (* GWlxDisplaySASNotice)(pWlxContext);
}

int WINAPI WlxLoggedOutSAS(
	PVOID pWlxContext,
	DWORD dwSasType,
	PLUID pAuthenticationId,
	PSID pLogonSid,
	PDWORD pdwOptions,
	PHANDLE phToken,
	PWLX_MPR_NOTIFY_INFO pMprNotifyInfo,
	PVOID *pProfile)
{
    int iRet;
    iRet = (* GWlxLoggedOutSAS)(
	pWlxContext,
	dwSasType,
	pAuthenticationId,
	pLogonSid,
	pdwOptions,
	phToken,
	pMprNotifyInfo,
	pProfile
	);
    return iRet;
}

//ログオンが認証されたときに呼び出される
BOOL WINAPI WlxActivateUserShell(
	PVOID pWlxContext,
	PWSTR pszDesktopName,
	PWSTR pszMprLogonScript,
	PVOID pEnvironment)
{
    return (* GWlxActivateUserShell)(
	pWlxContext,
	pszDesktopName,
	pszMprLogonScript,
	pEnvironment
	);
}

//ログオン中にSASイベントが発生すると呼び出される
int WINAPI WlxLoggedOnSAS(PVOID pWlxContext,DWORD dwSasType,PVOID pReserved){
    return (* GWlxLoggedOnSAS)(pWlxContext, dwSasType, pReserved);
}

//ディスプレイがロックされたときに呼び出される
VOID WINAPI WlxDisplayLockedNotice(PVOID pWlxContext){
    (* GWlxDisplayLockedNotice)(pWlxContext);
}

//ワークステーションをロックする前に呼び出される
BOOL WINAPI WlxIsLockOk(PVOID pWlxContext){
    return (* GWlxIsLockOk)(pWlxContext);
}

//ワークステーションがロックされたときに呼び出される
int WINAPI WlxWkstaLockedSAS(PVOID pWlxContext,DWORD dwSasType){
    return (* GWlxWkstaLockedSAS)(pWlxContext, dwSasType);
}

//ExitWindowsExでログオフを始めたとき呼び出される
BOOL WINAPI WlxIsLogoffOk(PVOID pWlxContext){
    BOOL bSuccess;
    bSuccess = (* GWlxIsLogoffOk)(pWlxContext);
    return bSuccess;
}

//ログオフの直前に呼び出される
VOID WINAPI WlxLogoff(PVOID pWlxContext){
    (* GWlxLogoff)(pWlxContext);
}

//システムがシャットダウンされる直前に呼び出される
VOID WINAPI WlxShutdown(PVOID pWlxContext,DWORD ShutdownType){
    (* GWlxShutdown)(pWlxContext, ShutdownType);
}
