
#include "..\CommonFiles\CmnHdr.h"     /* See Appendix A. */
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <StrSafe.h>
#include <iostream>

//pause
#define  PAUSE()  (system("pause"))
//cout<< * <<cout<<endl;
#define  COUT_LINE(content)  (cout<<content<<endl)
//cout without \n;
#define  COUT_WITHOUT_LINE(content)  (cout<<content<<endl)
using namespace std;

int main(){
	_SYSTEM_INFO sinf;
	GetSystemInfo(&sinf);
	cout<<"wProcessorArchitecture:";
	COUT_LINE(sinf.wProcessorArchitecture);
	cout<<"wReserved:";
	COUT_LINE(sinf.wReserved);
	cout<<"dwPageSize:";
	COUT_LINE(sinf.dwPageSize);
	PAUSE();
	return 0;
}