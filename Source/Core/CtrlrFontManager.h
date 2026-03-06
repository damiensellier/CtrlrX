#ifndef CTRLR_FONT_MANAGER
#define CTRLR_FONT_MANAGER

#include "CtrlrMacros.h"

class CtrlrPanel;
class CtrlrManager;

class CtrlrFontManager
{
	public:
		enum FontSet
		{
			osFontSet,
			importedFontSet,
			builtInFontSet,
			juceFontSet,
			unknownFontSet
		};

		CtrlrFontManager(CtrlrManager &_owner);
		~CtrlrFontManager();
		void reloadOSFonts();
		void reloadBuiltInFonts();
		void reloadImportedFonts(CtrlrPanel *panelToLoadFrom=nullptr);
		void reloadJuceFonts();
		void reloadFonts();
		const Array<Font> &getOsFontArray();
		void fillCombo (ComboBox &comboToFill, const bool showOsFonts=true, const bool showBuiltInFonts=true, const bool showImportedFonts=true, const bool showJuceFonts=true);
		Font getFont(const int fontIndex);
		Font getFont(const File &fontFile);
		const String getDefaultMonoFontName();
		const Font getFontFromString (const String &string);
		const String getStringFromFont (const Font &font);
		int getNumBuiltInFonts();
		FontSet getFontSetEnum (const Font &font);
		Array<Font> &getFontSet (const FontSet fontSetToFetch);

		Font getDefaultLargeFont();
		Font getDefaultSmallFont();
		Font getDefaultNormalFont();

		// --- JUCE 8 UPDATES START HERE ---
		// Changed int to size_t to match JUCE 8 BinaryData and Typeface API
		static Font getBuiltInFont(const char *fontData, const size_t fontDataSize);
		static Font getBuiltInFont(const String &fontResourceName);
		static const Font getFont (const char *fontData, const size_t fontDataSize);
		// --- JUCE 8 UPDATES END HERE ---
	
		static const Font getBuiltInFont(const int fontIndex);
		static bool isFontFile(const File &fontFile) { return (fontFile.hasFileExtension("jfont") || fontFile.hasFileExtension("ttf") || fontFile.hasFileExtension("otf")); }

		JUCE_LEAK_DETECTOR(CtrlrFontManager);

	private:
	    CtrlrManager &owner;
		Array<Font> osFonts;
		Array<Font> builtInFonts;
		Array<Font> juceFonts;
		Array<Font> importedFonts;
		StringArray osTypefaceNames;
		StringArray osTypefaceStyles;
        Font defaultFont;
		int64 allFontCount;
};

#endif
