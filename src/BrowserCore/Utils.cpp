#include "Utils.h"

void MakeUrl(const String& url, const String& basepath, String& out )
{
	// TODO: make a real solution
	
	if(PathIsRelative(url) && !PathIsURL(url))
	{
		int res = UrlCombine(basepath, url, out);
		ASSERT(!res);
	} else
	{
		if(PathIsURL(url))
		{
			out = url;
		} else
		{
			int res = UrlCreateFromPath(url, out);
			ASSERT(!res);
		}
	}
	if(out.Mid(0, 8) == "file:///")
	{
		out = out.Mid(8);
	}
	else if(out.Mid(0, 7) == "file://")
	{
		out = out.Mid(7);
	}
	
}



// Very simple check
bool PathIsURL(const String& s) {
	// TODO: make a real solution
	
	if (s.GetCount() < 3) return 0;
	
	// Windows path C:
	if (IsAlpha(s[0]) && s[1] == ':') return 0;
	
	// POSIX path /
	if (s[0] == '/') return 0;
	if (s[0] == '.' && s[1] == '/') return 0;
	
	// Web protocols
	int a = s.Find(":");
	if (a != -1) {
		if ( a <= 8 && s.Mid(a, 3) == "://")
			return 1;
	}
	//if (s.Find("://") != -1) return 1;
	
	return 0;
}

bool PathIsAbsolute(const String& s) {
	// TODO: make a real solution
	
	if (s.GetCount() < 2) return 0;
	
	// Windows path C:
	if (IsAlpha(s[0]) && s[1] == ':') return 1;
	
	// POSIX path /
	//if (s[0] == '/') return 1;
	
	// Web path
	if (s.Find("://") != -1) return 1;
	
	return 0;
}

bool PathIsRelative(const String& s) {
	if (PathIsAbsolute(s)) return 0;
	return 1;
}

int UrlCombine(const String& basepath, const String& relpath, String& url) {
	// TODO: make a real solution
	if (basepath.GetCount() == 0) {
		Panic("no base path");
	}
	ASSERT(basepath.GetCount());
	ASSERT(relpath.GetCount());
	int c1 = basepath.GetCount();
	int c2 = relpath.GetCount();
	url.Clear();
	
	if (basepath[c1-1] == '/') url = basepath;
	else url = basepath + "/";
	
	if (relpath[0] == '/') url += relpath.Mid(1);
	url += relpath;
	
	return 0;
}


int UrlCreateFromPath(const String& filepath, String& out) {
	// TODO: make a real solution
	
	if (PathIsAbsolute(filepath)) {
		if (filepath.Left(7) != "file://")
			out = "file://" + filepath;
		else
			out = filepath;
	} else {
		int ret = UrlCombine("file://" + GetCurrentDirectory(), filepath, out);
		ASSERT(!ret);
	}
	return 0;
}

String GetUrlBasePath(String url) {
	// TODO: make a real solution
	
	if (PathIsAbsolute(url)) {
		int i = url.Find("://");
		if (i == -1) return url; // File path
		url = url.Mid(i+3);
	}
	
	int i = url.Find("/");
	if (i != -1) url = url.Left(i);
	
	return url;
}

String GetUrlName(const String& url) {
	// TODO: make a real solution
	
	int i = url.ReverseFind("/");
	if (i == -1) return url;
	if (i == url.GetCount() - 1) {
		i = url.ReverseFind("/", i-1);
	}
	return url.Mid(i+1);
}

String GetUrlProtocol(const String& url) {
	// TODO: make a real solution
	
	int i = url.Find("://");
	if (i == -1) return ""; // File path
	if (i > 8) return ""; // TODO: what is max length for protocol string? guessing 8
	return url.Left(i);
}

