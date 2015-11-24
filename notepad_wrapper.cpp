#ifndef UNICODE
#define UNICODE
#endif
#include<windows.h>

static LPTSTR search_open_file_path( LPTSTR cmdline )
{
	unsigned int skip_argc = 2;	// Skip��������̎c��

	// �I�[�v������t�@�C�����̐擪�𓪏o������
	// e.g.
	// "C:\path to notepad_wrapper\notepad_wrapper.exe" "notepad.exe" C:\path to text file.txt
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~~ ^
	// ��0����skip                                      ��1����skip   �����𓾂�


	while( *cmdline != L'\0' ) {
		// �u"�v��������A�΂ɂȂ�u"�v�܂ŃX�L�b�v
		if( *cmdline == L'"' ) {
			for( ; *cmdline != L'\0'; cmdline++ ) {
				if( *cmdline == L'"' ) {
					cmdline++;
					break;
				}
			}
			continue;
		}

		if( *cmdline == L' ' ) {
			// �u �v(space)��������skip�c�������炷
			skip_argc--;

			// �A������space�͍��킹��skip
			for( ; *cmdline != L'\0'; cmdline++ ) {
				if( *cmdline != L' ' ) {
					break;
				}
			}

			// skip�c��=0�ɂȂ�����A�������t�@�C�����擪
			if( skip_argc == 0 ) {
				return cmdline;
			}
			continue;
		}

		cmdline++;
	}

	return NULL;
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
	// �f�t�H���g�̃G�f�B�^�p�X�Bnotepad.exe���w�肷���notepad_wrapper.exe���ċN�ŌĂ΂�Ă��܂����ߕʂȃR�}���h�ɂ���B
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
	// �R�}���h���C�����擾���Aargv�𓾂�
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
	// "/p"���w�肳��Ă����ꍇ�A�v�����^���[�h�ɂ���
	//
	if( wcsncmp( open_file_path, TEXT("/p "), 3 ) == 0 ) {
		open_file_path += 3;	// "/p "
		// " "��skip�B
		for( ; *open_file_path != L'\0'; open_file_path++ ) {
			if( *open_file_path != L' ' ) {
				break;
			}
		}
		print_mode = true;
	}
	else
	if( lstrcmp( open_file_path, TEXT("/p") ) == 0 ) {
		open_file_path += 2;	// "/p"
		print_mode = true;
	}


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
	// open�p�R�}���h���C���I�v�V����
	GetPrivateProfileString(
			TEXT("default"),
			TEXT("exe_options"),
			editor_cmdopt_default,
			editor_cmdopt,
			sizeof(editor_cmdopt) / sizeof(TCHAR),
			ini_path);
	// print�p�R�}���h���C���I�v�V����
	GetPrivateProfileString(
			TEXT("default"),
			TEXT("exe_print_options"),
			editor_print_cmdopt_default,
			editor_print_cmdopt,
			sizeof(editor_print_cmdopt) / sizeof(TCHAR),
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
	next_arg = (LPTSTR)malloc( (lstrlen(editor_exepath)  + 1 + lstrlen(editor_cmdopt) + lstrlen(editor_print_cmdopt) + 1 + 1 + lstrlen(open_file_path) + 1 + 1) * sizeof(TCHAR) );
	lstrcpy( next_arg, TEXT("\"") );
	lstrcat( next_arg, editor_exename );
	lstrcat( next_arg, TEXT("\"") );
	// �G�f�B�^�̌Œ������ǉ��B�v�����g���[�h���ǂ����Ő؂蕪����B
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
	// �I�[�v������t�@�C������ǉ��B�u"�v�ň͂܂�Ă��Ȃ��ꍇ�͈͂ށB
	if( lstrlen(open_file_path) != 0 ) {
		if( (open_file_path[0] == L'"') && (open_file_path[lstrlen(open_file_path) -1] == L'"') ) {
			lstrcat( next_arg, TEXT(" ") );
			lstrcat( next_arg, open_file_path );
		}
		else {
			lstrcat( next_arg, TEXT(" \"") );
			lstrcat( next_arg, open_file_path );
			lstrcat( next_arg, TEXT("\"") );
		}
	}


#if 0
	MessageBox( NULL, cmdline, TEXT("dump"), MB_OK );
	MessageBox( NULL, next_arg, TEXT("dump"), MB_OK );
	MessageBox( NULL, open_file_path, TEXT("dump"), MB_OK );
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

