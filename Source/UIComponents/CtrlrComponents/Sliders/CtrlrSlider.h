#ifndef __CTRLR_SLIDER__
#define __CTRLR_SLIDER__

#include "../CtrlrComponent.h"
#include "CtrlrSliderInternal.h"
#include "CtrlrPanel/CtrlrPanelEditor.h"

class CtrlrSlider  : public CtrlrComponent,
                     public SettableTooltipClient,
                     public Slider::Listener
{
    
    public:
        CtrlrSlider (CtrlrModulator &owner);
        ~CtrlrSlider();
        Slider &getOwnedSlider() { return (ctrlrSlider); }
        void setComponentValue (const double newValue, const bool sendChangeMessage=false);
        double getComponentValue();
        int getComponentMidiValue();
        double getComponentMaxValue();
        const String getComponentText();
        const String getCurrentLF();
        const Array<Font> getFontList();
        void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property);
        void valueTreeChildrenChanged (ValueTree &treeWhoseChildHasChanged){}
        void valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged){}
        void valueTreeChildAdded (ValueTree& parentTree, ValueTree& childWhichHasBeenAdded){}
        void valueTreeChildRemoved (ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int){}
        void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved, int, int){}
        static void wrapForLua(lua_State *L);
        void resized();
        void sliderValueChanged (Slider* sliderThatWasMoved);
        void mouseUp (const MouseEvent& e);
        void customLookAndFeelChanged(LookAndFeelBase *customLookAndFeel = nullptr);
        static LookAndFeel* getLookAndFeelFromComponentProperty(const String &lookAndFeelComponentProperty);
        void resetLookAndFeelOverrides();
        void updatePropertiesPanel();
        JUCE_LEAK_DETECTOR(CtrlrSlider)

    private:
        CtrlrSliderLookAndFeel_V2 lf;
        CtrlrSliderLookAndFeel_V2 lfV2;
        CtrlrSliderLookAndFeel_V3 lfV3;
        CtrlrSliderLookAndFeel_V4 lfV4;
        CtrlrSliderInternal ctrlrSlider;
};

#endif
