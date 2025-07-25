// CtrlInlineUtilityGUI.cpp
#include "CtrlrInlineUtilitiesGUI.h"
#include <juce_gui_basics/juce_gui_basics.h>

namespace gui {

// Define your custom ColourScheme getter functions here
juce::LookAndFeel_V4::ColourScheme getJetBlackColourScheme()
{
    return { 0xff0b0b0b, 0xff151515, 0xff111111,
             0xff666666, 0xffffffff, 0xffd8d8d8,
             0xffffffff, 0xff606060, 0xffffffff };
}

juce::LookAndFeel_V4::ColourScheme getYamDxColourScheme()
{
    return { 0xff0b0b0b, 0xff0f0f0f, 0xff111111,
             0xff666666, 0xfffffbed, 0xff29cfc1,
             0xffffffff, 0xff8584bc, 0xfffffdf2 };
}

juce::LookAndFeel_V4::ColourScheme getAkApcColourScheme()
{
    return { 0xff101112, 0xff222326, 0xff101112,
             0xff666666, 0xffffffff, 0xffb3b3b3,
             0xffffffff, 0xffd01634, 0xffffffff };
}

juce::LookAndFeel_V4::ColourScheme getAkMpcColourScheme()
{
    return { 0xffefefef, 0xffffffff, 0xffffffff,
             0xffdddddd, 0xff000000, 0xffacacbf,
             0xffffffff, 0xffd01634, 0xff000000 };
}

juce::LookAndFeel_V4::ColourScheme getLexiBlueColourScheme()
{
    return { 0xff0d0f0d, 0xff1a1a1a, 0xff111111,
             0xff666666, 0xffffffff, 0xff515459,
             0xffffffff, 0xff5794c7, 0xffffffff };
}

juce::LookAndFeel_V4::ColourScheme getKurzGreenColourScheme()
{
    return { 0xff16171a, 0xff111214, 0xff111214,
             0xff666666, 0xffffffff, 0xffd9d1ad,
             0xffffffff, 0xff00a66e, 0xffffffff };
}

juce::LookAndFeel_V4::ColourScheme getKorGreyColourScheme()
{
    return { 0xffdddddd, 0xffc1c3c7, 0xffdddddd,
             0xff666666, 0xff0e0e0f, 0xff8c785e,
             0xffe4e4e4, 0xff4a4a4a, 0xff0e0e0f };
}

juce::LookAndFeel_V4::ColourScheme getKorGoldColourScheme()
{
    return { 0xff16171f, 0xff0e0f12, 0xff1b1b21,
             0xffdddddd, 0xffffffff, 0xff736745,
             0xffffffff, 0xffa28f57, 0xffffffff };
}

juce::LookAndFeel_V4::ColourScheme getArturOrangeColourScheme()
{
    return { 0xff161a1f, 0xff0e1012, 0xff0e1012,
             0xff666666, 0xffffffff, 0xff46494d,
             0xffffffff, 0xffe24a21, 0xffffffff };
}

juce::LookAndFeel_V4::ColourScheme getAiraGreenColourScheme()
{
    return { 0xff191919, 0xff111111, 0xff212121,
             0xff666666, 0xffffffff, 0xffffffff,
             0xffffffff, 0xff00955a, 0xffffffff };
}
// ADD YOUR CUSTOM LookAndFeel colourScheme HERE -->


// Your existing colourSchemeFromProperty function
juce::LookAndFeel_V4::ColourScheme colourSchemeFromProperty(const juce::var &property)
{
    if (property == "Light")
        return juce::LookAndFeel_V4::getLightColourScheme(); // These are still JUCE's default schemes
    if (property == "Grey")
        return juce::LookAndFeel_V4::getGreyColourScheme();
    if (property == "Dark")
        return juce::LookAndFeel_V4::getDarkColourScheme();
    if (property == "Midnight")
        return juce::LookAndFeel_V4::getMidnightColourScheme();
    if (property == "JetBlack")
        return getJetBlackColourScheme(); // Now calling your local function
    if (property == "YamDX")
        return getYamDxColourScheme();
    // You had "YamDX" twice, corrected to just one.
    if (property == "AkAPC")
        return getAkApcColourScheme();
    if (property == "AkMPC")
        return getAkMpcColourScheme();
    if (property == "LexiBlue")
        return getLexiBlueColourScheme();
    if (property == "KurzGreen")
        return getKurzGreenColourScheme();
    if (property == "KorGrey")
        return getKorGreyColourScheme();
    if (property == "KorGold")
        return getKorGoldColourScheme();
    if (property == "ArturOrange")
        return getArturOrangeColourScheme();
    if (property == "AiraGreen")
        return getAiraGreenColourScheme();
    // ADD YOUR CUSTOM LookAndFeel colourScheme HERE -->
     
    return juce::LookAndFeel_V4::getLightColourScheme();
}

juce::LookAndFeel* createLookAndFeelFromDescription(const juce::String& description, bool returnDefaultV4ForUnknown)
    {
        // This handles all the V4 schemes
        if (description == "V4" || description == "V4 Light")
            return new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getLightColourScheme());
        else if (description == "V4 Grey")
            return new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getGreyColourScheme());
        else if (description == "V4 Dark")
            return new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getDarkColourScheme());
        else if (description == "V4 Midnight")
            return new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getMidnightColourScheme());
        
         // Use your custom gui namespace function
        else if (description == "V4 JetBlack")
            return new juce::LookAndFeel_V4(gui::getJetBlackColourScheme());
        else if (description == "V4 YamDX")
            return new juce::LookAndFeel_V4(gui::getYamDxColourScheme());
        else if (description == "V4 AkAPC")
            return new juce::LookAndFeel_V4(gui::getAkApcColourScheme());
        else if (description == "V4 AkMPC")
            return new juce::LookAndFeel_V4(gui::getAkMpcColourScheme());
        else if (description == "V4 LexiBlue")
            return new juce::LookAndFeel_V4(gui::getLexiBlueColourScheme());
        else if (description == "V4 KurzGreen")
            return new juce::LookAndFeel_V4(gui::getKurzGreenColourScheme());
        else if (description == "V4 KorGrey")
            return new juce::LookAndFeel_V4(gui::getKorGreyColourScheme());
        else if (description == "V4 KorGold")
            return new juce::LookAndFeel_V4(gui::getKorGoldColourScheme());
        else if (description == "V4 ArturOrange")
            return new juce::LookAndFeel_V4(gui::getArturOrangeColourScheme());
        else if (description == "V4 AiraGreen")
            return new juce::LookAndFeel_V4(gui::getAiraGreenColourScheme());
        // ADD YOUR CUSTOM LookAndFeel colourScheme HERE -->
        
        // Handle older JUCE LookAndFeels
        else if (description == "V3")
            return new juce::LookAndFeel_V3();
        else if (description == "V2")
            return new juce::LookAndFeel_V2();
        else if (description == "V1")
            return new juce::LookAndFeel_V1();
        else
        {
            // If the description doesn't match any known scheme
            if (returnDefaultV4ForUnknown)
                return new juce::LookAndFeel_V4(juce::LookAndFeel_V4::getLightColourScheme()); // Default fallback
            else
                return nullptr; // Or return nullptr if "unknown" means "no specific L&F"
        }
    }

} // namespace gui
