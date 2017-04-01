#include "WebPage.h"
#include "WebPageCtrl.h"
#include "BrowserCtrl.h"
#include <HtmlCtrl/ElScript.h>


NAMESPACE_HTMLCTRL


VectorMap<int64, WebPage*> web_pages;

void scWebPagePrint( ExeVar& ret, ExeVar& c, ExeThread* data ) {
	WebPage* page = web_pages.Get((int64)data);
	MemVar line = c.mem->FindOuterKeyOrCreate ( "line" );
	BrowserCtrl* bc = page->GetParentClass<BrowserCtrl>();
	if (!bc) return;
	WebTools* wt = bc->FindNode<WebTools>();
	if (!wt) return;
	Console* cons = wt->FindNode<Console>();
	cons->Print(line->GetString());
}

void scWebPageGetDocumentCtrl( ExeVar& ret, ExeVar& c, ExeThread* data ) {
	WebPage* page = web_pages.Get((int64)data);
	WebPageCtrl* pc = page->GetParentClass<WebPageCtrl>();
	if (!pc) return;
	ret = page->GetScriptThread().NewObjectWithoutCtor("ParentCtrl");
	ASSERT(!ret.mem->GetDataClass().IsWrap()); // should be uninitialized (but it was the old version)
	ret.mem->GetDataClass().SetUnsafePtr<ParentCtrl>(pc->GetGuiCanvas()); // initialize, set ptr
}

WebPage::WebPage(HtmlNode& doc) : HtmlCtrl(doc) {
	exiting = false;
	vm_has_init = false;
	
	m_refCount		= 1;
	m_http.WhenFinish = THISBACK(OnFinish);
	
	needs_redraw = 0;
	
	script_ctx = new RefContext();
	script_vm = new ExeThread(script_ctx);
	
	web_pages.GetAdd((int64)&*script_vm, this); // link vm to this webpage (slow but easy)
	
	InitScriptVm();
}

WebPage::~WebPage()
{
	exiting = true;
	//Clear();
	/*m_http.stop();
	DeinitScriptVm();
	m_doc.Clear();*/
}

void WebPage::Clear() {
	m_http.stop();
	m_doc.Clear();
	
	if (vm_has_init) {
		DeinitScriptVm();
	}
}

void WebPage::InitScriptVm() {
	RegisterFunctions(script_vm);
	RegisterMathFunctions(script_vm);
	RegisterCtrlLibFunctions(script_vm);
	RegisterWebPageScript(script_vm);
}

void WebPage::RegisterWebPageScript(ExeThreadVar thrd) {
	thrd->AddNative ( "def Print(line)", scWebPagePrint);
	thrd->AddNative ( "def GetDocumentCtrl()", scWebPageGetDocumentCtrl);
}

void WebPage::DeinitScriptVm() {
	script_vm.Clear();
	vm_has_init = false;
}

void WebPage::ExecuteScripts() {
	if (!script_elements.GetCount())
		return;
	
	if (!vm_has_init) {
		InitScriptVm();
		vm_has_init = true;
	}
	
	for(int i = 0; i < script_elements.GetCount(); i++) {
		try {
			ElScript& es = *script_elements[i];
			String script = es.GetContent();
			LOG(script);
			
			ParserVar parser = new Parser(script_ctx);
			parser->SetThread(script_vm);
			NodeVar n = parser->Parse(script);
			if (!n.Is()) {
				LOG("Parsing failed");
				break;
			}
			//LOG(n->AsString("app"));
			ExeVar app_var(n, script_vm->NewMemory());
			ExeVar ret_var;
			script_vm->Execute(ret_var, app_var);
			
			LOG(" --- script success");
		}
		catch (ScriptException e) {
			LOG("  Error: ScriptException: " << (String)e);
		}
		catch (...) {
			
		}
	}
}


void WebPage::Load(String url)
{
	if (url == "about:")
		url = GetHomeDirFile(".thebrowser" DIR_SEPS "about.html");
	
	SetUrl(url);
	
	
	LOG("Load: " << url << " (parsed base path: " << GetBaseUrl() << ")");
	
	//if (PathIsURL(url))
	if (GetProtocol().GetCount())
	{
		Cache& cache = GetCache();
		int cache_id = cache.FindCacheObject(url);
		if (cache_id != -1) {
			CacheObject& co = cache.GetCacheObject(cache_id);
			//if (LoadCacheFile(co)) {
			OnDocumentLoaded(url, "UTF-8");
			return;
			//}
		}
		String proto = GetProtocol();
		if (proto == "http" || proto == "https") {
			m_http.DownloadFile( url, WebFileDocument );
		}
		else if (proto == "file") {
			CacheString& cs = cache.GetAdd<CacheString>(url);
			if (url.Left(7) == "file://") url = url.Mid(7);
			if (FileExists(url))
				cs.SetData(LoadFile(url));
			OnDocumentLoaded(url, "UTF-8");
		}
	} else
	{
		OnDocumentLoaded(url, "UTF-8");
	}
}

// Sends signal to WebPageCtrl that the download is ready
// TODO: clean realUrl... it is from litehtml and this is not using it directly
void WebPage::OnDocumentLoaded(const String& url, const String& encoding)
{
	
	LOG("OnDocumentLoaded url=\"" << url << "\"");
	
	if (url.GetCount() == 0) {
		Panic("wtf");
	}
	
	String html_text;
	Cache& cache = GetCache();
	int cache_id = cache.FindCacheObject(url);
	ASSERT(cache_id != -1);
	CacheObject& co = cache.GetCacheObject(cache_id);
	ASSERT(co.Is<CacheString>());
	
	html_text = co.Get<CacheString>().GetData();
	
	ASSERT(html_text.GetCount());
	LOG("html_text size " << html_text.GetCount());
	
	BrowserCtrl* bc = HtmlNode::GetParent()->GetParent()->Get<BrowserCtrl>();
	ASSERT(bc);
	Context* context =  bc->FindNode<Context>();
	ASSERT(context);
	
	script_elements.Clear();
	//m_text.Clear();
	m_doc = CreateFromString(html_text, this, context);
	
	if (exiting || Thread::IsShutdownThreads())
		return;
	
	ExecuteScripts();
	
	
	WhenPageReady();
}

void WebPage::LoadImage(const String& src, const String& baseurl_, bool redraw_on_ready) {
	LOG("LoadImage " << src << ", " << baseurl_ << ", " << (int) redraw_on_ready);
	String url;
	String baseurl = baseurl_;
	if (baseurl.GetCount() == 0)
		baseurl = GetBaseUrl();
	MakeUrl(src, baseurl, url);
	
	Cache& cache = GetCache();
	int i = cache.FindCacheObject(url);
	
	// Image is not cached
	if(i == -1)
	{
		GetImage(url, redraw_on_ready);
		
	}
	else {
		CacheObject& co = cache.GetCacheObject(i);
		if (co.Is<CacheImage>()) {
			LOG("LoadImage cache ok");
			// Image is cached
			if (redraw_on_ready)
				NeedsRedraw();
		} 
		else {
			LOG("LoadImage cache object found, but cant get image raster");
			GetImage(url, redraw_on_ready);
		}
	}
}

void WebPage::NeedsRedraw() {
	//needs_redraw = 1;
	SetNeedsRedraw(1);
	WhenRedraw();
}

void WebPage::SetCaption( const String& caption )
{
	WhenTitleChange(caption);
}




void WebPage::Link( Document* doc, Element::ptr el )
{
	String rel, media, href;
	rel = el->GetAttr("rel");
	media = el->GetAttr("media");
	href = el->GetAttr("href");
	LOG("Link rel=" << rel << " media=" << media << " href=" << href);
	
}

void WebPage::ImportCss( String& text, const String& url, String& baseurl )
{
	LOG("ImportCss " << text << " " << url << " " << baseurl);
	
}

void WebPage::OnAnchorClick( const String& url, Element::ptr el )
{
	String anchor;
	
	WebPageCtrl* wpc = HtmlNode::GetParent()->Get<WebPageCtrl>();
	if (wpc) {
		String open_url;
		LOG("OnAnchorClick " << url);
		if (PathIsRelative(url))
			UrlCombine(GetBaseUrl(), url, open_url);
		else open_url = url;
		wpc->Open(open_url);
	}
	
}

void WebPage::SetCursor( const String& cursor )
{
	
	m_cursor = cursor;
}

const Image* WebPage::GetCachedImage(int cache_id) {
	if (cache_id == -1) 
		return 0;

	CacheObject& co = GetCache().GetCacheObject(cache_id);
	
	if (co.Is<CacheImage>()) {
		return co.Get<CacheImage>();
	}
	if (co.Is<CacheString>()) {
		
	}
	return 0;
}

void WebPage::GetImage(const String& url, bool redraw_on_ready)
{
	LOG("GetImage " << url << " " << redraw_on_ready);
	if(PathIsURL(url))
	{
		if(redraw_on_ready)
		{
			m_http.DownloadFile( url, WebFileImageRedraw);
		} else
		{
			m_http.DownloadFile( url, WebFileImageRenderer);
		}
	} else {
		Panic("todo");
		// Cache local file
		
	}
}




void WebPage::OnDocumentError(dword dwError, const String& errMsg)
{
	
}

void WebPage::OnImageLoaded(const String& url, bool redraw_only )
{
	NeedsRedraw();
}

bool WebPage::DownloadAndWait( String& url )
{

}

void WebPage::OnWaitedFinished( dword dwError, const String& file )
{

}

void WebPage::GetClientRect( Position& client )
{
	HtmlNode::GetParent()->Get<WebPageCtrl>()->GetClientRect(client);
}

void WebPage::AddRef()
{
	
}

void WebPage::Release()
{

}

void WebPage::GetUrl( String& url )
{
	LOG("GetUrl " << url);
}

unsigned char* WebPage::LoadUtf8File( String& path, bool is_html, const String& defEncoding )
{
	LOG("LoadUtf8File " << path);
}


// When NetworkRequest or inherited download is finished, it returns here
// Functions are moved here from WebFile::OnFinish because calls switched on WebPage
void WebPage::OnFinish(WebFile& wf) {
	int err = wf.IsError();
	const String& url = wf.GetUrl();
	
	if(err)
	{
		switch(wf.GetType())
		{
		case WebFileDocument:
			OnDocumentError(err, wf.GetErrorMessage());
			break;
		case WebFileWaited:
			OnWaitedFinished(err, url);
			break;
		}
	}
	else
	{
		Cache& cache = GetCache();
		int type = wf.GetType();
		if (type == WebFileDocument) {
			CacheString& co = cache.GetAdd<CacheString>(url);
			co.SetData(wf.GetContentStringStream());
			co.StoreThis();
			OnDocumentLoaded(url, "UTF-8");
		}
		else if (type == WebFileImageRedraw || type == WebFileImageRenderer) {
			LOG("WebPage::OnFinish url=" << url);
			CacheImage& co = cache.GetAdd<CacheImage>(url);
			co.SetData(wf.GetContentStringStream());
			co.StoreThis();
			OnImageLoaded(url, type == WebFileImageRedraw);
		}
		else if (type == WebFileWaited) {
			OnWaitedFinished(err, url);
		}
	}
}




END_HTMLCTRL_NAMESPACE
