#include "pch.h"
#include "framework.h"
#include "FBSCompileTool.h"
#include "FBSCompileToolDlg.h"
#include "afxdialogex.h"
#include "Common/Common.h"
#include <direct.h>
#include <corecrt_io.h>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WMSG_FUNCTION		WM_USER + 1

CFBSCompileToolDlg::CFBSCompileToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FBSCOMPILETOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFBSCompileToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FLATC_PATH, _editFlatcPath);
	DDX_Control(pDX, IDC_COMBO_SAVE_TYPE, _comboboxSaveType);
	DDX_Control(pDX, IDC_EDIT_SAVE_PATH, _editSavePath);
	DDX_Control(pDX, IDC_COMBO_SELECT_TYPE, _comboboxSelectType);
	DDX_Control(pDX, IDC_EDIT_FBS_PATH, _editFBSPath);
	DDX_Control(pDX, IDC_BTN_SAVE_PATH, _btnSavePath);
	DDX_Control(pDX, IDC_BTN_FBS_PATH, _btnFBSPath);
	DDX_Control(pDX, IDC_EDIT_RECV, _editRecv);
	DDX_Control(pDX, IDC_EDIT_OTHER_PARAMS, _editOtherParams);
	DDX_Control(pDX, IDC_COMBO_COMPILE_LANG, _comboCompileLang);
	DDX_Control(pDX, IDC_BTN_GENERATE, _btnGenerate);
}

BEGIN_MESSAGE_MAP(CFBSCompileToolDlg, CDialogEx)
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WMSG_FUNCTION, &CFBSCompileToolDlg::OnFunction)
	ON_BN_CLICKED(IDC_BTN_FLATC_PATH, &CFBSCompileToolDlg::OnBtnFlatcPath)
	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, &CFBSCompileToolDlg::OnBtnSavePath)
	ON_CBN_SELCHANGE(IDC_COMBO_SAVE_TYPE, &CFBSCompileToolDlg::OnCbnSelchangeComboSaveType)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_TYPE, &CFBSCompileToolDlg::OnCbnSelchangeComboSelectType)
	ON_BN_CLICKED(IDC_BTN_FBS_PATH, &CFBSCompileToolDlg::OnBtnFBSPath)
	ON_BN_CLICKED(IDC_BTN_GENERATE, &CFBSCompileToolDlg::OnBtnGenerate)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPILE_LANG, &CFBSCompileToolDlg::OnCbnSelchangeComboCompileLang)
END_MESSAGE_MAP()


BOOL CFBSCompileToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// 加载配置文件
	LoadConfig();

	return TRUE;
}

HCURSOR CFBSCompileToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFBSCompileToolDlg::LoadConfig()
{
	_config = make_shared<CConfigFile>();
	_config->Initialize(L"config.ini");

	// 读取flatc.exe路径
	CString flatcPath = _config->GetString(CFGKEY_COMMON, CFG_FlatcPath);
	if (!flatcPath.IsEmpty())
	{
		if (PathFileExists(flatcPath))
		{
			_editFlatcPath.SetWindowText(flatcPath);
		}

		if (!LoadFlatcVersion(flatcPath))
		{
			// 删除配置
			_config->SetString(CFGKEY_COMMON, CFG_FlatcPath, L"");

			// 提示路径
			AppendMsg(L"flatc.exe路径 例如：vcpkg\\installed\\x64-windows\\tools\\flatbuffers\\flatc.exe");
		}
	}
	else
	{
		// 提示路径
		AppendMsg(L"flatc.exe路径 例如：vcpkg\\installed\\x64-windows\\tools\\flatbuffers\\flatc.exe");
	}

	// 读取编译语言
	int compileLang = _config->GetInt(CFGKEY_COMMON, CFG_FlatcLang);
	_comboCompileLang.AddString(L"cpp");
//	_comboCompileLang.AddString(L"csharp");
	_comboCompileLang.AddString(L"dart");
// 	_comboCompileLang.AddString(L"go");
// 	_comboCompileLang.AddString(L"java");
// 	_comboCompileLang.AddString(L"jsonschema");
// 	_comboCompileLang.AddString(L"kotlin");
// 	_comboCompileLang.AddString(L"lobster");
// 	_comboCompileLang.AddString(L"lua");
// 	_comboCompileLang.AddString(L"php");
// 	_comboCompileLang.AddString(L"python");
// 	_comboCompileLang.AddString(L"rust");
// 	_comboCompileLang.AddString(L"swift");
// 	_comboCompileLang.AddString(L"ts");
	_comboCompileLang.SetCurSel(compileLang);
	OnCbnSelchangeComboCompileLang();

	// 读取其他参数
	CString otherParams;
	CString otherParamsConfig = _config->GetString(CFGKEY_COMMON, CFG_OtherParams);
	_editOtherParams.SetWindowText(otherParamsConfig);

	// 读取保存路径配置
	_btnSavePath.EnableWindow(FALSE);
	_editSavePath.EnableWindow(FALSE);
	_comboboxSaveType.AddString(L"fbs同级文件夹");
	_comboboxSaveType.AddString(L"指定文件夹");
	int saveType = _config->GetInt(CFGKEY_COMMON, CFG_SaveType);
	if (saveType < 2)
	{
		_comboboxSaveType.SetCurSel(saveType);

		if (saveType == 1)
		{
			_btnSavePath.EnableWindow(TRUE);
			_editSavePath.EnableWindow(TRUE);
			CString savePath = _config->GetString(CFGKEY_COMMON, CFG_SavePath);
			if (PathIsDirectory(savePath))
			{
				_editSavePath.SetWindowText(savePath);
			}
			else
			{
				// 删除配置
				_config->SetString(CFGKEY_COMMON, CFG_SavePath, L"");
			}
		}
	}
	else
	{
		_comboboxSaveType.SetCurSel(0);
		_config->SetInt(CFGKEY_COMMON, CFG_SaveType, 0);
	}

	// 读取使用fbs方式配置
	_comboboxSelectType.AddString(L"单个fbs文件");
	_comboboxSelectType.AddString(L"包含fbs文件的文件夹");
	int selectType = _config->GetInt(CFGKEY_COMMON, CFG_SelectType);
	if (selectType < 2)
	{
		_comboboxSelectType.SetCurSel(selectType);

		if (selectType == 1)
		{
			CString fbsPath = _config->GetString(CFGKEY_COMMON, CFG_FBSFilesPath);
			if (PathIsDirectory(fbsPath))
			{
				_editFBSPath.SetWindowText(fbsPath);
			}
			else
			{
				// 删除配置
				_config->SetString(CFGKEY_COMMON, CFG_FBSFilesPath, L"");
			}
		}
	}
	else
	{
		_comboboxSelectType.SetCurSel(0);
		_config->SetInt(CFGKEY_COMMON, CFG_SelectType, 0);
	}
}

bool CFBSCompileToolDlg::IsFBSFileHasService(const CString& fbsPath)
{
	bool isHas = false;
	CStdioFile file;
	if (!file.Open(fbsPath, CFile::modeRead))
	{
		return isHas;
	}

	WCHAR buf[BUFSIZ + 1];
	while (file.ReadString(buf, BUFSIZ))
	{
		// 通过检查是否包含service 来判断是否是RPC文件
		if (wcsstr(buf, L"service ") != nullptr)
		{
			isHas = true;
			break;
		}
	}

	file.Close();
	return isHas;
}

bool CFBSCompileToolDlg::RunFlatc(const CString& flatcPath, CString param)
{
	PROCESS_INFORMATION ProceInfo;
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	HANDLE hRead = NULL;
	HANDLE hWrite = NULL;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return false;
	}

	STARTUPINFO StartInfo;
	ZeroMemory(&StartInfo, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);
	StartInfo.wShowWindow = SW_HIDE;
	//使用指定的句柄作为标准输入输出的文件句柄,使用指定的显示方式
	StartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	StartInfo.hStdError = hWrite;
	StartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	StartInfo.hStdOutput = hWrite;

	bool ret = true;
	param.Format(L"%s %s", flatcPath, param);
	if (CreateProcess(nullptr, param.GetBuffer(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &StartInfo, &ProceInfo))
	{
		WaitForSingleObject(ProceInfo.hProcess, /*INFINITE*/3000); // 等待3秒

		CHAR chBuf[1024] = { 0 };
		DWORD dwRead = 0;
		DWORD dwAvail = 0;
		if (PeekNamedPipe(hRead, NULL, NULL, &dwRead, &dwAvail, NULL) && dwAvail > 0)//PeekNamePipe用来预览一个管道中的数据，用来判断管道中是否为空
		{
			BOOL bSuccess = ReadFile(hRead, chBuf, 1024, &dwRead, NULL); // 这里是读管道，即便已经没有数据，仍然会等待接收数据，所以才需要PeekNamePipe
			if (bSuccess && dwRead != 0)
			{
				// 检测是否有错误消息
				CString strRet(chBuf);
				if (strRet.Find(L"error") != -1)
				{
					ret = false;
				}
				AppendMsg(CString(chBuf));
			}
		}

		CloseHandle(ProceInfo.hThread);
		CloseHandle(ProceInfo.hProcess);
	}
	else
	{
		ret = false;
	}

	CloseHandle(hRead);
	CloseHandle(hWrite);

	return ret;
}

void CFBSCompileToolDlg::OnBtnFlatcPath()
{
	CString defaultDir = GetModuleDir();	//默认打开的文件路径
	CString fileName = L"";			//默认打开的文件名
	CString filter = L"flatc.exe|flatc.exe||";	//文件过虑的类型
	CFileDialog openFileDlg(TRUE, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	INT_PTR result = openFileDlg.DoModal();
	if (result == IDOK)
	{
		CString flatcPath = openFileDlg.GetPathName();
		_editFlatcPath.SetWindowText(flatcPath);

		// 获取flatc版本
		LoadFlatcVersion(flatcPath);

		// 保存配置
		_config->SetString(CFGKEY_COMMON, CFG_FlatcPath, flatcPath);
	}
}

void CFBSCompileToolDlg::OnBtnSavePath()
{
	TCHAR szFolderPath[MAX_PATH] = { 0 };
	BROWSEINFO sInfo;
	ZeroMemory(&sInfo, sizeof(BROWSEINFO));

	sInfo.pidlRoot = CSIDL_DESKTOP;//文件夹的根目录，此处为桌面
	sInfo.lpszTitle = L"请选择一个文件夹";
	sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;

	// 显示文件夹选择对话框
	LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);
	if (lpidlBrowse != nullptr)
	{
		// 取得文件夹名
		if (SHGetPathFromIDList(lpidlBrowse, szFolderPath))
		{
			_editSavePath.SetWindowText(szFolderPath);

			// 保存配置
			_config->SetString(CFGKEY_COMMON, CFG_SavePath, szFolderPath);
		}

		CoTaskMemFree(lpidlBrowse);
	}
}

void CFBSCompileToolDlg::OnBtnFBSPath()
{
	if (_comboboxSelectType.GetCurSel() == 0) // 单个fbs文件
	{
		CString defaultDir = GetModuleDir();	//默认打开的文件路径
		CString fileName = L"";			//默认打开的文件名
		CString filter = L"fbs文件|*.fbs||";	//文件过虑的类型
		CFileDialog openFileDlg(TRUE, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
		INT_PTR result = openFileDlg.DoModal();
		if (result == IDOK)
		{
			CString fbsFilePath = openFileDlg.GetPathName();
			_editFBSPath.SetWindowText(fbsFilePath);
		}
	}
	else// 包含fbs文件的文件夹
	{
		TCHAR szFolderPath[MAX_PATH] = { 0 };
		BROWSEINFO sInfo;
		ZeroMemory(&sInfo, sizeof(BROWSEINFO));

		sInfo.pidlRoot = CSIDL_DESKTOP;//文件夹的根目录，此处为桌面
		sInfo.lpszTitle = L"请选择fbs文件所在文件夹";
		sInfo.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;

		// 显示文件夹选择对话框
		LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);
		if (lpidlBrowse != nullptr)
		{
			// 取得文件夹名
			if (SHGetPathFromIDList(lpidlBrowse, szFolderPath))
			{
				_editFBSPath.SetWindowText(szFolderPath);

				// 保存配置
				_config->SetString(CFGKEY_COMMON, CFG_FBSFilesPath, szFolderPath);
			}

			CoTaskMemFree(lpidlBrowse);
		}
	}
}

void CFBSCompileToolDlg::OnCbnSelchangeComboSaveType()
{
	int saveType = _comboboxSaveType.GetCurSel();
	_btnSavePath.EnableWindow(saveType == 1);
	_editSavePath.EnableWindow(saveType == 1);
	_editSavePath.SetWindowText(L"");
	_config->SetInt(CFGKEY_COMMON, CFG_SaveType, saveType);
}

void CFBSCompileToolDlg::OnCbnSelchangeComboSelectType()
{
	int selectType = _comboboxSelectType.GetCurSel();
	_editFBSPath.SetWindowText(L"");
	_config->SetInt(CFGKEY_COMMON, CFG_SelectType, selectType);
}

void CFBSCompileToolDlg::OnBtnGenerate()
{
	// 检查配置项
	// flatc位置
	CString flatcPath;
	_editFlatcPath.GetWindowText(flatcPath);
	if (flatcPath.IsEmpty())
	{
		MessageBox(L"请选择flatc.exe位置");
		return;
	}
	_config->SetString(CFGKEY_COMMON, CFG_FlatcPath, flatcPath);

	// 编译语言
	CString strCompileLang;
	_comboCompileLang.GetWindowText(strCompileLang);
	CString compileLang = L"--" + strCompileLang;
	_config->SetInt(CFGKEY_COMMON, CFG_FlatcLang, _comboCompileLang.GetCurSel());

	// 读取其他参数
	CString otherParams;
	CString otherParamsConfig;
	_editOtherParams.GetWindowText(otherParamsConfig);
	if (!otherParamsConfig.IsEmpty())
	{
		vector<CString> vecParams;
		StrSplit(otherParamsConfig, L",", vecParams);
		for (auto& param : vecParams)
		{
			otherParams += param;
			otherParams += " "; // 空格间隔
		}
	}
	_config->SetString(CFGKEY_COMMON, CFG_OtherParams, otherParamsConfig);

	// 缓存所有fbs文件路径
	CString fbsPath;
	_editFBSPath.GetWindowText(fbsPath);
	vector<CString> fbsFiles;
	if (_comboboxSelectType.GetCurSel() == 0) // 单个fbs文件
	{

		// 检查文件是否存在
		if (fbsPath.IsEmpty())
		{
			MessageBox(L"请选择一个fbs文件");
			return;
		}

		if (!PathFileExists(fbsPath))
		{
			MessageBox(L"fbs文件不存在");
			_editFBSPath.SetWindowText(L"");
			return;
		}

		fbsFiles.push_back(fbsPath);
	}
	else // 包含fbs文件的文件夹
	{
		if (!PathIsDirectory(fbsPath))
		{
			MessageBox(L"fbs文件夹路径错误");
			_editFBSPath.SetWindowText(L"");
			return;
		}

		// 遍历文件夹
		_chdir(CStringA(fbsPath));
		_finddata_t fileinfo;
		intptr_t hFile = _findfirst("*.fbs", &fileinfo);
		if (hFile != -1)
		{
			do
			{
				// 检查是不是目录，如果不是则保存
				if (!(fileinfo.attrib & _A_SUBDIR))
				{
					fbsFiles.push_back(CombinePath(fbsPath, CString(fileinfo.name)));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}

	// 确定保存文件夹位置
	CString savePath;
	if (_comboboxSaveType.GetCurSel() == 0) // fbs同级文件夹
	{
		if (_comboboxSelectType.GetCurSel() == 0) // 单个fbs文件
		{
			savePath = PathGetDir(fbsPath);
		}
		else // 包含fbs文件的文件夹
		{
			savePath = fbsPath;
		}
	}
	else // 指定文件夹
	{
		_editSavePath.GetWindowText(savePath);
		if (!PathIsDirectory(savePath))
		{
			MessageBox(L"存放位置不存在");
			_editSavePath.SetWindowText(L"");
			return;
		}
	}

	// 保存配置
	_config->SetString(CFGKEY_COMMON, CFG_SavePath, savePath);
	_config->SetString(CFGKEY_COMMON, CFG_FBSFilesPath, fbsPath);

	// 开始转换
	_btnGenerate.SetWindowText(L"正在转换...");
	CString param;
	for each (const auto & filePath in fbsFiles)
	{
		// 生成FlatBuffers类文件
		param.Format(L"-o \"%s\" %s %s \"%s\"", savePath, otherParams, compileLang, filePath);

		if (!RunFlatc(flatcPath, param))
		{
			_btnGenerate.SetWindowText(L"开始转换");
			AppendMsg(L"转换失败");
			return;
		}
	}

	_btnGenerate.SetWindowText(L"开始转换");
	AppendMsg(L"转换完成");
	MessageBox(L"转换完成");
}

void CFBSCompileToolDlg::AppendMsg(const WCHAR* msg)
{
	WCHAR* tmpMsg = new WCHAR[wcslen(msg) + 1];
	memset(tmpMsg, 0, sizeof(WCHAR) * (wcslen(msg) + 1));
	wsprintf(tmpMsg, msg);

	TheadFunc* pFunc = new TheadFunc;
	pFunc->Func = ([=]()
		{
			if (_editRecv.GetLineCount() > 100)
			{
				_editRecv.Clear();
			}

			CString curMsg;
			_editRecv.GetWindowTextW(curMsg);
			curMsg += "\r\n";

			CString strTime;
			SYSTEMTIME   tSysTime;
			GetLocalTime(&tSysTime);
			strTime.Format(L"[%02ld:%02ld:%02ld.%03ld] ",
				tSysTime.wHour, tSysTime.wMinute, tSysTime.wSecond, tSysTime.wMilliseconds);

			curMsg += strTime;
			curMsg += tmpMsg;
			_editRecv.SetWindowTextW(curMsg);
			_editRecv.LineScroll(_editRecv.GetLineCount());

			delete[] tmpMsg;
		});

	PostMessage(WMSG_FUNCTION, (WPARAM)pFunc);
}

LRESULT CFBSCompileToolDlg::OnFunction(WPARAM wParam, LPARAM lParam)
{
	TheadFunc* pFunc = (TheadFunc*)wParam;
	pFunc->Func();
	delete pFunc;

	return TRUE;
}

bool CFBSCompileToolDlg::LoadFlatcVersion(const CString& flatcPath)
{
	PROCESS_INFORMATION ProceInfo;
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	HANDLE hRead = NULL;
	HANDLE hWrite = NULL;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return false;
	}

	STARTUPINFO StartInfo;
	ZeroMemory(&StartInfo, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);
	StartInfo.wShowWindow = SW_HIDE;
	//使用指定的句柄作为标准输入输出的文件句柄,使用指定的显示方式
	StartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	StartInfo.hStdError = hWrite;
	StartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	StartInfo.hStdOutput = hWrite;

	bool ret = true;
	CString param;
	param.Format(L"%s --version", flatcPath);
	if (CreateProcess(nullptr, param.GetBuffer(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &StartInfo, &ProceInfo))
	{
		WaitForSingleObject(ProceInfo.hProcess, /*INFINITE*/3000); // 等待3秒

		CHAR chBuf[1024] = { 0 };
		DWORD dwRead = 0;
		DWORD dwAvail = 0;
		if (PeekNamedPipe(hRead, NULL, NULL, &dwRead, &dwAvail, NULL) && dwAvail > 0)//PeekNamePipe用来预览一个管道中的数据，用来判断管道中是否为空
		{
			BOOL bSuccess = ReadFile(hRead, chBuf, 1024, &dwRead, NULL); // 这里是读管道，即便已经没有数据，仍然会等待接收数据，所以才需要PeekNamePipe
			if (bSuccess && dwRead != 0)
			{
				// 检测是否有错误消息
				CString strRet(chBuf);
				if (strRet.Find(L"error") != -1)
				{
					ret = false;
					strRet.Format(L"读取flatc版本号错误：%s", CString(chBuf));
					AppendMsg(strRet);
				}
				else
				{
					AppendMsg(CString(chBuf));
				}
			}
		}

		CloseHandle(ProceInfo.hThread);
		CloseHandle(ProceInfo.hProcess);
	}
	else
	{
		ret = false;
	}

	CloseHandle(hRead);
	CloseHandle(hWrite);

	return ret;
}

void CFBSCompileToolDlg::OnCbnSelchangeComboCompileLang()
{
	CString strCompileLang;
	_comboCompileLang.GetWindowText(strCompileLang);
	if (strCompileLang == L"lua")
	{
		AppendMsg(L"注意：生成的lua文件存放于本软件所在文件夹下！");
	}
}
