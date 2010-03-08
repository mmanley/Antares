/*
 * Copyright 2006-2009, Antares, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _GRADIENT_RADIAL_H
#define _GRADIENT_RADIAL_H


#include <Gradient.h>


class BPoint;


// WARNING! This is experimental API and may change! Be prepared to
// recompile your software in a next version of antares.


class BGradientRadial : public BGradient {
public:
								BGradientRadial();
								BGradientRadial(const BPoint& center,
									float radius);
								BGradientRadial(float cx, float cy,
									float radius);
	
			BPoint				Center() const;
			void				SetCenter(const BPoint& center);
			void				SetCenter(float cx, float cy);
	
			float				Radius() const;
			void				SetRadius(float radius);
};

#endif // _GRADIENT_RADIAL_H
