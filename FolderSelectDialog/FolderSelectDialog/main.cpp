
#include <windows.h>
#include <Shlobj.h>
#include <stdio.h>

#pragma comment(lib, "Ole32.lib")

char* FolderSelectDialog( HWND p_hWnd, char* p_cSetStr,
	char* p_cRootStr, char* p_cCaptionStr, UINT p_uiFlags );

int main(){
	return 0;
}

char* FolderSelectDialog( HWND p_hWnd, char* p_cSetStr,
	char* p_cRootStr, char* p_cCaptionStr, UINT p_uiFlags ){

	BOOL			bRes;
	char			chPutFolder[MAX_PATH];
	LPITEMIDLIST	pidlRetFolder;
	BROWSEINFO		stBInfo;
	char*			cRetStr;

	//�@�\���̂����������܂��B
	stBInfo.pidlRoot = NULL;	//���[�g�t�H���_�ł��B
	stBInfo.hwndOwner = p_hWnd;	//�\������_�C�A���O�̐e�E�B���h�E�̃n���h���ł��B
	stBInfo.pszDisplayName = chPutFolder;	//�I������Ă���t�H���_�����󂯂܂��B
	stBInfo.lpszTitle = p_cCaptionStr;	//�����̕�����ł��B
	stBInfo.ulFlags = p_uiFlags;	//�\���t���O�ł��B
	stBInfo.lpfn = NULL;	//�_�C�A���O�v���V�[�W���ւ̃|�C���^�ł��B
	stBInfo.lParam = 0L;	//�v���V�[�W���ɑ���p�����[�^�[�ł��B

	//�@�_�C�A���O�{�b�N�X��\�����܂��B
	pidlRetFolder = ::SHBrowseForFolder( &stBInfo );

	//�@pidlRetFolder�ɂ͂��̃t�H���_��\���A�C�e���h�c���X�g�ւ̃|�C���^��
	//�@�����Ă��܂��BchPutFolder�ɂ͑I�����ꂽ�t�H���_���i��t���p�X�j����
	//�@���������Ă��Ȃ��̂ŁA���̃|�C���^�𗘗p���܂��B

	if(pidlRetFolder){
		//�@�t���p�X���擾���܂��B
		bRes = SHGetPathFromIDList( pidlRetFolder, chPutFolder );
		if( bRes != FALSE )
			cRetStr = chPutFolder;

		CoTaskMemFree( pidlRetFolder );
	}

	return cRetStr;
}
	
