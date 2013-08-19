
// EST_MFCDoc.cpp : implementation of the CEST_MFCDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "EST_MFC.h"
#endif

#include "EST_MFCDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEST_MFCDoc

IMPLEMENT_DYNCREATE(CEST_MFCDoc, CDocument)

BEGIN_MESSAGE_MAP(CEST_MFCDoc, CDocument)
END_MESSAGE_MAP()


// CEST_MFCDoc construction/destruction

CEST_MFCDoc::CEST_MFCDoc()
{
	// TODO: add one-time construction code here
	// m_csFileName = "bh_11.ive";
	m_csFileName = "1.ive";
}

CEST_MFCDoc::~CEST_MFCDoc()
{
}

BOOL CEST_MFCDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CEST_MFCDoc serialization

void CEST_MFCDoc::Serialize(CArchive& ar)
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
void CEST_MFCDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
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
void CEST_MFCDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CEST_MFCDoc::SetSearchContent(const CString& value)
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

// CEST_MFCDoc diagnostics

#ifdef _DEBUG
void CEST_MFCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEST_MFCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

BOOL CEST_MFCDoc::OnOpenDocument( LPCTSTR lpszPathName )
{
	m_csFileName = lpszPathName;
	if (!CDocument::OnOpenDocument(lpszPathName))
	{
		return FALSE;
	}
	return TRUE;
	
}

#endif //_DEBUG


// CEST_MFCDoc commands
