#ifndef _InstaBrowser_InstaBrowser_h_
#define _InstaBrowser_InstaBrowser_h_

#include <CtrlUtils/CtrlUtils.h>
#include <BrowserCtrl/BrowserCtrl.h>
using namespace HtmlCtrl;

#include "BookmarkBar.h"

#define IMAGECLASS InstaBrowserImgs
#define IMAGEFILE <InstaBrowser/InstaBrowser.iml>
#include <Draw/iml_header.h>

class InstaBrowser : public TopWindow, public HtmlNode {
	MenuBar menu;
	ToolBar nav;
	BookmarkBar bmark;
	TabCtrl tabs;
	SuggestCtrl addr;
	
	Array<BrowserCtrl> browser_ctrls;
	
public:
	typedef InstaBrowser CLASSNAME;
	InstaBrowser();
	
	void SetNavigator(Bar& bar);
	void PopupMainMenu();
	void Previous();
	void Next();
	void Refresh();
	void GoToAddress();
	
	void MainMenu(Bar& bar);
	void NewWindow();
	void NewTab();
	void History();
	void FullScreen();
	void Preferences();
	void Developer();
	
	
	void DoClose() {Close();}
	void PostClose() {PostCallback(THISBACK(DoClose));}
	
	void TitleChange(String title, BrowserCtrl* bc);
	void OpenPageChange(BrowserCtrl* bc);
	
};

#endif
