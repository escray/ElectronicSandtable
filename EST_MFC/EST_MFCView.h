
// EST_MFCView.h : interface of the CEST_MFCView class
//

#pragma once
#include "ESTCoreOSG.h"

class CEST_MFCView : public CView
{
protected: // create from serialization only
	CEST_MFCView();
	DECLARE_DYNCREATE(CEST_MFCView)

// Attributes
public:
	CEST_MFCDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CEST_MFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	ESTCoreOSG* m_osg;
	HANDLE m_ThreadHandle;
	int m_filenum;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();

	afx_msg void OnEditPath();
	afx_msg void OnOutputPath();
	afx_msg void OnDisplayPath();
	afx_msg void OnStopDisplay();

	std::string pathName;
};

#ifndef _DEBUG  // debug version in EST_MFCView.cpp
inline CEST_MFCDoc* CEST_MFCView::GetDocument() const
   { return reinterpret_cast<CEST_MFCDoc*>(m_pDocument); }
#endif

