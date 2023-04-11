#pragma once
#include "Common/ConfigFile.h"
#include <memory>
#include <functional>
using namespace std;

class CFBSCompileToolDlg : public CDialogEx
{
public:
	CFBSCompileToolDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum {
		IDD = IDD_FBSCOMPILETOOL_DIALOG
	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	struct TheadFunc
	{
		function<void()> Func;
	};
protected:
	HICON m_hIcon;
	CEdit _editFlatcPath;
	CComboBox _comboboxSaveType;
	CEdit _editSavePath;
	CComboBox _comboboxSelectType;
	CEdit _editFBSPath;
	CButton _btnSavePath;
	CButton _btnFBSPath;
	CEdit _editRecv;
	CEdit _editOtherParams;
	CComboBox _comboCompileLang;

	virtual BOOL OnInitDialog();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	shared_ptr<CConfigFile>	_config = nullptr;

	void LoadConfig();
	bool IsFBSFileHasService(const CString& fbsPath);
	bool RunFlatc(const CString& flatcPath, CString param);
	void AppendMsg(const WCHAR* msg);
	LRESULT OnFunction(WPARAM wParam, LPARAM lParam);
	bool LoadFlatcVersion(const CString& flatcPath);

	afx_msg void OnBtnFlatcPath();
	afx_msg void OnBtnSavePath();
	afx_msg void OnCbnSelchangeComboSaveType();
	afx_msg void OnCbnSelchangeComboSelectType();
	afx_msg void OnBtnFBSPath();
	afx_msg void OnBtnGenerate();
	afx_msg void OnCbnSelchangeComboCompileLang();
};
