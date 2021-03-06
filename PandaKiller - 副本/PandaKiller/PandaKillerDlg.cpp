
// PandaKillerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "PandaKiller.h"
#include "PandaKillerDlg.h"
#include "afxdialogex.h"

#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString csTxt;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPandaKillerDlg 对话框



CPandaKillerDlg::CPandaKillerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PANDAKILLER_DIALOG, pParent)
	, m_edit(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPandaKillerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_edit);
}

BEGIN_MESSAGE_MAP(CPandaKillerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CPandaKillerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPandaKillerDlg 消息处理程序

BOOL CPandaKillerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPandaKillerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPandaKillerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPandaKillerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//1.在内存中查找病毒是否存在
bool CPandaKillerDlg::FindTargetProcess(char * pszProcessName, DWORD * dwpid)
{

	TCHAR szadb[] = _T("spo0lsv.exe");


	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnap)
	{
		return FALSE;
	}
	if (Process32First(hSnap, &pe))
	{
		do
		{
			if (lstrcmpi(szadb, pe.szExeFile) == 0)
			{
				*dwpid = pe.th32ProcessID;
				return TRUE;
			}
		} while (Process32Next(hSnap, &pe));
	}
	CloseHandle(hSnap);
	return FALSE; //其余都返回FALSE
}

//2.提升权限,访问一些受限制的系统资源
bool CPandaKillerDlg::EnableDebugPrivilege(char * pszPrivilege)
{
	HANDLE hToken = INVALID_HANDLE_VALUE;
	LUID luid;
	TOKEN_PRIVILEGES tp;

	BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	if (bRet == FALSE)
	{
		return bRet;
	}

	bRet = LookupPrivilegeValue(NULL, (LPCWSTR)pszPrivilege, &luid);//这里需要进行类型转换,将pszPrivilege转换成LPCWSTR类型
	if (bRet == FALSE)
	{
		return bRet;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

	return bRet;
}

DWORD CRC32(BYTE* ptr, DWORD Size)
{
	DWORD crcTable[256], crcTmp1;
	//动态生成CRC-32表
	for (int i = 0; i < 256; i++)
	{
		crcTmp1 = i;
		for (int j = 8; j > 0; j--)
		{
			if (crcTmp1 & 1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
			else crcTmp1 >>= 1;
		}

		crcTable[i] = crcTmp1;
	}
	//计算CRC32值
	DWORD crcTmp2 = 0xFFFFFFFF;
	while (Size--)
	{
		crcTmp2 = ((crcTmp2 >> 8) & 0x00FFFFFF) ^ crcTable[(crcTmp2 ^ (*ptr)) & 0xFF];
		ptr++;
	}
	return (crcTmp2 ^ 0xFFFFFFFF);
}


DWORD WINAPI FindFiles(LPVOID lpszPath)
{
	WIN32_FIND_DATA stFindFile;
	HANDLE hFindFile;
	// 扫描路径
	char szPath[MAX_PATH];
	char szFindFile[MAX_PATH];
	char szSearch[MAX_PATH];
	char *szFilter;
	int len;
	int ret = 0;

	szFilter = "*.*";
	lstrcpyA(szPath, (char *)lpszPath);

	len = lstrlenA(szPath);
	if (szPath[len - 1] != '\\')
	{
		szPath[len] = '\\';
		szPath[len + 1] = '\0';
	}

	lstrcpyA(szSearch, szPath);
	lstrcatA(szSearch, szFilter);

	hFindFile = FindFirstFile((LPCWSTR)szSearch, &stFindFile);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			lstrcpyA(szFindFile, szPath);
			lstrcatA(szFindFile, (LPCSTR)stFindFile.cFileName);

			if (stFindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (stFindFile.cFileName[0] != '.')
				{
					FindFiles(szFindFile);
				}
			}
			else
			{
				if (!lstrcmpA((LPCSTR)stFindFile.cFileName, "Desktop_.ini"))
				{
					// 去除文件的隐藏、系统以及只读属性
					DWORD dwFileAttributes = GetFileAttributesA(szFindFile);
					dwFileAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
					dwFileAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
					dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
					SetFileAttributesA(szFindFile, dwFileAttributes);
					// 删除Desktop_.ini
					BOOL bRet = DeleteFileA(szFindFile);
					csTxt += szFindFile;
					if (bRet)
					{
						csTxt += _T("被删除！\r\n");
					}
					else
					{
						csTxt += _T("无法删除\r\n");
					}
				}
			}
			ret = FindNextFile(hFindFile, &stFindFile);
		} while (ret != 0);
	}

	FindClose(hFindFile);

	return 0;
}


void CPandaKillerDlg::OnBnClickedButton1()
{
	csTxt = "";
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	BOOL bRet = FALSE;
	DWORD dwPid = 0;
	///////////////////////////////////////////////////////////////////
	//  结束spo0lsv.exe进程，并删除病毒程序本身
	///////////////////////////////////////////////////////////////////
	bRet = FindTargetProcess("spo0lsv.exe", &dwPid);
	if (bRet == TRUE)
	{
		csTxt = _T("检查系统内存...\r\n");
		csTxt += _T("系统中存在病毒进程:spo0lsv.exe\r\n");
		csTxt += _T("准备进行查杀...\r\n");
		SetDlgItemText(IDC_EDIT1, csTxt);
		// 提升权限
		bRet = EnableDebugPrivilege((char*)SE_DEBUG_NAME);
		if (bRet == FALSE)
		{
			csTxt += _T("提升权限失败\r\n");
		}
		else
		{
			csTxt += _T("提升权限成功！\r\n");
		}
		SetDlgItemText(IDC_EDIT1, csTxt);
		// 打开并尝试结束病毒进程
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
		if (hProcess == INVALID_HANDLE_VALUE)
		{
			csTxt += _T("无法结束病毒进程\r\n");
			return;
		}
		bRet = TerminateProcess(hProcess, 0);
		if (bRet == FALSE)
		{
			csTxt += _T("无法结束病毒进程\r\n");
			return;
		}
		csTxt += _T("病毒进程已经结束\r\n");
		SetDlgItemText(IDC_EDIT1, csTxt);
		CloseHandle(hProcess);
	}
	else
	{
		csTxt += _T("系统中不存在spo0lsv.exe病毒进程\r\n");
	}

	Sleep(10);
	// 查杀磁盘中是否存在名为spo0lsv.exe的病毒文件
	char szSysPath[MAX_PATH] = { 0 };
	GetSystemDirectoryA(szSysPath, MAX_PATH);

	lstrcatA(szSysPath, "\\drivers\\spo0lsv.exe");

	csTxt += _T("检查硬盘中是否存在spo0lsv.exe文件...\r\n");

	if (GetFileAttributesA(szSysPath) == 0xFFFFFFFF)
	{
		csTxt += _T("spo0lsv.exe病毒文件不存在\r\n");
	}
	else
	{
		csTxt += _T("spo0lsv.exe病毒文件存在，正在计算散列值\r\n");
		csTxt += _T("是否与病毒库收录散列值E63D45D3相等\r\n");

		HANDLE hFile = CreateFileA(szSysPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(L"Create Error");
			return;
		}
		DWORD dwSize = GetFileSize(hFile, NULL);
		if (dwSize == 0xFFFFFFFF)
		{
			MessageBox(L"GetFileSize Error");
			return;
		}
		BYTE *pFile = (BYTE*)malloc(dwSize);
		if (pFile == NULL)
		{
			MessageBox(L"malloc Error");
			return;
		}

		DWORD dwNum = 0;
		ReadFile(hFile, pFile, dwSize, &dwNum, NULL);
		// 计算spo0lsv.exe的散列值
		DWORD dwCrc32 = CRC32(pFile, dwSize);

		if (pFile != NULL)
		{
			free(pFile);
			pFile = NULL;
		}

		CloseHandle(hFile);
		// 0xE63D45D3是“熊猫烧香”病毒的散列值
		if (dwCrc32 != 0xE63D45D3)
		{
			csTxt += _T("spo0lsv.exe校验和验证失败\r\n");
		}
		else
		{
			csTxt += _T("spo0lsv.exe校验和验证成功，正在删除...\r\n");
			// 去除文件的隐藏、系统以及只读属性
			DWORD dwFileAttributes = GetFileAttributesA(szSysPath);
			dwFileAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
			dwFileAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
			dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributesA(szSysPath, dwFileAttributes);
			// 删除spo0lsv.exe
			bRet = DeleteFileA(szSysPath);
			if (bRet)
			{
				csTxt += _T("spo0lsv.exe病毒被删除！\r\n");
			}
			else
			{
				csTxt += _T("spo0lsv.exe病毒无法删除\r\n");
			}
		}
	}
	SetDlgItemText(IDC_EDIT1, csTxt);
	Sleep(10);
	///////////////////////////////////////////////////////////////////
	//  删除每个盘符下的setup.exe与autorun.inf，以及Desktop_.ini
	///////////////////////////////////////////////////////////////////
	char szDriverString[MAXBYTE] = { 0 };
	char *pTmp = NULL;
	//获取字符串类型的驱动器列表  
	GetLogicalDriveStringsA(MAXBYTE, szDriverString);

	pTmp = szDriverString;

	while (*pTmp)
	{
		char szAutorunPath[MAX_PATH] = { 0 };
		char szSetupPath[MAX_PATH] = { 0 };
		lstrcatA(szAutorunPath, pTmp);
		lstrcatA(szAutorunPath, "autorun.inf");
		lstrcatA(szSetupPath, pTmp);
		lstrcatA(szSetupPath, "setup.exe");

		if (GetFileAttributesA(szSetupPath) == 0xFFFFFFFF)
		{
			csTxt += pTmp;
			csTxt += _T("setup.exe病毒文件不存在\r\n");
		}
		else
		{
			csTxt += pTmp;
			csTxt += _T("setup.exe病毒文件存在，正在进行计算校验和...\r\n");
			csTxt += _T("是否与病毒库收录散列值E63D45D3相等\r\n");
			HANDLE hFile = CreateFileA(szSetupPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				MessageBox(L"Create Error");
				return;
			}
			DWORD dwSize = GetFileSize(hFile, NULL);
			if (dwSize == 0xFFFFFFFF)
			{
				MessageBox(L"GetFileSize Error");
				return;
			}
			BYTE *pFile = (BYTE*)malloc(dwSize);
			if (pFile == NULL)
			{
				MessageBox(L"malloc Error");
				return;
			}

			DWORD dwNum = 0;
			ReadFile(hFile, pFile, dwSize, &dwNum, NULL);

			DWORD dwCrc32 = CRC32(pFile, dwSize);
			if (pFile != NULL)
			{
				free(pFile);
				pFile = NULL;
			}
			CloseHandle(hFile);
			if (dwCrc32 != 0xE63D45D3)
			{
				csTxt += _T("校验和验证失败\r\n");
			}
			else
			{
				csTxt += _T("校验和验证成功，正在删除...\r\n");
				// 去除文件的隐藏、系统以及只读属性
				DWORD dwFileAttributes = GetFileAttributesA(szSetupPath);
				dwFileAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
				dwFileAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
				dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributesA(szSetupPath, dwFileAttributes);
				// 删除setup.exe
				bRet = DeleteFileA(szSetupPath);
				if (bRet)
				{
					csTxt += pTmp;
					csTxt += _T("setup.exe病毒被删除!\r\n");
				}
				else
				{
					csTxt += pTmp;
					csTxt += _T("setup.exe病毒无法删除\r\n");
				}
			}
		}
		// 去除文件的隐藏、系统以及只读属性
		DWORD dwFileAttributes = GetFileAttributesA(szAutorunPath);
		dwFileAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
		dwFileAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
		dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributesA(szAutorunPath, dwFileAttributes);
		// 删除autorun.inf
		bRet = DeleteFileA(szAutorunPath);
		csTxt += pTmp;
		if (bRet)
		{
			csTxt += _T("autorun.inf被删除!\r\n");
		}
		else
		{
			csTxt += _T("autorun.inf不存在或无法删除\r\n");
		}
		// 删除Desktop_.ini
		FindFiles(pTmp);
		// 检查下一个盘符
		pTmp += 4;
	}
	Sleep(10);
	///////////////////////////////////////////////////////////////////
	//  修复注册表内容，删除病毒启动项并修复文件的隐藏显示
	///////////////////////////////////////////////////////////////////
	csTxt += _T("正在检查注册表...\r\n");
	SetDlgItemText(IDC_EDIT1, csTxt);
	// 首先检查启动项
	char RegRun[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKeyHKCU = NULL;
	LONG lSize = MAXBYTE;
	char cData[MAXBYTE] = { 0 };

	long lRet = RegOpenKeyA(HKEY_CURRENT_USER, RegRun, &hKeyHKCU);
	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegQueryValueExA(hKeyHKCU, "svcshare", NULL, NULL, (unsigned char *)cData, (unsigned long *)&lSize);
		if (lRet == ERROR_SUCCESS)
		{
			if (lstrcmpA(cData, "C:\\WINDOWS\\system32\\drivers\\spo0lsv.exe") == 0)
			{
				csTxt += _T("注册表启动项中存在病毒信息\r\n");
			}

			lRet = RegDeleteValueA(hKeyHKCU, "svcshare");
			if (lRet == ERROR_SUCCESS)
			{
				csTxt += _T("注册表启动项中的病毒信息已删除！\r\n");
			}
			else
			{
				csTxt += _T("注册表启动项中的病毒信息无法删除\r\n");
			}
		}
		else
		{
			csTxt += _T("注册表启动项中不存在病毒信息\r\n");
		}
		RegCloseKey(hKeyHKCU);
	}
	else
	{
		csTxt += _T("注册表启动项信息读取失败\r\n");
	}
	// 接下来修复文件的隐藏显示，需要将CheckedValue的值设置为1
	char RegHide[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\Folder\\Hidden\\SHOWALL";
	HKEY hKeyHKLM = NULL;
	DWORD dwFlag = 1;

	long lRetHide = RegOpenKeyA(HKEY_LOCAL_MACHINE, RegHide, &hKeyHKLM);
	if (lRetHide == ERROR_SUCCESS)
	{
		csTxt += _T("检测注册表的文件隐藏选项...\r\n");
		if (ERROR_SUCCESS == RegSetValueExA(
			hKeyHKLM,             //subkey handle  
			"CheckedValue",       //value name  
			0,                    //must be zero  
			REG_DWORD,            //value type  
			(CONST BYTE*)&dwFlag, //pointer to value data  
			4))                   //length of value data
		{
			csTxt += _T("注册表修复完毕！\r\n");
		}
		else
		{
			csTxt += _T("无法恢复注册表的文件隐藏选项\r\n");
		}
	}
	///////////////////////////////////////////////////////////////////
	// 病毒查杀完成
	///////////////////////////////////////////////////////////////////
	csTxt += _T("病毒查杀完成，请使用专业杀毒软件进行全面扫描！\r\n");
	SetDlgItemText(IDC_EDIT1, csTxt);
}


