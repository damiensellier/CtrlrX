/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== gen_LookAndFeelBase.h.sh ==================
static const unsigned char temp_binary_data_124[] =
"#!/bin/bash\r\n"
"HEADER=\"$1\"\r\n"
"\r\n"
"if [ ! -f \"$HEADER\" ]; then\r\n"
"\techo \"Need the header as parameter\"\r\n"
"\texit 1\r\n"
"fi\r\n"
"\r\n"
"cat << _EOF_\r\n"
"#ifndef LOOK_AND_FEEL_BASE\r\n"
"#define LOOK_AND_FEEL_BASE\r\n"
"\r\n"
"#include \"CtrlrLuaManager.h\"\r\n"
"#include \"LookAndFeelParamWrapper.h\"\r\n"
"\r\n"
"class LLookAndFeel;\r\n"
"\r\n"
"class LookAndFeelBase : public LookAndFeel_V3\r\n"
"{\r\n"
"\tpublic:\r\n"
"\t\tLookAndFeelBase(LLookAndFeel &_owner) : owner(_owner) {}\r\n"
"\t\tLookAndFeel_V3 v3;\r\n"
"_EOF_\r\n"
"\r\n"
"cat $HEADER | grep \"def_\" | awk '{\r\n"
"\tconstNow \t= 0;\r\n"
"\thack \t\t= 0;\r\n"
"\tnumParams\t= 0;\r\n"
"\tnumRefParams\t= 0;\r\n"
"\tretTypeOffset  \t= 2;\r\n"
"\tfuncNameOffset \t= 3;\r\n"
"\tparamsOffset   \t= 5;\r\n"
"\tneedsToReturn  \t= 0;\r\n"
"\tnumRawParams\t= 0;\r\n"
"\tmethodName\t= \"\";\r\n"
"\treturnType\t= \"\";\r\n"
"\treturnTypeStr\t= \"\";\r\n"
"\t\r\n"
"\tfor (i=1; i<=NF; i++)\r\n"
"\t{\r\n"
"\t\tif (i == retTypeOffset)\r\n"
"\t\t{\r\n"
"\t\t\t# Return type\r\n"
"\t\t\t# printf (\"[rettype] offset %d value %s\\n\", i, $i);\r\n"
"\t\t\tif ($i == \"const\")\r\n"
"\t\t\t{\r\n"
"\t\t\t\tfuncNameOffset++;\r\n"
"\t\t\t\tparamsOffset++;\r\n"
"\t\t\t\tneedsToReturn = 1;\r\n"
"\t\t\t\treturnType = sprintf (\"const %s\", $(i+1));\r\n"
"\t\t\t}\r\n"
"\t\t\telse if ($i != \"void\")\r\n"
"\t\t\t{\r\n"
"\t\t\t\tneedsToReturn = 1;\r\n"
"\t\t\t\treturnType = sprintf (\"%s\", $i);\r\n"
"\t\t\t}\r\n"
"\t\t\telse\r\n"
"\t\t\t{\r\n"
"\t\t\t\tneedsToReturn = 0;\r\n"
"\t\t\t\treturnType = sprintf (\"void\");\r\n"
"\t\t\t}\r\n"
"\t\t\t\r\n"
"\t\t\t# printf (\"returnType=%s funcNameOffset=%d\\n\", returnType, funcNameOffset);\r\n"
"\t\t}\r\n"
"\t\t\r\n"
"\t\tif (i == funcNameOffset)\r\n"
"\t\t{\r\n"
"\t\t\t# printf (\"[method] offset %d value %s\\n\", i, $i);\r\n"
"\t\t\tmethodName = sprintf (\"%s\", substr ($i,5));\r\n"
"\t\t\t# printf (\"methodName=%s\\n\", methodName);\r\n"
"\t\t}\r\n"
"\t\tif (i > paramsOffset)\r\n"
"\t\t{\r\n"
"\t\t\tprintf (\"\\n[params] offset %d value [%s]\\n\", i, $i);\r\n"
"\t\t\t\r\n"
"\t\t\tparam = i;\r\n"
"\t\t\t\r\n"
"\t\t\tif (index($param, \")\"))\r\n"
"\t\t\t\t$param = substr ($param, 0, index($param, \")\") - 1);\r\n"
"\t\t\t\t\r\n"
"\t\t\trawParams[++numRawParams] = sprintf (\"%s\", $param);\r\n"
"\t\t}\r\n"
"\t}\r\n"
"\t\r\n"
"\tprintf (\"\\n\");\r\n"
"\t\r\n"
"\tprintf (\"\\t\\t%s %s (\", returnType, methodName);\r\n"
"\t\r\n"
"\tfor (x=1;x<=numRawParams;x++)\r\n"
"\t{\r\n"
"\t\tif (x == numRawParams || numRawParams == 0)\r\n"
"\t\t\tprintf (\"%s\", rawParams[x])\r\n"
"\t\telse\r\n"
"\t\t\tprintf (\"%s \", rawParams[x]);\r\n"
"\t}\r\n"
"\tprintf (\");\");\r\n"
"}'\r\n"
"\r\n"
"echo -e \"\\n\\n\\tprivate:\"\r\n"
"echo -e \"\\t\\tLLookAndFeel &owner;\"\r\n"
"echo \"};\"\r\n"
"echo\r\n"
"echo \"#endif\"";

const char* gen_LookAndFeelBase_h_sh = (const char*) temp_binary_data_124;

//================== gen_LookAndFeelParamWrapper-Lua-Member-Binding.sh ==================
static const unsigned char temp_binary_data_125[] =
"#!/bin/bash\r\n"
"cat << _EOF_\r\n"
"\tstatic void wrapForLua (lua_State *L)\r\n"
"\t{\r\n"
"\t\tusing namespace luabind;\r\n"
"\t\tmodule(L)\r\n"
"\t\t[\r\n"
"\t\t\tclass_<LookAndFeelParamWrapper>(\"LookAndFeelParamWrapper\")\r\n"
"_EOF_\r\n"
"\r\n"
"cat $1 | awk '{\r\n"
"\tfor (i=1; i<=NF; i++)\r\n"
"\t{\r\n"
"\t\tif (index($i, \";\"))\r\n"
"\t\t{\r\n"
"\t\t\tparam = sprintf (\"%s\", substr ($i, 0, index($i,\";\")-1));\t\t\r\n"
"\t\t\t\r\n"
"\t\t\tif (index ($param, \"*\"))\r\n"
"\t\t\t\tparam = sprintf (\"%s\", substr (param, 2));\r\n"
"\t\t\t\t\r\n"
"\t\t\tprintf (\"\\t\\t\\t\\t.def_readonly (\\\"%s\\\", &LookAndFeelParamWrapper::%s)\\n\", param, param);\r\n"
"\t\t}\r\n"
"\t}\r\n"
"}'\r\n"
"\r\n"
"echo -e \"\\t\\t\\t];\"\r\n"
"echo -e \"\\t\\t}\"";

const char* gen_LookAndFeelParamWrapperLuaMemberBinding_sh = (const char*) temp_binary_data_125;

//================== gen_LookAndFeelParamWrapper.h.sh ==================
static const unsigned char temp_binary_data_126[] =
"#!/bin/bash\r\n"
"HEADER=\"$1\"\r\n"
"\r\n"
"if [ ! -f \"$HEADER\" ]; then\r\n"
"\techo \"Need the header as parameter\"\r\n"
"\texit 1\r\n"
"fi\r\n"
"\r\n"
"cat << _EOF_\r\n"
"#ifndef LOOK_AND_FEEL_PARAM_WRAPPER\r\n"
"#define LOOK_AND_FEEL_PARAM_WRAPPER\r\n"
"\r\n"
"#include \"CtrlrMacros.h\"\r\n"
"\r\n"
"struct LookAndFeelParamWrapper\r\n"
"{\r\n"
"    /* Common */\r\n"
"\tGraphics *g;\r\n"
"\tint x;\r\n"
"\tint y;\r\n"
"\tint w;\r\n"
"\tint h;\r\n"
"\tint width;\r\n"
"\tint height;\r\n"
"\tint colourId;\r\n"
"\tint buttonX;\r\n"
"\tint buttonY;\r\n"
"\tint buttonW;\r\n"
"\tint buttonH;\r\n"
"\tint titleSpaceX;\r\n"
"\tint titleSpaceW;\r\n"
"\tint buttonType;\r\n"
"\tint titleBarX;\r\n"
"\tint titleBarY;\r\n"
"\tint titleBarW;\r\n"
"\tint titleBarH;\r\n"
"\tint tabDepth;\r\n"
"\tint imageX;\r\n"
"\tint imageY;\r\n"
"\tint imageW;\r\n"
"\tint imageH;\r\n"
"\tint columnId;\r\n"
"\tint columnFlags;\r\n"
"\r\n"
"\tfloat sliderPos;\r\n"
"\tfloat minSliderPos;\r\n"
"\tfloat maxSliderPos;\r\n"
"\tfloat imageOpacity;\r\n"
"\tfloat level;\r\n"
"\r\n"
"\tFont *font;\r\n"
"\tComponent *component;\r\n"
"\tButton *button;\r\n"
"\tImageButton *imageButton;\r\n"
"\tTextButton *textButton;\r\n"
"\tToggleButton *toggleButton;\r\n"
"\tColour colour;\r\n"
"\tColour backgroundColour;\r\n"
"\tColour textColour;\r\n"
"\tColour overlayColour;\r\n"
"\tColour *backgroundColourRef;\r\n"
"\tColour *colourRef;\r\n"
"\tColour *textColourPtr;\r\n"
"\tColour *overlayColourRef;\r\n"
"\r\n"
"\tbool isMouseOverButton;\r\n"
"\tbool isButtonDown;\r\n"
"\tbool ticked;\r\n"
"\tbool isEnabled;\r\n"
"\tbool isOpen;\r\n"
"\tbool isMouseOver;\r\n"
"\tbool isActive;\r\n"
"\tbool isHighlighted;\r\n"
"\tbool isTicked;\r\n"
"\tbool hasSubMenu;\r\n"
"\tbool isScrollUpArrow;\r\n"
"\tbool isMouseOverBar;\r\n"
"\tbool isMouseOverItem;\r\n"
"\tbool isMenuOpen;\r\n"
"\tbool isIncrement;\r\n"
"\tbool isMouseDown;\r\n"
"\tbool isMouseDragging;\r\n"
"\tbool drawTitleTextOnLeft;\r\n"
"\tbool positionTitleBarButtonsOnLeft;\r\n"
"\tbool isVerticalBar;\r\n"
"\r\n"
"\tconst String *keyDescription;\r\n"
"\tconst String *name;\r\n"
"\tconst String *text;\r\n"
"\tconst String *title;\r\n"
"\tconst String *message;\r\n"
"\tconst String *button1;\r\n"
"\tconst String *button2;\r\n"
"\tconst String *button3;\r\n"
"\tconst String *instructions;\r\n"
"\tconst String *filename;\r\n"
"\tconst String *shortcutKeyText;\r\n"
"\tconst String *itemText;\r\n"
"\tconst String *tipText;\r\n"
"\tconst String *columnName;\r\n"
"\r\n"
"\tImage *icon;\r\n"
"\tImage *image;\r\n"
"\tDrawable *drawableIcon;\r\n"
"\t/* Common */\r\n"
"\r\n"
"\t/* getIdealPopupMenuItemSize */\r\n"
"\tbool isSeparator;\r\n"
"\tint standardMenuItemHeight;\r\n"
"\tint idealWidth;\r\n"
"\tint idealHeight;\r\n"
"\t/* getIdealPopupMenuItemSize */\r\n"
"\r\n"
"\t/* drawRotarySlider */\r\n"
"\tfloat sliderPosProportional, rotaryStartAngle, rotaryEndAngle;\r\n"
"\tSlider *slider;\r\n"
"\t/* drawRotarySlider */\r\n"
"\r\n"
"\t/* createAlertWindow */\r\n"
"\tAlertWindow::AlertIconType  \ticonType;\r\n"
"\tint numButtons;\r\n"
"\tComponent *associatedComponent;\r\n"
"\t/* createAlertWindow */\r\n"
"\r\n"
"\t/* drawAlertBox */\r\n"
"\tAlertWindow *window;\r\n"
"\tRectangle<int> *textArea;\r\n"
"\tTextLayout *layout;\r\n"
"\t/* drawAlertBox */\r\n"
"\r\n"
"\t/* drawProgressBar */\r\n"
"\tProgressBar *progressBar;\r\n"
"\tdouble progress;\r\n"
"\tString *textToShow;\r\n"
"\t/* drawProgressBar */\r\n"
"\r\n"
"\tScrollBar *scrollBar;\r\n"
"\t\r\n"
"\tRectangle<float> *area;\r\n"
"\tRectangle<int> *areaInt;\r\n"
"\r\n"
"\tTextEditor *textEditor, *editor;\r\n"
"\r\n"
"\tComponent *keyFocusOwner;\r\n"
"\r\n"
"\t/* def_drawFileBrowserRow */\r\n"
"\tDirectoryContentsDisplayComponent *directoryContentsDisplayComponent;\r\n"
"\tString *fileSizeDescription;\r\n"
"\tString *fileTimeDescription;\r\n"
"\tbool isDirectory;\r\n"
"\tbool isItemSelected;\r\n"
"\tint itemIndex;\r\n"
"\t/* def_drawFileBrowserRow */\r\n"
"\tFileBrowserComponent *fileBrowserComponent;\r\n"
"\tFilePreviewComponent *filePreviewComponent;\r\n"
"\tComboBox *currentPathBox;\r\n"
"\tTextEditor *filenameBox;\r\n"
"\tButton *goUpButton;\r\n"
"\tBubbleComponent *bubble, *bubbleComponent;\r\n"
"\tconst Point<float> *tip;\r\n"
"\tconst Rectangle<float> *body;\r\n"
"\tMenuBarComponent *menuBarComponent;\r\n"
"\tComboBox *comboBox;\r\n"
"\tLabel *label;\r\n"
"\tSlider::SliderStyle sliderStyle;\r\n"
"\tFilenameComponent *filenameComponent;\r\n"
"\tConcertinaPanel *concertinaPanel;\r\n"
"\tBorderSize<int> *borderSize;\r\n"
"\tResizableWindow *resizableWindow;\r\n"
"\tDocumentWindow *documentWindow;\r\n"
"\tButton *minimiseButton;\r\n"
"\tButton *maximiseButton;\r\n"
"\tButton *closeButton;\r\n"
"\tJustification *justification;\r\n"
"\tGroupComponent *groupComponent;\r\n"
"\tTabBarButton *tabBarButton;\r\n"
"\tTabbedButtonBar *tabbedButtonBar;\r\n"
"\tPath *p, *path;\r\n"
"\tTableHeaderComponent *tableHeaderComponent;\r\n"
"\tToolbar *toolbar;\r\n"
"\tToolbarItemComponent *toolbarItemComponent;\r\n"
"\tPropertyComponent *propertyComponent;\r\n"
"\tCallOutBox *callOutBox;\r\n"
"\r\n"
"_EOF_\r\n"
"\r\n"
"cat $HEADER | grep \"def_\" | awk '\r\n"
"function rindex(str,c)\r\n"
"{\r\n"
"\treturn (match(str,\"\\\\\" c \"[^\" c \"]*$\") ? RSTART : 0);\r\n"
"}\r\n"
"{\r\n"
"\tconstNow \t= 0;\r\n"
"\thack \t\t= 0;\r\n"
"\tnumParams\t= 0;\r\n"
"\tnumRefParams\t= 0;\r\n"
"\tretTypeOffset  \t= 2;\r\n"
"\tfuncNameOffset \t= 3;\r\n"
"\tparamsOffset   \t= 5;\r\n"
"\tneedsToReturn  \t= 0;\r\n"
"\tnumRawParams\t= 0;\r\n"
"\tmethodName\t= \"\";\r\n"
"\treturnType\t= \"\";\r\n"
"\treturnTypeStr\t= \"\";\r\n"
"\r\n"
"\tfor (i=1; i<=NF; i++)\r\n"
"\t{\r\n"
"\t\tif (i == retTypeOffset)\r\n"
"\t\t{\r\n"
"\t\t\t# Return type\r\n"
"\t\t\t# printf (\"[rettype] offset %d value %s\\n\", i, $i);\r\n"
"\t\t\tif ($i == \"const\")\r\n"
"\t\t\t{\r\n"
"\t\t\t\tfuncNameOffset++;\r\n"
"\t\t\t\tparamsOffset++;\r\n"
"\t\t\t\tneedsToReturn = 1;\r\n"
"\t\t\t\treturnType = sprintf (\"const %s\", $(i+1));\r\n"
"\t\t\t}\r\n"
"\t\t\telse if ($i != \"void\")\r\n"
"\t\t\t{\r\n"
"\t\t\t\tneedsToReturn = 1;\r\n"
"\t\t\t\treturnType = sprintf (\"%s\", $i);\r\n"
"\t\t\t}\r\n"
"\t\t\telse\r\n"
"\t\t\t{\r\n"
"\t\t\t\tneedsToReturn = 0;\r\n"
"\t\t\t\treturnType = sprintf (\"void\");\r\n"
"\t\t\t}\r\n"
"\r\n"
"\t\t\t# printf (\"returnType=%s funcNameOffset=%d\\n\", returnType, funcNameOffset);\r\n"
"\t\t}\r\n"
"\r\n"
"\t\tif (i == funcNameOffset)\r\n"
"\t\t{\r\n"
"\t\t\t# printf (\"[method] offset %d value %s\\n\", i, $i);\r\n"
"\t\t\tmethodName = sprintf (\"%s\", substr ($i,5));\r\n"
"\t\t\t# printf (\"methodName=%s\\n\", methodName);\r\n"
"\t\t}\r\n"
"\t\tif (i > paramsOffset)\r\n"
"\t\t{\r\n"
"\t\t\t#printf (\"[params] offset %d value [%s]\\n\", i, $i);\r\n"
"\t\t\tisReference \t= 0;\r\n"
"\r\n"
"\t\t\tif ($i == \"const\" && constNow == 0)\r\n"
"\t\t\t{\r\n"
"\t\t\t\trawParams[++numRawParams] = sprintf (\"const\");\r\n"
"\t\t\t\tconstNow = 1;\r\n"
"\t\t\t\tcontinue;\r\n"
"\t\t\t}\r\n"
"\r\n"
"\t\t\tif (constNow == 1)\r\n"
"\t\t\t{\r\n"
"\t\t\t\tif ($i == \"const\")\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\trawParams[++numRawParams] = sprintf (\"const\");\r\n"
"\t\t\t\t\thack = 0;\r\n"
"\t\t\t\t\tconstNow = 0;\r\n"
"\t\t\t\t\tcontinue;\r\n"
"\t\t\t\t}\r\n"
"\t\t\t\telse\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\thack = 1;\r\n"
"\t\t\t\t}\r\n"
"\t\t\t}\r\n"
"\r\n"
"\t\t\tparam = i;\r\n"
"\t\t\tif (index($param, \")\"))\r\n"
"\t\t\t\t$param = substr ($param, 0, index($param, \")\") - 1);\r\n"
"\r\n"
"\t\t\trawParams[++numRawParams] = sprintf (\"%s\", $param);\r\n"
"\r\n"
"\t\t\tif (index($param, \",\"))\r\n"
"\t\t\t\t$param = substr ($param, 0, index($param, \",\") - 1);\r\n"
"\r\n"
"\r\n"
"\r\n"
"\t\t\tif (index($param, \"&\"))\r\n"
"\t\t\t{\r\n"
"\t\t\t\tisReference = 1;\r\n"
"\t\t\t\t$param = substr ($param, 2);\r\n"
"\t\t\t}\r\n"
"\r\n"
"\t\t\tif (index($param, \"*\"))\r\n"
"\t\t\t{\r\n"
"\t\t\t\t$param = substr ($param, 2);\r\n"
"\t\t\t}\r\n"
"\r\n"
"\t\t\tif (index($param, \"<\"))\r\n"
"\t\t\t\t$param = substr ($param, 0, index($param, \"<\") - 1);\r\n"
"\r\n"
"\r\n"
"\t\t\tif (hack)\r\n"
"\t\t\t{\r\n"
"\t\t\t\tif (! (i%2))\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\tparamArray[++numParams] \t= param;\r\n"
"\t\t\t\t\tparamRefArray[numParams] \t= isReference;\r\n"
"\t\t\t\t}\r\n"
"\t\t\t}\r\n"
"\t\t\telse\r\n"
"\t\t\t{\r\n"
"\t\t\t\tif (i%2)\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\tparamArray[++numParams] \t= param;\r\n"
"\t\t\t\t\tparamRefArray[numParams]\t= isReference;\r\n"
"\t\t\t\t}\r\n"
"\t\t\t}\r\n"
"\t\t}\r\n"
"\t}\r\n"
"\r\n"
"\tif (numParams > 0)\r\n"
"\t{\r\n"
"\t\tprintf (\"\\n\");\r\n"
"\t\tprintf (\"\\tLookAndFeelParamWrapper (\");\r\n"
"\t\tfor (x=1;x<=numRawParams;x++)\r\n"
"\t\t{\r\n"
"\t\t\tif (x == numRawParams || numRawParams == 0)\r\n"
"\t\t\t{\r\n"
"\t\t\t\tprintf (\"%s_\", rawParams[x])\r\n"
"\t\t\t}\r\n"
"\t\t\telse\r\n"
"\t\t\t{\r\n"
"\t\t\t\tif (rindex(rawParams[x], \",\") > 0)\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\tprintf (\"%s_, \", substr (rawParams[x], 0, index (rawParams[x], \",\") - 1));\r\n"
"\t\t\t\t}\r\n"
"\t\t\t\telse\r\n"
"\t\t\t\t{\r\n"
"\t\t\t\t\tprintf (\"%s \", rawParams[x]);\r\n"
"\t\t\t\t}\r\n"
"\t\t\t}\r\n"
"\t\t}\r\n"
"\r\n"
"\t\tprintf (\") : \");\r\n"
"\t\tfor (x=1;x<=numParams;x++)\r\n"
"\t\t{\r\n"
"\t\t\tif (x==numParams || numParams==0)\r\n"
"\t\t\t{\r\n"
"\t\t\t\tprintf (\"%s(%s%s_)\", $paramArray[x], paramRefArray[x] ? \"&\" : \"\", $paramArray[x]);\r\n"
"\t\t\t}\r\n"
"\t\t\telse\r\n"
"\t\t\t{\r\n"
"\t\t\t\tprintf (\"%s(%s%s_), \", $paramArray[x], paramRefArray[x] ? \"&\" : \"\", $paramArray[x]);\r\n"
"\t\t\t}\r\n"
"\t\t}\r\n"
"\t\tprintf (\" {}\");\r\n"
"\t}\r\n"
"}'\r\n"
"\r\n"
"cat << _EOF_\r\n"
"\t\r\n"
"\tstatic void wrapForLua (lua_State *L)\r\n"
"\t{\r\n"
"\t\tusing namespace luabind;\r\n"
"\t\tmodule(L)\r\n"
"\t\t[\r\n"
"\t            class_<LookAndFeelParamWrapper>(\"LookAndFeelParamWrapper\")\r\n"
"\t            \t.def_readwrite (\"idealWidth\", &LookAndFeelParamWrapper::idealWidth)\r\n"
"\t                .def_readwrite (\"idealHeight\", &LookAndFeelParamWrapper::idealHeight)\r\n"
"        \t        .def_readwrite (\"g\", &LookAndFeelParamWrapper::g)\r\n"
"\t                .def_readonly (\"x\", &LookAndFeelParamWrapper::x)\r\n"
"\t                .def_readonly (\"y\", &LookAndFeelParamWrapper::y)\r\n"
"\t                .def_readonly (\"width\", &LookAndFeelParamWrapper::width)\r\n"
"\t                .def_readonly (\"height\", &LookAndFeelParamWrapper::height)\r\n"
"\t                .def_readonly (\"isSeparator\", &LookAndFeelParamWrapper::isSeparator)\r\n"
"\t                .def_readonly (\"standardMenuItemHeight\", &LookAndFeelParamWrapper::standardMenuItemHeight)\r\n"
"\t                .def_readonly (\"sliderPosProportional\", &LookAndFeelParamWrapper::sliderPosProportional)\r\n"
"\t                .def_readonly (\"rotaryStartAngle\", &LookAndFeelParamWrapper::rotaryStartAngle)\r\n"
"\t                .def_readonly (\"rotaryEndAngle\", &LookAndFeelParamWrapper::rotaryEndAngle)\r\n"
"\t                .def_readwrite (\"slider\", &LookAndFeelParamWrapper::slider)\r\n"
"\t                \r\n"
"\t        ];\r\n"
"\t}\r\n"
"_EOF_\r\n"
"echo \"};\"\r\n"
"echo\r\n"
"echo \"#endif\"\r\n"
"echo";

const char* gen_LookAndFeelParamWrapper_h_sh = (const char*) temp_binary_data_126;

//================== inspect.lua ==================
static const unsigned char temp_binary_data_127[] =
"function getInspect()\r\n"
"local inspect ={\r\n"
"  _VERSION = 'inspect.lua 3.0.0',\r\n"
"  _URL     = 'http://github.com/kikito/inspect.lua',\r\n"
"  _DESCRIPTION = 'human-readable representations of tables',\r\n"
"  _LICENSE = [[\r\n"
"    MIT LICENSE\r\n"
"\r\n"
"    Copyright (c) 2013 Enrique Garc\xc3\xad""a Cota\r\n"
"\r\n"
"    Permission is hereby granted, free of charge, to any person obtaining a\r\n"
"    copy of this software and associated documentation files (the\r\n"
"    \"Software\"), to deal in the Software without restriction, including\r\n"
"    without limitation the rights to use, copy, modify, merge, publish,\r\n"
"    distribute, sublicense, and/or sell copies of the Software, and to\r\n"
"    permit persons to whom the Software is furnished to do so, subject to\r\n"
"    the following conditions:\r\n"
"\r\n"
"    The above copyright notice and this permission notice shall be included\r\n"
"    in all copies or substantial portions of the Software.\r\n"
"\r\n"
"    THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS\r\n"
"    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\r\n"
"    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\r\n"
"    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY\r\n"
"    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,\r\n"
"    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE\r\n"
"    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\r\n"
"  ]]\r\n"
"}\r\n"
"\r\n"
"inspect.KEY       = setmetatable({}, {__tostring = function() return 'inspect.KEY' end})\r\n"
"inspect.METATABLE = setmetatable({}, {__tostring = function() return 'inspect.METATABLE' end})\r\n"
"\r\n"
"-- Apostrophizes the string if it has quotes, but not aphostrophes\r\n"
"-- Otherwise, it returns a regular quoted string\r\n"
"local function smartQuote(str)\r\n"
"  if str:match('\"') and not str:match(\"'\") then\r\n"
"    return \"'\" .. str .. \"'\"\r\n"
"  end\r\n"
"  return '\"' .. str:gsub('\"', '\\\\\"') .. '\"'\r\n"
"end\r\n"
"\r\n"
"local controlCharsTranslation = {\r\n"
"  [\"\\a\"] = \"\\\\a\",  [\"\\b\"] = \"\\\\b\", [\"\\f\"] = \"\\\\f\",  [\"\\n\"] = \"\\\\n\",\r\n"
"  [\"\\r\"] = \"\\\\r\",  [\"\\t\"] = \"\\\\t\", [\"\\v\"] = \"\\\\v\"\r\n"
"}\r\n"
"\r\n"
"local function escape(str)\r\n"
"  local result = str:gsub(\"\\\\\", \"\\\\\\\\\"):gsub(\"(%c)\", controlCharsTranslation)\r\n"
"  return result\r\n"
"end\r\n"
"\r\n"
"local function isIdentifier(str)\r\n"
"  return type(str) == 'string' and str:match( \"^[_%a][_%a%d]*$\" )\r\n"
"end\r\n"
"\r\n"
"local function isSequenceKey(k, length)\r\n"
"  return type(k) == 'number'\r\n"
"     and 1 <= k\r\n"
"     and k <= length\r\n"
"     and math.floor(k) == k\r\n"
"end\r\n"
"\r\n"
"local defaultTypeOrders = {\r\n"
"  ['number']   = 1, ['boolean']  = 2, ['string'] = 3, ['table'] = 4,\r\n"
"  ['function'] = 5, ['userdata'] = 6, ['thread'] = 7\r\n"
"}\r\n"
"\r\n"
"local function sortKeys(a, b)\r\n"
"  local ta, tb = type(a), type(b)\r\n"
"\r\n"
"  -- strings and numbers are sorted numerically/alphabetically\r\n"
"  if ta == tb and (ta == 'string' or ta == 'number') then return a < b end\r\n"
"\r\n"
"  local dta, dtb = defaultTypeOrders[ta], defaultTypeOrders[tb]\r\n"
"  -- Two default types are compared according to the defaultTypeOrders table\r\n"
"  if dta and dtb then return defaultTypeOrders[ta] < defaultTypeOrders[tb]\r\n"
"  elseif dta     then return true  -- default types before custom ones\r\n"
"  elseif dtb     then return false -- custom types after default ones\r\n"
"  end\r\n"
"\r\n"
"  -- custom types are sorted out alphabetically\r\n"
"  return ta < tb\r\n"
"end\r\n"
"\r\n"
"local function getNonSequentialKeys(t)\r\n"
"  local keys, length = {}, #t\r\n"
"  for k,_ in pairs(t) do\r\n"
"    if not isSequenceKey(k, length) then table.insert(keys, k) end\r\n"
"  end\r\n"
"  table.sort(keys, sortKeys)\r\n"
"  return keys\r\n"
"end\r\n"
"\r\n"
"local function getToStringResultSafely(t, mt)\r\n"
"  local __tostring = type(mt) == 'table' and rawget(mt, '__tostring')\r\n"
"  local str, ok\r\n"
"  if type(__tostring) == 'function' then\r\n"
"    ok, str = pcall(__tostring, t)\r\n"
"    str = ok and str or 'error: ' .. tostring(str)\r\n"
"  end\r\n"
"  if type(str) == 'string' and #str > 0 then return str end\r\n"
"end\r\n"
"\r\n"
"local maxIdsMetaTable = {\r\n"
"  __index = function(self, typeName)\r\n"
"    rawset(self, typeName, 0)\r\n"
"    return 0\r\n"
"  end\r\n"
"}\r\n"
"\r\n"
"local idsMetaTable = {\r\n"
"  __index = function (self, typeName)\r\n"
"    local col = setmetatable({}, {__mode = \"kv\"})\r\n"
"    rawset(self, typeName, col)\r\n"
"    return col\r\n"
"  end\r\n"
"}\r\n"
"\r\n"
"local function countTableAppearances(t, tableAppearances)\r\n"
"  tableAppearances = tableAppearances or setmetatable({}, {__mode = \"k\"})\r\n"
"\r\n"
"  if type(t) == 'table' then\r\n"
"    if not tableAppearances[t] then\r\n"
"      tableAppearances[t] = 1\r\n"
"      for k,v in pairs(t) do\r\n"
"        countTableAppearances(k, tableAppearances)\r\n"
"        countTableAppearances(v, tableAppearances)\r\n"
"      end\r\n"
"      countTableAppearances(getmetatable(t), tableAppearances)\r\n"
"    else\r\n"
"      tableAppearances[t] = tableAppearances[t] + 1\r\n"
"    end\r\n"
"  end\r\n"
"\r\n"
"  return tableAppearances\r\n"
"end\r\n"
"\r\n"
"local copySequence = function(s)\r\n"
"  local copy, len = {}, #s\r\n"
"  for i=1, len do copy[i] = s[i] end\r\n"
"  return copy, len\r\n"
"end\r\n"
"\r\n"
"local function makePath(path, ...)\r\n"
"  local keys = {...}\r\n"
"  local newPath, len = copySequence(path)\r\n"
"  for i=1, #keys do\r\n"
"    newPath[len + i] = keys[i]\r\n"
"  end\r\n"
"  return newPath\r\n"
"end\r\n"
"\r\n"
"local function processRecursive(process, item, path)\r\n"
"  if item == nil then return nil end\r\n"
"\r\n"
"  local processed = process(item, path)\r\n"
"  if type(processed) == 'table' then\r\n"
"    local processedCopy = {}\r\n"
"    local processedKey\r\n"
"\r\n"
"    for k,v in pairs(processed) do\r\n"
"      processedKey = processRecursive(process, k, makePath(path, k, inspect.KEY))\r\n"
"      if processedKey ~= nil then\r\n"
"        processedCopy[processedKey] = processRecursive(process, v, makePath(path, processedKey))\r\n"
"      end\r\n"
"    end\r\n"
"\r\n"
"    local mt  = processRecursive(process, getmetatable(processed), makePath(path, inspect.METATABLE))\r\n"
"    setmetatable(processedCopy, mt)\r\n"
"    processed = processedCopy\r\n"
"  end\r\n"
"  return processed\r\n"
"end\r\n"
"\r\n"
"\r\n"
"-------------------------------------------------------------------\r\n"
"\r\n"
"local Inspector = {}\r\n"
"local Inspector_mt = {__index = Inspector}\r\n"
"\r\n"
"function Inspector:puts(...)\r\n"
"  local args   = {...}\r\n"
"  local buffer = self.buffer\r\n"
"  local len    = #buffer\r\n"
"  for i=1, #args do\r\n"
"    len = len + 1\r\n"
"    buffer[len] = tostring(args[i])\r\n"
"  end\r\n"
"end\r\n"
"\r\n"
"function Inspector:down(f)\r\n"
"  self.level = self.level + 1\r\n"
"  f()\r\n"
"  self.level = self.level - 1\r\n"
"end\r\n"
"\r\n"
"function Inspector:tabify()\r\n"
"  self:puts(self.newline, string.rep(self.indent, self.level))\r\n"
"end\r\n"
"\r\n"
"function Inspector:alreadyVisited(v)\r\n"
"  return self.ids[type(v)][v] ~= nil\r\n"
"end\r\n"
"\r\n"
"function Inspector:getId(v)\r\n"
"  local tv = type(v)\r\n"
"  local id = self.ids[tv][v]\r\n"
"  if not id then\r\n"
"    id              = self.maxIds[tv] + 1\r\n"
"    self.maxIds[tv] = id\r\n"
"    self.ids[tv][v] = id\r\n"
"  end\r\n"
"  return id\r\n"
"end\r\n"
"\r\n"
"function Inspector:putKey(k)\r\n"
"  if isIdentifier(k) then return self:puts(k) end\r\n"
"  self:puts(\"[\")\r\n"
"  self:putValue(k)\r\n"
"  self:puts(\"]\")\r\n"
"end\r\n"
"\r\n"
"function Inspector:putTable(t)\r\n"
"  if t == inspect.KEY or t == inspect.METATABLE then\r\n"
"    self:puts(tostring(t))\r\n"
"  elseif self:alreadyVisited(t) then\r\n"
"    self:puts('<table ', self:getId(t), '>')\r\n"
"  elseif self.level >= self.depth then\r\n"
"    self:puts('{...}')\r\n"
"  else\r\n"
"    if self.tableAppearances[t] > 1 then self:puts('<', self:getId(t), '>') end\r\n"
"\r\n"
"    local nonSequentialKeys = getNonSequentialKeys(t)\r\n"
"    local length            = #t\r\n"
"    local mt                = getmetatable(t)\r\n"
"    local toStringResult    = getToStringResultSafely(t, mt)\r\n"
"\r\n"
"    self:puts('{')\r\n"
"    self:down(function()\r\n"
"      if toStringResult then\r\n"
"        self:puts(' -- ', escape(toStringResult))\r\n"
"        if length >= 1 then self:tabify() end\r\n"
"      end\r\n"
"\r\n"
"      local count = 0\r\n"
"      for i=1, length do\r\n"
"        if count > 0 then self:puts(',') end\r\n"
"        self:puts(' ')\r\n"
"        self:putValue(t[i])\r\n"
"        count = count + 1\r\n"
"      end\r\n"
"\r\n"
"      for _,k in ipairs(nonSequentialKeys) do\r\n"
"        if count > 0 then self:puts(',') end\r\n"
"        self:tabify()\r\n"
"        self:putKey(k)\r\n"
"        self:puts(' = ')\r\n"
"        self:putValue(t[k])\r\n"
"        count = count + 1\r\n"
"      end\r\n"
"\r\n"
"      if mt then\r\n"
"        if count > 0 then self:puts(',') end\r\n"
"        self:tabify()\r\n"
"        self:puts('<metatable> = ')\r\n"
"        self:putValue(mt)\r\n"
"      end\r\n"
"    end)\r\n"
"\r\n"
"    if #nonSequentialKeys > 0 or mt then -- result is multi-lined. Justify closing }\r\n"
"      self:tabify()\r\n"
"    elseif length > 0 then -- array tables have one extra space before closing }\r\n"
"      self:puts(' ')\r\n"
"    end\r\n"
"\r\n"
"    self:puts('}')\r\n"
"  end\r\n"
"end\r\n"
"\r\n"
"function Inspector:putValue(v)\r\n"
"  local tv = type(v)\r\n"
"\r\n"
"  if tv == 'string' then\r\n"
"    self:puts(smartQuote(escape(v)))\r\n"
"  elseif tv == 'number' or tv == 'boolean' or tv == 'nil' then\r\n"
"    self:puts(tostring(v))\r\n"
"  elseif tv == 'table' then\r\n"
"    self:putTable(v)\r\n"
"  else\r\n"
"    self:puts('<',tv,' ',self:getId(v),'>')\r\n"
"  end\r\n"
"end\r\n"
"\r\n"
"-------------------------------------------------------------------\r\n"
"\r\n"
"function inspect.inspect(root, options)\r\n"
"  options       = options or {}\r\n"
"\r\n"
"  local depth   = options.depth   or math.huge\r\n"
"  local newline = options.newline or '\\n'\r\n"
"  local indent  = options.indent  or '  '\r\n"
"  local process = options.process\r\n"
"\r\n"
"  if process then\r\n"
"    root = processRecursive(process, root, {})\r\n"
"  end\r\n"
"\r\n"
"  local inspector = setmetatable({\r\n"
"    depth            = depth,\r\n"
"    buffer           = {},\r\n"
"    level            = 0,\r\n"
"    ids              = setmetatable({}, idsMetaTable),\r\n"
"    maxIds           = setmetatable({}, maxIdsMetaTable),\r\n"
"    newline          = newline,\r\n"
"    indent           = indent,\r\n"
"    tableAppearances = countTableAppearances(root)\r\n"
"  }, Inspector_mt)\r\n"
"\r\n"
"  inspector:putValue(root)\r\n"
"\r\n"
"  return table.concat(inspector.buffer)\r\n"
"end\r\n"
"\r\n"
"setmetatable(inspect, { __call = function(_, ...) return inspect.inspect(...) end })\r\n"
"\r\n"
"return inspect\r\n"
"end\r\n"
"\r\n"
"inspect = getInspect()";

const char* inspect_lua = (const char*) temp_binary_data_127;

//================== lf.template ==================
static const unsigned char temp_binary_data_128[] =
"        static Colour def_findColour (LookAndFeel_V3 *ptr, int colourId)\r\n"
"        static void def_setColour (LookAndFeel_V3 *ptr, int colourId, Colour colour)\r\n"
"        static bool def_isColourSpecified (LookAndFeel_V3 *ptr, int colourId)\r\n"
"        static Typeface::Ptr def_getTypefaceForFont (LookAndFeel_V3 *ptr, const Font &font)\r\n"
"        static MouseCursor def_getMouseCursorFor (LookAndFeel_V3 *ptr, Component &component)\r\n"
"        static void def_drawButtonBackground (LookAndFeel_V3 *ptr, Graphics &g, Button &button, const Colour &backgroundColourRef, bool isMouseOverButton, bool isButtonDown)\r\n"
"        static void def_drawButtonText (LookAndFeel_V3 *ptr, Graphics &g, TextButton &textButton, bool isMouseOverButton, bool isButtonDown)\r\n"
"        static void def_drawToggleButton (LookAndFeel_V3 *ptr, Graphics &g, ToggleButton &toggleButton, bool isMouseOverButton, bool isButtonDown)\r\n"
"        static void def_changeToggleButtonWidthToFitText (LookAndFeel_V3 *ptr, ToggleButton &toggleButton)\r\n"
"        static void def_drawTickBox (LookAndFeel_V3 *ptr, Graphics &g, Component &component, float x, float y, float w, float h, bool ticked, bool isEnabled, bool isMouseOverButton, bool isButtonDown)\r\n"
"        static AlertWindow* def_createAlertWindow (LookAndFeel_V3 *ptr, const String &title, const String &message, const String &button1, const String &button2, const String &button3, AlertWindow::AlertIconType iconType, int numButtons, Component* c"
"omponent)\r\n"
"        static void def_drawAlertBox (LookAndFeel_V3 *ptr, Graphics &g, AlertWindow &window, const Rectangle<int> &textArea, TextLayout &layout)\r\n"
"        static int def_getAlertBoxWindowFlags (LookAndFeel_V3 *ptr)\r\n"
"        static int def_getAlertWindowButtonHeight (LookAndFeel_V3 *ptr)\r\n"
"        static Font def_getAlertWindowMessageFont (LookAndFeel_V3 *ptr)\r\n"
"        static Font def_getAlertWindowFont (LookAndFeel_V3 *ptr)\r\n"
"        static void def_drawProgressBar (LookAndFeel_V3 *ptr, Graphics &g, ProgressBar &progressBar, int width, int height, double progress, const String &textToShow)\r\n"
"        static void def_drawSpinningWaitAnimation (LookAndFeel_V3 *ptr, Graphics &g, const Colour &colourRef, int x, int y, int w, int h)\r\n"
"        static bool def_areScrollbarButtonsVisible (LookAndFeel_V3 *ptr)\r\n"
"        static ImageEffectFilter* def_getScrollbarEffect (LookAndFeel_V3 *ptr)\r\n"
"        static int def_getMinimumScrollbarThumbSize (LookAndFeel_V3 *ptr, ScrollBar &scrollBar)\r\n"
"        static int def_getDefaultScrollbarWidth (LookAndFeel_V3 *ptr)\r\n"
"        static int def_getScrollbarButtonSize (LookAndFeel_V3 *ptr, ScrollBar &scrollBar)\r\n"
"        static Path def_getTickShape (LookAndFeel_V3 *ptr, float height)\r\n"
"        static Path def_getCrossShape (LookAndFeel_V3 *ptr, float height)\r\n"
"        static void def_drawTreeviewPlusMinusBox (LookAndFeel_V3 *ptr, Graphics &g, const Rectangle<float> &area, Colour backgroundColour, bool isOpen, bool isMouseOver)\r\n"
"        static void def_fillTextEditorBackground (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, TextEditor &editor)\r\n"
"        static void def_drawTextEditorOutline (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, TextEditor &editor)\r\n"
"        static CaretComponent* def_createCaretComponent (LookAndFeel_V3 *ptr, Component* component)\r\n"
"        static const Drawable* def_getDefaultFolderImage (LookAndFeel_V3 *ptr)\r\n"
"        static const Drawable* def_getDefaultDocumentFileImage (LookAndFeel_V3 *ptr)\r\n"
"        static AttributedString def_createFileChooserHeaderText (LookAndFeel_V3 *ptr, const String &title, const String &instructions)\r\n"
"        static void def_drawFileBrowserRow (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, const String &filename, Image* icon, const String &fileSizeDescription, const String &fileTimeDescription, bool isDirectory, bool isItemSelected, int"
" itemIndex, DirectoryContentsDisplayComponent &directoryContentsDisplayComponent)\r\n"
"        static Button* def_createFileBrowserGoUpButton (LookAndFeel_V3 *ptr)\r\n"
"        static void def_layoutFileBrowserComponent (LookAndFeel_V3 *ptr, FileBrowserComponent &fileBrowserComponent, DirectoryContentsDisplayComponent *directoryContentsDisplayComponent, FilePreviewComponent *filePreviewComponent, ComboBox *comboBox,"
" TextEditor *textEditor, Button *goUpButton)\r\n"
"        static void def_drawBubble (LookAndFeel_V3 *ptr, Graphics &g, BubbleComponent &bubbleComponent, const Point<float> &tip, const Rectangle<float> &body)\r\n"
"        static void def_drawLasso (LookAndFeel_V3 *ptr, Graphics &g, Component &component)\r\n"
"        static void def_drawPopupMenuBackground (LookAndFeel_V3 *ptr, Graphics &g, int width, int height)\r\n"
"        static void def_drawPopupMenuItem (LookAndFeel_V3 *ptr, Graphics &g, const Rectangle<int> &areaInt, bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu, const String &text, const String &shortcutKeyText, const "
"Drawable* drawableIcon, const Colour *textColourPtr)\r\n"
"        static Font def_getPopupMenuFont (LookAndFeel_V3 *ptr)\r\n"
"        static void def_drawPopupMenuUpDownArrow (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, bool isScrollUpArrow)\r\n"
"        static void def_getIdealPopupMenuItemSize (LookAndFeel_V3 *ptr, const String &text, bool isSeparator, int standardMenuItemHeight, int &idealWidth, int &idealHeight)\r\n"
"        static int def_getMenuWindowFlags (LookAndFeel_V3 *ptr)\r\n"
"        static void def_drawMenuBarBackground (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBarComponent)\r\n"
"        static int def_getMenuBarItemWidth (LookAndFeel_V3 *ptr, MenuBarComponent &menuBarComponent, int itemIndex, const String &itemText)\r\n"
"        static Font def_getMenuBarFont (LookAndFeel_V3 *ptr, MenuBarComponent &menuBarComponent, int itemIndex, const String &itemText)\r\n"
"        static void def_drawMenuBarItem (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, int itemIndex, const String &itemText, bool isMouseOverItem, bool isMenuOpen, bool isMouseOverBar, MenuBarComponent &menuBarComponent)\r\n"
"        static void def_drawComboBox (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox &comboBox)\r\n"
"        static Font def_getComboBoxFont (LookAndFeel_V3 *ptr, ComboBox &comboBox)\r\n"
"        static Label* def_createComboBoxTextBox (LookAndFeel_V3 *ptr, ComboBox &comboBox)\r\n"
"        static void def_positionComboBoxText (LookAndFeel_V3 *ptr, ComboBox &comboBox, Label &label)\r\n"
"        static void def_drawLabel (LookAndFeel_V3 *ptr, Graphics &g, Label &label)\r\n"
"        static void def_drawLinearSlider (LookAndFeel_V3 *ptr, Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle sliderStyle, Slider &slider)\r\n"
"        static void def_drawLinearSliderBackground (LookAndFeel_V3 *ptr, Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle sliderStyle, Slider &slider)\r\n"
"        static void def_drawLinearSliderThumb (LookAndFeel_V3 *ptr, Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle sliderStyle, Slider &slider)\r\n"
"        static int def_getSliderThumbRadius (LookAndFeel_V3 *ptr, Slider &slider)\r\n"
"        static void def_drawRotarySlider (LookAndFeel_V3 *ptr, Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider &slider)\r\n"
"        static Button* def_createSliderButton (LookAndFeel_V3 *ptr, Slider &slider, bool isIncrement)\r\n"
"        static Label* def_createSliderTextBox (LookAndFeel_V3 *ptr, Slider &slider)\r\n"
"        static ImageEffectFilter* def_getSliderEffect (LookAndFeel_V3 *ptr, Slider &slider)\r\n"
"        static void def_getTooltipSize (LookAndFeel_V3 *ptr, const String &tipText, int &width, int &height)\r\n"
"        static void def_drawTooltip (LookAndFeel_V3 *ptr, Graphics &g, const String &text, int width, int height)\r\n"
"        static Button* def_createFilenameComponentBrowseButton (LookAndFeel_V3 *ptr, const String &text)\r\n"
"        static void def_layoutFilenameComponent (LookAndFeel_V3 *ptr, FilenameComponent &filenameComponent, ComboBox* comboBox, Button* button)\r\n"
"        static void def_drawConcertinaPanelHeader (LookAndFeel_V3 *ptr, Graphics &g, const Rectangle<int> &areaInt, bool isMouseOver, bool isMouseDown, ConcertinaPanel &concertinaPanel, Component &component)\r\n"
"        static void def_drawCornerResizer (LookAndFeel_V3 *ptr, Graphics &g, int w, int h, bool isMouseOver, bool isMouseDragging)\r\n"
"        static void def_drawResizableFrame (LookAndFeel_V3 *ptr, Graphics &g, int w, int h, const BorderSize<int> &borderSize)\r\n"
"        static void def_fillResizableWindowBackground (LookAndFeel_V3 *ptr, Graphics &g, int w, int h, const BorderSize<int> &borderSize, ResizableWindow &resizableWindow)\r\n"
"        static void def_drawResizableWindowBorder (LookAndFeel_V3 *ptr, Graphics &g, int w, int h, const BorderSize<int> &borderSize, ResizableWindow &resizableWindow)\r\n"
"        static void def_drawDocumentWindowTitleBar (LookAndFeel_V3 *ptr, DocumentWindow &documentWindow, Graphics &g, int w, int h, int titleSpaceX, int titleSpaceW, const Image* icon, bool drawTitleTextOnLeft)\r\n"
"        static Button* def_createDocumentWindowButton (LookAndFeel_V3 *ptr, int buttonType)\r\n"
"        static void def_positionDocumentWindowButtons (LookAndFeel_V3 *ptr, DocumentWindow &documentWindow, int titleBarX, int titleBarY, int titleBarW, int titleBarH, Button* minimiseButton, Button* maximiseButton, Button* closeButton, bool position"
"TitleBarButtonsOnLeft)\r\n"
"        static int def_getDefaultMenuBarHeight (LookAndFeel_V3 *ptr)\r\n"
"        static DropShadower* def_createDropShadowerForComponent (LookAndFeel_V3 *ptr, Component* component)\r\n"
"        static void def_drawGroupComponentOutline (LookAndFeel_V3 *ptr, Graphics &g, int w, int h, const String &text, const Justification &justification, GroupComponent &groupComponent)\r\n"
"        static int def_getTabButtonSpaceAroundImage (LookAndFeel_V3 *ptr)\r\n"
"        static int def_getTabButtonOverlap (LookAndFeel_V3 *ptr, int tabDepth)\r\n"
"        static int def_getTabButtonBestWidth (LookAndFeel_V3 *ptr, TabBarButton &tabBarButton, int tabDepth)\r\n"
"        static Rectangle<int> def_getTabButtonExtraComponentBounds (LookAndFeel_V3 *ptr, const TabBarButton &tabBarButton, Rectangle<int> &areaInt, Component &component)\r\n"
"        static void def_drawTabButton (LookAndFeel_V3 *ptr, TabBarButton &tabBarButton, Graphics &g, bool isMouseOver, bool isMouseDown)\r\n"
"        static void def_drawTabButtonText (LookAndFeel_V3 *ptr, TabBarButton &tabBarButton, Graphics &g, bool isMouseOver, bool isMouseDown)\r\n"
"        static void def_drawTabAreaBehindFrontButton (LookAndFeel_V3 *ptr, TabbedButtonBar &tabbedButtonBar, Graphics &g, int w, int h)\r\n"
"        static void def_createTabButtonShape (LookAndFeel_V3 *ptr, TabBarButton &tabBarButton, Path &path, bool isMouseOver, bool isMouseDown)\r\n"
"        static void def_fillTabButtonShape (LookAndFeel_V3 *ptr, TabBarButton &tabBarButton, Graphics &g, const Path &path, bool isMouseOver, bool isMouseDown)\r\n"
"        static Button* def_createTabBarExtrasButton (LookAndFeel_V3 *ptr)\r\n"
"        static void def_drawImageButton (LookAndFeel_V3 *ptr, Graphics &g, Image* image, int imageX, int imageY, int imageW, int imageH, const Colour &overlayColourRef, float imageOpacity, ImageButton &imageButton)\r\n"
"        static void def_drawTableHeaderBackground (LookAndFeel_V3 *ptr, Graphics &g, TableHeaderComponent &tableHeaderComponent)\r\n"
"        static void def_drawTableHeaderColumn (LookAndFeel_V3 *ptr, Graphics &g, const String &columnName, int columnId, int width, int height, bool isMouseOver, bool isMouseDown, int columnFlags)\r\n"
"        static void def_paintToolbarBackground (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, Toolbar &toolbar)\r\n"
"        static Button* def_createToolbarMissingItemsButton (LookAndFeel_V3 *ptr, Toolbar &toolbar)\r\n"
"        static void def_paintToolbarButtonBackground (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, bool isMouseOver, bool isMouseDown, ToolbarItemComponent &toolbarItemComponent)\r\n"
"        static void def_paintToolbarButtonLabel (LookAndFeel_V3 *ptr, Graphics &g, int x, int y, int width, int height, const String &text, ToolbarItemComponent &toolbarItemComponent)\r\n"
"        static void def_drawStretchableLayoutResizerBar (LookAndFeel_V3 *ptr, Graphics &g, int w, int h, bool isVerticalBar, bool isMouseOver, bool isMouseDragging)\r\n"
"        static void def_drawPropertyPanelSectionHeader (LookAndFeel_V3 *ptr, Graphics &g, const String &name, bool isOpen, int width, int height)\r\n"
"        static void def_drawPropertyComponentBackground (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, PropertyComponent &propertyComponent)\r\n"
"        static void def_drawPropertyComponentLabel (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, PropertyComponent &propertyComponent)\r\n"
"        static Rectangle<int> def_getPropertyComponentContentPosition (LookAndFeel_V3 *ptr, PropertyComponent &propertyComponent)\r\n"
"        static void def_drawCallOutBoxBackground (LookAndFeel_V3 *ptr, CallOutBox &callOutBox, Graphics &g, const Path &path, Image &image)\r\n"
"        static void def_drawLevelMeter (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, float level)\r\n"
"        static void def_drawKeymapChangeButton (LookAndFeel_V3 *ptr, Graphics &g, int width, int height, Button &button, const String &keyDescription)\r\n";

const char* lf_template = (const char*) temp_binary_data_128;

//================== LookAndFeel.lua ==================
static const unsigned char temp_binary_data_129[] =
"class '__method_name' (LookAndFeel)\r\n"
"\r\n"
"function __method_name:__init()\r\n"
"\t--\r\n"
"\t-- This is needed for luabind, please don't remove this line\r\n"
"\t-- Add as much code as you like here, just leave this\r\n"
"\t--\r\n"
"\tLookAndFeel.__init(self)\r\n"
"end\r\n"
"\r\n"
"-- function __method_name:findColour (--[[ int p.colourId --]] p)\r\n"
"--\t return Colour(0xff000000)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:setColour (--[[ int p.colourId, Colour p.colour --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:isColourSpecified (--[[ int p.colourId --]] p)\r\n"
"--\t return bool\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getTypefaceForFont (--[[ Font p.font --]] p)\r\n"
"--\t return Typeface::Ptr\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getMouseCursorFor (--[[ Component p.component --]] p)\r\n"
"--\t return MouseCursor()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawButtonBackground (--[[ Graphics p.g, Button p.button, Colour p.backgroundColourRef, bool p.isMouseOverButton, bool p.isButtonDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawButtonText (--[[ Graphics p.g, TextButton p.textButton, bool p.isMouseOverButton, bool p.isButtonDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawToggleButton (--[[ Graphics p.g, ToggleButton p.toggleButton, bool p.isMouseOverButton, bool p.isButtonDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:changeToggleButtonWidthToFitText (--[[ ToggleButton p.toggleButton --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTickBox (--[[ Graphics p.g, Component p.component, float p.x, float p.y, float p.w, float p.h, bool p.ticked, bool p.isEnabled, bool p.isMouseOverButton, bool p.isButtonDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createAlertWindow (--[[ String p.title, String p.message, String p.button1, String p.button2, String p.button3, AlertWindow::AlertIconType p.iconType, int p.numButtons, Component* p.component --]] p)\r\n"
"--\t return AlertWindow*\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawAlertBox (--[[ Graphics p.g, AlertWindow p.window, Rectangle p.textArea, TextLayout p.layout --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getAlertBoxWindowFlags (--[[  --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getAlertWindowButtonHeight (--[[  --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getAlertWindowMessageFont (--[[  --]] p)\r\n"
"--\t return Font\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getAlertWindowFont (--[[  --]] p)\r\n"
"--\t return Font()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawProgressBar (--[[ Graphics p.g, ProgressBar p.progressBar, int p.width, int p.height, double p.progress, String p.textToShow --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawSpinningWaitAnimation (--[[ Graphics p.g, Colour p.colourRef, int p.x, int p.y, int p.w, int p.h --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:areScrollbarButtonsVisible (--[[  --]] p)\r\n"
"--\t return bool\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getScrollbarEffect (--[[  --]] p)\r\n"
"--\t return ImageEffectFilter*\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getMinimumScrollbarThumbSize (--[[ ScrollBar p.scrollBar --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getDefaultScrollbarWidth (--[[  --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getScrollbarButtonSize (--[[ ScrollBar p.scrollBar --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getTickShape (--[[ float p.height --]] p)\r\n"
"--\t return Path()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getCrossShape (--[[ float p.height --]] p)\r\n"
"--\t return Path()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTreeviewPlusMinusBox (--[[ Graphics p.g, Rectangle p.area, Colour p.backgroundColour, bool p.isOpen, bool p.isMouseOver --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:fillTextEditorBackground (--[[ Graphics p.g, int p.width, int p.height, TextEditor p.editor --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTextEditorOutline (--[[ Graphics p.g, int p.width, int p.height, TextEditor p.editor --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createCaretComponent (--[[ Component* p.component --]] p)\r\n"
"--\t return CaretComponent()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getDefaultFolderImage (--[[  --]] p)\r\n"
"--\t return Drawable()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getDefaultDocumentFileImage (--[[  --]] p)\r\n"
"--\t return Drawable()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createFileChooserHeaderText (--[[ String p.title, String p.instructions --]] p)\r\n"
"--\t return AttributedString()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawFileBrowserRow (--[[ Graphics p.g, int p.width, int p.height, String p.filename, Image* p.icon, String p.fileSizeDescription, String p.fileTimeDescription, bool p.isDirectory, bool p.isItemSelected, int p.itemIndex, Dire"
"ctoryContentsDisplayComponent p.directoryContentsDisplayComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createFileBrowserGoUpButton (--[[  --]] p)\r\n"
"--\t return Button()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:layoutFileBrowserComponent (--[[ FileBrowserComponent p.fileBrowserComponent, DirectoryContentsDisplayComponent p.*directoryContentsDisplayComponent, FilePreviewComponent p.*filePreviewComponent, ComboBox p.*comboBox, TextEd"
"itor p.*textEditor, Button p.*goUpButton --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawBubble (--[[ Graphics p.g, BubbleComponent p.bubbleComponent, Point p.tip, Rectangle p.body --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawLasso (--[[ Graphics p.g, Component p.component --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawPopupMenuBackground (--[[ Graphics p.g, int p.width, int p.height --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawPopupMenuItem (--[[ Graphics p.g, Rectangle p.menuItemArea, bool p.isSeparator, bool p.isActive, bool p.isHighlighted, bool p.isTicked, bool p.hasSubMenu, String p.text, String p.shortcutKeyText, Drawable* p.drawableIcon"
", Colour p.*textColourPtr --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getPopupMenuFont (--[[  --]] p)\r\n"
"--\t return Font\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawPopupMenuUpDownArrow (--[[ Graphics p.g, int p.width, int p.height, bool p.isScrollUpArrow --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getIdealPopupMenuItemSize (--[[ String p.text, bool p.isSeparator, int p.standardMenuItemHeight, int p.idealWidth, int p.idealHeight --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getMenuWindowFlags (--[[  --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawMenuBarBackground (--[[ Graphics p.g, int p.width, int p.height, bool p.isMouseOverBar, MenuBarComponent p.menuBarComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getMenuBarItemWidth (--[[ MenuBarComponent p.menuBarComponent, int p.itemIndex, String p.itemText --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getMenuBarFont (--[[ MenuBarComponent p.menuBarComponent, int p.itemIndex, String p.itemText --]] p)\r\n"
"--\t return Font()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawMenuBarItem (--[[ Graphics p.g, int p.width, int p.height, int p.itemIndex, String p.itemText, bool p.isMouseOverItem, bool p.isMenuOpen, bool p.isMouseOverBar, MenuBarComponent p.menuBarComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawComboBox (--[[ Graphics p.g, int p.width, int p.height, bool p.isButtonDown, int p.buttonX, int p.buttonY, int p.buttonW, int p.buttonH, ComboBox p.comboBox --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getComboBoxFont (--[[ ComboBox p.comboBox --]] p)\r\n"
"--\t return Font()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createComboBoxTextBox (--[[ ComboBox p.comboBox --]] p)\r\n"
"--\t return Label()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:positionComboBoxText (--[[ ComboBox p.comboBox, Label p.label --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawLabel (--[[ Graphics p.g, Label p.label --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawLinearSlider (--[[ Graphics p.g, int p.x, int p.y, int p.width, int p.height, float p.sliderPos, float p.minSliderPos, float p.maxSliderPos, Slider::SliderStyle p.sliderStyle, Slider p.slider --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawLinearSliderBackground (--[[ Graphics p.g, int p.x, int p.y, int p.width, int p.height, float p.sliderPos, float p.minSliderPos, float p.maxSliderPos, Slider::SliderStyle p.sliderStyle, Slider p.slider --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawLinearSliderThumb (--[[ Graphics p.g, int p.x, int p.y, int p.width, int p.height, float p.sliderPos, float p.minSliderPos, float p.maxSliderPos, Slider::SliderStyle p.sliderStyle, Slider p.slider --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getSliderThumbRadius (--[[ Slider p.slider --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawRotarySlider (--[[ Graphics p.g, int p.x, int p.y, int p.width, int p.height, float p.sliderPosProportional, float p.rotaryStartAngle, float p.rotaryEndAngle, Slider p.slider --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createSliderButton (--[[ Slider p.slider, bool p.isIncrement --]] p)\r\n"
"--\t return Button()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createSliderTextBox (--[[ Slider p.slider --]] p)\r\n"
"--\t return Label()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getSliderEffect (--[[ Slider p.slider --]] p)\r\n"
"--\t return ImageEffectFilter()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getTooltipSize (--[[ String p.tipText, int p.width, int p.height --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTooltip (--[[ Graphics p.g, String p.text, int p.width, int p.height --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createFilenameComponentBrowseButton (--[[ String p.text --]] p)\r\n"
"--\t return Button()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:layoutFilenameComponent (--[[ FilenameComponent p.filenameComponent, ComboBox* p.comboBox, Button* p.button --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawConcertinaPanelHeader (--[[ Graphics p.g, Rectangle p.areaInt, bool p.isMouseOver, bool p.isMouseDown, ConcertinaPanel p.concertinaPanel, Component p.component --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawCornerResizer (--[[ Graphics p.g, int p.w, int p.h, bool p.isMouseOver, bool p.isMouseDragging --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawResizableFrame (--[[ Graphics p.g, int p.w, int p.h, BorderSize p.borderSize --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:fillResizableWindowBackground (--[[ Graphics p.g, int p.w, int p.h, BorderSize p.borderSize, ResizableWindow p.resizableWindow --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawResizableWindowBorder (--[[ Graphics p.g, int p.w, int p.h, BorderSize p.borderSize, ResizableWindow p.resizableWindow --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawDocumentWindowTitleBar (--[[ DocumentWindow p.documentWindow, Graphics p.g, int p.w, int p.h, int p.titleSpaceX, int p.titleSpaceW, Image* p.icon, bool p.drawTitleTextOnLeft --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createDocumentWindowButton (--[[ int p.buttonType --]] p)\r\n"
"--\t return Button()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:positionDocumentWindowButtons (--[[ DocumentWindow p.documentWindow, int p.titleBarX, int p.titleBarY, int p.titleBarW, int p.titleBarH, Button* p.minimiseButton, Button* p.maximiseButton, Button* p.closeButton, bool p.posit"
"ionTitleBarButtonsOnLeft --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getDefaultMenuBarHeight (--[[  --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createDropShadowerForComponent (--[[ Component* p.component --]] p)\r\n"
"--\t return DropShadower()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawGroupComponentOutline (--[[ Graphics p.g, int p.w, int p.h, String p.text, Justification p.justification, GroupComponent p.groupComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getTabButtonSpaceAroundImage (--[[  --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getTabButtonOverlap (--[[ int p.tabDepth --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getTabButtonBestWidth (--[[ TabBarButton p.tabBarButton, int p.tabDepth --]] p)\r\n"
"--\t return int\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getTabButtonExtraComponentBounds (--[[ TabBarButton p.tabBarButton, Rectangle p.areaInt, Component p.component --]] p)\r\n"
"--\t return Rectangle()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTabButton (--[[ TabBarButton p.tabBarButton, Graphics p.g, bool p.isMouseOver, bool p.isMouseDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTabButtonText (--[[ TabBarButton p.tabBarButton, Graphics p.g, bool p.isMouseOver, bool p.isMouseDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTabAreaBehindFrontButton (--[[ TabbedButtonBar p.tabbedButtonBar, Graphics p.g, int p.w, int p.h --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createTabButtonShape (--[[ TabBarButton p.tabBarButton, Path p.path, bool p.isMouseOver, bool p.isMouseDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:fillTabButtonShape (--[[ TabBarButton p.tabBarButton, Graphics p.g, Path p.path, bool p.isMouseOver, bool p.isMouseDown --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createTabBarExtrasButton (--[[  --]] p)\r\n"
"--\t return Button()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawImageButton (--[[ Graphics p.g, Image* p.image, int p.imageX, int p.imageY, int p.imageW, int p.imageH, Colour p.overlayColourRef, float p.imageOpacity, ImageButton p.imageButton --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTableHeaderBackground (--[[ Graphics p.g, TableHeaderComponent p.tableHeaderComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawTableHeaderColumn (--[[ Graphics p.g, String p.columnName, int p.columnId, int p.width, int p.height, bool p.isMouseOver, bool p.isMouseDown, int p.columnFlags --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:paintToolbarBackground (--[[ Graphics p.g, int p.width, int p.height, Toolbar p.toolbar --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:createToolbarMissingItemsButton (--[[ Toolbar p.toolbar --]] p)\r\n"
"--\t return Button()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:paintToolbarButtonBackground (--[[ Graphics p.g, int p.width, int p.height, bool p.isMouseOver, bool p.isMouseDown, ToolbarItemComponent p.toolbarItemComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:paintToolbarButtonLabel (--[[ Graphics p.g, int p.x, int p.y, int p.width, int p.height, String p.text, ToolbarItemComponent p.toolbarItemComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawStretchableLayoutResizerBar (--[[ Graphics p.g, int p.w, int p.h, bool p.isVerticalBar, bool p.isMouseOver, bool p.isMouseDragging --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawPropertyPanelSectionHeader (--[[ Graphics p.g, String p.name, bool p.isOpen, int p.width, int p.height --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawPropertyComponentBackground (--[[ Graphics p.g, int p.width, int p.height, PropertyComponent p.propertyComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawPropertyComponentLabel (--[[ Graphics p.g, int p.width, int p.height, PropertyComponent p.propertyComponent --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:getPropertyComponentContentPosition (--[[ PropertyComponent p.propertyComponent --]] p)\r\n"
"--\t return Rectangle()\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawCallOutBoxBackground (--[[ CallOutBox p.callOutBox, Graphics p.g, Path p.path, Image p.image --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawLevelMeter (--[[ Graphics p.g, int p.width, int p.height, float p.level --]] p)\r\n"
"-- end\r\n"
"\r\n"
"-- function __method_name:drawKeymapChangeButton (--[[ Graphics p.g, int p.width, int p.height, Button p.button, String p.keyDescription --]] p)\r\n"
"-- end\r\n"
"\r\n";

const char* LookAndFeel_lua = (const char*) temp_binary_data_129;

//================== paramWrapper.template ==================
static const unsigned char temp_binary_data_130[] =
"\tGraphics *g;\r\n"
"\tint x;\r\n"
"\tint y;\r\n"
"\tint w;\r\n"
"\tint h;\r\n"
"\tint width;\r\n"
"\tint height;\r\n"
"\tint colourId;\r\n"
"\tint buttonX;\r\n"
"\tint buttonY;\r\n"
"\tint buttonW;\r\n"
"\tint buttonH;\r\n"
"\tint titleSpaceX;\r\n"
"\tint titleSpaceW;\r\n"
"\tint buttonType;\r\n"
"\tint titleBarX;\r\n"
"\tint titleBarY;\r\n"
"\tint titleBarW;\r\n"
"\tint titleBarH;\r\n"
"\tint tabDepth;\r\n"
"\tint imageX;\r\n"
"\tint imageY;\r\n"
"\tint imageW;\r\n"
"\tint imageH;\r\n"
"\tint columnId;\r\n"
"\tint columnFlags;\r\n"
"\tfloat sliderPos;\r\n"
"\tfloat minSliderPos;\r\n"
"\tfloat maxSliderPos;\r\n"
"\tfloat imageOpacity;\r\n"
"\tfloat level;\r\n"
"\tconst Font *font;\r\n"
"\tComponent *component;\r\n"
"\tButton *button;\r\n"
"\tImageButton *imageButton;\r\n"
"\tTextButton *textButton;\r\n"
"\tToggleButton *toggleButton;\r\n"
"\tColour colour;\r\n"
"\tColour backgroundColour;\r\n"
"\tColour textColour;\r\n"
"\tColour overlayColour;\r\n"
"\tconst Colour *backgroundColourRef;\r\n"
"\tconst Colour *colourRef;\r\n"
"\tconst Colour *textColourPtr;\r\n"
"\tconst Colour *overlayColourRef;\r\n"
"\tbool isMouseOverButton;\r\n"
"\tbool isButtonDown;\r\n"
"\tbool ticked;\r\n"
"\tbool isEnabled;\r\n"
"\tbool isOpen;\r\n"
"\tbool isMouseOver;\r\n"
"\tbool isActive;\r\n"
"\tbool isHighlighted;\r\n"
"\tbool isTicked;\r\n"
"\tbool hasSubMenu;\r\n"
"\tbool isScrollUpArrow;\r\n"
"\tbool isMouseOverBar;\r\n"
"\tbool isMouseOverItem;\r\n"
"\tbool isMenuOpen;\r\n"
"\tbool isIncrement;\r\n"
"\tbool isMouseDown;\r\n"
"\tbool isMouseDragging;\r\n"
"\tbool drawTitleTextOnLeft;\r\n"
"\tbool positionTitleBarButtonsOnLeft;\r\n"
"\tbool isVerticalBar;\r\n"
"\tconst String *keyDescription;\r\n"
"\tconst String *name;\r\n"
"\tconst String *text;\r\n"
"\tconst String *title;\r\n"
"\tconst String *message;\r\n"
"\tconst String *button1;\r\n"
"\tconst String *button2;\r\n"
"\tconst String *button3;\r\n"
"\tconst String *instructions;\r\n"
"\tconst String *filename;\r\n"
"\tconst String *shortcutKeyText;\r\n"
"\tconst String *itemText;\r\n"
"\tconst String *tipText;\r\n"
"\tconst String *columnName;\r\n"
"\tconst Image *icon;\r\n"
"\tImage *image;\r\n"
"\tconst Drawable *drawableIcon;\r\n"
"\tbool isSeparator;\r\n"
"\tint standardMenuItemHeight;\r\n"
"\tint idealWidth;\r\n"
"\tint idealHeight;\r\n"
"\tfloat sliderPosProportional;\r\n"
"\tfloat rotaryStartAngle;\r\n"
"\tfloat rotaryEndAngle;\r\n"
"\tSlider *slider;\r\n"
"\tAlertWindow::AlertIconType  \ticonType;\r\n"
"\tint numButtons;\r\n"
"\tComponent *associatedComponent;\r\n"
"\tAlertWindow *window;\r\n"
"\tconst Rectangle<int> *textArea;\r\n"
"\tTextLayout *layout;\r\n"
"\tProgressBar *progressBar;\r\n"
"\tdouble progress;\r\n"
"\tconst String *textToShow;\r\n"
"\tScrollBar *scrollBar;\r\n"
"\tconst Rectangle<float> *area;\r\n"
"\tRectangle<int> *areaInt;\r\n"
"\tRectangle<int> areaIntConst;\r\n"
"\tTextEditor *textEditor, *editor;\r\n"
"\tComponent *keyFocusOwner;\r\n"
"\tDirectoryContentsDisplayComponent *directoryContentsDisplayComponent;\r\n"
"\tconst String *fileSizeDescription;\r\n"
"\tconst String *fileTimeDescription;\r\n"
"\tbool isDirectory;\r\n"
"\tbool isItemSelected;\r\n"
"\tint itemIndex;\r\n"
"\tFileBrowserComponent *fileBrowserComponent;\r\n"
"\tFilePreviewComponent *filePreviewComponent;\r\n"
"\tComboBox *currentPathBox;\r\n"
"\tTextEditor *filenameBox;\r\n"
"\tButton *goUpButton;\r\n"
"\tBubbleComponent *bubble, *bubbleComponent;\r\n"
"\tconst Point<float> *tip;\r\n"
"\tconst Rectangle<float> *body;\r\n"
"\tMenuBarComponent *menuBarComponent;\r\n"
"\tComboBox *comboBox;\r\n"
"\tLabel *label;\r\n"
"\tSlider::SliderStyle sliderStyle;\r\n"
"\tFilenameComponent *filenameComponent;\r\n"
"\tConcertinaPanel *concertinaPanel;\r\n"
"\tconst BorderSize<int> *borderSize;\r\n"
"\tResizableWindow *resizableWindow;\r\n"
"\tDocumentWindow *documentWindow;\r\n"
"\tButton *minimiseButton;\r\n"
"\tButton *maximiseButton;\r\n"
"\tButton *closeButton;\r\n"
"\tconst Justification *justification;\r\n"
"\tGroupComponent *groupComponent;\r\n"
"\tTabBarButton *tabBarButton;\r\n"
"\tTabbedButtonBar *tabbedButtonBar;\r\n"
"\tPath *p;\r\n"
"\tconst Path *path;\r\n"
"\tTableHeaderComponent *tableHeaderComponent;\r\n"
"\tToolbar *toolbar;\r\n"
"\tToolbarItemComponent *toolbarItemComponent;\r\n"
"\tPropertyComponent *propertyComponent;\r\n"
"\tCallOutBox *callOutBox;";

const char* paramWrapper_template = (const char*) temp_binary_data_130;

//================== RSRC.zip ==================
static const unsigned char temp_binary_data_131[] =
{ 80,75,3,4,20,0,0,0,8,0,202,128,4,69,232,71,224,189,228,0,0,0,217,2,0,0,13,0,28,0,114,101,115,117,108,116,95,49,46,114,115,114,99,85,84,9,0,3,92,147,223,83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,82,97,96,96,4,98,134,
173,12,35,15,176,178,248,90,41,248,1,25,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,82,22,148,88,154,91,234,28,18,228,227,233,23,28,130,106,66,112,72,144,2,243,11,48,249,18,102,40,19,136,228,4,179,153,4,128,68,74,78,78,42,
243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,156,12,195,52,6,38,144,121,12,204,12,82,32,
213,192,196,228,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,6,219,195,126,1,204,174,5,179,47,130,217,179,193,106,128,234,129,148,24,76,13,144,218,9,17,7,27,161,14,17,103,96,5,82,231,64,108,112,172,225,14,31,0,80,75,3,4,20,0,0,0,8,
0,203,128,4,69,117,140,95,228,231,0,0,0,235,2,0,0,14,0,28,0,114,101,115,117,108,116,95,49,48,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,101,96,96,4,98,134,125,12,35,15,240,
241,250,90,41,248,193,1,80,132,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,234,131,18,75,115,75,157,67,130,124,60,253,130,67,80,141,10,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,
48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,8,101,24,166,49,48,129,204,99,96,102,144,2,
169,6,166,42,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,182,16,136,205,126,1,204,110,3,179,47,130,217,75,192,106,128,234,129,148,60,76,13,144,58,4,17,7,27,97,0,17,103,224,3,82,215,65,108,212,232,195,29,80,0,80,75,3,4,20,0,0,0,
8,0,207,128,4,69,206,128,143,162,233,0,0,0,159,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,48,46,114,115,114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,103,96,96,4,97,9,134,145,7,
50,210,125,173,20,252,232,0,128,118,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,37,65,137,165,185,165,206,33,65,62,158,126,193,33,168,142,12,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,
153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,114,100,24,166,49,48,129,204,99,96,102,
144,2,169,6,166,87,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,14,136,205,126,1,204,126,0,102,95,4,179,255,129,213,0,213,3,169,74,152,26,96,130,151,129,136,131,141,232,130,136,51,100,0,197,13,65,108,122,37,12,220,81,0,0,80,
75,3,4,20,0,0,0,8,0,207,128,4,69,184,149,111,39,233,0,0,0,161,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,49,46,114,115,114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,234,96,96,96,4,97,
73,134,145,7,50,51,124,173,20,252,232,1,128,150,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,41,65,137,165,185,165,206,33,65,62,158,126,193,33,168,174,12,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,
82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,118,100,24,166,49,48,129,
204,99,96,102,144,2,169,6,38,88,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,46,136,205,126,1,204,126,8,102,95,4,179,255,131,213,0,213,3,169,42,152,26,96,138,151,133,136,131,141,232,134,136,51,100,2,197,141,64,108,186,165,12,
220,113,0,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,41,97,197,38,232,0,0,0,163,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,50,46,114,115,114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,234,
100,96,96,4,97,41,134,145,7,178,50,125,173,20,252,232,2,128,182,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,45,65,137,165,185,165,206,33,65,62,158,126,193,33,168,206,12,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,
152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,122,100,
24,166,49,48,129,204,99,96,102,144,2,169,6,166,88,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,30,136,205,126,1,204,126,4,102,95,4,177,25,33,106,128,234,129,84,53,76,13,48,201,203,65,196,193,210,61,16,113,134,44,160,184,49,136,
77,191,164,129,59,18,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,224,216,51,137,234,0,0,0,165,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,51,46,114,115,114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,
96,96,96,234,98,96,96,4,97,105,134,145,7,178,179,124,173,20,252,232,3,128,214,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,49,65,137,165,185,165,206,33,65,62,158,126,193,33,168,238,12,14,9,82,96,126,1,38,95,66,133,88,
153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,
129,51,126,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,89,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,62,136,205,126,1,204,126,12,102,95,4,177,25,25,193,106,128,234,129,84,13,76,13,48,205,203,67,196,193,70,244,66,196,
25,178,129,226,38,32,54,29,211,6,238,88,0,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,18,234,160,117,233,0,0,0,167,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,52,46,114,115,114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,
20,0,0,0,99,96,96,100,96,96,96,234,102,96,96,4,97,25,134,145,7,114,178,125,173,20,252,232,4,128,246,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,53,65,137,165,185,165,206,33,65,62,158,126,193,33,168,14,13,14,9,82,96,
126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,
128,112,38,144,205,129,51,130,100,24,166,49,48,129,204,99,96,102,144,2,169,6,166,89,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,1,136,205,126,1,204,126,2,102,95,4,177,25,193,142,0,169,7,82,181,48,53,192,68,175,0,17,7,27,209,
7,17,103,200,1,138,155,130,216,244,76,28,184,163,1,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,124,167,24,12,234,0,0,0,169,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,53,46,114,115,114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,
1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,234,97,96,96,4,97,89,134,145,7,114,115,124,173,20,252,232,5,128,22,114,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,57,65,137,165,185,165,206,33,65,62,158,126,193,33,168,46,13,14,9,
82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,
100,191,128,112,38,144,205,129,51,134,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,90,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,33,136,205,126,1,204,126,10,102,95,4,177,25,153,193,106,128,234,129,84,29,76,13,48,213,43,
66,196,193,70,244,67,196,25,114,129,226,102,32,54,93,83,7,238,120,0,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,218,70,177,193,234,0,0,0,171,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,54,46,114,115,114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,
120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,234,101,96,96,4,97,57,134,145,7,242,114,125,173,20,252,232,6,128,54,114,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,61,65,137,165,185,165,206,33,65,62,158,126,193,
33,168,78,13,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,
192,228,69,124,206,100,191,128,112,38,144,205,129,51,138,100,24,166,49,48,129,204,99,96,102,144,2,169,6,166,90,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,17,136,205,126,1,204,126,6,102,95,4,177,25,89,192,106,128,234,129,84,
61,76,13,48,217,43,65,196,193,70,76,128,136,51,228,1,197,205,65,108,250,38,15,220,17,1,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,110,96,93,186,234,0,0,0,173,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,55,46,114,115,114,99,85,84,9,0,3,101,147,223,83,
101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,234,99,96,96,4,97,121,134,145,7,242,243,124,173,20,252,232,7,128,86,114,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,65,65,137,165,185,165,206,33,
65,62,158,126,193,33,168,110,13,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,
142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,142,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,91,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,49,136,205,126,1,204,126,14,102,95,4,177,25,89,193,
106,128,234,129,84,3,76,13,48,221,43,67,196,193,70,76,132,136,51,228,3,197,45,64,108,58,167,15,220,49,1,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,205,85,200,97,234,0,0,0,175,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,56,46,114,115,114,99,85,84,9,0,
3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,234,103,96,96,4,97,5,134,145,7,10,242,125,173,20,252,232,8,128,118,114,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,69,65,137,165,
185,165,206,33,65,62,158,126,193,33,168,142,13,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,
145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,146,100,24,166,49,48,129,204,99,96,102,144,2,169,6,166,91,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,9,136,205,126,1,204,126,1,102,95,4,
177,25,217,192,106,128,234,129,84,35,76,13,48,225,171,64,196,193,70,76,130,136,51,20,0,197,45,65,108,122,39,16,220,81,1,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,212,59,155,129,234,0,0,0,177,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,48,57,46,114,115,
114,99,85,84,9,0,3,101,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,192,192,192,8,194,138,12,35,15,20,22,248,90,41,248,209,19,0,45,229,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,162,
74,144,147,130,18,75,115,75,157,67,130,124,60,253,130,67,80,93,27,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,203,
9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,44,201,48,76,99,96,2,153,199,192,204,32,5,82,13,76,184,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,236,82,16,155,253,2,152,13,14,
40,246,139,32,54,35,59,88,13,80,61,144,106,130,169,1,166,124,85,136,56,216,136,201,16,113,134,66,160,184,21,136,77,247,20,130,59,46,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,222,162,66,46,231,0,0,0,237,2,0,0,14,0,28,0,114,101,115,117,108,116,95,49,49,46,
114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,99,96,96,4,98,134,253,12,35,15,240,243,249,90,41,248,33,0,80,136,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,
138,42,65,26,130,18,75,115,75,157,67,130,124,60,253,130,67,80,205,10,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,
4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,12,101,24,166,49,48,129,204,99,96,102,144,2,169,6,38,43,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,182,48,136,205,126,1,
204,110,7,179,47,130,217,75,193,106,128,234,129,148,2,76,13,144,58,12,17,7,27,97,8,17,103,224,7,82,55,64,108,180,248,195,29,82,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,109,200,53,89,233,0,0,0,179,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,49,48,46,114,
115,114,99,85,84,9,0,3,102,147,223,83,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,200,192,192,8,194,74,12,35,15,20,21,250,90,41,248,209,21,0,109,229,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,
162,74,144,155,130,18,75,115,75,157,67,130,124,60,253,130,67,80,157,27,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,
203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,52,201,48,76,99,96,2,153,199,192,204,32,5,82,13,76,185,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,236,50,16,155,253,2,152,
253,10,204,190,8,98,51,130,109,0,169,7,82,205,48,53,192,164,175,6,17,7,27,49,5,34,206,80,4,20,183,6,177,233,159,68,112,71,6,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,37,80,205,143,234,0,0,0,181,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,49,49,46,114,115,
114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,196,192,192,8,194,202,12,35,15,20,23,249,90,41,248,209,23,0,173,229,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,
162,74,144,163,130,18,75,115,75,157,67,130,124,60,253,130,67,80,221,27,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,
203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,60,201,48,76,99,96,2,153,199,192,204,32,5,82,13,76,186,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,144,163,25,216,47,128,
217,175,193,236,139,32,54,35,216,126,144,122,32,213,2,83,3,76,251,234,16,113,176,17,83,33,226,12,197,64,113,27,16,123,0,210,8,238,216,0,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,140,159,95,150,234,0,0,0,183,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,49,
50,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,204,192,192,8,194,42,12,35,15,148,20,251,90,41,248,209,25,0,237,229,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,
215,188,146,162,74,144,171,130,18,75,115,75,157,67,130,124,60,253,130,67,80,29,28,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,
21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,68,201,48,76,99,96,2,153,199,192,204,32,5,82,13,76,187,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,236,10,16,155,
253,2,152,253,6,204,190,8,98,51,114,129,213,0,213,3,169,86,152,26,96,226,215,128,136,131,141,152,6,17,103,40,1,138,219,130,216,3,145,72,112,71,7,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,87,82,62,74,234,0,0,0,185,3,0,0,15,0,28,0,114,101,115,117,108,116,95,
49,49,51,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,194,192,192,8,194,170,12,35,15,148,150,248,90,41,248,209,27,0,45,230,228,112,46,41,202,41,210,117,12,5,114,120,193,108,
199,80,215,188,146,162,74,144,179,130,18,75,115,75,157,67,130,124,60,253,130,67,80,93,28,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,
48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,76,201,48,76,99,96,2,153,199,192,204,32,5,82,13,76,188,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,
176,3,217,47,128,217,111,193,236,139,32,54,35,55,88,13,80,61,144,106,131,169,1,166,126,77,136,56,216,136,233,16,113,134,82,160,184,29,136,61,32,169,4,119,124,0,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,157,131,185,211,234,0,0,0,187,3,0,0,15,0,28,0,114,101,
115,117,108,116,95,49,49,52,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,202,192,192,8,194,106,12,35,15,148,149,250,90,41,248,209,29,0,109,230,228,112,46,41,202,41,210,117,
12,5,114,120,193,108,199,80,215,188,146,162,74,144,187,130,18,75,115,75,157,67,130,124,60,253,130,67,80,157,28,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,
137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,84,201,48,76,99,96,2,153,199,192,204,32,5,82,13,76,189,94,37,25,121,233,64,58,137,249,197,255,255,10,80,
133,204,47,193,236,42,16,155,253,2,152,253,14,204,190,8,98,51,242,128,213,0,213,3,169,118,152,26,96,242,215,130,136,131,141,152,1,17,103,40,3,138,219,131,216,3,147,76,112,71,8,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,60,236,242,144,234,0,0,0,189,3,0,0,15,
0,28,0,114,101,115,117,108,116,95,49,49,53,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,198,192,192,8,194,234,12,35,15,148,151,249,90,41,248,209,31,0,173,230,228,112,46,41,
202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,162,74,144,195,130,18,75,115,75,157,67,130,124,60,253,130,67,80,221,28,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,
197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,92,201,48,76,99,96,2,153,199,192,204,32,5,82,13,76,190,94,37,25,121,233,64,58,137,249,197,
255,255,10,80,133,204,47,193,236,106,16,155,253,2,152,253,30,204,190,8,98,51,242,130,213,0,213,3,169,14,152,26,96,250,215,134,136,131,141,152,9,17,103,0,122,156,209,1,196,30,160,116,130,59,70,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,123,139,94,31,234,0,
0,0,191,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,49,54,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,206,192,192,8,194,26,12,35,15,84,148,251,90,41,248,13,0,0,218,205,
201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,151,5,37,150,230,150,58,135,4,249,120,250,5,135,160,58,58,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,0,50,
165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,200,146,97,152,198,192,4,50,143,129,153,65,10,164,26,152,126,189,74,50,242,
210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,53,32,54,251,5,48,251,3,152,125,17,196,102,228,3,171,1,170,7,82,157,48,53,192,12,160,3,17,7,27,49,11,34,206,80,1,20,119,4,177,7,42,161,224,142,18,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,250,175,209,
49,235,0,0,0,193,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,49,55,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,193,192,192,8,194,154,12,35,15,84,86,248,90,41,248,13,4,0,
90,206,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,167,5,37,150,230,150,58,135,4,249,120,250,5,135,160,186,58,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,
0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,216,146,97,152,198,192,4,50,143,129,153,65,10,164,26,152,128,189,74,
50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,181,32,54,251,5,48,251,35,152,125,17,196,102,228,7,171,1,170,7,82,93,48,53,192,28,160,11,17,7,27,49,27,34,206,0,244,36,163,19,136,61,96,41,5,119,156,0,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,
115,35,90,147,234,0,0,0,195,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,49,56,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,201,192,192,8,194,90,12,35,15,84,85,250,90,41,
248,13,8,0,218,206,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,183,5,37,150,230,150,58,135,4,249,120,250,5,135,160,58,59,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,
134,196,230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,232,146,97,152,198,192,4,50,143,129,153,65,10,164,26,152,
130,189,74,50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,117,32,54,251,5,48,251,19,152,125,17,196,102,20,0,171,1,170,7,82,221,48,53,192,44,160,7,17,7,27,49,7,34,206,80,5,20,119,6,177,7,46,169,224,142,20,0,80,75,3,4,20,0,0,0,8,0,207,128,
4,69,62,13,39,110,234,0,0,0,197,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,49,57,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,197,192,192,8,194,218,12,35,15,84,87,249,90,
41,248,13,12,0,90,207,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,199,5,37,150,230,150,58,135,4,249,120,250,5,135,160,186,59,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,
192,134,196,230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,248,146,97,152,198,192,4,50,143,129,153,65,10,164,26,
152,132,189,74,50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,245,32,54,251,5,48,251,51,152,125,17,196,102,20,4,171,1,170,7,82,61,48,53,192,60,160,15,17,7,27,49,23,34,206,80,13,20,119,1,177,7,48,173,224,142,21,0,80,75,3,4,20,0,0,0,8,0,
203,128,4,69,218,109,58,25,231,0,0,0,239,2,0,0,14,0,28,0,114,101,115,117,108,116,95,49,50,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,103,96,96,4,98,134,3,12,35,15,8,240,251,
90,41,248,33,1,160,24,39,135,115,73,81,78,145,174,99,40,144,195,11,102,59,134,186,230,149,20,85,130,116,4,37,150,230,150,58,135,4,249,120,250,5,135,160,26,22,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,
96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,32,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,87,
94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,17,16,155,253,2,152,221,1,102,95,4,179,151,129,213,0,213,3,41,69,152,26,32,117,4,34,14,54,194,8,34,206,0,114,216,77,16,27,61,2,113,7,21,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,80,233,53,
113,234,0,0,0,199,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,48,46,114,115,114,99,85,84,9,0,3,102,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,205,192,192,8,194,58,12,35,15,212,84,251,90,41,248,13,16,
0,218,207,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,215,5,37,150,230,150,58,135,4,249,120,250,5,135,160,58,60,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,
230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,8,147,97,152,198,192,4,50,143,129,153,65,10,164,26,152,134,189,74,
50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,13,32,54,251,5,48,251,11,152,125,17,196,102,20,2,171,1,170,7,82,189,48,53,192,76,96,0,17,7,27,49,15,34,206,80,3,20,119,5,177,7,50,177,224,142,22,0,80,75,3,4,20,0,0,0,8,0,208,128,4,69,59,105,
19,97,234,0,0,0,201,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,49,46,114,115,114,99,85,84,9,0,3,103,147,223,83,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,195,192,192,8,194,186,12,35,15,212,214,248,90,41,248,13,
20,0,58,128,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,206,11,74,44,205,45,117,14,9,242,241,244,11,14,65,117,121,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,137,
205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,49,38,195,48,141,129,9,100,30,3,51,131,20,72,53,48,17,123,149,100,
228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,27,65,108,246,11,96,246,87,48,251,34,136,205,40,12,86,3,84,15,164,250,96,106,128,185,192,16,34,14,54,98,62,68,156,161,22,40,238,6,98,15,104,106,193,29,47,0,80,75,3,4,20,0,0,0,8,0,208,128,4,69,195,
163,20,47,234,0,0,0,203,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,50,46,114,115,114,99,85,84,9,0,3,103,147,223,83,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,203,192,192,8,194,122,12,35,15,212,213,250,90,41,248,
13,24,0,186,128,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,238,11,74,44,205,45,117,14,9,242,241,244,11,14,65,117,122,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,
137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,81,38,195,48,141,129,9,100,30,3,51,131,20,72,53,48,21,123,149,
100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,155,64,108,246,11,96,246,55,48,251,34,136,205,40,2,86,3,84,15,164,250,97,106,128,217,192,8,34,14,54,98,1,68,156,161,14,40,238,14,98,15,108,114,193,29,49,0,80,75,3,4,20,0,0,0,8,0,208,128,4,69,106,
168,210,101,234,0,0,0,205,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,51,46,114,115,114,99,85,84,9,0,3,103,147,223,83,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,199,192,192,8,194,250,12,35,15,212,215,249,90,41,
248,13,28,0,58,129,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,14,12,74,44,205,45,117,14,9,242,241,244,11,14,65,117,123,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,
137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,113,38,195,48,141,129,9,100,30,3,51,131,20,72,53,48,25,123,
149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,155,65,108,246,11,96,246,119,48,251,34,136,205,40,10,86,3,84,15,164,38,192,212,0,243,129,49,68,28,108,196,66,136,56,67,61,80,220,3,196,30,224,244,130,59,102,0,80,75,3,4,20,0,0,0,8,0,208,128,
4,69,33,174,57,145,234,0,0,0,207,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,52,46,114,115,114,99,85,84,9,0,3,103,147,223,83,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,154,207,192,192,8,194,6,12,35,15,52,212,251,90,
41,248,13,32,0,186,129,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,46,12,74,44,205,45,117,14,9,242,241,244,11,14,65,117,124,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,
13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,145,38,195,48,141,129,9,100,30,3,51,131,20,72,53,48,29,123,
149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,91,64,108,246,11,96,246,15,48,251,34,136,205,40,6,86,3,84,15,164,38,194,212,0,51,130,9,68,28,108,196,34,136,56,67,3,80,220,19,196,30,232,4,131,59,106,0,80,75,3,4,20,0,0,0,8,0,208,128,4,69,
59,134,124,168,234,0,0,0,209,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,53,46,114,115,114,99,85,84,9,0,3,103,147,223,83,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,90,192,192,192,8,194,134,12,35,15,52,54,248,90,41,
248,13,36,0,58,130,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,78,12,74,44,205,45,117,14,9,242,241,244,11,14,65,117,125,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,
137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,177,38,195,48,141,129,9,100,30,3,51,131,20,72,53,48,33,123,
149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,91,65,108,246,11,96,246,79,48,251,34,136,205,40,14,86,3,84,15,164,38,193,212,0,115,130,41,68,28,108,196,98,136,56,67,35,80,220,11,196,30,240,20,131,59,110,0,80,75,3,4,20,0,0,0,8,0,208,128,
4,69,97,182,64,157,234,0,0,0,211,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,54,46,114,115,114,99,85,84,9,0,3,103,147,223,83,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,90,200,192,192,8,194,70,12,35,15,52,53,250,90,
41,248,13,40,0,186,130,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,110,12,74,44,205,45,117,14,9,242,241,244,11,14,65,117,126,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,
129,13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,209,38,195,48,141,129,9,100,30,3,51,131,20,72,53,48,
37,123,149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,219,64,108,246,11,96,246,47,48,251,34,136,205,40,1,86,3,84,15,164,38,195,212,0,179,130,25,68,28,108,196,18,136,56,67,19,80,220,27,196,30,248,36,131,59,114,0,80,75,3,4,20,0,0,0,8,0,208,
128,4,69,159,57,50,10,235,0,0,0,213,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,55,46,114,115,114,99,85,84,9,0,3,103,147,223,83,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,90,196,192,192,8,194,198,12,35,15,52,55,249,
90,41,248,13,44,0,58,131,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,142,12,74,44,205,45,117,14,9,242,241,244,11,14,65,117,127,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,
129,13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,241,38,195,48,141,129,9,100,30,3,51,131,20,72,53,48,
41,123,149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,219,65,108,246,11,96,246,111,48,251,34,136,205,40,9,86,3,84,15,164,166,192,212,0,243,130,57,68,28,108,196,82,136,56,67,51,80,220,7,196,30,4,105,6,119,236,0,0,80,75,3,4,20,0,0,0,8,0,
208,128,4,69,68,6,226,207,235,0,0,0,215,3,0,0,15,0,28,0,114,101,115,117,108,116,95,49,50,56,46,114,115,114,99,85,84,9,0,3,103,147,223,83,116,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,90,204,192,192,8,194,38,12,35,15,180,52,
251,90,41,248,13,48,0,186,131,147,195,185,164,40,167,72,215,49,20,200,225,5,179,29,67,93,243,74,138,42,65,174,12,74,44,205,45,117,14,9,242,241,244,11,14,65,245,64,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,
130,129,13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,17,39,195,48,141,129,9,100,30,3,51,131,20,72,53,
48,45,123,149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,59,64,108,246,11,96,246,31,48,251,34,136,205,40,5,86,3,84,15,164,166,194,212,0,51,131,5,68,28,108,196,50,136,56,67,11,80,220,23,196,30,12,137,6,119,244,0,0,80,75,3,4,20,0,0,0,8,0,
203,128,4,69,106,156,219,100,231,0,0,0,241,2,0,0,14,0,28,0,114,101,115,117,108,116,95,49,51,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,96,96,96,4,98,134,131,12,35,15,8,10,248,
90,41,248,33,3,160,32,39,135,115,73,81,78,145,174,99,40,144,195,11,102,59,134,186,230,149,20,85,130,180,4,37,150,230,150,58,135,4,249,120,250,5,135,160,154,22,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,
96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,40,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,88,
94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,81,16,155,253,2,152,221,9,102,95,4,179,151,131,213,0,213,3,41,37,152,26,32,117,20,34,14,54,194,24,34,206,32,8,164,110,129,216,24,49,136,59,172,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,76,
165,169,7,231,0,0,0,243,2,0,0,14,0,28,0,114,101,115,117,108,116,95,49,52,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,100,96,96,4,98,134,67,12,35,15,8,9,250,90,41,248,161,0,160,
40,39,135,115,73,81,78,145,174,99,40,144,195,11,102,59,134,186,230,149,20,85,130,244,4,37,150,230,150,58,135,4,249,120,250,5,135,160,26,23,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,
82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,48,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,89,94,37,25,121,233,64,
58,137,249,197,255,255,10,80,133,204,47,193,108,49,16,155,253,2,152,221,5,102,95,4,179,87,128,213,0,213,3,41,101,152,26,32,117,12,34,14,54,194,4,34,206,32,4,164,110,131,216,152,81,136,59,176,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,12,23,213,244,232,0,0,
0,245,2,0,0,14,0,28,0,114,101,115,117,108,116,95,49,53,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,98,96,96,4,98,134,195,12,35,15,8,11,249,90,41,248,161,2,160,48,39,135,115,
73,81,78,145,174,99,40,144,195,11,102,59,134,186,230,149,20,85,130,52,5,37,150,230,150,58,135,4,249,120,250,5,135,160,154,23,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,
197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,56,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,90,94,37,25,121,233,64,58,137,249,197,
255,255,10,80,133,204,47,193,108,113,16,155,253,2,152,221,13,102,95,4,179,87,130,213,0,213,3,41,21,152,26,32,117,28,34,14,54,194,20,34,206,32,12,164,238,128,216,88,226,16,119,104,1,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,233,45,180,135,232,0,0,0,247,2,
0,0,14,0,28,0,114,101,115,117,108,116,95,49,54,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,102,96,96,4,98,134,35,12,35,15,136,8,251,90,41,248,161,1,160,56,39,135,115,73,81,78,
145,174,99,40,144,195,11,102,59,134,186,230,149,20,85,130,116,5,37,150,230,150,58,135,4,249,120,250,5,135,160,26,24,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,
233,137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,64,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,91,94,37,25,121,233,64,58,137,249,197,255,255,10,
80,133,204,47,193,108,9,16,155,253,2,152,221,3,102,95,4,179,87,129,213,0,213,3,41,85,152,26,32,117,2,34,14,54,194,12,34,206,32,2,164,238,130,216,216,34,17,119,112,1,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,220,115,75,74,232,0,0,0,249,2,0,0,14,0,28,0,114,
101,115,117,108,116,95,49,55,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,97,96,96,4,98,134,163,12,35,15,136,138,248,90,41,248,161,3,160,4,39,135,115,73,81,78,145,174,99,40,144,
195,11,102,59,134,186,230,149,20,85,130,180,5,37,150,230,150,58,135,4,249,120,250,5,135,160,154,24,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,
69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,72,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,92,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,
193,108,73,16,155,253,2,152,221,11,102,95,4,179,87,131,213,0,213,3,41,53,152,26,32,117,18,34,14,54,194,28,34,206,32,10,164,238,129,216,88,99,17,119,120,1,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,221,225,147,159,232,0,0,0,251,2,0,0,14,0,28,0,114,101,115,
117,108,116,95,49,56,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,101,96,96,4,98,134,99,12,35,15,136,137,250,90,41,248,97,0,160,12,39,135,115,73,81,78,145,174,99,40,144,195,11,
102,59,134,186,230,149,20,85,130,244,5,37,150,230,150,58,135,4,249,120,250,5,135,160,26,25,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,
48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,80,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,93,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,41,
16,155,253,2,152,221,7,102,95,4,179,215,128,213,0,213,3,41,117,152,26,32,117,10,34,14,54,194,2,34,206,32,6,164,238,131,216,216,163,17,119,128,1,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,179,25,152,22,232,0,0,0,253,2,0,0,14,0,28,0,114,101,115,117,108,116,
95,49,57,46,114,115,114,99,85,84,9,0,3,93,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,99,96,96,4,98,134,227,12,35,15,136,139,249,90,41,248,97,2,160,20,39,135,115,73,81,78,145,174,99,40,144,195,11,102,59,134,
186,230,149,20,85,130,52,6,37,150,230,150,58,135,4,249,120,250,5,135,160,154,25,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,
194,50,83,203,9,202,10,66,157,8,226,34,59,179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,88,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,94,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,105,16,155,
253,2,152,221,15,102,95,4,179,215,130,213,0,213,3,41,13,152,26,32,117,26,34,14,54,194,18,34,206,32,14,164,30,128,216,56,226,17,119,136,1,0,80,75,3,4,20,0,0,0,8,0,202,128,4,69,207,112,51,4,231,0,0,0,219,2,0,0,13,0,28,0,114,101,115,117,108,116,95,50,46,
114,115,114,99,85,84,9,0,3,92,147,223,83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,82,101,96,96,4,98,134,109,12,35,15,176,177,250,90,41,248,249,1,89,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,
84,9,82,23,148,88,154,91,234,28,18,228,227,233,23,28,130,106,68,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,
39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,65,39,195,48,141,129,9,100,30,3,51,131,20,72,53,48,53,121,149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,185,64,108,246,11,96,118,
29,152,125,17,204,158,3,86,3,84,15,164,196,97,106,128,212,46,136,56,216,8,13,136,56,3,27,144,58,15,118,58,56,218,112,7,16,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,118,247,124,99,232,0,0,0,255,2,0,0,14,0,28,0,114,101,115,117,108,116,95,50,48,46,114,115,114,
99,85,84,9,0,3,94,147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,50,103,96,96,4,98,134,19,12,35,15,72,136,251,90,41,248,97,1,64,57,78,14,231,146,162,156,34,93,199,80,32,135,23,204,118,12,117,205,43,41,170,4,233,12,
74,44,205,45,117,14,9,242,241,244,11,14,65,53,52,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,
58,17,196,69,118,102,89,57,110,103,178,95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,192,148,97,152,198,192,4,50,143,129,153,65,10,164,26,152,190,188,74,50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,50,32,54,251,5,48,123,2,152,125,
17,204,94,7,86,3,84,15,164,52,97,106,128,212,25,136,56,216,8,43,136,56,131,4,144,122,8,98,227,138,72,220,65,6,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,43,89,3,94,232,0,0,0,1,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,49,46,114,115,114,99,85,84,9,0,3,
94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,178,96,96,96,4,98,134,147,12,35,15,72,74,248,90,41,248,97,3,64,73,78,14,231,146,162,156,34,93,199,80,32,135,23,204,118,12,117,205,43,41,170,4,105,13,74,44,205,45,
117,14,9,242,241,244,11,14,65,53,53,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,
118,102,89,57,110,103,178,95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,208,148,97,152,198,192,4,50,143,129,153,65,10,164,26,152,192,188,74,50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,178,32,54,251,5,48,123,34,152,125,17,204,94,
15,86,3,84,15,164,180,96,106,128,212,89,136,56,216,8,107,136,56,131,36,144,122,4,98,227,140,73,220,97,6,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,16,2,104,93,230,0,0,0,3,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,50,46,114,115,114,99,85,84,9,0,3,94,147,
223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,178,100,96,96,4,98,134,83,12,35,15,72,73,250,90,41,248,97,5,64,89,78,14,231,146,162,156,34,93,199,80,32,135,23,204,118,12,117,205,43,41,170,4,233,13,74,44,205,45,117,14,
9,242,241,244,11,14,65,53,54,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,0,187,200,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,92,100,103,150,149,227,
118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,78,25,134,105,12,76,32,243,24,152,25,164,64,170,129,41,204,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,45,7,98,179,95,0,179,39,129,217,23,193,236,13,96,53,64,245,64,74,27,166,
6,72,157,131,136,131,141,176,129,136,51,72,1,169,199,32,54,238,168,196,29,104,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,228,245,119,102,232,0,0,0,5,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,51,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,
117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,178,98,96,96,4,98,134,211,12,35,15,72,75,249,90,41,248,97,7,64,105,78,14,231,146,162,156,34,93,199,80,32,135,23,204,118,12,117,205,43,41,170,4,105,14,74,44,205,45,117,14,9,242,241,244,11,14,
65,53,55,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,95,
0,147,23,241,57,147,253,2,194,153,64,54,7,206,240,148,97,152,198,192,4,50,143,129,153,65,10,164,26,152,196,188,74,50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,242,32,54,251,5,48,123,50,152,125,17,204,222,8,86,3,84,15,164,116,96,106,
128,212,121,136,56,216,8,91,136,56,131,52,144,122,2,98,227,137,75,220,161,6,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,169,150,8,204,230,0,0,0,7,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,52,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,
120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,178,102,96,96,4,98,134,51,12,35,15,200,72,251,90,41,248,225,0,64,121,78,14,231,146,162,156,34,93,199,80,32,135,23,204,118,12,117,205,43,41,170,4,233,14,74,44,205,45,117,14,9,242,241,244,11,14,65,
53,56,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,95,0,
147,23,241,57,147,253,2,194,153,64,54,7,238,0,101,152,198,192,4,50,143,129,153,65,10,164,26,152,198,188,74,50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,96,46,251,5,48,123,10,152,125,17,204,222,4,86,3,84,15,164,116,97,106,128,212,5,136,
56,216,8,59,136,56,208,78,6,134,167,32,54,190,200,196,29,108,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,120,2,2,181,232,0,0,0,9,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,53,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,
1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,178,97,96,96,4,98,134,179,12,35,15,200,202,248,90,41,248,225,2,64,5,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,210,30,148,88,154,91,234,28,18,228,227,233,23,28,130,106,114,112,72,144,
2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,
251,5,132,51,129,108,14,156,33,42,195,48,141,129,9,100,30,3,51,131,20,72,53,48,145,121,149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,21,65,108,246,11,96,246,84,48,251,34,152,189,25,172,6,168,30,72,233,193,212,0,169,139,16,113,176,17,246,
16,113,6,89,32,245,12,108,57,190,216,196,29,110,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,162,111,105,169,232,0,0,0,11,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,54,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,
20,0,0,0,99,96,96,100,96,96,96,178,101,96,96,4,98,134,115,12,35,15,200,201,250,90,41,248,225,4,64,21,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,210,31,148,88,154,91,234,28,18,228,227,233,23,28,130,106,116,112,72,144,2,243,
11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,
5,132,51,129,108,14,156,65,42,195,48,141,129,9,100,30,3,51,131,20,72,53,48,149,121,149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,149,64,108,246,11,96,246,52,48,251,34,152,189,5,172,6,168,30,72,233,195,212,0,169,75,16,113,176,17,14,16,
113,6,57,32,245,28,196,198,31,157,184,3,14,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,230,201,105,190,231,0,0,0,13,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,55,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,
0,0,99,96,96,100,96,96,96,178,99,96,96,4,98,134,243,12,35,15,200,203,249,90,41,248,225,6,64,37,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,50,32,40,177,52,183,212,57,36,200,199,211,47,56,4,213,236,224,144,32,5,230,23,96,
242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,
28,56,195,84,134,97,26,3,19,200,60,6,102,6,41,144,106,96,50,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,43,131,216,236,23,192,236,233,96,246,69,48,123,43,88,13,80,61,144,50,128,169,1,82,151,33,226,96,35,28,33,226,12,242,64,10,44,
66,32,62,113,135,28,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,94,136,134,120,231,0,0,0,15,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,56,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,
96,178,103,96,96,4,98,134,11,12,35,15,40,200,251,90,41,248,225,1,64,53,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,50,33,40,177,52,183,212,57,36,200,199,211,47,56,4,213,240,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,
130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,3,85,134,97,26,3,19,
200,60,6,102,6,41,144,106,96,58,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,171,128,216,236,23,192,236,25,96,246,69,48,123,27,88,13,80,61,144,50,132,169,1,82,87,32,226,96,35,156,32,226,12,32,35,193,129,76,40,66,113,7,29,0,80,75,
3,4,20,0,0,0,8,0,203,128,4,69,189,127,66,125,232,0,0,0,17,3,0,0,14,0,28,0,114,101,115,117,108,116,95,50,57,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,96,96,96,4,98,134,139,
12,35,15,40,42,248,90,41,248,225,3,64,69,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,50,34,40,177,52,183,212,57,36,200,199,211,47,56,4,213,244,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,
149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,67,85,134,97,26,3,19,200,60,6,102,6,41,144,106,
96,66,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,171,130,216,236,23,192,236,153,96,246,69,48,123,59,88,13,80,61,144,50,130,169,1,82,87,33,226,96,35,156,33,226,12,138,64,234,21,136,77,48,70,113,135,29,0,80,75,3,4,20,0,0,0,8,0,202,
128,4,69,23,0,64,246,231,0,0,0,221,2,0,0,13,0,28,0,114,101,115,117,108,116,95,51,46,114,115,114,99,85,84,9,0,3,92,147,223,83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,82,99,96,96,4,98,134,237,12,35,15,176,179,249,90,41,
248,249,249,1,153,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,82,24,148,88,154,91,234,28,18,228,227,233,23,28,130,106,70,112,72,144,2,243,11,48,249,18,42,196,202,4,34,57,193,108,38,1,32,145,146,147,147,202,252,130,129,13,
137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,97,39,195,48,141,129,9,100,30,3,51,131,20,72,53,48,57,121,149,
100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,185,193,1,126,1,204,174,7,179,47,130,217,115,193,106,128,234,129,148,4,76,13,144,218,13,17,7,27,161,9,17,103,96,7,82,23,64,108,104,188,225,14,33,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,102,251,10,
93,232,0,0,0,19,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,48,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,100,96,96,4,98,134,75,12,35,15,40,41,250,90,41,248,225,5,64,85,
156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,50,35,40,177,52,183,212,57,36,200,199,211,47,56,4,213,248,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,
204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,131,85,134,97,26,3,19,200,60,6,102,6,41,144,106,96,74,243,42,201,200,75,7,210,73,204,47,
254,255,87,128,42,100,126,9,102,171,129,216,236,23,192,236,89,96,246,69,48,123,7,88,13,80,61,144,50,134,169,1,82,215,32,226,96,35,92,32,226,12,74,64,234,53,136,77,56,74,113,7,30,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,76,179,122,77,232,0,0,0,21,3,0,0,14,
0,28,0,114,101,115,117,108,116,95,51,49,46,114,115,114,99,85,84,9,0,3,94,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,98,96,96,4,98,134,203,12,35,15,40,43,249,90,41,248,225,7,64,101,156,28,206,37,69,57,69,186,
142,161,64,14,47,152,237,24,234,154,87,82,84,9,50,36,40,177,52,183,212,57,36,200,199,211,47,56,4,213,252,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,
47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,195,85,134,97,26,3,19,200,60,6,102,6,41,144,106,96,82,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,
9,102,171,131,216,236,23,192,236,217,96,246,69,48,123,39,88,13,80,61,144,50,129,169,1,82,215,33,226,96,35,92,33,226,12,202,64,234,13,136,77,68,156,226,14,61,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,8,221,198,230,232,0,0,0,23,3,0,0,14,0,28,0,114,101,115,
117,108,116,95,51,50,46,114,115,114,99,85,84,9,0,3,95,147,223,83,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,102,96,96,4,98,134,43,12,35,15,168,40,251,90,41,248,17,0,64,117,156,28,206,37,69,57,69,186,142,161,64,14,47,
152,237,24,234,154,87,82,84,9,50,37,40,177,52,183,212,57,36,200,199,211,47,56,4,213,130,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,
175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,3,86,134,97,26,3,19,200,60,6,102,6,41,144,106,96,90,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,107,128,216,236,
23,192,236,57,96,246,69,48,123,23,88,13,80,61,144,50,133,169,1,82,55,32,226,96,35,220,32,226,12,42,64,234,45,136,77,76,164,226,14,62,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,57,227,223,176,232,0,0,0,25,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,51,46,
114,115,114,99,85,84,9,0,3,95,147,223,83,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,97,96,96,4,98,134,171,12,35,15,168,170,248,90,41,248,17,2,64,133,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,
82,84,9,50,38,40,177,52,183,212,57,36,200,199,211,47,56,4,213,134,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,
80,86,16,234,68,16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,67,86,134,97,26,3,19,200,60,6,102,6,41,144,106,96,98,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,107,130,216,236,23,192,236,185,96,
246,69,48,123,55,88,13,80,61,144,50,131,169,1,82,55,33,226,96,35,220,33,226,12,170,64,234,29,136,77,84,172,226,14,63,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,158,117,209,166,232,0,0,0,27,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,52,46,114,115,114,99,
85,84,9,0,3,95,147,223,83,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,101,96,96,4,98,134,107,12,35,15,168,169,250,90,41,248,17,4,64,149,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,50,39,
40,177,52,183,212,57,36,200,199,211,47,56,4,213,138,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,
16,23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,131,86,134,97,26,3,19,200,60,6,102,6,41,144,106,96,106,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,107,129,216,236,23,192,236,121,96,246,69,48,123,
15,88,13,80,61,144,50,135,169,1,82,183,32,226,96,35,60,32,226,12,106,64,234,61,136,77,92,180,226,14,64,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,181,254,81,233,232,0,0,0,29,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,53,46,114,115,114,99,85,84,9,0,3,95,
147,223,83,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,99,96,96,4,98,134,235,12,35,15,168,171,249,90,41,248,17,6,64,165,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,87,82,84,9,50,40,40,177,52,183,212,
57,36,200,199,211,47,56,4,213,142,224,144,32,5,230,23,96,242,37,84,136,149,9,68,114,130,217,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,101,
229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,195,86,134,97,26,3,19,200,60,6,102,6,41,144,106,96,114,243,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,107,131,216,236,23,192,236,249,96,246,69,48,123,47,88,13,80,61,144,
178,128,169,1,82,183,33,226,96,35,60,33,226,12,234,64,234,3,136,77,100,188,226,14,65,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,99,228,89,2,232,0,0,0,31,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,54,46,114,115,114,99,85,84,9,0,3,95,147,223,83,95,147,223,
83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,114,103,96,96,4,98,134,27,12,35,15,104,168,251,90,41,248,17,1,128,106,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,82,80,98,105,110,169,115,72,144,143,
167,95,112,8,170,37,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,113,59,
147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,174,12,195,52,6,38,144,121,12,204,12,82,32,213,192,244,230,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,214,1,177,217,47,128,217,11,192,236,139,96,246,62,176,26,160,122,32,
101,9,83,3,164,238,64,196,193,70,120,65,196,25,52,128,212,71,16,155,216,136,197,29,132,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,106,77,1,227,232,0,0,0,33,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,55,46,114,115,114,99,85,84,9,0,3,95,147,223,83,95,147,
223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,96,96,96,4,98,134,155,12,35,15,104,106,248,90,41,248,17,3,128,138,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,84,80,98,105,110,169,115,72,144,
143,167,95,112,8,170,45,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,113,
59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,174,12,195,52,6,38,144,121,12,204,12,82,32,213,192,4,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,214,5,177,217,47,128,217,11,193,236,139,96,246,126,176,26,160,122,
32,101,5,83,3,164,238,66,196,193,70,120,67,196,25,52,129,212,39,16,155,232,152,197,29,134,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,228,98,44,130,232,0,0,0,35,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,56,46,114,115,114,99,85,84,9,0,3,95,147,223,83,95,
147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,100,96,96,4,98,134,91,12,35,15,104,105,250,90,41,248,17,5,128,170,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,86,80,98,105,110,169,115,72,
144,143,167,95,112,8,170,53,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,
113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,175,12,195,52,6,38,144,121,12,204,12,82,32,213,192,20,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,214,3,177,217,47,128,217,139,192,236,139,96,246,1,176,26,160,
122,32,101,13,83,3,164,238,65,196,193,70,248,64,196,25,180,128,212,103,16,155,248,168,197,29,136,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,105,149,187,88,232,0,0,0,37,3,0,0,14,0,28,0,114,101,115,117,108,116,95,51,57,46,114,115,114,99,85,84,9,0,3,95,147,223,
83,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,98,96,96,4,98,134,219,12,35,15,104,107,249,90,41,248,17,7,128,202,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,88,80,98,105,110,169,115,
72,144,143,167,95,112,8,170,61,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,
202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,175,12,195,52,6,38,144,121,12,204,12,82,32,213,192,36,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,214,7,177,217,47,128,217,139,193,236,139,96,246,65,176,
26,160,122,32,101,3,83,3,164,238,67,196,193,70,248,66,196,25,180,129,212,23,16,155,132,184,197,29,138,0,80,75,3,4,20,0,0,0,8,0,202,128,4,69,202,7,27,88,229,0,0,0,223,2,0,0,13,0,28,0,114,101,115,117,108,116,95,52,46,114,115,114,99,85,84,9,0,3,92,147,223,
83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,82,103,96,96,4,98,134,29,12,35,15,112,176,251,90,41,248,1,1,144,205,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,149,65,137,165,185,165,206,
33,65,62,158,126,193,33,168,134,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,
206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,120,50,12,211,24,152,64,230,49,48,51,72,129,84,3,211,147,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,155,7,196,102,191,0,102,55,128,217,23,193,236,121,96,53,64,
245,64,74,18,166,6,72,237,129,136,131,141,208,130,136,131,93,3,118,61,44,226,112,7,17,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,54,67,159,23,232,0,0,0,39,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,48,46,114,115,114,99,85,84,9,0,3,95,147,223,83,95,147,
223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,102,96,96,4,98,134,59,12,35,15,232,104,251,90,41,248,17,9,128,234,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,90,80,98,105,110,169,115,72,144,
143,167,95,112,8,170,69,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,113,
59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,176,12,195,52,6,38,144,121,12,204,12,82,32,213,192,52,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,0,177,217,47,128,217,75,192,236,139,96,246,33,176,26,160,122,32,
101,11,83,3,164,30,64,196,193,70,248,65,196,25,116,128,212,87,16,155,148,200,197,29,140,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,29,63,119,121,232,0,0,0,41,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,49,46,114,115,114,99,85,84,9,0,3,95,147,223,83,95,147,
223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,97,96,96,4,98,134,187,12,35,15,232,234,248,90,41,248,17,11,128,26,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,92,80,98,105,110,169,115,72,144,
143,167,95,112,8,170,77,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,113,
59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,176,12,195,52,6,38,144,121,12,204,12,82,32,213,192,68,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,4,177,217,47,128,217,75,193,236,139,96,246,97,176,26,160,122,
32,101,7,83,3,164,30,66,196,193,70,248,67,196,25,116,129,212,55,16,155,164,216,197,29,142,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,184,19,87,108,232,0,0,0,43,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,50,46,114,115,114,99,85,84,9,0,3,95,147,223,83,95,
147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,101,96,96,4,98,134,123,12,35,15,232,233,250,90,41,248,17,13,128,58,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,94,80,98,105,110,169,115,72,
144,143,167,95,112,8,170,85,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,
113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,177,12,195,52,6,38,144,121,12,204,12,82,32,213,192,84,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,2,177,217,47,128,217,203,192,236,139,96,246,17,176,26,160,
122,32,101,15,83,3,164,30,65,196,193,70,4,64,196,25,244,128,212,119,16,155,180,232,197,29,144,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,77,157,228,225,232,0,0,0,45,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,51,46,114,115,114,99,85,84,9,0,3,96,147,223,
83,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,99,96,96,4,98,134,251,12,35,15,232,235,249,90,41,248,17,15,128,90,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,96,80,98,105,110,169,115,
72,144,143,167,95,112,8,170,93,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,
202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,177,12,195,52,6,38,144,121,12,204,12,82,32,213,192,100,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,6,177,217,47,128,217,203,193,236,139,96,246,81,176,
26,160,122,32,229,0,83,3,164,30,67,196,193,70,4,66,196,25,244,129,212,15,16,155,196,248,197,29,146,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,222,195,228,236,232,0,0,0,47,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,52,46,114,115,114,99,85,84,9,0,3,96,147,
223,83,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,242,103,96,96,4,98,134,7,12,35,15,24,232,251,90,41,248,145,0,128,122,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,98,80,98,105,110,169,
115,72,144,143,167,95,112,8,170,101,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,
203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,178,12,195,52,6,38,144,121,12,204,12,82,32,213,192,116,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,1,177,217,47,128,217,43,192,236,139,96,246,49,176,
26,160,122,32,229,8,83,3,164,158,64,196,193,70,4,65,196,25,12,128,212,79,16,155,212,8,198,29,148,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,140,18,220,69,231,0,0,0,49,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,53,46,114,115,114,99,85,84,9,0,3,96,147,223,
83,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,96,96,96,4,98,134,135,12,35,15,24,26,248,90,41,248,145,2,128,154,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,100,80,98,105,110,169,115,
72,144,143,167,95,112,8,170,109,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,
202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,178,12,195,52,6,38,144,121,12,204,12,82,32,213,192,132,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,5,177,217,47,128,217,43,193,236,139,96,246,113,176,
26,160,122,32,229,4,83,3,164,158,66,196,33,142,135,136,51,24,2,169,95,32,54,201,49,140,59,44,1,80,75,3,4,20,0,0,0,8,0,204,128,4,69,21,143,194,71,232,0,0,0,51,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,54,46,114,115,114,99,85,84,9,0,3,96,147,223,83,
96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,100,96,96,4,98,134,71,12,35,15,24,25,250,90,41,248,145,4,128,186,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,102,80,98,105,110,169,115,72,
144,143,167,95,112,8,170,117,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,
113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,179,12,195,52,6,38,144,121,12,204,12,82,32,213,192,148,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,3,177,217,47,128,217,171,192,236,139,96,246,9,176,26,160,
122,32,229,12,83,3,164,158,65,196,193,70,132,64,196,25,140,128,212,111,16,155,244,40,198,29,152,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,126,174,36,243,232,0,0,0,53,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,55,46,114,115,114,99,85,84,9,0,3,96,147,223,
83,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,98,96,96,4,98,134,199,12,35,15,24,27,249,90,41,248,145,6,128,218,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,104,80,98,105,110,169,115,
72,144,143,167,95,112,8,170,125,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,
202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,179,12,195,52,6,38,144,121,12,204,12,82,32,213,192,164,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,54,7,177,217,47,128,217,171,193,236,139,96,246,73,176,
26,160,122,32,229,2,83,3,164,158,67,196,193,70,132,66,196,25,140,129,212,31,16,155,140,56,198,29,154,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,224,246,116,46,233,0,0,0,55,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,56,46,114,115,114,99,85,84,9,0,3,96,147,
223,83,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,102,96,96,4,98,134,39,12,35,15,152,24,251,90,41,248,145,8,128,250,56,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,106,80,98,105,110,169,
115,72,144,143,167,95,112,8,170,133,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,
203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,180,12,195,52,96,74,3,154,199,192,204,32,5,82,13,76,123,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,11,16,155,253,2,152,189,6,204,190,8,102,159,2,171,1,
170,7,82,174,48,53,64,234,5,68,28,108,68,24,68,156,193,4,72,253,5,177,201,137,100,220,193,9,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,245,209,122,114,232,0,0,0,57,3,0,0,14,0,28,0,114,101,115,117,108,116,95,52,57,46,114,115,114,99,85,84,9,0,3,96,147,223,83,
96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,97,96,96,4,98,134,167,12,35,15,152,154,248,90,41,248,145,10,128,26,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,108,80,98,105,110,169,115,
72,144,143,167,95,112,8,170,141,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,
202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,180,12,195,52,6,38,144,121,12,204,12,82,32,213,192,196,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,182,4,177,217,47,128,217,107,193,236,139,96,246,105,176,
26,160,122,32,229,6,83,3,164,94,66,196,193,70,132,67,196,25,76,129,212,63,16,155,172,88,198,29,158,0,80,75,3,4,20,0,0,0,8,0,202,128,4,69,132,3,129,91,227,0,0,0,225,2,0,0,13,0,28,0,114,101,115,117,108,116,95,53,46,114,115,114,99,85,84,9,0,3,92,147,223,
83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,96,96,96,4,98,134,157,12,35,15,112,114,248,90,41,248,129,0,152,227,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,165,65,137,165,185,165,206,33,
65,62,158,126,193,33,168,166,4,135,4,41,48,191,0,147,47,161,66,172,76,96,227,193,108,38,1,32,145,146,147,147,202,252,130,129,13,137,205,1,100,74,73,122,230,21,151,100,166,39,150,228,23,89,41,192,236,87,8,203,76,45,39,40,43,8,117,34,136,139,236,204,178,
114,220,206,100,191,0,38,47,226,115,38,251,5,132,51,129,108,14,156,161,39,195,48,141,129,9,100,30,3,51,131,20,72,53,48,65,121,149,100,228,165,3,233,36,230,23,255,255,43,64,21,50,191,4,179,121,65,108,246,11,96,118,35,152,125,17,204,158,15,86,3,84,15,242,
50,76,13,144,218,11,17,7,27,161,13,17,7,187,245,18,136,13,143,57,220,97,4,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,137,161,55,110,232,0,0,0,59,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,48,46,114,115,114,99,85,84,9,0,3,96,147,223,83,96,147,223,83,117,
120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,101,96,96,4,98,134,103,12,35,15,152,153,250,90,41,248,145,12,128,58,57,57,156,75,138,114,138,116,29,65,65,199,11,102,59,134,186,230,149,20,85,130,204,13,74,44,205,45,117,14,9,242,241,244,11,14,
65,181,50,56,36,72,129,249,5,152,124,9,21,98,101,2,145,156,96,54,147,0,144,72,201,201,73,101,126,193,192,134,196,230,0,50,165,36,61,243,138,75,50,211,19,75,242,139,172,20,96,246,43,132,101,166,150,19,148,21,132,58,17,196,69,118,102,89,57,110,103,178,
95,0,147,23,241,57,147,253,2,194,153,64,54,7,206,160,150,97,152,198,192,4,50,143,129,153,65,10,164,26,152,250,188,74,50,242,210,129,116,18,243,139,255,255,21,160,10,153,95,130,217,86,32,54,251,5,48,123,29,152,125,17,204,62,3,86,3,84,15,164,220,97,106,
128,212,43,136,56,216,136,8,136,56,131,25,144,2,201,49,144,23,205,184,3,20,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,123,37,41,49,233,0,0,0,61,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,49,46,114,115,114,99,85,84,9,0,3,96,147,223,83,96,147,223,83,117,
120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,99,96,96,4,98,134,231,12,35,15,152,155,249,90,41,248,145,14,128,90,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,112,80,98,105,110,169,115,72,144,143,167,95,
112,8,170,157,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,113,59,147,253,
2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,181,12,195,52,6,38,144,121,12,204,12,82,32,213,192,228,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,182,6,177,217,47,128,217,235,193,236,139,96,246,89,176,26,160,122,32,229,
1,83,3,164,94,67,196,193,70,68,66,196,25,204,25,192,105,156,129,129,204,120,198,29,162,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,204,206,194,167,234,0,0,0,63,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,50,46,114,115,114,99,85,84,9,0,3,96,147,223,83,96,
147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,10,103,96,96,4,98,134,23,12,35,15,88,152,251,90,41,248,145,1,128,122,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,114,80,98,105,110,169,115,72,
144,143,167,95,112,8,170,165,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,203,202,
113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,182,12,195,52,6,38,144,121,12,204,12,82,32,213,192,244,231,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,182,1,177,217,47,128,217,27,192,236,139,96,246,57,176,26,160,
122,32,229,9,83,3,164,222,64,196,193,70,68,65,196,25,44,128,137,27,148,202,25,200,141,104,220,65,10,0,80,75,3,4,20,0,0,0,8,0,204,128,4,69,210,61,170,101,232,0,0,0,65,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,51,46,114,115,114,99,85,84,9,0,3,96,147,
223,83,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,96,96,96,4,98,134,151,12,35,15,88,90,248,90,41,248,145,3,128,154,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,116,80,98,105,110,169,
115,72,144,143,167,95,112,8,170,173,193,33,65,10,204,47,192,36,44,208,89,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,
89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,180,101,24,166,49,48,129,204,99,96,102,144,2,169,6,38,64,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,182,45,136,205,126,1,204,222,8,102,95,4,179,207,
131,213,0,213,3,41,47,152,26,32,245,22,34,14,54,34,26,34,206,96,9,76,221,96,71,147,29,211,184,195,20,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,13,51,216,32,233,0,0,0,67,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,52,46,114,115,114,99,85,84,9,0,3,97,147,
223,83,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,100,96,96,4,98,134,87,12,35,15,88,89,250,90,41,248,145,5,128,186,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,118,80,98,105,110,169,
115,72,144,143,167,95,112,8,170,181,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,
203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,183,12,195,52,6,38,144,121,12,204,12,82,32,213,192,20,232,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,182,3,177,217,47,128,217,155,192,236,139,96,246,5,176,
26,160,122,32,229,13,83,3,164,222,65,196,193,70,196,64,196,25,172,128,201,155,25,196,38,63,170,113,7,42,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,91,59,26,7,232,0,0,0,69,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,53,46,114,115,114,99,85,84,9,0,3,97,147,
223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,98,96,96,4,98,134,215,12,35,15,88,91,249,90,41,248,145,7,128,218,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,120,80,98,105,110,169,
115,72,144,143,167,95,112,8,170,189,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,51,
203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,134,183,12,195,52,6,38,144,121,12,204,12,82,32,213,192,36,232,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,182,7,177,217,47,128,217,155,193,236,139,96,54,216,
53,32,245,64,202,7,166,6,72,189,135,136,131,141,136,133,136,51,88,3,211,55,11,136,77,65,92,227,14,85,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,62,10,85,12,233,0,0,0,71,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,54,46,114,115,114,99,85,84,9,0,3,97,147,
223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,102,96,96,4,98,134,55,12,35,15,216,88,251,90,41,248,145,9,128,250,57,57,156,75,138,114,138,116,29,67,129,28,94,48,219,49,212,53,175,164,168,18,100,122,80,98,105,110,
169,115,72,144,143,167,95,112,8,170,197,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,32,212,137,32,46,178,
51,203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,6,184,12,195,52,6,38,144,121,12,204,12,82,32,213,192,52,232,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,118,0,177,217,47,128,217,91,192,236,139,96,246,37,
176,26,160,122,32,229,11,83,3,164,62,64,196,193,70,196,65,196,25,108,128,9,156,21,196,166,36,178,113,7,43,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,39,70,249,247,233,0,0,0,73,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,55,46,114,115,114,99,85,84,9,0,3,
97,147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,97,96,96,4,98,134,183,12,35,15,216,218,248,90,41,248,145,11,128,6,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,248,160,196,210,
220,82,231,144,32,31,79,191,224,16,84,155,131,67,130,20,152,95,128,201,151,80,33,86,38,16,201,9,102,51,9,0,137,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,92,
100,103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,113,25,134,105,12,76,32,243,24,152,25,164,64,170,129,137,208,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,237,8,98,179,95,0,179,183,130,217,23,193,236,203,
96,53,64,245,64,202,15,166,6,72,125,132,136,131,141,136,135,136,51,216,2,83,56,27,136,77,81,108,227,14,87,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,31,69,205,107,233,0,0,0,75,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,56,46,114,115,114,99,85,84,9,0,3,
97,147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,101,96,96,4,98,134,119,12,35,15,216,217,250,90,41,248,145,13,128,38,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,252,160,196,
210,220,82,231,144,32,31,79,191,224,16,84,171,131,67,130,20,152,95,128,201,151,80,33,86,38,16,201,9,102,51,9,0,137,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,
92,100,103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,114,25,134,105,12,76,32,243,24,152,25,164,64,170,129,169,208,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,237,4,98,179,95,0,179,183,129,217,23,193,236,
43,96,53,64,245,64,202,31,166,6,72,125,130,136,131,141,72,128,136,51,216,1,147,56,59,136,77,89,116,227,14,88,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,129,21,26,153,233,0,0,0,77,3,0,0,14,0,28,0,114,101,115,117,108,116,95,53,57,46,114,115,114,99,85,84,9,0,
3,97,147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,99,96,96,4,98,134,247,12,35,15,216,219,249,90,41,248,145,15,128,70,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,130,160,196,
210,220,82,231,144,32,31,79,191,224,16,84,187,131,67,130,20,152,95,128,201,151,80,33,86,38,16,201,9,102,51,9,0,137,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,
92,100,103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,115,25,134,105,12,76,32,243,24,152,25,164,64,170,129,201,208,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,237,12,98,179,95,0,179,183,131,217,23,193,236,
171,96,53,64,245,64,42,0,166,6,72,125,134,136,131,141,72,132,136,51,216,3,211,56,216,69,20,198,55,238,144,5,0,80,75,3,4,20,0,0,0,8,0,202,128,4,69,110,88,193,15,232,0,0,0,227,2,0,0,13,0,28,0,114,101,115,117,108,116,95,54,46,114,115,114,99,85,84,9,0,3,
92,147,223,83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,100,96,96,4,98,134,93,12,35,15,112,113,250,90,41,248,129,1,144,199,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,181,65,137,
165,185,165,206,33,65,62,158,126,193,33,168,198,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,
80,39,130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,124,50,12,211,24,152,64,230,49,48,51,72,129,84,3,83,148,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,155,15,196,102,191,0,102,55,129,217,23,193,
236,5,96,53,64,245,64,74,26,166,6,72,237,131,136,131,141,208,129,136,51,112,1,169,203,96,71,195,163,14,119,32,1,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,243,38,14,109,233,0,0,0,79,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,48,46,114,115,114,99,85,84,
9,0,3,97,147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,138,103,96,96,4,98,134,15,12,35,15,56,216,251,90,41,248,81,0,128,102,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,134,160,
196,210,220,82,231,144,32,31,79,191,224,16,84,203,131,67,130,20,152,95,128,201,151,80,33,86,38,16,201,9,102,51,9,0,137,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,
65,92,100,103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,116,25,134,105,12,76,32,243,24,152,25,164,64,170,129,233,208,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,237,2,98,179,95,0,179,119,128,217,23,193,
236,107,96,53,64,245,64,42,16,166,6,72,125,129,136,131,141,72,130,136,51,56,0,19,57,216,189,148,70,56,238,160,5,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,136,161,34,101,233,0,0,0,81,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,49,46,114,115,114,99,85,84,
9,0,3,97,147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,96,96,96,4,98,134,143,12,35,15,56,58,248,90,41,248,81,2,128,134,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,138,160,196,
210,220,82,231,144,32,31,79,191,224,16,84,219,131,67,130,20,152,95,128,201,151,80,33,86,38,16,201,9,102,51,9,0,137,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,
92,100,103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,117,25,134,105,12,76,32,243,24,152,25,164,64,170,129,9,209,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,237,10,98,179,95,0,179,119,130,217,23,193,236,
235,96,53,64,245,32,247,195,212,0,169,175,16,113,176,17,201,16,113,6,71,96,42,231,2,177,41,142,113,220,97,11,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,142,215,14,50,233,0,0,0,83,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,50,46,114,115,114,99,85,84,9,0,
3,97,147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,100,96,96,4,98,134,79,12,35,15,56,57,250,90,41,248,81,4,128,166,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,142,160,196,210,
220,82,231,144,32,31,79,191,224,16,84,235,131,67,130,20,152,95,128,201,151,80,33,86,38,16,201,9,102,51,9,0,137,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,92,
100,103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,118,25,134,105,12,76,32,243,24,152,25,164,64,170,129,41,209,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,237,6,98,179,95,0,179,119,129,217,23,193,236,27,
96,53,64,245,32,71,194,212,0,169,111,16,113,176,17,41,16,113,6,39,96,50,231,6,177,41,143,114,220,129,11,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,117,133,246,137,233,0,0,0,85,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,51,46,114,115,114,99,85,84,9,0,3,
97,147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,98,96,96,4,98,134,207,12,35,15,56,59,249,90,41,248,81,6,128,198,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,146,160,196,210,
220,82,231,144,32,31,79,191,224,16,84,251,131,67,130,20,152,95,128,201,151,80,33,86,38,16,201,9,102,51,9,0,137,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,92,
100,103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,12,119,25,134,105,12,76,32,243,24,152,25,164,64,170,129,73,209,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,237,14,98,179,95,0,179,119,131,217,23,193,236,155,
96,53,64,245,64,42,4,166,6,72,125,135,136,131,141,72,133,136,51,56,3,211,57,15,136,77,133,56,199,29,186,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,153,31,52,238,233,0,0,0,87,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,52,46,114,115,114,99,85,84,9,0,3,97,
147,223,83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,102,96,96,4,98,134,47,12,35,15,184,56,251,90,41,248,81,8,128,230,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,150,160,196,210,220,
82,231,144,32,31,79,191,224,16,84,7,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,
200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,240,50,12,211,24,152,64,230,49,48,51,72,129,84,3,211,162,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,219,3,196,102,191,0,102,239,1,179,47,130,217,183,192,106,
128,234,129,84,40,76,13,144,250,1,17,7,27,145,6,17,103,112,1,38,116,94,16,155,26,145,142,59,120,1,80,75,3,4,20,0,0,0,8,0,205,128,4,69,86,187,225,243,233,0,0,0,89,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,53,46,114,115,114,99,85,84,9,0,3,97,147,223,
83,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,97,96,96,4,98,134,175,12,35,15,184,186,248,90,41,248,81,10,128,6,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,154,160,196,210,220,82,231,
144,32,31,79,191,224,16,84,23,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,
206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,242,50,12,211,24,152,64,230,49,48,51,72,129,84,3,19,163,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,219,19,196,102,191,0,102,239,5,179,47,130,217,183,193,106,128,
234,129,84,24,76,13,144,250,9,17,7,27,145,14,17,103,112,5,166,116,62,16,155,42,177,142,59,124,1,80,75,3,4,20,0,0,0,8,0,205,128,4,69,52,226,88,52,233,0,0,0,91,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,54,46,114,115,114,99,85,84,9,0,3,98,147,223,83,
98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,101,96,96,4,98,134,111,12,35,15,184,185,250,90,41,248,81,12,128,38,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,158,160,196,210,220,82,231,
144,32,31,79,191,224,16,84,39,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,
206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,244,50,12,211,24,152,64,230,49,48,51,72,129,84,3,83,163,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,219,11,196,102,191,0,102,239,3,179,47,130,217,119,192,106,128,
234,129,84,56,76,13,144,250,5,17,7,27,145,1,17,103,112,3,38,117,126,16,155,58,209,142,59,128,1,80,75,3,4,20,0,0,0,8,0,205,128,4,69,128,144,115,254,234,0,0,0,93,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,55,46,114,115,114,99,85,84,9,0,3,98,147,223,
83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,99,96,96,4,98,134,239,12,35,15,184,187,249,90,41,248,81,14,128,70,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,162,160,196,210,220,82,231,
144,32,31,79,191,224,16,84,55,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,
206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,246,50,12,211,24,152,64,230,49,48,51,72,129,84,3,147,163,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,219,27,196,102,191,0,102,239,7,179,47,130,217,119,193,106,
128,234,129,84,4,76,13,144,250,13,17,7,27,145,9,17,103,112,7,166,117,144,227,24,168,20,239,184,67,24,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,117,144,110,67,234,0,0,0,95,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,56,46,114,115,114,99,85,84,9,0,3,98,147,
223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,74,103,96,96,4,98,134,31,12,35,15,120,184,251,90,41,248,81,1,0,205,226,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,162,74,144,77,65,137,165,185,
165,206,33,65,62,158,126,193,33,168,142,8,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,
157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,240,101,24,166,49,48,129,204,99,96,102,144,2,169,6,166,71,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,182,15,136,205,126,1,204,62,0,102,95,4,179,
239,129,213,0,213,3,169,72,152,26,32,245,7,34,14,54,34,11,34,206,224,1,76,236,130,32,54,181,34,30,119,16,3,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,103,250,70,51,234,0,0,0,97,3,0,0,14,0,28,0,114,101,115,117,108,116,95,54,57,46,114,115,114,99,85,84,9,0,3,
98,147,223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,96,96,96,4,98,134,159,12,35,15,120,122,248,90,41,248,81,3,0,13,227,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,162,74,144,85,65,137,165,
185,165,206,33,65,62,158,126,193,33,168,174,8,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,
145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,244,101,24,166,49,48,129,204,99,96,102,144,2,169,6,38,72,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,182,47,136,205,126,1,204,62,8,102,95,4,179,
239,131,213,0,213,3,169,40,152,26,32,245,23,34,14,54,34,27,34,206,224,9,76,237,66,32,54,213,98,30,119,24,3,0,80,75,3,4,20,0,0,0,8,0,202,128,4,69,91,156,220,62,231,0,0,0,229,2,0,0,13,0,28,0,114,101,115,117,108,116,95,55,46,114,115,114,99,85,84,9,0,3,92,
147,223,83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,98,96,96,4,98,134,221,12,35,15,112,115,249,90,41,248,65,0,144,203,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,32,197,65,137,165,
185,165,206,33,65,62,158,126,193,33,168,230,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,
130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,126,50,12,211,24,152,64,230,49,48,51,72,129,84,3,147,148,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,155,31,196,102,191,0,102,55,131,217,23,193,236,
133,96,53,64,245,96,51,161,106,128,212,126,136,56,216,8,93,136,56,3,55,144,186,2,98,35,197,29,238,80,2,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,31,201,99,29,234,0,0,0,99,3,0,0,14,0,28,0,114,101,115,117,108,116,95,55,48,46,114,115,114,99,85,84,9,0,3,98,147,
223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,100,96,96,4,98,134,95,12,35,15,120,121,250,90,41,248,81,5,0,77,227,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,162,74,144,93,65,137,165,185,
165,206,33,65,62,158,126,193,33,168,206,8,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,
157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,248,101,24,166,49,48,129,204,99,96,102,144,2,169,6,166,72,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,54,40,144,25,216,47,128,217,135,192,236,139,
96,246,3,176,26,160,122,32,21,13,83,3,164,254,65,196,193,70,228,64,196,25,188,128,201,93,24,196,166,94,212,227,14,100,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,225,238,226,45,234,0,0,0,101,3,0,0,14,0,28,0,114,101,115,117,108,116,95,55,49,46,114,115,114,99,
85,84,9,0,3,98,147,223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,98,96,96,4,98,134,223,12,35,15,120,123,249,90,41,248,81,7,0,141,227,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,162,74,144,
101,65,137,165,185,165,206,33,65,62,158,126,193,33,168,238,8,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,
5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,252,101,24,166,49,48,129,204,99,96,102,144,2,169,6,38,73,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,182,63,136,205,126,1,204,
62,12,102,95,4,179,31,130,213,0,213,3,169,24,152,26,32,245,31,34,14,54,34,23,34,206,224,13,76,239,34,32,54,21,227,30,119,40,3,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,173,150,58,56,233,0,0,0,103,3,0,0,14,0,28,0,114,101,115,117,108,116,95,55,50,46,114,115,
114,99,85,84,9,0,3,98,147,223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,102,96,96,4,98,134,63,12,35,15,248,120,251,90,41,248,81,9,0,205,227,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,162,
74,144,109,65,137,165,185,165,206,33,65,62,158,126,193,33,168,14,9,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,
4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,2,100,24,166,49,48,129,204,99,96,102,144,2,169,6,166,73,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,0,136,205,126,1,
204,62,2,102,95,4,179,31,129,213,0,213,3,169,88,152,26,6,112,2,7,137,131,169,60,136,56,131,15,80,92,20,196,166,102,228,227,14,102,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,76,217,183,98,234,0,0,0,105,3,0,0,14,0,28,0,114,101,115,117,108,116,95,55,51,46,114,
115,114,99,85,84,9,0,3,98,147,223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,97,96,96,4,98,134,191,12,35,15,248,250,248,90,41,248,81,11,0,13,228,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,80,215,188,146,
162,74,144,117,65,137,165,185,165,206,33,65,62,158,126,193,33,168,46,9,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,
229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,6,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,74,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,32,136,205,126,
1,204,62,10,102,95,4,179,31,131,213,0,213,3,169,56,152,26,96,202,102,132,136,131,141,200,135,136,51,248,2,197,197,64,108,170,198,62,238,112,6,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,220,212,42,26,234,0,0,0,107,3,0,0,14,0,28,0,114,101,115,117,108,116,95,
55,52,46,114,115,114,99,85,84,9,0,3,98,147,223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,101,96,96,4,98,134,127,12,35,15,248,249,250,90,41,248,81,13,0,77,228,228,112,46,41,202,41,210,117,12,5,114,120,193,108,199,
80,215,188,146,162,74,144,125,65,137,165,185,165,206,33,65,62,158,126,193,33,168,78,9,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,
10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,10,100,24,166,49,48,129,204,99,96,102,144,2,169,6,166,74,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,
16,136,205,126,1,204,62,6,102,95,4,179,159,128,213,0,213,3,169,120,152,26,96,210,102,130,136,131,141,40,128,136,51,0,35,138,81,28,196,166,110,244,227,14,104,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,238,131,18,210,234,0,0,0,109,3,0,0,14,0,28,0,114,101,115,
117,108,116,95,55,53,46,114,115,114,99,85,84,9,0,3,98,147,223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,99,96,96,4,98,134,255,12,35,15,248,251,249,90,41,248,81,15,0,141,228,228,112,46,41,202,41,210,117,12,5,114,
120,193,108,199,80,215,188,146,162,74,144,133,65,137,165,185,165,206,33,65,62,158,126,193,33,168,110,9,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,
34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,14,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,75,175,146,140,188,116,32,157,196,252,226,255,127,5,168,
66,230,151,96,118,48,136,205,126,1,204,62,14,102,95,4,179,159,130,213,0,213,3,169,4,152,26,96,218,102,134,136,131,141,40,132,136,51,248,3,197,37,64,108,42,199,63,238,144,6,0,80,75,3,4,20,0,0,0,8,0,205,128,4,69,92,1,49,232,231,0,0,0,111,3,0,0,14,0,28,
0,114,101,115,117,108,116,95,55,54,46,114,115,114,99,85,84,9,0,3,98,147,223,83,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,202,103,96,96,4,97,134,17,8,2,252,125,173,20,252,168,8,128,102,114,114,56,151,20,229,20,233,58,134,
2,57,188,96,182,99,168,107,94,73,81,37,200,198,160,196,210,220,82,231,144,32,31,79,191,224,16,84,199,4,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,
145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,9,50,12,211,24,152,64,230,49,48,51,72,129,84,3,211,162,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,
75,48,27,236,30,246,11,96,246,9,48,251,34,152,253,12,172,6,168,30,72,37,194,212,0,19,54,11,68,28,108,68,17,68,156,33,0,40,46,9,98,83,59,1,224,14,106,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,15,153,7,71,233,0,0,0,113,3,0,0,14,0,28,0,114,101,115,117,108,116,
95,55,55,46,114,115,114,99,85,84,9,0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,96,96,96,4,97,16,103,164,129,192,0,95,43,5,63,106,2,160,161,156,28,206,37,69,57,69,186,142,161,64,14,47,152,237,24,234,154,
87,82,84,9,178,50,40,177,52,183,212,57,36,200,199,211,47,56,4,213,53,193,33,65,10,204,47,192,228,75,168,16,43,19,136,228,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,
160,172,32,212,137,32,46,178,51,203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,198,130,12,195,52,6,38,144,121,12,204,12,82,32,213,192,132,233,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,6,133,43,3,251,5,48,
251,36,152,125,17,204,126,14,86,3,84,15,164,146,96,106,128,41,155,21,34,14,54,162,24,34,206,16,8,20,151,2,177,169,158,2,112,135,53,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,196,119,138,226,231,0,0,0,115,3,0,0,14,0,28,0,114,101,115,117,108,116,95,55,56,46,
114,115,114,99,85,84,9,0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,100,96,96,4,97,38,134,145,7,130,2,125,173,20,252,168,10,128,166,114,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,
81,37,216,206,196,210,220,82,231,144,32,31,79,191,224,16,84,231,4,135,4,41,48,191,0,147,47,161,66,172,224,104,225,4,179,153,4,128,68,74,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,
32,212,137,32,46,178,51,203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,70,131,12,195,52,6,38,144,121,12,204,12,82,32,213,192,148,233,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,14,3,177,217,47,128,217,167,
192,236,139,96,246,11,176,26,160,122,32,149,12,83,3,76,218,108,16,113,176,17,37,16,113,134,32,160,184,52,136,77,253,36,128,59,176,1,80,75,3,4,20,0,0,0,8,0,206,128,4,69,179,18,51,31,231,0,0,0,117,3,0,0,14,0,28,0,114,101,115,117,108,116,95,55,57,46,114,
115,114,99,85,84,9,0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,98,96,96,4,97,102,134,145,7,130,131,124,173,20,252,168,11,128,198,114,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,
37,200,210,160,196,210,220,82,231,144,32,31,79,191,224,16,52,247,132,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,
130,178,130,80,39,130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,15,50,12,211,24,152,64,230,49,48,51,72,129,84,3,147,166,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,28,196,102,191,0,102,159,6,
179,47,130,217,224,64,3,169,7,89,10,83,3,76,219,236,16,113,176,17,165,16,113,134,96,160,184,12,216,163,212,79,3,184,67,27,0,80,75,3,4,20,0,0,0,8,0,202,128,4,69,94,148,161,244,232,0,0,0,231,2,0,0,13,0,28,0,114,101,115,117,108,116,95,56,46,114,115,114,
99,85,84,9,0,3,92,147,223,83,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,102,96,96,4,98,134,61,12,35,15,240,112,251,90,41,248,65,1,144,207,201,225,92,82,148,83,164,235,24,10,228,240,130,217,142,161,174,121,37,69,149,
32,213,65,137,165,185,165,206,33,65,62,158,126,193,33,168,6,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,
130,178,130,80,39,130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,128,50,12,211,24,152,64,230,49,48,51,72,129,84,3,211,148,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,27,100,40,3,251,5,48,187,5,204,
190,8,102,47,2,171,1,170,7,82,178,48,53,64,234,0,68,28,108,132,30,68,156,129,7,72,93,5,177,145,35,15,119,48,1,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,56,12,166,165,234,0,0,0,119,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,48,46,114,115,114,99,85,84,9,
0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,102,96,96,4,97,22,134,145,7,66,130,125,173,20,252,168,12,128,230,114,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,214,160,196,
210,220,82,231,144,32,31,79,191,224,16,84,7,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,
130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,17,50,12,211,24,152,64,230,49,48,51,72,129,84,3,211,166,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,2,196,102,191,0,102,159,1,179,47,130,217,175,
192,106,128,234,129,84,42,76,13,48,113,115,64,196,193,70,148,65,196,25,128,126,98,148,5,251,134,6,137,0,119,112,3,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,140,150,10,90,233,0,0,0,121,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,49,46,114,115,114,99,85,
84,9,0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,97,96,96,4,97,86,134,145,7,66,67,124,173,20,252,168,13,128,6,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,218,160,196,
210,220,82,231,144,32,31,79,191,224,16,84,23,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,
39,130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,19,50,12,211,24,152,64,230,49,48,51,72,129,84,3,19,167,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,18,196,102,191,0,102,159,5,179,47,130,217,
175,193,106,128,234,129,84,26,76,13,48,117,115,66,196,193,70,148,67,196,25,128,113,195,40,7,98,211,36,21,224,14,111,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,26,241,151,21,233,0,0,0,123,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,50,46,114,115,114,99,85,
84,9,0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,101,96,96,4,97,54,134,145,7,194,66,125,173,20,252,168,14,128,38,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,222,160,
196,210,220,82,231,144,32,31,79,191,224,16,84,39,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,
80,39,130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,21,50,12,211,24,152,64,230,49,48,51,72,129,84,3,83,167,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,10,196,102,191,0,102,159,3,179,47,130,217,
111,192,106,128,234,129,84,58,76,13,48,121,115,65,196,193,70,84,64,196,25,194,128,226,242,32,54,109,146,1,238,0,7,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,162,3,21,38,232,0,0,0,125,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,51,46,114,115,114,99,85,84,
9,0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,99,96,96,4,97,118,134,145,7,194,195,124,173,20,252,168,15,128,70,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,226,160,196,
210,220,82,231,144,32,31,79,191,224,16,84,55,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,
39,130,184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,23,50,12,211,24,152,64,230,49,48,51,72,129,84,3,147,167,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,26,196,102,191,0,102,159,7,179,47,130,217,
111,193,106,128,234,129,84,6,76,13,48,125,115,67,196,193,70,84,66,196,25,194,129,226,96,99,105,148,14,112,135,56,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,12,141,252,22,231,0,0,0,127,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,52,46,114,115,114,99,85,84,
9,0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,42,103,96,96,4,97,14,134,145,7,34,194,125,173,20,252,104,0,128,102,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,230,160,196,
210,220,82,231,144,32,31,79,191,224,16,84,71,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,160,248,147,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,
23,217,153,101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,184,147,153,12,195,52,6,38,144,121,12,204,12,82,32,213,192,244,233,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,142,1,177,217,47,128,217,23,192,236,139,96,246,
59,176,26,160,122,32,149,9,83,3,76,224,60,16,113,176,17,85,16,113,134,8,160,184,34,136,77,171,132,128,59,200,1,80,75,3,4,20,0,0,0,8,0,206,128,4,69,175,128,45,202,233,0,0,0,129,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,53,46,114,115,114,99,85,84,9,
0,3,99,147,223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,96,96,96,4,97,78,134,145,7,34,35,124,173,20,252,104,1,128,134,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,234,160,196,210,
220,82,231,144,32,31,79,191,224,16,84,87,5,135,4,41,48,191,0,147,47,161,66,172,76,32,18,18,69,76,2,64,34,37,39,39,149,249,5,3,27,18,155,3,200,148,146,244,204,43,46,201,76,79,44,201,47,178,82,128,217,175,16,150,153,90,78,80,86,16,234,68,16,23,217,153,
101,229,184,157,201,126,1,76,94,196,231,76,246,11,8,103,2,217,28,56,99,67,134,97,26,3,19,200,60,6,102,6,41,144,106,96,2,245,42,201,200,75,7,210,73,204,47,254,255,87,128,42,100,126,9,102,199,130,216,236,23,192,108,176,11,216,47,130,217,239,193,106,128,
234,129,84,22,76,13,48,133,243,66,196,193,70,84,67,196,25,34,129,226,74,32,54,205,82,2,238,48,7,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,122,116,4,14,233,0,0,0,131,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,54,46,114,115,114,99,85,84,9,0,3,99,147,223,
83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,100,96,96,4,97,46,134,145,7,162,34,125,173,20,252,104,2,128,166,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,238,160,196,210,220,82,231,
144,32,31,79,191,224,16,84,103,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,
206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,29,50,12,211,24,152,64,230,49,48,51,72,129,84,3,83,168,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,14,196,102,191,0,102,95,2,179,47,130,217,31,192,106,128,234,
129,84,54,76,13,48,137,243,65,196,193,70,212,64,196,25,162,128,226,202,32,54,237,146,2,238,64,7,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,76,80,198,48,233,0,0,0,133,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,55,46,114,115,114,99,85,84,9,0,3,99,147,223,
83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,98,96,96,4,97,110,134,145,7,162,163,124,173,20,252,104,3,128,198,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,242,160,196,210,220,82,231,
144,32,31,79,191,224,16,84,119,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,
206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,31,50,12,211,24,152,64,230,49,48,51,72,129,84,3,147,168,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,30,196,102,191,0,102,95,6,179,47,130,217,31,193,106,128,
234,129,84,14,76,13,48,141,243,67,196,193,70,212,66,196,25,162,129,226,42,32,54,13,211,2,238,80,7,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,12,248,192,133,233,0,0,0,135,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,56,46,114,115,114,99,85,84,9,0,3,100,147,
223,83,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,102,96,96,4,97,30,134,145,7,98,162,125,173,20,252,104,4,128,230,115,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,246,160,196,210,220,82,
231,144,32,31,79,191,224,16,84,135,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,
200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,33,50,12,211,24,152,64,230,49,48,51,72,129,84,3,211,168,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,1,196,102,191,0,102,95,1,179,47,130,217,159,192,106,
128,234,129,84,46,76,13,48,145,11,64,196,193,70,212,65,196,25,98,128,226,170,32,54,45,19,3,238,96,7,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,179,236,183,66,233,0,0,0,137,3,0,0,14,0,28,0,114,101,115,117,108,116,95,56,57,46,114,115,114,99,85,84,9,0,3,100,
147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,97,96,96,4,97,94,134,145,7,98,99,124,173,20,252,104,5,128,22,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,250,160,196,210,220,
82,231,144,32,31,79,191,224,16,84,151,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,
184,200,206,44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,35,50,12,211,24,152,64,230,49,48,51,72,129,84,3,19,169,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,17,196,102,191,0,102,95,5,179,47,130,217,159,193,
106,128,234,129,84,30,76,13,48,149,11,66,196,193,70,212,67,196,25,98,129,226,106,32,54,77,83,3,238,112,7,0,80,75,3,4,20,0,0,0,8,0,203,128,4,69,55,86,176,127,230,0,0,0,233,2,0,0,13,0,28,0,114,101,115,117,108,116,95,57,46,114,115,114,99,85,84,9,0,3,93,
147,223,83,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,210,97,96,96,4,98,134,189,12,35,15,240,242,248,90,41,248,193,0,80,128,147,195,185,164,40,167,72,215,49,20,36,11,102,59,134,186,230,149,20,85,130,148,7,37,150,230,150,
58,135,4,249,120,250,5,135,160,154,20,28,18,164,192,252,2,76,190,132,10,177,50,129,72,78,48,155,73,0,72,164,228,228,164,50,191,96,96,67,98,115,0,153,82,146,158,121,197,37,153,233,137,37,249,69,86,10,48,251,21,194,50,83,203,9,202,10,66,157,8,226,34,59,
179,172,28,183,51,217,47,128,201,139,248,156,201,126,1,225,76,32,155,3,103,8,202,48,76,99,96,2,153,199,192,204,32,5,82,13,76,84,94,37,25,121,233,64,58,137,249,197,255,255,10,80,133,204,47,193,108,65,16,155,253,2,152,221,10,102,95,4,179,23,131,213,0,213,
3,41,57,152,26,32,117,16,34,14,54,66,31,34,206,192,11,164,174,129,216,40,177,135,59,156,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,150,196,103,101,233,0,0,0,139,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,48,46,114,115,114,99,85,84,9,0,3,100,147,223,83,
100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,101,96,96,4,97,62,134,145,7,226,98,125,173,20,252,104,6,128,54,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,254,160,196,210,220,82,231,144,
32,31,79,191,224,16,84,167,5,135,4,41,48,191,0,147,47,161,66,172,76,32,146,19,204,102,18,0,18,41,57,57,169,204,47,24,216,144,216,28,64,166,148,164,103,94,113,73,102,122,98,73,126,145,149,2,204,126,133,176,204,212,114,130,178,130,80,39,130,184,200,206,
44,43,199,237,76,246,11,96,242,34,62,103,178,95,64,56,19,200,230,192,25,37,50,12,211,24,152,64,230,49,48,51,72,129,84,3,83,169,87,73,70,94,58,144,78,98,126,241,255,191,2,84,33,243,75,48,59,9,196,102,191,0,102,95,3,179,47,130,217,95,192,106,128,234,129,
84,62,76,13,48,153,11,65,196,193,70,52,64,196,25,226,128,226,234,32,54,109,147,3,238,128,7,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,151,192,122,148,233,0,0,0,141,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,49,46,114,115,114,99,85,84,9,0,3,100,147,223,
83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,99,96,96,4,97,126,134,145,7,226,227,124,173,20,252,104,7,128,86,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,1,65,137,165,185,165,206,
33,65,62,158,126,193,33,168,110,11,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,
86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,78,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,83,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,50,136,205,126,1,204,190,14,102,95,4,179,191,130,
213,0,213,3,169,2,152,26,96,58,23,134,136,131,141,104,132,136,51,196,3,197,53,64,108,26,167,7,220,33,15,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,228,11,174,206,233,0,0,0,143,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,50,46,114,115,114,99,85,84,9,0,3,
100,147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,170,103,96,96,4,97,1,134,145,7,18,226,125,173,20,252,104,8,128,118,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,5,65,137,165,185,
165,206,33,65,62,158,126,193,33,168,142,11,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,4,138,176,148,156,156,84,230,23,12,108,72,108,14,32,83,74,210,51,175,184,36,51,61,177,36,191,200,74,1,102,191,66,88,102,106,57,65,89,65,168,19,65,92,100,
103,150,149,227,118,38,251,5,48,121,17,159,51,217,47,32,156,9,100,115,224,140,20,25,134,105,12,76,32,243,24,152,25,164,64,170,129,233,212,171,36,35,47,29,72,39,49,191,248,255,95,1,170,144,249,37,152,157,2,98,179,95,0,179,111,128,217,23,193,236,111,96,
53,64,245,64,170,16,166,6,152,208,69,32,226,96,35,154,32,226,12,9,64,113,77,16,155,214,9,2,119,208,3,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,223,50,157,234,233,0,0,0,145,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,51,46,114,115,114,99,85,84,9,0,3,100,
147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,96,96,96,4,97,65,134,145,7,18,19,124,173,20,252,104,9,128,150,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,9,65,137,165,185,165,
206,33,65,62,158,126,193,33,168,174,11,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,
89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,86,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,84,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,42,136,205,126,1,204,190,9,102,95,4,179,191,131,
213,0,213,3,169,34,152,26,96,74,23,133,136,131,141,104,134,136,51,36,2,197,181,64,108,154,167,8,220,97,15,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,22,170,235,130,233,0,0,0,147,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,52,46,114,115,114,99,85,84,9,0,
3,100,147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,100,96,96,4,97,33,134,145,7,146,18,125,173,20,252,104,10,128,182,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,13,65,137,
165,185,165,206,33,65,62,158,126,193,33,168,206,11,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,
113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,90,100,24,166,49,48,129,204,99,96,102,144,2,169,6,166,84,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,26,136,205,126,1,204,190,5,102,95,
4,179,127,128,213,0,213,3,169,98,152,26,96,82,23,131,136,131,141,104,129,136,51,36,1,197,181,65,108,218,39,9,220,129,15,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,238,16,95,130,233,0,0,0,149,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,53,46,114,115,114,
99,85,84,9,0,3,100,147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,98,96,96,4,97,97,134,145,7,146,147,124,173,20,252,104,11,128,214,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,37,200,
17,65,137,165,185,165,206,33,65,62,158,126,193,33,168,238,11,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,4,101,
5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,94,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,85,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,58,136,205,126,1,204,190,
13,102,95,4,179,127,130,213,0,213,3,169,18,152,26,96,90,23,135,136,131,141,104,133,136,51,36,3,197,117,64,108,58,164,9,220,161,15,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,228,223,165,33,232,0,0,0,151,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,54,46,114,
115,114,99,85,84,9,0,3,100,147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,102,96,96,4,97,17,134,145,7,82,146,125,173,20,252,104,12,128,246,112,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,
81,37,200,21,65,137,165,185,165,206,33,65,62,158,126,193,33,168,14,12,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,242,68,78,78,42,243,11,6,54,36,54,7,144,41,37,233,153,87,92,146,153,158,88,146,95,100,165,0,179,95,33,44,51,181,156,160,172,
32,212,137,32,46,178,51,203,202,113,59,147,253,2,152,188,136,207,153,236,23,16,206,4,178,57,112,70,140,12,195,52,6,38,144,121,12,204,12,82,32,213,192,180,234,85,146,145,151,14,164,147,152,95,252,255,175,0,85,200,252,18,204,206,0,177,217,47,128,217,119,
192,236,139,96,246,47,176,26,160,122,32,85,10,83,3,76,236,18,16,113,176,17,109,16,113,134,20,160,184,46,136,77,143,68,129,59,248,1,80,75,3,4,20,0,0,0,8,0,206,128,4,69,172,109,99,88,233,0,0,0,153,3,0,0,14,0,28,0,114,101,115,117,108,116,95,57,55,46,114,
115,114,99,85,84,9,0,3,100,147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,97,96,96,4,97,81,134,145,7,82,83,124,173,20,252,104,13,128,22,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,168,107,94,73,81,
37,200,25,65,137,165,185,165,206,33,65,62,158,126,193,33,168,46,12,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,97,153,169,229,
4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,102,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,86,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,38,136,205,126,
1,204,190,11,102,95,4,179,127,131,213,0,213,3,169,50,152,26,96,106,151,132,136,131,141,104,135,136,51,164,2,197,245,64,108,186,164,10,220,225,15,0,80,75,3,4,20,0,0,0,8,0,206,128,4,69,82,25,142,119,233,0,0,0,155,3,0,0,14,0,28,0,114,101,115,117,108,116,
95,57,56,46,114,115,114,99,85,84,9,0,3,100,147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,101,96,96,4,97,49,134,145,7,210,82,125,173,20,252,104,14,128,54,113,114,56,151,20,229,20,233,58,134,2,57,188,96,182,99,
168,107,94,73,81,37,200,29,65,137,165,185,165,206,33,65,62,158,126,193,33,168,78,12,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,43,5,152,253,10,
97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,106,100,24,166,49,48,129,204,99,96,102,144,2,169,6,166,86,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,230,151,96,118,
22,136,205,126,1,204,190,7,102,95,4,179,255,128,213,0,213,3,169,114,152,26,96,114,151,130,136,131,141,232,128,136,51,164,1,197,245,65,108,250,36,11,220,17,0,0,80,75,3,4,20,0,0,0,8,0,207,128,4,69,212,116,58,233,233,0,0,0,157,3,0,0,14,0,28,0,114,101,115,
117,108,116,95,57,57,46,114,115,114,99,85,84,9,0,3,101,147,223,83,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,99,96,96,100,96,96,96,106,99,96,96,4,97,113,134,145,7,210,211,124,173,20,252,104,15,128,86,113,114,56,151,20,229,20,233,58,134,2,57,
188,96,182,99,168,107,94,73,81,37,200,33,65,137,165,185,165,206,33,65,62,158,126,193,33,168,110,12,14,9,82,96,126,1,38,95,66,133,88,153,64,36,39,152,205,36,0,36,82,114,114,82,153,95,48,176,33,177,57,128,76,41,73,207,188,226,146,204,244,196,146,252,34,
43,5,152,253,10,97,153,169,229,4,101,5,161,78,4,113,145,157,89,86,142,219,153,236,23,192,228,69,124,206,100,191,128,112,38,144,205,129,51,110,100,24,166,49,48,129,204,99,96,102,144,2,169,6,38,87,175,146,140,188,116,32,157,196,252,226,255,127,5,168,66,
230,151,96,118,54,136,205,126,1,204,190,15,102,95,4,179,255,130,213,0,213,3,169,10,152,26,96,122,151,134,136,131,141,232,132,136,51,164,3,197,13,64,108,58,165,11,220,49,0,0,80,75,1,2,30,3,20,0,0,0,8,0,202,128,4,69,232,71,224,189,228,0,0,0,217,2,0,0,13,
0,24,0,0,0,0,0,0,0,0,0,164,129,0,0,0,0,114,101,115,117,108,116,95,49,46,114,115,114,99,85,84,5,0,3,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,117,140,95,228,231,0,0,0,235,2,0,0,14,0,24,0,0,0,0,0,0,0,0,
0,164,129,43,1,0,0,114,101,115,117,108,116,95,49,48,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,206,128,143,162,233,0,0,0,159,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,90,2,
0,0,114,101,115,117,108,116,95,49,48,48,46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,184,149,111,39,233,0,0,0,161,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,140,3,0,0,114,101,
115,117,108,116,95,49,48,49,46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,41,97,197,38,232,0,0,0,163,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,190,4,0,0,114,101,115,117,108,116,
95,49,48,50,46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,224,216,51,137,234,0,0,0,165,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,239,5,0,0,114,101,115,117,108,116,95,49,48,51,
46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,18,234,160,117,233,0,0,0,167,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,34,7,0,0,114,101,115,117,108,116,95,49,48,52,46,114,115,114,
99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,124,167,24,12,234,0,0,0,169,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,84,8,0,0,114,101,115,117,108,116,95,49,48,53,46,114,115,114,99,85,84,5,0,
3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,218,70,177,193,234,0,0,0,171,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,135,9,0,0,114,101,115,117,108,116,95,49,48,54,46,114,115,114,99,85,84,5,0,3,101,147,223,
83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,110,96,93,186,234,0,0,0,173,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,186,10,0,0,114,101,115,117,108,116,95,49,48,55,46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,
11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,205,85,200,97,234,0,0,0,175,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,237,11,0,0,114,101,115,117,108,116,95,49,48,56,46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,
1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,212,59,155,129,234,0,0,0,177,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,32,13,0,0,114,101,115,117,108,116,95,49,48,57,46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,
0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,222,162,66,46,231,0,0,0,237,2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,83,14,0,0,114,101,115,117,108,116,95,49,49,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,
30,3,20,0,0,0,8,0,207,128,4,69,109,200,53,89,233,0,0,0,179,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,130,15,0,0,114,101,115,117,108,116,95,49,49,48,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,
0,8,0,207,128,4,69,37,80,205,143,234,0,0,0,181,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,180,16,0,0,114,101,115,117,108,116,95,49,49,49,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,
4,69,140,159,95,150,234,0,0,0,183,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,231,17,0,0,114,101,115,117,108,116,95,49,49,50,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,87,82,
62,74,234,0,0,0,185,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,26,19,0,0,114,101,115,117,108,116,95,49,49,51,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,157,131,185,211,234,
0,0,0,187,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,77,20,0,0,114,101,115,117,108,116,95,49,49,52,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,60,236,242,144,234,0,0,0,189,3,
0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,128,21,0,0,114,101,115,117,108,116,95,49,49,53,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,123,139,94,31,234,0,0,0,191,3,0,0,15,0,24,
0,0,0,0,0,0,0,0,0,164,129,179,22,0,0,114,101,115,117,108,116,95,49,49,54,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,250,175,209,49,235,0,0,0,193,3,0,0,15,0,24,0,0,0,0,0,
0,0,0,0,164,129,230,23,0,0,114,101,115,117,108,116,95,49,49,55,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,115,35,90,147,234,0,0,0,195,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,
129,26,25,0,0,114,101,115,117,108,116,95,49,49,56,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,62,13,39,110,234,0,0,0,197,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,77,26,0,0,
114,101,115,117,108,116,95,49,49,57,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,218,109,58,25,231,0,0,0,239,2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,128,27,0,0,114,101,115,
117,108,116,95,49,50,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,80,233,53,113,234,0,0,0,199,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,175,28,0,0,114,101,115,117,108,116,95,
49,50,48,46,114,115,114,99,85,84,5,0,3,102,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,59,105,19,97,234,0,0,0,201,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,226,29,0,0,114,101,115,117,108,116,95,49,50,49,46,
114,115,114,99,85,84,5,0,3,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,195,163,20,47,234,0,0,0,203,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,21,31,0,0,114,101,115,117,108,116,95,49,50,50,46,114,115,114,
99,85,84,5,0,3,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,106,168,210,101,234,0,0,0,205,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,72,32,0,0,114,101,115,117,108,116,95,49,50,51,46,114,115,114,99,85,84,5,
0,3,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,33,174,57,145,234,0,0,0,207,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,123,33,0,0,114,101,115,117,108,116,95,49,50,52,46,114,115,114,99,85,84,5,0,3,103,147,
223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,59,134,124,168,234,0,0,0,209,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,174,34,0,0,114,101,115,117,108,116,95,49,50,53,46,114,115,114,99,85,84,5,0,3,103,147,223,83,117,
120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,97,182,64,157,234,0,0,0,211,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,225,35,0,0,114,101,115,117,108,116,95,49,50,54,46,114,115,114,99,85,84,5,0,3,103,147,223,83,117,120,11,0,1,
4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,159,57,50,10,235,0,0,0,213,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,20,37,0,0,114,101,115,117,108,116,95,49,50,55,46,114,115,114,99,85,84,5,0,3,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,
20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,208,128,4,69,68,6,226,207,235,0,0,0,215,3,0,0,15,0,24,0,0,0,0,0,0,0,0,0,164,129,72,38,0,0,114,101,115,117,108,116,95,49,50,56,46,114,115,114,99,85,84,5,0,3,103,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,
1,2,30,3,20,0,0,0,8,0,203,128,4,69,106,156,219,100,231,0,0,0,241,2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,124,39,0,0,114,101,115,117,108,116,95,49,51,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,
0,0,8,0,203,128,4,69,76,165,169,7,231,0,0,0,243,2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,171,40,0,0,114,101,115,117,108,116,95,49,52,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,
4,69,12,23,213,244,232,0,0,0,245,2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,218,41,0,0,114,101,115,117,108,116,95,49,53,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,233,45,180,
135,232,0,0,0,247,2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,10,43,0,0,114,101,115,117,108,116,95,49,54,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,220,115,75,74,232,0,0,0,249,
2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,58,44,0,0,114,101,115,117,108,116,95,49,55,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,221,225,147,159,232,0,0,0,251,2,0,0,14,0,24,
0,0,0,0,0,0,0,0,0,164,129,106,45,0,0,114,101,115,117,108,116,95,49,56,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,179,25,152,22,232,0,0,0,253,2,0,0,14,0,24,0,0,0,0,0,0,0,0,
0,164,129,154,46,0,0,114,101,115,117,108,116,95,49,57,46,114,115,114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,202,128,4,69,207,112,51,4,231,0,0,0,219,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,202,47,
0,0,114,101,115,117,108,116,95,50,46,114,115,114,99,85,84,5,0,3,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,118,247,124,99,232,0,0,0,255,2,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,248,48,0,0,114,101,115,117,
108,116,95,50,48,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,43,89,3,94,232,0,0,0,1,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,40,50,0,0,114,101,115,117,108,116,95,50,49,46,114,
115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,16,2,104,93,230,0,0,0,3,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,88,51,0,0,114,101,115,117,108,116,95,50,50,46,114,115,114,99,85,84,5,0,
3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,228,245,119,102,232,0,0,0,5,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,134,52,0,0,114,101,115,117,108,116,95,50,51,46,114,115,114,99,85,84,5,0,3,94,147,223,83,
117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,169,150,8,204,230,0,0,0,7,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,182,53,0,0,114,101,115,117,108,116,95,50,52,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,
245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,120,2,2,181,232,0,0,0,9,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,228,54,0,0,114,101,115,117,108,116,95,50,53,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,
0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,162,111,105,169,232,0,0,0,11,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,20,56,0,0,114,101,115,117,108,116,95,50,54,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,
3,20,0,0,0,8,0,203,128,4,69,230,201,105,190,231,0,0,0,13,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,68,57,0,0,114,101,115,117,108,116,95,50,55,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,
203,128,4,69,94,136,134,120,231,0,0,0,15,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,115,58,0,0,114,101,115,117,108,116,95,50,56,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,189,
127,66,125,232,0,0,0,17,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,162,59,0,0,114,101,115,117,108,116,95,50,57,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,202,128,4,69,23,0,64,246,231,0,0,
0,221,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,210,60,0,0,114,101,115,117,108,116,95,51,46,114,115,114,99,85,84,5,0,3,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,102,251,10,93,232,0,0,0,19,3,0,0,14,0,24,
0,0,0,0,0,0,0,0,0,164,129,0,62,0,0,114,101,115,117,108,116,95,51,48,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,76,179,122,77,232,0,0,0,21,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,
164,129,48,63,0,0,114,101,115,117,108,116,95,51,49,46,114,115,114,99,85,84,5,0,3,94,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,8,221,198,230,232,0,0,0,23,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,96,64,0,0,
114,101,115,117,108,116,95,51,50,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,57,227,223,176,232,0,0,0,25,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,144,65,0,0,114,101,115,117,
108,116,95,51,51,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,158,117,209,166,232,0,0,0,27,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,192,66,0,0,114,101,115,117,108,116,95,51,
52,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,181,254,81,233,232,0,0,0,29,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,240,67,0,0,114,101,115,117,108,116,95,51,53,46,114,115,114,
99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,99,228,89,2,232,0,0,0,31,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,32,69,0,0,114,101,115,117,108,116,95,51,54,46,114,115,114,99,85,84,5,0,3,95,147,
223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,106,77,1,227,232,0,0,0,33,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,80,70,0,0,114,101,115,117,108,116,95,51,55,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,
0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,228,98,44,130,232,0,0,0,35,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,128,71,0,0,114,101,115,117,108,116,95,51,56,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,
4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,105,149,187,88,232,0,0,0,37,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,176,72,0,0,114,101,115,117,108,116,95,51,57,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,
1,2,30,3,20,0,0,0,8,0,202,128,4,69,202,7,27,88,229,0,0,0,223,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,224,73,0,0,114,101,115,117,108,116,95,52,46,114,115,114,99,85,84,5,0,3,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,
0,204,128,4,69,54,67,159,23,232,0,0,0,39,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,12,75,0,0,114,101,115,117,108,116,95,52,48,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,29,
63,119,121,232,0,0,0,41,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,60,76,0,0,114,101,115,117,108,116,95,52,49,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,184,19,87,108,232,0,
0,0,43,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,108,77,0,0,114,101,115,117,108,116,95,52,50,46,114,115,114,99,85,84,5,0,3,95,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,77,157,228,225,232,0,0,0,45,3,0,0,14,
0,24,0,0,0,0,0,0,0,0,0,164,129,156,78,0,0,114,101,115,117,108,116,95,52,51,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,222,195,228,236,232,0,0,0,47,3,0,0,14,0,24,0,0,0,0,0,
0,0,0,0,164,129,204,79,0,0,114,101,115,117,108,116,95,52,52,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,140,18,220,69,231,0,0,0,49,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,
252,80,0,0,114,101,115,117,108,116,95,52,53,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,21,143,194,71,232,0,0,0,51,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,43,82,0,0,114,101,
115,117,108,116,95,52,54,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,126,174,36,243,232,0,0,0,53,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,91,83,0,0,114,101,115,117,108,116,
95,52,55,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,224,246,116,46,233,0,0,0,55,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,139,84,0,0,114,101,115,117,108,116,95,52,56,46,114,
115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,245,209,122,114,232,0,0,0,57,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,188,85,0,0,114,101,115,117,108,116,95,52,57,46,114,115,114,99,85,
84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,202,128,4,69,132,3,129,91,227,0,0,0,225,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,236,86,0,0,114,101,115,117,108,116,95,53,46,114,115,114,99,85,84,5,0,3,92,147,223,
83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,137,161,55,110,232,0,0,0,59,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,22,88,0,0,114,101,115,117,108,116,95,53,48,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,
1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,123,37,41,49,233,0,0,0,61,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,70,89,0,0,114,101,115,117,108,116,95,53,49,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,
0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,204,128,4,69,204,206,194,167,234,0,0,0,63,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,119,90,0,0,114,101,115,117,108,116,95,53,50,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,
2,30,3,20,0,0,0,8,0,204,128,4,69,210,61,170,101,232,0,0,0,65,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,169,91,0,0,114,101,115,117,108,116,95,53,51,46,114,115,114,99,85,84,5,0,3,96,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,
8,0,205,128,4,69,13,51,216,32,233,0,0,0,67,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,217,92,0,0,114,101,115,117,108,116,95,53,52,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,
91,59,26,7,232,0,0,0,69,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,10,94,0,0,114,101,115,117,108,116,95,53,53,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,62,10,85,12,233,0,0,
0,71,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,58,95,0,0,114,101,115,117,108,116,95,53,54,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,39,70,249,247,233,0,0,0,73,3,0,0,14,0,24,
0,0,0,0,0,0,0,0,0,164,129,107,96,0,0,114,101,115,117,108,116,95,53,55,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,31,69,205,107,233,0,0,0,75,3,0,0,14,0,24,0,0,0,0,0,0,0,0,
0,164,129,156,97,0,0,114,101,115,117,108,116,95,53,56,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,129,21,26,153,233,0,0,0,77,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,205,98,
0,0,114,101,115,117,108,116,95,53,57,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,202,128,4,69,110,88,193,15,232,0,0,0,227,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,254,99,0,0,114,101,115,
117,108,116,95,54,46,114,115,114,99,85,84,5,0,3,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,243,38,14,109,233,0,0,0,79,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,45,101,0,0,114,101,115,117,108,116,95,54,48,
46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,136,161,34,101,233,0,0,0,81,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,94,102,0,0,114,101,115,117,108,116,95,54,49,46,114,115,114,
99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,142,215,14,50,233,0,0,0,83,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,143,103,0,0,114,101,115,117,108,116,95,54,50,46,114,115,114,99,85,84,5,0,3,
97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,117,133,246,137,233,0,0,0,85,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,192,104,0,0,114,101,115,117,108,116,95,54,51,46,114,115,114,99,85,84,5,0,3,97,147,223,83,
117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,153,31,52,238,233,0,0,0,87,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,241,105,0,0,114,101,115,117,108,116,95,54,52,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,
4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,86,187,225,243,233,0,0,0,89,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,34,107,0,0,114,101,115,117,108,116,95,54,53,46,114,115,114,99,85,84,5,0,3,97,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,
0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,52,226,88,52,233,0,0,0,91,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,83,108,0,0,114,101,115,117,108,116,95,54,54,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,
3,20,0,0,0,8,0,205,128,4,69,128,144,115,254,234,0,0,0,93,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,132,109,0,0,114,101,115,117,108,116,95,54,55,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,
0,205,128,4,69,117,144,110,67,234,0,0,0,95,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,182,110,0,0,114,101,115,117,108,116,95,54,56,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,
103,250,70,51,234,0,0,0,97,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,232,111,0,0,114,101,115,117,108,116,95,54,57,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,202,128,4,69,91,156,220,62,231,
0,0,0,229,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,26,113,0,0,114,101,115,117,108,116,95,55,46,114,115,114,99,85,84,5,0,3,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,31,201,99,29,234,0,0,0,99,3,0,0,14,0,
24,0,0,0,0,0,0,0,0,0,164,129,72,114,0,0,114,101,115,117,108,116,95,55,48,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,225,238,226,45,234,0,0,0,101,3,0,0,14,0,24,0,0,0,0,0,0,
0,0,0,164,129,122,115,0,0,114,101,115,117,108,116,95,55,49,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,173,150,58,56,233,0,0,0,103,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,
172,116,0,0,114,101,115,117,108,116,95,55,50,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,76,217,183,98,234,0,0,0,105,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,221,117,0,0,114,
101,115,117,108,116,95,55,51,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,220,212,42,26,234,0,0,0,107,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,15,119,0,0,114,101,115,117,108,
116,95,55,52,46,114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,238,131,18,210,234,0,0,0,109,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,65,120,0,0,114,101,115,117,108,116,95,55,53,46,
114,115,114,99,85,84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,205,128,4,69,92,1,49,232,231,0,0,0,111,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,115,121,0,0,114,101,115,117,108,116,95,55,54,46,114,115,114,99,85,
84,5,0,3,98,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,15,153,7,71,233,0,0,0,113,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,162,122,0,0,114,101,115,117,108,116,95,55,55,46,114,115,114,99,85,84,5,0,3,99,147,
223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,196,119,138,226,231,0,0,0,115,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,211,123,0,0,114,101,115,117,108,116,95,55,56,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,
11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,179,18,51,31,231,0,0,0,117,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,2,125,0,0,114,101,115,117,108,116,95,55,57,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,
0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,202,128,4,69,94,148,161,244,232,0,0,0,231,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,49,126,0,0,114,101,115,117,108,116,95,56,46,114,115,114,99,85,84,5,0,3,92,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,
1,2,30,3,20,0,0,0,8,0,206,128,4,69,56,12,166,165,234,0,0,0,119,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,96,127,0,0,114,101,115,117,108,116,95,56,48,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,
0,8,0,206,128,4,69,140,150,10,90,233,0,0,0,121,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,146,128,0,0,114,101,115,117,108,116,95,56,49,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,
4,69,26,241,151,21,233,0,0,0,123,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,195,129,0,0,114,101,115,117,108,116,95,56,50,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,162,3,21,
38,232,0,0,0,125,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,244,130,0,0,114,101,115,117,108,116,95,56,51,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,12,141,252,22,231,0,0,0,127,
3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,36,132,0,0,114,101,115,117,108,116,95,56,52,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,175,128,45,202,233,0,0,0,129,3,0,0,14,0,24,
0,0,0,0,0,0,0,0,0,164,129,83,133,0,0,114,101,115,117,108,116,95,56,53,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,122,116,4,14,233,0,0,0,131,3,0,0,14,0,24,0,0,0,0,0,0,0,0,
0,164,129,132,134,0,0,114,101,115,117,108,116,95,56,54,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,76,80,198,48,233,0,0,0,133,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,181,135,
0,0,114,101,115,117,108,116,95,56,55,46,114,115,114,99,85,84,5,0,3,99,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,12,248,192,133,233,0,0,0,135,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,230,136,0,0,114,101,115,
117,108,116,95,56,56,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,179,236,183,66,233,0,0,0,137,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,23,138,0,0,114,101,115,117,108,116,95,
56,57,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,203,128,4,69,55,86,176,127,230,0,0,0,233,2,0,0,13,0,24,0,0,0,0,0,0,0,0,0,164,129,72,139,0,0,114,101,115,117,108,116,95,57,46,114,115,
114,99,85,84,5,0,3,93,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,150,196,103,101,233,0,0,0,139,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,117,140,0,0,114,101,115,117,108,116,95,57,48,46,114,115,114,99,85,84,
5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,151,192,122,148,233,0,0,0,141,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,166,141,0,0,114,101,115,117,108,116,95,57,49,46,114,115,114,99,85,84,5,0,3,100,147,
223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,228,11,174,206,233,0,0,0,143,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,215,142,0,0,114,101,115,117,108,116,95,57,50,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,
11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,223,50,157,234,233,0,0,0,145,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,8,144,0,0,114,101,115,117,108,116,95,57,51,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,
0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,22,170,235,130,233,0,0,0,147,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,57,145,0,0,114,101,115,117,108,116,95,57,52,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,
0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,238,16,95,130,233,0,0,0,149,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,106,146,0,0,114,101,115,117,108,116,95,57,53,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,
3,20,0,0,0,8,0,206,128,4,69,228,223,165,33,232,0,0,0,151,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,155,147,0,0,114,101,115,117,108,116,95,57,54,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,
0,206,128,4,69,172,109,99,88,233,0,0,0,153,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,203,148,0,0,114,101,115,117,108,116,95,57,55,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,206,128,4,69,
82,25,142,119,233,0,0,0,155,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,252,149,0,0,114,101,115,117,108,116,95,57,56,46,114,115,114,99,85,84,5,0,3,100,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,1,2,30,3,20,0,0,0,8,0,207,128,4,69,212,116,58,233,
233,0,0,0,157,3,0,0,14,0,24,0,0,0,0,0,0,0,0,0,164,129,45,151,0,0,114,101,115,117,108,116,95,57,57,46,114,115,114,99,85,84,5,0,3,101,147,223,83,117,120,11,0,1,4,245,1,0,0,4,20,0,0,0,80,75,5,6,0,0,0,0,128,0,128,0,20,42,0,0,94,152,0,0,0,0,0,0 };

const char* RSRC_zip = (const char*) temp_binary_data_131;

//================== CtrlrIDs.xml ==================
static const unsigned char temp_binary_data_132[] =
{ 60,63,120,109,108,32,118,101,114,115,105,111,110,61,34,49,46,48,34,32,101,110,99,111,100,105,110,103,61,34,85,84,70,45,56,34,63,62,13,10,13,10,60,99,116,114,108,114,73,100,115,62,13,10,32,32,60,33,45,45,32,77,111,100,117,108,97,116,111,114,32,112,114,
111,112,101,114,116,105,101,115,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,86,97,108,117,101,34,32,9,9,9,9,9,116,101,120,116,61,34,67,117,114,114,101,110,116,32,109,111,100,117,108,97,116,111,114,32,
118,97,108,117,101,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,73,115,83,116,97,116,105,99,34,32,9,9,9,9,9,116,101,120,116,61,34,77,111,100,117,108,
97,116,111,114,32,100,111,101,115,32,110,111,116,32,114,101,97,99,116,32,116,111,32,118,97,108,117,101,32,99,104,97,110,103,101,115,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,83,116,97,116,105,99,47,68,121,110,
97,109,105,99,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,71,108,111,98,97,108,86,97,114,105,97,98,108,101,34,32,9,9,9,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,97,116,116,97,99,104,101,
100,32,116,111,32,103,108,111,98,97,108,32,118,97,114,105,97,98,108,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,49,34,32,109,97,120,61,34,54,52,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,
117,108,97,116,111,114,86,97,108,117,101,69,120,112,114,101,115,115,105,111,110,34,32,9,9,9,116,101,120,116,61,34,69,120,112,114,101,115,115,105,111,110,32,116,111,32,101,118,97,108,117,97,116,101,32,119,104,101,110,32,99,97,108,99,117,108,97,116,105,
110,103,32,116,104,101,32,109,105,100,105,32,109,101,115,115,97,103,101,32,118,97,108,117,101,32,102,114,111,109,32,116,104,101,32,109,111,100,117,108,97,116,111,114,32,118,97,108,117,101,34,32,116,121,112,101,61,34,69,120,112,114,101,115,115,105,111,
110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,86,97,108,117,101,69,120,112,114,101,115,115,105,111,110,82,101,118,101,114,115,101,34,32,9,116,101,120,116,61,34,69,120,112,114,101,115,115,105,111,110,32,
116,111,32,101,118,97,108,117,97,116,101,32,119,104,101,110,32,99,97,108,99,117,108,97,116,105,110,103,32,116,104,101,32,109,111,100,117,108,97,116,111,114,32,118,97,108,117,101,32,102,114,111,109,32,116,104,101,32,109,105,100,105,32,109,101,115,115,
97,103,101,32,118,97,108,117,101,34,32,116,121,112,101,61,34,69,120,112,114,101,115,115,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,67,111,110,116,114,111,108,108,101,114,69,120,112,114,101,115,
115,105,111,110,34,32,32,32,32,32,32,116,101,120,116,61,34,69,120,112,114,101,115,115,105,111,110,32,116,111,32,101,118,97,108,117,97,116,101,32,119,104,101,110,32,99,97,108,99,117,108,97,116,105,110,103,32,116,104,101,32,109,111,100,117,108,97,116,111,
114,32,118,97,108,117,101,32,102,114,111,109,32,109,105,100,105,32,99,111,110,116,114,111,108,108,101,114,32,109,101,115,115,97,103,101,34,32,116,121,112,101,61,34,69,120,112,114,101,115,115,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,109,111,100,117,108,97,116,111,114,77,117,116,101,79,110,83,116,97,114,116,34,32,9,9,9,9,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,100,111,101,115,32,110,111,116,32,111,117,116,112,117,116,32,97,110,121,32,77,73,68,73,32,100,117,
114,105,110,103,32,115,116,97,114,116,117,112,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,77,117,116,101,47,85,110,77,117,116,101,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,
116,111,114,77,117,116,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,100,111,101,115,32,110,111,116,32,111,117,116,112,117,116,32,97,110,121,32,77,73,68,73,34,32,116,
121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,77,117,116,101,47,85,110,77,117,116,101,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,76,105,110,107,101,100,84,111,80,97,110,
101,108,80,114,111,112,101,114,116,121,34,32,9,116,101,120,116,61,34,76,105,110,107,32,116,111,32,80,65,78,69,76,32,112,114,111,112,101,114,116,121,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,109,111,100,117,108,97,116,111,114,76,105,110,107,101,100,84,111,77,111,100,117,108,97,116,111,114,80,114,111,112,101,114,116,121,34,32,116,101,120,116,61,34,76,105,110,107,32,116,111,32,77,79,68,85,76,65,84,79,82,32,112,114,111,112,101,114,116,
121,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,76,105,110,107,101,100,84,111,77,111,100,117,108,97,116,111,114,83,111,117,114,99,101,34,32,9,116,101,
120,116,61,34,76,105,110,107,32,116,111,32,77,79,68,85,76,65,84,79,82,32,109,111,100,117,108,97,116,105,111,110,32,116,121,112,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,78,117,109,101,114,105,99,47,84,101,
120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,76,105,110,107,101,100,84,111,77,111,100,117,108,97,116,111,114,34,32,9,9,116,101,120,116,61,34,76,105,110,107,32,116,111,32,77,79,68,85,76,65,84,79,82,
34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,76,105,110,107,101,100,84,111,67,111,109,112,111,110,101,110,116,34,32,9,9,116,101,120,116,61,34,76,105,110,
107,32,116,111,32,119,104,97,116,32,116,121,112,101,32,111,102,32,77,79,68,85,76,65,84,79,82,32,112,114,111,112,101,114,116,121,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,77,111,100,117,
108,97,116,111,114,61,48,44,67,111,109,112,111,110,101,110,116,61,49,44,77,73,68,73,32,77,101,115,115,97,103,101,61,50,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,77,105,110,34,32,9,9,9,9,9,9,116,101,
120,116,61,34,77,105,110,105,109,117,109,32,118,97,108,117,101,32,116,104,101,32,109,111,100,117,108,97,116,111,114,32,99,97,110,32,104,97,118,101,44,32,115,104,111,117,108,100,32,98,101,32,115,101,116,32,102,114,111,109,32,116,104,101,32,85,73,32,99,
111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,109,111,100,117,108,97,116,111,114,77,97,120,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,120,105,109,117,109,32,118,97,108,117,101,32,116,104,101,32,109,111,100,117,108,97,116,111,
114,32,99,97,110,32,104,97,118,101,44,32,115,104,111,117,108,100,32,98,101,32,115,101,116,32,102,114,111,109,32,116,104,101,32,85,73,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,109,105,110,61,34,
45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,69,120,99,108,117,100,101,70,114,111,109,83,110,97,112,115,
104,111,116,34,32,9,9,116,101,120,116,61,34,68,111,110,39,116,32,115,101,110,100,32,116,104,105,115,32,109,111,100,117,108,97,116,111,114,32,100,117,114,105,110,103,32,115,110,97,112,115,104,111,116,115,34,32,116,121,112,101,61,34,66,111,111,108,34,32,
100,101,102,97,117,108,116,115,61,34,68,111,110,39,116,32,115,101,110,100,47,83,101,110,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,66,97,115,101,86,97,108,117,101,34,32,9,9,9,9,116,101,120,116,61,34,
66,97,115,101,47,102,97,108,108,98,97,99,107,32,118,97,108,117,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,109,111,100,117,108,97,116,111,114,67,117,115,116,111,109,73,110,100,101,120,34,32,9,9,9,9,116,101,120,116,61,34,67,117,115,116,111,109,32,109,111,100,117,108,97,116,111,114,32,105,110,100,101,120,34,32,116,121,112,101,61,34,84,101,120,116,34,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,67,117,115,116,111,109,73,110,100,101,120,71,114,111,117,112,34,32,9,9,9,116,101,120,116,61,34,67,117,115,116,111,109,32,109,111,100,117,108,97,116,111,114,32,105,110,
100,101,120,32,103,114,111,117,112,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,86,115,116,69,120,112,111,114,116,101,100,34,32,9,9,9,9,116,101,120,116,61,34,69,
120,112,111,114,116,32,112,97,114,97,109,101,116,101,114,32,116,111,32,86,83,84,32,104,111,115,116,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,120,112,111,114,116,47,68,111,110,39,116,32,101,120,112,111,114,
116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,86,115,116,78,97,109,101,70,111,114,109,97,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,32,102,111,114,109,97,116,32,115,116,114,
105,110,103,32,40,106,117,115,116,32,108,105,107,101,32,116,104,101,32,108,97,98,101,108,32,112,114,111,112,101,114,116,121,32,102,111,114,32,100,105,115,112,108,97,121,105,110,103,32,99,104,97,110,103,101,100,32,109,111,100,117,108,97,116,111,114,115,
41,32,116,104,97,116,32,119,105,108,108,32,114,101,112,111,114,116,32,116,104,101,32,110,97,109,101,32,116,111,32,116,104,101,32,72,111,115,116,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,
111,100,117,108,97,116,111,114,67,117,115,116,111,109,78,97,109,101,71,114,111,117,112,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,32,99,117,115,116,111,109,32,103,114,111,117,112,32,110,97,109,101,34,32,32,32,32,32,32,116,121,112,101,
61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,111,100,117,108,97,116,111,114,67,117,115,116,111,109,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,32,99,117,115,116,111,
109,32,109,111,100,117,108,97,116,111,114,32,110,97,109,101,34,32,32,116,121,112,101,61,34,84,101,120,116,34,32,115,101,99,116,105,111,110,61,34,77,111,100,117,108,97,116,111,114,34,32,47,62,13,10,32,13,10,32,32,60,33,45,45,32,80,97,110,101,108,32,101,
100,105,116,111,114,32,112,114,111,112,101,114,116,105,101,115,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,67,97,110,118,97,115,82,101,99,116,97,110,103,108,101,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,
34,67,97,110,118,97,115,32,98,111,117,110,100,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,69,100,105,116,77,111,100,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,101,120,116,61,34,69,100,105,116,32,77,111,100,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,76,111,99,107,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,116,101,120,116,61,34,76,111,99,107,32,97,108,108,32,99,111,109,112,111,110,101,110,116,115,32,105,110,32,112,108,97,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,117,110,108,111,99,107,101,100,
47,76,111,99,107,101,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,68,105,115,97,98,108,101,100,79,110,69,100,105,116,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,115,
32,97,114,101,32,100,105,115,97,98,108,101,100,32,119,104,101,110,32,101,100,105,116,105,110,103,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,77,101,110,117,66,97,114,86,105,115,105,98,108,101,34,32,32,32,32,32,32,116,101,120,116,61,34,73,115,32,109,101,110,117,32,98,97,114,32,118,105,115,105,98,108,101,32,102,111,114,
32,116,104,105,115,32,112,97,110,101,108,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,77,101,110,117,66,97,114,72,105,100,101,79,110,69,120,112,111,114,116,34,32,116,101,
120,116,61,34,72,105,100,101,32,116,104,101,32,109,101,110,117,32,98,97,114,32,119,104,101,110,32,101,120,112,111,114,116,105,110,103,32,97,110,32,105,110,115,116,97,110,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,80,114,111,103,114,97,109,115,77,101,110,117,72,105,100,101,79,110,69,120,112,111,114,116,34,32,116,101,120,116,61,34,72,105,100,101,32,116,104,101,32,80,114,111,103,114,97,109,115,32,109,101,110,
117,32,119,104,101,110,32,101,120,112,111,114,116,105,110,103,32,97,110,32,105,110,115,116,97,110,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,77,105,100,105,67,
111,110,116,114,111,108,108,101,114,77,101,110,117,72,105,100,101,79,110,69,120,112,111,114,116,34,32,116,101,120,116,61,34,72,105,100,101,32,116,104,101,32,77,105,100,105,32,67,111,110,116,114,111,108,108,101,114,32,109,101,110,117,32,119,104,101,110,
32,101,120,112,111,114,116,105,110,103,32,97,110,32,105,110,115,116,97,110,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,77,105,100,105,84,104,114,117,77,101,110,
117,72,105,100,101,79,110,69,120,112,111,114,116,34,32,116,101,120,116,61,34,72,105,100,101,32,116,104,101,32,77,105,100,105,32,84,104,114,117,32,109,101,110,117,32,119,104,101,110,32,101,120,112,111,114,116,105,110,103,32,97,110,32,105,110,115,116,97,
110,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,77,105,100,105,67,104,97,110,110,101,108,77,101,110,117,72,105,100,101,79,110,69,120,112,111,114,116,34,32,116,
101,120,116,61,34,72,105,100,101,32,116,104,101,32,77,105,100,105,32,67,104,97,110,110,101,108,32,109,101,110,117,32,119,104,101,110,32,101,120,112,111,114,116,105,110,103,32,97,110,32,105,110,115,116,97,110,99,101,34,32,116,121,112,101,61,34,66,111,
111,108,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,86,105,101,119,80,111,114,116,83,105,122,101,34,32,9,9,9,116,101,120,116,61,34,69,100,105,116,111,114,32,86,105,101,119,80,111,114,116,32,119,105,
100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,80,114,111,112,101,114,116,105,101,115,83,105,122,101,34,32,9,9,9,116,101,120,116,61,34,69,100,105,116,111,114,32,
112,114,111,112,101,114,116,121,32,112,97,110,101,32,119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,80,111,114,116,87,105,100,116,104,34,32,9,9,9,9,116,101,
120,116,61,34,86,105,101,119,80,111,114,116,32,99,117,114,114,101,110,116,32,119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,80,111,114,116,72,101,105,103,104,
116,34,32,9,9,9,9,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,99,117,114,114,101,110,116,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,80,
111,114,116,82,101,115,105,122,97,98,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,105,115,32,114,101,115,105,122,97,98,108,101,32,111,110,32,97,32,114,101,115,116,114,105,99,116,101,100,32,105,
110,115,116,97,110,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,82,101,115,105,122,97,98,108,101,47,76,111,99,107,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,
80,111,114,116,83,104,111,119,83,99,114,111,108,108,66,97,114,115,34,32,32,32,32,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,83,99,114,111,108,108,98,97,114,115,32,118,105,115,105,98,105,108,105,116,121,32,111,110,32,97,110,32,101,
120,112,111,114,116,101,100,32,105,110,115,116,97,110,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,86,105,115,105,98,108,101,47,72,105,100,100,101,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,86,105,101,119,80,111,114,116,69,110,97,98,108,101,82,101,115,105,122,101,76,105,109,105,116,115,34,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,114,101,115,105,122,101,32,108,105,109,105,116,115,32,101,110,97,98,108,
101,100,32,111,110,32,97,110,32,101,120,112,111,114,116,101,100,32,105,110,115,116,97,110,99,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,80,111,114,116,77,105,110,87,105,100,116,104,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,109,105,110,105,109,117,109,32,119,105,
100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,80,111,114,116,77,105,110,72,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,105,
101,119,80,111,114,116,32,109,105,110,105,109,117,109,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,80,111,114,116,77,97,120,87,105,100,116,104,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,109,97,120,105,109,117,109,32,119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,
86,105,101,119,80,111,114,116,77,97,120,72,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,109,97,120,105,109,117,109,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,84,101,120,
116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,86,105,101,119,80,111,114,116,69,110,97,98,108,101,70,105,120,101,100,65,115,112,101,99,116,82,97,116,105,111,34,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,101,110,
97,98,108,101,32,97,115,112,101,99,116,32,114,97,116,105,111,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,86,105,101,119,80,111,114,116,70,105,120,101,100,65,115,112,101,99,116,82,97,116,105,111,34,32,32,32,32,32,116,101,120,116,61,34,86,105,101,119,80,111,114,116,32,102,105,120,101,100,32,97,115,112,101,99,116,32,114,97,116,105,111,34,
32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,34,32,105,110,116,61,34,48,46,48,48,48,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,90,111,111,109,34,32,
116,101,120,116,61,34,67,97,110,118,97,115,32,122,111,111,109,32,114,97,116,105,111,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,46,48,48,49,34,32,109,97,120,61,34,49,48,46,48,48,34,32,105,110,116,61,34,48,46,48,48,
49,34,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,86,105,101,119,80,111,114,116,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,86,105,
101,119,80,111,114,116,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,66,97,99,107,103,114,111,117,
110,100,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
117,105,80,97,110,101,108,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,49,34,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,91,49,93,34,32,116,121,112,101,61,34,67,111,108,111,
117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,50,34,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,
111,117,114,32,91,50,93,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,66,97,99,107,103,114,111,117,110,100,71,114,97,100,105,101,110,116,84,121,112,101,34,32,32,
116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,103,114,97,100,105,101,110,116,32,116,121,112,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,
34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,73,109,97,103,101,76,97,121,111,117,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,109,97,103,101,32,108,97,121,111,117,116,34,32,116,121,112,
101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,105,109,97,103,101,76,97,121,111,117,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,73,109,97,103,101,82,101,115,111,117,
114,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,98,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,117,105,80,97,110,101,108,73,109,97,103,101,65,108,112,104,97,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,32,111,112,97,99,105,116,121,34,32,
116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,83,110,97,112,65,99,116,105,118,101,34,9,32,9,9,9,116,101,120,116,61,34,83,110,97,112,32,
99,111,109,112,111,110,101,110,116,32,112,111,115,105,116,105,111,110,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,83,110,97,112,83,105,122,101,34,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,110,97,112,32,115,105,122,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,67,108,105,112,98,111,97,114,100,84,114,101,101,
34,32,9,9,9,116,101,120,116,61,34,117,105,80,97,110,101,108,67,108,105,112,98,111,97,114,100,84,114,101,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,80,114,111,112,101,
114,116,105,101,115,79,110,82,105,103,104,116,34,32,9,9,116,101,120,116,61,34,80,114,111,112,101,114,116,105,101,115,32,112,97,110,101,108,32,112,111,115,105,116,105,111,110,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,
115,61,34,82,105,103,104,116,47,76,101,102,116,34,32,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,73,110,118,105,115,105,98,108,101,67,111,109,112,111,110,101,110,116,65,108,112,104,97,34,32,116,101,120,116,61,
34,65,108,112,104,97,32,102,111,114,32,105,110,118,105,115,105,98,108,101,32,99,111,109,112,111,110,101,110,116,115,32,105,110,32,101,100,105,116,32,109,111,100,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,49,34,32,
105,110,116,61,34,48,46,48,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,68,105,115,97,98,108,101,67,111,109,98,111,115,79,110,69,100,105,116,34,32,9,116,101,120,116,61,34,68,105,115,97,98,108,101,32,99,111,109,
98,111,115,32,119,104,101,110,32,101,100,105,116,105,110,103,32,112,97,110,101,108,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,84,111,111,108,116,105,112,
66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,116,101,120,116,61,34,84,111,111,108,116,105,112,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,9,9,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,
10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,84,111,111,108,116,105,112,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,9,116,101,120,116,61,34,84,111,111,108,116,105,112,32,111,117,116,108,105,110,101,32,99,111,108,111,
117,114,34,32,9,9,9,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,84,111,111,108,116,105,112,67,111,114,110,101,114,82,111,117,110,100,34,32,9,9,116,101,120,116,61,34,
84,111,111,108,116,105,112,32,114,101,99,116,97,110,103,108,101,32,114,111,117,110,100,110,101,115,115,34,32,9,9,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,50,52,34,32,105,110,116,61,34,48,46,49,
34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,84,111,111,108,116,105,112,80,108,97,99,101,109,101,110,116,34,32,9,9,116,101,120,116,61,34,84,111,111,108,116,105,112,32,112,108,97,99,101,109,101,110,116,34,32,9,9,
9,9,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,65,98,111,118,101,61,49,44,66,101,108,111,119,61,50,44,76,101,102,116,61,52,44,82,105,103,104,116,61,56,34,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,117,105,80,97,110,101,108,84,111,111,108,116,105,112,70,111,110,116,34,32,9,9,9,116,101,120,116,61,34,84,111,111,108,116,105,112,32,102,111,110,116,34,9,9,9,9,9,9,116,121,112,101,61,34,70,111,110,116,34,32,47,62,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,117,105,80,97,110,101,108,84,111,111,108,116,105,112,67,111,108,111,117,114,34,32,9,9,9,116,101,120,116,61,34,84,111,111,108,116,105,112,32,102,111,110,116,32,99,111,108,111,117,114,34,9,9,9,9,116,121,112,101,61,34,67,111,108,111,
117,114,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,76,111,111,107,65,110,100,70,101,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,
32,115,99,104,101,109,101,32,102,111,114,32,112,97,110,101,108,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,108,111,111,107,97,110,100,102,101,101,108,115,99,104,101,109,101,115,34,32,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,67,111,108,111,117,114,83,99,104,101,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,52,32,99,111,108,111,117,114,32,
115,99,104,101,109,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,99,111,108,111,117,114,83,99,104,101,109,101,115,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,
108,76,101,103,97,99,121,77,111,100,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,51,32,102,111,114,32,111,108,100,101,114,32,112,97,110,101,108,115,34,32,116,121,112,101,61,34,66,
111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,76,101,103,97,99,121,32,77,111,100,101,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,
87,105,110,100,111,119,66,97,99,107,103,114,111,117,110,100,34,32,32,32,32,116,101,120,116,61,34,87,105,110,100,111,119,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,87,105,100,103,101,116,66,97,99,107,103,114,111,117,110,100,34,32,32,32,32,116,101,120,116,61,34,87,105,100,103,101,116,32,66,97,99,107,103,114,111,
117,110,100,32,67,111,108,111,117,114,34,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,77,101,110,117,66,97,99,107,103,114,111,117,
110,100,34,32,32,32,32,32,32,116,101,120,116,61,34,77,101,110,117,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,79,117,116,108,105,110,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,
101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,68,101,102,97,117,108,116,84,101,120,116,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,101,120,
116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,68,101,102,97,
117,108,116,70,105,108,108,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,108,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,72,105,103,104,108,105,103,104,116,101,100,84,101,120,116,34,32,32,32,32,32,116,101,120,116,61,34,72,105,103,104,108,105,103,104,116,101,100,32,116,101,120,116,32,99,111,108,
111,117,114,34,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,72,105,103,104,108,105,103,104,116,101,100,70,105,108,108,34,32,32,
32,32,32,116,101,120,116,61,34,72,105,103,104,108,105,103,104,116,101,100,32,102,105,108,108,32,99,111,108,111,117,114,34,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,
101,108,85,73,67,111,108,111,117,114,77,101,110,117,84,101,120,116,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,101,110,117,32,116,101,120,116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,
108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,85,73,67,111,108,111,117,114,78,117,109,67,111,108,111,117,114,115,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,117,109,32,99,111,108,111,
117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,80,97,110,101,108,32,112,114,111,112,101,114,116,105,101,115,32,45,45,62,13,10,13,10,32,32,60,33,45,45,
32,77,73,68,73,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,73,110,112,117,116,67,104,97,110,110,101,108,68,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,
73,68,73,32,105,110,112,117,116,32,99,104,97,110,110,101,108,32,40,102,114,111,109,32,100,101,118,105,99,101,41,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,32,100,101,102,97,117,
108,116,115,61,34,64,109,105,100,105,73,110,112,117,116,67,104,97,110,110,101,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,73,110,112,117,116,68,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,105,110,112,117,116,32,100,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,84,
101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,79,117,116,112,117,116,67,104,97,110,110,101,108,68,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,
68,73,32,111,117,116,112,117,116,32,99,104,97,110,110,101,108,32,40,116,111,32,100,101,118,105,99,101,41,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,32,100,101,102,97,117,108,
116,115,61,34,64,109,105,100,105,79,117,116,112,117,116,67,104,97,110,110,101,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,79,117,116,112,117,116,68,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,111,117,116,112,117,116,32,100,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,
84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,67,111,110,116,114,111,108,108,101,114,67,104,97,110,110,101,108,68,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,
34,77,73,68,73,32,99,111,110,116,114,111,108,108,101,114,32,99,104,97,110,110,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,32,100,101,102,97,117,
108,116,115,61,34,64,109,105,100,105,73,110,112,117,116,67,104,97,110,110,101,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,67,111,110,116,114,111,108,108,101,114,68,101,118,105,99,101,34,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,99,111,110,116,114,111,108,108,101,114,32,100,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,
97,114,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,79,117,116,112,117,116,68,101,118,105,99,101,80,97,110,101,108,84,104,114,101,97,100,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,
61,34,85,115,101,32,112,97,110,101,108,32,116,104,114,101,97,100,32,102,111,114,32,109,105,100,105,32,111,117,116,112,117,116,32,40,116,111,32,100,101,118,105,99,101,41,34,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,73,110,112,117,116,70,114,111,109,72,111,115,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,105,110,112,117,116,32,102,114,111,
109,32,104,111,115,116,32,101,110,97,98,108,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,73,
110,112,117,116,70,114,111,109,72,111,115,116,67,111,109,112,97,114,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,105,110,112,117,116,32,102,114,111,109,32,104,111,115,116,32,99,111,109,112,97,114,101,100,32,116,
111,32,109,111,100,117,108,97,116,111,114,115,34,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,73,110,112,117,116,67,104,97,110,110,101,108,72,111,115,
116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,105,110,112,117,116,32,102,114,111,109,32,104,111,115,116,32,99,104,97,110,110,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,109,105,100,105,73,110,112,117,116,67,104,97,110,110,101,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,
105,79,117,116,112,117,116,84,111,72,111,115,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,111,117,116,112,117,116,32,116,111,32,104,111,115,116,32,101,110,97,98,108,101,100,34,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,79,117,116,112,117,116,67,104,97,110,110,101,108,72,111,115,116,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,111,117,116,112,117,116,32,116,111,32,104,111,115,116,32,99,104,97,110,110,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,
112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,109,105,100,105,79,117,116,112,117,116,67,104,97,110,110,101,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,
84,104,114,117,72,50,72,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,84,72,82,85,32,104,111,115,116,45,62,104,111,115,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,
77,105,100,105,84,104,114,117,72,50,72,67,104,97,110,110,101,108,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,84,72,82,85,32,104,111,115,116,45,62,104,111,115,116,32,40,99,104,97,110,110,101,108,
105,122,101,41,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,112,97,110,101,108,77,105,100,105,84,104,114,117,72,50,68,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,84,72,82,85,32,104,111,115,116,45,62,100,101,118,105,99,101,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,84,104,114,117,72,50,68,67,104,97,110,110,101,108,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,84,72,82,85,32,104,111,115,116,45,62,100,
101,118,105,99,101,32,40,99,104,97,110,110,101,108,105,122,101,41,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,
32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,84,104,114,117,68,50,68,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,84,72,82,85,32,
100,101,118,105,99,101,45,62,100,101,118,105,99,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,
98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,84,104,114,117,68,50,68,67,104,97,110,110,101,108,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,
68,73,32,84,72,82,85,32,100,101,118,105,99,101,45,62,100,101,118,105,99,101,32,40,99,104,97,110,110,101,108,105,122,101,41,34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,
98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,84,104,114,117,68,50,72,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,77,73,68,73,32,84,72,82,85,32,100,101,118,105,99,101,45,62,104,111,115,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,
61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,84,104,114,117,68,50,72,67,104,97,110,110,101,108,105,122,101,34,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,84,72,82,85,32,100,101,118,105,99,101,45,62,104,111,115,116,32,40,99,104,97,110,110,101,108,105,122,101,41,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,
32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,73,110,112,117,116,84,104,114,101,97,100,80,114,105,111,114,
105,116,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,73,110,112,117,116,32,84,104,114,101,97,100,32,112,114,105,111,114,105,116,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,105,110,116,61,34,49,34,32,109,97,120,61,34,49,48,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,80,114,111,103,114,97,109,34,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,80,114,111,103,114,97,109,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,50,55,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,66,97,110,107,76,115,98,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,66,97,110,107,32,76,83,66,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,50,55,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,66,97,110,107,77,
115,98,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,66,97,110,107,32,77,83,66,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,50,55,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,83,101,110,
100,80,114,111,103,114,97,109,67,104,97,110,103,101,79,110,76,111,97,100,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,101,110,100,32,77,73,68,73,32,112,114,111,103,114,97,109,32,97,110,100,32,98,97,110,107,32,99,104,97,110,103,101,32,
111,110,32,112,97,110,101,108,32,108,111,97,100,34,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,83,110,97,112,115,104,111,116,65,102,116,101,114,76,111,97,100,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,101,110,100,32,77,73,68,73,32,115,110,97,112,115,104,111,116,32,97,102,116,101,114,32,112,97,110,101,108,32,108,111,97,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,83,110,97,112,115,104,111,116,65,102,116,101,114,80,114,111,103,114,97,109,67,104,97,110,103,101,34,32,32,32,32,32,32,32,32,
116,101,120,116,61,34,83,101,110,100,32,77,73,68,73,32,115,110,97,112,115,104,111,116,32,97,102,116,101,114,32,112,114,111,103,114,97,109,32,99,104,97,110,103,101,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,83,110,97,112,115,104,111,116,68,101,108,97,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,115,110,97,112,115,104,111,
116,32,100,101,108,97,121,32,98,101,116,119,101,101,110,32,109,101,115,115,97,103,101,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,50,48,48,48,48,34,32,
105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,80,114,111,103,114,97,109,67,104,97,110,103,101,77,101,116,104,111,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,
34,77,101,116,104,111,100,32,102,111,114,32,112,114,111,103,114,97,109,32,99,104,97,110,103,101,32,114,101,113,117,101,115,116,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,80,114,111,103,114,97,109,67,97,108,108,111,117,116,79,110,112,114,111,103,114,97,109,67,104,97,110,103,101,34,32,32,32,32,32,116,101,120,116,61,34,83,101,110,100,32,112,114,111,
103,114,97,109,32,114,101,113,117,101,115,116,32,111,110,32,112,114,111,103,114,97,109,32,99,104,97,110,103,101,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,89,101,115,47,78,111,34,
47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,78,117,109,80,114,111,103,114,97,109,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,117,109,98,101,114,32,111,
102,32,112,114,111,103,114,97,109,115,32,112,101,114,32,98,97,110,107,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,49,50,56,34,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,78,117,109,66,97,110,107,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,117,109,98,101,114,32,111,102,32,
98,97,110,107,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,49,50,56,34,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,77,97,116,99,104,67,97,99,104,101,83,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,116,99,104,101,100,32,105,110,112,117,116,
32,109,101,115,115,97,103,101,115,32,99,97,99,104,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,50,53,54,34,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,82,101,97,108,116,105,109,101,73,103,110,111,114,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,103,110,111,114,101,32,114,101,97,108,116,105,
109,101,32,77,73,68,73,32,109,101,115,115,97,103,101,115,32,111,110,32,105,110,112,117,116,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,
105,71,108,111,98,97,108,68,101,108,97,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,71,108,111,98,97,108,32,100,101,108,97,121,32,102,111,114,32,77,73,68,73,32,109,101,115,115,97,103,101,115,32,91,
109,115,93,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,32,109,105,110,61,34,48,34,32,109,97,120,61,34,53,48,48,48,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,
105,100,105,80,97,117,115,101,79,117,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,111,32,77,73,68,73,32,111,117,116,112,117,116,32,102,114,111,109,32,112,97,110,101,108,34,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,80,97,117,115,101,73,110,34,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,111,32,77,73,68,73,32,105,110,112,117,116,32,116,111,32,112,97,110,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,
66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,83,110,97,112,115,104,111,116,83,104,111,119,80,114,111,103,114,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,
83,104,111,119,32,112,114,111,103,114,101,115,115,32,98,97,114,32,119,105,110,100,111,119,32,119,104,101,110,32,115,101,110,100,105,110,103,32,115,110,97,112,115,104,111,116,34,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,79,83,67,69,110,97,98,108,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,69,110,97,98,108,101,32,79,83,67,32,102,111,114,32,
116,104,105,115,32,112,97,110,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,79,83,67,80,111,114,116,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,83,67,32,112,111,114,116,32,116,111,32,108,105,115,116,101,110,32,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,54,53,53,51,53,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,79,83,67,80,114,111,116,111,99,
111,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,83,67,32,110,101,116,119,111,114,107,32,112,114,111,116,111,99,111,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,68,101,102,97,117,108,116,61,48,44,85,68,80,61,49,44,85,78,73,88,47,76,79,67,65,76,61,50,44,84,67,80,61,52,34,32,47,62,13,
10,13,10,32,32,60,33,45,45,32,71,101,110,101,114,97,108,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,70,105,108,101,80,97,116,104,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,
101,108,32,102,105,108,101,32,112,97,116,104,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,73,110,100,101,120,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,103,108,111,98,97,108,32,105,110,100,101,120,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,85,73,68,34,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,85,110,105,113,117,101,32,112,97,110,101,108,32,105,100,101,110,116,105,102,105,101,114,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,73,110,115,116,97,110,99,101,85,73,68,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,85,110,105,113,117,101,32,112,97,110,101,108,32,73,68,32,102,111,114,32,112,108,117,103,
105,110,32,105,110,115,116,97,110,99,101,115,32,40,52,32,99,104,97,114,97,99,116,101,114,115,41,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,73,110,115,116,97,110,99,101,77,97,
110,117,102,97,99,116,117,114,101,114,73,68,34,32,32,32,32,116,101,120,116,61,34,85,110,105,113,117,101,32,112,97,110,101,108,32,109,97,110,117,102,97,99,116,117,114,101,114,32,73,68,32,40,52,32,99,104,97,114,97,99,116,101,114,115,41,34,32,116,121,112,
101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,73,115,68,105,114,116,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,110,101,101,100,115,
32,116,111,32,98,101,32,115,97,118,101,100,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,105,100,105,86,115,116,73,110,112,117,116,34,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,114,101,99,101,105,118,101,115,32,77,73,68,73,32,102,114,111,109,32,104,111,115,116,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,
101,108,77,105,100,105,86,115,116,79,117,116,112,117,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,115,101,110,100,115,32,77,73,68,73,32,116,111,32,104,111,115,116,34,32,116,121,112,101,61,34,66,111,111,108,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,67,111,109,112,111,110,101,110,116,71,114,111,117,112,76,105,115,116,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,103,114,111,117,112,
115,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,71,108,111,98,97,108,86,97,114,105,97,98,108,101,115,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,71,108,
111,98,97,108,32,118,97,114,105,97,98,108,101,115,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,65,117,116,104,111,114,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,97,117,116,104,111,114,32,110,97,109,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,65,117,116,104,111,114,68,111,
110,97,116,101,85,114,108,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,97,117,116,104,111,114,32,100,111,110,97,116,101,32,117,114,108,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,112,97,110,101,108,65,117,116,104,111,114,69,109,97,105,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,97,117,116,104,111,114,32,101,109,97,105,108,34,32,116,121,112,101,61,34,84,101,120,
116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,65,117,116,104,111,114,85,114,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,97,117,116,104,111,114,32,117,114,108,34,
32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,65,117,116,104,111,114,68,101,115,99,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,97,117,
116,104,111,114,32,100,101,115,99,114,105,112,116,105,111,110,34,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,83,109,97,108,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,86,101,114,115,105,111,110,77,97,
106,111,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,101,114,115,105,111,110,32,77,65,74,79,82,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,50,48,52,56,34,32,105,
110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,86,101,114,115,105,111,110,77,105,110,111,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,101,114,115,105,111,110,32,77,73,78,79,
82,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,50,48,52,56,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,86,101,114,115,105,111,110,
78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,101,114,115,105,111,110,32,78,97,109,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,67,116,
114,108,114,82,101,118,105,115,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,116,114,108,114,32,114,101,118,105,115,105,111,110,32,116,104,97,116,32,116,104,105,115,32,112,97,110,101,108,32,119,97,115,32,115,97,118,101,
100,32,119,105,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,86,101,110,100,111,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,
111,110,116,114,111,108,108,101,100,32,100,101,118,105,99,101,32,98,114,97,110,100,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,68,101,118,105,99,101,34,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,118,105,99,101,32,119,104,105,99,104,32,116,104,105,115,32,112,97,110,101,108,32,99,111,110,116,114,111,108,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,112,97,110,101,108,82,101,112,108,97,99,101,86,115,116,51,80,108,117,103,105,110,73,100,115,34,32,32,32,32,32,32,116,101,120,116,61,34,86,83,84,51,32,105,100,101,110,116,105,102,105,101,114,115,34,32,116,121,112,101,61,34,66,111,
111,108,34,32,100,101,102,97,117,108,116,115,61,34,82,101,112,108,97,99,101,32,119,105,116,104,32,80,97,110,101,108,32,73,68,115,47,75,101,101,112,32,100,101,102,97,117,108,116,32,73,68,115,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,
97,110,101,108,69,120,112,111,114,116,82,101,115,111,117,114,99,101,69,110,99,114,121,112,116,105,111,110,34,32,32,116,101,120,116,61,34,69,110,99,114,121,112,116,32,101,120,112,111,114,116,101,100,32,112,97,110,101,108,34,32,116,121,112,101,61,34,66,
111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,69,120,112,111,114,116,68,101,108,97,121,66,116,119,83,116,101,
112,115,34,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,108,97,121,32,98,101,116,119,101,101,110,32,115,116,101,112,115,32,97,116,32,101,120,112,111,114,116,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,
110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,69,120,112,111,114,116,67,111,100,101,115,105,103,110,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,
111,100,101,115,105,103,110,32,101,120,112,111,114,116,101,100,32,112,97,110,101,108,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,80,108,117,103,84,121,112,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,83,84,51,32,101,120,112,111,114,116,101,100,32,105,110,115,116,97,110,99,101,32,112,
108,117,103,105,110,32,116,121,112,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,112,108,117,103,84,121,112,101,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,67,101,
114,116,105,102,105,99,97,116,101,77,97,99,83,101,108,101,99,116,73,100,34,32,32,32,32,116,101,120,116,61,34,65,112,112,108,101,32,99,101,114,116,105,102,105,99,97,116,101,32,73,68,34,32,98,117,116,116,111,110,84,101,120,116,61,34,67,108,105,99,107,32,
116,111,32,115,101,108,101,99,116,32,73,68,34,32,116,121,112,101,61,34,65,99,116,105,111,110,66,117,116,116,111,110,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,67,101,114,116,105,102,105,99,97,116,101,
77,97,99,73,100,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,112,112,108,101,32,99,101,114,116,105,102,105,99,97,116,101,32,73,68,32,115,101,108,101,99,116,101,100,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,67,101,114,116,105,102,105,99,97,116,101,87,105,110,83,101,108,101,99,116,80,97,116,104,34,32,32,116,101,120,116,61,34,87,105,110,100,111,119,115,32,80,70,88,32,99,101,114,116,105,102,105,99,
97,116,101,34,32,116,121,112,101,61,34,70,105,108,101,80,114,111,112,101,114,116,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,67,101,114,116,105,102,105,99,97,116,101,87,105,110,80,97,116,104,34,32,32,32,32,32,32,
32,32,116,101,120,116,61,34,87,105,110,100,111,119,115,32,80,70,88,32,99,101,114,116,105,102,105,99,97,116,101,32,112,97,116,104,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,
110,101,108,67,101,114,116,105,102,105,99,97,116,101,87,105,110,80,97,115,115,67,111,100,101,34,32,32,32,32,116,101,120,116,61,34,87,105,110,100,111,119,115,32,80,70,88,32,99,101,114,116,105,102,105,99,97,116,101,32,112,97,115,115,119,111,114,100,34,
32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,111,100,117,108,97,116,111,114,76,105,115,116,67,111,108,117,109,110,115,34,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,
108,97,116,111,114,32,108,105,115,116,32,99,111,108,117,109,110,32,115,116,97,116,101,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,111,100,117,108,97,116,111,114,
76,105,115,116,67,115,118,68,101,108,105,109,105,116,101,114,34,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,108,105,115,116,32,67,83,86,32,100,101,108,105,109,105,116,101,114,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,111,100,117,108,97,116,111,114,76,105,115,116,88,109,108,82,111,111,116,34,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,108,105,115,116,32,88,77,76,32,114,
111,111,116,32,101,108,101,109,101,110,116,32,110,97,109,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,111,100,117,108,97,116,111,114,76,105,115,116,88,109,108,77,111,100,
117,108,97,116,111,114,34,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,108,105,115,116,32,88,77,76,32,109,111,100,117,108,97,116,111,114,32,101,108,101,109,101,110,116,32,110,97,109,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,111,100,117,108,97,116,111,114,76,105,115,116,83,111,114,116,79,112,116,105,111,110,34,32,32,32,116,101,120,116,61,34,83,111,114,116,32,111,112,116,105,111,110,34,32,116,121,112,
101,61,34,78,117,109,101,114,105,99,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,82,101,115,111,117,114,99,101,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,114,101,
115,111,117,114,99,101,115,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,80,114,111,112,101,114,116,121,68,105,115,112,108,97,121,73,68,115,34,32,32,32,32,32,32,
32,32,116,101,120,116,61,34,68,105,115,112,108,97,121,32,112,114,111,112,101,114,116,121,32,73,68,115,32,105,110,115,116,101,97,100,32,111,102,32,102,117,108,108,32,100,101,115,99,114,105,112,116,105,111,110,115,34,32,116,121,112,101,61,34,66,111,111,
108,34,32,100,101,102,97,117,108,116,115,61,34,73,68,115,47,68,101,115,99,114,105,112,116,105,111,110,115,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,83,99,104,101,109,101,34,9,9,9,9,9,116,101,120,116,61,34,80,97,110,
101,108,32,115,99,104,101,109,101,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,83,104,111,119,68,105,97,108,111,103,115,34,9,9,9,9,116,101,120,116,61,34,83,104,
111,119,32,100,105,97,108,111,103,115,32,100,117,114,105,110,103,32,112,97,110,101,108,32,108,105,102,101,116,105,109,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,77,101,
115,115,97,103,101,84,105,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,72,111,119,32,108,111,110,103,32,109,101,115,115,97,103,101,115,32,115,104,111,117,108,100,32,97,112,112,101,97,114,32,111,110,32,115,99,114,101,101,
110,32,40,109,115,41,34,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,48,48,48,48,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,76,97,115,116,83,97,118,101,
68,105,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,105,114,101,99,116,111,114,121,32,108,97,115,116,32,117,115,101,100,32,102,111,114,32,112,97,110,101,108,32,115,97,118,101,34,32,116,121,112,101,61,34,84,101,120,116,
34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,69,120,112,111,114,116,80,117,98,108,105,99,75,101,121,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,101,103,105,115,116,114,97,116,105,111,110,32,80,117,98,
108,105,99,32,107,101,121,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,69,120,112,111,114,116,80,114,105,118,97,116,101,75,101,121,34,32,32,32,32,32,32,32,32,32,32,116,101,120,
116,61,34,82,101,103,105,115,116,114,97,116,105,111,110,32,80,114,105,118,97,116,101,32,107,101,121,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,97,110,101,108,69,120,112,111,114,116,83,101,
114,118,101,114,65,117,116,104,85,82,76,34,32,32,32,32,32,32,32,116,101,120,116,61,34,82,101,103,105,115,116,114,97,116,105,111,110,32,115,101,114,118,101,114,32,117,114,108,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,13,10,32,32,60,
33,45,45,32,71,101,110,101,114,105,99,32,99,111,109,112,111,110,101,110,116,32,112,114,111,112,101,114,116,105,101,115,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,82,101,99,116,97,110,103,108,101,34,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,112,111,115,105,116,105,111,110,32,97,110,100,32,115,105,122,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,83,101,110,116,66,97,99,107,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,105,115,32,97,116,32,116,104,101,32,
98,97,99,107,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,86,105,115,105,98,108,101,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,
116,61,34,86,105,115,105,98,108,101,32,110,97,109,101,34,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,83,109,97,108,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,77,111,117,115,101,67,117,
114,115,111,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,99,117,114,115,111,114,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,109,
111,117,115,101,67,117,114,115,111,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,71,114,111,117,112,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,71,114,
111,117,112,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,84,97,98,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,
61,34,79,119,110,101,114,32,116,97,98,115,32,110,97,109,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,84,97,98,73,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,119,110,101,114,32,116,97,98,32,105,110,100,101,120,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,
71,114,111,117,112,112,101,100,34,9,9,9,9,9,32,32,116,101,120,116,61,34,73,115,32,99,111,109,112,111,110,101,110,116,32,97,32,109,101,109,98,101,114,32,111,102,32,97,32,103,114,111,117,112,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,83,110,97,112,83,105,122,101,65,108,108,111,119,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,108,108,111,119,32,115,110,97,112,32,99,111,109,112,111,
110,101,110,116,32,115,105,122,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,83,110,97,112,83,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,116,101,120,116,61,34,83,110,97,112,32,99,111,109,112,111,110,101,110,116,32,115,105,122,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,50,53,54,34,32,105,110,116,61,34,49,34,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,73,115,76,111,99,107,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,115,105,122,101,
32,97,110,100,32,112,111,115,105,116,105,111,110,32,105,115,32,108,111,99,107,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,68,105,115,97,98,108,101,100,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,105,115,32,100,105,115,97,98,108,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,99,111,109,112,111,110,101,110,116,82,97,100,105,111,71,114,111,117,112,73,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,97,100,105,111,32,103,114,111,117,112,34,32,116,121,112,101,61,34,78,117,109,101,114,
105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,56,49,57,50,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,82,97,100,105,111,71,114,111,117,112,78,111,116,105,102,121,
77,105,100,105,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,101,110,100,32,77,73,68,73,32,102,114,111,109,32,111,116,104,101,114,32,109,101,109,98,101,114,115,32,111,102,32,116,104,101,32,103,114,111,117,112,34,32,116,121,112,101,61,34,66,111,
111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,86,105,115,105,98,105,108,105,116,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,115,32,99,111,109,112,111,110,101,
110,116,32,118,105,115,105,98,108,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,121,101,114,85,105,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,108,97,121,101,114,32,110,97,109,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,47,62,32,60,33,45,45,85,112,100,97,116,101,100,32,118,53,46,54,46,51,52,46,
32,119,97,115,32,82,101,97,100,79,110,108,121,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,65,108,112,104,97,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,
67,111,109,112,111,110,101,110,116,32,65,108,112,104,97,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,32,109,105,110,61,34,48,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,99,111,109,112,111,110,101,110,116,69,102,102,101,99,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,101,102,102,101,99,116,34,32,116,121,112,101,61,34,86,97,114,
78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,105,109,97,103,101,69,102,102,101,99,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,69,102,102,101,99,116,82,97,100,105,117,115,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,101,102,102,101,99,116,32,114,97,100,105,117,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,46,53,34,32,
109,97,120,61,34,50,53,53,34,32,105,110,116,61,34,48,46,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,69,102,102,101,99,116,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,101,102,102,101,99,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,69,
102,102,101,99,116,79,102,102,115,101,116,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,101,102,102,101,99,116,32,111,102,102,115,101,116,32,88,32,40,115,104,97,100,111,119,41,34,32,116,
121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,32,105,110,116,61,34,48,46,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,69,102,102,101,99,116,79,102,102,115,101,116,89,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,101,102,102,101,99,116,32,111,102,102,115,101,116,32,89,32,40,115,104,97,100,111,119,41,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,
32,109,97,120,61,34,50,53,53,34,32,105,110,116,61,34,48,46,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,80,111,115,105,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,78,97,109,101,32,108,97,98,101,108,32,112,111,115,105,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,114,101,108,97,116,105,118,101,80,111,115,105,116,105,111,110,34,
47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,74,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,97,109,101,32,108,97,98,101,108,32,106,
117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,72,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,97,109,101,32,108,97,98,101,108,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,84,101,
120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,87,105,100,116,104,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,97,109,101,32,108,97,98,101,108,32,
119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,86,105,115,105,98,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,78,97,109,101,32,108,97,98,101,108,32,118,105,115,105,98,108,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,65,108,119,97,121,
115,79,110,84,111,112,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,97,109,101,32,108,97,98,101,108,32,97,108,119,97,121,115,32,111,110,32,116,111,112,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,97,109,101,32,108,97,98,101,108,32,99,111,108,111,117,114,34,32,116,121,112,101,
61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,97,98,101,108,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,97,109,101,32,
108,97,98,101,108,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,82,111,116,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,114,111,116,97,116,105,111,110,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,50,34,32,105,110,116,61,34,48,46,48,49,34,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,69,120,99,108,117,100,101,100,70,114,111,109,76,97,98,101,108,68,105,115,112,108,97,121,34,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,99,
104,97,110,103,101,115,32,119,105,108,108,32,110,111,116,32,97,112,112,101,97,114,32,105,110,32,108,97,98,101,108,115,32,116,104,97,116,32,100,105,115,112,108,97,121,32,116,104,101,109,34,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,13,
10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,117,98,98,108,101,82,111,117,110,100,65,110,103,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,82,111,117,110,100,101,100,
32,99,111,114,110,101,114,32,97,110,103,108,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,52,34,32,105,110,116,61,34,48,46,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
99,111,109,112,111,110,101,110,116,66,117,98,98,108,101,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,49,34,32,32,32,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,49,
34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,117,98,98,108,101,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,50,34,32,32,32,32,32,116,101,
120,116,61,34,66,117,98,98,108,101,58,32,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,50,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,
117,98,98,108,101,66,97,99,107,103,114,111,117,110,100,71,114,97,100,105,101,110,116,84,121,112,101,34,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,66,97,99,107,103,114,111,117,110,100,32,71,114,97,100,105,101,110,116,32,83,116,121,108,101,
34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,117,98,98,108,
101,86,97,108,117,101,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,86,97,108,117,101,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,
47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,117,98,98,108,101,86,97,108,117,101,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,86,97,108,117,101,
32,116,101,120,116,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,117,98,98,108,101,86,97,108,117,101,74,117,115,116,105,102,105,99,97,116,
105,111,110,34,32,32,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,86,97,108,117,101,32,116,101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,
115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,117,98,98,108,101,78,97,109,101,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,
32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,78,97,109,101,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,
116,66,117,98,98,108,101,78,97,109,101,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,78,97,109,101,32,116,101,120,116,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,32,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,66,117,98,98,108,101,78,97,109,101,74,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,116,101,120,116,61,34,66,117,98,98,108,101,58,32,78,97,109,32,116,
101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,47,62,13,10,32,32,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,111,111,107,65,110,100,70,101,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,76,111,111,107,65,110,100,
70,101,101,108,32,116,121,112,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,99,111,109,112,111,110,101,110,116,108,111,111,107,97,110,100,102,101,101,108,34,32,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,111,111,107,65,110,100,70,101,101,108,73,115,67,117,115,116,111,109,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,76,111,111,107,65,110,100,70,101,101,
108,32,105,115,32,99,117,115,116,111,109,105,122,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,67,117,115,116,111,109,32,67,111,108,111,117,114,115,47,68,101,102,97,117,108,116,32,67,111,108,111,117,114,
115,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,86,97,108,117,101,68,101,99,105,109,97,108,80,108,97,99,101,115,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,99,105,109,97,
108,32,112,108,97,99,101,115,32,116,111,32,112,114,105,110,116,32,105,102,32,117,115,105,110,103,32,102,108,111,97,116,105,110,103,32,112,111,105,110,116,32,110,117,109,98,101,114,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,
110,61,34,48,34,32,109,97,120,61,34,49,48,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,117,97,77,111,117,115,101,77,111,118,101,100,34,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,109,111,117,115,101,32,105,115,32,109,111,118,101,100,32,111,118,101,114,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,
76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,117,97,77,111,117,115,101,68,111,119,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,
108,108,101,100,32,119,104,101,110,32,97,32,109,111,117,115,101,32,105,115,32,100,111,119,110,32,111,110,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,117,97,77,111,117,115,101,85,112,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,109,111,117,115,101,32,105,
115,32,117,112,32,111,110,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,117,97,77,
111,117,115,101,68,114,97,103,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,109,111,117,115,101,32,105,115,32,100,114,97,103,103,101,100,32,40,100,111,119,110,32,43,32,108,101,
102,116,32,99,108,105,99,107,41,32,111,118,101,114,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,
101,110,116,76,117,97,77,111,117,115,101,68,111,117,98,108,101,67,108,105,99,107,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,100,111,117,98,108,101,45,99,108,105,99,107,32,101,118,101,110,116,
32,111,99,99,117,114,115,32,111,110,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,
117,97,77,111,117,115,101,69,110,116,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,109,111,117,115,101,32,101,110,116,101,114,115,32,116,104,105,115,32,99,111,109,112,
111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,76,117,97,77,111,117,115,101,69,120,105,116,34,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,109,111,117,115,101,32,108,101,97,118,101,115,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,
100,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,73,110,116,101,114,110,97,108,70,117,110,99,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,116,101,114,
110,97,108,32,102,117,110,99,116,105,111,110,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,32,115,104,111,117,108,100,32,101,120,101,99,117,116,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,
116,115,61,34,64,105,110,116,101,114,110,97,108,70,117,110,99,116,105,111,110,115,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,83,108,105,100,101,114,32,45,45,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,
76,111,111,107,65,110,100,70,101,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,116,121,112,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,
61,34,64,99,111,109,112,111,110,101,110,116,108,111,111,107,97,110,100,102,101,101,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,76,111,111,107,65,110,100,70,101,101,108,73,115,67,117,115,116,111,109,34,
32,32,32,32,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,105,115,32,99,117,115,116,111,109,105,122,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,67,117,115,116,111,109,32,67,111,108,
111,117,114,115,47,68,101,102,97,117,108,116,32,67,111,108,111,117,114,115,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,83,116,121,108,101,34,32,9,9,9,9,9,116,101,120,116,61,34,83,108,105,100,101,114,32,115,
116,121,108,101,34,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,76,105,110,101,97,114,72,111,114,105,122,111,110,116,97,108,44,76,105,110,101,97,114,86,101,114,116,105,99,97,108,44,
76,105,110,101,97,114,66,97,114,44,76,105,110,101,97,114,66,97,114,86,101,114,116,105,99,97,108,44,82,111,116,97,114,121,44,82,111,116,97,114,121,72,111,114,105,122,111,110,116,97,108,68,114,97,103,44,82,111,116,97,114,121,86,101,114,116,105,99,97,108,
68,114,97,103,44,82,111,116,97,114,121,72,111,114,105,122,111,110,116,97,108,86,101,114,116,105,99,97,108,68,114,97,103,44,73,110,99,68,101,99,66,117,116,116,111,110,115,44,84,119,111,86,97,108,117,101,72,111,114,105,122,111,110,116,97,108,44,84,119,
111,86,97,108,117,101,86,101,114,116,105,99,97,108,44,84,104,114,101,101,86,97,108,117,101,72,111,114,105,122,111,110,116,97,108,44,84,104,114,101,101,86,97,108,117,101,86,101,114,116,105,99,97,108,34,47,62,13,10,32,32,13,10,32,32,60,33,45,45,32,85,112,
100,97,116,101,100,32,118,53,46,54,46,51,50,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,77,105,110,34,32,9,9,9,9,9,116,101,120,116,61,34,77,105,110,105,109,117,109,32,118,97,108,117,101,34,32,32,32,32,116,
121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,32,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,
101,114,77,97,120,34,32,9,9,9,9,9,116,101,120,116,61,34,77,97,120,105,109,117,109,32,118,97,108,117,101,34,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,
32,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,73,110,116,101,114,118,97,108,34,32,9,9,9,9,116,101,120,116,61,34,73,110,116,101,114,118,97,108,34,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,54,34,32,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,
114,68,101,99,105,109,97,108,80,108,97,99,101,115,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,99,105,109,97,108,32,112,108,97,99,101,34,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,
97,120,61,34,52,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,83,117,102,102,105,120,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,97,108,117,101,32,115,117,102,102,105,120,
34,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,68,111,117,98,108,101,67,108,105,99,107,86,97,108,117,101,34,32,9,9,116,101,120,116,61,34,84,
104,101,32,118,97,108,117,101,32,116,111,32,115,101,116,32,119,104,101,110,32,100,111,117,98,108,101,32,99,108,105,99,107,101,100,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,50,49,52,55,52,56,51,54,52,56,34,32,109,
97,120,61,34,50,49,52,55,52,56,51,54,52,55,34,32,105,110,116,61,34,48,46,48,48,48,48,48,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,68,111,117,98,108,101,67,108,105,99,107,69,110,97,98,108,101,
100,34,32,9,116,101,120,116,61,34,68,111,117,98,108,101,32,99,108,105,99,107,101,100,32,115,101,116,116,105,110,103,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,69,110,97,98,108,101,100,47,68,105,115,97,98,108,
101,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,80,111,115,105,116,105,111,110,34,32,9,9,9,116,101,120,116,61,34,86,97,108,117,101,32,112,111,115,105,116,105,111,110,34,32,116,121,112,101,
61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,78,111,84,101,120,116,66,111,120,61,48,44,84,101,120,116,66,111,120,76,101,102,116,61,49,44,84,101,120,116,66,111,120,82,105,103,104,116,61,50,44,84,101,120,116,66,111,
120,65,98,111,118,101,61,51,44,84,101,120,116,66,111,120,66,101,108,111,119,61,52,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,72,101,105,103,104,116,34,32,9,9,9,116,101,120,116,61,34,86,97,
108,117,101,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,87,105,100,116,104,34,32,9,9,9,116,101,120,116,61,34,86,97,108,
117,101,32,119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,84,114,97,99,107,67,111,114,110,101,114,83,105,122,101,34,32,9,9,116,101,120,116,61,34,76,
111,111,107,65,110,100,70,101,101,108,32,86,50,32,76,105,110,101,97,114,32,115,108,105,100,101,114,32,116,114,97,99,107,32,99,111,114,110,101,114,32,115,105,122,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,
109,97,120,61,34,52,56,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,67,111,114,110,101,114,83,105,122,101,34,9,9,116,101,120,116,61,34,76,111,111,107,65,110,100,70,
101,101,108,32,86,50,32,116,104,117,109,98,32,99,111,114,111,110,101,114,32,115,105,122,101,40,114,111,117,110,100,110,101,115,115,41,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,32,109,105,110,61,34,48,34,32,109,97,120,61,34,52,56,34,
32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,87,105,100,116,104,34,32,9,9,9,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,50,32,116,104,117,109,
98,32,119,105,100,116,104,32,40,48,61,100,101,102,97,117,108,116,41,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,52,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,72,101,105,103,104,116,34,32,9,9,9,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,50,32,76,105,110,101,97,114,32,115,108,105,100,101,114,32,116,104,117,109,98,32,104,
101,105,103,104,116,32,40,48,61,100,101,102,97,117,108,116,41,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,52,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,70,108,97,116,79,110,76,101,102,116,34,32,9,9,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,50,32,84,104,117,109,98,32,102,108,97,116,32,111,110,32,108,101,102,116,34,32,116,
121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,70,108,97,116,79,110,82,105,103,104,116,34,32,9,9,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,
32,86,50,32,84,104,117,109,98,32,102,108,97,116,32,111,110,32,114,105,103,104,116,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,70,108,97,116,79,110,
84,111,112,34,32,9,9,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,50,32,84,104,117,109,98,32,102,108,97,116,32,111,110,32,116,111,112,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,70,108,97,116,79,110,66,111,116,116,111,109,34,32,9,9,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,50,32,84,104,117,109,98,32,102,108,97,116,32,111,110,32,98,111,116,116,111,
109,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,84,101,120,116,67,111,108,111,117,114,34,32,9,9,116,101,120,116,61,34,86,97,108,117,101,32,116,101,
120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,66,103,67,111,108,111,117,114,34,32,9,9,9,116,101,120,116,61,
34,86,97,108,117,101,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,82,111,116,97,114,121,79,
117,116,108,105,110,101,67,111,108,111,117,114,34,32,9,116,101,120,116,61,34,82,111,116,97,114,121,32,111,117,116,108,105,110,101,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,83,108,105,100,101,114,82,111,116,97,114,121,70,105,108,108,67,111,108,111,117,114,34,32,9,9,116,101,120,116,61,34,82,111,116,97,114,121,32,102,105,108,108,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,84,104,117,109,98,67,111,108,111,117,114,34,32,9,9,9,116,101,120,116,61,34,84,104,117,109,98,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,
114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,72,105,103,104,108,105,103,104,116,67,111,108,111,117,114,34,32,9,116,101,120,116,61,34,86,97,108,117,101,32,116,101,120,116,32,99,111,108,111,
117,114,32,104,105,103,104,108,105,103,104,116,101,100,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,79,117,116,108,105,110,101,67,111,108,111,
117,114,34,32,9,116,101,120,116,61,34,86,97,108,117,101,32,111,117,116,108,105,110,101,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,
84,114,97,99,107,67,111,108,111,117,114,34,32,9,9,9,116,101,120,116,61,34,84,114,97,99,107,32,99,111,108,111,117,114,32,40,108,105,110,101,97,114,41,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,83,108,105,100,101,114,73,110,99,68,101,99,66,117,116,116,111,110,67,111,108,111,117,114,34,32,9,116,101,120,116,61,34,73,110,99,47,68,101,99,32,98,117,116,116,111,110,115,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,
34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,73,110,99,68,101,99,84,101,120,116,67,111,108,111,117,114,34,32,9,9,116,101,120,116,61,34,73,110,99,47,68,101,99,
32,98,117,116,116,111,110,115,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,70,111,110,116,
34,32,9,9,9,9,116,101,120,116,61,34,86,97,108,117,101,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,97,108,117,101,84,101,120,116,74,117,115,116,
105,102,105,99,97,116,105,111,110,34,32,116,101,120,116,61,34,86,97,108,117,101,32,116,101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,
106,117,115,116,105,102,105,99,97,116,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,101,108,111,99,105,116,121,77,111,100,101,34,32,9,9,9,116,101,120,116,61,34,86,101,108,111,99,105,116,121,32,77,
111,100,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,101,108,111,99,105,116,121,77,111,100,101,75,101,121,84,114,105,103,103,101,114,34,32,116,101,120,116,61,
34,75,101,121,98,111,97,114,100,32,116,114,105,103,103,101,114,32,45,32,118,101,108,111,99,105,116,121,32,77,111,100,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,
86,101,108,111,99,105,116,121,83,101,110,115,105,116,105,118,105,116,121,34,32,9,116,101,120,116,61,34,83,101,110,115,105,116,105,118,105,116,121,32,45,32,118,101,108,111,99,105,116,121,32,77,111,100,101,34,32,116,121,112,101,61,34,78,117,109,101,114,
105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,54,53,53,51,54,34,32,105,110,116,61,34,48,46,48,48,48,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,101,108,111,99,105,116,121,84,104,114,101,115,
104,111,108,100,34,32,9,9,116,101,120,116,61,34,84,104,114,101,115,104,111,108,100,32,45,32,118,101,108,111,99,105,116,121,32,77,111,100,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,54,53,
53,51,54,34,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,86,101,108,111,99,105,116,121,79,102,102,115,101,116,34,32,9,9,116,101,120,116,61,34,79,102,102,115,101,116,32,
105,110,32,112,105,120,101,108,115,32,45,32,118,101,108,111,99,105,116,121,32,77,111,100,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,49,50,55,34,32,105,110,116,61,34,48,46,53,34,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,117,105,83,108,105,100,101,114,83,112,114,105,110,103,77,111,100,101,34,32,9,9,9,116,101,120,116,61,34,83,108,105,100,101,114,32,115,112,114,105,110,103,32,109,111,100,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,83,112,114,105,110,103,86,97,108,117,101,34,32,9,9,9,116,101,120,116,61,34,83,108,105,100,101,114,32,115,112,114,105,110,103,32,116,111,32,118,97,108,117,101,34,32,116,121,112,101,61,
34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,32,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,77,111,
117,115,101,87,104,101,101,108,73,110,116,101,114,118,97,108,34,32,9,116,101,120,116,61,34,73,110,116,101,114,118,97,108,32,102,111,114,32,109,111,117,115,101,32,119,104,101,101,108,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,
110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,54,34,32,105,110,116,61,34,48,46,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,80,111,112,117,112,66,117,98,98,108,101,34,32,9,9,9,116,101,120,116,
61,34,68,105,115,112,108,97,121,32,112,111,112,117,112,32,98,117,98,98,108,101,32,119,104,101,110,32,100,114,97,103,103,105,110,103,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,
100,101,114,83,101,116,78,111,116,105,102,105,99,97,116,105,111,110,79,110,108,121,79,110,82,101,108,101,97,115,101,34,32,116,101,120,116,61,34,83,101,110,100,32,86,97,108,117,101,32,111,110,108,121,32,111,110,32,114,101,108,101,97,115,101,34,32,116,
121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,101,110,97,98,108,101,100,47,68,105,115,97,98,108,101,100,34,47,62,13,10,13,10,32,32,60,33,45,45,32,70,105,120,101,100,32,115,108,105,100,101,114,32,45,45,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,70,105,120,101,100,83,108,105,100,101,114,67,111,110,116,101,110,116,34,32,116,101,120,116,61,34,83,108,105,100,101,114,32,99,111,110,116,101,110,116,115,34,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,
34,47,62,13,10,13,10,32,32,60,33,45,45,32,73,109,97,103,101,32,115,108,105,100,101,114,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,83,108,105,100,101,114,82,101,115,111,117,114,99,101,34,32,116,101,120,116,61,
34,73,109,97,103,101,32,114,101,115,111,117,114,99,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,83,108,105,100,101,114,82,101,115,111,117,114,99,101,70,114,
97,109,101,87,105,100,116,104,34,32,116,101,120,116,61,34,73,109,97,103,101,32,114,101,115,111,117,114,99,101,32,102,114,97,109,101,32,119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,117,105,73,109,97,103,101,83,108,105,100,101,114,82,101,115,111,117,114,99,101,70,114,97,109,101,72,101,105,103,104,116,34,32,116,101,120,116,61,34,73,109,97,103,101,32,114,101,115,111,117,114,99,101,32,102,114,97,109,101,32,104,101,105,103,104,116,
34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,82,101,115,111,117,114,99,101,34,32,116,101,120,116,61,34,73,109,97,103,101,32,114,101,115,111,117,114,99,101,34,32,116,121,112,
101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,32,32,13,10,32,32,60,33,45,45,32,67,111,109,98,111,32,98,111,120,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,65,114,114,111,119,67,111,108,111,117,114,34,
32,116,101,120,116,61,34,65,114,114,111,119,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,79,117,116,108,105,110,101,67,111,108,111,117,114,
34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,84,101,120,116,67,111,108,111,117,114,
34,32,116,101,120,116,61,34,84,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,84,101,120,116,74,117,115,116,105,102,105,99,97,
116,105,111,110,34,32,116,101,120,116,61,34,84,101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,
111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,66,117,116,116,111,110,67,111,108,111,117,114,34,32,116,101,120,116,61,34,66,117,116,116,111,110,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,
111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,66,103,67,111,108,111,117,114,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,
108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,67,111,110,116,101,110,116,34,32,116,101,120,116,61,34,67,111,109,98,111,32,99,111,110,116,101,110,116,115,34,32,116,121,112,101,61,34,77,117,108,116,105,
76,105,110,101,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,70,111,110,116,34,32,116,101,120,116,61,34,67,111,109,98,111,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,67,111,109,98,111,77,101,110,117,70,111,110,116,34,32,116,101,120,116,61,34,67,111,109,98,111,32,109,101,110,117,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,67,111,109,98,111,77,101,110,117,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,116,101,120,116,61,34,67,111,109,98,111,32,109,101,110,117,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,
121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,77,101,110,117,70,111,110,116,67,111,108,111,117,114,34,32,116,101,120,116,61,34,67,111,109,98,111,32,109,101,110,117,32,102,111,
110,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,77,101,110,117,70,111,110,116,72,105,103,104,108,105,103,104,116,101,100,67,111,108,
111,117,114,34,32,116,101,120,116,61,34,67,111,109,98,111,32,109,101,110,117,32,104,105,103,104,108,105,103,104,116,101,100,32,102,111,110,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,67,111,109,98,111,77,101,110,117,72,105,103,104,108,105,103,104,116,67,111,108,111,117,114,34,32,116,101,120,116,61,34,67,111,109,98,111,32,109,101,110,117,32,104,105,103,104,108,105,103,104,116,32,99,111,108,111,117,114,
34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,77,101,110,117,66,97,99,107,103,114,111,117,110,100,82,105,98,98,101,100,34,32,116,101,120,116,61,34,67,111,109,98,111,
32,109,101,110,117,32,98,97,99,107,103,114,111,117,110,100,32,104,97,115,32,114,105,98,115,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,68,121,110,97,109,105,99,67,111,110,
116,101,110,116,34,32,116,101,120,116,61,34,73,110,115,116,101,97,100,32,111,102,32,99,111,110,116,101,110,116,115,32,100,105,115,112,108,97,121,32,100,121,110,97,109,105,99,32,118,97,108,117,101,115,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,
114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,100,121,110,97,109,105,99,67,111,110,116,101,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,66,117,116,116,111,110,71,114,97,100,105,101,110,116,34,32,
116,101,120,116,61,34,85,115,101,32,97,32,103,114,97,100,105,101,110,116,32,102,111,114,32,116,104,101,32,98,117,116,116,111,110,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,
111,66,117,116,116,111,110,71,114,97,100,105,101,110,116,67,111,108,111,117,114,49,34,32,116,101,120,116,61,34,67,111,109,98,111,32,98,117,116,116,111,110,32,103,114,97,100,105,101,110,116,32,99,111,108,111,117,114,49,34,32,116,121,112,101,61,34,67,111,
108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,66,117,116,116,111,110,71,114,97,100,105,101,110,116,67,111,108,111,117,114,50,34,32,116,101,120,116,61,34,67,111,109,98,111,32,98,117,116,116,111,110,32,
103,114,97,100,105,101,110,116,32,99,111,108,111,117,114,50,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,66,117,116,116,111,110,87,105,100,116,104,79,118,101,114,
114,105,100,101,34,32,116,101,120,116,61,34,79,118,101,114,114,105,100,101,32,116,104,101,32,100,101,102,97,117,108,116,32,98,117,116,116,111,110,32,119,105,100,116,104,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,117,105,67,111,109,98,111,66,117,116,116,111,110,87,105,100,116,104,34,32,116,101,120,116,61,34,66,117,116,116,111,110,32,119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,67,111,109,98,111,83,101,108,101,99,116,101,100,73,100,34,32,116,101,120,116,61,34,67,117,114,114,101,110,116,32,115,101,108,101,99,116,101,100,32,105,116,101,109,32,73,68,34,32,116,121,112,101,61,34,82,101,97,100,111,110,108,121,32,34,
32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,83,101,108,101,99,116,101,100,73,110,100,101,120,34,32,116,101,120,116,61,34,67,117,114,114,101,110,116,32,115,101,108,101,99,116,101,100,32,105,116,101,109,32,105,110,
100,101,120,34,32,116,121,112,101,61,34,82,101,97,100,111,110,108,121,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,76,105,115,116,32,98,111,120,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,67,111,110,116,
101,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,99,111,110,116,101,110,116,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,82,111,119,72,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,114,111,119,32,104,101,105,103,104,116,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,52,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
117,105,76,105,115,116,66,111,120,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,72,105,103,104,108,105,103,104,116,66,103,67,111,108,111,117,114,34,32,32,32,116,101,120,116,61,34,76,105,115,
116,66,111,120,32,115,101,108,101,99,116,101,100,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,
120,72,105,103,104,108,105,103,104,116,70,103,67,111,108,111,117,114,34,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,115,101,108,101,99,116,101,100,32,116,101,120,116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,116,121,112,101,61,
34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,84,101,120,116,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,116,101,120,
116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,70,111,110,116,34,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,102,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,76,105,115,116,66,111,120,72,105,103,104,108,105,103,104,116,70,111,110,116,34,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,104,105,103,104,108,105,103,104,116,32,102,111,110,116,34,32,32,32,32,32,32,32,32,
32,32,32,32,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,79,117,116,108,105,110,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,
111,120,32,111,117,116,108,105,110,101,32,116,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,
47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,111,117,116,108,105,110,101,32,99,111,
108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,86,83,99,114,111,108,108,66,103,67,111,108,111,117,114,34,
32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,118,101,114,116,105,99,97,108,32,115,99,114,111,108,108,98,97,114,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,32,116,121,112,101,61,34,67,111,108,111,117,
114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,86,83,99,114,111,108,108,84,104,117,109,98,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,118,101,114,116,105,99,97,108,
32,115,99,114,111,108,108,98,97,114,32,116,104,117,109,98,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,86,83,
99,114,111,108,108,84,114,97,99,107,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,118,101,114,116,105,99,97,108,32,115,99,114,111,108,108,98,97,114,32,116,114,97,99,107,32,99,111,108,111,117,114,34,32,32,32,32,32,
32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,72,83,99,114,111,108,108,66,103,67,111,108,111,117,114,34,32,32,32,32,32,116,101,120,116,61,34,76,105,115,
116,66,111,120,32,104,111,114,105,122,111,110,116,97,108,32,115,99,114,111,108,108,98,97,114,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,117,105,76,105,115,116,66,111,120,72,83,99,114,111,108,108,84,104,117,109,98,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,104,111,114,105,122,111,110,116,97,108,32,115,99,114,111,108,108,98,97,114,32,
116,104,117,109,98,32,99,111,108,111,117,114,34,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,72,83,99,114,111,108,108,84,114,97,99,107,67,111,
108,111,117,114,34,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,104,111,114,105,122,111,110,116,97,108,32,115,99,114,111,108,108,98,97,114,32,116,114,97,99,107,32,99,111,108,111,117,114,34,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,
111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,74,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,116,101,120,116,32,106,117,
115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,
34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,77,117,108,116,105,112,108,101,83,101,108,101,99,116,105,111,110,34,32,32,32,116,101,120,116,61,34,76,105,115,116,66,111,120,32,109,117,108,116,105,112,108,101,
32,115,101,108,101,99,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,73,116,101,109,67,
108,105,99,107,101,100,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,105,116,101,109,32,105,115,32,99,108,105,99,107,101,100,34,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,73,116,101,109,68,111,117,98,108,101,67,108,105,99,107,101,100,34,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,105,116,101,109,32,105,
115,32,100,111,117,98,108,101,32,99,108,105,99,107,101,100,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,73,116,101,109,68,101,108,101,116,101,
75,101,121,80,114,101,115,115,101,100,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,105,116,101,109,32,104,97,115,32,68,101,108,101,116,101,32,107,101,121,32,112,114,101,115,115,101,100,34,32,32,116,
121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,105,115,116,66,111,120,73,116,101,109,82,101,116,117,114,110,75,101,121,80,114,101,115,115,101,100,34,32,32,32,32,116,101,120,116,
61,34,67,97,108,108,101,100,32,119,104,101,110,32,105,116,101,109,32,104,97,115,32,82,101,116,117,114,110,32,107,101,121,32,112,114,101,115,115,101,100,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,13,10,32,32,60,33,45,
45,32,70,105,108,101,32,108,105,115,116,32,98,111,120,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,66,103,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,
105,108,101,76,105,115,116,66,111,120,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,
76,105,110,101,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,108,105,110,101,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,
114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,73,110,100,101,110,116,83,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,105,110,100,
101,110,116,32,115,105,122,101,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,117,105,70,105,108,101,76,105,115,116,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,102,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,
101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,84,101,120,116,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,
120,32,116,101,120,116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,72,105,103,104,108,105,103,104,116,
84,101,120,116,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,104,105,103,104,108,105,103,104,116,32,99,111,108,111,117,114,34,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,72,105,103,104,108,105,103,104,116,66,103,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,104,105,103,104,108,105,103,104,116,
32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,72,105,103,104,108,105,
103,104,116,70,111,110,116,34,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,104,105,103,104,108,105,103,104,116,32,102,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,70,111,110,116,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,79,117,116,108,105,110,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,111,117,116,
108,105,110,101,32,116,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,111,117,116,108,105,110,101,32,99,
111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,86,83,99,114,111,108,108,66,103,67,111,
108,111,117,114,34,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,118,101,114,116,105,99,97,108,32,115,99,114,111,108,108,98,97,114,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,32,116,121,
112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,86,83,99,114,111,108,108,84,104,117,109,98,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,70,105,108,
101,76,105,115,116,66,111,120,32,118,101,114,116,105,99,97,108,32,115,99,114,111,108,108,98,97,114,32,116,104,117,109,98,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,86,83,99,114,111,108,108,84,114,97,99,107,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,118,101,114,116,105,99,97,108,32,115,
99,114,111,108,108,98,97,114,32,116,114,97,99,107,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,
72,83,99,114,111,108,108,66,103,67,111,108,111,117,114,34,32,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,104,111,114,105,122,111,110,116,97,108,32,115,99,114,111,108,108,98,97,114,32,98,97,99,107,103,114,111,117,110,
100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,72,83,99,114,111,108,108,84,104,117,109,98,67,111,108,111,117,114,
34,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,104,111,114,105,122,111,110,116,97,108,32,115,99,114,111,108,108,98,97,114,32,116,104,117,109,98,32,99,111,108,111,117,114,34,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,
111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,66,111,120,72,83,99,114,111,108,108,84,114,97,99,107,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,
120,32,104,111,114,105,122,111,110,116,97,108,32,115,99,114,111,108,108,98,97,114,32,116,114,97,99,107,32,99,111,108,111,117,114,34,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
117,105,70,105,108,101,76,105,115,116,79,112,101,110,66,117,116,116,111,110,86,105,115,105,98,108,101,34,32,32,32,32,116,101,120,116,61,34,70,105,108,101,76,105,115,116,66,111,120,32,105,115,32,111,112,101,110,32,98,117,116,116,111,110,32,118,105,115,
105,98,108,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,70,105,108,101,67,108,105,99,107,101,100,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,
97,108,108,101,100,32,119,104,101,110,32,97,32,102,105,108,101,32,105,115,32,99,108,105,99,107,101,100,34,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,
105,115,116,70,105,108,101,68,111,117,98,108,101,67,108,105,99,107,101,100,34,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,102,105,108,101,32,105,115,32,100,111,117,98,108,101,32,99,108,105,99,107,101,100,34,32,
116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,60,33,45,45,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,73,116,101,109,68,101,108,101,116,101,75,101,121,80,114,101,115,115,101,100,34,32,
116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,102,105,108,101,32,104,97,115,32,68,101,108,101,116,101,32,107,101,121,32,112,114,101,115,115,101,100,34,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,101,76,105,115,116,73,116,101,109,82,101,116,117,114,110,75,101,121,80,114,101,115,115,101,100,34,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,102,105,108,
101,32,104,97,115,32,82,101,116,117,114,110,32,107,101,121,32,112,114,101,115,115,101,100,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,108,
101,76,105,115,116,67,117,114,114,101,110,116,82,111,111,116,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,111,111,116,32,100,105,114,101,99,116,111,114,121,32,98,114,111,119,115,105,110,103,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,
84,101,120,116,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,76,97,98,101,108,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,66,103,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,84,101,120,116,67,111,108,111,117,114,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,79,117,
116,108,105,110,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,97,98,101,108,32,111,117,116,108,105,110,101,32,116,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,
109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,32,32,
32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,70,105,116,70,
111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,70,105,116,32,102,111,110,116,32,115,105,122,101,32,116,111,32,108,97,98,101,108,32,115,105,122,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,
102,97,117,108,116,115,61,34,89,101,115,47,78,111,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,97,98,101,
108,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,84,101,120,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,
34,76,97,98,101,108,32,116,101,120,116,34,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,68,105,115,112,108,97,121,115,65,108,108,86,97,108,117,101,115,34,32,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,97,98,101,108,32,100,105,115,112,108,97,121,115,32,108,97,115,116,32,99,104,97,110,103,101,100,32,109,111,100,117,108,97,116,111,114,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,68,105,115,112,108,97,121,70,111,114,109,97,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,87,104,101,110,32,100,105,115,112,108,97,121,105,110,103,32,108,97,115,116,32,99,104,
97,110,103,101,100,32,109,111,100,117,108,97,116,111,114,44,32,117,115,101,32,116,104,105,115,32,102,111,114,109,97,116,34,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,83,109,97,108,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,76,97,98,101,108,74,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,
84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,69,100,105,116,79,110,83,105,110,103,108,101,67,108,105,99,
107,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,69,100,105,116,105,110,103,32,98,101,103,105,110,115,32,111,110,32,111,110,101,32,99,108,105,99,107,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,117,105,76,97,98,101,108,69,100,105,116,79,110,68,111,117,98,108,101,67,108,105,99,107,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,69,100,105,116,105,110,103,32,98,101,103,105,110,115,32,111,110,32,100,111,117,98,108,101,32,99,108,105,99,107,
34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,69,100,105,116,70,111,99,117,115,68,105,115,99,97,114,100,115,67,104,97,110,103,101,115,34,32,32,116,101,120,116,61,34,73,102,
32,116,114,117,101,44,32,99,108,105,99,107,32,111,117,116,115,105,100,101,32,108,97,98,101,108,32,100,105,115,99,97,114,100,115,32,99,104,97,110,103,101,115,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,76,97,98,101,108,67,104,97,110,103,101,100,67,98,107,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,116,104,101,32,108,97,98,101,108,32,99,111,110,116,101,110,116,115,
32,99,104,97,110,103,101,115,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,73,110,112,117,116,65,108,108,111,119,101,100,67,104,97,114,115,34,32,32,32,
32,32,32,32,32,32,116,101,120,116,61,34,65,108,108,111,119,101,100,32,99,104,97,114,97,99,116,101,114,115,32,116,104,101,32,117,115,101,114,32,99,97,110,32,105,110,112,117,116,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,73,110,112,117,116,77,97,120,76,101,110,103,116,104,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,120,105,109,117,109,32,108,101,110,103,116,104,32,111,102,32,116,104,101,32,115,
116,114,105,110,103,32,97,32,117,115,101,114,32,99,97,110,32,105,110,112,117,116,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,56,49,57,50,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,73,110,112,117,116,72,105,103,104,108,105,103,104,116,84,101,120,116,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,69,100,105,116,105,110,103,58,32,115,101,108,101,99,116,101,100,32,99,
111,117,108,111,117,114,32,116,101,120,116,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,73,110,112,117,116,72,105,103,104,108,105,103,104,116,67,111,108,111,117,
114,34,32,32,32,32,32,32,116,101,120,116,61,34,69,100,105,116,105,110,103,58,32,115,101,108,101,99,116,105,111,110,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,76,67,68,32,108,
97,98,101,108,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,67,68,76,97,98,101,108,70,111,110,116,34,32,116,101,120,116,61,34,76,67,68,32,70,111,110,116,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,
101,102,97,117,108,116,115,61,34,64,108,99,100,70,111,110,116,115,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,67,68,76,97,98,101,108,70,111,110,116,72,101,105,103,104,116,34,32,116,101,120,116,61,34,76,67,68,32,70,111,110,116,32,
115,105,122,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,53,49,50,34,32,105,110,116,61,34,49,34,47,62,13,10,13,10,32,32,60,33,45,45,32,71,114,111,117,112,32,45,45,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,71,114,111,117,112,79,117,116,108,105,110,101,67,111,108,111,117,114,49,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,32,91,49,93,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,
47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,79,117,116,108,105,110,101,67,111,108,111,117,114,50,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,32,91,50,93,34,32,116,121,112,101,61,
34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,79,117,116,108,105,110,101,71,114,97,100,105,101,110,116,84,121,112,101,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,71,114,97,100,
105,101,110,116,32,83,116,121,108,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,
111,117,112,79,117,116,108,105,110,101,84,104,105,99,107,110,101,115,115,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,116,104,105,99,107,110,101,115,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,
32,109,97,120,61,34,50,53,53,34,32,105,110,116,61,34,48,46,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,79,117,116,108,105,110,101,82,111,117,110,100,65,110,103,108,101,34,32,116,101,120,116,61,34,82,111,117,110,
100,101,100,32,99,111,114,101,110,101,114,32,97,110,103,108,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,50,53,53,34,32,105,110,116,61,34,48,46,49,34,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,117,105,71,114,111,117,112,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,49,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,91,49,93,34,32,116,121,112,101,61,34,67,111,108,111,
117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,50,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,91,
50,93,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,66,97,99,107,103,114,111,117,110,100,71,114,97,100,105,101,110,116,84,121,112,101,34,32,116,101,120,116,61,34,
66,97,99,107,103,114,111,117,110,100,32,71,114,97,100,105,101,110,116,32,83,116,121,108,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,84,101,120,116,67,111,108,111,117,114,34,32,116,101,120,116,61,34,84,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,84,101,120,116,80,108,97,99,101,109,101,110,116,34,32,116,101,120,116,61,34,84,101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,
34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,84,101,120,116,70,111,110,116,34,32,116,101,120,116,61,34,84,101,120,116,
32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,84,101,120,116,34,32,116,101,120,116,61,34,84,101,120,116,34,32,116,121,112,101,61,34,77,117,108,116,105,
76,105,110,101,83,109,97,108,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,84,101,120,116,77,97,114,103,105,110,34,32,116,101,120,116,61,34,84,101,120,116,32,77,97,114,103,105,110,34,32,116,121,112,101,61,34,78,
117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,66,97,99,107,103,114,111,117,110,100,73,109,97,103,101,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,
110,100,32,105,109,97,103,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,66,97,99,107,103,114,111,117,110,100,73,109,97,103,101,76,97,121,111,117,116,34,32,116,
101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,32,108,97,121,111,117,116,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,105,109,97,103,101,76,97,121,111,117,
116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,66,97,99,107,103,114,111,117,110,100,73,109,97,103,101,65,108,112,104,97,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,32,
97,108,112,104,97,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,13,10,32,32,60,33,45,45,32,84,111,103,103,108,101,32,98,117,116,116,111,110,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,117,105,84,111,103,103,108,101,66,117,116,116,111,110,84,101,120,116,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,101,120,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,117,105,84,111,103,103,108,101,66,117,116,116,111,110,116,105,99,107,67,111,108,111,117,114,34,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,105,99,107,32,109,97,114,107,32,99,111,108,111,117,114,34,32,116,121,112,101,61,
34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,111,103,103,108,101,66,117,116,116,111,110,70,111,99,117,115,79,117,116,108,105,110,101,34,32,116,101,120,116,61,34,66,117,116,116,111,110,32,111,117,116,
108,105,110,101,32,119,104,101,110,32,102,111,99,117,115,101,100,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,84,101,120,116,32,98,117,116,116,111,110,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,66,117,116,116,111,110,76,111,111,107,65,110,100,70,101,101,108,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,76,111,111,107,65,110,100,70,101,101,108,32,116,121,112,101,34,32,116,121,112,101,61,34,86,97,
114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,99,111,109,112,111,110,101,110,116,108,111,111,107,97,110,100,102,101,101,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,76,111,111,107,65,
110,100,70,101,101,108,73,115,67,117,115,116,111,109,34,32,116,101,120,116,61,34,66,117,116,116,111,110,32,76,111,111,107,65,110,100,70,101,101,108,32,105,115,32,99,117,115,116,111,109,105,122,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,32,
100,101,102,97,117,108,116,115,61,34,67,117,115,116,111,109,32,67,111,108,111,117,114,115,47,68,101,102,97,117,108,116,32,67,111,108,111,117,114,115,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,84,101,120,
116,67,111,108,111,117,114,79,102,102,34,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,101,120,116,32,99,111,108,111,117,114,32,91,111,102,102,93,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,84,101,120,116,67,111,108,111,117,114,79,110,34,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,101,120,116,32,99,111,108,111,117,114,32,91,111,110,93,34,32,116,121,112,101,
61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,84,101,120,116,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,101,120,116,32,102,
111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,84,101,120,116,74,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,116,101,120,116,61,34,66,117,116,116,
111,110,32,116,101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,34,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,67,111,108,111,117,114,79,102,102,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,91,
111,102,102,93,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,67,111,108,111,117,114,79,110,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,
116,111,110,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,91,111,110,93,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,67,111,110,116,101,
110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,118,97,108,117,101,115,34,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,
116,116,111,110,73,115,84,111,103,103,108,101,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,105,115,32,116,111,103,103,108,101,32,98,117,116,116,111,110,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,73,115,77,111,109,101,110,116,97,114,121,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,105,115,32,97,32,109,111,109,101,110,116,97,114,121,32,98,117,116,
116,111,109,32,40,115,112,114,105,110,103,115,32,98,97,99,107,32,116,111,32,48,41,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,84,114,117,101,86,97,108,117,101,34,32,
32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,97,108,117,101,32,102,111,114,32,79,78,32,115,116,97,116,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,
32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,70,97,108,115,101,86,97,108,117,101,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,97,108,117,101,32,102,111,114,32,79,70,70,32,
115,116,97,116,101,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,
117,116,116,111,110,67,111,110,110,101,99,116,101,100,76,101,102,116,34,32,32,32,32,32,32,116,101,120,116,61,34,67,111,110,110,101,99,116,101,100,32,76,101,102,116,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,66,117,116,116,111,110,67,111,110,110,101,99,116,101,100,82,105,103,104,116,34,32,32,32,32,32,116,101,120,116,61,34,67,111,110,110,101,99,116,101,100,32,82,105,103,104,116,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,67,111,110,110,101,99,116,101,100,84,111,112,34,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,110,110,101,99,116,101,100,32,84,111,112,34,32,116,121,112,101,61,34,66,111,111,108,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,67,111,110,110,101,99,116,101,100,66,111,116,116,111,109,34,32,32,32,32,116,101,120,116,61,34,67,111,110,110,101,99,116,101,100,32,66,111,116,116,111,109,34,32,116,
121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,82,101,112,101,97,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,75,101,101,112,105,110,103,32,98,117,116,116,
111,110,32,79,78,32,119,105,108,108,32,112,114,111,100,117,99,101,32,97,32,99,111,110,116,105,110,117,101,115,32,115,116,114,101,97,109,32,111,102,32,109,101,115,115,97,103,101,115,46,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,82,101,112,101,97,116,82,97,116,101,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,97,116,101,32,97,116,32,119,105,116,104,32,116,104,101,32,109,101,115,115,97,103,101,115,32,119,
105,108,108,32,98,101,32,115,101,110,116,44,32,105,110,32,109,105,108,108,105,115,101,99,111,110,100,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,48,48,48,34,32,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,84,114,105,103,103,101,114,79,110,77,111,117,115,101,68,111,119,110,34,32,116,101,120,116,61,34,84,114,105,103,103,101,114,32,98,117,116,116,111,110,32,111,110,32,109,111,117,115,101,32,100,
111,119,110,32,101,118,101,110,116,115,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,73,109,97,103,101,32,98,117,116,116,111,110,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,
101,66,117,116,116,111,110,82,101,115,111,117,114,99,101,34,32,116,101,120,116,61,34,73,109,97,103,101,32,114,101,115,111,117,114,99,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,73,109,97,103,101,66,117,116,116,111,110,84,101,120,116,67,111,108,111,117,114,34,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,66,117,116,116,111,110,84,101,120,116,87,105,100,116,104,34,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,101,120,116,32,119,105,100,116,104,32,114,101,99,116,97,110,103,108,101,34,
32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,66,117,116,116,111,110,84,101,120,116,72,101,105,103,104,116,34,32,116,101,120,116,61,34,66,117,116,116,111,110,32,116,101,120,116,
32,104,101,105,103,104,116,32,114,101,99,116,97,110,103,108,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,66,117,116,116,111,110,67,111,110,116,101,110,116,34,32,116,101,
120,116,61,34,66,117,116,116,111,110,32,118,97,108,117,101,115,34,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,66,117,116,116,111,110,73,109,97,103,101,87,
105,100,116,104,34,32,116,101,120,116,61,34,82,101,115,111,117,114,99,101,32,105,109,97,103,101,32,119,105,100,116,104,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,66,117,
116,116,111,110,73,109,97,103,101,72,101,105,103,104,116,34,32,116,101,120,116,61,34,82,101,115,111,117,114,99,101,32,105,109,97,103,101,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,73,109,97,103,101,66,117,116,116,111,110,84,101,120,116,80,111,115,105,116,105,111,110,34,32,116,101,120,116,61,34,84,101,120,116,32,112,111,115,105,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,
32,100,101,102,97,117,108,116,115,61,34,64,114,101,108,97,116,105,118,101,80,111,115,105,116,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,66,117,116,116,111,110,77,111,100,101,34,32,116,101,120,116,61,
34,66,117,116,116,111,110,32,109,111,100,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,78,111,114,109,97,108,61,48,44,77,111,109,101,110,116,97,114,121,61,49,44,78,111,114,109,97,108,43,
77,111,117,115,101,79,118,101,114,61,50,44,77,111,109,101,110,116,97,114,121,43,77,111,117,115,101,79,118,101,114,61,51,34,47,62,13,10,13,10,32,32,60,33,45,45,32,77,73,68,73,32,75,101,121,98,111,97,114,100,32,45,45,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,79,114,105,101,110,116,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,75,101,121,98,111,97,114,100,32,79,114,105,101,110,116,97,116,105,111,110,34,32,32,
32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,104,111,114,105,122,111,110,116,97,108,75,101,121,98,111,97,114,100,44,118,101,114,116,105,99,97,108,75,101,121,98,111,97,114,100,
70,97,99,105,110,103,76,101,102,116,44,118,101,114,116,105,99,97,108,75,101,121,98,111,97,114,100,70,97,99,105,110,103,82,105,103,104,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,87,104,
105,116,101,66,117,116,116,111,110,67,111,108,111,117,114,34,32,32,32,32,32,32,116,101,120,116,61,34,87,104,105,116,101,32,98,117,116,116,111,110,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,66,108,97,99,107,66,117,116,116,111,110,67,111,108,111,117,114,34,32,32,32,32,32,32,116,101,120,116,61,34,66,108,97,99,107,32,98,117,116,116,111,
110,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,83,101,112,97,114,97,116,111,114,76,
105,110,101,67,111,108,111,117,114,34,32,32,32,32,116,101,120,116,61,34,83,101,112,97,114,97,116,111,114,32,108,105,110,101,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,77,111,117,115,101,79,118,101,114,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,111,118,101,114,32,99,111,108,111,117,114,
34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,77,111,117,115,101,68,111,119,110,67,111,108,111,117,114,34,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,100,111,119,110,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
117,105,77,105,100,105,75,101,121,98,111,97,114,100,84,101,120,116,76,97,98,101,108,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,101,120,116,32,108,97,98,101,108,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,66,117,116,116,111,110,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,116,
101,120,116,61,34,66,117,116,116,111,110,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,
101,121,98,111,97,114,100,66,117,116,116,111,110,65,114,114,111,119,67,111,108,111,117,114,34,32,32,32,32,32,32,116,101,120,116,61,34,66,117,116,116,111,110,32,97,114,114,111,119,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,116,121,112,
101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,76,111,119,101,115,116,86,105,115,105,98,108,101,75,101,121,34,32,32,32,32,32,32,32,116,101,120,116,61,34,70,
105,114,115,116,32,107,101,121,32,115,104,111,119,110,32,105,110,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,
111,97,114,100,66,97,115,101,79,99,116,97,118,101,75,101,121,80,114,101,115,115,34,32,32,32,32,32,116,101,120,116,61,34,75,101,121,32,112,114,101,115,115,32,102,111,114,32,109,105,100,100,108,101,32,111,99,116,97,118,101,34,32,32,116,121,112,101,61,34,
78,117,109,101,114,105,99,34,32,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,34,32,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,79,99,116,97,118,101,70,
114,111,77,105,100,100,108,101,67,34,32,32,32,32,32,32,32,116,101,120,116,61,34,79,99,116,97,118,101,32,110,117,109,98,101,114,32,102,111,114,32,109,105,100,100,108,101,32,67,34,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,32,109,
105,110,61,34,45,49,48,34,32,109,97,120,61,34,49,48,34,32,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,77,97,112,84,111,78,111,116,101,78,117,109,98,101,114,34,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,112,32,109,111,100,117,108,97,116,111,114,32,118,97,108,117,101,32,116,111,32,110,111,116,101,32,110,117,109,98,101,114,32,105,110,115,116,101,97,100,32,111,102,32,118,101,108,111,99,105,116,121,34,
32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,79,99,116,97,118,101,75,101,121,68,111,119,110,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,
34,75,101,121,32,102,111,114,32,111,99,116,97,118,101,32,100,111,119,110,34,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,77,105,100,105,75,101,121,98,111,97,114,100,79,99,116,97,118,
101,75,101,121,85,112,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,75,101,121,32,102,111,114,32,111,99,116,97,118,101,32,117,112,34,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,13,10,32,32,60,33,45,45,32,84,
97,98,115,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,78,117,109,98,101,114,79,102,84,97,98,115,34,32,116,101,120,116,61,34,78,117,109,98,101,114,32,111,102,32,116,97,98,115,34,32,116,121,112,101,61,34,78,117,109,101,
114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,54,52,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,79,114,105,101,110,116,97,116,105,111,110,34,32,116,101,120,116,61,34,84,97,98,
115,32,111,114,105,101,110,116,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,84,97,98,115,65,116,84,111,112,61,48,44,84,97,98,115,65,116,66,111,116,116,111,109,61,49,44,84,
97,98,115,65,116,76,101,102,61,50,44,84,97,98,115,65,116,82,105,103,104,116,61,51,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,68,101,112,116,104,34,32,116,101,120,116,61,34,84,97,98,115,32,100,101,112,116,104,34,32,116,
121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,50,52,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,79,117,116,108,105,110,101,84,104,105,
99,107,110,101,115,115,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,116,104,105,99,107,110,101,115,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,
61,34,49,48,50,52,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,70,114,111,110,116,84,97,98,79,117,116,108,105,110,101,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,116,104,105,
99,107,110,101,115,115,32,40,99,117,114,114,101,110,116,32,116,97,98,41,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,50,52,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,79,117,116,108,105,110,101,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,116,104,105,99,107,110,101,115,115,32,40,111,116,104,101,114,32,116,97,98,41,34,32,116,121,112,101,61,34,78,117,
109,101,114,105,99,34,32,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,50,52,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,73,110,100,101,110,116,84,104,105,99,107,110,101,115,115,34,
32,116,101,120,116,61,34,71,97,112,32,97,114,111,117,110,100,32,116,104,101,32,99,111,110,116,101,110,116,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,
48,50,52,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,65,100,100,84,97,98,34,32,116,101,120,116,61,34,65,100,100,32,116,97,98,34,32,116,121,112,101,61,34,65,99,116,105,111,110,66,117,116,116,
111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,82,101,109,111,118,101,84,97,98,34,32,116,101,120,116,61,34,82,101,109,111,118,101,32,116,97,98,34,32,116,121,112,101,61,34,65,99,116,105,111,110,66,117,116,116,111,
110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,67,117,114,114,101,110,116,84,97,98,34,32,116,101,120,116,61,34,67,117,114,114,101,110,116,32,97,99,116,105,118,101,32,116,97,98,34,32,116,121,112,101,61,34,78,117,109,101,
114,105,99,34,32,109,97,120,61,34,50,53,54,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,79,117,116,108,105,110,101,71,108,111,98,97,108,67,111,108,111,117,114,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,
111,108,111,117,114,32,102,111,114,32,101,110,116,105,114,101,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,79,117,116,108,105,110,
101,71,108,111,98,97,108,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,98,101,104,105,110,100,32,116,97,98,115,34,32,116,121,112,101,61,34,67,
111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,67,117,114,114,101,110,116,84,97,98,67,104,97,110,103,101,100,34,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,116,104,101,32,99,117,
114,114,101,110,116,32,116,97,98,32,99,104,97,110,103,101,115,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,70,114,111,110,116,84,97,98,70,111,110,116,34,32,116,
101,120,116,61,34,84,97,98,32,116,101,120,116,32,102,111,110,116,32,40,99,117,114,114,101,110,116,41,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,70,111,110,116,34,32,
116,101,120,116,61,34,84,97,98,32,116,101,120,116,32,102,111,110,116,32,40,111,116,104,101,114,41,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,79,117,116,108,105,110,101,84,97,
98,67,111,108,111,117,114,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,32,102,111,114,32,116,97,98,115,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,84,97,98,115,84,101,120,116,84,97,98,67,111,108,111,117,114,34,32,116,101,120,116,61,34,84,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,
97,98,115,70,114,111,110,116,84,97,98,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,32,111,102,32,116,104,101,32,115,101,108,101,99,116,101,100,32,116,97,98,34,32,116,
121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,70,114,111,110,116,84,97,98,84,101,120,116,67,111,108,111,117,114,34,32,116,101,120,116,61,34,84,101,120,116,32,99,111,108,111,117,114,
32,111,102,32,116,104,101,32,115,101,108,101,99,116,101,100,32,116,97,98,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,34,32,116,101,120,116,61,34,117,105,84,97,
98,115,84,97,98,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,78,97,109,101,34,32,116,101,120,116,61,34,84,97,98,32,110,97,109,101,34,32,116,121,112,101,61,34,84,101,120,
116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,73,110,100,101,120,34,32,116,101,120,116,61,34,84,97,98,32,105,110,100,101,120,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,
34,32,109,97,120,61,34,49,48,50,52,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,67,111,110,116,101,110,116,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,116,101,
120,116,61,34,67,111,110,116,101,110,116,32,98,97,99,107,103,114,111,117,110,100,32,67,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,66,97,
99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,116,101,120,116,61,34,84,97,98,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,117,105,84,97,98,115,84,97,98,66,97,99,107,103,114,111,117,110,100,73,109,97,103,101,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,
10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,66,97,99,107,103,114,111,117,110,100,73,109,97,103,101,76,97,121,111,117,116,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,32,108,97,121,
111,117,116,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,105,109,97,103,101,76,97,121,111,117,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,84,97,98,66,
97,99,107,103,114,111,117,110,100,73,109,97,103,101,65,108,112,104,97,34,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,32,97,108,112,104,97,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,
61,34,48,34,32,105,110,116,61,34,49,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,13,10,32,32,60,33,45,45,32,65,114,114,111,119,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,65,114,114,111,119,67,111,108,111,117,114,34,32,116,101,
120,116,61,34,65,114,114,111,119,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,65,114,114,111,119,76,105,110,101,84,104,105,99,107,110,101,115,115,34,32,116,
101,120,116,61,34,65,114,114,111,119,32,108,105,110,101,32,116,104,105,99,107,110,101,115,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,32,47,62,13,
10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,65,114,114,111,119,72,101,97,100,87,105,100,116,104,34,32,116,101,120,116,61,34,65,114,114,111,119,32,104,101,97,100,32,119,105,100,116,104,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,
109,105,110,61,34,48,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,65,114,114,111,119,72,101,97,100,72,101,105,103,104,116,34,32,116,101,120,116,61,34,65,114,114,111,119,
32,104,101,97,100,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,52,48,57,54,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,65,114,114,111,119,79,114,105,101,110,116,97,116,105,111,110,34,32,116,101,120,116,61,34,65,114,114,111,119,32,111,114,105,101,110,116,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,
116,115,61,34,80,111,105,110,116,105,110,103,84,111,112,61,48,44,80,111,105,110,116,105,110,103,66,111,116,116,111,109,61,49,44,80,111,105,110,116,105,110,103,76,101,102,116,61,50,44,80,111,105,110,116,105,103,82,105,103,104,116,61,51,34,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,117,105,65,114,114,111,119,82,111,116,97,116,105,111,110,34,32,116,101,120,116,61,34,65,114,114,111,119,32,114,111,116,97,116,105,111,110,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,
61,34,48,34,32,109,97,120,61,34,51,54,48,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,65,114,114,111,119,83,116,111,107,101,84,104,105,99,107,110,101,115,115,34,32,116,101,120,116,61,34,65,114,114,111,
119,32,115,116,114,111,107,101,32,116,104,105,99,107,110,101,115,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,50,53,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,13,10,32,32,60,33,45,45,
32,67,117,115,116,111,109,32,99,111,109,112,111,110,101,110,116,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,82,101,115,105,122,101,100,67,97,108,108,98,97,99,107,34,32,116,101,120,116,61,34,82,101,115,105,
122,101,100,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,80,97,105,110,116,67,97,108,108,98,97,99,107,34,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,105,110,116,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,80,
97,105,110,116,79,118,101,114,67,104,105,108,100,114,101,110,67,97,108,108,98,97,99,107,34,32,32,116,101,120,116,61,34,80,97,105,110,116,32,111,118,101,114,32,99,104,105,108,100,114,101,110,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,
117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,68,111,119,110,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,
32,100,111,119,110,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,85,112,67,97,108,108,98,97,99,107,
34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,117,112,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,
115,116,111,109,77,111,117,115,101,68,111,117,98,108,101,67,108,105,99,107,67,97,108,108,98,97,99,107,34,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,100,111,117,98,108,101,32,99,108,105,99,107,32,99,97,108,108,98,97,99,107,34,32,116,121,112,
101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,69,110,116,101,114,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,
111,117,115,101,32,101,110,116,101,114,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,69,120,105,116,
67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,101,120,105,116,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,68,114,97,103,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,68,114,97,103,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,
111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,77,111,118,101,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,109,111,118,101,32,
99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,87,104,101,101,108,77,111,118,101,67,97,108,108,98,97,99,
107,34,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,119,104,101,101,108,32,109,111,118,101,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
117,105,67,117,115,116,111,109,75,101,121,68,111,119,110,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,75,101,121,32,100,111,119,110,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,
116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,75,101,121,83,116,97,116,101,67,104,97,110,103,101,100,67,97,108,108,98,97,99,107,34,32,32,32,32,116,101,120,116,61,34,75,101,121,32,115,116,97,116,
101,32,99,104,97,110,103,101,100,32,99,97,108,108,98,97,99,107,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,77,111,117,115,101,68,111,119,110,71,
114,97,98,115,70,111,99,117,115,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,117,115,101,32,100,111,119,110,32,103,114,97,98,115,32,107,101,121,98,111,97,114,100,32,102,111,99,117,115,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,87,97,110,116,115,75,101,121,98,111,97,114,100,70,111,99,117,115,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,104,105,115,32,99,111,109,112,111,110,101,110,116,32,
110,101,101,100,115,32,107,101,121,98,111,97,114,100,32,102,111,99,117,115,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,83,101,116,84,101,120,116,34,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,115,101,116,32,116,104,101,32,99,111,109,112,111,110,101,110,116,115,32,116,101,120,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,
34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,115,116,111,109,71,101,116,84,101,120,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,103,101,116,32,
116,104,101,32,99,111,109,112,111,110,101,110,116,115,32,116,101,120,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,83,101,116,86,97,108,117,101,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,115,101,116,32,116,104,101,32,99,111,109,112,111,110,101,110,116,115,32,118,97,108,117,101,34,32,116,121,112,101,61,34,76,117,97,77,
101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,71,101,116,86,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,
111,32,103,101,116,32,116,104,101,32,99,111,109,112,111,110,101,110,116,115,32,118,97,108,117,101,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,68,
114,97,103,65,110,100,68,114,111,112,84,97,114,103,101,116,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,115,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,32,97,32,100,114,97,103,32,97,110,100,32,100,114,111,112,32,116,97,114,103,
101,116,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,68,114,97,103,65,110,100,68,114,111,112,67,111,110,116,97,105,110,101,114,34,32,32,32,32,32,32,32,116,101,120,
116,61,34,73,115,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,32,97,32,100,114,97,103,32,97,110,100,32,100,114,111,112,32,99,111,110,116,97,105,110,101,114,40,115,111,117,114,99,101,41,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,68,114,97,119,68,114,97,103,73,109,97,103,101,87,104,101,110,79,118,101,114,34,32,32,32,32,32,32,116,101,120,116,61,34,83,104,111,117,108,100,32,97,32,100,114,97,103,32,97,
110,100,32,100,114,111,112,32,105,109,97,103,101,32,98,101,32,100,114,97,119,110,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,65,108,108,111,119,69,120,116,101,114,
110,97,108,68,114,97,103,115,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,104,111,117,108,100,32,119,101,32,97,108,108,111,119,32,100,114,97,103,32,97,110,100,32,100,114,111,112,32,101,118,101,110,116,115,32,102,114,111,109,32,111,117,116,115,
105,100,101,32,67,116,114,108,114,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,83,116,97,114,116,68,114,97,103,103,105,110,103,67,97,108,108,98,97,99,107,34,32,32,
32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,100,114,97,103,103,105,110,103,32,115,116,97,114,116,115,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,117,105,67,117,115,116,111,109,73,115,73,110,116,101,114,101,115,116,101,100,73,110,68,114,97,103,83,111,117,114,99,101,67,97,108,108,98,97,99,107,34,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,99,104,101,99,107,32,105,102,32,
116,104,101,32,99,111,109,112,111,110,101,110,116,32,105,115,32,105,110,116,101,114,101,115,116,101,100,32,105,110,32,116,104,101,32,100,114,97,103,32,101,118,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,73,116,101,109,68,114,97,103,69,110,116,101,114,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,100,114,97,103,103,
105,110,103,32,101,110,116,101,114,115,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,73,
116,101,109,68,114,97,103,77,111,118,101,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,100,114,97,103,103,105,110,103,32,104,97,112,112,101,110,101,100,32,111,118,101,114,32,116,104,
105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,73,116,101,109,68,114,97,103,69,120,105,116,67,97,108,
108,98,97,99,107,34,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,100,114,97,103,103,105,110,103,32,101,120,105,116,115,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,
97,77,101,116,104,111,100,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,73,116,101,109,68,114,111,112,112,101,100,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,
32,119,104,101,110,32,97,110,32,105,116,101,109,32,105,115,32,100,114,111,112,112,101,100,32,111,110,32,116,104,105,115,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,13,10,32,32,60,
33,45,45,32,87,97,118,101,102,111,114,109,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,102,111,114,109,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,87,97,118,101,102,111,114,109,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,
87,97,118,101,102,111,114,109,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,49,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,91,49,93,34,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,102,111,114,109,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,50,34,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,91,50,93,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,
10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,102,111,114,109,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,
111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,102,111,114,109,83,111,117,114,99,101,
83,97,109,112,108,101,115,80,101,114,84,104,117,109,98,110,97,105,108,83,97,109,112,108,101,34,32,32,116,101,120,116,61,34,87,104,101,110,32,99,114,101,97,116,105,110,103,32,97,32,115,116,111,114,101,100,44,32,108,111,119,45,114,101,115,32,118,101,114,
115,105,111,110,32,111,102,32,116,104,101,32,97,117,100,105,111,32,100,97,116,97,44,32,116,104,105,115,32,105,115,32,116,104,101,32,115,99,97,108,101,32,97,116,32,119,104,105,99,104,32,105,116,32,115,104,111,117,108,100,32,98,101,32,100,111,110,101,46,
32,40,84,104,105,115,32,110,117,109,98,101,114,32,105,115,32,116,104,101,32,110,117,109,98,101,114,32,111,102,32,111,114,105,103,105,110,97,108,32,115,97,109,112,108,101,115,32,116,104,97,116,32,119,105,108,108,32,98,101,32,97,118,101,114,97,103,101,
100,32,102,111,114,32,101,97,99,104,32,108,111,119,45,114,101,115,32,115,97,109,112,108,101,41,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,52,48,57,54,34,32,109,105,110,61,34,48,34,32,105,110,116,61,34,49,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,70,111,114,109,83,111,117,114,99,101,67,104,97,110,103,101,100,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,104,
101,110,32,116,104,101,32,115,111,117,114,99,101,32,111,102,32,116,104,101,32,97,117,100,105,111,32,100,97,116,97,32,104,97,115,32,99,104,97,110,103,101,100,44,32,111,114,32,114,101,115,101,116,40,41,32,104,97,115,32,98,101,101,110,32,99,97,108,108,101,
100,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,102,111,114,109,79,117,116,108,105,110,101,84,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,116,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,
34,32,32,109,97,120,61,34,49,50,56,34,32,32,32,105,110,116,61,34,48,46,53,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,70,111,114,109,68,114,97,119,83,101,99,111,110,100,115,83,116,97,114,116,34,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,32,115,101,99,111,110,100,115,32,119,104,101,114,101,32,116,111,32,115,116,97,114,116,32,100,114,97,119,105,110,103,34,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,
110,61,34,48,34,32,32,109,97,120,61,34,51,54,48,48,34,32,32,105,110,116,61,34,48,46,48,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,70,111,114,109,68,114,97,119,83,101,99,111,110,100,115,69,110,100,34,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,32,115,101,99,111,110,100,115,32,119,104,101,114,101,32,116,111,32,101,110,100,32,100,114,97,119,105,110,103,34,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,
114,105,99,34,32,109,105,110,61,34,45,49,34,32,109,97,120,61,34,51,54,48,48,34,32,32,105,110,116,61,34,48,46,48,48,48,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,70,111,114,109,86,101,114,105,116,99,97,108,90,
111,111,109,70,97,99,116,111,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,101,114,116,105,99,97,108,32,122,111,111,109,32,102,97,99,116,111,114,32,40,49,46,48,32,61,61,32,110,111,32,122,111,111,109,41,34,32,116,121,112,
101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,32,109,97,120,61,34,50,53,53,46,48,34,32,105,110,116,61,34,48,46,48,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,70,111,114,109,84,104,117,109,
98,110,97,105,108,67,104,97,110,103,101,100,67,97,108,108,98,97,99,107,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,116,104,101,32,116,104,117,109,98,110,97,105,108,32,99,104,97,110,103,101,115,34,32,
32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,87,97,118,101,70,111,114,109,70,105,108,101,115,68,114,111,112,112,101,100,67,97,108,108,98,97,99,107,34,32,32,32,32,32,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,98,97,99,107,32,116,111,32,105,110,100,105,99,97,116,101,32,116,104,97,116,32,116,104,101,32,117,115,101,114,32,104,97,115,32,100,114,111,112,112,101,100,32,116,104,101,32,102,105,108,101,115,
32,111,110,116,111,32,116,104,105,115,32,112,97,110,101,108,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,72,121,112,101,114,108,105,110,103,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,72,121,112,101,114,108,105,110,107,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,72,121,112,101,114,108,105,110,107,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,
108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,72,121,112,101,114,108,105,110,107,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,72,121,112,101,114,108,105,110,107,32,116,101,120,116,
32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,72,121,112,101,114,108,105,110,107,70,105,116,84,101,120,116,84,111,83,105,122,101,34,32,32,32,32,32,116,101,120,116,
61,34,70,105,116,32,116,101,120,116,32,116,111,32,115,105,122,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,72,121,112,101,114,108,105,110,107,84,101,120,116,74,117,115,116,105,102,105,
99,97,116,105,111,110,34,32,116,101,120,116,61,34,84,101,120,116,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,
116,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,72,121,112,101,114,108,105,110,107,79,112,101,110,115,85,114,108,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,104,111,117,108,100,32,116,104,101,32,98,117,116,
116,111,110,32,97,99,116,117,97,108,108,121,32,111,112,101,110,32,116,104,101,32,119,101,98,32,98,114,111,119,115,101,114,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,72,121,112,101,114,
108,105,110,107,85,114,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,72,121,112,101,114,108,105,110,107,32,85,82,76,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,13,10,32,32,60,33,45,45,32,88,89,32,83,117,114,102,
97,99,101,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,66,103,71,114,97,100,105,101,110,116,84,121,112,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,
114,111,117,110,100,32,103,114,97,100,105,101,110,116,32,116,121,112,101,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,49,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,
32,99,111,108,111,117,114,49,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,66,97,99,107,103,114,111,117,
110,100,67,111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,
111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,79,117,116,108,105,110,101,71,114,97,100,105,101,110,116,84,121,112,101,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,
105,110,101,32,103,114,97,100,105,101,110,116,32,116,121,112,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,79,117,116,108,105,110,101,67,111,108,111,117,114,49,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,
108,111,117,114,49,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,79,117,116,108,105,110,101,67,
111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,
117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,79,117,116,108,105,110,101,84,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,
110,101,32,116,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,
89,83,117,114,102,97,99,101,67,111,114,110,101,114,83,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,114,110,101,114,32,115,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,66,103,73,109,97,103,101,82,101,115,111,117,114,99,101,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,97,110,101,108,32,98,97,99,107,103,114,111,117,110,100,32,105,109,97,103,101,32,114,101,115,111,117,114,99,101,34,32,32,32,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,66,103,73,109,97,103,101,76,97,121,111,117,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,109,97,103,101,32,108,97,121,111,117,116,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,105,109,97,103,101,76,97,121,111,117,116,34,47,62,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,66,103,73,109,97,103,101,65,108,112,104,97,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,109,97,103,101,32,97,108,112,104,97,34,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,88,84,114,97,99,107,69,110,97,
98,108,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,88,47,89,32,116,114,97,99,107,105,110,103,32,101,110,97,98,108,101,100,34,32,32,32,32,32,116,121,112,101,61,34,66,111,111,
108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,88,84,114,97,99,107,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,88,32,84,114,97,99,107,105,110,
103,32,97,120,105,115,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,88,84,114,97,99,107,
84,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,88,32,84,114,97,99,107,105,110,103,32,97,120,105,115,32,116,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,
78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,89,84,114,97,99,107,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,116,101,120,116,61,34,89,32,84,114,97,99,107,105,110,103,32,97,120,105,115,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,88,89,83,117,114,102,97,99,101,89,84,114,97,99,107,84,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,89,32,84,114,97,99,107,105,110,103,32,97,120,105,115,32,116,104,105,99,107,110,101,115,115,
34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,83,101,99,116,105,111,110,76,111,
99,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,87,104,101,114,101,32,116,111,32,112,117,116,32,116,104,101,32,109,111,100,117,108,97,116,111,114,115,32,99,111,109,98,111,32,116,111,112,47,98,111,116,116,111,109,34,
32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,83,101,99,116,105,111,110,72,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,
116,61,34,77,111,100,117,108,97,116,111,114,115,32,99,111,109,98,111,32,104,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,73,110,102,111,76,97,98,101,108,86,105,115,105,98,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,102,111,32,108,97,98,101,108,32,118,105,115,105,98,108,101,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,73,110,102,111,76,97,98,101,108,76,111,99,97,116,105,111,110,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,102,111,32,108,97,98,101,108,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,
97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,97,116,105,111,110,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,73,110,102,111,76,97,98,101,108,67,111,108,111,117,114,34,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,102,111,32,108,97,98,101,108,32,116,101,120,116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,73,110,102,111,76,97,98,101,108,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,102,111,32,108,97,98,101,108,32,102,111,110,116,34,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,70,111,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,66,103,71,114,97,100,105,101,110,116,84,
121,112,101,34,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,98,97,99,107,103,114,111,117,110,100,32,103,114,97,100,105,101,110,116,32,116,121,112,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,
32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,66,103,67,111,108,111,117,114,49,34,32,32,32,32,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,49,34,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,66,103,67,111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,98,97,99,107,103,114,111,
117,110,100,32,99,111,108,111,117,114,50,34,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,79,117,116,
108,105,110,101,71,114,97,100,105,101,110,116,84,121,112,101,34,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,111,117,116,108,105,110,101,32,103,114,97,100,105,101,110,116,32,116,121,112,101,34,32,32,32,32,116,121,112,101,61,34,86,
97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,79,117,116,
108,105,110,101,67,111,108,111,117,114,49,34,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,111,117,116,108,105,110,101,32,99,111,108,111,117,114,49,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,
117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,79,117,116,108,105,110,101,67,111,108,111,117,114,50,34,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,
108,97,116,111,114,32,111,117,116,108,105,110,101,32,99,111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,
77,111,100,117,108,97,116,111,114,79,117,116,108,105,110,101,84,104,105,99,107,110,101,115,115,34,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,111,117,116,108,105,110,101,32,116,104,105,99,107,110,101,115,115,34,32,32,32,
32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,87,105,100,116,104,34,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,119,105,100,116,104,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,
34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,111,100,117,108,97,116,111,114,72,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,100,117,108,
97,116,111,114,32,104,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,
83,117,114,102,97,99,101,77,97,120,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,120,32,118,97,108,117,101,32,102,111,114,32,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,54,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,68,101,115,116,105,110,
97,116,105,111,110,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,115,116,105,110,97,116,105,111,110,32,109,111,100,117,108,97,116,111,114,32,102,111,114,32,88,32,97,120,105,115,34,32,32,32,116,121,112,101,61,
34,77,111,100,117,108,97,116,111,114,76,105,115,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,77,97,120,89,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,
116,61,34,77,97,120,32,118,97,108,117,101,32,102,111,114,32,89,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,54,34,32,109,97,120,61,34,54,53,53,51,
54,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,68,101,115,116,105,110,97,116,105,111,110,89,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,115,116,105,110,97,116,
105,111,110,32,109,111,100,117,108,97,116,111,114,32,102,111,114,32,89,32,97,120,105,115,34,32,32,32,116,121,112,101,61,34,77,111,100,117,108,97,116,111,114,76,105,115,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,
102,97,99,101,71,114,97,100,105,101,110,116,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,117,108,101,114,115,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,71,114,97,100,105,101,110,116,71,114,97,105,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,82,117,108,101,114,115,32,100,101,110,115,105,116,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,97,120,61,34,50,53,53,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,88,89,83,117,114,97,99,101,88,70,108,105,112,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,101,118,101,114,115,101,32,116,104,101,32,118,97,108,117,101,32,112,
114,111,112,97,103,97,116,105,111,110,32,111,110,32,88,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,97,99,101,89,70,108,105,112,34,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,101,118,101,114,115,101,32,116,104,101,32,118,97,108,117,101,32,112,114,111,112,97,103,97,116,105,111,110,32,111,110,32,89,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,97,99,101,83,104,111,119,82,105,103,104,116,67,108,105,99,107,77,101,110,117,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,104,111,119,32,97,32,109,111,100,117,108,97,
116,111,114,32,115,101,108,101,99,116,105,111,110,32,109,101,110,117,34,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,68,101,115,116,105,110,97,116,105,
111,110,88,71,114,111,117,112,70,105,108,116,101,114,34,32,32,32,32,32,32,32,116,101,120,116,61,34,85,115,101,32,111,110,108,121,32,109,111,100,117,108,97,116,111,114,115,32,102,111,114,32,88,32,97,120,105,115,32,119,105,116,104,32,116,104,105,115,32,
99,117,115,116,111,109,32,103,114,111,117,112,32,110,97,109,101,34,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,68,101,115,116,105,110,97,116,105,111,110,
89,71,114,111,117,112,70,105,108,116,101,114,34,32,32,32,32,32,32,32,116,101,120,116,61,34,85,115,101,32,111,110,108,121,32,109,111,100,117,108,97,116,111,114,115,32,102,111,114,32,89,32,97,120,105,115,32,119,105,116,104,32,116,104,105,115,32,99,117,
115,116,111,109,32,103,114,111,117,112,32,110,97,109,101,34,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,69,110,118,101,108,111,112,101,32,112,114,111,112,101,114,116,105,101,115,32,45,45,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,83,116,97,116,101,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,69,110,118,101,108,111,112,101,32,115,116,97,116,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,65,100,100,80,111,105,110,116,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,100,100,32,112,
111,105,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,65,99,116,105,111,110,66,117,116,116,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,
111,112,101,82,101,109,111,118,101,80,111,105,110,116,34,32,32,32,32,32,32,116,101,120,116,61,34,82,101,109,111,118,101,32,112,111,105,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,65,99,116,105,
111,110,66,117,116,116,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,66,103,71,114,97,100,105,101,110,116,84,121,112,101,34,32,32,32,116,101,120,116,61,34,71,114,97,100,105,101,110,116,32,98,
97,99,107,103,114,111,117,110,100,32,116,121,112,101,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,32,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,66,103,67,111,108,111,117,114,49,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,71,114,97,100,105,101,110,116,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,
49,34,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,66,103,67,111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,
71,114,97,100,105,101,110,116,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,32,50,34,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,
108,111,112,101,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,32,32,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,
108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,79,117,116,108,105,110,101,84,104,105,99,107,110,101,115,115,34,32,116,101,120,116,61,34,79,117,116,108,105,110,101,32,116,104,105,99,107,
110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,50,56,34,32,105,110,116,61,34,48,46,49,34,32,47,62,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,105,110,101,67,111,108,111,117,114,34,32,32,32,32,32,32,32,116,101,120,116,61,34,76,105,110,101,32,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,105,110,101,84,104,105,99,107,110,101,115,115,34,32,32,32,32,116,101,120,116,61,34,76,105,110,101,
32,116,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,50,56,34,32,105,110,116,61,34,48,46,49,34,32,
47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,73,110,118,101,114,116,89,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,118,101,114,116,32,89,32,97,120,105,115,32,119,104,101,110,32,99,97,108,
99,117,108,97,116,105,110,103,32,118,97,108,117,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,83,116,114,97,105,103,104,116,47,73,110,118,101,114,116,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,69,110,118,101,108,111,112,101,68,114,97,119,80,111,105,110,116,66,111,117,110,100,115,34,32,32,116,101,120,116,61,34,68,114,97,119,32,112,111,105,110,116,32,98,111,117,110,100,114,105,101,115,34,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,105,110,101,70,105,108,108,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,69,110,118,101,108,111,112,
101,32,97,114,101,97,32,102,105,108,108,32,116,121,112,101,34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,103,114,97,100,105,101,110,116,34,32,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,105,110,101,70,105,108,108,67,111,108,111,117,114,49,34,32,32,116,101,120,116,61,34,69,110,118,101,108,111,112,101,32,97,114,101,97,32,102,105,108,108,32,99,111,108,111,
117,114,49,34,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,105,110,101,70,105,108,108,67,111,108,111,117,114,50,34,32,32,116,
101,120,116,61,34,69,110,118,101,108,111,112,101,32,97,114,101,97,32,102,105,108,108,32,99,111,108,111,117,114,50,34,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,87,105,100,116,104,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,119,105,100,116,104,32,40,48,61,61,119,105,100,116,104,32,111,102,32,101,110,118,
41,34,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,
112,101,76,101,103,101,110,100,72,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,104,101,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,
34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,86,105,115,
105,98,108,101,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,118,105,115,105,98,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,66,103,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,98,97,99,107,103,114,111,117,110,100,32,99,111,
108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,67,111,108,111,117,114,34,32,32,
32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,102,111,110,116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,102,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,70,111,110,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,74,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,
116,101,120,116,61,34,76,101,103,101,110,100,32,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,106,117,115,116,105,102,105,99,
97,116,105,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,
32,111,117,116,108,105,110,101,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,
79,118,101,114,108,97,121,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,105,115,32,111,118,101,114,32,116,104,101,32,101,110,118,32,108,105,110,101,115,34,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,
32,100,101,102,97,117,108,116,115,61,34,79,118,101,114,32,101,110,118,101,108,111,112,101,32,108,105,110,101,115,47,85,110,100,101,114,32,101,110,118,101,108,111,112,101,32,108,105,110,101,115,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
117,105,69,110,118,101,108,111,112,101,76,101,103,101,110,100,70,111,114,109,97,116,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,103,101,110,100,32,102,111,114,109,97,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,34,32,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,67,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,101,120,116,61,34,80,111,105,110,116,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,69,110,118,101,108,111,112,101,80,111,105,110,116,67,111,114,110,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,111,117,110,100,101,100,32,99,111,114,110,101,114,32,115,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,
80,111,105,110,116,73,110,100,101,120,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,111,105,110,116,32,105,110,100,101,120,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,82,101,97,100,79,110,
108,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,111,105,110,116,32,110,97,109,101,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,80,111,115,105,116,105,
111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,111,105,110,116,32,112,111,115,105,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,97,98,101,108,70,111,114,109,97,116,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,111,105,110,116,32,108,97,98,101,108,32,102,111,114,109,97,116,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,97,98,101,108,67,111,108,111,117,
114,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,111,105,110,116,32,108,97,98,101,108,32,99,111,108,111,117,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,97,98,101,108,70,111,110,116,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,111,105,110,116,32,108,97,98,101,108,32,102,111,110,116,34,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,116,121,112,101,61,34,70,111,110,116,34,32,32,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,97,98,101,108,66,103,67,111,108,111,117,114,34,32,32,32,32,32,32,32,
116,101,120,116,61,34,80,111,105,110,116,32,108,97,98,101,108,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,
110,118,101,108,111,112,101,80,111,105,110,116,76,97,98,101,108,79,117,116,108,105,110,101,67,111,108,111,117,114,34,32,32,116,101,120,116,61,34,80,111,105,110,116,32,108,97,98,101,108,32,111,117,116,108,105,110,101,32,99,111,108,111,117,114,34,32,32,
32,32,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,97,98,101,108,86,105,115,105,98,108,101,34,32,32,32,32,32,32,32,32,116,101,120,
116,61,34,80,111,105,110,116,32,108,97,98,101,108,32,118,105,115,105,98,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,
101,80,111,105,110,116,83,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,111,105,110,116,32,115,105,122,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,
114,105,99,34,32,109,105,110,61,34,52,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,111,99,107,89,34,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,89,32,97,120,105,115,32,105,115,32,108,111,99,107,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,111,99,107,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,88,32,97,120,105,115,32,105,115,32,108,111,99,107,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,77,97,120,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,
120,32,88,32,112,111,115,105,116,105,111,110,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,77,105,110,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,105,110,32,88,32,112,111,115,105,116,105,111,110,34,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,69,110,118,101,108,111,112,101,80,111,105,110,116,77,97,120,88,86,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,120,32,88,32,118,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,
112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,77,
97,120,89,86,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,77,97,120,32,89,32,118,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,
61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,105,110,107,88,34,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,101,120,116,61,34,88,32,97,120,105,115,32,108,105,110,107,101,100,32,116,111,32,109,111,100,117,108,97,116,111,114,34,32,32,32,32,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,69,110,118,101,108,111,112,101,80,111,105,110,116,76,105,110,107,89,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,89,32,97,120,105,115,32,108,105,110,107,101,100,32,116,111,32,109,111,100,117,108,97,116,111,114,34,32,32,32,
32,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,69,120,112,114,101,115,115,105,111,110,88,34,32,32,32,32,32,32,32,32,32,116,101,120,
116,61,34,69,120,112,114,101,115,115,105,111,110,32,116,111,32,99,97,108,99,117,108,97,116,101,32,88,32,99,111,111,114,100,105,110,97,116,101,32,119,104,101,110,32,111,116,104,101,114,32,112,111,105,110,116,115,32,99,104,97,110,103,101,34,32,32,116,121,
112,101,61,34,69,120,112,114,101,115,115,105,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,69,120,112,114,101,115,115,105,111,110,89,34,32,32,32,32,32,32,32,32,32,116,101,
120,116,61,34,69,120,112,114,101,115,115,105,111,110,32,116,111,32,99,97,108,99,117,108,97,116,101,32,89,32,99,111,111,114,100,105,110,97,116,101,32,119,104,101,110,32,111,116,104,101,114,32,112,111,105,110,116,115,32,99,104,97,110,103,101,34,32,32,116,
121,112,101,61,34,69,120,112,114,101,115,115,105,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,69,120,112,114,101,115,115,105,111,110,86,97,108,117,101,88,34,32,32,32,32,116,
101,120,116,61,34,69,120,112,114,101,115,115,105,111,110,32,116,111,32,99,97,108,99,117,108,97,116,101,32,88,32,108,105,110,107,101,100,32,118,97,108,117,101,34,32,32,116,121,112,101,61,34,69,120,112,114,101,115,115,105,111,110,34,32,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,105,110,116,69,120,112,114,101,115,115,105,111,110,86,97,108,117,101,89,34,32,32,32,32,116,101,120,116,61,34,69,120,112,114,101,115,115,105,111,110,32,116,111,32,99,97,108,
99,117,108,97,116,101,32,89,32,108,105,110,107,101,100,32,118,97,108,117,101,34,32,32,116,121,112,101,61,34,69,120,112,114,101,115,115,105,111,110,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,80,111,
105,110,116,86,97,108,117,101,67,97,108,99,117,108,97,116,105,111,110,77,101,116,104,111,100,34,32,32,116,101,120,116,61,34,72,111,119,32,116,111,32,99,97,108,99,117,108,97,116,101,32,116,104,101,32,88,32,118,97,108,117,101,32,98,97,115,101,100,32,111,
110,32,116,104,101,32,112,111,115,105,116,105,111,110,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,77,105,110,47,77,97,120,32,98,97,115,101,100,61,48,44,78,101,105,103,104,98,111,117,114,32,
112,111,105,110,116,115,32,98,97,115,101,100,61,49,44,79,118,101,114,114,105,100,101,32,98,121,32,101,120,112,114,101,115,115,105,111,110,61,50,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,80,114,111,103,114,101,115,115,32,66,97,114,32,45,45,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,114,111,103,114,101,115,115,66,97,114,70,111,114,101,103,114,111,117,110,100,67,111,108,111,117,114,34,32,32,32,32,32,32,116,101,120,116,61,34,70,111,114,101,103,114,111,117,110,100,32,99,111,108,111,
117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,114,111,103,114,101,115,115,66,97,114,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,32,32,32,32,32,116,
101,120,116,61,34,66,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,114,111,103,114,101,115,115,66,97,114,68,105,115,
112,108,97,121,80,101,114,99,101,110,116,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,105,115,112,108,97,121,32,112,101,114,99,101,110,116,97,103,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,67,111,
109,112,111,110,101,110,116,32,116,121,112,101,115,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,83,108,105,100,101,114,34,32,116,101,120,116,61,34,78,111,114,109,97,108,32,115,108,105,100,101,114,44,32,108,105,110,107,115,32,116,
111,32,99,111,110,116,105,110,117,101,115,32,110,117,109,101,114,105,99,32,118,97,108,117,101,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,120,101,100,83,108,105,100,101,114,34,
32,116,101,120,116,61,34,83,108,105,100,101,114,32,116,104,97,116,32,104,97,115,32,97,32,108,105,115,116,32,111,102,32,112,114,101,45,100,101,102,105,110,101,100,32,118,97,108,117,101,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,83,108,105,100,101,114,34,32,116,101,120,116,61,34,76,105,107,101,32,110,111,114,109,97,108,32,83,108,105,100,101,114,32,98,117,116,32,119,105,116,104,32,99,117,115,116,111,109,32,103,114,97,
112,104,105,99,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,70,105,120,101,100,73,109,97,103,101,83,108,105,100,101,114,34,32,116,101,120,116,61,34,70,105,120,101,100,32,115,108,105,100,
101,114,32,119,105,116,104,32,99,117,115,116,111,109,32,103,114,97,112,104,105,99,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,111,103,103,108,101,66,117,116,116,111,110,34,32,116,101,
120,116,61,34,83,105,109,112,108,101,32,116,111,103,103,108,101,32,98,117,116,116,111,110,32,119,105,116,104,32,111,110,47,111,102,102,32,115,116,97,116,101,115,46,32,83,116,97,116,101,32,99,97,110,32,98,101,32,108,105,110,107,101,100,32,116,111,32,99,
117,115,116,111,109,32,110,117,109,101,114,105,99,32,118,97,108,117,101,46,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,66,117,116,116,111,110,34,32,116,101,120,116,61,34,67,111,110,116,97,
105,110,115,32,97,32,108,105,115,116,32,111,102,32,118,97,108,117,101,115,32,97,110,100,32,99,121,99,108,101,115,32,98,101,116,119,101,101,110,32,116,104,101,109,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,117,105,73,109,97,103,101,66,117,116,116,111,110,34,32,116,101,120,116,61,34,76,105,107,101,32,97,32,66,117,116,116,111,110,32,98,117,116,32,119,105,116,104,32,99,117,115,116,111,109,32,103,114,97,112,104,105,99,115,32,102,111,114,32,111,110,
47,111,102,102,32,115,116,97,116,101,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,111,109,98,111,34,32,116,101,120,116,61,34,65,32,99,111,109,98,111,32,98,111,120,32,119,105,116,104,
32,97,32,108,105,115,116,32,111,102,32,118,97,108,117,101,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,97,98,101,108,34,32,116,101,120,116,61,34,65,32,115,116,97,116,105,99,32,108,97,
98,101,108,32,102,111,114,32,100,105,115,112,108,97,121,105,110,103,32,115,111,109,101,32,116,101,120,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,67,68,76,97,98,101,108,34,32,116,101,
120,116,61,34,65,32,115,116,97,116,105,99,32,76,69,68,32,108,105,107,101,32,108,97,98,101,108,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,71,114,111,117,112,34,32,116,101,120,116,61,34,65,
32,103,114,111,117,112,105,110,103,32,99,111,109,112,111,110,101,110,116,46,32,72,97,115,32,97,110,32,111,117,116,108,105,110,101,32,97,110,100,32,97,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,111,117,114,46,32,34,32,116,121,112,101,61,34,
84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,73,109,97,103,101,34,32,116,101,120,116,61,34,68,105,115,112,108,97,121,115,32,97,110,100,32,105,109,97,103,101,32,102,114,111,109,32,97,32,114,101,115,111,117,114,99,101,
34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,67,117,115,116,111,109,67,111,109,112,111,110,101,110,116,34,32,116,101,120,116,61,34,65,32,99,117,115,116,111,109,32,99,111,109,112,111,110,101,
110,116,32,119,104,105,99,104,32,105,115,32,100,114,97,119,110,32,97,110,100,32,104,97,110,100,108,101,100,32,119,105,116,104,105,110,32,76,85,65,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,
77,105,100,105,75,101,121,98,111,97,114,100,34,32,116,101,120,116,61,34,65,32,77,73,68,73,32,107,101,121,98,111,97,114,100,32,119,105,116,104,32,99,117,115,116,111,109,32,111,114,105,101,110,116,97,116,105,111,110,32,97,110,100,32,99,111,108,111,117,
114,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,69,110,118,101,108,111,112,101,34,32,116,101,120,116,61,34,65,32,109,117,108,116,105,45,112,111,105,110,116,32,101,110,118,101,108,111,
112,101,32,101,100,105,116,111,114,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,97,98,115,34,32,116,101,120,116,61,34,84,97,98,98,101,100,32,99,111,109,112,111,110,101,110,116,34,32,116,
121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,80,97,110,101,108,69,100,105,116,111,114,34,32,116,101,120,116,61,34,117,105,80,97,110,101,108,69,100,105,116,111,114,34,32,116,121,112,101,61,34,84,101,
120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,65,114,114,111,119,34,32,116,101,120,116,61,34,65,110,32,97,114,114,111,119,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,
105,87,97,118,101,102,111,114,109,34,32,116,101,120,116,61,34,65,32,115,105,109,112,108,101,32,119,97,118,101,102,111,114,109,32,100,114,97,119,105,110,103,32,99,111,109,112,111,110,101,110,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,117,105,72,121,112,101,114,108,105,110,107,34,32,116,101,120,116,61,34,65,32,104,121,112,101,114,108,105,110,107,32,98,117,116,116,111,110,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,117,105,88,89,83,117,114,102,97,99,101,34,32,116,101,120,116,61,34,65,32,88,89,32,109,111,100,117,108,97,116,111,114,32,115,117,114,102,97,99,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,13,10,32,32,60,33,
45,45,32,77,73,68,73,32,109,101,115,115,97,103,101,32,112,114,111,112,101,114,116,105,101,115,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,77,101,115,115,97,103,101,67,116,114,108,114,78,117,109,98,101,114,34,32,116,101,
120,116,61,34,77,73,68,73,32,67,111,110,116,114,111,108,108,101,114,32,110,117,109,98,101,114,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,50,55,34,32,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,109,105,100,105,77,101,115,115,97,103,101,67,104,97,110,110,101,108,34,32,116,101,120,116,61,34,77,73,68,73,32,67,104,97,110,110,101,108,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,
34,49,54,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,77,101,115,115,97,103,101,67,116,114,108,114,86,97,108,117,101,34,32,116,101,120,116,61,34,77,73,68,73,32,67,111,110,116,114,111,108,108,101,114,32,118,97,108,117,101,34,
32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,50,55,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,77,101,115,115,97,103,101,84,121,112,101,34,32,116,101,120,116,61,
34,77,73,68,73,32,109,101,115,115,97,103,101,32,116,121,112,101,34,32,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,32,100,101,102,97,117,108,116,115,61,34,67,67,61,48,44,65,102,116,101,114,116,111,117,99,104,61,49,44,67,104,97,110,
110,101,108,80,114,101,115,115,117,114,101,61,50,44,78,111,116,101,79,110,61,51,44,78,111,116,101,79,102,102,61,52,44,83,121,115,69,120,61,53,44,77,117,108,116,105,61,54,44,80,114,111,103,114,97,109,67,104,97,110,103,101,61,55,44,80,105,116,99,104,87,
104,101,101,108,61,56,44,78,111,110,101,61,57,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,77,101,115,115,97,103,101,83,121,115,69,120,70,111,114,109,117,108,97,34,32,116,101,120,116,61,34,83,121,115,69,120,32,70,111,114,109,
117,108,97,34,32,116,121,112,101,61,34,83,121,115,69,120,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,77,101,115,115,97,103,101,67,104,97,110,110,101,108,79,118,101,114,114,105,100,101,34,32,116,101,120,116,61,34,79,118,101,
114,114,105,100,101,32,112,97,110,101,108,32,77,73,68,73,32,99,104,97,110,110,101,108,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,89,101,115,47,78,111,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,
105,100,105,77,101,115,115,97,103,101,77,117,108,116,105,76,105,115,116,34,32,116,101,120,116,61,34,77,117,108,116,105,32,77,101,115,115,97,103,101,32,108,105,115,116,34,32,116,121,112,101,61,34,77,117,108,116,105,77,105,100,105,34,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,109,105,100,105,73,110,67,104,97,110,110,101,108,34,32,116,101,120,116,61,34,77,73,68,73,32,73,110,112,117,116,32,99,104,97,110,110,101,108,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,
34,49,34,32,109,97,120,61,34,49,54,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,79,117,116,67,104,97,110,110,101,108,34,32,116,101,120,116,61,34,77,73,68,73,32,79,117,116,112,117,116,32,99,104,97,110,110,101,108,34,32,116,121,
112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,49,54,34,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,68,101,118,105,99,101,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,
105,99,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,68,101,118,105,99,101,78,97,109,101,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,105,99,101,78,97,109,101,34,32,116,
121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,68,101,118,105,99,101,84,121,112,101,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,105,99,101,84,121,112,101,34,32,116,121,112,101,61,34,
66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,68,101,118,105,99,101,73,110,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,105,99,101,73,110,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,68,101,118,105,99,101,79,117,116,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,105,99,101,79,117,116,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,
101,61,34,109,105,100,105,68,101,118,105,99,101,79,112,101,110,101,100,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,105,99,101,79,112,101,110,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,109,105,100,105,68,101,118,105,99,101,69,114,114,111,114,83,116,97,116,101,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,105,99,101,69,114,114,111,114,83,116,97,116,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,109,105,100,105,68,101,118,105,99,101,77,97,110,97,103,101,114,34,32,116,101,120,116,61,34,109,105,100,105,68,101,118,105,99,101,77,97,110,97,103,101,114,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,13,10,
32,32,60,105,100,32,110,97,109,101,61,34,105,110,118,97,108,105,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,105,110,118,97,108,105,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,111,112,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,116,111,112,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,98,111,116,116,111,109,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,
98,111,116,116,111,109,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,101,102,116,34,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,108,101,102,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,114,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,114,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,
112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,98,111,116,116,111,109,82,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,98,111,116,116,111,109,82,105,103,104,116,34,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,98,111,116,116,111,109,76,101,102,116,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,98,
111,116,116,111,109,76,101,102,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,111,112,82,105,103,104,116,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,116,111,112,82,105,103,104,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,
100,32,110,97,109,101,61,34,116,111,112,76,101,102,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,116,111,112,76,101,102,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,111,118,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,111,118,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,110,100,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,117,110,100,
101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,100,101,115,99,114,105,112,116,105,111,110,34,
32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,115,99,114,105,112,116,105,111,110,32,111,102,32,97,110,32,111,98,106,101,99,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,83,109,
97,108,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,101,101,69,100,105,116,111,114,83,116,97,116,101,34,32,32,32,116,101,120,116,61,34,108,117,97,84,114,101,101,69,100,105,116,111,114,83,116,97,116,101,34,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,114,111,112,101,114,116,105,101,115,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,112,114,
111,112,101,114,116,105,101,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,105,109,101,111,117,116,34,9,9,
9,32,32,116,101,120,116,61,34,84,105,109,101,111,117,116,34,9,9,9,9,9,9,9,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,48,48,48,48,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,101,110,97,98,108,101,100,34,9,9,9,32,32,116,101,120,116,61,34,69,110,97,98,108,101,100,32,97,110,100,32,97,99,116,105,118,101,34,9,9,9,9,9,9,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,105,110,116,101,114,110,97,108,34,9,9,9,32,32,116,101,120,116,61,34,73,110,116,101,114,110,97,108,32,111,114,32,98,117,105,108,116,45,105,110,34,9,9,9,9,9,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,13,10,32,32,60,33,45,
45,32,82,101,115,111,117,114,99,101,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,114,101,115,111,117,114,99,101,83,105,122,101,34,32,9,9,9,9,116,101,120,116,61,34,114,101,115,111,117,114,99,101,83,105,122,101,34,32,9,9,9,116,121,112,101,
61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,114,101,115,111,117,114,99,101,73,109,97,103,101,34,32,9,9,9,9,116,101,120,116,61,34,114,101,115,111,117,114,99,101,73,109,97,103,101,34,32,9,9,9,116,121,112,101,61,34,84,101,
120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,114,101,115,111,117,114,99,101,73,109,97,103,101,87,105,100,116,104,34,32,9,9,116,101,120,116,61,34,70,114,97,109,101,32,119,105,100,116,104,34,32,9,9,9,9,116,121,112,101,61,34,84,101,120,
116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,114,101,115,111,117,114,99,101,73,109,97,103,101,72,101,105,103,104,116,34,32,9,9,116,101,120,116,61,34,70,114,97,109,101,32,104,101,105,103,104,116,34,32,9,9,9,116,121,112,101,61,34,84,101,
120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,114,101,115,111,117,114,99,101,73,109,97,103,101,80,97,105,110,116,77,111,100,101,34,32,9,116,101,120,116,61,34,70,114,97,109,101,32,112,97,105,110,116,32,109,111,100,101,34,32,9,9,116,121,
112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,61,34,64,105,109,97,103,101,76,97,121,111,117,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,114,101,115,111,117,114,99,101,73,109,97,103,101,79,114,105,
101,110,116,97,116,105,111,110,34,32,9,116,101,120,116,61,34,73,109,97,103,101,32,102,114,97,109,101,32,111,114,105,101,110,116,97,116,105,111,110,34,32,9,116,121,112,101,61,34,86,97,114,78,117,109,101,114,105,99,34,32,100,101,102,97,117,108,116,115,
61,34,72,111,114,105,122,111,110,116,97,108,61,48,44,86,101,114,116,105,99,97,108,61,49,34,47,62,13,10,13,10,32,32,60,33,45,45,32,73,110,115,116,97,110,99,101,32,115,101,116,116,105,110,103,115,32,40,119,104,101,110,32,112,97,110,101,108,32,105,115,32,
101,120,112,111,114,116,101,100,32,97,115,32,97,110,32,105,110,115,116,97,110,99,101,41,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,73,110,115,116,97,110,99,101,73,115,82,101,115,105,97,122,98,108,101,34,32,32,32,32,
32,32,32,32,32,116,101,120,116,61,34,69,120,112,111,114,116,101,100,32,105,110,115,116,97,110,99,101,32,119,105,108,108,32,98,101,32,114,101,115,105,122,97,98,108,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,
34,82,101,115,105,122,97,98,108,101,47,78,111,116,32,82,101,115,105,122,97,98,108,101,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,71,108,111,98,97,108,32,67,116,114,108,114,32,115,101,116,116,105,110,103,115,32,45,45,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,99,116,114,108,114,77,97,120,69,120,112,111,114,116,101,100,86,115,116,80,97,114,97,109,101,116,101,114,115,34,32,32,32,32,116,101,120,116,61,34,77,97,120,32,101,120,112,111,114,116,101,100,32,86,83,84,32,112,97,114,97,109,115,34,32,
116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,57,57,57,57,57,57,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,111,103,84,111,70,105,108,
101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,111,103,32,101,118,101,114,121,116,104,105,110,103,32,116,111,32,102,105,108,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,99,116,114,108,114,87,97,114,110,105,110,103,73,110,66,111,111,116,115,116,114,97,112,83,116,97,116,101,34,32,32,32,32,32,116,101,120,116,61,34,82,117,110,32,109,111,100,117,108,97,116,111,114,32,118,97,108,117,101,67,104,97,110,
103,101,32,76,85,65,32,99,97,108,108,98,97,99,107,32,105,110,32,66,111,111,116,115,116,114,97,112,32,115,116,97,116,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,117,97,
68,101,98,117,103,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,69,110,97,98,108,101,32,76,85,65,32,100,101,98,117,103,103,105,110,103,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,99,116,114,108,114,83,104,117,116,100,111,119,110,68,101,108,97,121,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,108,117,103,105,110,32,115,104,117,116,100,111,119,110,32,100,101,108,97,121,34,32,116,
121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,49,48,48,48,48,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,85,115,101,69,100,105,116,111,114,87,
114,97,112,112,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,87,114,97,112,32,116,104,101,32,86,83,84,50,46,52,32,112,108,117,103,105,110,32,111,110,32,65,98,108,101,116,111,110,32,76,105,118,101,32,102,111,114,32,87,105,110,100,
111,119,115,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,86,101,114,115,105,111,110,83,101,112,97,114,97,116,111,114,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,
86,101,114,115,105,111,110,32,102,105,101,108,100,32,115,101,112,97,114,97,116,111,114,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,86,101,114,115,105,111,110,67,111,109,112,114,
101,115,115,101,100,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,83,97,118,101,32,118,101,114,115,105,111,110,101,100,32,112,97,110,101,108,115,32,99,111,109,112,114,101,115,115,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,86,115,116,70,105,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,83,84,32,70,105,108,101,34,32,116,121,112,101,61,34,70,105,108,101,
80,114,111,112,101,114,116,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,65,117,70,105,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,85,32,70,105,108,101,34,32,
32,116,121,112,101,61,34,70,105,108,101,80,114,111,112,101,114,116,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,65,112,112,70,105,108,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,
116,61,34,65,80,80,32,70,105,108,101,34,32,116,121,112,101,61,34,70,105,108,101,80,114,111,112,101,114,116,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,69,100,105,116,111,114,66,111,117,110,100,115,34,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,69,100,105,116,111,114,32,98,111,117,110,100,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,105,100,105,68,101,118,
105,99,101,78,117,109,80,114,111,103,114,97,109,115,34,32,32,32,32,32,32,32,116,101,120,116,61,34,78,117,109,98,101,114,32,111,102,32,112,114,111,103,114,97,109,115,32,116,104,101,32,100,101,118,105,99,101,32,99,97,110,32,115,116,111,114,101,32,112,101,
114,32,98,97,110,107,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,56,49,57,50,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,105,100,
105,68,101,118,105,99,101,78,117,109,66,97,110,107,115,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,117,109,98,101,114,32,111,102,32,98,97,110,107,115,32,116,104,101,32,100,101,118,105,99,101,32,104,97,115,34,32,116,121,112,101,61,34,78,
117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,56,49,57,50,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,75,101,121,98,111,97,114,100,77,97,112,112,105,110,103,34,32,32,
32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,75,101,121,98,111,97,114,100,32,109,97,112,112,105,110,103,115,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,118,115,116,73,110,
100,101,120,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,86,83,84,32,73,110,100,101,120,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,110,97,109,
101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,97,109,101,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,84,121,112,101,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,111,109,112,111,110,101,110,116,32,84,121,112,101,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,105,110,104,101,114,105,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,73,110,104,101,114,105,116,32,102,114,111,109,32,112,97,114,101,110,116,34,32,116,121,112,101,61,
34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,112,114,111,103,114,97,109,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,80,114,111,103,114,97,109,34,32,32,109,105,
110,61,34,48,34,32,109,97,120,61,34,49,50,55,34,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,98,97,110,107,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,116,101,120,116,61,34,66,97,110,107,34,32,32,32,32,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,115,
98,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,101,97,115,116,32,115,105,103,110,105,102,105,99,97,110,116,32,112,97,114,116,32,111,102,32,97,32,110,117,109,98,101,114,34,32,116,
121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,51,53,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,115,98,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,116,101,120,116,61,34,77,111,115,116,32,32,115,105,103,110,105,102,105,99,97,110,116,32,112,97,114,116,32,111,102,32,97,32,110,117,109,98,101,114,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,
32,109,97,120,61,34,54,53,53,51,53,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,118,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,117,114,114,101,110,116,32,
118,97,108,117,101,34,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,32,109,105,110,61,34,45,54,53,53,51,53,34,32,109,97,120,61,34,54,53,53,51,53,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,105,109,101,34,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,105,109,101,115,116,97,109,112,34,32,116,121,112,101,61,34,84,105,109,101,115,116,97,109,112,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,110,
117,109,98,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,78,117,109,98,101,114,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,45,54,53,53,51,53,34,32,109,
97,120,61,34,54,53,53,51,53,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,117,105,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,85,110,105,113,117,101,32,105,100,101,
110,116,105,102,105,101,114,34,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,83,99,114,111,108,108,98,97,114,84,104,105,99,107,110,101,115,115,34,32,32,32,32,32,32,
32,32,32,32,116,101,120,116,61,34,84,104,105,99,107,110,101,115,115,32,102,111,114,32,97,108,108,32,115,99,114,111,108,108,98,97,114,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,56,34,32,109,97,120,61,34,49,50,56,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,101,103,97,99,121,77,111,100,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,111,111,107,65,110,100,70,101,101,108,32,86,51,32,102,111,
114,32,111,108,100,101,114,32,112,97,110,101,108,115,34,32,116,121,112,101,61,34,66,111,111,108,34,32,100,101,102,97,117,108,116,115,61,34,76,101,103,97,99,121,32,77,111,100,101,47,68,105,115,97,98,108,101,100,34,32,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,99,116,114,108,114,67,111,108,111,117,114,83,99,104,101,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,102,97,117,108,116,32,108,111,111,107,65,110,100,70,101,101,108,32,86,52,32,99,111,108,111,
117,114,83,99,104,101,109,101,34,32,116,121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,99,116,114,108,114,67,111,108,111,117,114,83,99,104,101,109,101,115,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,99,116,114,108,114,76,111,111,107,65,110,100,70,101,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,102,97,117,108,116,32,108,111,111,107,65,110,100,70,101,101,108,32,86,101,114,115,105,111,110,34,32,116,
121,112,101,61,34,86,97,114,84,101,120,116,34,32,100,101,102,97,117,108,116,115,61,34,64,108,111,111,107,97,110,100,102,101,101,108,34,47,62,13,10,32,32,13,10,13,10,13,10,32,32,60,33,45,45,32,76,111,103,103,105,110,103,32,115,101,116,116,105,110,103,
115,32,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,105,100,105,77,111,110,73,110,112,117,116,66,117,102,102,101,114,83,105,122,101,34,32,32,32,32,116,101,120,116,61,34,77,73,68,73,32,77,111,110,32,105,110,112,117,
116,32,98,117,102,102,101,114,32,115,105,122,101,34,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,53,49,50,34,32,109,97,120,61,34,54,53,53,51,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,
116,114,108,114,77,105,100,105,77,111,110,79,117,116,112,117,116,66,117,102,102,101,114,83,105,122,101,34,32,32,32,116,101,120,116,61,34,77,73,68,73,32,77,111,110,32,111,117,116,112,117,116,32,98,117,102,102,101,114,32,115,105,122,101,34,32,32,32,32,
32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,53,49,50,34,32,109,97,120,61,34,54,53,53,51,53,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,111,103,77,105,100,105,73,110,112,117,116,34,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,111,103,103,108,101,32,77,73,68,73,32,73,110,112,117,116,32,108,111,103,103,105,110,103,34,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,99,116,114,108,114,76,111,103,77,105,100,105,79,117,116,112,117,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,111,103,103,108,101,32,77,73,68,73,32,111,117,116,112,117,116,32,108,111,103,103,105,110,103,
34,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,111,103,79,112,116,105,111,110,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,
77,73,68,73,32,76,111,103,32,111,112,116,105,111,110,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,111,103,84,111,70,105,108,101,34,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,111,103,32,100,101,98,117,103,32,109,101,115,115,97,103,101,115,32,116,111,32,97,32,102,105,108,101,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,99,116,114,108,114,77,105,100,105,68,101,118,105,99,101,77,97,110,97,103,101,114,83,97,118,101,100,71,108,111,98,97,108,121,34,32,116,101,120,116,61,34,72,111,119,32,116,104,101,32,115,116,97,116,101,32,111,102,32,116,104,101,32,100,101,118,105,99,
101,32,109,97,110,97,103,101,114,32,105,115,32,114,101,109,101,109,98,101,114,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,117,97,68,105,115,97,98,108,101,100,34,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,85,65,32,105,115,32,100,105,115,97,98,108,101,100,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,117,97,66,111,111,
116,115,116,114,97,112,70,105,108,101,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,76,85,65,32,115,99,114,105,112,116,32,108,111,97,100,101,100,32,111,110,32,67,116,114,108,114,32,115,116,97,114,116,34,32,116,121,112,101,61,34,84,101,120,116,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,97,115,116,66,114,111,119,115,101,100,82,101,115,111,117,114,99,101,68,105,114,34,32,32,32,32,116,101,120,116,61,34,76,97,115,116,32,100,105,114,101,99,116,111,114,121,32,98,
114,111,119,115,101,100,32,102,111,114,32,114,101,115,111,117,114,99,101,115,34,32,116,121,112,101,61,34,84,101,120,116,34,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,79,118,101,114,119,114,105,116,101,82,101,115,111,
117,114,99,101,115,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,118,101,114,119,114,105,116,101,32,105,109,112,111,114,116,101,100,32,114,101,115,111,117,114,99,101,115,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,99,116,114,108,114,65,117,116,111,83,97,118,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,117,116,111,115,97,118,101,32,101,110,97,98,108,101,100,34,32,116,121,112,101,61,34,66,111,111,108,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,65,117,116,111,83,97,118,101,73,110,116,101,114,118,97,108,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,117,116,111,115,97,118,101,32,105,110,116,101,114,118,97,108,
32,105,110,32,115,101,99,111,110,100,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,49,34,32,109,97,120,61,34,57,57,57,57,57,57,34,32,105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,
114,108,114,79,112,101,110,87,105,110,100,111,119,83,116,97,116,101,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,79,112,101,110,32,112,97,110,101,108,32,100,105,97,108,111,103,32,115,116,97,116,101,34,32,116,121,112,101,61,34,84,101,120,
116,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,80,114,111,112,101,114,116,105,101,115,65,114,101,85,82,76,115,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,87,104,101,110,32,101,100,105,116,105,110,103,32,97,32,112,
97,110,101,108,32,116,104,101,32,112,114,111,112,101,114,116,121,32,110,97,109,101,115,32,99,97,110,32,98,101,32,99,108,105,99,107,97,98,108,101,32,85,82,76,115,32,116,111,32,114,101,108,101,118,97,110,116,32,87,73,75,73,32,112,97,103,101,115,34,32,116,
121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,82,101,99,101,110,101,116,79,112,101,110,101,100,80,97,110,101,108,70,105,108,101,115,34,32,32,32,116,101,120,116,61,34,82,101,99,101,110,116,
32,111,112,101,110,101,100,32,80,97,110,101,108,32,102,105,108,101,115,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,76,97,115,116,66,114,111,119,115,101,100,70,105,108,101,68,
105,114,101,99,116,111,114,121,34,32,32,116,101,120,116,61,34,76,97,115,116,32,98,114,111,119,115,101,100,32,102,105,108,101,32,100,105,114,101,99,116,111,114,121,34,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,13,10,32,32,60,105,100,32,
110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,73,116,101,109,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,34,32,32,9,116,101,120,116,61,34,77,101,110,117,32,105,116,101,109,32,98,97,99,107,103,114,111,117,110,100,32,99,111,108,
111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,73,116,101,109,84,101,120,116,67,111,108,111,117,114,34,32,9,9,9,116,101,120,116,61,34,77,101,
110,117,32,105,116,101,109,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,73,116,101,109,72,105,103,104,108,
105,103,104,116,101,100,84,101,120,116,67,111,108,111,117,114,34,32,116,101,120,116,61,34,77,101,110,117,32,105,116,101,109,32,104,105,103,104,108,105,103,104,116,101,100,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,
108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,73,116,101,109,72,105,103,104,108,105,103,104,116,67,111,108,111,117,114,34,32,9,9,116,101,120,116,61,34,77,101,110,117,32,105,116,101,109,32,
104,105,103,104,108,105,103,104,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,73,116,101,109,70,111,110,116,34,32,9,9,9,9,
9,116,101,120,116,61,34,77,101,110,117,32,105,116,101,109,32,102,111,110,116,34,32,116,121,112,101,61,34,70,111,110,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,73,116,101,109,83,101,112,97,114,97,116,
111,114,67,111,108,111,117,114,34,9,9,116,101,120,116,61,34,77,101,110,117,32,115,101,112,97,114,97,116,111,114,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,
116,114,108,114,77,101,110,117,73,116,101,109,72,101,97,100,101,114,67,111,108,111,117,114,34,9,9,9,116,101,120,116,61,34,77,101,110,117,32,104,101,97,100,101,114,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,
13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,66,97,114,66,97,99,107,103,114,111,117,110,100,67,111,108,111,117,114,49,34,32,9,9,116,101,120,116,61,34,77,101,110,117,32,98,97,114,32,98,97,99,107,103,114,111,117,
110,100,32,103,114,97,100,105,101,110,116,32,99,111,108,111,117,114,32,116,111,112,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,66,97,114,66,97,99,107,
103,114,111,117,110,100,67,111,108,111,117,114,50,34,32,9,9,116,101,120,116,61,34,77,101,110,117,32,98,97,114,32,98,97,99,107,103,114,111,117,110,100,32,103,114,97,100,105,101,110,116,32,99,111,108,111,117,114,32,98,111,116,116,111,109,34,32,116,121,
112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,66,97,114,84,101,120,116,67,111,108,111,117,114,34,32,9,9,9,116,101,120,116,61,34,77,101,110,117,32,98,97,114,32,116,101,
120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,66,97,114,72,105,103,104,108,105,103,104,116,101,100,84,101,120,116,67,111,
108,111,117,114,34,32,9,116,101,120,116,61,34,77,101,110,117,32,98,97,114,32,104,105,103,104,108,105,103,104,116,101,100,32,116,101,120,116,32,99,111,108,111,117,114,34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,
32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,66,97,114,72,105,103,104,108,105,103,104,116,67,111,108,111,117,114,34,32,9,9,116,101,120,116,61,34,77,101,110,117,32,98,97,114,32,104,105,103,104,108,105,103,104,116,32,99,111,108,111,117,114,
34,32,116,121,112,101,61,34,67,111,108,111,117,114,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,66,97,114,70,111,110,116,34,32,9,9,9,9,9,116,101,120,116,61,34,77,101,110,117,32,98,97,114,32,102,111,110,
116,34,32,116,121,112,101,61,34,70,111,110,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,77,101,110,117,66,97,114,72,101,105,103,104,116,34,32,9,9,9,9,116,101,120,116,61,34,77,101,110,117,32,98,97,114,32,104,101,105,
103,104,116,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,52,34,32,109,97,120,61,34,49,50,56,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,78,97,116,105,118,101,65,108,101,114,116,115,34,9,
9,9,9,9,116,101,120,116,61,34,85,115,101,32,79,83,32,110,97,116,105,118,101,32,100,105,97,108,111,103,32,119,105,110,100,111,119,115,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,
114,78,97,116,105,118,101,70,105,108,101,68,105,97,108,111,103,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,85,115,101,32,79,83,32,110,97,116,105,118,101,32,102,105,108,101,32,111,112,101,110,47,115,97,118,101,32,100,105,97,108,
111,103,115,32,40,100,105,115,97,98,108,101,32,102,111,114,32,76,105,110,117,120,41,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,80,114,105,118,97,116,101,75,101,121,34,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,83,65,32,112,114,105,118,97,116,101,32,107,101,121,32,117,115,101,100,32,116,111,32,115,105,103,110,32,101,120,112,111,114,116,101,100,32,112,97,110,101,108,115,34,32,116,
121,112,101,61,34,70,105,108,101,80,114,111,112,101,114,116,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,70,111,110,116,83,105,122,101,66,97,115,101,86,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,66,97,115,101,32,118,97,108,117,101,32,102,111,114,32,97,108,108,32,102,111,110,116,32,115,105,122,101,115,32,40,119,111,110,39,116,32,97,102,102,101,99,116,32,112,97,110,101,108,32,85,73,41,34,32,116,121,112,101,61,34,78,117,109,101,
114,105,99,34,32,109,105,110,61,34,54,46,48,34,32,109,97,120,61,34,49,50,56,46,48,34,32,105,110,116,61,34,48,46,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,84,97,98,66,97,114,68,101,112,116,104,34,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,112,116,104,32,111,102,32,97,108,108,32,116,97,98,32,98,97,114,115,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,54,34,32,109,97,120,61,34,49,50,56,34,32,
105,110,116,61,34,49,34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,80,114,111,112,101,114,116,121,76,105,110,101,104,101,105,103,104,116,66,97,115,101,86,97,108,117,101,34,32,32,32,116,101,120,116,61,34,66,97,115,101,32,118,
97,108,117,101,32,102,111,114,32,112,114,111,112,101,114,116,121,32,108,105,110,101,32,104,101,105,103,104,116,34,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,50,52,34,32,109,97,120,61,34,55,50,34,32,105,110,116,61,34,49,
34,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,99,116,114,108,114,80,114,111,112,101,114,116,121,76,105,110,101,73,109,112,114,111,118,101,100,76,101,103,105,98,105,108,105,116,121,34,32,32,32,116,101,120,116,61,34,73,109,112,114,111,118,101,
32,108,101,103,105,98,105,108,105,116,121,32,111,102,32,112,114,111,112,101,114,116,121,32,102,105,101,108,100,115,34,32,116,121,112,101,61,34,66,111,111,108,34,47,62,13,10,13,10,32,32,60,33,45,45,32,76,85,65,32,99,97,108,108,98,97,99,107,115,32,45,45,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,97,110,97,103,101,114,77,101,116,104,111,100,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,108,117,97,77,97,110,97,103,101,114,77,101,116,104,111,100,115,34,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,101,116,104,111,100,67,111,100,101,
34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,108,117,97,77,101,116,104,111,100,67,111,100,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,101,116,104,111,100,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,108,117,97,77,101,
116,104,111,100,78,97,109,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,
117,97,77,101,116,104,111,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,108,117,97,77,101,116,104,111,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,97,110,97,103,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,
101,120,116,61,34,108,117,97,77,97,110,97,103,101,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,108,117,97,77,101,116,104,111,100,86,97,108,105,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,108,117,97,77,101,116,104,111,100,86,97,108,105,100,34,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,111,100,117,108,97,116,111,114,86,97,108,117,101,67,104,
97,110,103,101,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,116,104,101,32,109,111,100,117,108,97,116,111,114,32,118,97,108,117,101,32,99,104,97,110,103,101,115,34,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,111,100,117,108,97,116,111,114,67,111,109,112,111,110,101,110,116,67,104,97,110,103,101,34,32,32,32,32,32,32,116,
101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,116,104,101,32,85,73,32,99,104,97,110,103,101,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,
34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,111,100,117,108,97,116,111,114,80,114,111,112,101,114,116,121,67,104,97,110,103,101,100,34,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,
32,112,114,111,112,101,114,116,121,32,99,104,97,110,103,101,115,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
108,117,97,77,111,100,117,108,97,116,111,114,71,101,116,86,97,108,117,101,70,111,114,77,73,68,73,34,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,99,97,108,99,117,108,97,116,101,32,116,104,101,32,77,73,68,73,32,118,97,108,
117,101,32,116,111,32,115,101,110,100,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,111,100,117,108,97,116,111,114,71,101,116,86,97,
108,117,101,70,114,111,109,77,73,68,73,34,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,99,97,108,99,117,108,97,116,101,32,110,101,119,32,109,111,100,117,108,97,116,111,114,32,118,97,108,117,101,32,102,114,111,109,32,97,32,
77,73,68,73,32,118,97,108,117,101,34,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,77,111,100,117,108,97,116,111,114,77,105,100,105,80,97,116,116,101,114,110,67,104,
97,110,103,101,100,34,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,105,102,32,116,104,101,32,77,73,68,73,32,112,97,116,116,101,114,110,32,99,104,97,110,103,101,115,32,111,114,32,110,101,101,100,115,32,117,112,100,97,116,105,110,103,34,32,116,
121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,76,111,97,100,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,
108,101,100,32,119,104,101,110,32,116,104,101,32,112,97,110,101,108,32,104,97,115,32,102,105,110,105,115,104,101,100,32,108,111,97,100,105,110,103,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,77,111,100,117,108,97,116,111,114,86,97,108,117,101,67,104,97,110,103,101,100,34,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,110,121,32,109,
111,100,117,108,97,116,111,114,32,99,104,97,110,103,101,115,32,105,116,115,32,118,97,108,117,101,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,
97,80,97,110,101,108,66,101,102,111,114,101,76,111,97,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,98,101,102,111,114,101,32,97,110,121,32,109,111,100,117,108,97,116,111,114,115,32,97,114,101,32,99,
114,101,97,116,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,77,105,100,105,67,104,97,110,110,101,108,67,
104,97,110,103,101,100,34,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,77,73,68,73,32,99,104,97,110,110,101,108,32,99,104,97,110,103,101,115,32,102,111,114,32,97,32,112,97,110,101,108,34,32,32,32,32,32,
32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,77,105,100,105,82,101,99,101,105,118,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,
61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,112,97,110,101,108,32,114,101,99,101,105,118,101,115,32,97,32,77,73,68,73,32,109,101,115,115,97,103,101,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,
32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,77,105,100,105,77,117,108,116,105,82,101,99,101,105,118,101,100,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,112,
97,110,101,108,32,114,101,99,101,105,118,101,115,32,97,32,109,117,108,116,105,32,77,73,68,73,32,109,101,115,115,97,103,101,34,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
108,117,97,80,97,110,101,108,79,83,67,82,101,99,101,105,118,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,112,97,110,101,108,32,114,101,99,101,105,118,101,115,32,97,32,79,
83,67,32,109,101,115,115,97,103,101,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,80,97,105,110,116,66,97,99,107,103,
114,111,117,110,100,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,112,97,105,110,116,32,97,32,99,117,115,116,111,109,32,112,97,110,101,108,32,98,97,99,107,103,114,111,117,110,100,34,32,32,32,32,32,32,32,32,
32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,83,97,118,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,
61,34,67,97,108,108,101,100,32,98,101,102,111,114,101,32,97,32,112,97,110,101,108,32,105,115,32,115,97,118,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,
62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,86,105,101,119,80,111,114,116,82,101,115,105,122,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,116,104,101,32,86,
105,101,119,80,111,114,116,32,105,115,32,114,101,115,105,122,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,
80,97,110,101,108,82,101,115,105,122,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,112,97,110,101,108,32,99,97,110,118,97,115,32,105,115,32,114,101,115,105,122,
101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,80,114,111,103,114,97,109,67,104,97,110,103,101,
100,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,97,102,116,101,114,32,97,32,112,114,111,103,114,97,109,32,99,104,97,110,103,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,
112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,71,108,111,98,97,108,67,104,97,110,103,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,
108,101,100,32,119,104,101,110,32,97,32,103,108,111,98,97,108,32,118,97,114,105,97,98,108,101,32,105,115,32,99,104,97,110,103,101,100,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,77,101,110,117,98,97,114,67,117,115,116,111,109,34,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,116,111,32,103,101,116,32,116,104,101,32,109,101,
110,117,98,97,114,32,105,116,101,109,115,32,102,111,114,32,116,104,105,115,32,112,97,110,101,108,34,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,
110,101,108,77,101,115,115,97,103,101,72,97,110,100,108,101,114,34,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,97,32,109,101,115,115,97,103,101,32,110,101,101,100,115,32,116,111,32,98,101,32,100,
105,115,112,108,97,121,101,100,34,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,70,105,108,101,68,114,97,103,68,114,111,112,
72,97,110,100,108,101,114,34,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,98,97,99,107,32,116,111,32,105,110,100,105,99,97,116,101,32,116,104,97,116,32,116,104,101,32,117,115,101,114,32,104,97,115,32,100,114,111,112,112,101,100,32,116,104,101,
32,102,105,108,101,115,32,111,110,116,111,32,116,104,105,115,32,112,97,110,101,108,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,108,70,105,108,101,68,
114,97,103,69,110,116,101,114,72,97,110,100,108,101,114,34,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,98,97,99,107,32,116,111,32,105,110,100,105,99,97,116,101,32,116,104,97,116,32,115,111,109,101,32,102,105,108,101,115,32,97,114,101,32,98,101,
105,110,103,32,100,114,97,103,103,101,100,32,111,118,101,114,32,116,104,105,115,32,112,97,110,101,108,34,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,80,97,110,101,
108,70,105,108,101,68,114,97,103,69,120,105,116,72,97,110,100,108,101,114,34,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,98,97,99,107,32,116,111,32,105,110,100,105,99,97,116,101,32,116,104,97,116,32,116,104,101,32,109,111,117,115,101,32,104,
97,115,32,109,111,118,101,100,32,97,119,97,121,32,102,114,111,109,32,116,104,105,115,32,112,97,110,101,108,34,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,
97,80,97,110,101,108,82,101,115,111,117,114,99,101,115,76,111,97,100,101,100,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,98,97,99,107,32,116,111,32,105,110,100,105,99,97,116,101,32,116,104,97,116,32,97,108,108,32,114,101,115,
111,117,114,99,101,115,32,97,114,101,32,114,101,97,100,121,32,102,111,114,32,97,32,112,97,110,101,108,34,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,108,117,97,80,97,110,101,108,83,97,118,101,83,116,97,116,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,67,116,114,108,114,32,115,116,97,116,101,32,105,115,32,115,97,118,101,
100,32,40,68,65,87,32,115,97,118,101,115,32,97,32,112,114,111,106,101,99,116,41,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,
34,108,117,97,80,97,110,101,108,82,101,115,116,111,114,101,83,116,97,116,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,67,116,114,108,114,32,105,115,32,108,111,97,100,101,100,32,40,68,
65,87,32,111,112,101,110,115,32,97,32,112,114,111,106,101,99,116,41,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,108,117,97,80,97,110,101,108,77,105,100,105,83,110,97,112,115,104,111,116,80,114,101,34,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,98,101,102,111,114,101,32,97,32,77,73,68,73,32,115,110,97,112,115,104,111,116,32,
105,115,32,115,101,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,
108,117,97,80,97,110,101,108,77,105,100,105,83,110,97,112,115,104,111,116,80,111,115,116,34,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,97,102,116,101,114,32,97,32,77,73,68,73,32,115,110,97,112,115,104,111,116,32,104,97,
115,32,98,101,101,110,32,115,101,110,116,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,108,117,97,84,114,97,110,115,80,114,111,99,101,115,115,34,9,9,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,112,114,111,99,101,115,115,34,32,9,32,32,32,32,9,9,9,116,121,112,101,61,34,76,117,97,77,
101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,97,110,115,68,97,116,97,85,110,112,97,99,107,34,9,9,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,100,97,
116,97,32,117,110,112,97,99,107,105,110,103,34,32,9,9,9,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,97,110,115,68,97,116,97,80,97,99,107,34,9,9,32,32,32,32,32,32,
32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,100,97,116,97,32,112,97,99,107,105,110,103,34,32,9,9,32,32,32,32,9,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,
61,34,108,117,97,84,114,97,110,115,82,101,113,117,101,115,116,34,9,9,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,114,101,113,117,101,115,116,34,32,9,9,9,32,32,32,32,32,32,32,32,116,121,112,101,61,34,
76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,97,110,115,78,97,109,101,68,97,116,97,34,9,9,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,
103,101,116,32,110,97,109,101,34,32,9,9,9,32,32,32,32,9,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,97,110,115,82,101,113,117,101,115,116,68,97,116,97,34,9,32,32,
32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,103,101,116,32,114,101,113,117,101,115,116,32,100,97,116,97,34,32,9,9,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,
60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,97,110,115,82,101,115,112,111,110,115,101,68,97,116,97,34,9,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,103,101,116,32,114,101,115,112,111,110,115,
101,32,100,97,116,97,34,32,32,9,9,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,97,110,115,67,111,110,102,68,97,116,97,34,9,9,32,32,32,32,32,32,32,32,32,32,116,101,
120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,103,101,116,32,99,111,110,102,105,114,109,97,116,105,111,110,32,100,97,116,97,34,32,32,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,108,117,97,84,114,97,110,115,84,105,109,101,111,117,116,34,9,9,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,32,116,105,109,101,111,117,116,32,104,97,110,100,108,101,114,34,32,9,9,9,116,121,
112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,84,114,97,110,115,73,110,102,111,34,9,9,9,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,
32,105,110,102,111,114,109,97,116,105,111,110,32,113,117,101,114,121,34,9,9,32,32,32,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,67,116,114,108,114,83,97,118,101,83,116,
97,116,101,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,67,116,114,108,114,32,115,116,97,116,101,32,105,115,32,115,97,118,101,100,32,40,68,65,87,32,115,97,118,101,115,32,97,32,112,
114,111,106,101,99,116,41,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,67,116,114,108,114,82,101,115,116,111,114,101,83,116,97,116,101,34,32,32,32,32,32,32,32,32,32,
32,32,32,32,116,101,120,116,61,34,67,97,108,108,101,100,32,119,104,101,110,32,67,116,114,108,114,32,105,115,32,108,111,97,100,101,100,32,40,68,65,87,32,111,112,101,110,115,32,97,32,112,114,111,106,101,99,116,41,34,32,116,121,112,101,61,34,76,117,97,77,
101,116,104,111,100,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,117,105,76,117,97,67,111,110,115,111,108,101,73,110,112,117,116,82,101,109,111,118,101,65,102,116,101,114,82,117,110,34,32,32,116,101,120,116,61,34,67,108,101,97,114,32,99,
111,110,115,111,108,101,32,105,110,112,117,116,32,97,102,116,101,114,32,101,120,101,99,117,116,101,34,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,108,117,97,65,117,100,105,111,80,114,111,99,
101,115,115,66,108,111,99,107,34,32,32,32,32,32,32,32,32,32,32,32,32,32,116,101,120,116,61,34,65,85,68,73,79,58,32,109,97,105,110,32,112,114,111,99,101,115,115,32,98,108,111,99,107,32,102,111,114,32,116,104,101,32,97,117,100,105,111,47,109,105,100,105,
32,116,104,114,101,97,100,34,32,116,121,112,101,61,34,76,117,97,77,101,116,104,111,100,34,32,47,62,13,10,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,84,114,101,101,83,116,97,116,101,34,32,32,32,32,32,9,9,9,
9,32,32,32,32,116,101,120,116,61,34,84,114,101,101,32,115,116,97,116,101,34,32,32,32,32,32,9,9,9,9,9,9,9,9,9,9,9,9,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,
114,97,114,121,67,97,110,71,101,116,73,116,101,109,34,32,32,32,32,9,9,9,9,32,32,32,32,116,101,120,116,61,34,67,97,110,32,103,101,116,32,105,116,101,109,34,32,32,32,9,9,9,9,9,9,9,9,9,9,9,9,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,
105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,67,97,110,83,101,110,100,73,116,101,109,34,32,32,32,9,9,9,9,32,32,32,32,116,101,120,116,61,34,67,97,110,32,115,101,110,100,32,105,116,101,109,34,32,32,9,9,9,9,9,9,9,9,9,9,9,9,116,
121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,65,108,108,80,114,111,103,114,97,109,115,83,117,112,112,111,114,116,34,32,32,9,32,32,32,32,116,101,120,116,61,34,68,101,
118,105,99,101,32,115,117,112,112,111,114,116,115,32,115,101,110,100,105,110,103,32,97,108,108,32,112,114,111,103,114,97,109,32,100,97,116,97,32,105,110,32,111,110,101,32,114,101,113,117,101,115,116,34,32,32,9,116,121,112,101,61,34,66,111,111,108,34,
32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,83,101,110,100,83,110,97,112,65,102,116,101,114,80,67,104,103,34,9,9,9,32,32,32,32,116,101,120,116,61,34,83,101,110,100,32,77,73,68,73,32,115,110,97,112,
115,104,111,116,32,97,102,116,101,114,32,99,104,97,110,103,105,110,103,32,112,114,111,103,114,97,109,115,34,32,9,9,9,9,9,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,
114,121,80,97,114,97,109,101,116,101,114,73,110,100,101,120,80,114,111,112,101,114,116,121,34,9,32,32,116,101,120,116,61,34,87,104,97,116,32,112,114,111,112,101,114,116,121,32,104,111,108,100,115,32,116,104,101,32,112,97,114,97,109,101,116,101,114,32,
105,110,100,101,120,32,105,110,32,116,104,101,32,100,117,109,112,32,100,97,116,97,34,32,9,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,77,105,100,105,80,114,111,
103,114,97,109,67,104,97,110,103,101,67,111,110,116,114,111,108,34,32,32,116,101,120,116,61,34,83,101,110,100,32,77,73,68,73,32,112,114,111,103,114,97,109,32,99,104,97,110,103,101,32,101,118,101,110,116,32,97,102,116,101,114,32,99,104,97,110,103,105,
110,103,32,116,104,101,32,112,114,111,103,114,97,109,34,32,9,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,68,101,102,97,117,108,116,66,97,110,107,78,97,109,101,
34,9,9,9,9,32,32,32,32,116,101,120,116,61,34,68,101,102,97,117,108,116,32,98,97,110,107,32,110,97,109,101,32,40,84,104,105,115,32,117,115,101,115,32,116,104,101,32,67,32,115,116,114,102,116,105,109,101,41,34,9,9,9,9,9,116,121,112,101,61,34,84,101,120,
116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,68,101,102,97,117,108,116,80,114,111,103,114,97,109,78,97,109,101,34,9,9,9,32,32,116,101,120,116,61,34,68,101,102,97,117,108,116,32,98,97,110,107,
32,110,97,109,101,32,40,84,104,105,115,32,117,115,101,115,32,116,104,101,32,67,32,115,116,114,102,116,105,109,101,41,34,9,9,9,9,9,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,
114,97,114,121,68,101,102,97,117,108,116,83,110,97,112,115,104,111,116,78,97,109,101,34,32,32,32,32,32,32,32,116,101,120,116,61,34,68,101,102,97,117,108,116,32,115,110,97,112,115,104,111,116,32,110,97,109,101,32,40,84,104,105,115,32,117,115,101,115,32,
116,104,101,32,67,32,115,116,114,102,116,105,109,101,41,34,9,9,9,9,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,109,105,100,105,76,105,98,114,97,114,121,67,117,115,116,111,109,82,101,113,117,101,115,
116,115,34,32,32,9,32,32,32,32,32,32,32,32,116,101,120,116,61,34,67,117,115,116,111,109,32,114,101,113,117,101,115,116,32,116,111,32,97,100,100,32,116,111,32,116,104,101,32,109,101,110,117,32,98,97,114,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,32,32,32,32,9,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,34,32,47,62,13,10,13,10,9,60,33,45,45,32,84,114,97,110,115,97,99,116,105,111,110,32,45,45,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,34,32,32,32,
32,32,32,32,9,9,9,32,32,32,32,32,32,116,101,120,116,61,34,84,114,97,110,115,97,99,116,105,111,110,34,9,9,9,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,82,101,97,100,79,110,108,121,34,32,47,62,
13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,67,97,112,34,9,9,9,9,32,32,32,32,32,32,32,32,116,101,120,116,61,34,87,104,97,116,32,108,105,98,114,97,114,121,32,105,116,101,109,115,32,99,97,110,32,116,104,105,115,32,116,114,97,110,115,
97,99,116,105,111,110,32,104,97,110,100,108,101,34,9,116,121,112,101,61,34,77,117,108,116,105,76,105,110,101,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,77,111,100,73,100,120,80,114,111,112,34,32,32,32,32,32,32,32,32,
116,101,120,116,61,34,77,111,100,117,108,97,116,111,114,32,112,114,111,112,101,114,116,121,32,102,111,114,32,105,110,100,101,120,105,110,103,32,100,97,116,97,34,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,84,101,120,116,34,32,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,113,70,111,114,109,117,108,97,80,114,101,102,105,120,34,32,9,116,101,120,116,61,34,82,101,113,117,101,115,116,32,102,111,114,109,117,108,97,32,112,114,101,102,105,120,34,9,9,32,32,32,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,83,121,115,69,120,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,113,70,111,114,109,117,108,97,83,117,102,102,105,120,34,32,9,116,101,120,
116,61,34,82,101,113,117,101,115,116,32,102,111,114,109,117,108,97,32,115,117,102,102,105,120,34,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,83,121,115,69,120,34,32,47,62,13,10,32,32,60,105,100,32,110,97,
109,101,61,34,116,114,97,110,115,82,101,113,68,97,116,97,76,101,110,34,9,9,32,32,32,32,116,101,120,116,61,34,82,101,113,117,101,115,116,32,100,97,116,97,32,108,101,110,103,116,104,34,9,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,54,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,115,112,70,111,114,109,
117,108,97,80,114,101,102,105,120,34,9,116,101,120,116,61,34,82,101,115,112,111,110,115,101,32,102,111,114,109,117,108,97,32,112,114,101,102,105,120,34,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,83,121,115,69,
120,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,115,112,70,111,114,109,117,108,97,83,117,102,102,105,120,34,32,116,101,120,116,61,34,82,101,115,112,111,110,115,101,32,102,111,114,109,117,108,97,32,115,117,102,
102,105,120,34,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,83,121,115,69,120,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,115,112,68,97,116,97,76,101,110,34,32,32,32,32,
32,32,32,116,101,120,116,61,34,82,101,115,112,111,110,115,101,32,100,97,116,97,32,108,101,110,103,116,104,34,9,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,
34,48,34,32,109,97,120,61,34,54,53,53,54,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,115,112,68,97,116,97,67,104,101,99,107,34,32,32,32,32,32,116,101,120,116,61,34,82,101,115,112,
111,110,115,101,32,100,97,116,97,32,99,104,101,99,107,34,9,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,
115,112,83,105,122,101,67,104,101,99,107,34,32,32,32,32,32,116,101,120,116,61,34,82,101,115,112,111,110,115,101,32,115,105,122,101,32,99,104,101,99,107,34,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,
101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,67,111,110,102,70,111,114,109,117,108,97,80,114,101,102,105,120,34,32,116,101,120,116,61,34,67,111,110,102,105,114,109,97,116,105,111,110,32,102,111,
114,109,117,108,97,32,112,114,101,102,105,120,34,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,83,121,115,69,120,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,67,111,110,102,70,111,114,109,
117,108,97,83,117,102,102,105,120,34,32,116,101,120,116,61,34,67,111,110,102,105,114,109,97,116,105,111,110,32,102,111,114,109,117,108,97,32,115,117,102,102,105,120,34,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,83,121,
115,69,120,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,67,111,110,102,68,97,116,97,76,101,110,34,32,9,9,32,32,116,101,120,116,61,34,67,111,110,102,105,114,109,97,116,105,111,110,32,100,97,116,97,32,108,101,110,103,116,
104,34,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,54,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,
97,109,101,61,34,116,114,97,110,115,82,101,113,69,120,105,115,116,115,34,32,32,32,32,32,32,32,9,116,101,120,116,61,34,82,101,113,117,101,115,116,32,101,120,105,115,116,115,34,9,9,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,
32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,115,112,69,120,105,115,116,115,34,32,32,32,32,32,32,32,32,116,101,120,116,61,34,82,101,115,112,111,110,115,101,32,101,120,
105,115,116,115,34,9,9,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,67,111,110,102,69,120,105,115,116,115,
34,9,32,32,32,32,32,32,116,101,120,116,61,34,67,111,110,102,105,114,109,97,116,105,111,110,32,101,120,105,115,116,115,34,9,9,9,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,116,121,112,101,61,34,66,111,111,108,34,32,47,62,13,10,32,
32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,115,112,78,97,109,101,76,101,110,34,9,9,32,32,32,32,116,101,120,116,61,34,82,101,115,112,111,110,115,101,32,110,97,109,101,32,108,101,110,103,116,104,34,9,9,9,116,121,112,101,61,34,78,117,
109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,54,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,115,112,78,97,109,101,79,102,102,115,101,116,34,9,32,32,
116,101,120,116,61,34,82,101,115,112,111,110,115,101,32,110,97,109,101,32,111,102,102,115,101,116,34,9,9,9,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,54,53,34,32,105,110,116,61,34,49,34,
32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,113,78,97,109,101,76,101,110,34,9,9,32,32,32,32,116,101,120,116,61,34,82,101,113,117,101,115,116,32,110,97,109,101,32,108,101,110,103,116,104,34,9,9,9,116,121,112,101,61,
34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,54,53,34,32,105,110,116,61,34,49,34,32,47,62,13,10,32,32,60,105,100,32,110,97,109,101,61,34,116,114,97,110,115,82,101,113,78,97,109,101,79,102,102,115,101,116,34,
9,9,32,32,116,101,120,116,61,34,82,101,113,117,101,115,116,32,110,97,109,101,32,111,102,102,115,101,116,34,9,9,9,116,121,112,101,61,34,78,117,109,101,114,105,99,34,32,109,105,110,61,34,48,34,32,109,97,120,61,34,54,53,53,54,53,34,32,105,110,116,61,34,
49,34,32,47,62,13,10,13,10,32,32,60,33,45,45,32,67,111,110,115,116,97,110,116,115,32,117,115,101,100,32,105,110,32,112,114,111,112,101,114,116,105,101,115,32,45,45,62,13,10,32,32,60,99,111,110,115,116,97,110,116,115,62,13,10,32,32,32,32,60,99,111,110,
115,116,32,110,97,109,101,61,34,106,117,115,116,105,102,105,99,97,116,105,111,110,34,32,32,32,32,32,118,97,108,117,101,61,34,108,101,102,116,61,49,44,114,105,103,104,116,61,50,44,104,111,114,105,122,111,110,116,97,108,108,121,67,101,110,116,114,101,100,
61,52,44,116,111,112,61,56,44,98,111,116,116,111,109,61,49,54,44,118,101,114,116,105,99,97,108,108,121,67,101,110,116,114,101,100,61,51,50,44,104,111,114,105,122,111,110,116,97,108,108,121,74,117,115,116,105,102,105,101,100,61,54,52,44,99,101,110,116,
114,101,100,61,51,54,44,99,101,110,116,114,101,100,76,101,102,116,61,51,51,44,99,101,110,116,114,101,100,82,105,103,104,116,61,51,52,44,99,101,110,116,114,101,100,84,111,112,61,49,50,44,99,101,110,116,114,101,100,66,111,116,116,111,109,61,50,48,44,116,
111,112,76,101,102,116,61,57,44,116,111,112,82,105,103,104,116,61,49,48,44,98,111,116,116,111,109,76,101,102,116,61,49,55,44,98,111,116,116,111,109,82,105,103,104,116,61,49,56,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,
103,114,97,100,105,101,110,116,34,32,32,32,32,32,32,32,32,32,32,118,97,108,117,101,61,34,78,111,110,101,61,48,44,86,101,114,116,105,99,97,108,61,49,44,72,111,114,105,122,111,110,116,97,108,61,50,44,82,97,100,105,97,108,61,51,34,32,47,62,13,10,32,32,32,
32,60,99,111,110,115,116,32,110,97,109,101,61,34,105,109,97,103,101,76,97,121,111,117,116,34,32,32,32,32,32,32,32,118,97,108,117,101,61,34,116,105,108,101,61,56,49,57,50,44,120,76,101,102,116,61,49,44,120,82,105,103,104,116,61,50,44,120,77,105,100,61,
52,44,121,84,111,112,61,56,44,121,66,111,116,116,111,109,61,49,54,44,121,77,105,100,61,51,50,44,115,116,114,101,116,99,104,84,111,70,105,116,61,54,52,44,102,105,108,108,68,101,115,116,105,110,97,116,105,111,110,61,49,50,56,44,111,110,108,121,82,101,100,
117,99,101,73,110,83,105,122,101,61,50,53,54,44,111,110,108,121,73,110,99,114,101,97,115,101,73,110,83,105,122,101,61,53,49,50,44,99,101,110,116,114,101,100,61,51,54,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,114,101,
108,97,116,105,118,101,80,111,115,105,116,105,111,110,34,32,32,118,97,108,117,101,61,34,110,111,110,101,44,116,111,112,44,98,111,116,116,111,109,44,108,101,102,116,44,114,105,103,104,116,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,
101,61,34,109,105,100,105,73,110,112,117,116,67,104,97,110,110,101,108,34,32,32,118,97,108,117,101,61,34,65,108,108,32,67,104,97,110,110,101,108,115,61,48,44,67,104,97,110,110,101,108,32,49,61,49,44,67,104,97,110,110,101,108,32,50,61,50,44,67,104,97,
110,110,101,108,32,51,61,51,44,67,104,97,110,110,101,108,32,52,61,52,44,67,104,97,110,110,101,108,32,53,61,53,44,67,104,97,110,110,101,108,32,54,61,54,44,67,104,97,110,110,101,108,32,55,61,55,44,67,104,97,110,110,101,108,32,56,61,56,44,67,104,97,110,
110,101,108,32,57,61,57,44,67,104,97,110,110,101,108,32,49,48,61,49,48,44,67,104,97,110,110,101,108,32,49,49,61,49,49,44,67,104,97,110,110,101,108,32,49,50,61,49,50,44,67,104,97,110,110,101,108,32,49,51,61,49,51,44,67,104,97,110,110,101,108,32,49,52,
61,49,52,44,67,104,97,110,110,101,108,32,49,53,61,49,53,44,67,104,97,110,110,101,108,32,49,54,61,49,54,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,109,105,100,105,79,117,116,112,117,116,67,104,97,110,110,101,108,34,32,
118,97,108,117,101,61,34,67,104,97,110,110,101,108,32,49,61,49,44,67,104,97,110,110,101,108,32,50,61,50,44,67,104,97,110,110,101,108,32,51,61,51,44,67,104,97,110,110,101,108,32,52,61,52,44,67,104,97,110,110,101,108,32,53,61,53,44,67,104,97,110,110,101,
108,32,54,61,54,44,67,104,97,110,110,101,108,32,55,61,55,44,67,104,97,110,110,101,108,32,56,61,56,44,67,104,97,110,110,101,108,32,57,61,57,44,67,104,97,110,110,101,108,32,49,48,61,49,48,44,67,104,97,110,110,101,108,32,49,49,61,49,49,44,67,104,97,110,
110,101,108,32,49,50,61,49,50,44,67,104,97,110,110,101,108,32,49,51,61,49,51,44,67,104,97,110,110,101,108,32,49,52,61,49,52,44,67,104,97,110,110,101,108,32,49,53,61,49,53,44,67,104,97,110,110,101,108,32,49,54,61,49,54,34,32,47,62,13,10,32,32,32,32,60,
99,111,110,115,116,32,110,97,109,101,61,34,109,111,117,115,101,67,117,114,115,111,114,34,32,32,32,32,32,32,32,118,97,108,117,101,61,34,80,97,114,101,110,116,67,117,114,115,111,114,61,48,44,78,111,67,117,114,115,111,114,61,49,44,78,111,114,109,97,108,
61,50,44,87,97,105,116,67,117,114,115,111,114,61,51,44,73,66,101,97,109,67,117,114,115,111,114,61,52,44,67,114,111,115,115,104,97,105,114,67,117,114,115,111,114,61,53,44,67,111,112,121,105,110,103,67,117,114,115,111,114,61,54,44,80,111,105,110,116,105,
110,103,72,97,110,100,67,117,114,115,111,114,61,55,44,68,114,97,103,103,105,110,103,72,97,110,100,67,117,114,115,111,114,61,56,44,76,101,102,116,82,105,103,104,116,82,101,115,105,122,101,67,117,114,115,111,114,61,57,44,85,112,68,111,119,110,82,101,115,
105,122,101,67,117,114,115,111,114,61,49,48,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,105,109,97,103,101,69,102,102,101,99,116,34,32,32,32,32,32,32,32,118,97,108,117,101,61,34,78,111,32,69,102,102,101,99,116,44,71,108,
111,119,44,83,104,97,100,111,119,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,100,121,110,97,109,105,99,67,111,110,116,101,110,116,34,32,32,32,32,118,97,108,117,101,61,34,78,111,110,101,61,48,44,77,111,100,117,108,97,116,
111,114,32,76,105,115,116,61,49,44,70,105,108,101,115,121,115,116,101,109,61,50,44,66,97,110,107,44,80,114,111,103,114,97,109,61,51,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,108,99,100,70,111,110,116,115,34,32,32,32,
32,32,32,32,32,32,32,118,97,108,117,101,61,34,76,67,68,61,48,44,68,105,103,105,116,97,108,45,55,61,49,44,68,111,116,116,121,45,83,104,97,100,111,119,61,50,44,90,88,56,49,61,51,44,73,110,118,97,115,105,111,110,50,48,48,48,61,52,44,68,105,103,105,116,61,
53,44,67,111,109,112,117,116,101,114,102,111,110,116,61,54,44,69,108,101,99,116,114,111,110,105,99,45,72,105,103,104,119,97,121,45,83,105,103,110,61,55,44,75,97,114,109,97,116,105,99,45,65,114,99,97,100,101,61,56,44,54,48,115,101,107,117,110,116,105,
97,61,57,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,105,110,116,101,114,110,97,108,70,117,110,99,116,105,111,110,115,34,32,32,32,32,118,97,108,117,101,61,34,45,45,32,78,111,110,101,61,48,44,77,73,68,73,32,68,101,118,105,
99,101,32,83,101,108,101,99,116,105,111,110,61,49,44,77,73,68,73,32,77,111,110,105,116,111,114,61,50,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,99,111,109,112,111,110,101,110,116,108,111,111,107,97,110,100,102,101,101,
108,34,32,118,97,108,117,101,61,34,68,101,102,97,117,108,116,44,86,52,32,76,105,103,104,116,44,86,52,32,71,114,101,121,44,86,52,32,68,97,114,107,44,86,52,32,77,105,100,110,105,103,104,116,44,86,52,32,74,101,116,66,108,97,99,107,44,86,52,32,89,97,109,
68,88,44,86,52,32,65,107,65,80,67,44,86,52,32,65,107,77,80,67,44,86,52,32,76,101,120,105,66,108,117,101,44,86,52,32,75,117,114,122,71,114,101,101,110,44,86,52,32,75,111,114,71,114,101,121,44,86,52,32,75,111,114,71,111,108,100,44,86,52,32,65,114,116,117,
114,79,114,97,110,103,101,44,86,52,32,65,105,114,97,71,114,101,101,110,44,86,51,44,86,50,44,86,49,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,108,111,111,107,97,110,100,102,101,101,108,115,99,104,101,109,101,115,34,32,
32,32,118,97,108,117,101,61,34,86,52,32,76,105,103,104,116,44,86,52,32,71,114,101,121,44,86,52,32,68,97,114,107,44,86,52,32,77,105,100,110,105,103,104,116,44,86,52,32,74,101,116,66,108,97,99,107,44,86,52,32,89,97,109,68,88,44,86,52,32,65,107,65,80,67,
44,86,52,32,65,107,77,80,67,44,86,52,32,76,101,120,105,66,108,117,101,44,86,52,32,75,117,114,122,71,114,101,101,110,44,86,52,32,75,111,114,71,114,101,121,44,86,52,32,75,111,114,71,111,108,100,44,86,52,32,65,114,116,117,114,79,114,97,110,103,101,44,86,
52,32,65,105,114,97,71,114,101,101,110,44,86,51,44,86,50,44,86,49,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,108,111,111,107,97,110,100,102,101,101,108,34,32,32,32,32,32,32,32,32,32,32,118,97,108,117,101,61,34,86,52,44,
86,51,44,86,50,44,86,49,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,99,111,108,111,117,114,83,99,104,101,109,101,115,34,32,32,32,32,32,32,32,32,118,97,108,117,101,61,34,68,101,102,97,117,108,116,44,76,105,103,104,116,44,
71,114,101,121,44,68,97,114,107,44,77,105,100,110,105,103,104,116,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,99,116,114,108,114,67,111,108,111,117,114,83,99,104,101,109,101,115,34,32,32,32,118,97,108,117,101,61,34,76,
105,103,104,116,44,71,114,101,121,44,68,97,114,107,44,77,105,100,110,105,103,104,116,44,74,101,116,66,108,97,99,107,44,89,97,109,68,88,44,65,107,65,80,67,44,65,107,77,80,67,44,76,101,120,105,66,108,117,101,44,75,117,114,122,71,114,101,101,110,44,75,111,
114,71,114,101,121,44,75,111,114,71,111,108,100,44,65,114,116,117,114,79,114,97,110,103,101,44,65,105,114,97,71,114,101,101,110,34,32,47,62,13,10,32,32,32,32,60,99,111,110,115,116,32,110,97,109,101,61,34,112,108,117,103,84,121,112,101,34,32,32,32,32,
32,32,32,32,118,97,108,117,101,61,34,73,110,115,116,114,117,109,101,110,116,124,68,114,117,109,44,73,110,115,116,114,117,109,101,110,116,124,80,105,97,110,111,44,73,110,115,116,114,117,109,101,110,116,124,83,121,110,116,104,44,73,110,115,116,114,117,
109,101,110,116,124,84,111,111,108,115,44,70,120,124,68,101,108,97,121,44,70,120,124,68,105,115,116,111,114,116,105,111,110,44,70,120,124,68,121,110,97,109,105,99,115,44,70,120,124,69,81,44,70,120,124,70,105,108,116,101,114,44,70,120,124,73,110,115,116,
114,117,109,101,110,116,44,70,120,124,77,97,115,116,101,114,105,110,103,44,70,120,124,77,111,100,117,108,97,116,105,111,110,44,70,120,124,80,105,116,99,104,32,83,104,105,102,116,44,70,120,124,82,101,115,116,111,114,97,116,105,111,110,44,70,120,124,82,
101,118,101,114,98,44,70,120,124,83,112,97,116,105,97,108,44,70,120,124,83,121,110,116,104,44,70,120,124,84,111,111,108,115,34,32,47,62,13,10,32,32,60,47,99,111,110,115,116,97,110,116,115,62,13,10,13,10,60,47,99,116,114,108,114,73,100,115,62,13,10,0,0 };

const char* CtrlrIDs_xml = (const char*) temp_binary_data_132;

//================== CtrlrLuaClassTemplates.xml ==================
static const unsigned char temp_binary_data_133[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
"<classes>\r\n"
"  <class name=\"LookAndFeel_V3\">\r\n"
"\r\n"
"  </class>\r\n"
"</methods>\r\n";

const char* CtrlrLuaClassTemplates_xml = (const char*) temp_binary_data_133;

//================== CtrlrLuaMethodTemplates.xml ==================
static const unsigned char temp_binary_data_134[] =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
"<methods>\r\n"
"  <luaMethod name=\"luaModulatorValueChange\">\r\n"
"    --\r\n"
"    -- Called when a modulator value changes\r\n"
"    -- @mod   http://ctrlr.org/api/class_ctrlr_modulator.html\r\n"
"    -- @value    new numeric value of the modulator\r\n"
"    --\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] mod, --[[ number --]] value, --[[ number --]] source)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"componentLuaMouseMoved\">\r\n"
"    --\r\n"
"    -- Called when the mouse moves over a component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"componentLuaMouseDown\">\r\n"
"    --\r\n"
"    -- Called when a mouse is down on this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"componentLuaMouseUp\">\r\n"
"    --\r\n"
"    -- Called when a mouse is up on this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"  \r\n"
"  <luaMethod name=\"componentLuaMouseDrag\">\r\n"
"    --\r\n"
"    -- Called when a mouse is dragged (down + left click) over this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"componentLuaMouseDoubleClick\">\r\n"
"    --\r\n"
"    -- Called when a double-click event occurs on this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"  \r\n"
"  <luaMethod name=\"componentLuaMouseEnter\">\r\n"
"    --\r\n"
"    -- Called when a mouse enters this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"  \r\n"
"    <luaMethod name=\"componentLuaMouseExit\">\r\n"
"    --\r\n"
"    -- Called when a mouse leaves this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomResizedCallback\">\r\n"
"    --\r\n"
"    -- Called when a component is resized\r\n"
"    -- @comp\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomPaintCallback\">\r\n"
"    --\r\n"
"    -- Called when a component needs repainting\r\n"
"    -- @comp\r\n"
"    -- @g    http://ctrlr.org/api/class_ctrlr_lua_graphics.html\r\n"
"    --              see also http://www.rawmaterialsoftware.com/juce/api/classGraphics.html\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ CtrlrComponent --]] g)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomPaintOverChildrenCallback\">\r\n"
"    --\r\n"
"    -- Called when you want to paint over all other elements of the component\r\n"
"    -- @comp\r\n"
"    -- @g    http://ctrlr.org/api/class_ctrlr_lua_graphics.html\r\n"
"    --              see also http://www.rawmaterialsoftware.com/juce/api/classGraphics.html\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ Graphics --]] g)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseDownCallback\">\r\n"
"    --\r\n"
"    -- Called when a mouse DOWN (any mouse button) event happens on a component\r\n"
"    -- @comp\r\n"
"    -- @event   http://www.rawmaterialsoftware.com/juce/api/classMouseEvent.html\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseUpCallback\">\r\n"
"    --\r\n"
"    -- Called when a mouse UP (any mouse button) event happens on a component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseDoubleClickCallback\">\r\n"
"    --\r\n"
"    -- Called when a mouse DOUBLE CLICK (any mouse button) event happens on a component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseEnterCallback\">\r\n"
"    --\r\n"
"    -- Called when the mouse cursors enters a component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseWheelMoveCallback\">\r\n"
"    --\r\n"
"    -- Called when the mouse wheel is moved\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event, --[[ number --]] incX, --[[ number --]] incY)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseExitCallback\">\r\n"
"    --\r\n"
"    -- Called when the mouse cursors exits a component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseDragCallback\">\r\n"
"    --\r\n"
"    -- Called when the mouse is dragged over a component (mouse button is down and the mouse is moved)\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomMouseMoveCallback\">\r\n"
"    --\r\n"
"    -- Called when the mouse moves over a component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ MouseEvent --]] event)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomKeyDownCallback\">\r\n"
"    --\r\n"
"    -- Called when the a key is pressed and the component has focus\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[KeyPress --]] keyEvent, --[[ Component --]] originatingComponent)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomKeyStateChangedCallback\">\r\n"
"    --\r\n"
"    -- Called when the a key state changes\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] comp, --[[ bool --]] isKeyDown, --[[ Component --]] originatingComponent)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomIsInterestedInDragSourceCallback\">\r\n"
"    --\r\n"
"    -- This method needs to return information if this component\r\n"
"    -- is interested in a drag and drop event\r\n"
"    --\r\n"
"    -- the parameter is a DragAndDropSourceDetails object\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] component, --[[ DragAndDropSourceDetails --]] dragSourceDetails)\r\n"
"        return (1)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomItemDragEnterCallback\">\r\n"
"    --\r\n"
"    -- Called when a DragAndDrop event enters this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] component, --[[ DragAndDropSourceDetails --]] dragSourceDetails)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomItemDragMoveCallback\">\r\n"
"    --\r\n"
"    -- Called when a DragAndDrop event occurs over this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] component, --[[ DragAndDropSourceDetails --]] dragSourceDetails)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomItemDragExitCallback\">\r\n"
"    --\r\n"
"    -- Called when a DragAndDrop event exits this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] component, --[[ DragAndDropSourceDetails --]] dragSourceDetails)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomStartDraggingCallback\">\r\n"
"    --\r\n"
"    -- This method needs to return source information for a drag event\r\n"
"    -- the return value is a DragAndDropSourceDetails object\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] component, --[[ MouseEvent --]] mouseEvent)\r\n"
"        details = DragAndDropSourceDetails()\r\n"
"        return (details)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiCustomItemDroppedCallback\">\r\n"
"    --\r\n"
"    -- Called when something is dropped on this component\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrComponent --]] component, --[[ DragAndDropSourceDetails --]] dragSourceDetails)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiTabsCurrentTabChanged\">\r\n"
"    --\r\n"
"    -- Called when the current tab in an uiTabs component is changed\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] mod, --[[ number --]] tabIndex)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelPaintBackground\">\r\n"
"    --\r\n"
"    -- Called when the panel background needs repainting\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrPanelCanvas --]] canvas, --[[ Graphics --]] g)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"  \r\n"
"   <luaMethod name=\"luaViewPortResized\">\r\n"
"    --\r\n"
"    -- Called when the ViewPort is resized\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrPanelEditor --]] panel)\r\n"
"    end\r\n"
"  </luaMethod> \r\n"
"  \r\n"
"  <luaMethod name=\"luaPanelResized\">\r\n"
"    --\r\n"
"    -- Called when the panel is resized\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrPanelEditor --]] panel)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelSaved\">\r\n"
"    --\r\n"
"    -- Called when the panel is saved\r\n"
"    -- @saveType         A number that indicates the type of file beeing saved http://ctrlr.org/api/class_ctrlr_panel.html\r\n"
"    -- @destinationFile  A file object http://ctrlr.org/api/class_ctrlr_lua_file.html\r\n"
"    --                      see more http://www.rawmaterialsoftware.com/juce/api/classFile.html\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ number --]] saveType, --[[ File --]] destinationFile)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelBeforeLoad\">\r\n"
"    --\r\n"
"    -- Called when the panel is created, no modulators will exist at this point\r\n"
"    -- consider this the panels constructor\r\n"
"    --\r\n"
"    __method_name = function()\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelProgramChanged\">\r\n"
"    --\r\n"
"    -- Called after a program change has been sent from the panel\r\n"
"    -- it's called after to give the user a chance to update the panel state based on the state of the device\r\n"
"    --\r\n"
"    -- @program program number\r\n"
"    -- @bankLsb bank number (LSB part CC32)\r\n"
"    -- @bankMsb bank number (MSB part CC0)\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ number --]] program, --[[ number --]] bankLsb, --[[ number --]] bankMsb)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelModulatorValueChanged\">\r\n"
"    --\r\n"
"    -- Called when any modulator changes on the panel\r\n"
"    --\r\n"
"    -- @program modulator that changed\r\n"
"    -- @value new modulator value\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ number --]] value)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelMidiReceived\">\r\n"
"    --\r\n"
"    -- Called when a panel receives a midi message (does not need to match any modulator mask)\r\n"
"    -- @midi   CtrlrMidiMessage object\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrMidiMessage --]] midi)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelMidiMultiReceived\">\r\n"
"    --\r\n"
"    -- Called when a panel receives a multi MIDI message\r\n"
"    -- (needs to match a modulator mask)\r\n"
"    -- @midi   CtrlrMidiMessage object, this will contain all the midi messages\r\n"
"    --         that make up this multi message\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrMidiMessage --]] multiMidi)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelOSCReceived\">\r\n"
"      --\r\n"
"      -- Called when a panel receives a OSC message\r\n"
"      -- @path - osc path\r\n"
"      -- @types - a string representing what types are included in the incoming messages\r\n"
"      -- @arguments - a lua table of all incoming arguments\r\n"
"      --\r\n"
"\r\n"
"      __method_name = function(--[[ string --]] path, --[[ string --]] types, --[[ table --]] arguments)\r\n"
"      end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiLabelChangedCbk\">\r\n"
"    --\r\n"
"    -- Called when the contents of a Label are changed\r\n"
"    -- @label\r\n"
"    -- @newContent    a string that the label now contains\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrLabel --]] label, --[[ String --]] newContent)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiFileListFileClicked\">\r\n"
"    --\r\n"
"    -- Called when a file is clicked\r\n"
"    --\r\n"
"    -- @modulator the modulator the event occured on\r\n"
"    -- @file      a File object that represents the clicked file\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ File --]] file)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiFileListFileDoubleClicked\">\r\n"
"    --\r\n"
"    -- Called when a file is double clicked\r\n"
"    --\r\n"
"    -- @modulator the modulator the event occured on\r\n"
"    -- @file      a File object that represents the double clicked file\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ File --]] file)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiListBoxItemClicked\">\r\n"
"    --\r\n"
"    -- Called when an item is clicked\r\n"
"    --\r\n"
"    -- @modulator the modulator the event occured on\r\n"
"    -- @value      a integer that represents the clicked item\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ number --]] value)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiListBoxItemDoubleClicked\">\r\n"
"    --\r\n"
"    -- Called when an item is double clicked\r\n"
"    --\r\n"
"    -- @modulator the modulator the event occured on\r\n"
"    -- @value      an integer that represents the double clicked item\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ number --]] value)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiListBoxItemDeleteKeyPressed\">\r\n"
"    --\r\n"
"    -- Called when a selected item has delete key pressed\r\n"
"    --\r\n"
"    -- @modulator the modulator the event occured on\r\n"
"    -- @value      a integer that represents the selected item\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ number --]] value)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiListBoxItemReturnKeyPressed\">\r\n"
"    --\r\n"
"    -- Called when a selected item has return key pressed\r\n"
"    --\r\n"
"    -- @modulator the modulator the event occured on\r\n"
"    -- @value      an integer that represents the selected item\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ number --]] value)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiWaveFormThumbnailChangedCallback\">\r\n"
"    --\r\n"
"    -- Called a AudioThumbnail has changed\r\n"
"    --\r\n"
"    -- @comp The Waveform component that holds the changed AudioThumbnail\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrWaveform --]] comp)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"uiWaveFormFilesDroppedCallback\">\r\n"
"    --\r\n"
"    -- Callback to indicate that the user has dropped the files onto this waveform\r\n"
"    --\r\n"
"\r\n"
"    -- @files   - StringArray object that has the file paths\r\n"
"    -- @x       - x coordinate where the event occured\r\n"
"    -- @y       - y coordinate where the event occured\r\n"
"\r\n"
"    __method_name = function(--[[ StringArray --]] files, --[[ number --]] x, --[[ number --]] y)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"\r\n"
"  <luaMethod name=\"uiWaveFormSourceChangedCallback\">\r\n"
"    --\r\n"
"    -- Called when the source for a Waveform component has changed (a new file has been loaded or the reset() method has been called)\r\n"
"    --\r\n"
"    -- @comp The Waveform component that holds the changed AudioThumbnail\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ CtrlrWaveform --]] comp)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelLoaded\">\r\n"
"    --\r\n"
"    -- Called when the panel has finished loading\r\n"
"    --\r\n"
"    -- @type the type of instance beeing started, types available in the CtrlrPanel\r\n"
"    -- class as enum\r\n"
"    --\r\n"
"    -- InstanceSingle\r\n"
"\t-- InstanceMulti\r\n"
"\t-- InstanceSingleRestriced\r\n"
"\t-- InstanceSingleEngine\r\n"
"\t-- InstanceMultiEngine\r\n"
"\t-- InstanceSingleRestrictedEngine\r\n"
"\t--\r\n"
"    __method_name = function(--[[ CtrlrInstance --]] type)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelMidiChannelChanged\">\r\n"
"    --\r\n"
"    -- Called when a MIDI channel property for the panel changes\r\n"
"    --\r\n"
"    __method_name = function(--[[ number --]] channelType, --[[ number --]] newChannel)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaModulatorGetValueForMIDI\">\r\n"
"    --\r\n"
"    --\r\n"
"    --\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ number --]] numericModulatorValue)\r\n"
"      return numericModulatorValue\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaModulatorGetValueFromMIDI\">\r\n"
"    --\r\n"
"    --\r\n"
"    --\r\n"
"    __method_name = function(--[[ CtrlrModulator --]] modulator, --[[ CtrlrMidiMessage --]] midiMessage, --[[ number --]] numericMidiValue)\r\n"
"      return numericMidiValue\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelGlobalChanged\">\r\n"
"    --\r\n"
"    -- Called when a global variable has changed its value\r\n"
"    --\r\n"
"\r\n"
"    __method_name = function(--[[ number --]] index, --[[ number --]] value)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelMessageHandler\">\r\n"
"    --\r\n"
"    -- Called when a messages needs to be displayed to the user.\r\n"
"    --\r\n"
"\r\n"
"    -- @message   - a String object with the message contents\r\n"
"    -- @type      - a type of message (4 levels defined success,failure,information,warning)\r\n"
"\r\n"
"    __method_name = function(--[[ String --]] message, --[[ number --]] type)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelFileDragDropHandler\">\r\n"
"    --\r\n"
"    -- Callback to indicate that the user has dropped the files onto this panel\r\n"
"    --\r\n"
"\r\n"
"    -- @files   - StringArray object that has the file paths\r\n"
"    -- @x       - x coordinate where the event occured\r\n"
"    -- @y       - y coordinate where the event occured\r\n"
"\r\n"
"    __method_name = function(--[[ StringArray --]] files, --[[ number --]] x, --[[ number --]] y)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelFileDragEnterHandler\">\r\n"
"    --\r\n"
"    -- Callback to indicate that some files are being dragged over this panel\r\n"
"    --\r\n"
"\r\n"
"    -- @files   - StringArray object that has the file paths\r\n"
"    -- @x       - x coordinate where the event occured\r\n"
"    -- @y       - y coordinate where the event occured\r\n"
"\r\n"
"    __method_name = function(--[[ StringArray --]] files, --[[ number --]] x, --[[ number --]] y)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelFileDragExitHandler\">\r\n"
"    --\r\n"
"    -- Callback to indicate that the mouse has moved away from this panel\r\n"
"    --\r\n"
"\r\n"
"    -- @files   - StringArray object that has the file paths\r\n"
"\r\n"
"    __method_name = function(--[[ StringArray --]] files)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelResourcesLoaded\">\r\n"
"      --\r\n"
"      -- Callback to indicate that all resources are ready for a panel\r\n"
"      --\r\n"
"      __method_name = function()\r\n"
"      end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaCtrlrSaveState\">\r\n"
"    --\r\n"
"    -- Called when data needs saving\r\n"
"    --\r\n"
"    __method_name = function(--[[ ValueTree --]] stateData)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"  <luaMethod name=\"luaCtrlrResoreState\">\r\n"
"    --\r\n"
"    -- Called when data is restored\r\n"
"    --\r\n"
"    __method_name = function(--[[ ValueTree --]] stateData)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"  <luaMethod name=\"luaPanelSaveState\">\r\n"
"    --\r\n"
"    -- Called when data needs saving\r\n"
"    --\r\n"
"    __method_name = function(--[[ ValueTree --]]stateData)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"  <luaMethod name=\"luaPanelRestoreState\">\r\n"
"    --\r\n"
"    -- Called when data is restored\r\n"
"    --\r\n"
"    __method_name = function(--[[ ValueTree --]] stateData)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelMidiSnapshotPre\">\r\n"
"    --\r\n"
"    -- Called before a snapshot is sent to the device\r\n"
"    --\r\n"
"    -- @buffer MidiBuffer all the midi messages that will be sent\r\n"
"    --\r\n"
"    __method_name = function(--[[ MidiBuffer --]] buffer)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaPanelMidiSnapshotPost\">\r\n"
"    --\r\n"
"    -- Called when data is restored\r\n"
"    --\r\n"
"    -- @buffer MidiBuffer all the midi messages that have been sent\r\n"
"    --\r\n"
"    __method_name = function(--[[ MidiBuffer --]] buffer)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"luaAudioProcessBlock\">\r\n"
"    --\r\n"
"    -- Called on the audio thread, can't access any\r\n"
"    -- panel/modulator/component properties or objects\r\n"
"    -- this is only for MIDI data processing\r\n"
"    --\r\n"
"    -- @midiData midi data coming from the plugin host\r\n"
"    -- @position sample accurate position, midi events in the buffer will\r\n"
"    --           be timestamped according to this data\r\n"
"    --\r\n"
"    __method_name = function(--[[ MidiBuffer --]] midiData, --[[ AudioProcessor::CurrentPositionInfo --]] position)\r\n"
"    end\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <luaMethod name=\"LookAndFeel\" resource=\"LookAndFeel_lua\">\r\n"
"  </luaMethod>\r\n"
"\r\n"
"  <utilityMethods>\r\n"
"  \t<utility name=\"table_dump\" description=\"Dump information about a table\" alwaysUpdate=\"1\" uuid=\"00000000000000000000000000000001\">\r\n"
"-- @1.1\r\n"
"--\r\n"
"-- Print table contents\r\n"
"--\r\n"
"function table_dump(table)\r\n"
"\tfor key,value in ipairs(table) do\r\n"
"\t\t_DBG (\"KEY= [\"..key..\"]\")\r\n"
"\r\n"
"\t\tif (type(value) == \"table\") then\r\n"
"\t\t\ttable_dump(value)\r\n"
"\t\telseif (type(value) == \"nil\") then\r\n"
"\t\t\t_DBG (\" = NIL\")\r\n"
"\t\telse\r\n"
"\t\t\twhat (value)\r\n"
"\t\tend\r\n"
"\tend\r\n"
"end\r\n"
"\t</utility>\r\n"
"\r\n"
"    <utility name=\"what\" description=\"Prints information on any variable passed in as argument, useful if you need to find out what you are dealing with\" alwaysUpdate=\"1\" uuid=\"00000000000000000000000000000002\">\r\n"
"-- @1.1\r\n"
"--\r\n"
"-- Print methods for an object\r\n"
"--\r\n"
"function what(o)\r\n"
"\tinfo = class_info(o)\r\n"
"\tif info ~= nil then\r\n"
"\t\tret = \"Object type [\" .. info.name .. \"]\\n-----------------------------------------------------------------\\n\\n\"..\"Members:\\n\"\r\n"
"\r\n"
"\t\tif info.name == \"table\" then\r\n"
"\t\t\ttable_dump(o)\r\n"
"\t\tend\r\n"
"\r\n"
"\t\tfor k, v in pairs(info.methods) do\r\n"
"\t\t\tret = ret .. string.format (\"\\t%30s:\\t%5s\\n\", k, type(v))\r\n"
"\t\tend\r\n"
"\t\tret = ret .. \"\\n\\nAttributes:\\n\"\r\n"
"\t\tfor k, v in pairs(info.attributes) do\r\n"
"\t\t\tret = ret .. string.format (\"\\t%30s:\\t%5s\\n\", k, type(v))\r\n"
"\t\tend\r\n"
"\t\tret = ret .. \"\\n-----------------------------------------------------------------\"\r\n"
"\tend\r\n"
"\r\n"
"\tconsole (ret)\r\n"
"\treturn ret\r\n"
"end\r\n"
"    </utility>\r\n"
"\r\n"
"    <utility name=\"how\" description=\"Prints all available lua classes bound from Ctrlr\" alwaysUpdate=\"1\" uuid=\"00000000000000000000000000000003\">\r\n"
"-- @1.1\r\n"
"--\r\n"
"-- Print all available classes\r\n"
"--\r\n"
"function how()\r\n"
"\tret = \"Available classes:\\n\"\r\n"
"\tret = ret .. \"\\n-----------------------------------------------------------------\"\r\n"
"\tfor i,v in ipairs(class_names()) do\r\n"
"\t\tret = ret .. \"\\t\".. v .. \"\\n\"\r\n"
"\tend\r\n"
"\tret = ret .. \"\\n-----------------------------------------------------------------\"\r\n"
"\tconsole (J(ret))\r\n"
"\treturn ret\r\n"
"end\r\n"
"  </utility>\r\n"
"\r\n"
"  <utility name=\"json4lua\" description=\"JSON implementation\" alwaysUpdate=\"1\" uuid=\"00000000000000000000000000000004\" resource=\"json_lua\" />\r\n"
"  <utility name=\"debugger.lua\" description=\"clidebugger\" alwaysUpdate=\"1\" uuid=\"00000000000000000000000000000005\" resource=\"debugger_lua\" />\r\n"
"  <utility name=\"inspect\" description=\"Lua table inspector\" alwaysUpdate=\"1\" uuid=\"00000000000000000000000000000006\" resource=\"inspect_lua\" />\r\n"
"  </utilityMethods>\r\n"
"</methods>\r\n";

const char* CtrlrLuaMethodTemplates_xml = (const char*) temp_binary_data_134;

//================== CtrlrMidiMultiTemplate.xml ==================
static const unsigned char temp_binary_data_135[] =
"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n"
"<templates>\r\n"
"  <template name=\"RPN\">\r\n"
"    <message>\r\n"
"      CC,ByteValue,MSB7bitValue,101,-2:CC,ByteValue,LSB7bitValue,100,-2:CC,ByteValue,MSB7bitValue,6,-1:CC,ByteValue,LSB7bitValue,38,-1\r\n"
"    </message>\r\n"
"  </template>\r\n"
"\r\n"
"  <template name=\"NRPN\">\r\n"
"    <message>\r\n"
"      CC,ByteValue,MSB7bitValue,99,-2:CC,ByteValue,LSB7bitValue,98,-2:CC,ByteValue,MSB7bitValue,6,-1:CC,ByteValue,LSB7bitValue,38,-1\r\n"
"    </message>\r\n"
"  </template>\r\n"
"\r\n"
"  <template name=\"NRPN Korg\">\r\n"
"    <message>\r\n"
"      CC,ByteValue,MSB7bitValue,99,-2:CC,ByteValue,LSB7bitValue,98,-2:CC,ByteValue,LSB7bitValue,6,-1\r\n"
"    </message>\r\n"
"  </template>\r\n"
"</templates>";

const char* CtrlrMidiMultiTemplate_xml = (const char*) temp_binary_data_135;

//================== CtrlrMIDITransactions.xml ==================
static const unsigned char temp_binary_data_136[] =
"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n"
"<transactions>\r\n"
"\t<trans \t\tname=\"Request Identity\"\r\n"
"\t\t\t\tenabled=\"1\"\r\n"
"\t\t\t\tinternal=\"1\"\r\n"
"\t\t\t\ttransCap=\"midiLibrary\"\r\n"
"\t\t\t\ttimeout=\"1000\"\r\n"
"\t\t\t\ttransReqFormulaPrefix=\"F0 7E 7F 06 01\"\r\n"
"\t\t\t\ttransReqFormulaSuffix=\"F7\"\r\n"
"        transRespSizeCheck=\"0\"\r\n"
"\t\t\t\ttransReqDataLen=\"0\"\r\n"
"\t\t\t\ttransReqExists=\"1\"\r\n"
"\t\t\t\ttransConfExists=\"0\"\r\n"
"\t\t\t\ttransRespFormulaPrefix=\"F0 7E 7F 06 02\"\r\n"
"\t\t\t\ttransRespFormulaSuffix=\"F7\"\r\n"
"\t\t\t\ttransRespDataLen=\"8\"\r\n"
"\t\t\t\ttransRespDataCheck=\"1\"\r\n"
"\t\t\t\ttransRespExists=\"1\"\r\n"
"\t/>\r\n"
"\r\n"
"\t<trans \t\tname=\"Request Identity [LUA]\"\r\n"
"\t\t\t\tenabled=\"1\"\r\n"
"\t\t\t\tinternal=\"1\"\r\n"
"\t\t\t\ttransCap=\"midiLibrary\"\r\n"
"\t\t\t\ttimeout=\"1000\"\r\n"
"\t\t\t\ttransReqFormulaPrefix=\"\"\r\n"
"\t\t\t\ttransReqFormulaSuffix=\"\"\r\n"
"\t\t\t\ttransReqDataLen=\"0\"\r\n"
"\t\t\t\ttransReqExists=\"1\"\r\n"
"\t\t\t\ttransConfExists=\"0\"\r\n"
"\t\t\t\ttransRespFormulaPrefix=\"\"\r\n"
"\t\t\t\ttransRespFormulaSuffix=\"\"\r\n"
"\t\t\t\ttransRespDataLen=\"8\"\r\n"
"\t\t\t\ttransRespDataCheck=\"0\"\r\n"
"        transRespSizeCheck=\"0\"\r\n"
"\t\t\t\ttransRespExists=\"1\"\r\n"
"\r\n"
"\t\t\t\tluaTransRequest=\"transIdReq\"\r\n"
"\t\t\t\tluaTransProcess=\"transIdProc\"\r\n"
"\t/>\r\n"
"\r\n"
"\t<trans \t\tname=\"Send SYX file\"\r\n"
"\t\t\t\tenabled=\"1\"\r\n"
"\t\t\t\tinternal=\"1\"\r\n"
"\t\t\t\ttransCap=\"midiLibrary\"\r\n"
"\t\t\t\ttimeout=\"1000\"\r\n"
"\t\t\t\ttransReqFormulaPrefix=\"\"\r\n"
"\t\t\t\ttransReqFormulaSuffix=\"\"\r\n"
"\t\t\t\ttransReqDataLen=\"0\"\r\n"
"\t\t\t\ttransReqExists=\"1\"\r\n"
"\t\t\t\ttransConfExists=\"0\"\r\n"
"\t\t\t\ttransRespFormulaPrefix=\"\"\r\n"
"\t\t\t\ttransRespFormulaSuffix=\"\"\r\n"
"\t\t\t\ttransRespDataLen=\"0\"\r\n"
"\t\t\t\ttransRespDataCheck=\"0\"\r\n"
"\t\t\t\ttransRespExists=\"0\"\r\n"
"\r\n"
"\t\t\t\tluaTransRequest=\"transSYXSendReq\"\r\n"
"\t\t/>\r\n"
"\r\n"
"\t<trans\t\tname=\"Recieve SYX file\"\r\n"
"\t\t\t\tenabled=\"1\"\r\n"
"\t\t\t\tinternal=\"1\"\r\n"
"\t\t\t\ttransCap=\"midiLibrary\"\r\n"
"\t\t\t\ttimeout=\"1000\"\r\n"
"\t\t\t\ttransReqFormulaPrefix=\"\"\r\n"
"\t\t\t\ttransReqFormulaSuffix=\"\"\r\n"
"\t\t\t\ttransReqDataLen=\"0\"\r\n"
"\t\t\t\ttransReqExists=\"1\"\r\n"
"\t\t\t\ttransConfExists=\"0\"\r\n"
"\t\t\t\ttransRespFormulaPrefix=\"\"\r\n"
"\t\t\t\ttransRespFormulaSuffix=\"\"\r\n"
"\t\t\t\ttransRespDataLen=\"0\"\r\n"
"\t\t\t\ttransRespDataCheck=\"0\"\r\n"
"\t\t\t\ttransRespExists=\"1\"\r\n"
"\r\n"
"\t\t\t\tluaTransRequest=\"transSYXRcvReq\"\r\n"
"\t\t\t\tluaTransProcess=\"transSYXRcvProc\"\r\n"
"\t\t/>\r\n"
"</transactions>\r\n";

const char* CtrlrMIDITransactions_xml = (const char*) temp_binary_data_136;

//================== CtrlrMIDIVendors.xml ==================
static const unsigned char temp_binary_data_137[] =
"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n"
"<vendors>\r\n"
"\t\t<vendor name=\"Ta Horng Musical Instrument\" id=\"000074\" />\r\n"
"\t\t<vendor name=\"e-Tek Labs (Forte Tech)\" id=\"000075\" />\r\n"
"\t\t<vendor name=\"Electro-Voice\" id=\"000076\" />\r\n"
"\t\t<vendor name=\"Midisoft Corporation\" id=\"000077\" />\r\n"
"\t\t<vendor name=\"QSound Labs\" id=\"000078\" />\r\n"
"\t\t<vendor name=\"Westrex\" id=\"000079\" />\r\n"
"\t\t<vendor name=\"Nvidia\" id=\"00007A\" />\r\n"
"\t\t<vendor name=\"ESS Technology\" id=\"00007B\" />\r\n"
"\t\t<vendor name=\"Media Trix Peripherals\" id=\"00007C\" />\r\n"
"\t\t<vendor name=\"Brooktree Corp\" id=\"00007D\" />\r\n"
"\t\t<vendor name=\"Otari Corp\" id=\"00007E\" />\r\n"
"\t\t<vendor name=\"Key Electronics\" id=\"00007F\" />\r\n"
"\t\t<vendor name=\"Shure incorporated\" id=\"000100\" />\r\n"
"\t\t<vendor name=\"AuraSound\" id=\"000101\" />\r\n"
"\t\t<vendor name=\"Crystal Semiconductor\" id=\"000102\" />\r\n"
"\t\t<vendor name=\"Conexant (Rockwell)\" id=\"000103\" />\r\n"
"\t\t<vendor name=\"Silicon Graphics\" id=\"000104\" />\r\n"
"\t\t<vendor name=\"M-Audio (Midiman)\" id=\"000105\" />\r\n"
"\t\t<vendor name=\"PreSonus\" id=\"000106\" />\r\n"
"\t\t<vendor name=\"Topaz Enterprises\" id=\"000108\" />\r\n"
"\t\t<vendor name=\"Cast Lighting\" id=\"000109\" />\r\n"
"\t\t<vendor name=\"Microsoft\" id=\"00010A\" />\r\n"
"\t\t<vendor name=\"Sonic Foundry\" id=\"00010B\" />\r\n"
"\t\t<vendor name=\"Line 6 (Fast Forward)\" id=\"00010C\" />\r\n"
"\t\t<vendor name=\"Beatnik Inc\" id=\"00010D\" />\r\n"
"\t\t<vendor name=\"Van Koevering Company\" id=\"00010E\" />\r\n"
"\t\t<vendor name=\"Altech Systems\" id=\"00010F\" />\r\n"
"\t\t<vendor name=\"S &amp; S Research\" id=\"000110\" />\r\n"
"\t\t<vendor name=\"VLSI Technology\" id=\"000111\" />\r\n"
"\t\t<vendor name=\"Chromatic Research\" id=\"000112\" />\r\n"
"\t\t<vendor name=\"Sapphire\" id=\"000113\" />\r\n"
"\t\t<vendor name=\"IDRC\" id=\"000114\" />\r\n"
"\t\t<vendor name=\"Justonic Tuning\" id=\"000115\" />\r\n"
"\t\t<vendor name=\"TorComp Research Inc.\" id=\"000116\" />\r\n"
"\t\t<vendor name=\"Newtek Inc.\" id=\"000117\" />\r\n"
"\t\t<vendor name=\"Sound Sculpture\" id=\"000118\" />\r\n"
"\t\t<vendor name=\"Walker Technical\" id=\"000119\" />\r\n"
"\t\t<vendor name=\"Digital Harmony (PAVO)\" id=\"00011A\" />\r\n"
"\t\t<vendor name=\"InVision Interactive\" id=\"00011B\" />\r\n"
"\t\t<vendor name=\"T-Square Design\" id=\"00011C\" />\r\n"
"\t\t<vendor name=\"Nemesys Music Technology\" id=\"00011D\" />\r\n"
"\t\t<vendor name=\"DBX Professional (Harman Intl)\" id=\"00011E\" />\r\n"
"\t\t<vendor name=\"Syndyne Corporation\" id=\"00011F\" />\r\n"
"\t\t<vendor name=\"Bitheadz\" id=\"000120\" />\r\n"
"\t\t<vendor name=\"Cakewalk Music Software\" id=\"000121\" />\r\n"
"\t\t<vendor name=\"Analog Devices\" id=\"000122\" />\r\n"
"\t\t<vendor name=\"National Semiconductor\" id=\"000123\" />\r\n"
"\t\t<vendor name=\"Boom Theory / Adinolfi Alternative Percussion\" id=\"000124\" />\r\n"
"\t\t<vendor name=\"Virtual DSP Corporation\" id=\"000125\" />\r\n"
"\t\t<vendor name=\"Antares Systems\" id=\"000126\" />\r\n"
"\t\t<vendor name=\"Angel Software\" id=\"000127\" />\r\n"
"\t\t<vendor name=\"St Louis Music\" id=\"000128\" />\r\n"
"\t\t<vendor name=\"Lyrrus dba G-VOX\" id=\"000129\" />\r\n"
"\t\t<vendor name=\"Ashley Audio Inc.\" id=\"00012A\" />\r\n"
"\t\t<vendor name=\"Vari-Lite Inc.\" id=\"00012B\" />\r\n"
"\t\t<vendor name=\"Summit Audio Inc.\" id=\"00012C\" />\r\n"
"\t\t<vendor name=\"Aureal Semiconductor Inc.\" id=\"00012D\" />\r\n"
"\t\t<vendor name=\"SeaSound LLC\" id=\"00012E\" />\r\n"
"\t\t<vendor name=\"U.S. Robotics\" id=\"00012F\" />\r\n"
"\t\t<vendor name=\"Aurisis Research\" id=\"000130\" />\r\n"
"\t\t<vendor name=\"Nearfield Research\" id=\"000131\" />\r\n"
"\t\t<vendor name=\"FM7 Inc\" id=\"000132\" />\r\n"
"\t\t<vendor name=\"Swivel Systems\" id=\"000133\" />\r\n"
"\t\t<vendor name=\"Hyperactive Audio Systems\" id=\"000134\" />\r\n"
"\t\t<vendor name=\"MidiLite (Castle Studios Productions)\" id=\"000135\" />\r\n"
"\t\t<vendor name=\"Radikal Technologies\" id=\"000136\" />\r\n"
"\t\t<vendor name=\"Roger Linn Design\" id=\"000137\" />\r\n"
"\t\t<vendor name=\"TC-Helicon Vocal Technologies\" id=\"000138\" />\r\n"
"\t\t<vendor name=\"Event Electronics\" id=\"000139\" />\r\n"
"\t\t<vendor name=\"Sonic Network Inc\" id=\"00013A\" />\r\n"
"\t\t<vendor name=\"Realtime Music Solutions\" id=\"00013B\" />\r\n"
"\t\t<vendor name=\"Apogee Digital\" id=\"00013C\" />\r\n"
"\t\t<vendor name=\"Classical Organs\" id=\"00013D\" />\r\n"
"\t\t<vendor name=\"Microtools Inc.\" id=\"00013E\" />\r\n"
"\t\t<vendor name=\"Numark Industries\" id=\"00013F\" />\r\n"
"\t\t<vendor name=\"Frontier Design Group\" id=\"000140\" />\r\n"
"\t\t<vendor name=\"Recordare LLC\" id=\"000141\" />\r\n"
"\t\t<vendor name=\"Starr Labs\" id=\"000142\" />\r\n"
"\t\t<vendor name=\"Voyager Sound Inc.\" id=\"000143\" />\r\n"
"\t\t<vendor name=\"Manifold Labs\" id=\"000144\" />\r\n"
"\t\t<vendor name=\"Aviom Inc.\" id=\"000145\" />\r\n"
"\t\t<vendor name=\"Mixmeister Technology\" id=\"000146\" />\r\n"
"\t\t<vendor name=\"Notation Software\" id=\"000147\" />\r\n"
"\t\t<vendor name=\"Mercurial Communications\" id=\"000148\" />\r\n"
"\t\t<vendor name=\"Wave Arts\" id=\"000149\" />\r\n"
"\t\t<vendor name=\"Logic Sequencing Devices\" id=\"00014A\" />\r\n"
"\t\t<vendor name=\"Axess Electronics\" id=\"00014B\" />\r\n"
"\t\t<vendor name=\"Muse Research\" id=\"00014C\" />\r\n"
"\t\t<vendor name=\"Open Labs\" id=\"00014D\" />\r\n"
"\t\t<vendor name=\"Guillemot R&amp;D Inc\" id=\"00014E\" />\r\n"
"\t\t<vendor name=\"Samson Technologies\" id=\"00014F\" />\r\n"
"\t\t<vendor name=\"Electronic Theatre Controls\" id=\"000150\" />\r\n"
"\t\t<vendor name=\"Blackberry (RIM)\" id=\"000151\" />\r\n"
"\t\t<vendor name=\"Mobileer\" id=\"000152\" />\r\n"
"\t\t<vendor name=\"Synthogy\" id=\"000153\" />\r\n"
"\t\t<vendor name=\"Lynx Studio Technology Inc.\" id=\"000154\" />\r\n"
"\t\t<vendor name=\"Damage Control Engineering LLC\" id=\"000155\" />\r\n"
"\t\t<vendor name=\"Yost Engineering\" id=\"000156\" />\r\n"
"\t\t<vendor name=\"Brooks &amp; Forsman Designs LLC / DrumLite\" id=\"000157\" />\r\n"
"\t\t<vendor name=\"Infinite Response\" id=\"000158\" />\r\n"
"\t\t<vendor name=\"Garritan Corp\" id=\"000159\" />\r\n"
"\t\t<vendor name=\"Plogue Art et Technologie\" id=\"00015A\" />\r\n"
"\t\t<vendor name=\"RJM Music Technology\" id=\"00015B\" />\r\n"
"\t\t<vendor name=\" Custom Solutions Software\" id=\"00015C\" />\r\n"
"\t\t<vendor name=\"Sonarcana LLC\" id=\"00015D\" />\r\n"
"\t\t<vendor name=\"Centrance\" id=\"00015E\" />\r\n"
"\t\t<vendor name=\"Kesumo LLC\" id=\"00015F\" />\r\n"
"\t\t<vendor name=\"Stanton (Gibson)\" id=\"000160\" />\r\n"
"\t\t<vendor name=\"Livid Instruments\" id=\"000161\" />\r\n"
"\t\t<vendor name=\"First Act / 745 Media\" id=\"000162\" />\r\n"
"\t\t<vendor name=\"Pygraphics\" id=\"000163\" />\r\n"
"\t\t<vendor name=\"Panadigm Innovations Ltd\" id=\"000164\" />\r\n"
"\t\t<vendor name=\"Avedis Zildjian Co\" id=\"000165\" />\r\n"
"\t\t<vendor name=\"Auvital Music Corp\" id=\"000166\" />\r\n"
"\t\t<vendor name=\"Inspired Instruments Inc\" id=\"000167\" />\r\n"
"\t\t<vendor name=\"Chris Grigg Designs\" id=\"000168\" />\r\n"
"\t\t<vendor name=\"Slate Digital LLC\" id=\"000169\" />\r\n"
"\t\t<vendor name=\"Mixware\" id=\"00016A\" />\r\n"
"\t\t<vendor name=\"Social Entropy\" id=\"00016B\" />\r\n"
"\t\t<vendor name=\"Source Audio LLC\" id=\"00016C\" />\r\n"
"\t\t<vendor name=\"Ernie Ball / Music Man\" id=\"00016D\" />\r\n"
"\t\t<vendor name=\"Fishman Transducers\" id=\"00016E\" />\r\n"
"\t\t<vendor name=\"Custom Audio Electronics\" id=\"00016F\" />\r\n"
"\t\t<vendor name=\"American Audio/DJ\" id=\"000170\" />\r\n"
"\t\t<vendor name=\"Mega Control Systems\" id=\"000171\" />\r\n"
"\t\t<vendor name=\"Kilpatrick Audio\" id=\"000172\" />\r\n"
"\t\t<vendor name=\"iConnectivity\" id=\"000173\" />\r\n"
"\t\t<vendor name=\"Fractal Audio\" id=\"000174\" />\r\n"
"\t\t<vendor name=\"NetLogic Microsystems\" id=\"000175\" />\r\n"
"\t\t<vendor name=\"Music Computing\" id=\"000176\" />\r\n"
"\t\t<vendor name=\"Nektar Technology Inc\" id=\"000177\" />\r\n"
"\t\t<vendor name=\"Zenph Sound Innovations\" id=\"000178\" />\r\n"
"\t\t<vendor name=\"DJTechTools.com\" id=\"000179\" />\r\n"
"\t\t<vendor name=\"Rezonance Labs\" id=\"00017A\" />\r\n"
"\t\t<vendor name=\"Decibel Eleven\" id=\"00017B\" />\r\n"
"\t\t<vendor name=\"Focusrite/Novation\" id=\"002029\" />\r\n"
"\t\t<vendor name=\"Samkyung Mechatronics\" id=\"00202A\" />\r\n"
"\t\t<vendor name=\"Medeli Electronics Co.\" id=\"00202B\" />\r\n"
"\t\t<vendor name=\"Charlie Lab SRL\" id=\"00202C\" />\r\n"
"\t\t<vendor name=\"Blue Chip Music Technology\" id=\"00202D\" />\r\n"
"\t\t<vendor name=\"BEE OH Corp\" id=\"00202E\" />\r\n"
"\t\t<vendor name=\"LG Semicon America\" id=\"00202F\" />\r\n"
"\t\t<vendor name=\"TESI\" id=\"002030\" />\r\n"
"\t\t<vendor name=\"EMAGIC\" id=\"002031\" />\r\n"
"\t\t<vendor name=\"Behringer GmbH\" id=\"002032\" />\r\n"
"\t\t<vendor name=\"Access Music Electronics\" id=\"002033\" />\r\n"
"\t\t<vendor name=\"Synoptic\" id=\"002034\" />\r\n"
"\t\t<vendor name=\"Hanmesoft\" id=\"002035\" />\r\n"
"\t\t<vendor name=\"Terratec Electronic GmbH\" id=\"002036\" />\r\n"
"\t\t<vendor name=\"Proel SpA\" id=\"002037\" />\r\n"
"\t\t<vendor name=\"IBK MIDI\" id=\"002038\" />\r\n"
"\t\t<vendor name=\"IRCAM\" id=\"002039\" />\r\n"
"\t\t<vendor name=\"Propellerhead Software\" id=\"00203A\" />\r\n"
"\t\t<vendor name=\"Red Sound Systems Ltd\" id=\"00203B\" />\r\n"
"\t\t<vendor name=\"Elektron ESI AB\" id=\"00203C\" />\r\n"
"\t\t<vendor name=\"Sintefex Audio\" id=\"00203D\" />\r\n"
"\t\t<vendor name=\"MAM (Music and More)\" id=\"00203E\" />\r\n"
"\t\t<vendor name=\"Amsaro GmbH\" id=\"00203F\" />\r\n"
"\t\t<vendor name=\"CDS Advanced Technology BV\" id=\"002040\" />\r\n"
"\t\t<vendor name=\"Touched By Sound GmbH\" id=\"002041\" />\r\n"
"\t\t<vendor name=\"DSP Arts\" id=\"002042\" />\r\n"
"\t\t<vendor name=\"Phil Rees Music Tech\" id=\"002043\" />\r\n"
"\t\t<vendor name=\"Stamer Musikanlagen GmbH\" id=\"002044\" />\r\n"
"\t\t<vendor name=\"Musical Muntaner S.A. dba Soundart\" id=\"002045\" />\r\n"
"\t\t<vendor name=\"C-Mexx Software\" id=\"002046\" />\r\n"
"\t\t<vendor name=\"Klavis Technologies\" id=\"002047\" />\r\n"
"\t\t<vendor name=\"Noteheads AB\" id=\"002048\" />\r\n"
"\t\t<vendor name=\"Algorithmix\" id=\"002049\" />\r\n"
"\t\t<vendor name=\"Skrydstrup R&amp;D\" id=\"00204A\" />\r\n"
"\t\t<vendor name=\"Professional Audio Company\" id=\"00204B\" />\r\n"
"\t\t<vendor name=\"NewWave Labs (MadWaves)\" id=\"00204C\" />\r\n"
"\t\t<vendor name=\"Vermona\" id=\"00204D\" />\r\n"
"\t\t<vendor name=\"Nokia\" id=\"00204E\" />\r\n"
"\t\t<vendor name=\"Wave Idea\" id=\"00204F\" />\r\n"
"\t\t<vendor name=\"Hartmann GmbH\" id=\"002050\" />\r\n"
"\t\t<vendor name=\"Lion's Tracs\" id=\"002051\" />\r\n"
"\t\t<vendor name=\"Analogue Systems\" id=\"002052\" />\r\n"
"\t\t<vendor name=\"Focal-JMlab\" id=\"002053\" />\r\n"
"\t\t<vendor name=\"Ringway Electronics (Chang-Zhou) Co Ltd\" id=\"002054\" />\r\n"
"\t\t<vendor name=\"Faith Technologies (Digiplug)\" id=\"002055\" />\r\n"
"\t\t<vendor name=\"Showworks\" id=\"002056\" />\r\n"
"\t\t<vendor name=\"Manikin Electronic\" id=\"002057\" />\r\n"
"\t\t<vendor name=\"1 Come Tech\" id=\"002058\" />\r\n"
"\t\t<vendor name=\"Phonic Corp\" id=\"002059\" />\r\n"
"\t\t<vendor name=\"Dolby Australia (Lake)\" id=\"00205A\" />\r\n"
"\t\t<vendor name=\"Silansys Technologies\" id=\"00205B\" />\r\n"
"\t\t<vendor name=\"Winbond Electronics\" id=\"00205C\" />\r\n"
"\t\t<vendor name=\"Cinetix Medien und Interface GmbH\" id=\"00205D\" />\r\n"
"\t\t<vendor name=\"A&amp;G Soluzioni Digitali\" id=\"00205E\" />\r\n"
"\t\t<vendor name=\"Sequentix Music Systems\" id=\"00205F\" />\r\n"
"\t\t<vendor name=\"Oram Pro Audio\" id=\"002060\" />\r\n"
"\t\t<vendor name=\"Be4 Ltd\" id=\"002061\" />\r\n"
"\t\t<vendor name=\"Infection Music\" id=\"002062\" />\r\n"
"\t\t<vendor name=\"Central Music Co. (CME)\" id=\"002063\" />\r\n"
"\t\t<vendor name=\"genoQs Machines GmbH\" id=\"002064\" />\r\n"
"\t\t<vendor name=\"Medialon\" id=\"002065\" />\r\n"
"\t\t<vendor name=\"Waves Audio Ltd\" id=\"002066\" />\r\n"
"\t\t<vendor name=\"Jerash Labs\" id=\"002067\" />\r\n"
"\t\t<vendor name=\"Da Fact\" id=\"002068\" />\r\n"
"\t\t<vendor name=\"Elby Designs\" id=\"002069\" />\r\n"
"\t\t<vendor name=\"Spectral Audio\" id=\"00206A\" />\r\n"
"\t\t<vendor name=\"Arturia\" id=\"00206B\" />\r\n"
"\t\t<vendor name=\"Vixid\" id=\"00206C\" />\r\n"
"\t\t<vendor name=\"C-Thru Music\" id=\"00206D\" />\r\n"
"\t\t<vendor name=\"Ya Horng Electronic Co LTD\" id=\"00206E\" />\r\n"
"\t\t<vendor name=\"SM Pro Audio\" id=\"00206F\" />\r\n"
"\t\t<vendor name=\"OTO MACHINES\" id=\"002070\" />\r\n"
"\t\t<vendor name=\"ELZAB S.A.\" id=\"002071\" />\r\n"
"\t\t<vendor name=\"Blackstar Amplification Ltd\" id=\"002072\" />\r\n"
"\t\t<vendor name=\"M3i Technologies GmbH\" id=\"002073\" />\r\n"
"\t\t<vendor name=\"Gemalto (from Xiring)\" id=\"002074\" />\r\n"
"\t\t<vendor name=\"Prostage SL\" id=\"002075\" />\r\n"
"\t\t<vendor name=\"Teenage Engineering\" id=\"002076\" />\r\n"
"\t\t<vendor name=\"Tobias Erichsen Consulting\" id=\"002077\" />\r\n"
"\t\t<vendor name=\"Nixer Ltd\" id=\"002078\" />\r\n"
"\t\t<vendor name=\"Hanpin Electron Co Ltd\" id=\"002079\" />\r\n"
"\t\t<vendor name=\"MIDI-hardware R.Sowa\" id=\"00207A\" />\r\n"
"\t\t<vendor name=\"Beyond Music Industrial Ltd\" id=\"00207B\" />\r\n"
"\t\t<vendor name=\"Kiss Box B.V.\" id=\"00207C\" />\r\n"
"\t\t<vendor name=\"Misa Digital Technologies Ltd\" id=\"00207D\" />\r\n"
"\t\t<vendor name=\"AI Musics Technology Inc\" id=\"00207E\" />\r\n"
"\t\t<vendor name=\"Serato Inc LP\" id=\"00207F\" />\r\n"
"\t\t<vendor name=\"Limex Music Handles GmbH\" id=\"002100\" />\r\n"
"\t\t<vendor name=\"Kyodday/Tokai\" id=\"002101\" />\r\n"
"\t\t<vendor name=\"Mutable Instruments\" id=\"002102\" />\r\n"
"\t\t<vendor name=\"PreSonus Software Ltd\" id=\"002103\" />\r\n"
"\t\t<vendor name=\"Xiring\" id=\"002104\" />\r\n"
"\t\t<vendor name=\"Fairlight Instruments Pty Ltd\" id=\"002105\" />\r\n"
"\t\t<vendor name=\"Musicom Lab\" id=\"002106\" />\r\n"
"\t\t<vendor name=\"VacoLoco\" id=\"002107\" />\r\n"
"\t\t<vendor name=\"RWA (Hong Kong) Limited\" id=\"002108\" />\r\n"
"\t\t<vendor name=\"Native Instruments\" id=\"002109\" />\r\n"
"\t\t<vendor name=\"Naonext\" id=\"00210A\" />\r\n"
"\t\t<vendor name=\"MFB\" id=\"00210B\" />\r\n"
"\t\t<vendor name=\"Teknel Research\" id=\"00210C\" />\r\n"
"\t\t<vendor name=\"Ploytec GmbH\" id=\"00210D\" />\r\n"
"\t\t<vendor name=\"Surfin Kangaroo Studio\" id=\"00210E\" />\r\n"
"\t\t<vendor name=\"Crimson Technology Inc.\" id=\"004000\" />\r\n"
"\t\t<vendor name=\"Softbank Mobile Corp\" id=\"004001\" />\r\n"
"\t\t<vendor name=\"D&amp;M Holdings Inc.\" id=\"004003\" />\r\n"
"\t\t<vendor name=\"Sequential Circuits\"\tid=\"01\" />\r\n"
"\t\t<vendor name=\"Big Briar\"\tid=\"02\" />\r\n"
"\t\t<vendor name=\"Octave / Plateau\"\tid=\"03\" />\r\n"
"\t\t<vendor name=\"Moog\"\tid=\"04\" />\r\n"
"\t\t<vendor name=\"Passport Designs\"\tid=\"05\" />\r\n"
"\t\t<vendor name=\"Lexicon\"\tid=\"06\" />\r\n"
"\t\t<vendor name=\"Kurzweil\"\tid=\"07\" />\r\n"
"\t\t<vendor name=\"Fender\"\tid=\"08\" />\r\n"
"\t\t<vendor name=\"Gulbransen\"\tid=\"09\" />\r\n"
"\t\t<vendor name=\"Delta Labs\"\tid=\"0A\" />\r\n"
"\t\t<vendor name=\"Sound Comp.\"\tid=\"0B\" />\r\n"
"\t\t<vendor name=\"General Electro\"\tid=\"0C\" />\r\n"
"\t\t<vendor name=\"Techmar\"\tid=\"0D\" />\r\n"
"\t\t<vendor name=\"Matthews Research\"\tid=\"0E\" />\r\n"
"\t\t<vendor name=\"Oberheim\"\tid=\"10\" />\r\n"
"\t\t<vendor name=\"PAIA\"\tid=\"11\" />\r\n"
"\t\t<vendor name=\"Simmons\"\tid=\"12\" />\r\n"
"\t\t<vendor name=\"DigiDesign\"\tid=\"13\" />\r\n"
"\t\t<vendor name=\"Fairlight\"\tid=\"14\" />\r\n"
"\t\t<vendor name=\"Peavey\"\tid=\"1B\" />\r\n"
"\t\t<vendor name=\"JL Cooper\"\tid=\"15\" />\r\n"
"\t\t<vendor name=\"Lowery\"\tid=\"16\" />\r\n"
"\t\t<vendor name=\"Lin\"\tid=\"17\" />\r\n"
"\t\t<vendor name=\"Emu\"\tid=\"18\" />\r\n"
"\t\t<vendor name=\"Bon Tempi\"\tid=\"20\" />\r\n"
"\t\t<vendor name=\"S.I.E.L.\"\tid=\"21\" />\r\n"
"\t\t<vendor name=\"SyntheAxe\"\tid=\"23\" />\r\n"
"\t\t<vendor name=\"Hohner\"\tid=\"24\" />\r\n"
"\t\t<vendor name=\"Crumar\"\tid=\"25\" />\r\n"
"\t\t<vendor name=\"Solton\"\tid=\"26\" />\r\n"
"\t\t<vendor name=\"Jellinghaus Ms\"\tid=\"27\" />\r\n"
"\t\t<vendor name=\"CTS\"\tid=\"28\" />\r\n"
"\t\t<vendor name=\"PPG\"\tid=\"29\" />\r\n"
"\t\t<vendor name=\"Elka\"\tid=\"2F\" />\r\n"
"\t\t<vendor name=\"Cheetah\"\tid=\"36\" />\r\n"
"\t\t<vendor name=\"Waldorf\"\tid=\"3E\" />\r\n"
"\t\t<vendor name=\"Kawai\"\tid=\"40\" />\r\n"
"\t\t<vendor name=\"Roland\"\tid=\"41\" />\r\n"
"\t\t<vendor name=\"Korg\"\tid=\"42\" />\r\n"
"\t\t<vendor name=\"Yamaha\"\tid=\"43\" />\r\n"
"\t\t<vendor name=\"Casio\"\tid=\"44\" />\r\n"
"\t\t<vendor name=\"Akai\"\tid=\"45\" />\r\n"
"\t\t<vendor name=\"Kawai Musical Instruments MFG. CO. Ltd\"\tid=\"40\" />\r\n"
"\t\t<vendor name=\"Roland Corporation\"\tid=\"41\" />\r\n"
"\t\t<vendor name=\"Korg Inc.\"\tid=\"42\" />\r\n"
"\t\t<vendor name=\"Yamaha Corporation\"\tid=\"43\" />\r\n"
"\t\t<vendor name=\"Casio Computer Co. Ltd\"\tid=\"44\" />\r\n"
"\t\t<vendor name=\"Kamiya Studio Co. Ltd\"\tid=\"46\" />\r\n"
"\t\t<vendor name=\"Akai Electric Co. Ltd.\"\tid=\"47\" />\r\n"
"\t\t<vendor name=\"Victor Company of Japan, Ltd.\"\tid=\"48\" />\r\n"
"\t\t<vendor name=\"Fujitsu Limited\"\tid=\"4B\" />\r\n"
"\t\t<vendor name=\"Sony Corporation\"\tid=\"4C\" />\r\n"
"\t\t<vendor name=\"Teac Corporation\"\tid=\"4E\" />\r\n"
"\t\t<vendor name=\"Matsushita Electric Industrial Co. , Ltd\"\tid=\"50\" />\r\n"
"\t\t<vendor name=\"Fostex Corporation\"\tid=\"51\" />\r\n"
"\t\t<vendor name=\"Zoom Corporation\"\tid=\"52\" />\r\n"
"\t\t<vendor name=\"Matsushita Communication Industrial Co., Ltd.\"\tid=\"54\" />\r\n"
"\t\t<vendor name=\"Suzuki Musical Instruments MFG. Co., Ltd.\"\tid=\"55\" />\r\n"
"\t\t<vendor name=\"Fuji Sound Corporation Ltd.\"\tid=\"56\" />\r\n"
"\t\t<vendor name=\"Acoustic Technical Laboratory, Inc.\"\tid=\"57\" />\r\n"
"\t\t<vendor name=\"Faith, Inc.\"\tid=\"59\" />\r\n"
"\t\t<vendor name=\"Internet Corporation\"\tid=\"5A\" />\r\n"
"\t\t<vendor name=\"Seekers Co. Ltd.\"\tid=\"5C\" />\r\n"
"\t\t<vendor name=\"SD Card Association\"\tid=\"5F\" />\r\n"
"</vendors>\r\n";

const char* CtrlrMIDIVendors_xml = (const char*) temp_binary_data_137;
}
