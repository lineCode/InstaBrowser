#pragma once

#include <HtmlCtrl/HtmlCtrl.h>
#include <WrapLib/WrapLib.h>


NAMESPACE_HTMLCTRL

class WebPageCtrl;

class WebPage :	public HtmlCtrl
{
	int					m_refCount;
public:
	NetworkIO			m_http;
	Document::ptr		m_doc;
	String				m_caption;
	String				m_cursor;
	dword				m_hWaitDownload;
	String				m_waited_file;
	String				m_hash;
	
	bool exiting;
	bool needs_redraw;
	bool vm_has_init;
	
	Vector<ElScript*> script_elements;
	ExeThreadVar script_vm;
	RefCore::RefContextVar script_ctx;
	
public:
	typedef WebPage CLASSNAME;
	
	WebPage(HtmlNode& doc);
	virtual ~WebPage();

	void Clear();
	void Load(String url);
	void OnDocumentLoaded(const String& file, const String& encoding);
	void OnImageLoaded(const String& url, bool redraw_only);
	void OnDocumentError(dword dwError, const String& errMsg);
	void OnWaitedFinished(dword dwError, const String& file);
	void AddRef();
	void Release();
	void GetUrl(String& url);
	void InitScriptVm();
	void DeinitScriptVm();
	void ExecuteScripts();
	void RegisterWebPageScript(ExeThreadVar thrd);
	
	Document& GetDocument() {return *m_doc;}
	ExeThread& GetScriptThread() {return *script_vm;}
	
	// Document_container members
	virtual void		Script(ElScript* ptr) {script_elements.Add(ptr);}
	virtual void		LoadImage(const String& src, const String& baseurl, bool redraw_on_ready);
	
	virtual	void		SetCaption(const String& caption);
	virtual	void		Link(Document* doc, Element::ptr el);
	virtual void		ImportCss(String& text, const String& url, String& baseurl);
	virtual	void		OnAnchorClick(const String& url, Element::ptr el);
	virtual	void		SetCursor(const String& cursor);

	void				GetImage(const String& url, bool redraw_on_ready);
	virtual	const Image* GetCachedImage(int cache_id);
	virtual void		GetClientRect(Position& client);
	
	
	
	
	
	virtual void OnFinish(WebFile& wf);
	
	Callback WhenPageReady;
	Callback1<String> WhenTitleChange;
	Callback WhenRedraw;
	
	NetworkIO& GetNetworkIO() {return m_http;}
	
	
	void NeedsRedraw();
	void SetNeedsRedraw(bool b=true) {needs_redraw = b;}
	
	
	virtual WebPage* 			GetAsWebPage() 				{return this;}
	
private:
	unsigned char*		LoadUtf8File( String& path, bool is_html, const String& defEncoding = "UTF-8");
	bool				DownloadAndWait(String& url);
};



END_HTMLCTRL_NAMESPACE

