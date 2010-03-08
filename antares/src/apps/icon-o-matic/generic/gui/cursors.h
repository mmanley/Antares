/*
 * Copyright 2006, Antares.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Stephan Aßmus <superstippi@gmx.de>
 */

#ifndef CURSORS_H
#define CURSORS_H

const unsigned char kEmptyCursor[] = { 16, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


const unsigned char kDropperCursor[] = { 16, 1, 14, 1,
	0x00, 0x0e, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0xff,
	0x00, 0x7e, 0x00, 0xb8, 0x01, 0x18, 0x03, 0x28,
	0x04, 0x40, 0x0c, 0x80, 0x11, 0x00, 0x32, 0x00,
	0x44, 0x00, 0x48, 0x00, 0x30, 0x00, 0x00, 0x00,

	0x00, 0x0e, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0xff,
	0x00, 0x7e, 0x00, 0xf8, 0x01, 0xf8, 0x03, 0xe8,
	0x07, 0xc0, 0x0f, 0x80, 0x1f, 0x00, 0x3e, 0x00,
	0x7c, 0x00, 0x78, 0x00, 0x30, 0x00, 0x00, 0x00 };

const unsigned char kHandCursor[] = { 16, 1, 8, 9,
	0x01, 0x80, 0x1a, 0x70, 0x26, 0x48, 0x26, 0x4a,
	0x12, 0x4d, 0x12, 0x49, 0x68, 0x09, 0x98, 0x01,
	0x88, 0x02, 0x40, 0x02, 0x20, 0x02, 0x20, 0x04,
	0x10, 0x04, 0x08, 0x08, 0x04, 0x08, 0x04, 0x08,

	0x01, 0x80, 0x1b, 0xf0, 0x3f, 0xf8, 0x3f, 0xfa,
	0x1f, 0xff, 0x1f, 0xff, 0x6f, 0xff, 0xff, 0xff,
	0xff, 0xfe, 0x7f, 0xfe, 0x3f, 0xfe, 0x3f, 0xfc,
	0x1f, 0xfc, 0x0f, 0xf8, 0x07, 0xf8, 0x07, 0xf8 };

const unsigned char kCopyCursor[] = { 16, 1, 1, 1,
	0x00, 0x00, 0x70, 0x00, 0x48, 0x00, 0x48, 0x00,
	0x27, 0xc0, 0x24, 0xb8, 0x12, 0x54, 0x10, 0x02,
	0x79, 0xe2, 0x99, 0x22, 0x85, 0x7a, 0x61, 0x4a,
	0x19, 0xca, 0x04, 0x4a, 0x02, 0x78, 0x00, 0x00,

	0x00, 0x00, 0x70, 0x00, 0x78, 0x00, 0x78, 0x00,
	0x3f, 0xc0, 0x3f, 0xf8, 0x1f, 0xfc, 0x1f, 0xfe,
	0x7f, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0x7f, 0xfe,
	0x1f, 0xfe, 0x07, 0xfe, 0x03, 0xf8, 0x00, 0x00 };

const unsigned char kStopCursor[] = { 16, 1, 8, 9,
	0x07, 0xe0, 0x18, 0x18, 0x20, 0x04, 0x47, 0xe2,
	0x48, 0x42, 0x90, 0x89, 0x91, 0x19, 0x92, 0x29,
	0x94, 0x49, 0x98, 0x89, 0x91, 0x09, 0x42, 0x12,
	0x47, 0xe2, 0x20, 0x04, 0x18, 0x18, 0x07, 0xe0,

	0x07, 0xe0, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe,
	0x78, 0x7e, 0xf0, 0xff, 0xf1, 0xff, 0xf3, 0xef,
	0xf7, 0xcf, 0xff, 0x8f, 0xff, 0x0f, 0x7e, 0x1e,
	0x7f, 0xfe, 0x3f, 0xfc, 0x1f, 0xf8, 0x07, 0xe0 };

const unsigned char kGrabCursor[] = { 16, 1, 8, 9,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0d, 0xb0, 0x12, 0x4c, 0x10, 0x0a, 0x08, 0x02,
	0x18, 0x02, 0x20, 0x02, 0x20, 0x02, 0x20, 0x04,
	0x10, 0x04, 0x08, 0x08, 0x04, 0x08, 0x04, 0x08,

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x0d, 0xb0, 0x1f, 0xfc, 0x1f, 0xfe, 0x0f, 0xfe,
	0x1f, 0xfe, 0x3f, 0xfe, 0x3f, 0xfe, 0x3f, 0xfc,
	0x1f, 0xfc, 0x0f, 0xf8, 0x07, 0xf8, 0x07, 0xf8 };

const unsigned char kFillBucketCursor[] = { 16, 1, 15, 2,
	0x00, 0xe0, 0x01, 0x10, 0x01, 0x90, 0x03, 0x50,
	0x0d, 0x30, 0x39, 0x10, 0x71, 0x08, 0xe2, 0x84,
	0xa1, 0x02, 0xb0, 0x01, 0xa8, 0x01, 0xa4, 0x02,
	0xa2, 0x04, 0xa1, 0x08, 0x60, 0x90, 0x20, 0x60,

	0x00, 0xe0, 0x01, 0x10, 0x01, 0x90, 0x03, 0xd0,
	0x0f, 0xf0, 0x3f, 0xf0, 0x7f, 0xf8, 0xff, 0xfc,
	0xff, 0xfe, 0xff, 0xff, 0xef, 0xff, 0xe7, 0xfe,
	0xe3, 0xfc, 0xe1, 0xf8, 0x60, 0xf0, 0x20, 0x60 };

// ----------- Transformation cursors

const unsigned char kMoveCursor[] = { 16, 1, 8, 8,
	0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x08, 0x10,
	0x1e, 0x78, 0x2a, 0x54, 0x4e, 0x72, 0x80, 0x01,
	0x80, 0x01, 0x4e, 0x72, 0x2a, 0x54, 0x1e, 0x78,
	0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80,

	0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0,
	0x1f, 0xf8, 0x3b, 0xdc, 0x7f, 0xfe, 0xff, 0xff,
	0xff, 0xff, 0x7f, 0xfe, 0x3b, 0xdc, 0x1f, 0xf8,
	0x0f, 0xf0, 0x07, 0xe0, 0x03, 0xc0, 0x01, 0x80 };

const unsigned char kLeftRightCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x18, 0x18, 0x28, 0x14, 0x4f, 0xf2, 0x80, 0x01,
	0x80, 0x01, 0x4f, 0xf2, 0x28, 0x14, 0x18, 0x18,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x18, 0x18, 0x38, 0x1c, 0x7f, 0xfe, 0xff, 0xff,
	0xff, 0xff, 0x7f, 0xfe, 0x38, 0x1c, 0x18, 0x18,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const unsigned char kUpDownCursor[] = { 16, 1, 8, 8,
	0x01, 0x80, 0x02, 0x40, 0x04, 0x20, 0x08, 0x10,
	0x0e, 0x70, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40,
	0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x0e, 0x70,
	0x08, 0x10, 0x04, 0x20, 0x02, 0x40, 0x01, 0x80,

	0x01, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0,
	0x0f, 0xf0, 0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0,
	0x03, 0xc0, 0x03, 0xc0, 0x03, 0xc0, 0x0f, 0xf0,
	0x0f, 0xf0, 0x07, 0xe0, 0x03, 0xc0, 0x01, 0x80 };

const unsigned char kLeftTopRightBottomCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x21, 0x00,
	0x22, 0x00, 0x21, 0x00, 0x28, 0x80, 0x34, 0x40,
	0x02, 0x2c, 0x01, 0x14, 0x00, 0x84, 0x00, 0x44,
	0x00, 0x84, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x3f, 0x00,
	0x3e, 0x00, 0x3f, 0x00, 0x3f, 0x80, 0x37, 0xc0,
	0x03, 0xec, 0x01, 0xfc, 0x00, 0xfc, 0x00, 0x7c,
	0x00, 0xfc, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00 };

const unsigned char kLeftBottomRightTopCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x84,
	0x00, 0x44, 0x00, 0x84, 0x01, 0x14, 0x02, 0x2c,
	0x34, 0x40, 0x28, 0x80, 0x21, 0x00, 0x22, 0x00,
	0x21, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0xfc,
	0x00, 0x7c, 0x00, 0xfc, 0x01, 0xfc, 0x03, 0xec,
	0x37, 0xc0, 0x3f, 0x80, 0x3f, 0x00, 0x3e, 0x00,
	0x3f, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00 };

const unsigned char kRotateLCursor[] = { 16, 1, 8, 8,
	0x01, 0x80, 0x03, 0x40, 0x0c, 0x20, 0x10, 0x10,
	0x23, 0x20, 0x25, 0x40, 0x49, 0x80, 0x48, 0x00,
	0x48, 0x00, 0x49, 0x80, 0x25, 0x40, 0x23, 0x20,
	0x10, 0x10, 0x0c, 0x20, 0x03, 0x40, 0x01, 0x80,

	0x01, 0x80, 0x03, 0xc0, 0x0f, 0xe0, 0x1f, 0xf0,
	0x3f, 0xe0, 0x3d, 0xc0, 0x79, 0x80, 0x78, 0x00,
	0x78, 0x00, 0x79, 0x80, 0x3d, 0xc0, 0x3f, 0xe0,
	0x1f, 0xf0, 0x0f, 0xe0, 0x03, 0xc0, 0x01, 0x80 };

const unsigned char kRotateLBCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x3f, 0x00, 0x21, 0x00, 0x11, 0x00,
	0x21, 0x00, 0x25, 0x00, 0x4b, 0x00, 0x48, 0x00,
	0x48, 0x7e, 0x48, 0x42, 0x24, 0x22, 0x23, 0xc2,
	0x10, 0x0a, 0x0c, 0x36, 0x03, 0xc0, 0x00, 0x00,

	0x00, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x1f, 0x00,
	0x3f, 0x00, 0x3f, 0x00, 0x7b, 0x00, 0x78, 0x00,
	0x78, 0x7e, 0x78, 0x7e, 0x3c, 0x3e, 0x3f, 0xfe,
	0x1f, 0xfe, 0x0f, 0xf6, 0x03, 0xc0, 0x00, 0x00 };

const unsigned char kRotateBCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x08, 0x28, 0x14, 0x44, 0x22,
	0x82, 0x41, 0xce, 0x73, 0x48, 0x12, 0x24, 0x24,
	0x23, 0xc4, 0x10, 0x08, 0x0c, 0x30, 0x03, 0xc0,

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x10, 0x08, 0x38, 0x1c, 0x7c, 0x3e,
	0xfe, 0x7f, 0xfe, 0x7f, 0x78, 0x1e, 0x3c, 0x3c,
	0x3f, 0xfc, 0x1f, 0xf8, 0x0f, 0xf0, 0x03, 0xc0 };

const unsigned char kRotateRBCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x00, 0xfc, 0x00, 0x84, 0x00, 0x88,
	0x00, 0x84, 0x00, 0xa4, 0x00, 0xd2, 0x00, 0x12,
	0x7e, 0x12, 0x42, 0x12, 0x44, 0x24, 0x43, 0xc4,
	0x50, 0x08, 0x6c, 0x30, 0x03, 0xc0, 0x00, 0x00,

	0x00, 0x00, 0x00, 0xfc, 0x00, 0xfc, 0x00, 0xf8,
	0x00, 0xfc, 0x00, 0xfc, 0x00, 0xde, 0x00, 0x1e,
	0x7e, 0x1e, 0x7e, 0x1e, 0x7c, 0x3c, 0x7f, 0xfc,
	0x7f, 0xf8, 0x6f, 0xf0, 0x03, 0xc0, 0x00, 0x00 };

const unsigned char kRotateRCursor[] = { 16, 1, 8, 8,
	0x00, 0xc0, 0x01, 0x60, 0x02, 0x18, 0x04, 0x04,
	0x02, 0x62, 0x01, 0x52, 0x00, 0xc9, 0x00, 0x09,
	0x00, 0x09, 0x00, 0xc9, 0x01, 0x52, 0x02, 0x62,
	0x04, 0x04, 0x02, 0x18, 0x01, 0x60, 0x00, 0xc0,

	0x00, 0xc0, 0x01, 0xe0, 0x03, 0xf8, 0x07, 0xfc,
	0x03, 0xfe, 0x01, 0xde, 0x00, 0xcf, 0x00, 0x0f,
	0x00, 0x0f, 0x00, 0xcf, 0x01, 0xde, 0x03, 0xfe,
	0x07, 0xfc, 0x03, 0xf8, 0x01, 0xe0, 0x00, 0xc0 };

const unsigned char kRotateRTCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x03, 0xc0, 0x6c, 0x30, 0x50, 0x08,
	0x43, 0xc4, 0x44, 0x24, 0x42, 0x12, 0x7e, 0x12,
	0x00, 0x12, 0x00, 0xd2, 0x00, 0xa4, 0x00, 0x84,
	0x00, 0x88, 0x00, 0x84, 0x00, 0xfc, 0x00, 0x00,

	0x00, 0x00, 0x03, 0xc0, 0x6f, 0xf0, 0x7f, 0xf8,
	0x7f, 0xfc, 0x7c, 0x3c, 0x7e, 0x1e, 0x7e, 0x1e,
	0x00, 0x1e, 0x00, 0xde, 0x00, 0xfc, 0x00, 0xfc,
	0x00, 0xf8, 0x00, 0xfc, 0x00, 0xfc, 0x00, 0x00 };

const unsigned char kRotateTCursor[] = { 16, 1, 8, 8,
	0x03, 0xc0, 0x0c, 0x30, 0x10, 0x08, 0x23, 0xc4,
	0x24, 0x24, 0x48, 0x12, 0xce, 0x73, 0x82, 0x41,
	0x44, 0x22, 0x28, 0x14, 0x10, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x03, 0xc0, 0x0f, 0xf0, 0x1f, 0xf8, 0x3f, 0xfc,
	0x3c, 0x3c, 0x78, 0x1e, 0xfe, 0x7f, 0xfe, 0x7f,
	0x7c, 0x3e, 0x38, 0x1c, 0x10, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const unsigned char kRotateLTCursor[] = { 16, 1, 8, 8,
	0x00, 0x00, 0x03, 0xc0, 0x0c, 0x36, 0x10, 0x0a,
	0x23, 0xc2, 0x24, 0x22, 0x48, 0x42, 0x48, 0x7e,
	0x48, 0x00, 0x4b, 0x00, 0x25, 0x00, 0x21, 0x00,
	0x11, 0x00, 0x21, 0x00, 0x3f, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x03, 0xc0, 0x0f, 0xf6, 0x1f, 0xfe,
	0x3f, 0xfe, 0x3c, 0x3e, 0x78, 0x7e, 0x78, 0x7e,
	0x78, 0x00, 0x7b, 0x00, 0x3f, 0x00, 0x3f, 0x00,
	0x1f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x00, 0x00 };

// ------------- Path cursors

const unsigned char kPathNewCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x20, 0x08, 0x50, 0x09, 0x54,
	0x08, 0x88, 0x03, 0x06, 0x04, 0x01, 0x03, 0x06,
	0x00, 0x88, 0x01, 0x54, 0x00, 0x50, 0x00, 0x20,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0xa0, 0x1c, 0x70, 0x1d, 0x74,
	0x1c, 0xf8, 0x03, 0xfe, 0x07, 0xff, 0x03, 0xfe,
	0x00, 0xf8, 0x01, 0x74, 0x00, 0x70, 0x00, 0x20 };

const unsigned char kPathAddCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x00, 0x08, 0x78,
	0x08, 0x48, 0x01, 0xce, 0x01, 0x02, 0x01, 0x02,
	0x01, 0xce, 0x00, 0x48, 0x00, 0x78, 0x00, 0x00,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x00, 0x1c, 0x78,
	0x1c, 0x78, 0x01, 0xfe, 0x01, 0xfe, 0x01, 0xfe,
	0x01, 0xfe, 0x00, 0x78, 0x00, 0x78, 0x00, 0x00 };

const unsigned char kPathRemoveCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00,
	0x08, 0x00, 0x01, 0xfe, 0x01, 0x02, 0x01, 0x02,
	0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x00, 0x1c, 0x00,
	0x1c, 0x00, 0x01, 0xfe, 0x01, 0xfe, 0x01, 0xfe,
	0x01, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

const unsigned char kPathInsertCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x00, 0x09, 0xf0,
	0x09, 0x10, 0x01, 0x10, 0x00, 0xa0, 0x00, 0x40,
	0x1f, 0x1f, 0x11, 0x11, 0x1f, 0x1f, 0x00, 0x00,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x00, 0x1d, 0xf0,
	0x1d, 0xf0, 0x01, 0xf0, 0x00, 0xe0, 0x00, 0x40,
	0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00, 0x00 };

const unsigned char kPathMoveCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x20, 0x08, 0x50, 0x08, 0x88,
	0x09, 0x74, 0x02, 0x8a, 0x04, 0x89, 0x02, 0x8a,
	0x01, 0x74, 0x00, 0x88, 0x00, 0x50, 0x00, 0x20,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0xa0, 0x1c, 0x70, 0x1c, 0xf8,
	0x1d, 0x74, 0x03, 0x8e, 0x07, 0x8f, 0x03, 0x8e,
	0x01, 0x74, 0x00, 0xf8, 0x00, 0x70, 0x00, 0x20 };

const unsigned char kPathCloseCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x70, 0x08, 0x88,
	0x09, 0x04, 0x02, 0x72, 0x02, 0x52, 0x02, 0x72,
	0x01, 0x04, 0x00, 0x88, 0x00, 0x70, 0x00, 0x00,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x70, 0x1c, 0xf8,
	0x1d, 0xfc, 0x03, 0xfe, 0x03, 0xde, 0x03, 0xfe,
	0x01, 0xfc, 0x00, 0xf8, 0x00, 0x70, 0x00, 0x00 };

const unsigned char kPathSharpCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00,
	0x08, 0x00, 0x01, 0x8c, 0x02, 0x52, 0x02, 0x22,
	0x01, 0x04, 0x00, 0x88, 0x00, 0x50, 0x00, 0x20,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x00, 0x1c, 0x00,
	0x1c, 0x00, 0x01, 0x8c, 0x03, 0xde, 0x03, 0xfe,
	0x01, 0xfc, 0x00, 0xf8, 0x00, 0x70, 0x00, 0x20 };

const unsigned char kPathSelectCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00,
	0x09, 0x55, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x01, 0x55, 0x00, 0x00,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x00, 0x1c, 0x00,
	0x1d, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x00, 0x00 };

// -------------------

const unsigned char kEllipseCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x78, 0x09, 0x86,
	0x09, 0x02, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
	0x02, 0x01, 0x01, 0x02, 0x01, 0x86, 0x00, 0x78,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x78, 0x1d, 0xfe,
	0x1d, 0xfe, 0x03, 0xff, 0x03, 0xff, 0x03, 0xff,
	0x03, 0xff, 0x01, 0xfe, 0x01, 0xfe, 0x00, 0x78 };

const unsigned char kRectCursor[] = { 16, 1, 4, 4,
	0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00,
	0xe3, 0x80, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00,
	0x0b, 0xff, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
	0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x03, 0xff,

	0x1c, 0x00, 0x1c, 0x00, 0x1c, 0x00, 0xe3, 0x80,
	0xe3, 0x80, 0xe3, 0x80, 0x1c, 0x00, 0x1c, 0x00,
	0x1f, 0xff, 0x03, 0xff, 0x03, 0xff, 0x03, 0xff,
	0x03, 0xff, 0x03, 0xff, 0x03, 0xff, 0x03, 0xff };

#endif // CURSORS_H
