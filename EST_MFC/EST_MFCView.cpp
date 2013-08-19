
// EST_MFCView.cpp : implementation of the CEST_MFCView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "EST_MFC.h"
#endif

#include "EST_MFCDoc.h"
#include "EST_MFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEST_MFCView

IMPLEMENT_DYNCREATE(CEST_MFCView, CView)

BEGIN_MESSAGE_MAP(CEST_MFCView, CView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CEST_MFCView construction/destruction

CEST_MFCView::CEST_MFCView() : m_osg(0L)
{
	// TODO: add construction code here
	m_filenum = 1;
}

CEST_MFCView::~CEST_MFCView()
{
}

BOOL CEST_MFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CEST_MFCView drawing

void CEST_MFCView::OnDraw(CDC* /*pDC*/)
{
	CEST_MFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CEST_MFCView diagnostics

#ifdef _DEBUG
void CEST_MFCView::AssertValid() const
{
	CView::AssertValid();
}

void CEST_MFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEST_MFCDoc* CEST_MFCView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEST_MFCDoc)));
	return (CEST_MFCDoc*)m_pDocument;
}

#endif //_DEBUG

void CEST_MFCView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	
	CString csFileName = GetDocument()->GetFileName();

	if (m_filenum == 1)
	{
		m_osg->InitOSG(csFileName.GetString());
		m_ThreadHandle = (HANDLE)_beginthread(&ESTCoreOSG::Render, 0, m_osg);
	}
	else
	{
		m_osg->ReplaceSceneData(csFileName.GetString());
	}
	m_filenum++;
}

int CEST_MFCView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CView::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	m_osg = new ESTCoreOSG(m_hWnd);
	return 1;
}

BOOL CEST_MFCView::OnEraseBkgnd( CDC* pDC )
{
	return CView::OnEraseBkgnd(pDC);
}

void CEST_MFCView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{

}

void CEST_MFCView::OnDestroy()
{
	if (m_osg != 0)
	{
		delete m_osg;
	}
	CView::OnDestroy();
}




// CEST_MFCView message handlers