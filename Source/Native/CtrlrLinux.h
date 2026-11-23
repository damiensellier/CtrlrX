#ifndef CTRLR_LINUX_H
#define CTRLR_LINUX_H

#include "CtrlrNative.h"

class CtrlrManager;

class CtrlrLinux : public CtrlrNative
{
	public:
		CtrlrLinux(CtrlrManager &_owner);
		~CtrlrLinux();
		void exportWithDefaultPanel(CtrlrPanel*  panelToWrite, const bool isRestricted, const bool signPanel,
		                            std::function<void (juce::Result)> callback);
		const Result getDefaultPanel(MemoryBlock& dataToWrite);
		const Result getDefaultResources(MemoryBlock& dataToWrite);
        const Result sendKeyPressEvent(const KeyPress &event);
    private:
        const Result writeInstance(CtrlrPanel *panelToWrite, const File &me, File chosenFile,
                                   const bool isVST3, const bool isVST2, const bool isRestricted);
        CtrlrManager &owner;
};

#endif
