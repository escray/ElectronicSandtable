
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "EST_MFC.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	//CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	if (!m_osg)
	{
		m_osg = new osgScene(m_hWnd);
		m_osg->initializeOsgScene();
	}
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnDestroy()
{
	m_osg->getViewer()->setDone(true);
	if (m_osg != 0)
	{
		delete m_osg;
	}
	CWnd::OnDestroy();
}

void CChildView::OnKeyDown( UINT nChar, UINT RepCnt, UINT nFlags )
{
	// 首先使用 OSG 处理键盘消息
	m_osg->getViewer()->getEventQueue()->keyPress(nChar);
	// 如果是退出键，则在传递到窗口
	if (nChar == VK_ESCAPE)
	{
		GetParent()->SendMessage(WM_CLOSE);
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CChildView::OnEraseBkgnd( CDC* pdc )
{
	return true;
	//return CWnd::OnEraseBkgnd(pDC);
}

