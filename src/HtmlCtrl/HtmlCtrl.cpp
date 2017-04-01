#include <Core/Core.h>
using namespace Upp;

#include "HtmlCtrl.h"
#include "PerformanceMeter.h"
#include "Common.h"


NAMESPACE_HTMLCTRL



//HtmlCtrl() {
HtmlCtrl::HtmlCtrl(HtmlNode& dir) : HtmlFile(dir) {
	
}
	
HtmlCtrl::~HtmlCtrl() {
	// clear_images();
}


Array<Font> _fonts;

// Checks if font(s) exists and loads them to memory
uint32 HtmlCtrl::CreateFont(const String& faceName, int size, int weight, FontStyle italic, unsigned int decoration, FontMetrics* fm) {
	//LOG("CreateFont: " << faceName << " size=" << size << " weight=" << weight);
	PERFMETER_BEGIN(PERF_CREATEFONT);
	
	
	Vector<String> fonts;
	fonts = Split(faceName.ToString(), ",");
	if (!fonts.GetCount()) {
		PERFMETER_END(PERF_CREATEFONT);
		return -1;
	}
	
	bool found = false;
	int findex;
	int font_count = Font::GetFaceCount();
	for(int i = 0; i < fonts.GetCount(); i++) {
		String f = TrimBoth(fonts[i]);
		if (!f.GetCount()) continue;
		
		for(int j = 0; j < font_count; j++) {
			if (Font::GetFaceName(j) == fonts[i]) {
				findex = j;
				found = true;
				break;
			}
		}
		/*String& name = fonts[i];
		findex = Font::FindFaceNameIndex(name);
		String mmm = Font::GetFaceName(findex);
		if (findex >= 0) {
			found = true;
			break;
		}*/
	}
	
	
	
	
	//Font& fnt = _fonts.Add();
	Font fnt;
	
	if(!found)
		fnt = Serif();
	else
		fnt.Face(findex);
	
	if(italic == fontStyleItalic ) {
		fnt.Italic(1);
	}
	
	// http://www.w3.org/TR/css3-fonts/#font-weight-prop
	if (weight >= 600) fnt.Bold(1);
	
	if (size) 
		fnt.Height(size);
	
	if (decoration & font_decoration_linethrough)
		fnt.Strikeout(1);
	
	if (decoration & font_decoration_underline)
		fnt.Underline(1);
	
	if(fm)
	{
		fm->ascent		= fnt.GetAscent();
		fm->descent		= fnt.GetDescent();
		fm->height		= fnt.GetAscent() + fnt.GetDescent();
		fm->x_height	= fnt.GetHeight();
	}
	
	
	for(int i = 0; i < _fonts.GetCount(); i++) {
		if (_fonts[i] == fnt) {
			PERFMETER_END(PERF_CREATEFONT);
			return i+1;
		}
	}
	
	int i = _fonts.GetCount();
	_fonts.Add(fnt);
	
	PERFMETER_END(PERF_CREATEFONT);
	return i+1;
}

void HtmlCtrl::DeleteFont(uint32 hFont) {
	/*cairo_font* fnt = (cairo_font*) hFont;
	if(fnt)
	{
		cairo_font_face_destroy(fnt->font);
		delete fnt;
	}*/
}

int HtmlCtrl::TextWidth(const String& text, uint32 hFont) {
	
	/*cairo_font* fnt = (cairo_font*) hFont;

	cairo_save(m_temp_cr);

	cairo_set_font_size(m_temp_cr, fnt->size);
	cairo_set_font_face(m_temp_cr, fnt->font);
	cairo_text_extents_t ext;
	cairo_text_extents(m_temp_cr, text, &ext);

	cairo_restore(m_temp_cr);

	return (int) ext.x_advance;*/
	
	//if (hFont >= _fonts.GetCount()) hFont = 0;
	if (hFont == 0) return 0;
	
	PERFMETER_BEGIN(PERF_TEXTWIDTH);
	
	hFont--;
	
	Size sz(0,0);
	sz = GetTextSize(text, _fonts[hFont]);
	
	PERFMETER_END(PERF_TEXTWIDTH);
	return sz.cx;
}

void HtmlCtrl::DrawText(Draw& hdc, const String& text, uint32 hFont, WebColor color, const Position& pos) {
	
	//LOG("DrawText: " << text << " font=" << (int)hFont << " pos=" << pos.x << "," << pos.y);
	//if (color.alpha == 255) {
	//	hdc.DrawText(pos.x, pos.y, text, _fonts[hFont], color);
	
	//if (hFont >= _fonts.GetCount()) hFont = 0;
	
	if (hFont == 0) return;
	hFont--;
	
	PERFMETER_BEGIN(PERF_DRAWTEXT);
	
	//if (1) {
	if (color.alpha == 255) {
		hdc.DrawText(pos.x, pos.y, text, _fonts[hFont], color);
	} else {
		ImageDraw id(pos.width, pos.height);
		id.Alpha().DrawRect(0,0,pos.width,pos.height,GrayColor(color.alpha));
		id.DrawText(0, 0, text, _fonts[hFont], color);
		hdc.DrawImage(pos.x, pos.y, id);
	}
	
	PERFMETER_END(PERF_DRAWTEXT);
	
	/*cairo_font* fnt = (cairo_font*) hFont;
	cairo_t* cr		= (cairo_t*) hdc;
	cairo_save(cr);

	apply_clip(cr);

	cairo_set_font_face(cr, fnt->font);
	cairo_set_font_size(cr, fnt->size);
	cairo_font_extents_t ext;
	cairo_font_extents(cr, &ext);

	int x = pos.Left();
	int y = pos.Bottom()	- ext.descent;

	set_color(cr, color);

	cairo_move_to(cr, x, y);
	cairo_show_text(cr, text);

	int tw = 0;

	if(fnt->underline || fnt->strikeout)
	{
		tw = text_width(text, hFont);
	}

	if(fnt->underline)
	{
		cairo_set_line_width(cr, 1);
		cairo_move_to(cr, x, y + 1.5);
		cairo_line_to(cr, x + tw, y + 1.5);
		cairo_stroke(cr);
	}
	if(fnt->strikeout)
	{
		cairo_text_extents_t tex;
		cairo_text_extents(cr, "x", &tex);

		int ln_y = y - tex.height / 2.0;

		cairo_set_line_width(cr, 1);
		cairo_move_to(cr, x, (double) ln_y - 0.5);
		cairo_line_to(cr, x + tw, (double) ln_y - 0.5);
		cairo_stroke(cr);
	}

	cairo_restore(cr);*/
	
}

void HtmlCtrl::FillRect(Draw& hdc, const Position& pos, const WebColor color, const CssBorderRadius& radius)
{
	Panic("FillRect");
	/*if(hdc)
	{
		cairo_t* cr = (cairo_t*) hdc;
		cairo_save(cr);
		apply_clip(cr);

		cairo_new_path(cr);

		if(radius.top_left_x.val())
		{
			cairo_arc(cr, pos.Left() + radius.top_left_x.val(), pos.Top() + radius.top_left_x.val(), radius.top_left_x.val(), M_PI, M_PI * 3.0 / 2.0);
		} else
		{
			cairo_move_to(cr, pos.Left(), pos.Top());
		}

		cairo_line_to(cr, pos.Right() - radius.top_right_x.val(), pos.Top());

		if(radius.top_right_x.val())
		{
			cairo_arc(cr, pos.Right() - radius.top_right_x.val(), pos.Top() + radius.top_right_x.val(), radius.top_right_x.val(), M_PI * 3.0 / 2.0, 2.0 * M_PI);
		}

		cairo_line_to(cr, pos.Right(), pos.Bottom() - radius.bottom_right_x.val());

		if(radius.bottom_right_x.val())
		{
			cairo_arc(cr, pos.Right() - radius.bottom_right_x.val(), pos.Bottom() - radius.bottom_right_x.val(), radius.bottom_right_x.val(), 0, M_PI / 2.0);
		}

		cairo_line_to(cr, pos.Left() - radius.bottom_left_x.val(), pos.Bottom());

		if(radius.bottom_left_x.val())
		{
			cairo_arc(cr, pos.Left() + radius.bottom_left_x.val(), pos.Bottom() - radius.bottom_left_x.val(), radius.bottom_left_x.val(), M_PI / 2.0, M_PI);
		}

		set_color(cr, color);
		cairo_fill(cr);
		cairo_restore(cr);
	}*/
}

int HtmlCtrl::PtPx(int pt) {
	//LOG("PtPx");
	//GdkScreen* screen = gdk_screen_get_default();
	//double dpi = gdk_screen_get_resolution(screen);
	double dpi = 94; // TODO: get real
	return (int) ((double) pt * dpi / 72.0);
}

int HtmlCtrl::GetDefaultFontSize() const {
	return 16;
}

String HtmlCtrl::GetDefaultFontName() const {
	//return Serif().GetFaceName();// "Times New Roman";
	//String s = Serif().GetFaceName();
	//LOG("font: " << s);
	//return s;
	return "serif";
}

void HtmlCtrl::DrawListMarker(Draw& hdc, const ListMarker& marker) {
	PERFMETER_BEGIN(PERF_DRAWLISTMARKER);
	if(!marker.image.IsEmpty())
	{
		//litehtml::tstring url;
//		make_url(marker.image.c_str(), marker.baseurl, url);
//
//		lock_images_cache();
//		images_map::iterator img_i = m_images.find(url.c_str());
//		if(img_i != m_images.end())
//		{
//			if(img_i->second)
//			{
//				draw_txdib((cairo_t*) hdc, img_i->second, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height);
//			}
//		}
//		unlock_images_cache();
		String baseurl = marker.baseurl;
		if (baseurl.GetCount() == 0) baseurl = GetBaseUrl();
		Cache& cache = GetCache();
		int i = cache.FindCacheObject(marker.image, marker.baseurl);
		//LOG("DrawListMarker DrawImage cache_id=" << i);
		if (i == -1) {
			PERFMETER_END(PERF_DRAWLISTMARKER);
			return;
		}
		const Image* img = cache.GetCacheObject(i).Get<CacheImage>();
		if (img) {
			hdc.DrawImage(marker.pos.x, marker.pos.y, *img);
		}
	} else
	{
		switch(marker.marker_type)
		{
		case list_style_type_circle:
			{
				//draw_ellipse((cairo_t*) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color, 0.5);
				LOG("DrawListMarker DrawEllipse");
				//hdc.DrawEllipse(marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, Null, 0.5, marker.color);
			}
			break;
		case list_style_type_disc:
			{
				LOG("DrawListMarker FillEllipse");
				//fill_ellipse((cairo_t*) hdc, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color);
				//hdc.DrawEllipse(marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color);
			}
			break;
		case list_style_type_square:
			/*if(hdc)
			{*/
				/*cairo_t* cr = (cairo_t*) hdc;
				cairo_save(cr);

				cairo_new_path(cr);
				cairo_rectangle(cr, marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height);

				set_color(cr, marker.color);
				cairo_fill(cr);
				cairo_restore(cr);*/
			//LOG("DrawListMarker DrawRect");
			//hdc.DrawRect(marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color);
			if (marker.color.alpha == 255)
				;//hdc.DrawRect(marker.pos.x, marker.pos.y, marker.pos.width, marker.pos.height, marker.color);
			else if (marker.color.alpha > 0)
				Panic("todo");
			//}
			break;
		default:
			//do nothing
			break;
		}
	}
	PERFMETER_END(PERF_DRAWLISTMARKER);
}

void HtmlCtrl::LoadImage(const String& src, const String& baseurl, bool redraw_on_ready) {
	LOG("LoadImage " << src << " " << baseurl);
	/**/
}

void HtmlCtrl::GetImageSize(int cache_id, Size& sz) {
	PERFMETER_BEGIN(PERF_IMAGESIZEGET);
	
	ASSERT(cache_id != -1);
	Cache& cache = GetCache();
	Image* bgbmp = &GetCache().GetCacheObject(cache_id).Get<Image>();
	
	// If image is not downloaded
	if (!bgbmp) {
		sz.cx = 0;
		sz.cy = 0;
		
	} else {
		sz.cx = bgbmp->GetWidth();
		sz.cy = bgbmp->GetHeight();
	}
	
	PERFMETER_END(PERF_IMAGESIZEGET);
}

void HtmlCtrl::GetImageSize(const String& src, const String& baseurl_, Size& sz) {
	PERFMETER_BEGIN(PERF_IMAGESIZEFIND);
	
	//LOG("GetImageSize " << src << " " << baseurl);
	Cache& cache = GetCache();
	
	
	String baseurl = baseurl_;
	if (baseurl.GetCount() == 0)
		baseurl = GetBaseUrl();
	
	int cache_id = cache.FindCacheObject(src, baseurl);
	
	const Image* bgbmp = cache_id == -1 ? 0 : &cache.GetCacheObject(cache_id).Get<CacheImage>().GetImage();
	
	// If image is not downloaded
	if (!bgbmp) {
		sz.cx = 0;
		sz.cy = 0;
	} else {
		sz.cx = bgbmp->GetWidth();
		sz.cy = bgbmp->GetHeight();
	}
	
	
	PERFMETER_END(PERF_IMAGESIZEFIND);
	
	/*litehtml::tstring url;
	make_url(src, baseurl, url);

	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		sz.width	= img->second->get_width();
		sz.height	= img->second->get_height();
	} else
	{
		sz.width	= 0;
		sz.height	= 0;
	}*/
}

void HtmlCtrl::DrawImage(Draw& hdc, const String& src, const String& baseurl, const Position& pos) {
	LOG("DrawImage " << src << " " << baseurl);
	
	/*cairo_t* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);

	litehtml::tstring url;
	make_url(src, baseurl, url);
	images_map::iterator img = m_images.find(url.c_str());
	if(img != m_images.end())
	{
		draw_pixbuf(cr, img->second, pos.x, pos.y, pos.width, pos.height);
	}
	cairo_restore(cr);*/
}

void HtmlCtrl::DrawBackground(Draw& hdc, const BackgroundPaint& bg) {
	PERFMETER_BEGIN(PERF_DRAWBG);
	PERFMETER_BEGIN(PERF_DRAWBG0);
	
	Cache& cache = GetCache();
	
	//LOG("DrawBackground " << bg.baseurl << " " << bg.image << " cache_id=" << bg.cache_id);
	
	//if (bg.cache_id == -1) 
	//	return;
	
	/*cairo_t* cr = (cairo_t*) hdc;
	cairo_save(cr);
	apply_clip(cr);
	*/
	
	
	Rect r0(bg.border_box.GetRect());
	Rect r1(bg.clip_box.GetRect());
	
	ASSERT(	r0.left 	<= r1.left &&
			r0.right 	>= r1.right &&
			r0.top 		<= r1.top &&
			r0.bottom 	>= r1.bottom);
	
	Size sz(r0.GetSize());
	ImageDraw id(sz);
	
	int trans_x = bg.position_x - r0.left;
	int trans_y = bg.position_y - r0.top;
	
	// TODO: !!! rounded rectangle instead of rectangle
	//rounded_rectangle(cr, bg.border_box, bg.border_radius);
	//cairo_clip(cr);
	//id.Alpha().DrawRect(r0, GrayColor(255));
	id.DrawRect(sz, White());
	id.Alpha().DrawRect(sz, GrayColor(bg.color.alpha));
	//id.Alpha();
	
	//cairo_rectangle(cr, bg.clip_box.x, bg.clip_box.y, bg.clip_box.width, bg.clip_box.height);
	//cairo_clip(cr);
	//id.Alpha().DrawRect(r1, GrayColor(255));
	
	
	if(bg.color.alpha)
	{
		//set_color(cr, bg.color);
		//cairo_paint(cr);
		//hdc.DrawRect(bg.clip_box.GetRect(), bg.color);
		
		id.DrawRect(r0.GetSize(), bg.color);
	}
	//else
	//	return;
	//id.DrawRect(r0.GetSize(), bg.color);
	//id.DrawRect(r0.GetSize(), Yellow);
	
	/*litehtml::tstring url;
	make_url(bg.image.c_str(), bg.baseurl.c_str(), url);

	//lock_images_cache();
	images_map::iterator img_i = m_images.find(url.c_str());
	if(img_i != m_images.end() && img_i->second)
	{*/
	//Glib::RefPtr<Gdk::Pixbuf> bgbmp = img_i->second;
	
	const Image* bgbmp = bg.cache_id == -1 ? 0 : &cache.GetCacheObject(bg.cache_id).Get<CacheImage>().GetImage();
	
	
	PERFMETER_END(PERF_DRAWBG0);
	PERFMETER_BEGIN(PERF_DRAWBG1);
	
	// If image is not downloaded
	//if (!bgbmp) return;
	
	//ASSERT(bgbmp);

	bool del_img = 0;
	//Glib::RefPtr<Gdk::Pixbuf> new_img;
	if (bgbmp) {
		if(bg.image_size.cx != bgbmp->GetWidth() || bg.image_size.cy != bgbmp->GetHeight())
		{
			//Image new_img = bgbmp->scale_simple(bg.image_size.width, bg.image_size.height, Gdk::INTERP_BILINEAR);
			//bgbmp = new_img;
			//One<StreamRaster> r = StreamRaster::OpenAny(*bgbmp);
			Size rsz(bg.image_size.cx, bg.image_size.cy);
			//Size isz(bgbmp->GetSize());
			//ImageEncoder m;
			//Rescale(m, rsz, *bgbmp, isz);
			//img.SetImage(m);
			//bgbmp = new Image(m);
			bgbmp = new Image(Rescale(*bgbmp, rsz));
			del_img = true;
		}
	
		/*cairo_surface_t* img = surface_from_pixbuf(bgbmp);
		cairo_pattern_t *pattern = cairo_pattern_create_for_surface(img);
		cairo_matrix_t flib_m;
		cairo_matrix_init_identity(&flib_m);
		cairo_matrix_translate(&flib_m, -bg.position_x, -bg.position_y);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_pattern_set_matrix (pattern, &flib_m);*/
		
		//hdc.DrawImage(bg.position_x, bg.position_y, *bgbmp);
		int x, y, max_x, max_y, w, h, tmp;
		x = trans_x;
		y = trans_y;
		max_x = trans_x + r1.Width();
		max_y = trans_y + r1.Height();
		w = bgbmp->GetWidth();
		h = bgbmp->GetHeight();
		Rect trans_clip = RectC(r0.left - r1.left, r0.top - r1.top, r1.Width(), r1.Height());
		
		switch(bg.repeat)
		{
		case background_repeat_no_repeat:
//			draw_pixbuf(cr, bgbmp, bg.position_x, bg.position_y, bgbmp->get_width(), bgbmp->get_height());
			id.DrawImage(trans_x, trans_y, *bgbmp);
			id.Alpha().DrawRect(trans_x, trans_y, w, h, GrayColor(255));
			break;
	
		case background_repeat_repeat_x:
			//cairo_set_source(cr, pattern);
			//cairo_rectangle(cr, bg.clip_box.Left(), bg.position_y, bg.clip_box.width, bgbmp->get_height());
			while (x > 0) x -= w;
			for(; x < max_x; x += w) {
				id.DrawImage(x, y, *bgbmp, trans_clip);
				id.Alpha().DrawRect(x, y, w, h, GrayColor(255));
			}
			//cairo_fill(cr);
			break;
	
		case background_repeat_repeat_y:
			//cairo_set_source(cr, pattern);
			//cairo_rectangle(cr, bg.position_x, bg.clip_box.Top(), bgbmp->get_width(), bg.clip_box.height);
			//cairo_fill(cr);
			while (y > 0) y -= h;
			for(; y < max_y; y += h) {
				id.DrawImage(x, y, *bgbmp, trans_clip);
				id.Alpha().DrawRect(x, y, w, h, GrayColor(255));
			}
			break;
	
		case background_repeat_repeat:
			//cairo_set_source(cr, pattern);
			//cairo_rectangle(cr, bg.clip_box.Left(), bg.clip_box.Top(), bg.clip_box.width, bg.clip_box.height);
			//cairo_fill(cr);
			while (x > 0) x -= w;
			while (y > 0) y -= h;
			tmp = y;
			for(; x < max_x; x += w) {
				for(y = tmp; y < max_y; y += h) {
					id.DrawImage(x, y, *bgbmp, trans_clip);
					id.Alpha().DrawRect(x, y, w, h, GrayColor(255));
				}
			}
			break;
		}

	}
	
	PERFMETER_END(PERF_DRAWBG1);
	PERFMETER_BEGIN(PERF_DRAWBG2);
	
	hdc.DrawImage(r0.left, r0.top, id);
	//TODO: !!! Repeat background image
	
	
	
	//cairo_pattern_destroy(pattern);
	//cairo_surface_destroy(img);
	
	
	// If resized
	if (del_img)
		delete bgbmp;
	
	
	PERFMETER_END(PERF_DRAWBG2);
	PERFMETER_END(PERF_DRAWBG);
	/*}
//	unlock_images_cache();
	cairo_restore(cr);*/
}

void AddPathArc(Vector<Point>& p, double x, double y, double rx, double ry, double a1, double a2, bool neg) {
	if(rx > 0 && ry > 0)
	{

		/*cairo_save(cr);

		cairo_translate(cr, x, y);		// moves "top-left" cursor to x,y
		cairo_scale(cr, 1, ry / rx);	// multiplies y with ry/rx (total: y2 = y + (y2 - y) * (ry / rx))
		cairo_translate(cr, -x, -y);*/	// ...moves "top-left" cursor back to 0,0
		
		//if(neg) {
		double x1, y1, x2, y2;
		//double rx, ry;
		//rx = cx / 2;
		//ry = cy / 2;
		x += rx;
		y += ry;	
		double dt = (rx + ry) / 18 / 2;
		int steps = fabs((a2 - a1) / dt);
		
		
		
		//for ( double theta= 0; theta< 360; theta += dt )
		//for ( double theta= a1; (theta < a2) == only_inc; theta += dt )
		
		double theta = a1;
		if (neg) dt *= -1;
		
		x2 = ( rx * cos(theta*3.141593/180.0)+x);
		y2 = (-ry * sin(theta*3.141593/180.0)+y);
		y2 = y + (y2 - y) * (ry / rx); // Translate
		p << Point(x2, y2);
		
		for (int i = 0; i < steps; i++)
		{
			//x1 = ( rx*cos(theta*3.141593/180.0)+x);
			//y1 = (-ry*sin(theta*3.141593/180.0)+y);
	
			x2 = ( rx * cos((theta+dt)*3.141593/180.0)+x);
			y2 = (-ry * sin((theta+dt)*3.141593/180.0)+y);
			y2 = y + (y2 - y) * (ry / rx); // Translate
			p << Point(x2, y2);
			
			//FloatingPlotLineWidthAA(  x1, y1, x2, y2, pen, pencolor);
			//if (pen > 0) PlotLineWidthAA(  x1, y1, x2, y2, pen, c);
		}
		/*} else
		{
			cairo_arc(cr, x, y, rx, a1, a2);
		}*/

		//cairo_restore(cr);
	} else
	{
		//cairo_move_to(cr, x, y);
		p << Point(x, y);
	}
}

void HtmlCtrl::DrawBorders(Draw& hdc, const Borders& borders, const Position& draw_pos, bool root) {
	//LOG("DrawBorders");
	PERFMETER_BEGIN(PERF_DRAWBORDERS);
	
	Vector<Point> p;
	

	int bdr_top		= 0;
	int bdr_bottom	= 0;
	int bdr_left	= 0;
	int bdr_right	= 0;

	if(borders.top.width != 0 && borders.top.style > border_style_hidden)
	{
		bdr_top = (int) borders.top.width;
	}
	if(borders.bottom.width != 0 && borders.bottom.style > border_style_hidden)
	{
		bdr_bottom = (int) borders.bottom.width;
	}
	if(borders.left.width != 0 && borders.left.style > border_style_hidden)
	{
		bdr_left = (int) borders.left.width;
	}
	if(borders.right.width != 0 && borders.right.style > border_style_hidden)
	{
		bdr_right = (int) borders.right.width;
	}

	// draw right border
	if(bdr_right)
	{
		//set_color(cr, borders.right.color);
		p.Clear();
		
		double r_top	= borders.radius.top_right_x;
		double r_bottom	= borders.radius.bottom_right_x;
		
		Vector<Point> area;
		if(r_top)
		{
			double end_angle	= 2 * M_PI;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_top / (double) bdr_right + 1);

			AddPathArc(p,
				draw_pos.Right() - r_top,
				draw_pos.Top() + r_top,
				r_top - bdr_right,
				r_top - bdr_right + (bdr_right - bdr_top),
				end_angle,
				start_angle, true);

			AddPathArc(p,
				draw_pos.Right() - r_top,
				draw_pos.Top() + r_top,
				r_top,
				r_top,
				start_angle,
				end_angle, false);
		} else
		{
			/*cairo_move_to(cr, draw_pos.Right() - bdr_right, draw_pos.Top() + bdr_top);
			cairo_line_to(cr, draw_pos.Right(), draw_pos.Top());*/
			//hdc.DrawLine(draw_pos.Right() - bdr_right, draw_pos.Top() + bdr_top, draw_pos.Right(), draw_pos.Top(), 1, borders.right.color);
			p << Point(draw_pos.Right() - bdr_right, draw_pos.Top() + bdr_top);
			p << Point(draw_pos.Right(), draw_pos.Top());
		}

		if(r_bottom)
		{
			p << Point(draw_pos.Right(),	draw_pos.Bottom() - r_bottom);

			double start_angle	= 0;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_bottom / (double) bdr_right + 1);

			AddPathArc(p,
				draw_pos.Right() - r_bottom,
				draw_pos.Bottom() - r_bottom,
				r_bottom,
				r_bottom,
				start_angle,
				end_angle, false);

			AddPathArc(p,
				draw_pos.Right() - r_bottom,
				draw_pos.Bottom() - r_bottom,
				r_bottom - bdr_right,
				r_bottom - bdr_right + (bdr_right - bdr_bottom),
				end_angle,
				start_angle, true);
		} else
		{
			/*cairo_line_to(cr, draw_pos.Right(),	draw_pos.Bottom());
			cairo_line_to(cr, draw_pos.Right() - bdr_right,	draw_pos.Bottom() - bdr_bottom);*/
			//hdc.DrawLine(draw_pos.Right(),	draw_pos.Bottom(), draw_pos.Right() - bdr_right,	draw_pos.Bottom() - bdr_bottom, 1, borders.right.color);
			p << Point(draw_pos.Right(),	draw_pos.Bottom());
			p << Point(draw_pos.Right() - bdr_right,	draw_pos.Bottom() - bdr_bottom);
		}
		
		//if(r_top && r_bottom) hdc.DrawRect(draw_pos.Right() - bdr_right, draw_pos.Top() + bdr_top, bdr_right, bdr_top, borders.right.color);
		//hdc.DrawPolygon(p, borders.right.color);
		hdc.DrawPolyline(p, borders.right.width, borders.right.color);
			
		//cairo_fill(cr);
	}

	// draw bottom border
	if(bdr_bottom)
	{
		//set_color(cr, borders.bottom.color);
		p.Clear();
		
		double r_left	= borders.radius.bottom_left_x;
		double r_right	= borders.radius.bottom_right_x;

		if(r_left)
		{
			double start_angle	= M_PI / 2.0;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_left / (double) bdr_bottom + 1);

			AddPathArc(p,
				draw_pos.Left() + r_left,
				draw_pos.Bottom() - r_left,
				r_left - bdr_bottom + (bdr_bottom - bdr_left),
				r_left - bdr_bottom,
				start_angle,
				end_angle, false);

			AddPathArc(p,
				draw_pos.Left() + r_left,
				draw_pos.Bottom() - r_left,
				r_left,
				r_left,
				end_angle,
				start_angle, true);
		} else
		{
			/*cairo_move_to(cr, draw_pos.Left(), draw_pos.Bottom());
			cairo_line_to(cr, draw_pos.Left() + bdr_left, draw_pos.Bottom() - bdr_bottom);*/
			//hdc.DrawLine(draw_pos.Left(), draw_pos.Bottom(), draw_pos.Left() + bdr_left, draw_pos.Bottom() - bdr_bottom, 1, borders.bottom.color);
			p << Point(draw_pos.Left(), draw_pos.Bottom());
			p << Point(draw_pos.Left() + bdr_left, draw_pos.Bottom() - bdr_bottom);
		}

		if(r_right)
		{
			p << Point(draw_pos.Right() - r_right,	draw_pos.Bottom());

			double end_angle	= M_PI / 2.0;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_right / (double) bdr_bottom + 1);

			AddPathArc(p,
				draw_pos.Right() - r_right,
				draw_pos.Bottom() - r_right,
				r_right,
				r_right,
				end_angle,
				start_angle, true);

			AddPathArc(p,
				draw_pos.Right() - r_right,
				draw_pos.Bottom() - r_right,
				r_right - bdr_bottom + (bdr_bottom - bdr_right),
				r_right - bdr_bottom,
				start_angle,
				end_angle, false);
		} else
		{
			/*cairo_line_to(cr, draw_pos.Right() - bdr_right,	draw_pos.Bottom() - bdr_bottom);
			cairo_line_to(cr, draw_pos.Right(),	draw_pos.Bottom());*/
			//hdc.DrawLine(draw_pos.Right() - bdr_right,	draw_pos.Bottom() - bdr_bottom, draw_pos.Right(),	draw_pos.Bottom(), 1, borders.bottom.color);
			p << Point(draw_pos.Right() - bdr_right,	draw_pos.Bottom() - bdr_bottom);
			p << Point(draw_pos.Right(),	draw_pos.Bottom());
		}

		//cairo_fill(cr);
		//if(r_left && r_right) hdc.DrawRect(draw_pos.Left() , draw_pos.Bottom() - bdr_bottom, bdr_left, bdr_bottom, borders.bottom.color);
		//hdc.DrawPolygon(p, borders.bottom.color);
		hdc.DrawPolyline(p, borders.bottom.width, borders.bottom.color);
		
	}

	// draw top border
	if(bdr_top)
	{
		//set_color(cr, borders.top.color);
		p.Clear();
		
		double r_left	= borders.radius.top_left_x;
		double r_right	= borders.radius.top_right_x;

		if(r_left)
		{
			double end_angle	= M_PI * 3.0 / 2.0;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_left / (double) bdr_top + 1);

			AddPathArc(p,
				draw_pos.Left() + r_left,
				draw_pos.Top() + r_left,
				r_left,
				r_left,
				end_angle,
				start_angle, true);

			AddPathArc(p,
				draw_pos.Left() + r_left,
				draw_pos.Top() + r_left,
				r_left - bdr_top + (bdr_top - bdr_left),
				r_left - bdr_top,
				start_angle,
				end_angle, false);
		} else
		{
		/*	cairo_move_to(cr, draw_pos.Left(), draw_pos.Top());
			cairo_line_to(cr, draw_pos.Left() + bdr_left, draw_pos.Top() + bdr_top);*/
			//hdc.DrawLine(draw_pos.Left(), draw_pos.Top(), draw_pos.Left() + bdr_left, draw_pos.Top() + bdr_top, 1, borders.top.color);
			p << Point(draw_pos.Left(), draw_pos.Top());
			p << Point(draw_pos.Left() + bdr_left, draw_pos.Top() + bdr_top);
		}

		if(r_right)
		{
			p << Point(draw_pos.Right() - r_right,	draw_pos.Top() + bdr_top);

			double start_angle	= M_PI * 3.0 / 2.0;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_right / (double) bdr_top + 1);

			AddPathArc(p,
				draw_pos.Right() - r_right,
				draw_pos.Top() + r_right,
				r_right - bdr_top + (bdr_top - bdr_right),
				r_right - bdr_top,
				start_angle,
				end_angle, false);

			AddPathArc(p,
				draw_pos.Right() - r_right,
				draw_pos.Top() + r_right,
				r_right,
				r_right,
				end_angle,
				start_angle, true);
		} else
		{
			/*cairo_line_to(cr, draw_pos.Right() - bdr_right,	draw_pos.Top() + bdr_top);
			cairo_line_to(cr, draw_pos.Right(),	draw_pos.Top());*/
			//hdc.DrawLine(draw_pos.Right() - bdr_right,	draw_pos.Top() + bdr_top, draw_pos.Right(),	draw_pos.Top(), 1, borders.top.color);
			p << Point(draw_pos.Right() - bdr_right,	draw_pos.Top() + bdr_top);
			p << Point(draw_pos.Right(),	draw_pos.Top());
		}

		//cairo_fill(cr);
		//if(r_left && r_right) hdc.DrawRect(draw_pos.Left() , draw_pos.Top(), bdr_left, bdr_top, borders.top.color);
		//hdc.DrawPolygon(p, borders.top.color);
		hdc.DrawPolyline(p, borders.top.width, borders.top.color);
		
	}

	// draw left border
	if(bdr_left)
	{
		//set_color(cr, borders.left.color);
		p.Clear();
		
		double r_top	= borders.radius.top_left_x;
		double r_bottom	= borders.radius.bottom_left_x;

		if(r_top)
		{
			double start_angle	= M_PI;
			double end_angle	= start_angle + M_PI / 2.0  / ((double) bdr_top / (double) bdr_left + 1);

			AddPathArc(p,
				draw_pos.Left() + r_top,
				draw_pos.Top() + r_top,
				r_top - bdr_left,
				r_top - bdr_left + (bdr_left - bdr_top),
				start_angle,
				end_angle, false);

			AddPathArc(p,
				draw_pos.Left() + r_top,
				draw_pos.Top() + r_top,
				r_top,
				r_top,
				end_angle,
				start_angle, true);
		} else
		{
		/*	cairo_move_to(cr, draw_pos.Left() + bdr_left, draw_pos.Top() + bdr_top);
			cairo_line_to(cr, draw_pos.Left(), draw_pos.Top());*/
			//hdc.DrawLine(draw_pos.Left() + bdr_left, draw_pos.Top() + bdr_top, draw_pos.Left(), draw_pos.Top(), 1, borders.left.color);
			p << Point(draw_pos.Left() + bdr_left, draw_pos.Top() + bdr_top);
			p << Point(draw_pos.Left(), draw_pos.Top());
		}

		if(r_bottom)
		{
			p << Point(draw_pos.Left(),	draw_pos.Bottom() - r_bottom);

			double end_angle	= M_PI;
			double start_angle	= end_angle - M_PI / 2.0  / ((double) bdr_bottom / (double) bdr_left + 1);

			AddPathArc(p,
				draw_pos.Left() + r_bottom,
				draw_pos.Bottom() - r_bottom,
				r_bottom,
				r_bottom,
				end_angle,
				start_angle, true);

			AddPathArc(p,
				draw_pos.Left() + r_bottom,
				draw_pos.Bottom() - r_bottom,
				r_bottom - bdr_left,
				r_bottom - bdr_left + (bdr_left - bdr_bottom),
				start_angle,
				end_angle, false);
		} else
		{
			/*cairo_line_to(cr, draw_pos.Left(),	draw_pos.Bottom());
			cairo_line_to(cr, draw_pos.Left() + bdr_left,	draw_pos.Bottom() - bdr_bottom);*/
			//hdc.DrawLine(draw_pos.Left(),	draw_pos.Bottom(), draw_pos.Left() + bdr_left,	draw_pos.Bottom() - bdr_bottom, 1, borders.left.color);
			p << Point(draw_pos.Left(),	draw_pos.Bottom());
			p << Point(draw_pos.Left() + bdr_left,	draw_pos.Bottom() - bdr_bottom);
		}

		//cairo_fill(cr);
		//if(r_top && r_bottom) hdc.DrawRect(draw_pos.Left(), draw_pos.Bottom() - bdr_bottom, bdr_left, bdr_bottom, borders.left.color);
		//hdc.DrawPolygon(p, borders.left.color);
		hdc.DrawPolyline(p, borders.left.width, borders.left.color);
	}
	//cairo_restore(cr);
	
	PERFMETER_END(PERF_DRAWBORDERS);
}


void HtmlCtrl::TransformText(String& text, TextTransform tt) {
	LOG("TransformText");
	
}

void HtmlCtrl::SetClip(const Position& pos, const BorderRadiuses& bdr_radius, bool valid_x, bool valid_y) {
	LOG("SetClip");
	/*litehtml::position clip_pos = pos;
	litehtml::position client_pos;
	get_client_rect(client_pos);
	if(!valid_x)
	{
		clip_pos.x		= client_pos.x;
		clip_pos.width	= client_pos.width;
	}
	if(!valid_y)
	{
		clip_pos.y		= client_pos.y;
		clip_pos.height	= client_pos.height;
	}
	m_clips.emplace_back(clip_pos, bdr_radius);*/
}

void HtmlCtrl::DelClip() {
	LOG("DelClip");
	/*if(!m_clips.empty())
	{
		m_clips.RemoveLast();
	}*/
}

// Create GUI-widgets, like Button, EditString, DropList...
Element* HtmlCtrl::CreateElement(const String& tag_name, const StrMap& attributes, Document* doc) {
	//LOG("CreateElement: " << tag_name);
	return 0;
	/*if (!t_strcasecmp(tag_name, "input"))
	{
		auto iter = attributes.find("type");
		if (iter != attributes.end())
		{
			if (!t_strcasecmp(iter->second.c_str(), "text"))
			{
				if (m_omnibox)
				{
					m_omnibox->Unref();
					m_omnibox = nullptr;
				}

				m_omnibox = new el_omnibox(doc, m_hWnd, this);
				m_omnibox->Ref();
				return m_omnibox;
			}
		}
	}
	return 0;*/
}

void HtmlCtrl::GetMediaFeatures(MediaFeatures& media) {
	Position client;
	Rect workarea 		= GetWorkArea();
	media.type			= media_type_screen;
	media.width			= client.width;
	media.height		= client.height;
	media.device_width	= workarea.Width();// Gdk::screen_width();
	media.device_height	= workarea.Height(); //Gdk::screen_height();
	media.color			= 8;
	media.monochrome	= 0;
	media.color_index	= 256;
	media.resolution	= 96;
}


	//virtual void						MakeUrl( LPCWSTR url, LPCWSTR basepath, String& out ) = 0;
	//virtual image_ptr					GetImage(LPCWSTR url, bool redraw_on_ready) = 0;
	//virtual void						GetClientRect(Position& client) = 0;
	
void HtmlCtrl::SetCaption(const String& caption) {
	
}

void HtmlCtrl::SetBaseUrl(const String& base_url) {
	LOG("SetBaseUrl " << base_url);
}

void HtmlCtrl::Link(Document* doc, Element::ptr el) {
	
}

void HtmlCtrl::OnAnchorClick(const String& url, Element::ptr el) {
	
	
}

void HtmlCtrl::SetCursor(const String& cursor) {
	
}

void HtmlCtrl::ImportCss(String& text, const String& url, String& baseurl) {
	LOG("ImportCss " << url << " " << baseurl);
}

void HtmlCtrl::GetClientRect(Position& client) {
	
}

		
	
void HtmlCtrl::DrawEllipse(Draw& hdc, int x, int y, int width, int height, const WebColor& color, double line_width) {
	LOG("DrawEllipse");
	
	/*if(!cr) return;
	cairo_save(cr);

	apply_clip(cr);

	cairo_new_path(cr);

	cairo_translate (cr, x + width / 2.0, y + height / 2.0);
	cairo_scale (cr, width / 2.0, height / 2.0);
	cairo_arc (cr, 0, 0, 1, 0, 2 * M_PI);

	set_color(cr, color);
	cairo_set_line_width(cr, line_width);
	cairo_stroke(cr);

	cairo_restore(cr);*/
}

void HtmlCtrl::FillEllipse(Draw& hdc, int x, int y, int width, int height, const WebColor& color) {
	LOG("FillEllipse");
	
	/*if(!cr) return;
	cairo_save(cr);

	apply_clip(cr);

	cairo_new_path(cr);

	cairo_translate (cr, x + width / 2.0, y + height / 2.0);
	cairo_scale (cr, width / 2.0, height / 2.0);
	cairo_arc (cr, 0, 0, 1, 0, 2 * M_PI);

	set_color(cr, color);
	cairo_fill(cr);

	cairo_restore(cr);*/
}

void HtmlCtrl::RoundedRectangle(Draw& hdc, const Position &pos, const BorderRadiuses &radius ) {
	LOG("RoundedRectangle");
	
/*	cairo_new_path(cr);
	if(radius.top_left_x)
	{
		cairo_arc(cr, pos.Left() + radius.top_left_x, pos.Top() + radius.top_left_x, radius.top_left_x, M_PI, M_PI * 3.0 / 2.0);
	} else
	{
		cairo_move_to(cr, pos.Left(), pos.Top());
	}

	cairo_line_to(cr, pos.Right() - radius.top_right_x, pos.Top());

	if(radius.top_right_x)
	{
		cairo_arc(cr, pos.Right() - radius.top_right_x, pos.Top() + radius.top_right_x, radius.top_right_x, M_PI * 3.0 / 2.0, 2.0 * M_PI);
	}

	cairo_line_to(cr, pos.Right(), pos.Bottom() - radius.bottom_right_x);

	if(radius.bottom_right_x)
	{
		cairo_arc(cr, pos.Right() - radius.bottom_right_x, pos.Bottom() - radius.bottom_right_x, radius.bottom_right_x, 0, M_PI / 2.0);
	}

	cairo_line_to(cr, pos.Left() - radius.bottom_left_x, pos.Bottom());

	if(radius.bottom_left_x)
	{
		cairo_arc(cr, pos.Left() + radius.bottom_left_x, pos.Bottom() - radius.bottom_left_x, radius.bottom_left_x, M_PI / 2.0, M_PI);
	}*/
}


END_HTMLCTRL_NAMESPACE
	