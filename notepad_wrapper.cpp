#ifndef UNICODE
#define UNICODE
#endif
#include<windows.h>

static LPTSTR search_open_file_path( LPTSTR cmdline )
{
	//
	// "notepad.exe"をskipする。
	//
	for( ; cmdline[0] != L'\0'; cmdline++ ) {
		if( ((cmdline[ 0] == L'N') || (cmdline[ 0] == L'n'))
		 && ((cmdline[ 1] == L'O') || (cmdline[ 1] == L'o'))
		 && ((cmdline[ 2] == L'T') || (cmdline[ 2] == L't'))
		 && ((cmdline[ 3] == L'E') || (cmdline[ 3] == L'e'))
		 && ((cmdline[ 4] == L'P') || (cmdline[ 4] == L'p'))
		 && ((cmdline[ 5] == L'A') || (cmdline[ 5] == L'a'))
		 && ((cmdline[ 6] == L'D') || (cmdline[ 6] == L'd'))
		 &&  (cmdline[ 7] == L'.')
		 && ((cmdline[ 8] == L'E') || (cmdline[ 8] == L'e'))
		 && ((cmdline[ 9] == L'X') || (cmdline[ 9] == L'x'))
		 && ((cmdline[10] == L'E') || (cmdline[10] == L'e'))
		 ) {
			cmdline += 11;	// notepad.exe
			break;
		}
	}

	if( cmdline[0] == L'\0' ) {
		return NULL;
	}


	//
	// " "までskipする。
	//
	for( ; cmdline[0] != L'\0'; cmdline++ ) {
		if( cmdline[0] == L' ' ) {
			cmdline++;
			break;
		}
	}

	if( cmdline[0] == L'\0' ) {
		return NULL;
	}


	for( ; cmdline[0] != L'\0'; cmdline++ ) {
		if( cmdline[0] != L' ' ) {
			break;
		}
	}

	if( cmdline[0] == L'\0' ) {
		return NULL;
	}

#if 0
	MessageBox( NULL, cmdline, TEXT("dump"), MB_OK );
#endif

	return cmdline;
}

static LPTSTR create_open_file_path_from_argv( LPTSTR cmdline, int argc, LPTSTR argv[] )
{
	int		i;
	LPTSTR	open_file_path;

	open_file_path = (LPTSTR)malloc( (lstrlen(cmdline) +2) * sizeof(TCHAR) );

	open_file_path[0] = L'\0';
	lstrcpy( open_file_path, argv[2] );
	for( i = 3; i < argc; i++ ) {
		lstrcat( open_file_path, TEXT(" ") );
		lstrcat( open_file_path, argv[i] );
	}

	return open_file_path;
}

/*ARGSUSED*/
    int WINAPI
WinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst,
    LPSTR	lpszCmdLine,
    int		nCmdShow)
{
	int		i;
	// デフォルトのエディタパス。notepad.exeを指定するとnotepad_wrapper.exeが再起で呼ばれてしまうため別なコマンドにする。
	TCHAR	editor_exepath_default[] = TEXT("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe");
	TCHAR	editor_cmdopt_default[] = TEXT("");
	TCHAR	editor_print_cmdopt_default[] = TEXT("");
	TCHAR	editor_exepath[MAX_PATH] = TEXT("");
	TCHAR	editor_exename[MAX_PATH] = TEXT("");
	TCHAR	editor_cmdopt[MAX_PATH] = TEXT("");
	TCHAR	editor_print_cmdopt[MAX_PATH] = TEXT("");
	LPTSTR	exe_path;
	LPTSTR	ini_path;
	LPTSTR	cmdline;
	int		argc = 0;
	LPTSTR*	argv;
	LPTSTR	next_arg;
	LPTSTR	open_file_path = NULL;
	bool	print_mode = false;

	//
	// コマンドラインを取得し、argvを得る
	//
	cmdline = GetCommandLine();
	argv = CommandLineToArgvW( cmdline, &argc );
	open_file_path = search_open_file_path( cmdline );
	if( open_file_path == NULL ) {
		open_file_path = create_open_file_path_from_argv( cmdline, argc, argv );
	}
	if( open_file_path == NULL ) {
		open_file_path = TEXT("");	// Failsafe
	}


	//
	// "/p"が指定されていた場合、プリンタモードにする
	//
	if( (open_file_path[0] == L'/')
	 && (open_file_path[1] == L'p')
	 && (open_file_path[2] == L' ')
	 ) {
		open_file_path += 3;
		print_mode = true;
	}

	//
	// このコマンドのiniファイルパスを得る
	//
	exe_path = argv[0];
	ini_path = (LPTSTR)malloc( (lstrlen(exe_path) + 1) * sizeof(TCHAR) );
	if( lstrcmp( &exe_path[lstrlen(exe_path) -4], TEXT(".exe") ) == 0 ) {
		lstrcpy( ini_path, exe_path );
		lstrcpy( &ini_path[lstrlen(exe_path) -4], TEXT(".ini") );
	}

	//
	// iniファイルから次のeditorのコマンドパスと引数を得る
	//
	GetPrivateProfileString(
			TEXT("default"),
			TEXT("exe_path"),
			editor_exepath_default,
			editor_exepath,
			sizeof(editor_exepath) / sizeof(TCHAR),
			ini_path);
	// open用コマンドラインオプション
	GetPrivateProfileString(
			TEXT("default"),
			TEXT("exe_options"),
			editor_cmdopt_default,
			editor_cmdopt,
			sizeof(editor_cmdopt) / sizeof(TCHAR),
			ini_path);
	// print用コマンドラインオプション
	GetPrivateProfileString(
			TEXT("default"),
			TEXT("exe_print_options"),
			editor_print_cmdopt_default,
			editor_print_cmdopt,
			sizeof(editor_print_cmdopt) / sizeof(TCHAR),
			ini_path);

	//
	// editorコマンドのexeファイル名を得る
	//
	for( i = lstrlen(editor_exepath) -1; i != 0; i-- ) {
		if( editor_exepath[i] == L'\\' ) {
			lstrcpy( editor_exename, &editor_exepath[i +1] );
			break;
		}
	}

	//
	// コマンドラインを作成
	//
	next_arg = (LPTSTR)malloc( (lstrlen(editor_exepath)  + 1 + lstrlen(editor_cmdopt) + lstrlen(editor_print_cmdopt) + 1 + 1 + lstrlen(open_file_path) + 1 + 1) * sizeof(TCHAR) );
	lstrcpy( next_arg, TEXT("\"") );
	lstrcat( next_arg, editor_exename );
	lstrcat( next_arg, TEXT("\"") );
	if( print_mode == false ) {
		if( lstrlen(editor_cmdopt) > 0 ) {
			lstrcat( next_arg, TEXT(" ") );
			lstrcat( next_arg, editor_cmdopt );
		}
	}
	else {
		if( lstrlen(editor_print_cmdopt) > 0 ) {
			lstrcat( next_arg, TEXT(" ") );
			lstrcat( next_arg, editor_print_cmdopt );
		}
	}
	lstrcat( next_arg, TEXT(" \"") );
	lstrcat( next_arg, open_file_path );
	lstrcat( next_arg, TEXT("\"") );

#if 0
	MessageBox( NULL, next_arg, TEXT("dump"), MB_OK );
	return 0;
#endif


	//
	// エディタ起動
	//
	STARTUPINFO		si;
	si.cb = sizeof(si);
	si.lpReserved = NULL;
	si.lpDesktop = NULL;
	si.lpTitle = NULL;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;
	si.cbReserved2 = 0;
	si.lpReserved2 = NULL;
	PROCESS_INFORMATION	pi;
	CreateProcessW(
		editor_exepath,		/* Executable name */
		next_arg,			/* Command to execute */
		NULL,				/* Process security attributes */
		NULL,				/* Thread security attributes */
		FALSE,				/* Inherit handles */
		CREATE_NEW_CONSOLE,	/* Creation flags */
		NULL,				/* Environment */
		NULL,				/* Current directory */
		&si,				/* Startup information */
		&pi);				/* Process information */

	free( next_arg );
	LocalFree( argv );

	return 0;
}

