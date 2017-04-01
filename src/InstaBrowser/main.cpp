#include "InstaBrowser.h"

GUI_APP_MAIN {
	ChStdSkin();
	InstallGrayBar();
	
	BrowserCtrl_UnpackContent();
	
	InstaBrowser main;
	main.Run();
}
