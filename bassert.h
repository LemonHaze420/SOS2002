#ifndef _ASSERT_H_
#define _ASSERT_H_

//#include ".h"
#include <stdio.h>

extern void Assert( int exp );
//extern void Whinge( int exp );
extern void Warning( char *string );
extern void WarningOnce( char *string );
//extern void BreakProgram();

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif
#endif

#define Output printf

#ifdef DEBUG

#if defined ( WIN32 )

#include <CrtDbg.h>
#include <Windows.h>

//class WhingeHandler
//	{
//	public:
//		int value;
//		const char *comment;
//		WhingeHandler( const char *str ) { comment = str; value = 0; }
//		~WhingeHandler( ) { if( value ) Output("Whinge: Whinge %s, was triggered (%li times)\n", comment, value ); else Output("Whinge %s, was never triggered (cool)\n", comment ); }
//	};

class WarningOnceHandler
	{
	public:
		int value;
		const char *comment;
		WarningOnceHandler( const char *str ) { comment = str; value = 0; }
		~WarningOnceHandler( ) { if( value ) Output("WARNING: Warning %s, was triggered (%i times)\n", comment, value ); else Output("Warning %s, was never triggered (cool)\n", comment ); }
	};

#define Assert(expr) \
        do { if ( !(expr) && \
			(1 == _CrtDbgReport(_CRT_ERROR, __FILE__, __LINE__, NULL, #expr \
			)) ) _CrtDbgBreak(); } while( 0 )

//#define Whinge(expr)	{ \
//						static WhingeHandler doneYet( #expr ); \
//						if( !doneYet.value )\
//							{ \
//							doneYet.value = 1; \
//							MessageBox(NULL,TEXT(( #expr )),TEXT("Core Code"),MB_OK|MB_ICONWARNING);\
//							}\
//						else\
//							++(doneYet.value);\
//						}

#define Warning(string)    MessageBox(NULL,TEXT((string)),TEXT("Core Code"),MB_OK|MB_ICONWARNING)
#define WarningOnce(string)	{ \
								static WarningOnceHandler doneYet( string ); \
								if( !doneYet.value )\
									{ \
									doneYet.value = 1; \
									MessageBox(NULL,TEXT((string)),TEXT("Core Code"),MB_OK|MB_ICONWARNING);\
									}\
								else\
									++(doneYet.value);\
								}

//#define BreakProgram() _CrtDbgBreak()

#elif defined ( GCC )

#define Assert(expr) { static int val = 1; if( !( expr ) ) { \
	printf( "Error on LINE:%li, FILE:%s [%s]\n", __LINE__, __FILE__ ); \
	exit(0); \
		} while( val ) { if( !val ) { val = 1; break; } }  } \
	}

#define Whinge(expr) { static int val = 1; if( !( expr ) ) { \
	printf( "Error on LINE:%li, FILE:%s [%s]\n", __LINE__, __FILE__ ); \
	exit(0); \
		} while( val ) { if( !val ) { val = 1; break; } }  } \
	}

#define Warning(string) printf("Warning: %s", string )
#define WarningOnce(string) { static int val = 0; if( val ) { val = 1; printf("Warning: %s", string ); } else ++val; }

//#define BreakProgram() ((void)0)

#endif
// end of platform specific

#else 
// if not debug
#define Assert(exp)        ((void)0)
#define Whinge(exp)        ((void)0)
#define Warning(string)    ((void)0)
#define WarningOnce(string)    ((void)0)
//#define BreakProgram() ((void)0)
#endif


#endif
