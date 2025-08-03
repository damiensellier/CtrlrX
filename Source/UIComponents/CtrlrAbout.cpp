#include "stdafx.h"
#include "stdafx_luabind.h"
#include "CtrlrAbout.h"
#include "CtrlrRevision.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "CtrlrInlineUtilitiesGUI.h"

CtrlrAbout::CtrlrAbout (CtrlrManager &_owner)
    : owner(_owner)
{
    // CTRLRX LOGO SVG
    addAndMakeVisible (ctrlrLogo = gui::createDrawableButton("CtrlrX", BIN2STR(ctrlrx_logo_svg))); // Updated v5.6.31. It required to drag drop SVG file in the projucer in the icon folder to be embedded
    ctrlrLogo->addListener (this);
    ctrlrLogo->setTooltip (TRANS("Visit ctrlr.org"));
    ctrlrLogo->setMouseCursor(MouseCursor::PointingHandCursor);
    
    // "Ctrlr" logo as letters
    addAndMakeVisible (ctrlrName = new Label ("", TRANS("CtrlrX")));
    
    ctrlrName->setFont (Font (32, Font::bold)); // was 64 then 52
    ctrlrName->setJustificationType (Justification::topLeft); // was centredLeft
    ctrlrName->setEditable (false, false, false);
    ctrlrName->setMinimumHorizontalScale(1.0f);
    ctrlrName->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId))); // Updated v5.6.31 Colour(0xd6000000));
    ctrlrName->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    
    // CtrlrX version Label
    addAndMakeVisible (ctrlrxVersionLabel = new Label ("Version",  "Build version " + _STR(ctrlrRevision)));
    ctrlrxVersionLabel->setFont (Font (14.00f, Font::plain));
    ctrlrxVersionLabel->setJustificationType (Justification::topLeft);
    ctrlrxVersionLabel->setEditable (false, false, false);
    ctrlrxVersionLabel->setMinimumHorizontalScale(1.0f);
    ctrlrxVersionLabel->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.8f)));
    ctrlrxVersionLabel->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId)));
    
    // CtrlrX release date Label
    addAndMakeVisible (ctrlrxReleaseDateLabel = new Label ("Release date", " Released on " + _STR(ctrlrRevisionDate)));
    ctrlrxReleaseDateLabel->setFont (Font (14.00f, Font::plain));
    ctrlrxReleaseDateLabel->setJustificationType (Justification::topLeft);
    ctrlrxReleaseDateLabel->setEditable (false, false, false);
    ctrlrxReleaseDateLabel->setMinimumHorizontalScale(1.0f);
    ctrlrxReleaseDateLabel->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.8f)));
    ctrlrxReleaseDateLabel->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId)));
    
    // CtrlrX libs version Label
    String juceVersion = SystemStats::getJUCEVersion().fromLastOccurrenceOf("JUCE v", false, true);
    String luaVersion = LUA_RELEASE;
    String luabindVersion = _STR(LUABIND_VERSION / 1000) + "." + _STR(LUABIND_VERSION / 100 % 100) + "." + _STR(LUABIND_VERSION % 100);
    String boostVersion = _STR(BOOST_VERSION / 100000) + "." + _STR((BOOST_VERSION / 100) % 1000) + "." + _STR(BOOST_VERSION % 100);
    String buildDetails = "JUCE " + juceVersion + " | " + luaVersion + " | LuaBind " + luabindVersion + " | Boost " + boostVersion;
    addAndMakeVisible (ctrlrxLibsVersionLabel = new Label ("Version",  buildDetails));
    ctrlrxLibsVersionLabel->setFont (Font (12.00f, Font::plain));
    ctrlrxLibsVersionLabel->setJustificationType (Justification::topLeft);
    ctrlrxLibsVersionLabel->setEditable (false, false, false);
    ctrlrxLibsVersionLabel->setMinimumHorizontalScale(1.0f);
    ctrlrxLibsVersionLabel->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.8f)));
    ctrlrxLibsVersionLabel->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId)));
    
    // Credits Label
    addAndMakeVisible (creditsLabel = new TextEditor (""));
    creditsLabel->setFont (Font (13.00f, Font::plain));
    creditsLabel->setMultiLine (true, true);
    creditsLabel->setReturnKeyStartsNewLine (true);
    creditsLabel->setReadOnly (true);
    creditsLabel->setScrollbarsShown (false);
    creditsLabel->setCaretVisible (false);
    creditsLabel->setPopupMenuEnabled (false);
    creditsLabel->setIndents(0, 0);
    creditsLabel->setLineSpacing(1.1f);
    creditsLabel->setReturnKeyStartsNewLine(true);
    creditsLabel->setColour (TextEditor::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f)));
    creditsLabel->setColour (TextEditor::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId)));
    creditsLabel->setColour (TextEditor::outlineColourId, Colour(getLookAndFeel().findColour (TextEditor::outlineColourId).withAlpha(0.0f)));
    creditsLabel->setColour (TextEditor::shadowColourId, Colour(getLookAndFeel().findColour (TextEditor::shadowColourId).withAlpha(0.0f)));
    creditsLabel->setText ("CtrlrX by Damien Sellier is an updated version of \n"
                           "Ctrlr by Roman Kubiak under BSD|GPL license.");
    
    
    
    // Github LOGO SVG
    addAndMakeVisible (githubLogo = gui::createDrawableButton("Github Logo", BIN2STR(github_colour_svg))); // Updated v5.6.31. It required to drag drop SVG file in the projucer in the icon folder to be embedded
    githubLogo->addListener (this);
    githubLogo->setTooltip (TRANS("Visit CtrlrX github page"));
    githubLogo->setMouseCursor(MouseCursor::PointingHandCursor);
    
    // Github link
    addAndMakeVisible (ctrlrxUrl = new HyperlinkButton ("Visit CtrlrX github page", URL ("https://github.com/damiensellier/CtrlrX")));
    ctrlrxUrl->setTooltip (TRANS("Visit CtrlrX github page"));
    ctrlrxUrl->setFont(14.00f, Font::plain);
    ctrlrxUrl->setJustificationType(Justification::topLeft);
    ctrlrxUrl->setColour (HyperlinkButton::textColourId, Colour(getLookAndFeel().findColour (PopupMenu::highlightedBackgroundColourId)));
    
    // PayPal LOGO SVG
    addAndMakeVisible (paypalLogo = gui::createDrawableButton("PayPal Logo", BIN2STR(paypal_colour_svg))); // Updated v5.6.31. It required to drag drop SVG file in the projucer in the icon folder to be embedded
    paypalLogo->addListener (this);
    paypalLogo->setTooltip (TRANS("Donate to the CtrlrX project"));
    paypalLogo->setMouseCursor(MouseCursor::PointingHandCursor);
    
    // PayPal link
    addAndMakeVisible (ctrlrxDonateUrl = new HyperlinkButton ("Donate to the CtrlrX project", URL ("https://paypal.me/damiensellier"))); // Updated v5.6.31b
    ctrlrxDonateUrl->setTooltip (TRANS("Donate to the CtrlrX project"));
    ctrlrxDonateUrl->setFont(14.00f, Font::plain);
    ctrlrxDonateUrl->setJustificationType(Justification::topLeft);
    ctrlrxDonateUrl->setColour (HyperlinkButton::textColourId, Colour(getLookAndFeel().findColour (PopupMenu::highlightedBackgroundColourId)));
    
    // description Label
    addAndMakeVisible (descriptionLabel = new TextEditor (""));
    descriptionLabel->setFont (Font (13.00f, Font::plain));
    descriptionLabel->setMultiLine (true, true);
    descriptionLabel->setReturnKeyStartsNewLine (true);
    descriptionLabel->setReadOnly (true);
    descriptionLabel->setScrollbarsShown (false);
    descriptionLabel->setCaretVisible (false);
    descriptionLabel->setPopupMenuEnabled (false);
    descriptionLabel->setIndents(0, 0);
    descriptionLabel->setLineSpacing(1.1f);
    descriptionLabel->setReturnKeyStartsNewLine(true);
    descriptionLabel->setColour (TextEditor::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f)));
    descriptionLabel->setColour (TextEditor::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId)));
    descriptionLabel->setColour (TextEditor::outlineColourId, Colour(getLookAndFeel().findColour (TextEditor::outlineColourId).withAlpha(0.0f)));
    descriptionLabel->setColour (TextEditor::shadowColourId, Colour(getLookAndFeel().findColour (TextEditor::shadowColourId).withAlpha(0.0f)));
    descriptionLabel->setText ("With CtrlrX, control any MIDI enabled hardware: synthesizers, drum machines, effects, samplers.\n"
                               "Create custom user interfaces for your favorite MIDI devices.\n"
                               "Host your device as VST or AU plugins in your favorite DAW or as standalone software."); // Updated v5.6.32. Typo. @dnaldoog
    
    

    // JUCE & FRIENDS LOGO SVG
    addAndMakeVisible (vst3AuJuceLogo = gui::createDrawableButton("Copyright Logo", BIN2STR(vst3_au_juce_mini_logo_bg_rnd_svg))); // Updated v5.6.31. It required to drag drop SVG file in the projucer in the icon folder to be embedded
    vst3AuJuceLogo->addListener (this);
    vst3AuJuceLogo->setTooltip (TRANS("Visit ctrlr.org"));
    vst3AuJuceLogo->setMouseCursor(MouseCursor::PointingHandCursor);
    
    // JUCE & FRIENDS Label
    addAndMakeVisible (copyrightLabel = new TextEditor (""));
    copyrightLabel->setFont (Font (13.00f, Font::plain));
    copyrightLabel->setMultiLine (true, true);
    copyrightLabel->setReturnKeyStartsNewLine (true);
    copyrightLabel->setReadOnly (true);
    copyrightLabel->setScrollbarsShown (false);
    copyrightLabel->setCaretVisible (false);
    copyrightLabel->setPopupMenuEnabled (false);
    copyrightLabel->setIndents(0, 0);
    copyrightLabel->setLineSpacing(1.1f);
    copyrightLabel->setReturnKeyStartsNewLine(true);
    copyrightLabel->setColour (TextEditor::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f)));
    copyrightLabel->setColour (TextEditor::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId)));
    copyrightLabel->setColour (TextEditor::outlineColourId, Colour(getLookAndFeel().findColour (TextEditor::outlineColourId).withAlpha(0.0f)));
    copyrightLabel->setColour (TextEditor::shadowColourId, Colour(getLookAndFeel().findColour (TextEditor::shadowColourId).withAlpha(0.0f)));
    copyrightLabel->setText ("CtrlrX is based on JUCE audio framework by PACE Anti-Piracy Inc.\n"
                             "Audio Unit is a trademark of Apple Computer Inc.\n"
                             "VST is a trademark of Steinberg Media Technologies GmbH."); // Updated v5.6.32. Typo. @dnaldoog
    
    
    // Line Separator
        
    // Lib Version Info Label
    addAndMakeVisible (versionInfoLabel = new TextEditor (""));
    versionInfoLabel->setFont (Font (owner.getFontManager().getDefaultMonoFontName(), 14.0f, Font::plain));
    versionInfoLabel->setFont (Font (14.00f, Font::plain));
    versionInfoLabel->setMultiLine (true, true);
    versionInfoLabel->setReturnKeyStartsNewLine (true);
    versionInfoLabel->setReadOnly (true);
    versionInfoLabel->setScrollbarsShown (false);
    versionInfoLabel->setCaretVisible (false);
    versionInfoLabel->setPopupMenuEnabled (false);
    versionInfoLabel->setIndents(0, 0);
    versionInfoLabel->setReturnKeyStartsNewLine(true);
    versionInfoLabel->setColour (TextEditor::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f)));
    versionInfoLabel->setColour (TextEditor::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId)));
    versionInfoLabel->setColour (TextEditor::outlineColourId, Colour(getLookAndFeel().findColour (TextEditor::outlineColourId).withAlpha(0.0f)));
    versionInfoLabel->setColour (TextEditor::shadowColourId, Colour(getLookAndFeel().findColour (TextEditor::shadowColourId).withAlpha(0.0f)));
    versionInfoLabel->setText ("");
    
    addVersionInfo ("Version", STR(ctrlrRevision));
    addVersionInfo ("Build date", STR(ctrlrRevisionDate));
    
#if CTRLR_NIGHTLY == 1
    addVersionInfo ("Branch", "Nightly");
#else
    addVersionInfo ("Branch", "master");
#endif
    
    addVersionInfo ("Juce", SystemStats::getJUCEVersion().fromLastOccurrenceOf("JUCE v", false, true));
    addVersionInfo ("libusb", "1.0.19");
    addVersionInfo ("liblo", "0.28");
    addVersionInfo ("lua", LUA_RELEASE);
    addVersionInfo ("luabind", _STR(LUABIND_VERSION / 1000) + "." + _STR(LUABIND_VERSION / 100 % 100) + "." + _STR(LUABIND_VERSION % 100));
    addVersionInfo ("boost", _STR(BOOST_VERSION / 100000) + "." + _STR(BOOST_VERSION / 100 % 1000) + "." + _STR(BOOST_VERSION % 100));
    
    updateVersionLabel(); // Update lib version field
    
    
    
    
    // PANEL EXTRA LABELS
    // Instance Name Label
    addAndMakeVisible (label = new Label ("new label", TRANS("Instance name")));
    label->setFont (Font (14.00f, Font::plain));
    label->setJustificationType (Justification::topRight);
    label->setEditable (false, false, false);
    label->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f))); // Updated v5.6.31 Colours::black);
    label->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    
    // Instance version Label
    addAndMakeVisible (label3 = new Label ("new label", TRANS("Version")));
    label3->setFont (Font (14.00f, Font::plain));
    label3->setJustificationType (Justification::topRight);
    label3->setEditable (false, false, false);
    label3->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f))); // Updated v5.6.31 Colours::black);
    label3->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    

    // Author Name Label
    addAndMakeVisible (label2 = new Label ("new label", TRANS("Author")));
    label2->setFont (Font (14.00f, Font::plain));
    label2->setJustificationType (Justification::topRight);
    label2->setEditable (false, false, false);
    label2->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f))); // Updated v5.6.31 Colours::black);
    label2->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    
    // Author Email Label
    String authorEmail;
    if(owner.getActivePanel()) authorEmail = owner.getActivePanel()->getProperty(Ids::panelAuthorEmail);
        else authorEmail = "";
    
    if (!authorEmail.isEmpty())
    {
        addAndMakeVisible(labelAuthorEmail = new Label("new label", TRANS("Contact")));
        labelAuthorEmail->setFont(Font(14.00f, Font::plain));
        labelAuthorEmail->setJustificationType(Justification::topRight);
        labelAuthorEmail->setEditable(false, false, false);
        labelAuthorEmail->setColour(Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f))); // Updated v5.6.31 Colours::black);
        labelAuthorEmail->setColour(Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour(0x00000000));
    }
    
    // Author URL Label
    addAndMakeVisible (label4 = new Label ("new label", TRANS("URL")));
    label4->setFont (Font (14.00f, Font::plain));
    label4->setJustificationType (Justification::topRight);
    label4->setEditable (false, false, false);
    label4->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.6f))); // Updated v5.6.31 Colours::black);
    label4->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    
    // Author Donate Label
    String authorDonateUrl;
    if(owner.getActivePanel()) authorDonateUrl = owner.getActivePanel()->getProperty(Ids::panelAuthorDonateUrl);
        else authorDonateUrl = "";
    
	if (!authorDonateUrl.isEmpty())
	{
		addAndMakeVisible(labelDonate = new Label("new label", TRANS("Donate")));
		labelDonate->setFont(Font(14.00f, Font::plain));
		labelDonate->setJustificationType(Justification::topRight);
		labelDonate->setEditable(false, false, false);
		labelDonate->setColour(Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.7f))); // Updated v5.6.31 Colours::black);
		labelDonate->setColour(Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour(0x00000000));
	}


    addAndMakeVisible (instanceUrl = new HyperlinkButton ("", URL ("https://juce.com/")));
    instanceUrl->setTooltip (TRANS("https://juce.com/"));
    instanceUrl->setColour (HyperlinkButton::textColourId, Colour(getLookAndFeel().findColour (PopupMenu::highlightedBackgroundColourId)));
    instanceUrl->setFont(14.00f, Font::plain);
    instanceUrl->setJustificationType(Justification::topLeft);
    
    // Instance Name Field
    addAndMakeVisible (instanceName = new Label ("", ""));
    instanceName->setFont (Font (14.00f, Font::bold));
    instanceName->setJustificationType (Justification::topLeft);
    instanceName->setEditable (false, false, false);
    instanceName->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.7f))); // Updated v5.6.31 Colours::black);
    instanceName->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    
    // Instance Version Field
    addAndMakeVisible (instanceVersion = new Label ("", ""));
    instanceVersion->setFont (Font (14.00f, Font::plain));
    instanceVersion->setJustificationType (Justification::topLeft);
    instanceVersion->setEditable (false, false, false);
    instanceVersion->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.7f))); // Updated v5.6.31 Colours::black);
    instanceVersion->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    
    // Instance Author Field
    addAndMakeVisible (instanceAuthor = new Label ("", ""));
    instanceAuthor->setFont (Font (14.00f, Font::plain));
    instanceAuthor->setJustificationType (Justification::topLeft);
    instanceAuthor->setEditable (false, false, false);
    instanceAuthor->setColour (Label::textColourId, Colour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.7f))); // Updated v5.6.31 Colours::black);
    instanceAuthor->setColour (Label::backgroundColourId, Colour(getLookAndFeel().findColour (Label::backgroundColourId))); // Updated v5.6.31 Colour (0x00000000));
    
    // Instance Author Email Field
	if (!authorEmail.isEmpty())
	{
		addAndMakeVisible(instanceAuthorEmail = new HyperlinkButton(authorEmail,URL("mailto:" + authorEmail)));
        instanceAuthorEmail->setColour (HyperlinkButton::textColourId, Colour(getLookAndFeel().findColour (PopupMenu::highlightedBackgroundColourId)));
        instanceAuthorEmail->setFont(14.00f, Font::plain);
        instanceAuthorEmail->setJustificationType(Justification::topLeft);
	}
    
    // Instance Author Donate URL
	if (!authorDonateUrl.isEmpty())
	{
		addAndMakeVisible(instanceAuthorDonateUrl = new HyperlinkButton(authorDonateUrl, URL(authorDonateUrl)));
        instanceAuthorDonateUrl->setFont(14.00f, Font::plain);
        instanceAuthorDonateUrl->setJustificationType(Justification::topLeft);
        instanceAuthorDonateUrl->setColour (HyperlinkButton::textColourId, Colour(getLookAndFeel().findColour (PopupMenu::highlightedBackgroundColourId)));
	}

    // Instance Description Field
    addAndMakeVisible (instanceDescription = new TextEditor (""));
    instanceDescription->setMultiLine (true);
    instanceDescription->setReturnKeyStartsNewLine (true);
    instanceDescription->setReadOnly (true);
    instanceDescription->setScrollbarsShown (true);
    instanceDescription->setCaretVisible (false);
    instanceDescription->setPopupMenuEnabled (false);
    instanceDescription->setColour (TextEditor::backgroundColourId, Colour(getLookAndFeel().findColour (TextEditor::backgroundColourId).withAlpha(0.2f))); // Updated v5.6.31 Colour (0x00ffffff));
    instanceDescription->setColour (TextEditor::outlineColourId, Colour(getLookAndFeel().findColour (TextEditor::outlineColourId))); // Updated v5.6.31 Colour (0x59000000));
    instanceDescription->setColour (TextEditor::shadowColourId, Colour(getLookAndFeel().findColour (TextEditor::shadowColourId))); // Updated v5.6.31 Colour (0x00000000));
    instanceDescription->setText ("");
    

    // Overall Height declaration
    if (owner.getActivePanel())
    {
        instanceName->setText (owner.getActivePanel()->getProperty(Ids::name).toString(), dontSendNotification);
        instanceVersion->setText (owner.getActivePanel()->getVersionString(false, false, "."), dontSendNotification);
        instanceAuthor->setText (owner.getActivePanel()->getProperty(Ids::panelAuthorName).toString(), dontSendNotification);
        instanceUrl->setButtonText (owner.getActivePanel()->getProperty(Ids::panelAuthorUrl));
        instanceUrl->setURL(URL(owner.getActivePanel()->getProperty(Ids::panelAuthorUrl)));
        instanceDescription->setText (owner.getActivePanel()->getProperty(Ids::panelAuthorDesc).toString(), dontSendNotification);
    }
    
	if (owner.isSingleInstance())
	{
        int singleInstanceHeight = 580;
        
		if (!authorEmail.isEmpty())
		{
			singleInstanceHeight += 32;
		}

		if (!authorDonateUrl.isEmpty())
		{
			singleInstanceHeight += 32;
		}

		setSize (600, singleInstanceHeight); // if exported single instance
	}
	else
	{
        setSize (600, 340); // if not exported instance 340
	}
}

CtrlrAbout::~CtrlrAbout()
{
    
	if (ctrlrLogo)         ctrlrLogo->removeListener(this);
	if (githubLogo)        githubLogo->removeListener(this);
	if (paypalLogo)        paypalLogo->removeListener(this);
	if (vst3AuJuceLogo)    vst3AuJuceLogo->removeListener(this);
    
    ctrlrName = nullptr;
    ctrlrLogo = nullptr;
    vst3AuJuceLogo = nullptr;
    versionInfoLabel = nullptr;
    ctrlrxLibsVersionLabel = nullptr;
    creditsLabel = nullptr;
    ctrlrxVersionLabel = nullptr;
    ctrlrxReleaseDateLabel = nullptr;
    ctrlrxUrl = nullptr;
    ctrlrxDonateUrl = nullptr;
    descriptionLabel = nullptr;
    copyrightLabel = nullptr;
    
    label = nullptr;
    label2 = nullptr;
    label3 = nullptr;
    label4 = nullptr;
    labelDonate = nullptr;
    labelAuthorEmail = nullptr;
    
    instanceUrl = nullptr;
    instanceAuthorDonateUrl = nullptr;
    instanceAuthorEmail = nullptr;
    instanceVersion = nullptr;
    instanceAuthor = nullptr;
    instanceName = nullptr;
    instanceDescription = nullptr;
}

//==============================================================================
void CtrlrAbout::paint (Graphics& g)
{
    // g.setColour (Colour (0xff4c4c4c)); // Removed v5.6.31
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // Added v5.6.31
    
    // Horizontal line separator
    g.setColour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.5f));
    g.drawHorizontalLine(340, 12, (float) getWidth()-12 );
    
    // Vertical line separator
    g.setColour(getLookAndFeel().findColour (Label::textColourId).withAlpha(0.5f));
    g.drawVerticalLine(190, 280, 320); // vert separation @ 170

}

void CtrlrAbout::resized()
{
    int paddingSize = 10;
    int heightPosition = 10; // Initial vertical position
        

    // Left Side
    
    int ctrlrLogoSize = 170; // 150
    ctrlrLogo->setBounds ( paddingSize, paddingSize, ctrlrLogoSize, ctrlrLogoSize );
    
    
    // Right side
    int rightColumnWidth = (600 - ctrlrLogoSize - paddingSize*4);
    
    int ctrlrNameHeight = 32;
    heightPosition += 0; // 8
    ctrlrName->setBounds (ctrlrLogoSize + paddingSize*3, heightPosition, rightColumnWidth, ctrlrNameHeight);
    
    int ctrlrxVersionLabelheight = 18;
    heightPosition += ( ctrlrNameHeight + paddingSize );
    ctrlrxVersionLabel->setBounds (ctrlrLogoSize + paddingSize*3, heightPosition, rightColumnWidth, ctrlrxVersionLabelheight);
    
    int ctrlrxReleaseDateLabelheight = 18;
    heightPosition += ( ctrlrxVersionLabelheight );
    ctrlrxReleaseDateLabel->setBounds (ctrlrLogoSize + paddingSize*3, heightPosition, rightColumnWidth, ctrlrxReleaseDateLabelheight);
    
    int ctrlrxLibsVersionLabelheight = 18;
    heightPosition += ( ctrlrxReleaseDateLabelheight );
    ctrlrxLibsVersionLabel->setBounds (ctrlrLogoSize + paddingSize*3, heightPosition, rightColumnWidth, ctrlrxLibsVersionLabelheight);
    
    int creditsLabelheight = 32;
    heightPosition += ( ctrlrxLibsVersionLabelheight + paddingSize );
    creditsLabel->setBounds (ctrlrLogoSize + paddingSize*3 + 4, heightPosition, rightColumnWidth, creditsLabelheight);
        
    int ctrlrxUrlHeight = 18;
    heightPosition += ( creditsLabelheight + paddingSize );
    githubLogo->setBounds (ctrlrLogoSize + paddingSize*3, heightPosition -1, ctrlrxUrlHeight +2, ctrlrxUrlHeight +2);
    ctrlrxUrl->setBounds (ctrlrLogoSize + paddingSize*5 +4, heightPosition, rightColumnWidth, ctrlrxUrlHeight);
    
    int ctrlrxDonateUrlHeight = 18;
    heightPosition += ( ctrlrxUrlHeight );
    paypalLogo->setBounds (ctrlrLogoSize + paddingSize*3, heightPosition -1, ctrlrxUrlHeight +2, ctrlrxDonateUrlHeight +2);
    ctrlrxDonateUrl->setBounds (ctrlrLogoSize + paddingSize*5 +4, heightPosition, rightColumnWidth, ctrlrxDonateUrlHeight);
    
    
    // Centered
    int descriptionLabelheight = 48;
    heightPosition = ( ctrlrLogoSize + paddingSize*4 );
    descriptionLabel->setBounds (paddingSize, heightPosition, getWidth() - paddingSize*2, descriptionLabelheight);
    
    
    int copyrightLabelheight = 48;
    heightPosition += ( descriptionLabelheight + paddingSize*2 );
    vst3AuJuceLogo->setBounds (paddingSize, heightPosition, ctrlrLogoSize, copyrightLabelheight);
    copyrightLabel->setBounds (ctrlrLogoSize + paddingSize*3 +4, heightPosition, rightColumnWidth, copyrightLabelheight);
    
    
    // int versionInfoLabelHeight = 56;
    heightPosition += ( descriptionLabelheight + paddingSize*2 );
    // versionInfoLabel->setBounds (paddingSize, heightPosition, getWidth() - paddingSize*2, versionInfoLabelHeight); // Frame with codeEditor for CtrlrX libraries' versions
    
    
    // Panel Infos
    int height = 360;
    int labelHeight = 18;
    label->setBounds (paddingSize, height, ctrlrLogoSize, labelHeight); // Instance Name .y was 120
    instanceName->setBounds(ctrlrLogoSize + paddingSize*3, height, rightColumnWidth, labelHeight); // Instance Field
           
    height += ( labelHeight + paddingSize );
    label3->setBounds (paddingSize, height, ctrlrLogoSize, labelHeight); // Instance Version Label
    instanceVersion->setBounds(ctrlrLogoSize + paddingSize*3, height, rightColumnWidth, labelHeight); // Instance Version Field
    
    height += ( labelHeight + paddingSize );
    label4->setBounds (paddingSize, height, ctrlrLogoSize, labelHeight); // Instance URL Label
    instanceUrl->setBounds (ctrlrLogoSize + paddingSize*3, height, rightColumnWidth, labelHeight); // Instance URL Field
    
    height += ( labelHeight + paddingSize );
    label2->setBounds (paddingSize, height, ctrlrLogoSize, labelHeight); // Instance Author Label
    instanceAuthor->setBounds(ctrlrLogoSize + paddingSize*3, height, rightColumnWidth, labelHeight); // Instance Author Field
    	
    if (labelAuthorEmail)
	{
		height += ( labelHeight + paddingSize );
		labelAuthorEmail->setBounds(paddingSize, height, ctrlrLogoSize, labelHeight); // Instance Author Email Label
		instanceAuthorEmail->setBounds(ctrlrLogoSize + paddingSize*3, height, rightColumnWidth, labelHeight); // Instance Author Email Field
	}
    
    if (labelDonate)
    {
        height += ( labelHeight + paddingSize );
        labelDonate->setBounds(paddingSize, height, ctrlrLogoSize, labelHeight); // Instance Author Donate Label
        instanceAuthorDonateUrl->setBounds(ctrlrLogoSize + paddingSize*3, height, rightColumnWidth, labelHeight); // Instance Author Donate Field
    }
	
    height += 48;
    instanceDescription->setBounds (proportionOfWidth (0.0200f), height, proportionOfWidth (0.9600f), 80); // Instance Description Frame
}

void CtrlrAbout::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == ctrlrLogo || vst3AuJuceLogo )
    {
        //[UserButtonCode_ctrlrLogo] -- add your button handler code here..
        URL url("https://github.com/RomanKubiak/ctrlr/discussions");
        url.launchInDefaultBrowser();
        //[/UserButtonCode_ctrlrLogo]
    }
}


void CtrlrAbout::addVersionInfo (const String &name, const String &version)
{
	versionInformationArray.set (name, version);
}

void CtrlrAbout::updateVersionLabel()
{
	for (int i = 0; i < versionInformationArray.size(); i++)
	{
		versionInfoLabel->insertTextAtCaret(versionInformationArray.getAllKeys()[i] + ": ");
		versionInfoLabel->insertTextAtCaret(versionInformationArray.getAllValues()[i] + "\n");
	}
	versionInfoLabel->setText (versionInformationArray.getDescription());
}
