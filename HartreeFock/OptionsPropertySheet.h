#pragma once

#include "ChartPropertyPage.h"
#include "HartreeFockPropertyPage.h"
#include "ComputationPropertyPage.h"
#include "MoleculePropertyPage.h"


// OptionsPropertySheet

class COptionsPropertySheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(COptionsPropertySheet)

public:
	COptionsPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionsPropertySheet();

protected:
	HICON hIcon;

	MoleculePropertyPage page1;
	HartreeFockPropertyPage page2;
	ComputationPropertyPage page3;
	ChartPropertyPage page4;

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	void AddPages();
};


