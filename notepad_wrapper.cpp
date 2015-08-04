#ifndef UNICODE
#define UNICODE
#endif
#include<windows.h>

/*ARGSUSED*/
    int WINAPI
WinMain(
    HINSTANCE	hInstance,
    HINSTANCE	hPrevInst,
    LPSTR	lpszCmdLine,
    int		nCmdShow)
{
	int		i;
	// �f�t�H���g�̃G�f�B�^�p�X�Bnotepad.exe���w�肷���notepad_wrapper.exe���ċN�ŌĂ΂�Ă��܂����ߕʂȃR�}���h�ɂ���B
	TCHAR	editor_exepath_default[] = TEXT("C:\\Program Files\\Windows NT\\Accessories\\wordpad.exe");
	TCHAR	editor_cmdopt_default[] = TEXT("");
	TCHAR	editor_exepath[MAX_PATH] = TEXT("");
	TCHAR	editor_exename[MAX_PATH] = TEXT("");
	TCHAR	editor_cmdopt[MAX_PATH] = TEXT("");
	LPTSTR	exe_path;
	LPTSTR	ini_path;
	LPTSTR	cmdline;
	int		argc;
	LPTSTR*	argv;
	LPTSTR	next_arg;

	//
	// �R�}���h���C�����擾���Aargv�𓾂�
	//
	cmdline = GetCommandLine();
	argv = CommandLineToArgvW( cmdline, &argc );

	//
	// ���̃R�}���h��ini�t�@�C���p�X�𓾂�
	//
	exe_path = argv[0];
	ini_path = (LPTSTR)malloc( (lstrlen(exe_path) + 1) * sizeof(TCHAR) );
	if( lstrcmp( &exe_path[lstrlen(exe_path) -4], TEXT(".exe") ) == 0 ) {
		lstrcpy( ini_path, exe_path );
		lstrcpy( &ini_path[lstrlen(exe_path) -4], TEXT(".ini") );
	}

	//
	// ini�t�@�C�����玟��editor�̃R�}���h�p�X�ƈ����𓾂�
	//
	GetPrivateProfileString(
			TEXT("default"),
			TEXT("exe_path"),
			editor_exepath_default,
			editor_exepath,
			sizeof(editor_exepath) / sizeof(TCHAR),
			ini_path);
	GetPrivateProfileString(
			TEXT("default"),
			TEXT("exe_options"),
			editor_cmdopt_default,
			editor_cmdopt,
			sizeof(editor_cmdopt) / sizeof(TCHAR),
			ini_path);

	//
	// editor�R�}���h��exe�t�@�C�����𓾂�
	//
	for( i = lstrlen(editor_exepath) -1; i != 0; i-- ) {
		if( editor_exepath[i] == L'\\' ) {
			lstrcpy( editor_exename, &editor_exepath[i +1] );
			break;
		}
	}

	//
	// �R�}���h���C�����쐬
	//
	next_arg = (LPTSTR)malloc( (lstrlen(editor_exepath)  + 1 + lstrlen(cmdline) + (argc *2) + 1) * sizeof(TCHAR) );
	lstrcat( next_arg, TEXT("\"") );
	lstrcpy( next_arg, editor_exename );
	lstrcat( next_arg, TEXT("\"") );
	if( lstrlen(editor_cmdopt) > 0 ) {
		lstrcat( next_arg, TEXT(" ") );
		lstrcat( next_arg, editor_cmdopt );
	}
	for( i = 2; i < argc; i++ ) {
		lstrcat( next_arg, TEXT(" \"") );
		lstrcat( next_arg, argv[i] );
		lstrcat( next_arg, TEXT("\"") );
	}

#if 0
	MessageBox( NULL, next_arg, TEXT("dump"), MB_OK );
	return 0;
#endif


	//
	// �G�f�B�^�N��
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
