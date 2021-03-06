
// HartreeFockDoc.cpp : implementation of the CHartreeFockDoc class
//

#include "stdafx.h"

#include "MathUtils.h"

#include "GaussianOrbital.h"
#include "Molecule.h"
#include "IntegralsRepository.h"
#include "QuantumMatrix.h"

#include "BoysFunction.h"

#include "RestrictedHartreeFock.h"
#include "UnrestrictedHartreeFock.h"




// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "HartreeFock.h"
#endif

#include "HartreeFockDoc.h"
#include "HartreeFockView.h"




#include <algorithm>

#include <propkey.h>

//#include "Tests.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CHartreeFockDoc

IMPLEMENT_DYNCREATE(CHartreeFockDoc, CDocument)

BEGIN_MESSAGE_MAP(CHartreeFockDoc, CDocument)
	ON_COMMAND(ID_COMPUTATION_START, &CHartreeFockDoc::OnComputationStart)
	ON_UPDATE_COMMAND_UI(ID_COMPUTATION_START, &CHartreeFockDoc::OnUpdateComputationStart)
END_MESSAGE_MAP()


// CHartreeFockDoc construction/destruction

CHartreeFockDoc::CHartreeFockDoc()
	: convergenceProblem(false), runningThreads(0), atomsEnergy(0)
{
	m_Chart.title = L"Molecule Energy";
	m_Chart.XAxisLabel = L"Bond Length (�ngstr�ms)";
	m_Chart.YAxisLabel = L"Energy (eV)";
	m_Chart.useSpline = false;
}

CHartreeFockDoc::~CHartreeFockDoc()
{
	StopThreads(true);
}

BOOL CHartreeFockDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	options = theApp.options;

	SetChartBoundsAndTicks();

	basisSTO3G.Load("sto3g.txt");

	// Save is not used, it's implemented just for tests and maybe for some future usage
	//basisSTO3G.Save("sto3g_test.txt");

	basisSTO6G.Load("sto6g.txt");

#ifdef _DEBUG
//	Tests tests;
//	tests.Test(basisSTO3G);
#endif

	// Example for H2O and He:

		
	Systems::AtomWithShells H1, H2, O, N, C, He, Li, Ne, Ar;

	for (auto &atom : basisSTO6G.atoms)
	{
		if (atom.Z == 1) H1 = H2 = atom;
		else if (atom.Z == 2) He = atom;
		else if (atom.Z == 3) Li = atom;
		else if (atom.Z == 8) O = atom;
		else if (atom.Z == 6) C = atom;
		else if (atom.Z == 7) N = atom;
		else if (atom.Z == 10) Ne = atom;
		else if (atom.Z == 18) Ar = atom;
	}


	/*
	H1.position.X = H2.position.X = O.position.X = 0;

	H1.position.Y = 1.43233673;
	H1.position.Z = -0.96104039;
	H2.position.Y = -1.43233673;
	H2.position.Z = -0.96104039;

	O.position.Y = 0;
	O.position.Z = 0.24026010;

	Systems::Molecule H2O;

	H2O.atoms.push_back(H1);
	H2O.atoms.push_back(H2);
	H2O.atoms.push_back(O);

	H2O.Init();


	HartreeFock::RestrictedHartreeFock HartreeFockAlgorithm;
	HartreeFockAlgorithm.alpha = 0.5;
	HartreeFockAlgorithm.initGuess = 0;

	HartreeFockAlgorithm.Init(&H2O);
	double result = HartreeFockAlgorithm.Calculate();

	TRACE("H2O result: %f Hartree\n", result);

	Systems::Molecule Heatom;
	Heatom.atoms.push_back(He);
	Heatom.Init();
	
	HartreeFock::RestrictedHartreeFock HartreeFockAlgorithm;
	HartreeFockAlgorithm.alpha = 0.5;
	HartreeFockAlgorithm.initGuess = 0;

	HartreeFockAlgorithm.Init(&Heatom);
	double result = HartreeFockAlgorithm.Calculate();

	TRACE("He result: %f Hartree\n", result);
	
	Systems::Molecule Hatom;
	Hatom.atoms.push_back(H1);
	Hatom.Init();
	
	HartreeFock::UnrestrictedHartreeFock HartreeFockAlgorithm;
	HartreeFockAlgorithm.alpha = 0.5;
	HartreeFockAlgorithm.initGuess = 0;

	HartreeFockAlgorithm.Init(&Hatom);
	double result = HartreeFockAlgorithm.Calculate();

	CString str;
	str.Format(L"H energy: %f", result);
	AfxMessageBox(str);
	*/

	return TRUE;
}




// CHartreeFockDoc serialization

void CHartreeFockDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CHartreeFockDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CHartreeFockDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CHartreeFockDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CHartreeFockDoc diagnostics

#ifdef _DEBUG
void CHartreeFockDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CHartreeFockDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CHartreeFockDoc commands


void CHartreeFockDoc::OnComputationStart()
{
	if (isFinished()) StartThreads();
	else {
		CHartreeFockView *view = GetView();
		if (view) view->StopTimer();
		StopThreads(true);
	}
}


bool CHartreeFockDoc::isFinished()
{
	if (runningThreads) return false;
	
	return true;
}


void CHartreeFockDoc::StartThreads()
{
	options = theApp.options;

	SetTitle(L"Computing");


	unsigned int nrThreads = theApp.options.nrThreads;
	if (0 == nrThreads) options.nrThreads = nrThreads = 1;

	runningThreads = nrThreads;

	const double step = (options.XMaxBondLength - options.XMinBondLength) / options.numberOfPoints;
	const double interval = (options.XMaxBondLength - options.XMinBondLength) / nrThreads;

	double start = options.XMinBondLength;
	for (unsigned int i = 0; i < nrThreads; ++i)
	{
		threadsList.push_back(std::make_unique<HartreeFockThread>(options, this, start, start + interval + (i == nrThreads - 1 ? step : 0), step));

		if (0 == i)
			threadsList.back()->computeFirstAtom = true;

		if (nrThreads - 1 == i)
			threadsList.back()->computeSecondAtom = true;

		start += interval;

		threadsList.back()->Start();
	}

	GetView()->StartTimer();
}


void CHartreeFockDoc::StopThreads(bool cancel)
{
	for (auto &thrd : threadsList) thrd->Terminate();
	

	results.clear();
	
	convergenceProblem = false;


	atomsEnergy = 0;

	// now join them then get the data out of them
	for (auto &thrd : threadsList)
	{
		thrd->join();

		if (!cancel)
		{
			if (thrd->computeFirstAtom)
			{

				if (options.twoAtom1) atomsEnergy += thrd->firstAtomEnergy * 2;
				else atomsEnergy += thrd->firstAtomEnergy;
			}

			if (thrd->computeSecondAtom)
				atomsEnergy += thrd->secondAtomEnergy;

			results.insert(results.end(), thrd->results.begin(), thrd->results.end());
		}
		
		if (!thrd->Converged()) convergenceProblem = true;
	}

	threadsList.clear();

	SetChartData();

	if (cancel) SetTitle(L"Canceled");
	else SetTitle(L"Finished");

	CHartreeFockView* view = GetView();
	if (view) {
		view->StopTimer();
		view->Invalidate();
	}
}

CHartreeFockView* CHartreeFockDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	while (pos)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CHartreeFockView)))
			return dynamic_cast<CHartreeFockView*>(pView);
	}

	return nullptr;
}

void CHartreeFockDoc::OnUpdateComputationStart(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(runningThreads == 0 ? FALSE : TRUE);
}


void CHartreeFockDoc::SetChartBoundsAndTicks()
{
	m_Chart.useSpline = options.useSplines;

	m_Chart.XAxisMin = 0;
	m_Chart.XAxisMax = options.XMaxBondLength;

	m_Chart.YAxisMin = options.YMinEnergy;
	m_Chart.YAxisMax = options.YMaxEnergy;

	m_Chart.SetNumBigTicksX(options.XBigTicksBondLength);
	m_Chart.SetNumTicksX(options.XBigTicksBondLength * options.XSmallTicksBondLength);

	m_Chart.SetNumBigTicksY(options.YBigTicksEnergy);
	m_Chart.SetNumTicksY(options.YBigTicksEnergy * options.YSmallTicksEnergy);
}


void CHartreeFockDoc::ApplyChartOptions()
{
	options.useSplines = theApp.options.useSplines;
	options.XMaxBondLength = theApp.options.XMaxBondLength;
	options.YMinEnergy = theApp.options.YMinEnergy;
	options.YMaxEnergy = theApp.options.YMaxEnergy;
	options.XBigTicksBondLength = theApp.options.XBigTicksBondLength;
	options.XSmallTicksBondLength = theApp.options.XSmallTicksBondLength;
	options.YBigTicksEnergy = theApp.options.YBigTicksEnergy;
	options.YSmallTicksEnergy = theApp.options.YSmallTicksEnergy;

	options.DisplayHOMOEnergy = theApp.options.DisplayHOMOEnergy;

	SetChartData();

	CHartreeFockView* view = GetView();
	if (view) view->Invalidate();
}


void CHartreeFockDoc::SetChartData()
{
	if (results.size())
	{
		m_Chart.clear();

		SetChartBoundsAndTicks();
		
	    std::vector<std::pair<double, double>> chartData;

		for (const auto& val : results)
		{
			if (2 == options.DisplayHOMOEnergy)	chartData.push_back(std::make_pair(std::get<0>(val), atomsEnergy - std::get<1>(val)));
			else chartData.push_back(std::make_pair(std::get<0>(val), 0 == options.DisplayHOMOEnergy ? std::get<1>(val) : std::get<2>(val)));
		}

		m_Chart.AddDataSet(&chartData, 2, RGB(255,0,0));

		CString title;
		if (options.twoAtom1)
		{
			if (options.m_atom1 == options.m_atom2)
			{
				title = options.m_atom1 + L"3";
			}
			else if (options.m_atom1 == L"O")
			{
				title = options.m_atom2 + options.m_atom1 + L"2";
			}
			else
			{
				title = options.m_atom1 + L"2" + options.m_atom2;
			}
		}
		else
		{
			if (options.m_atom1 == options.m_atom2)
				title = options.m_atom1 + L"2";
			else 
				title = options.m_atom1 + options.m_atom2;
		}

		if (0 == options.DisplayHOMOEnergy)
			title += L" Molecule Energy";
		else if (1 == options.DisplayHOMOEnergy)
			title += L" HOMO Energy";
		else 
			title += L" Binding Energy";

		if (convergenceProblem) title += L" (convergence issues)";

		m_Chart.title = title;
	}
}
