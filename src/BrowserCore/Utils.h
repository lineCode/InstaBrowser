#ifndef _BrowserCore_Utils_h_
#define _BrowserCore_Utils_h_

#include <Core/Core.h>
using namespace Upp;

void MakeUrl(const String& url, const String& basepath, String& out);

bool PathIsURL(const String& s);
bool PathIsAbsolute(const String& s);
bool PathIsRelative(const String& s);

int UrlCombine(const String& basepath, const String& relpath, String& url);
int UrlCreateFromPath(const String& filepath, String& out);

String GetUrlBasePath(String url);
String GetUrlName(const String& url);
String GetUrlProtocol(const String& url);

#endif
