
// EST_MFCDoc.h : interface of the CEST_MFCDoc class
//


#pragma once


class CEST_MFCDoc : public CDocument
{
protected: // create from serialization only
	CEST_MFCDoc();
	DECLARE_DYNCREATE(CEST_MFCDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CEST_MFCDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString m_csFileName;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	CString GetFileName() const { return m_csFileName; }

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
