#include "BrowserCtrl.h"
#include "WebPageCtrl.h"

#include <plugin/bz2/bz2.h>
#include "BrowserCtrl.brc"

void BrowserCtrl_UnpackContent() {
	
	String dir_path = GetHomeDirFile(".thebrowser");
	RealizeDirectory(dir_path);
	
	for(int i = 0; i < browserctrl_css_all_count; i++) {
		MemReadStream file(browserctrl_css_all[i], browserctrl_css_all_length[i]);
		String file_content = BZ2Decompress(file);
		
		String file_path = AppendFileName(dir_path, browserctrl_css_all_files[i]);
		
		FileOut out(file_path);
		out << file_content;
		
		LOG("BrowserCtrl_UnpackContent wrote file: " << file_path << ", " << file_content.GetCount() << " bytes");
	}
	LOG("BrowserCtrl_UnpackContent wrote " << browserctrl_css_all_count << " files.");
}


NAMESPACE_HTMLCTRL


BrowserCtrl::BrowserCtrl(HtmlNode& f) : 
	HtmlNode(f),
	m_browser_context(*this), 
	m_view(*this), 
	wtools(*this)
{
	
	// load master.css
	
	Ctrl::Add(split.VSizePos().HSizePos());
	
	split.Set(webview, wtools);
	split.Horz();
	split.Zoom(0);
	
	String mcss = LoadFile(GetHomeDirFile(".thebrowser" DIR_SEPS "master.css"));
	m_browser_context.LoadMasterStylesheet(mcss);
	
	webview.Add(m_view.SizePos());
	webview.Vert();
	webview.SetPos(1000);
	
	m_view.WhenOpenPageChanged << THISBACK(OpenPageChanged);
	
	wtools.GetInspector().WhenSelection = callback(&m_view, &WebPageCtrl::InspectorSelection);// THISBACK(InspectorSelection);
}

BrowserCtrl::~BrowserCtrl(){}









void BrowserCtrl::Open( const String& path )
{
	m_view.Open(path, true);
}

void BrowserCtrl::Back()
{
	m_view.Back();
}

void BrowserCtrl::Forward()
{
	m_view.Forward();
}

void BrowserCtrl::Reload()
{
	m_view.Refresh();
}

/*void BrowserCtrl::CalcTime()
{
	//m_view.Render(true);
}*/

void BrowserCtrl::PageLoaded(String& url)
{
	
}

void BrowserCtrl::OpenPageChanged() {
	wtools.OpenPageChanged();
	WhenOpenPageChanged();
}


END_HTMLCTRL_NAMESPACE
