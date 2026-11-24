#ifndef CTRLR_NATIVE_H
#define CTRLR_NATIVE_H

#include "JuceHeader.h"
class CtrlrPanel;
class CtrlrManager;
#define CTRLR_NEW_INSTANCE_DIALOG_TITLE		L"Write new instance here"

#define	CTRLR_INTERNAL_PANEL_RESID				1040
#define CTRLR_INTERNAL_RESOURCES_RESID			1041
#define CTRLR_INTERNAL_SIGNATURE_RESID			1042
#define CTRLR_INTERNAL_SIGNATURE_MASTER_RESID	1043

#define CTRLR_MAC_PANEL_FILE					L"PanelZ"
#define CTRLR_MAC_RESOURCES_FILE				L"ResourcesZ"
#define CTRLR_MAC_SIGNATURE_FILE				L"Signature"
#define CTRLR_MAC_SIGNATURE_MASTER_FILE			L"MasterSignature"

#define CTRLR_INTERNAL_PANEL_SECTION			"ctrlr_panel"
#define CTRLR_INTERNAL_RESOURCES_SECTION		"ctrlr_panel_resources"
#define CTRLR_INTERNAL_SIGNATURE_SECTION		"ctrlr_panel_signature"
#define CTRLR_INTERNAL_SIGNATURE_MASTER_SECTION	"ctrlr_panel_signature_master"
class CtrlrNative
{
	public:
		virtual ~CtrlrNative() = default;
		static CtrlrNative *getNativeObject(CtrlrManager &owner);
		// Updated v8.0.100. Asynchronous: the chooser is shown with launchAsync() and the callback
		// gets the result once, after the export (or at once, if the export can't start). As John
		// Goodland's (dnaldoog) JUCE 8 line.
		virtual void exportWithDefaultPanel (CtrlrPanel *,
                                                        const bool,
                                                        const bool,
                                                        std::function<void (juce::Result)> callback)
		{
		    if (callback)
		        callback (Result::fail("Native, implement me"));
        }
		virtual const Result getDefaultPanel(MemoryBlock&)												{ return (Result::fail("Native, implement me")); }
		virtual const Result getDefaultResources(MemoryBlock&)											{ return (Result::fail("Native, implement me")); }
		virtual const Result registerFileHandler()														{ return (Result::fail("Native, implement me")); }
		virtual const Result sendKeyPressEvent (const KeyPress &)                                       { return (Result::fail("Native, implement me")); }
		virtual const Result sendKeyPressEvent (const KeyPress &, const String &)						{ return (Result::fail("Native, implement me")); }
};

#endif
