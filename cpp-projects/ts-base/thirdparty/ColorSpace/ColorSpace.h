

/*******************************************************************************
** ColorSpace: https://github.com/berendeanicolae/ColorSpace/tree/master      **
** MIT License                                                                **
** Copyright (c) 2017 Berendea Nicolae                                        **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H

#include <typeinfo>
#include "Conversion.h"

namespace ColorSpace {
	struct IColorSpace {
		IColorSpace() {}
		virtual ~IColorSpace() {}

		virtual void Initialize(Rgb *color) = 0;
		virtual void ToRgb(Rgb *color) = 0;
		virtual void Copy(IColorSpace *color) = 0;

		template <typename TColorSpace>
		void To(TColorSpace *color);
	};


	struct Rgb : public IColorSpace {
		double r, g, b;

		Rgb();
		Rgb(double r, double g, double b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Xyz : public IColorSpace {
		double x, y, z;

		Xyz();
		Xyz(double x, double y, double z);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Hsl : public IColorSpace {
		double h, s, l;

		Hsl();
		Hsl(double h, double s, double l);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Lab : public IColorSpace {
		double l, a, b;

		Lab();
		Lab(double l, double a, double b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Lch : public IColorSpace {
		double l, c, h;

		Lch();
		Lch(double l, double c, double h);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Luv : public IColorSpace {
		double l, u, v;

		Luv();
		Luv(double l, double u, double v);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Yxy : public IColorSpace {
		double y1, x, y2;

		Yxy();
		Yxy(double y1, double x, double y2);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Cmy : public IColorSpace {
		double c, m, y;

		Cmy();
		Cmy(double c, double m, double y);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Cmyk : public IColorSpace {
		double c, m, y, k;

		Cmyk();
		Cmyk(double c, double m, double y, double k);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Hsv : public IColorSpace {
		double h, s, v;

		Hsv();
		Hsv(double h, double s, double v);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct Hsb : public IColorSpace {
		double h, s, b;

		Hsb();
		Hsb(double h, double s, double b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	struct HunterLab : public IColorSpace {
		double l, a, b;

		HunterLab();
		HunterLab(double l, double a, double b);

		virtual void Initialize(Rgb *color);
		virtual void ToRgb(Rgb *color);
		virtual void Copy(IColorSpace *color);
	};

	template <typename TColorSpace>
	void IColorSpace::To(TColorSpace *color) {
		Rgb rgb;

		if (typeid(*this) == typeid(*color)) {
			this->Copy(color);
		}
		else {
			this->ToRgb(&rgb);
			IConverter<TColorSpace>::ToColorSpace(&rgb, color);
		}
	}

}

#endif // COLOR_SPACE_H

