//+---------------------------------------------------------------------------
// 将wchar类型转换为char
//---
#ifndef _W2C_H_
#define  _W2C_H_
//////////////////////////////////////////////////////////////////////////
/*
*/
char *w2c(,const wchar_t *pwstr){
	char *pcstr=NULL;
	int wclen=wcslen(pwstr);
	int nbytes = WideCharToMultiByte( 0, 	0,pwstr,   wclen,  NULL, 0,NULL,  NULL );  
}
#endif

