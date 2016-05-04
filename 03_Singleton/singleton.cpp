#include "..\CommonFiles\CmnHdr.h"     /* See Appendix A. */
#include <windowsx.h>
#include <Sddl.h>          // for SID management
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <locale.h>

//�򿪱�־
BOOL  bHasOpened=FALSE;
// ȫ�ֱ���
HANDLE   g_hSingleton = NULL; //����ģʽ��־
HANDLE  g_hBoundary=NULL;  //ȫ��������
HANDLE g_hNameSpace=NULL;//˽�������ռ�

// Names of boundary and private namespace
PCTSTR   g_szBoundary = TEXT("Boundary namespace");
PCTSTR   g_szNamespace = TEXT("private Namespace");

int main(){
	BOOL isCompleted=TRUE;
	setlocale(LC_ALL,"chs");
	//�����߽�������Handle
	g_hBoundary = CreateBoundaryDescriptor(g_szBoundary,0);//g_szBoundary��ʾ�߽�����
	//�������ع���Ա���Ӧ�ĵ�SID
	BYTE  localAdminSIDs[SECURITY_MAX_SID_SIZE];
	PSID pLocalAdminSID=&localAdminSIDs;
	DWORD cb=sizeof(localAdminSIDs);
	if (!CreateWellKnownSid(WinBuiltinAdministratorsSid,NULL,pLocalAdminSID,&cb))
	{
		_tprintf(TEXT("�޷��������ع���Ա�û����Ӧ��sid"));
		return 0;
	}
	//�󶨱��ع���ԱSID��߽�������
	if (!AddSIDToBoundaryDescriptor(&g_hBoundary,pLocalAdminSID))
	{
		_tprintf(TEXT("�޷��󶨱��ع���ԱSID��߽�������"));
		isCompleted=FALSE;
	}

	//����˽�������ռ䣨������SECURITY_ATTRIBUTES,�߽������������˽�������ռ����֣�
	//1)����SECURITY_ATTRIBUTES sa
	SECURITY_ATTRIBUTES sa;
	sa.bInheritHandle=FALSE;
	sa.lpSecurityDescriptor=NULL;
	sa.nLength=sizeof(sa);

	//2) ͨ��ָ���ַ�������	sa.lpSecurityDescriptor
	//ConvertStringSecurityDescriptorToSecurityDescriptor �������Խ�һ������ȫ������
	//��ʽ���ַ���ת����һ����Ч�İ�ȫ�������ṹ����������ConvertSecurityDescriptorToStringSecurityDescriptor�����Ĺ����෴��
	//http://baike.baidu.com/link?url=LW8rbYmyN10Akv2G_sKejVoP-z5pI9xWIHf2umH0vrbhFwYgnKoK-oCVXdf5UXw_ruessnBUFVBZ-pHDdQitDa
	if (!ConvertStringSecurityDescriptorToSecurityDescriptor(TEXT("D:(A;;GA;;;BA)"),SDDL_REVISION_1,&sa.lpSecurityDescriptor,NULL))
	{
		_tprintf(TEXT("�޷�����sa.lpSecurityDescriptor"));
		isCompleted=FALSE;
	}
	//3)����˽�����ƿռ�(�ں˶���)
	g_hNameSpace=CreatePrivateNamespace(&sa,g_hBoundary,g_szNamespace);//��������ȫ���ԣ��߽磬���ƿռ���(�ַ���)
	
	//4)��˽�����ƿռ���в���
	if (g_hNameSpace==NULL)
	{
		DWORD dwLastError=GetLastError();
		if (dwLastError==ERROR_ACCESS_DENIED)//��Ȩ����
		{
			_tprintf(TEXT("��ʹ�ù���ԱȨ��\n"));
			isCompleted=FALSE;
		}
		else{
			if (dwLastError==ERROR_ALREADY_EXISTS)//���ƿռ��Ѿ�����
			{
				_tprintf(TEXT("���ƿռ��Ѿ�����\n"));
				//�����ƿռ�
				g_hNameSpace=OpenPrivateNamespace(g_hBoundary,g_szNamespace);//�������ƺ�Ȩ�޴����ƿռ�
				if (g_hNameSpace==NULL)
				{
					_tprintf(TEXT("�޷�������˽�����ƿռ�\n"));
					isCompleted=FALSE;
				}else{
					bHasOpened=TRUE;
					_tprintf(TEXT("������˽�����ƿռ�ɹ�\n"));
				}
			}
			else{
				_tprintf(TEXT("δ֪����\n"));
					isCompleted=FALSE;
			}

		}
	}
	//��˽�����ƿռ䴴��mutex����
	TCHAR  szMutexName[64];
	StringCchPrintf(szMutexName,_countof(szMutexName),TEXT("%s\\%s"),g_szNamespace,TEXT("Singleton"));
	g_hSingleton = CreateMutex(NULL, FALSE, szMutexName);
	if (	isCompleted==FALSE||GetLastError()==ERROR_ALREADY_EXISTS)
	{
			_tprintf(TEXT("˽�����ƿռ��Ѿ�����\n"));
	}
	else{
		  _tprintf(TEXT("���Լ�����������\n"));
	}
	system("pause");
	//�ͷ��ں˶���

	//�ͷ�mutex����
	if (g_hSingleton!=NULL)
	{
		CloseHandle(g_hSingleton);
	}
	//�ͷ����ƿռ�
	if (g_hNameSpace != NULL) {
		if (bHasOpened) {  // Open namespace
			ClosePrivateNamespace(g_hNameSpace, 0);//���ڴ򿪵�namespace
		} else { // Created namespace
			ClosePrivateNamespace(g_hNameSpace, PRIVATE_NAMESPACE_FLAG_DESTROY);//���ڴ����� namespace
		}
	}
	//�ͷű߽��ʾ��
	if (g_hBoundary != NULL) {
		DeleteBoundaryDescriptor(g_hBoundary);
	}
	return 0;
}