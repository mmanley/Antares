/*
 * Copyright 2002-2009, Antares. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Michael Pfeiffer
 */

#include "pr_server.h"
#include "Printer.h"
#include "PrintServerApp.h"
#include "ConfigWindow.h"
#include "PrintUtils.h"

// posix
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// BeOS
#include <Application.h>
#include <Autolock.h>
#include <Debug.h>
#include <Window.h>

// Antares
#include <Catalog.h>
#include <Layout.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>


#undef TR_CONTEXT
#define TR_CONTEXT "ConfigWindow"


static const float a0_width = 2380.0;
static const float a0_height = 3368.0;
static const float a1_width = 1684.0;
static const float a1_height = 2380.0;
static const float a2_width = 1190.0;
static const float a2_height = 1684.0;
static const float a3_width = 842.0;
static const float a3_height = 1190.0;
static const float a4_width = 595.0;
static const float a4_height = 842.0;
static const float a5_width = 421.0;
static const float a5_height = 595.0;
static const float a6_width = 297.0;
static const float a6_height = 421.0;
static const float b5_width = 501.0;
static const float b5_height = 709.0;
static const float letter_width = 612.0;
static const float letter_height = 792.0;
static const float legal_width  = 612.0;
static const float legal_height  = 1008.0;
static const float ledger_width = 1224.0;
static const float ledger_height = 792.0;
static const float tabloid_width = 792.0;
static const float tabloid_height = 1224.0;


static struct PageFormat
{
	const char  *label;
	float width;
	float height;
} pageFormat[] =
{
	{TR_MARK("Letter"), letter_width, letter_height },
	{TR_MARK("Legal"),  legal_width,  legal_height  },
	{TR_MARK("Ledger"), ledger_width, ledger_height  },
	{TR_MARK("Tabloid"), tabloid_width, tabloid_height  },
	{TR_MARK("A0"),     a0_width,     a0_height     },
	{TR_MARK("A1"),     a1_width,     a1_height     },
	{TR_MARK("A2"),     a2_width,     a2_height     },
	{TR_MARK("A3"),     a3_width,     a3_height     },
	{TR_MARK("A4"),     a4_width,     a4_height     },
	{TR_MARK("A5"),     a5_width,     a5_height     },
	{TR_MARK("A6"),     a6_width,     a6_height     },
	{TR_MARK("B5"),     b5_width,     b5_height     },
};


static void GetPageFormat(float w, float h, BString& label)
{
	w = floor(w + 0.5); h = floor(h + 0.5);
	for (uint i = 0; i < sizeof(pageFormat) / sizeof(struct PageFormat); i ++) {
		struct PageFormat& pf = pageFormat[i];
		if (pf.width == w && pf.height == h || pf.width == h
			&& pf.height == w) {
			label = be_catalog->GetString(pf.label, TR_CONTEXT);
			return;
		}
	}

	float unit = 72.0; // currently inches only
	label << (w / unit) << "x" << (h / unit) << " in.";
}


static BGroupLayoutBuilder
LeftAlign(BView* view)
{
	return BGroupLayoutBuilder(B_HORIZONTAL)
		.Add(view)
		.AddGlue();
}


ConfigWindow::ConfigWindow(config_setup_kind kind, Printer* defaultPrinter,
	BMessage* settings, AutoReply* sender)
	: BWindow(ConfigWindow::GetWindowFrame(), TR("Page setup"),
		B_TITLED_WINDOW,
		B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS)
	, fKind(kind)
	, fDefaultPrinter(defaultPrinter)
	, fSettings(settings)
	, fSender(sender)
	, fCurrentPrinter(NULL)
	, fPageFormatText(NULL)
	, fJobSetupText(NULL)
{
	MimeTypeForSender(settings, fSenderMimeType);
	PrinterForMimeType();

	if (kind == kJobSetup)
		SetTitle(TR("Print setup"));

	BView* panel = new BBox(Bounds(), "temporary", B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(panel);

	BRect dummyRect(0, 0, 1, 1);

	// print selection pop up menu
	BPopUpMenu* menu = new BPopUpMenu(TR("Select a printer"));
	SetupPrintersMenu(menu);

	fPrinters = new BMenuField(TR("Printer:"), menu, NULL);

	// page format button
	fPageSetup = AddPictureButton(panel, dummyRect, "Paper setup",
		"PAGE_SETUP_ON", "PAGE_SETUP_OFF", MSG_PAGE_SETUP);

	// add description to button
	BStringView *pageFormatTitle = new BStringView("paperSetupTitle",
		TR("Paper setup:"));
	fPageFormatText = new BStringView("paperSetupText", "");

	// page selection button
	fJobSetup = NULL;
	BStringView* jobSetupTitle = NULL;
	if (kind == kJobSetup) {
		fJobSetup = AddPictureButton(panel, dummyRect, "Page setup",
			"JOB_SETUP_ON", "JOB_SETUP_OFF", MSG_JOB_SETUP);
		// add description to button
		jobSetupTitle = new BStringView("jobSetupTitle", TR("Page setup:"));
		fJobSetupText = new BStringView("jobSetupText", "");
	}

	// separator line
	BBox* separator = new BBox(dummyRect, "line",
		B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	separator->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, 1));

	// Cancel & OK button
	BButton* cancel = new BButton(dummyRect, "Cancel", TR("Cancel"),
		new BMessage(B_QUIT_REQUESTED));
	fOk = new BButton(dummyRect, "OK", TR("OK"), new BMessage(MSG_OK));

	RemoveChild(panel);

	SetLayout(new BGroupLayout(B_VERTICAL));
	BGroupLayoutBuilder builder(B_VERTICAL);

	builder
		.Add(fPrinters)
		.AddStrut(5)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL)
				.Add(fPageSetup)
				.AddStrut(5)
				.Add(BGroupLayoutBuilder(B_VERTICAL)
						.Add(LeftAlign(pageFormatTitle))
						.Add(LeftAlign(fPageFormatText))
				)
				.AddGlue()
		);

	if (fJobSetup != NULL) {
		builder
			.AddStrut(5)
			.Add(BGroupLayoutBuilder(B_HORIZONTAL)
					.Add(fJobSetup)
					.AddStrut(5)
					.Add(BGroupLayoutBuilder(B_VERTICAL)
							.Add(LeftAlign(jobSetupTitle))
							.Add(LeftAlign(fJobSetupText))
					)
					.AddGlue()
			);
	}

	builder
		.AddStrut(5)
		.Add(separator)
		.AddStrut(5)
		.Add(BGroupLayoutBuilder(B_HORIZONTAL)
				.AddGlue()
				.Add(cancel)
				.AddStrut(5)
				.Add(fOk)
				.AddGlue()
		)
		.SetInsets(5, 5, 5, 5);

	AddChild(builder);

	AddShortcut('a', 0, new BMessage(B_ABOUT_REQUESTED));

	SetDefaultButton(fOk);

	fPrinters->MakeFocus(true);

	UpdateSettings(true);
}

ConfigWindow::~ConfigWindow()
{
	if (fCurrentPrinter)
		fCurrentPrinter->Release();
	release_sem(fFinished);
}


void ConfigWindow::Go()
{
	sem_id sid = create_sem(0, "finished");
	if (sid >= 0) {
		fFinished = sid;
		Show();
		acquire_sem(sid);
		delete_sem(sid);
	} else {
		Quit();
	}
}


void ConfigWindow::MessageReceived(BMessage* m)
{
	switch (m->what) {
		case MSG_PAGE_SETUP:
			Setup(kPageSetup);
			break;
		case MSG_JOB_SETUP:
			Setup(kJobSetup);
			break;
		case MSG_PRINTER_SELECTED: {
				BString printer;
				if (m->FindString("name", &printer) == B_OK) {
					UpdateAppSettings(fSenderMimeType.String(),
						printer.String());
					PrinterForMimeType();
					UpdateSettings(true);
				}
			}
			break;
		case MSG_OK:
			UpdateSettings(false);
			if (fKind == kPageSetup)
				fSender->SetReply(&fPageSettings);
			else
				fSender->SetReply(&fJobSettings);
			Quit();
			break;
		case B_ABOUT_REQUESTED: AboutRequested();
			break;
		default:
			BWindow::MessageReceived(m);
	}
}


static const char*
kAbout =
"Printer server\n"
"© 2001-2010 Antares, Inc.\n"
"\n"
"\tIthamar R. Adema\n"
"\tMichael Pfeiffer\n"
;


void
ConfigWindow::AboutRequested()
{
	BAlert *about = new BAlert("About printer server", kAbout, TR("OK"));
	about->Go();
}


void ConfigWindow::FrameMoved(BPoint p)
{
	BRect frame = GetWindowFrame();
	frame.OffsetTo(p);
	SetWindowFrame(frame);
}


BRect ConfigWindow::GetWindowFrame()
{
	BAutolock lock(gLock);
	if (lock.IsLocked())
		return Settings::GetSettings()->ConfigWindowFrame();

	return BRect(30, 30, 300, 300);
}


void ConfigWindow::SetWindowFrame(BRect r)
{
	BAutolock lock(gLock);
	if (lock.IsLocked())
		Settings::GetSettings()->SetConfigWindowFrame(r);
}


BPictureButton* ConfigWindow::AddPictureButton(BView* panel, BRect frame,
	const char* name, const char* on, const char* off, uint32 what)
{
	BBitmap* onBM = LoadBitmap(on);
	BBitmap* offBM = LoadBitmap(off);

	BPicture* onPict = BitmapToPicture(panel, onBM);
	BPicture* offPict = BitmapToPicture(panel, offBM);

	BPictureButton* button = NULL;

	if (onPict != NULL && offPict != NULL) {
		button = new BPictureButton(frame, name, onPict, offPict,
			new BMessage(what));
		button->SetViewColor(B_TRANSPARENT_COLOR);
		panel->AddChild(button);
		onBM->Lock();
		int32 width = (int32)onBM->Bounds().Width();
		int32 height = (int32)onBM->Bounds().Height();
		button->ResizeTo(width, height);
		button->SetExplicitMaxSize(BSize(width, height));
		onBM->Unlock();
		panel->RemoveChild(button);

		BPicture* disabled = BitmapToGrayedPicture(panel, offBM);
		button->SetDisabledOn(disabled);
		delete disabled;

		disabled = BitmapToGrayedPicture(panel, onBM);
		button->SetDisabledOff(disabled);
		delete disabled;
	}

	delete onPict; delete offPict;
	delete onBM; delete offBM;

	return button;
}


void ConfigWindow::PrinterForMimeType()
{
	BAutolock lock(gLock);
	if (fCurrentPrinter) {
		fCurrentPrinter->Release();
		fCurrentPrinter = NULL;
	}

	if (lock.IsLocked()) {
		Settings* s = Settings::GetSettings();
		AppSettings* app = s->FindAppSettings(fSenderMimeType.String());
		if (app) {
			fPrinterName = app->GetPrinter();
		} else {
			fPrinterName = fDefaultPrinter ? fDefaultPrinter->Name() : "";
		}
		fCurrentPrinter = Printer::Find(fPrinterName);
		if (fCurrentPrinter)
			fCurrentPrinter->Acquire();
	}
}


void ConfigWindow::SetupPrintersMenu(BMenu* menu)
{
	// clear menu
	while (menu->CountItems() != 0)
		delete menu->RemoveItem(0L);

	// fill menu with printer names
	BAutolock lock(gLock);
	if (lock.IsLocked()) {
		BString n;
		BMessage* m;
		BMenuItem* item;
		for (int i = 0; i < Printer::CountPrinters(); i ++) {
			Printer::At(i)->GetName(n);
			m = new BMessage(MSG_PRINTER_SELECTED);
			m->AddString("name", n.String());
			menu->AddItem(item = new BMenuItem(n.String(), m));
			if (n == fPrinterName)
				item->SetMarked(true);
		}
	}
}


void ConfigWindow::UpdateAppSettings(const char* mime, const char* printer)
{
	BAutolock lock(gLock);
	if (lock.IsLocked()) {
		Settings* s = Settings::GetSettings();
		AppSettings* app = s->FindAppSettings(mime);
		if (app)
			app->SetPrinter(printer);
		else
			s->AddAppSettings(new AppSettings(mime, printer));
	}
}


void ConfigWindow::UpdateSettings(bool read)
{
	BAutolock lock(gLock);
	if (lock.IsLocked()) {
		Settings* s = Settings::GetSettings();
		PrinterSettings* p = s->FindPrinterSettings(fPrinterName.String());
		if (p == NULL) {
			p = new PrinterSettings(fPrinterName.String());
			s->AddPrinterSettings(p);
		}
		ASSERT(p != NULL);
		if (read) {
			fPageSettings = *p->GetPageSettings();
			fJobSettings = *p->GetJobSettings();
		} else {
			p->SetPageSettings(&fPageSettings);
			p->SetJobSettings(&fJobSettings);
		}
	}
	UpdateUI();
}


void ConfigWindow::UpdateUI()
{
	if (fCurrentPrinter == NULL) {
		fPageSetup->SetEnabled(false);
		if (fJobSetup) {
			fJobSetup->SetEnabled(false);
			fJobSetupText->SetText(TR("Undefined"));
		}
		fOk->SetEnabled(false);
		fPageFormatText->SetText(TR("Undefined"));
	} else {
		fPageSetup->SetEnabled(true);

		if (fJobSetup)
			fJobSetup->SetEnabled(fKind == kJobSetup
				&& !fPageSettings.IsEmpty());

		fOk->SetEnabled(fKind == kJobSetup && !fJobSettings.IsEmpty() ||
			fKind == kPageSetup && !fPageSettings.IsEmpty());

		// display information about page format
		BRect paperRect;
		BString pageFormat;
		if (fPageSettings.FindRect(PSRV_FIELD_PAPER_RECT, &paperRect) == B_OK) {
			GetPageFormat(paperRect.Width(), paperRect.Height(), pageFormat);

			int32 orientation = 0;
			fPageSettings.FindInt32(PSRV_FIELD_ORIENTATION, &orientation);
			if (orientation == 0)
				pageFormat << ", " << TR("Portrait");
			else
				pageFormat << ", " << TR("Landscape");
		} else {
			pageFormat << TR("Undefined");
		}
		fPageFormatText->SetText(pageFormat.String());

			// display information about job
		if (fKind == kJobSetup) {
			BString job;
			int32 first, last;
			if (fJobSettings.FindInt32(PSRV_FIELD_FIRST_PAGE, &first) == B_OK &&
				fJobSettings.FindInt32(PSRV_FIELD_LAST_PAGE, &last) == B_OK) {
				if (first >= 1 && first <= last && last != INT_MAX) {
					job << TR("Page") << " " << first << " " << TR("to") << " "
						<< last;
				} else {
					job << TR("All pages");
				}
				int32 copies;
				if (fJobSettings.FindInt32(PSRV_FIELD_COPIES, &copies)
					== B_OK && copies > 1) {
					job << ", " << copies << " " << TR("copies");
				}
			} else {
				job << TR("Undefined");
			}
			fJobSetupText->SetText(job.String());
		}
	}
}


void ConfigWindow::Setup(config_setup_kind kind)
{
	if (fCurrentPrinter) {
		Hide();
		if (kind == kPageSetup) {
			BMessage settings = fPageSettings;
			if (fCurrentPrinter->ConfigurePage(settings) == B_OK) {
				fPageSettings = settings;
				if (!fJobSettings.IsEmpty())
					AddFields(&fJobSettings, &fPageSettings);
			}
		} else {
			BMessage settings;
			if (fJobSettings.IsEmpty()) settings = fPageSettings;
			else settings = fJobSettings;

			if (fCurrentPrinter->ConfigureJob(settings) == B_OK)
				fJobSettings = settings;
		}
		UpdateUI();
		Show();
	}
}