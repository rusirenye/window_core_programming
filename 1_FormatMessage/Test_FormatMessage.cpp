#include "..\CommonFiles\CmnHdr.h"     /* See Appendix A. */
#include <Windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
using namespace std;

char *w2c(char *pcstr,const wchar_t *pwstr, size_t len)

{

	int nlength=wcslen(pwstr);

	//获取转换后的长度

	int nbytes = WideCharToMultiByte( 0, // specify the code page used to perform the conversion

		0,         // no special flags to handle unmapped characters

		pwstr,     // wide character string to convert

		nlength,   // the number of wide characters in that string

		NULL,      // no output buffer given, we just want to know how long it needs to be

		0,

		NULL,      // no replacement character given

		NULL );    // we don't want to know if a character didn't make it through the translation

	// make sure the buffer is big enough for this, making it larger if necessary

	if(nbytes>len)   nbytes=len;

	// 通过以上得到的结果，转换unicode 字符为ascii 字符

	WideCharToMultiByte( 0, // specify the code page used to perform the conversion

		0,         // no special flags to handle unmapped characters

		pwstr,   // wide character string to convert

		nlength,   // the number of wide characters in that string

		pcstr, // put the output ascii characters at the end of the buffer

		nbytes,                           // there is at least this much space there

		NULL,      // no replacement character given

		NULL );

	return pcstr ;

}
int  main()
{
	HLOCAL  hlocal=NULL;//buffer 指针用于指向 错误消息内容
	DWORD systemLocal=MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL);//查询系统语言id
	/*
	*/
	DWORD dwError;
	while(cin>>dwError){
		//Get the error code's textual description
		BOOL fOK=FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_ALLOCATE_BUFFER,
			NULL,dwError,systemLocal,
			(PTSTR) &hlocal,0,NULL);
			if(fOK&&hlocal!=NULL)
			{
				locale loc("chs");
				wcout.imbue( loc ); // 载入中文字符输入方式
				wcout<<(PTSTR)hlocal<<endl;
				continue;
				char * pcstr=(char * )malloc (sizeof(char )*(2*wcslen((PTSTR)hlocal)+1));
				memset(pcstr , 0 , 2 * wcslen((PTSTR)hlocal)+1 );
				w2c(pcstr,(PTSTR)hlocal,2*wcslen((PTSTR)hlocal+1));
				  printf("%s\n",pcstr);
				//wprintf_s(L"%s", (LPWSTR) LocalLock(hlocal));
				LocalFree(hlocal);
			}
	}


	system("pause");
	return 0;
}