#pragma once

#if !defined(__PLOTER_H__5FC0EE2E_6EFA_4841_83A6_C33363971954__)
#define __PLOTER_H__5FC0EE2E_6EFA_4841_83A6_C33363971954__

#include <math/utils.h>
#include <math/graphics.h>
#include <Images/MemoryDC.h>

struct Bounds
{
protected:
	real min_value;
	real max_value;

public:
	Bounds()
		:min_value(0)
		,max_value(1)
	{
	}

	real process(real _value) const
	{
		if(eql(min_value,max_value)) return 0;
		return (_value-min_value)/(max_value-min_value);
	}

	void set(real _value)
	{
		max_value = min_value = _value;
	}

	void minmax(real _value)
	{
		max_value = max(max_value,_value);
		min_value = min(min_value,_value);
	}

	real width() const
	{
		return max_value - min_value;
	}

	real get_min() const 
	{
		return min_value;
	}

	real get_max() const 
	{
		return max_value;
	}
};//struct Bounds

struct RectBounds
{
protected:
	Bounds x_bounds;
	Bounds y_bounds;
	real border;

public:
	RectBounds()
		:border(0)
	{
	}

	void clear()
	{
		x_bounds = Bounds();
		y_bounds = Bounds();
	}

	CPoint process(const CFPoint2D& _pt,const CSize& _imagesz,bool _FlipYMode) const
	{
		CFPoint2D pt;
		pt.x = x_bounds.process(_pt.x)*(_imagesz.cx*((real)1.0-border*(real)2.0)) + _imagesz.cx*border;
		pt.y = y_bounds.process(_pt.y)*(_imagesz.cy*((real)1.0-border*(real)2.0)) + _imagesz.cy*border;
		if(_FlipYMode)
			pt.y = _imagesz.cy - pt.y;
		return pt;
	}

	void set(const CFPoint2D& _pt)
	{
		x_bounds.set(_pt.x);
		y_bounds.set(_pt.y);
	}

	void minmax(const CFPoint2D& _pt)
	{
		x_bounds.minmax(_pt.x);
		y_bounds.minmax(_pt.y);
	}

	real get_min_x() const 
	{
		return x_bounds.get_min();
	}

	real get_min_y() const 
	{
		return y_bounds.get_min();
	}

	real get_max_x() const 
	{
		return x_bounds.get_max();
	}

	real get_max_y() const 
	{
		return y_bounds.get_max();
	}

	void set_border(real _border)
	{
		border = _border;
	}

	real get_border() const 
	{
		return max(x_bounds.width()*border,y_bounds.width()*border);
	}
};//struct RectBounds

struct ProportionalBounds : public RectBounds
{
public:
	CPoint process(const CFPoint2D& _pt,const CSize& _imagesz,bool _FlipYMode) const
	{
		CFPoint2D pt;
		if(y_bounds.width()/x_bounds.width()<(real)_imagesz.cy/_imagesz.cx)
		{
			pt.x = x_bounds.process(_pt.x)*_imagesz.cx*((real)1.0-border*(real)2.0) + _imagesz.cx*border;
			real h1 = _imagesz.cx*((real)1.0-border*(real)2.0)*y_bounds.width()/x_bounds.width();
			real h1p = (_imagesz.cy*((real)1.0-border*(real)2.0) - h1)*(real)0.5;
			pt.y = h1p + h1*y_bounds.process(_pt.y) + _imagesz.cy*border;

//			pt.x = x_bounds.process(_pt.x)*_imagesz.cx;
//			real h1 = _imagesz.cx*y_bounds.width()/x_bounds.width();
//			real h1p = (_imagesz.cy - h1)*0.5;
//			pt.y = h1p + h1*y_bounds.process(_pt.y);
		}
		else
		{
			pt.y = y_bounds.process(_pt.y)*_imagesz.cy*((real)1.0-border*(real)2.0) + _imagesz.cy*border;
			real w1 = _imagesz.cy*((real)1.0-border*(real)2.0)*x_bounds.width()/y_bounds.width();
			real w1p = (_imagesz.cx*((real)1.0-border*(real)2.0) - w1)*(real)0.5;
			pt.x = w1p + w1*x_bounds.process(_pt.x) + _imagesz.cx*border;

//			pt.y = y_bounds.process(_pt.y)*_imagesz.cy;
//			real w1 = _imagesz.cy*x_bounds.width()/y_bounds.width();
//			real w1p = (_imagesz.cx - w1)*0.5;
//			pt.x = w1p + w1*x_bounds.process(_pt.x);
		}
		if(_FlipYMode)
			pt.y = _imagesz.cy - pt.y;
		return pt;
	}
};

struct Ploter
{
protected:
	CDC* m_pDC;
	CSize m_sz;
	CPen* m_poldpen;
	CBrush* m_poldbrush;
	COLORREF m_clr;

public:
	Ploter()
		:m_pDC(NULL)
		,m_poldpen(NULL)
		,m_poldbrush(NULL)
		,m_clr(RGB(0xff,0xff,0xff))
	{
	}

	~Ploter()
	{
		if(NOT_NULL(m_pDC)) detach();
	}

	void attach(MemoryDC& _image)
	{
		VERIFY_EXIT(IS_NULL(m_pDC));

		m_pDC = new CDC();
		m_pDC->Attach(_image.getDC());
		m_sz = CSize(labs(_image.getWidth()),labs(_image.getHeight()));
	}

	void detach()
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));

		detach_brush();
		detach_pen();

		m_pDC->Detach();
		delete m_pDC;
		m_pDC = NULL;
	}

	void fill_background(COLORREF _clr = RGB(0,0,0))
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		CBrush brsh;
		brsh.CreateSolidBrush(_clr);
		CRect rcall(0,0,m_sz.cx,m_sz.cy);
		m_pDC->FillRect(rcall,&brsh);
	}

	void set_bkcolor(COLORREF _clr)
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		detach_brush();
		CBrush* pbr = new CBrush();
		pbr->CreateSolidBrush(_clr);
		m_poldbrush = m_pDC->SelectObject(pbr);
	}

	void set_color(COLORREF _clr,int _width = 1,int _pen_style = PS_SOLID)
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		detach_pen();
		CPen* ppen = new CPen();
		ppen->CreatePen(_pen_style,_width,_clr);
		m_clr = _clr;
		m_poldpen = m_pDC->SelectObject(ppen);
	}

	template<class _RectBounds>
	void point_to(const CFPoint2D& _pt,const _RectBounds& _bounds)
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		CPoint pt = _bounds.process(_pt,m_sz,true);
		//TRACE(_T("PointTo (%d,%d)\n"),pt.x,pt.y);
		m_pDC->SetPixel(pt.x,pt.y,m_clr);
	}

	void point_to(CFPoint2D& _pt)
	{
		point_to(_pt,RectBounds());
	}

	template<class _RectBounds>
	void move_to(const CFPoint2D& _pt,const _RectBounds& _bounds)
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		CPoint pt = _bounds.process(_pt,m_sz,true);
		//TRACE(_T("MoveTo (%d,%d)\n"),pt.x,pt.y);
		m_pDC->MoveTo(pt.x,pt.y);
	}

	void move_to(const CFPoint2D& _pt)
	{
		move_to(_pt,RectBounds());
	}

	template<class _RectBounds>
	void line_to(const CFPoint2D& _pt,const _RectBounds& _bounds)
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		CPoint pt = _bounds.process(_pt,m_sz,true);
		//TRACE(_T("LineTo (%d,%d)\n"),pt.x,pt.y);
		m_pDC->LineTo(pt.x,pt.y);
	}

	void line_to(const CFPoint2D& _pt)
	{
		line_to(_pt,RectBounds());
	}

	template<class _RectBounds>
	void circle(const CFPoint2D& _pt,int _d,const _RectBounds& _bounds)
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		CPoint pt = _bounds.process(_pt,m_sz,true);
		CRect rc(
			pt.x-_d/2,pt.y-_d/2
			,pt.x+_d-_d/2,pt.y+_d-_d/2
			);
		m_pDC->Ellipse(rc);
	}

	void circle(const CFPoint2D& _pt,int _d = 5)
	{
		circle(_pt,_d,RectBounds());
	}

	template<typename _RectBounds>
	void out_text(const CFPoint2D& _pt,const CString& _text,const _RectBounds& _bounds)
	{
		VERIFY_EXIT(NOT_NULL(m_pDC));
		CPoint pt = _bounds.process(_pt,m_sz,true);
		m_pDC->TextOut(pt.x,pt.y,_text);
	}

	void out_text(const CFPoint2D& _pt,const CString& _text)
	{
		out_text(_pt,_text,RectBounds());
	}

	template<typename _Iterator,class _RectBounds>
	static void measure(_Iterator _it,_Iterator _ite,_RectBounds& _bounds)
	{
		if(_it==_ite)
		{
			_bounds.clear();
			return;
		}
		_bounds.set(*_it);
		++_it;
		for(;_it!=_ite;++_it)
		{
			_bounds.minmax(*_it);
		}
	}

	template<class _RectBounds>
	static void measure(const CFPoint2D& _pt,_RectBounds& _bounds)
	{
		_bounds.minmax(_pt);
	}

	template<typename _It,class _RectBounds>
	void line(_It _it, _It _ite,const _RectBounds& _bounds)
	{
		if(_it==_ite) return;
		_It it1 = _it;
		++it1;
		if(it1!=_ite)
			move_to(*_it,_bounds);
		else
			point_to(*_it,_bounds);
		++_it;
		for(;_it!=_ite;++_it)
		{
			line_to(*_it,_bounds);
		}
	}

	template<typename _It>
	void line(_It _it, _It _ite)
	{
		line(_it,_ite,RectBounds());
	}

	template<typename _It,class _RectBounds>
	void points(_It _it, _It _ite,const _RectBounds& _bounds)
	{
		if(_it==_ite) return;
		point_to(*_it,_bounds);
		++_it;
		for(;_it!=_ite;++_it)
		{
			point_to(*_it,_bounds);
		}
	}

	template<typename _It>
	void points(_It _it, _It _ite)
	{
		points(_it,_ite,RectBounds());
	}

protected:

	void detach_brush()
	{
		if(NOT_NULL(m_poldbrush))
		{
			CBrush* pbr = m_pDC->SelectObject(m_poldbrush);
			delete pbr;
			m_poldbrush = NULL;
		}
	}

	void detach_pen()
	{
		if(NOT_NULL(m_poldpen))
		{
			CPen* ppen = m_pDC->SelectObject(m_poldpen);
			delete ppen;
			m_poldpen = NULL;
		}
	}
};

struct PloterTests
{
	PloterTests()
	{
		MemoryDC image;
		image.Create(CSize(1024,256),24,false);
		test_sin<ProportionalBounds>(image);
		image.Save(_T("ploter.test.test_sin.1.bmp"));
		test_sin<RectBounds>(image);
		image.Save(_T("ploter.test.test_sin.2.bmp"));
	}

	template<typename _Bounds>
	void test_sin(MemoryDC& _image)
	{
		Ploter ploter;
		ploter.attach(_image);
		ploter.fill_background(RGB(0x10,0x10,0x10));

		std::vector<CFPoint2D> pts;
		size_t i = 0;
		pts.resize(256);

		// prepare sin function 
		for(i=0;i<pts.size();i++)
		{
			real a = (real)i/pts.size()*3*((real)M_PI*2) - (real)M_PI/2;
			pts[i].x = a;
			pts[i].y = sin(a);
		}

		// calculate bounds of function
		_Bounds bounds;
		Ploter::measure(pts.begin(),pts.end(),bounds);

		//draw axes
		ploter.set_color(RGB(0,0x20,0));
		ploter.move_to(CFPoint2D((real)-M_PI,0),bounds);
		ploter.line_to(CFPoint2D((real)M_PI*2*3,0),bounds);
		ploter.set_color(RGB(0x20,0,0));
		ploter.move_to(CFPoint2D(0,(real)-1.5),bounds);
		ploter.line_to(CFPoint2D(0,(real)1.5),bounds);

		// plot sin function
		ploter.set_color(RGB(0,0,0x80));
		ploter.line(pts.begin(),pts.end(),bounds);
	}
};


#endif //#if !defined(__PLOTER_H__5FC0EE2E_6EFA_4841_83A6_C33363971954__)