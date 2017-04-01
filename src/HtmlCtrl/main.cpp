#ifdef flagHTMLCORE_TESTS

#include "HtmlCtrl.h"


CONSOLE_APP_MAIN {
	
	Vector<String> test_files;
	
	String dir = GetHomeDirFile("sppp-mirror") + DIR_SEPS "HtmlCtrl"  DIR_SEPS "tests";
	FindFile ff(AppendFileName(dir, "*.html"));
	while (ff) {
		if(ff.IsFile()) {
			test_files.Add(ff.GetPath());
		}
		ff.Next();
	}
	
	int failed = 0;
	for(int i = 0; i < test_files.GetCount(); i++) {
		LOG(i << ": Testing file: " << test_files[i]);
		Cout() << i << ": Testing file: " << test_files[i] << "\n";
		
		String addr = test_files[i];
		
		String html_text = LoadFile(addr);
		
		Nuller null;
		HtmlCtrl::HtmlNode node(null);
		HtmlCtrl::HtmlFileDummy html_file(node);
		HtmlCtrl::Context context(node);
		html_file.SetBaseUrl(GetFileDirectory(addr));
		html_file.SetUrl(addr);
		HtmlCtrl::Document::ptr m_doc = HtmlCtrl::CreateFromString(html_text, &html_file, &context);
		
	}
	
}

#endif
