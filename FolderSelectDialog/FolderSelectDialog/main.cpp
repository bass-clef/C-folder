
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

	//　構造体を初期化します。
	stBInfo.pidlRoot = NULL;	//ルートフォルダです。
	stBInfo.hwndOwner = p_hWnd;	//表示するダイアログの親ウィンドウのハンドルです。
	stBInfo.pszDisplayName = chPutFolder;	//選択されているフォルダ名を受けます。
	stBInfo.lpszTitle = p_cCaptionStr;	//説明の文字列です。
	stBInfo.ulFlags = p_uiFlags;	//表示フラグです。
	stBInfo.lpfn = NULL;	//ダイアログプロシージャへのポインタです。
	stBInfo.lParam = 0L;	//プロシージャに送るパラメーターです。

	//　ダイアログボックスを表示します。
	pidlRetFolder = ::SHBrowseForFolder( &stBInfo );

	//　pidlRetFolderにはそのフォルダを表すアイテムＩＤリストへのポインタが
	//　入っています。chPutFolderには選択されたフォルダ名（非フルパス）だけ
	//　しか入っていないので、このポインタを利用します。

	if(pidlRetFolder){
		//　フルパスを取得します。
		bRes = SHGetPathFromIDList( pidlRetFolder, chPutFolder );
		if( bRes != FALSE )
			cRetStr = chPutFolder;

		CoTaskMemFree( pidlRetFolder );
	}

	return cRetStr;
}
	
