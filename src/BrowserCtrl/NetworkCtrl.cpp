#include "NetworkCtrl.h"
#include "WebPageCtrl.h"

NAMESPACE_HTMLCTRL


NetworkCtrl::NetworkCtrl(HtmlNode& dir) : HtmlNode(dir) {
	
	Ctrl::Add(list.VSizePos().HSizePos());
	
	list.AddColumn("Method");
	list.AddColumn("File");
	list.AddColumn("Domain");
	list.AddColumn("Type");
	list.AddColumn("Transferred");
	list.AddColumn("Size");
	list.AddColumn("Begin");
	list.AddColumn("Duration");
	list.AddColumn("End");
	list.ColumnWidths("2 4 3 1 2 2 2 2 2");
	
	last_idle = 0;
}


void NetworkCtrl::RefreshData() {
	/*
	//Knot& head = *GetHead(0);
	
	//WebPageCtrl& wpc = head.GetWebPageCtrl();
	WebPageCtrl& wpc = *GetWebPageCtrl();
	
	// TODO: use another structure. Now hangs forever if problems in startup.
	GuiLock __;
	WebPage& wp = wpc.GetPage(0);
	
	NetworkIO& netio = wp.GetNetworkIO();
	
	// Is refresh really needed?
	if (netio.IsIdle() && last_idle) // && same ctrl / page
	{
		
	} else {
		for(int i = 0; i < netio.GetRequests().GetCount(); i++) {
			WebFile& wf = netio.GetRequests()[i];
			
			list.Set(i, 0, wf.GetMethod());
			list.Set(i, 1, wf.GetFilename());
			list.Set(i, 2, wf.GetDomain());
			list.Set(i, 3, wf.GetFileExt());
			list.Set(i, 4, wf.GetTransferred());
			list.Set(i, 5, wf.GetSize());
			
			int initial_delay = wf.GetInitialDelay();
			list.Set(i, 6, initial_delay);
			
			int first_packet = wf.GetFirstPacketDelay();
			if (first_packet >= 0)
				list.Set(i, 7, initial_delay + first_packet);
			
			int last_packet = wf.GetLastPacketDelay();
			if (last_packet >= 0)
				list.Set(i, 8, initial_delay + last_packet);
			
		}
		
		last_idle = netio.IsIdle();
	}
	*/
}

END_HTMLCTRL_NAMESPACE
