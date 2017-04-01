#include "InstaBrowser.h"

#define IMAGECLASS InstaBrowserImgs
#define IMAGEFILE <InstaBrowser/InstaBrowser.iml>
#include <Draw/iml_source.h>


InstaBrowser::InstaBrowser() : HtmlNode(Nuller()) {
	Title("BeBrowser");
	MaximizeBox();
	MinimizeBox();
	Sizeable();
	
	Icon(InstaBrowserImgs::icon());
	
	AddFrame(menu);
	AddFrame(nav);
	AddFrame(bmark);
	Add(tabs.SizePos());
	
	//String testdir = GetHomeDirectory() + DIR_SEPS "sppp-mirror" DIR_SEPS "BrowserCtrl" DIR_SEPS "tests";
	//String testfile = testdir + DIR_SEPS "page001.html";
	
	nav.Set(THISBACK(SetNavigator));
	addr.SetData("http://ultimatepp.org/");
	addr.WhenEnter = THISBACK(GoToAddress);
	
	//NewTab();
	
	PostCallback(THISBACK(NewTab));
	PostCallback(THISBACK(GoToAddress));
	
}


void InstaBrowser::SetNavigator(Bar& bar) {
	bar.Add(Images::view_list_details_symbolic(), THISBACK(PopupMainMenu));
	bar.Separator();
	bar.Add(Images::go_previous(), THISBACK(Previous));
	bar.Add(Images::go_next(), THISBACK(Next));
	bar.Add(Images::view_refresh(), THISBACK(Refresh));
	bar.Separator();
	bar.AddTool(addr, 640);
	
}

void InstaBrowser::Previous() {
	
}

void InstaBrowser::Next() {
	
}

void InstaBrowser::Refresh() {
	
}

void InstaBrowser::GoToAddress() {
	String url = addr.GetData();
	int i = tabs.Get();
	if (i < 0 || i >= browser_ctrls.GetCount()) return;
	BrowserCtrl& bc = browser_ctrls[i];
	bc.Open(url);
	
	//bc.WhenOpenPageChanged = THISBACK(PostClose);
}


void InstaBrowser::PopupMainMenu() {
	MenuBar::Execute(THISBACK(MainMenu));
}

void InstaBrowser::MainMenu(Bar& bar) {
	bar.Add("New Window", THISBACK(NewWindow));
	bar.Add("New Tab", THISBACK(NewTab)).Key(K_CTRL|K_T);
	bar.Separator();
	bar.Add("History", THISBACK(History));
	bar.Add("Full Screen", THISBACK(FullScreen)).Key(K_F11);
	bar.Add("Preferences", THISBACK(Preferences));
	bar.Separator();
	bar.Add("Composer", THISBACK(Developer));
	bar.Separator();
	bar.Add("Quit", THISBACK(PostClose));
}

void InstaBrowser::NewWindow() {
	
}

void InstaBrowser::NewTab() {
	BrowserCtrl& bw = browser_ctrls.Add(new BrowserCtrl(*this));
	bw.GetWebPageCtrl().WhenTitleChange = THISBACK1(TitleChange, &bw);
	bw.GetWebPageCtrl().WhenOpenPageChanged = THISBACK1(OpenPageChange, &bw);
	tabs.Add(bw);
	tabs.Add(bw, "Unnamed");
}

void InstaBrowser::History() {
	
}

void InstaBrowser::FullScreen() {
	TopWindow::FullScreen(!IsFullScreen());
}

void InstaBrowser::Preferences() {
	
}

void InstaBrowser::Developer() {
	static LocalProcess lp;
	if (lp.IsRunning()) return;
	String cmd;
	cmd << GetExeFilePath() << " -c";
	lp.Start(cmd);
}

void InstaBrowser::TitleChange(String title, BrowserCtrl* bc) {
	int tab = tabs.Get();
	BrowserCtrl& open_bw = browser_ctrls[tab];
	GuiLock __;
	if (&open_bw == bc) {
		Title(title);
	}
	tabs.GetItem(tab).Text(title.Left(32));
}

void InstaBrowser::OpenPageChange(BrowserCtrl* bc) {
	int tab = tabs.Get();
	BrowserCtrl& open_bw = browser_ctrls[tab];
	if (&open_bw == bc) {
		GuiLock __;
		addr.SetData(open_bw.GetWebPageCtrl().GetPage().HtmlPageNode::GetUrl());
		open_bw.SetFocus();
	}
}
