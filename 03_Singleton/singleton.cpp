#include "..\CommonFiles\CmnHdr.h"     /* See Appendix A. */
#include <windowsx.h>
#include <Sddl.h>          // for SID management
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <locale.h>

//打开标志
BOOL  bHasOpened=FALSE;
// 全局变量
HANDLE   g_hSingleton = NULL; //单例模式标志
HANDLE  g_hBoundary=NULL;  //全局描述符
HANDLE g_hNameSpace=NULL;//私有命名空间

// Names of boundary and private namespace
PCTSTR   g_szBoundary = TEXT("Boundary namespace");
PCTSTR   g_szNamespace = TEXT("private Namespace");

int main(){
	BOOL isCompleted=TRUE;
	setlocale(LC_ALL,"chs");
	//创建边界描述符Handle
	g_hBoundary = CreateBoundaryDescriptor(g_szBoundary,0);//g_szBoundary表示边界名称
	//创建本地管理员相对应的的SID
	BYTE  localAdminSIDs[SECURITY_MAX_SID_SIZE];
	PSID pLocalAdminSID=&localAdminSIDs;
	DWORD cb=sizeof(localAdminSIDs);
	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid,NULL,pLocalAdminSID,&cb))
	{
		_tprintf(TEXT("无法创建本地管理员用户组对应的sid"));
		return 0;
	}
	//绑定本地管理员SID与边界描述符
	if (!AddSIDToBoundaryDescriptor(&g_hBoundary,pLocalAdminSID))
	{
		_tprintf(TEXT("无法绑定本地管理员SID与边界描述符"));
		isCompleted=FALSE;
	}

	//创建私有命名空间（参数：SECURITY_ATTRIBUTES,边界描述符句柄，私有命名空间名字）
	//1)创建SECURITY_ATTRIBUTES sa
	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle=FALSE;
	sa.lpSecurityDescriptor=NULL;
	sa.nLength=sizeof(sa);

	//2) 通过指定字符串设置	sa.lpSecurityDescriptor
	//ConvertStringSecurityDescriptorToSecurityDescriptor 函数可以将一个按安全描述符
	//格式的字符串转换成一个有效的安全描述符结构。本函数和ConvertSecurityDescriptorToStringSecurityDescriptor函数的功能相反。
	//http://baike.baidu.com/link?url=LW8rbYmyN10Akv2G_sKejVoP-z5pI9xWIHf2umH0vrbhFwYgnKoK-oCVXdf5UXw_ruessnBUFVBZ-pHDdQitDa
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(TEXT("D:(A;;GA;;;BA)"),SDDL_REVISION_1,&sa.lpSecurityDescriptor,NULL))
	{
		_tprintf(TEXT("无法设置sa.lpSecurityDescriptor"));
		isCompleted=FALSE;
	}
	//3)创建私有名称空间(内核对象)
	g_hNameSpace=CreatePrivateNamespace(&sa,g_hBoundary,g_szNamespace);//参数，安全属性，边界，名称空间名(字符串)
	
	//4)对私有名称空间进行测试
	if (g_hNameSpace==NULL)
	{
		DWORD dwLastError=GetLastError();
		if (dwLastError==ERROR_ACCESS_DENIED)//无权创建
		{
			_tprintf(TEXT("请使用管理员权限\n"));
			isCompleted=FALSE;
		}
		else{
			if (dwLastError==ERROR_ALREADY_EXISTS)//名称空间已经存在
			{
				_tprintf(TEXT("名称空间已经存在\n"));
				//打开名称空间
				g_hNameSpace=OpenPrivateNamespace(g_hBoundary,g_szNamespace);//按照名称和权限打开名称空间
				if (g_hNameSpace==NULL)
				{
					_tprintf(TEXT("无法打开已有私有名称空间\n"));
					isCompleted=FALSE;
				}else{
					bHasOpened=TRUE;
					_tprintf(TEXT("打开已有私有名称空间成功\n"));
				}
			}
			else{
				_tprintf(TEXT("未知错误\n"));
					isCompleted=FALSE;
			}

		}
	}
	//用私有名称空间创建mutex对象
	TCHAR  szMutexName[64];
	StringCchPrintf(szMutexName,_countof(szMutexName),TEXT("%s\\%s"),g_szNamespace,TEXT("Singleton"));
	g_hSingleton = CreateMutex(NULL, FALSE, szMutexName);
	if (	isCompleted==FALSE||GetLastError()==ERROR_ALREADY_EXISTS)
	{
			_tprintf(TEXT("私有名称空间已经存在\n"));
	}
	else{
		  _tprintf(TEXT("可以继续其他事情\n"));
	}
	system("pause");
	//释放内核对象

	//释放mutex对象
	if (g_hSingleton!=NULL)
	{
		CloseHandle(g_hSingleton);
	}
	//释放名称空间
	if (g_hNameSpace != NULL) {
		if (bHasOpened) {  // Open namespace
			ClosePrivateNamespace(g_hNameSpace, 0);//对于打开的namespace
		} else { // Created namespace
			ClosePrivateNamespace(g_hNameSpace, PRIVATE_NAMESPACE_FLAG_DESTROY);//对于创建的 namespace
		}
	}
	//释放边界标示符
	if (g_hBoundary != NULL) {
		DeleteBoundaryDescriptor(g_hBoundary);
	}
	return 0;
}