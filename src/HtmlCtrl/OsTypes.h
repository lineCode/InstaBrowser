#pragma once

NAMESPACE_HTMLCTRL

#if defined( WIN32 ) || defined( WINCE )
/*
#ifndef LITEHTML_UTF8

	typedef String	String;
	typedef wchar_t			tchar_t;

	#define _t(quote)			L##quote

	#define t_StringLength			wcslen
	#define StringCompare			wcscmp
	#define t_strncmp			wcsncmp
	#define t_strcasecmp		_wcsicmp
	#define t_strncasecmp		_wcsnicmp
	#define t_strtol			wcstol
	#define t_atoi				_wtoi
	#define t_StrDbl			wcstod
	#define t_itoa(value, buffer, size, radix)	_itow_s(value, buffer, size, radix)
	#define t_strstr			wcsstr
	#define t_tolower			towlower
	#define t_isdigit			iswdigit

#else

	typedef String			String;
	typedef char				tchar_t;

	#define _t(quote)			quote

	//#define t_StringLength			StringLength
//	#define StringCompare			strcmp
	#define t_strncmp			strncmp
	#define t_strcasecmp		_stricmp
	#define t_strncasecmp		_strnicmp
	#define t_strtol			strtol
	#define t_atoi				atoi
	#define t_StrDbl			StrDbl
	#define t_itoa(value, buffer, size, radix)	_itoa_s(value, buffer, size, radix)
	#define t_strstr			strstr
	#define t_tolower			tolower
	#define t_isdigit			isdigit

#endif*/

	#ifdef _WIN64
		typedef unsigned __int64 uint_ptr;
	#else
		typedef unsigned int	uint_ptr;
	#endif

#else
	#define LITEHTML_UTF8

	typedef char				tchar_t;
	typedef void*				uint_ptr;

	#define _t(quote)			quote

	#define t_StringLength			StringLength
	
	#define t_strncmp			strncmp

	#define t_strcasecmp		strcasecmp
	#define t_strncasecmp		strncasecmp
	#define t_itoa(value, buffer, size, radix)	snprintf(buffer, size, "%d", value)

	#define t_strtol			strtol
	#define t_atoi				atoi
	#define t_StrDbl			StrDbl
	#define t_strstr			strstr
	#define t_tolower			tolower
	#define t_isdigit			isdigit

#endif


END_HTMLCTRL_NAMESPACE

