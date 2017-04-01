#include "Html.h"
#include "MediaQuery.h"
#include "Document.h"

NAMESPACE_HTMLCTRL

MediaQuery::MediaQuery(ObjectPool& pool) : Object(pool)
{
	m_MediaType		= media_type_all;
	m_not			= false;
}

MediaQuery::MediaQuery( const MediaQuery& val ) : Object(val.GetObjectPool())
{
	m_not			= val.m_not;
	m_expressions	<<= val.m_expressions;
	m_MediaType		= val.m_MediaType;
}

MediaQuery::ptr MediaQuery::CreateFromString( const String& str, Document* doc )
{
	MediaQuery::ptr query = new MediaQuery(doc->GetObjectPool());

	StringVector tokens;
	SplitString(str, tokens, " \t\r\n", "", "(");

	for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
	{
		if((*tok) == "not")
		{
			query->m_not = true;
		} else if((*tok)[0] == '(')
		{
			//tok->erase(0, 1);
			tok->Remove(0);
			
			if((*tok)[tok->GetCount() - 1] == ')')
			{
				tok->Remove(tok->GetCount() - 1, 1);
			}
			MediaQueryExpression expr;
			StringVector expr_tokens;
			SplitString((*tok), expr_tokens, ":");
			if(!expr_tokens.IsEmpty())
			{
				Trim(expr_tokens[0]);
				expr.feature = (MediaFeature) ValueIndex(expr_tokens[0], media_feature_strings, media_feature_none);
				if(expr.feature != media_feature_none)
				{
					if(expr_tokens.GetCount() == 1)
					{
						expr.check_as_bool = true;
					} else
					{
						Trim(expr_tokens[1]);
						expr.check_as_bool = false;
						if(expr.feature == media_feature_orientation)
						{
							expr.val = ValueIndex(expr_tokens[1], MediaOrientation_strings, MediaOrientation_landscape);
						} else
						{
							int slash_pos = expr_tokens[1].Find('/');
							if( slash_pos != -1 )
							{
								String val1 = expr_tokens[1].Mid(0, slash_pos);
								String val2 = expr_tokens[1].Mid(slash_pos + 1);
								Trim(val1);
								Trim(val2);
								expr.val = StrInt(val1);
								expr.val2 = StrInt(val2);
							} else
							{
								CssLength length;
								length.FromString(expr_tokens[1]);
								if(length.Units() == css_units_dpcm)
								{
									expr.val = (int) (length.Value() * 2.54);
								} else if(length.Units() == css_units_dpi)
								{
									expr.val = (int) (length.Value() * 2.54);
								} else
								{
									if(doc)
									{
										doc->CvtUnits(length, doc->GetHtmlFile()->GetDefaultFontSize());
									}
									expr.val = (int) length.Value();
								}
							}
						}
					}
					query->m_expressions.Add(expr);
				}
			}
		} else
		{
			query->m_MediaType = (MediaType) ValueIndex((*tok), media_type_strings, media_type_all);

		}
	}

	return query;
}

bool MediaQuery::Check( const MediaFeatures& features ) const
{
	bool res = false;
	if(m_MediaType == media_type_all || m_MediaType == features.type)
	{
		res = true;
		for(MediaQueryExpression::vector::ConstIterator expr = m_expressions.Begin(); expr != m_expressions.End() && res; expr++)
		{
			if(!expr->Check(features))
			{
				res = false;
			}
		}
	}

	if(m_not)
	{
		res = !res;
	}

	return res;
}

//////////////////////////////////////////////////////////////////////////

MediaQueryList::ptr MediaQueryList::CreateFromString( const String& str, Document* doc )
{
	MediaQueryList::ptr list = new MediaQueryList(doc->GetObjectPool());

	StringVector tokens;
	SplitString(str, tokens, ",");

	for(StringVector::Iterator tok = tokens.Begin(); tok != tokens.End(); tok++)
	{
		Trim(*tok);
		LowerCase(*tok);

		MediaQuery::ptr query = MediaQuery::CreateFromString(*tok, doc);
		if(query)
		{
			list->m_queries.Add(query);
		}
	}
	if(list->m_queries.IsEmpty())
	{
		list = 0;
	}

	return list;
}

bool MediaQueryList::ApplyMediaFeatures( const MediaFeatures& features )
{
	bool apply = false;
	
	for(MediaQuery::vector::Iterator iter = m_queries.Begin(); iter != m_queries.End() && !apply; iter++)
	{
		if((*iter)->Check(features))
		{
			apply = true;
		}
	}

	bool ret = (apply != m_is_used);
	m_is_used = apply;
	return ret;
}

bool MediaQueryExpression::Check( const MediaFeatures& features ) const
{
	switch(feature)
	{
	case media_feature_width:
		if(check_as_bool)
		{
			return (features.width != 0);
		} else if(features.width == val)
		{
			return true;
		}
		break;
	case media_feature_min_width:
		if(features.width >= val)
		{
			return true;
		}
		break;
	case media_feature_max_width:
		if(features.width <= val)
		{
			return true;
		}
		break;
	case media_feature_height:
		if(check_as_bool)
		{
			return (features.height != 0);
		} else if(features.height == val)
		{
			return true;
		}
		break;
	case media_feature_min_height:
		if(features.height >= val)
		{
			return true;
		}
		break;
	case media_feature_max_height:
		if(features.height <= val)
		{
			return true;
		}
		break;

	case media_feature_device_width:
		if(check_as_bool)
		{
			return (features.device_width != 0);
		} else if(features.device_width == val)
		{
			return true;
		}
		break;
	case media_feature_min_device_width:
		if(features.device_width >= val)
		{
			return true;
		}
		break;
	case media_feature_max_device_width:
		if(features.device_width <= val)
		{
			return true;
		}
		break;
	case media_feature_device_height:
		if(check_as_bool)
		{
			return (features.device_height != 0);
		} else if(features.device_height == val)
		{
			return true;
		}
		break;
	case media_feature_min_device_height:
		if(features.device_height <= val)
		{
			return true;
		}
		break;
	case media_feature_max_device_height:
		if(features.device_height <= val)
		{
			return true;
		}
		break;

	case media_feature_orientation:
		if(features.height >= features.width)
		{
			if(val == MediaOrientation_portrait)
			{
				return true;
			}
		} else
		{
			if(val == MediaOrientation_landscape)
			{
				return true;
			}
		}
		break;
	case media_feature_aspect_ratio:
		if(features.height && val2)
		{
			int ratio_this = RoundDbl( (double) val / (double) val2 * 100 );
			int ratio_feat = RoundDbl( (double) features.width / (double) features.height * 100.0 );
			if(ratio_this == ratio_feat)
			{
				return true;
			}
		}
		break;
	case media_feature_min_aspect_ratio:
		if(features.height && val2)
		{
			int ratio_this = RoundDbl( (double) val / (double) val2 * 100 );
			int ratio_feat = RoundDbl( (double) features.width / (double) features.height * 100.0 );
			if(ratio_feat >= ratio_this)
			{
				return true;
			}
		}
		break;
	case media_feature_max_aspect_ratio:
		if(features.height && val2)
		{
			int ratio_this = RoundDbl( (double) val / (double) val2 * 100 );
			int ratio_feat = RoundDbl( (double) features.width / (double) features.height * 100.0 );
			if(ratio_feat <= ratio_this)
			{
				return true;
			}
		}
		break;

	case media_feature_device_aspect_ratio:
		if(features.device_height && val2)
		{
			int ratio_this = RoundDbl( (double) val / (double) val2 * 100 );
			int ratio_feat = RoundDbl( (double) features.device_width / (double) features.device_height * 100.0 );
			if(ratio_feat == ratio_this)
			{
				return true;
			}
		}
		break;
	case media_feature_min_device_aspect_ratio:
		if(features.device_height && val2)
		{
			int ratio_this = RoundDbl( (double) val / (double) val2 * 100 );
			int ratio_feat = RoundDbl( (double) features.device_width / (double) features.device_height * 100.0 );
			if(ratio_feat >= ratio_this)
			{
				return true;
			}
		}
		break;
	case media_feature_max_device_aspect_ratio:
		if(features.device_height && val2)
		{
			int ratio_this = RoundDbl( (double) val / (double) val2 * 100 );
			int ratio_feat = RoundDbl( (double) features.device_width / (double) features.device_height * 100.0 );
			if(ratio_feat <= ratio_this)
			{
				return true;
			}
		}
		break;

	case media_feature_color:
		if(check_as_bool)
		{
			return (features.color != 0);
		} else if(features.color == val)
		{
			return true;
		}
		break;
	case media_feature_min_color:
		if(features.color >= val)
		{
			return true;
		}
		break;
	case media_feature_max_color:
		if(features.color <= val)
		{
			return true;
		}
		break;

	case media_feature_color_index:
		if(check_as_bool)
		{
			return (features.color_index != 0);
		} else if(features.color_index == val)
		{
			return true;
		}
		break;
	case media_feature_min_color_index:
		if(features.color_index >= val)
		{
			return true;
		}
		break;
	case media_feature_max_color_index:
		if(features.color_index <= val)
		{
			return true;
		}
		break;

	case media_feature_monochrome:
		if(check_as_bool)
		{
			return (features.monochrome != 0);
		} else if(features.monochrome == val)
		{
			return true;
		}
		break;
	case media_feature_min_monochrome:
		if(features.monochrome >= val)
		{
			return true;
		}
		break;
	case media_feature_max_monochrome:
		if(features.monochrome <= val)
		{
			return true;
		}
		break;

	case media_feature_resolution:
		if(features.resolution == val)
		{
			return true;
		}
		break;
	case media_feature_min_resolution:
		if(features.resolution >= val)
		{
			return true;
		}
		break;
	case media_feature_max_resolution:
		if(features.resolution <= val)
		{
			return true;
		}
		break;
	default:
		return false;
	}

	return false;
}

END_HTMLCTRL_NAMESPACE

