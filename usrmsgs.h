#ifndef NO5USERMSGS_H
#define NO5USERMSGS_H
#pragma once
#define NO5WM_FIRST			( WM_APP + 10 )
#define NO5WM_MDIACTIVATE	( NO5WM_FIRST + 1 )
#define NO5WM_CLOSE			( NO5WM_FIRST + 2 )
#define NO5WM_SETCHILDTEXT	( NO5WM_FIRST + 4 )
#define NO5WM_FADECLICK		( NO5WM_FIRST + 5 )
#define NO5WM_COLORDLG		( NO5WM_FIRST + 6 )		// ( 0 , this )
#define NO5WM_ENTER			( NO5WM_FIRST + 7 )		// ( text )
#define NO5WM_ROOMDLG		( NO5WM_FIRST + 8 )		// 
#define NO5WM_OPTIONSDLG	( NO5WM_FIRST + 9 )
#define NO5WM_UNIGNORE		( NO5WM_FIRST + 10 )	// (0,name) or ( 1,0) if ignore list reset	
#define NO5WM_CAMDLGDEL		( NO5WM_FIRST + 11 )	// ( 0, this ) delete this
#define NO5WM_CLRDLGDEL		( NO5WM_FIRST + 12 )	// ( 0, this ) delete this
#define NO5WM_SMILESWND		( NO5WM_FIRST + 13 )	// ( 0, this ) delete this
#define NO5WM_SCRIPTWND		( NO5WM_FIRST + 14 )	// ( 0, this ) delete this
#define NO5WM_FORMVIEW		( NO5WM_FIRST + 15 )	// ( 0, this ) delete this
#define NO5WM_TYPING		( NO5WM_FIRST + 16 )	// ( bStart, 0 ) 
#define NO5WM_VOICEDLGDEL	( NO5WM_FIRST + 17 )	// ( 0, this ) delete this

#endif // NO5USERMSGS_H
