#include "NetClient.h"



NetworkRequest::NetworkRequest()
{
	inst = 0;
	initial_delay = 0;
}

NetworkRequest::~NetworkRequest()
{
	
	cancel();
	
	if (inst) delete inst;
	
}

bool NetworkRequest::Create(const String& url, int hSession )
{
	
	LOG("Request url: " << url);
	
	ASSERT(PathIsAbsolute(url));
	
	m_url = url;
	
	// Start NetworkIORequest downloading
	ASSERT(!inst);
	
	//NetworkInstance& ins = inst.Add();
	inst = new NetworkInstance();
	
	inst->WhenFinished = THISBACK(onHandleClosing);
	
	inst->Get(url);
	
	
	
	// and when it finished call onHandleClosing
	//onHandleClosing();
	return true;
}

String NetworkRequest::GetContent() {
	if (!inst) 
		return "";
	ASSERT(inst->IsDownloaded());
	inst->lock.Enter();
	inst->content.Seek(0);
	String s(inst->content.Get(inst->content.GetSize()));
	inst->lock.Leave();
	return s;
}

StringStream& NetworkRequest::GetContentStringStream() {
	ASSERT(inst);
	ASSERT(inst->IsIdle());
	return inst->content;
}

NetworkInstance::NetworkInstance() {
	downloaded = 0;
}

NetworkInstance::~NetworkInstance() {
	thrd.Wait();
	
}

void NetworkInstance::Get(const String& url) {
	LOG("NetworkInstance::Get " << url);
	this->url = url;
	LOG("NetworkInstance::Get this=" << this->url);
	ASSERT(url == this->url);
	
	http.UserAgent("Mozilla/5.0 (X11)");
	
	
	http.Accept("image/png,image/*;q=0.8,*/*;q=0.5");
	http.Header("Accept-Encoding", "");
	http.KeepAlive();
	
	http.Trace();
	http.WhenContent = THISBACK(Content);
	http.WhenStart = THISBACK(Start);
	http.New();
	
	thrd.Run(THISBACK(Execute));
	
}

void NetworkInstance::Execute() {
	LOG("NetworkInstance::Execute " << url);
	
	ASSERT(url.GetCount());
	lock.Enter();
	timer.Reset();
	http.Url(url).Execute();
	lock.Leave();
	LOG("Finished");
	
	downloaded = 1;
	finished = timer.Elapsed();
	WhenFinished();
}

void NetworkInstance::Start() {
	content.SetSize(0);
	loaded = 0;
	finished = 0;
	waiting = 0;
	downloaded = 0;
}

void NetworkInstance::Content(const void *ptr, int size) {
	LOG("NetworkInstance::Content " << size);
	
	if (!loaded)
		waiting = timer.Elapsed();
	
	loaded += size;
	
	ASSERT(size != 0);
	content.Put(ptr, size);
	
}

void NetworkInstance::Progress() {
	double perc = double(loaded) / double(http.GetContentLength()) * 100.0;
	LOG("NetworkInstance::Progress: " << DblStr(perc) << "%");
	
}


void NetworkRequest::cancel()
{
	
}

dword NetworkRequest::OnSendRequestComplete()
{
	
}

dword NetworkRequest::OnHeadersAvailable()
{
	
}


// Called when NetworkIORequest download is ready.
// When file transfer (download) is ready, sends signal to route ready-signal according to file type
void NetworkRequest::onHandleClosing()
{
	
	if (!inst->IsDownloaded())
		m_error = 1;
	else
		m_error = 0;
	
	String errMsg;
	
	
	
	if (m_error) {
		
	}
	
	
	OnFinish(m_error, errMsg);
}

dword NetworkRequest::onRequestError(dword dwError)
{
	
}

dword NetworkRequest::ReadData()
{
	
}

dword NetworkRequest::onReadComplete(dword len)
{
	
}

void NetworkRequest::AddRef()
{
	
}

void NetworkRequest::Release()
{
	
}

void NetworkRequest::SetParent( NetworkIO* parent )
{
	
}

void NetworkRequest::OnFinish(dword dwError, String& errMsg) {}
void NetworkRequest::OnData(byte* data, dword len, uint64 downloaded, uint64 total) {}
	

void NetworkRequest::OnHeadersReady( int hRequest )
{

}









//WebFile::WebFile( WebPage* page, WebFileType type, void* data )
WebFile::WebFile( WebFileType type)//, void* data )
{
/*	m_data	= data;
	m_page	= page;
	m_type	= type;
	uint16 path[MAX_PATH];
	GetTempPath(MAX_PATH, path);
	GetTempFileName(path, L"lbr", 0, m_file);
	m_hFile = CreateFile(m_file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	m_page->AddRef();*/
	
	m_type	= type;
	
}

WebFile::~WebFile()
{
/*	if(m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
	if(m_type != WebFileWaited)
	{
		DeleteFile(m_file);
	}
	if(m_page)
	{
		m_page->Release();
	}*/
}

// Initiates signal of finished download. Switches between file types.
void WebFile::OnFinish( dword dwError, String& errMsg )
{
	/*if(m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}
	if(dwError)
	{
		String fileName = m_file;
		switch(m_type)
		{
		case WebFileDocument:
			m_page->OnDocumentError(dwError, errMsg);
			break;
		case WebFileWaited:
			m_page->OnWaitedFinished(dwError, m_file);
			break;
		}
	} else
	{
		switch(m_type)
		{
		case WebFileDocument:
			m_page->OnDocumentLoaded(m_file, L"UTF-8", m_real_url.IsEmpty() ? NULL : m_real_url.c_str());
			break;
		case WebFileImageRedraw:
			m_page->OnImageLoaded(m_file, m_url.c_str(), true);
			break;
		case WebFileImageRerender:
			m_page->OnImageLoaded(m_file, m_url.c_str(), false);
			break;
		case WebFileWaited:
			m_page->OnWaitedFinished(dwError, m_file);
			break;
		}		
	}*/
	
	//errorcode = dwError;
	
	// Switch moved to WebPage::OnFinish (because of m_page pointer)
	WhenFinish(*this);
}

void WebFile::OnData( byte* data, dword len, uint64 downloaded, uint64 total )
{
	/*if(m_hFile)
	{
		dword cbWritten = 0;
		WriteFile(m_hFile, data, len, &cbWritten, NULL);
	}*/
}

void WebFile::OnHeadersReady( int hRequest )
{
/*	uint16 buf[2048];
	dword len = sizeof(buf);
	if (WinNetworkIOQueryOption(m_hRequest, WINHTTP_OPTION_URL, buf, &len))
	{
		m_real_url = buf;
	}*/
}



















NetworkIO::NetworkIO()
{
	
}


NetworkIO::~NetworkIO()
{
	
}

bool NetworkIO::Open( String& pwszUserAgent, dword dwAccessType, String& pwszProxyName, String& pwszProxyBypass )
{
	
}

void NetworkIO::close()
{

}


bool NetworkIO::DownloadFile(const String& url, WebFileType type )
{
	
	if (IsRequested(url)) {
		LOG("NetworkIO::DownloadFile already requested: " << url);
		return true;
	}
	
	int id = requests.GetCount();
	WebFile& request = requests.Add(url, WebFile(type));
	
	request.WhenFinish = Proxy(WhenFinish); // from original code
	
	if (request.Create(url, m_hSession)) {
		return true;
	}
	requests.Remove(id);
	return false;
}

String NetworkIO::GetContent(const String& url) {
	int i = requests.Find(url);
	if (i == -1)	
		return "";
	return requests[i].GetContent();
}

bool NetworkIO::IsIdle() {
	for(int i = 0; i < requests.GetCount(); i++) {
		if (!requests[i].IsIdle()) return false;
	}
	return true;
}

void NetworkIO::RemoveRequest( NetworkRequest* request )
{

}

void NetworkIO::stop()
{
	
}