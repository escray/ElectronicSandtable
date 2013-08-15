
// ChildView.h : interface of the CChildView class
//


#pragma once


// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	osgScene* m_osg;
	HANDLE mThreadHandle;

	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pdc);
	afx_msg void OnKeyDown(UINT nChar, UINT RepCnt, UINT nFlags);
};

