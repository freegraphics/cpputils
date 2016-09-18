#pragma once

#include "graphics.h"

template<typename _Type>
struct Point2DTDefaultLayoutT : public Layout<Point2DT<_Type> >
{
	Point2DTDefaultLayoutT()
	{
		add_simple(_T("x"),&Point2DT<_Type>::x);
		add_simple(_T("y"),&Point2DT<_Type>::y);
	}
};


typedef Point2DTDefaultLayoutT<double> CFPoint2DTDefaultLayout;

DECLARE_OUTER_DEFAULT_LAYOUT(CFPoint2D,CFPoint2DTDefaultLayout);


template<typename _Type>
struct Point3DTDefaultLayoutT : public Layout<Point3DT<_Type> >
{
	Point3DTDefaultLayoutT()
	{
		add_simple(_T("x"),&Point3DT<_Type>::x);
		add_simple(_T("y"),&Point3DT<_Type>::y);
		add_simple(_T("z"),&Point3DT<_Type>::z);
		add_simple(_T("w"),&Point3DT<_Type>::w);
	}
};

typedef Point3DTDefaultLayoutT<double> CFPoint3DTDefaultLayout;

DECLARE_OUTER_DEFAULT_LAYOUT(CFPoint3D,CFPoint3DTDefaultLayout);

