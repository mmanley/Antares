/*****************************************************************************/
// IconView
// IconView.h
// Author: Michael Wilber
//
// This BView based object displays an icon
//
// Copyright (C) Antares, uses the MIT license
/*****************************************************************************/

#ifndef ICONVIEW_H
#define ICONVIEW_H

#include <View.h>
#include <Bitmap.h>
#include <Path.h>
#include <Entry.h>
#include <Node.h>
#include <NodeInfo.h>

class IconView : public BView {
public:
	IconView(const BRect &frame, const char *name, uint32 resize, uint32 flags);
		// sets up the view
	~IconView();
	
	bool DrawIcon(bool draw);
	
	bool SetIcon(const BPath &path);

	virtual	void Draw(BRect area);
		// draws the icon
private:
	BBitmap *fIconBitmap;
		// the icon
	bool fDrawIcon;
		// whether or not the icon is drawn
};

#endif // #ifndef ICONVIEW_H
