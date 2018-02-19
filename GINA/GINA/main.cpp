//MSGINA.DLL Ver1.0 API
//�Ȃɂ����Ȃ�GINA.DLL�̃T���v��

#include <windows.h>
#include <stdio.h>
#include <winwlx.h>

//MSGINA.dll�̃t�@���N�V�������Ăяo�����߂̊֐��|�C���^�\����

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

//��̊֐��|�C���^�\���̂̕ϐ�

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

//������
static BOOL MyInitialize(void){

	HINSTANCE hDll;
	//MSGINA.DLL�����[�h���܂�
	if (! (hDll = LoadLibrary(TEXT("MSGINA.DLL")))) return FALSE;

	//DLL�̊֐��Ɗ֐��|�C���^�ϐ���ڑ����܂��B

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

//���p�\�ȃo�[�W�����ł��邩��Ԃ�
BOOL WINAPI WlxNegotiate(DWORD dwWinlogonVersion,DWORD *pdwDllVersion){
    if (! MyInitialize()) return FALSE;
    return (* GWlxNegotiate)(dwWinlogonVersion, pdwDllVersion);
}

//MSGINA.DLL�̏���������
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

//�f�B�X�v���C�����b�N���ꂽ�Ƃ��ɌĂяo�����
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

//���O�I�����F�؂��ꂽ�Ƃ��ɌĂяo�����
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

//���O�I������SAS�C�x���g����������ƌĂяo�����
int WINAPI WlxLoggedOnSAS(PVOID pWlxContext,DWORD dwSasType,PVOID pReserved){
    return (* GWlxLoggedOnSAS)(pWlxContext, dwSasType, pReserved);
}

//�f�B�X�v���C�����b�N���ꂽ�Ƃ��ɌĂяo�����
VOID WINAPI WlxDisplayLockedNotice(PVOID pWlxContext){
    (* GWlxDisplayLockedNotice)(pWlxContext);
}

//���[�N�X�e�[�V���������b�N����O�ɌĂяo�����
BOOL WINAPI WlxIsLockOk(PVOID pWlxContext){
    return (* GWlxIsLockOk)(pWlxContext);
}

//���[�N�X�e�[�V���������b�N���ꂽ�Ƃ��ɌĂяo�����
int WINAPI WlxWkstaLockedSAS(PVOID pWlxContext,DWORD dwSasType){
    return (* GWlxWkstaLockedSAS)(pWlxContext, dwSasType);
}

//ExitWindowsEx�Ń��O�I�t���n�߂��Ƃ��Ăяo�����
BOOL WINAPI WlxIsLogoffOk(PVOID pWlxContext){
    BOOL bSuccess;
    bSuccess = (* GWlxIsLogoffOk)(pWlxContext);
    return bSuccess;
}

//���O�I�t�̒��O�ɌĂяo�����
VOID WINAPI WlxLogoff(PVOID pWlxContext){
    (* GWlxLogoff)(pWlxContext);
}

//�V�X�e�����V���b�g�_�E������钼�O�ɌĂяo�����
VOID WINAPI WlxShutdown(PVOID pWlxContext,DWORD ShutdownType){
    (* GWlxShutdown)(pWlxContext, ShutdownType);
}
