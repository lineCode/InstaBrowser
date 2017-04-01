#pragma once

NAMESPACE_HTMLCTRL

class Utf8ToWchar
{
	//const byte* m_utf8;
	WString m_str;
public:
	Utf8ToWchar(const char* val);
	operator const wchar*() const
	{
		return m_str;
	}
private:
/*	ucode_t GetB()
	{
		if (!(*m_utf8)) return 0;
		return *m_utf8++;
	}
	ucode_t GetNextUtf8(ucode_t val)
	{
		return (val & 0x3f);
	}
	ucode_t GetChar();*/
};

class WcharToUtf8
{
	String m_str;
public:
	WcharToUtf8(const wchar_t* val);
	WcharToUtf8(const String& val);
	
	operator String() const {
		return m_str.ToString();
	}
	operator const String&() const {
		return m_str;
	}
};


#define litehtml_from_utf8(str)		String(str)
#define litehtml_from_wchar(str)	String(str)
#define litehtml_to_utf8(str)		String(str)


END_HTMLCTRL_NAMESPACE
