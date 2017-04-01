#ifndef _BrowserCore_NetClient_h_
#define _BrowserCore_NetClient_h_

#include "Utils.h"

class NetworkIO;
class WebFile;


class NetworkInstance : NoCopy {
	
	
public:
	typedef NetworkInstance CLASSNAME;
	NetworkInstance();
	~NetworkInstance();
	
	void Get(const String& url);
	
	Callback			WhenFinished;
	
	
	Thread				thrd;
	HttpRequest			http;
	StringStream		content;
	Mutex				lock;
	TimeStop			timer;
	
	uint64				loaded;
	String				url;
	bool				downloaded;
	
	dword				waiting, finished;
	
	void Execute();
	void Start();
	void Content(const void *ptr, int size);
	void Progress();
	
	bool IsDownloaded() {return downloaded;}
	bool IsIdle() {if (!thrd.IsOpen()) return false; return downloaded;}
	
	int GetTransferred() {return loaded;}
	int GetSize() {return -1;}
	int GetFirstPacketDelay() {return waiting;}
	int GetLastPacketDelay() {return finished;}
	
	
protected:
	
};

class NetworkRequest
{
	friend class NetworkIO;
	
	
public:
	
protected:
	int			m_hConnection;
	int			m_hRequest;
	int	m_sync;
	//NetworkIO*				m_http;
	byte				m_buffer[8192];
	dword				m_error;
	uint64				m_content_length;
	uint64				m_downloaded_length;
	dword				m_status;
	String				m_url;
	int					m_refCount;
	
	
	NetworkInstance* 	inst;
	
	int initial_delay;
	
public:
	typedef NetworkRequest CLASSNAME;
	NetworkRequest();
	virtual ~NetworkRequest();

	
	

	virtual void OnFinish(dword dwError, String& errMsg);
	virtual void OnData(byte* data, dword len, uint64 downloaded, uint64 total);
	virtual void OnHeadersReady(int hRequest);

	bool	Create(const String& url, int hSession);
	void	cancel();
	void	Lock();
	void	Unlock();
	uint64	GetContentLength();
	dword	GetStatusCode();
	void	AddRef();
	void	Release();
	
	
	Callback1<WebFile&> WhenFinish;
	
	const String& GetUrl() {return m_url;}
	
	String GetContent();
	StringStream& GetContentStringStream();
	
	bool IsIdle() {if (!inst) return true; return inst->IsIdle();}
	int GetInitialDelay() const {return initial_delay;}
	
protected:
	dword	OnSendRequestComplete();
	dword	OnHeadersAvailable();
	void	onHandleClosing();
	dword	onRequestError(dword dwError);
	dword	onReadComplete(dword len);
	dword	ReadData();
	void	SetParent(NetworkIO* parent);
};



inline dword NetworkRequest::GetStatusCode()
{
	return m_status;
}

inline void NetworkRequest::Lock()
{
	//EnterCriticalSection(&m_sync);
}

inline void NetworkRequest::Unlock()
{
	//LeaveCriticalSection(&m_sync);
}

inline uint64	NetworkRequest::GetContentLength()
{
	return m_content_length;
}




enum WebFileType
{
	WebFileDocument,
	WebFileImageRedraw,
	WebFileImageRenderer,
	WebFileWaited,
};

class WebFile : public NetworkRequest
{
	//String			m_file;
	//WebPage*		m_page;
	WebFileType		m_type;
	dword			m_hFile;
	void*			m_data;
	//String			m_real_url;
	//int				errorcode;
public:
	//WebFile(WebFileType type, void* data = NULL);
	WebFile(WebFileType type);
	virtual ~WebFile();

	virtual void OnFinish(dword dwError, String& errMsg);
	virtual void OnData(byte* data, dword len, uint64 downloaded, uint64 total);
	virtual void OnHeadersReady(int hRequest);

	
	int IsError() {return m_error;}
	int GetType() {return m_type;}
	//const String& GetFilePath() {return m_file;}
	String GetErrorMessage() {return "";}
	
	
	// For web tools: network
	String GetMethod() {return "<method>";}
	String GetFilename() {return GetUrlName(m_url);}
	String GetFileExt() {return ::GetFileExt(m_url);}
	String GetDomain() {return "<domain>";}
	int		GetTransferred() {if (!inst) return 0; return inst->GetTransferred();}
	int		GetSize() {if (!inst) return 0; return inst->GetSize();}
	//int		GetInitialDelay() {}
	int		GetFirstPacketDelay() {if (!inst) return 0; return inst->GetFirstPacketDelay();}
	int		GetLastPacketDelay() {if (!inst) return 0; return inst->GetLastPacketDelay();}
};
















class NetworkIO
{
	friend class NetworkRequest;

	int				m_hSession;
	
	int		m_sync;
	dword					m_maxConnectionsPerServer;
	
	//Array<NetworkRequest> requests;
	ArrayMap<String, WebFile> requests;
	
public:
	NetworkIO();
	virtual ~NetworkIO();

	void	SetMaxConnectionsPerServer(dword max_con);
	bool	Open(String& pwszUserAgent, dword dwAccessType, String& pwszProxyName, String& pwszProxyBypass);
	bool	DownloadFile(const String& url, WebFileType type);//, NetworkRequest* request);
	void	stop();
	void	close();

	void Lock();
	void Unlock();
	
	
	bool IsRequested(const String& s) {return requests.Find(s) != -1;}
	
	ArrayMap<String, WebFile>& GetRequests() {return requests;}
	String	GetContent(const String& url);
	bool	IsIdle();
	
	Callback1<WebFile&> WhenFinish;

private:
	//static void CALLBACK NetworkIOCallback(int hInternet, dword* dwContext, dword dwInternetStatus, void* lpvStatusInformation, dword dwStatusInformationLength);
	//void NetworkIOCallback(int hInternet, dword* dwContext, dword dwInternetStatus, void* lpvStatusInformation, dword dwStatusInformationLength);
	
protected:
	void RemoveRequest(NetworkRequest* request);
};




inline void NetworkIO::Lock()
{
	//EnterCriticalSection(&m_sync);
}

inline void NetworkIO::Unlock()
{
	//LeaveCriticalSection(&m_sync);
}

inline void NetworkIO::SetMaxConnectionsPerServer(dword max_con)
{
	m_maxConnectionsPerServer = max_con;
}




#endif
