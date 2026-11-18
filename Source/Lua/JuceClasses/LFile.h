#ifndef __L_FILE__
#define __L_FILE__

#include "LMemoryBlock.h"

class LFile
{
	public:
		static void findChildFiles (const File& file, luabind::object const& table, int whatToLookFor, bool searchRecursively, const String wildcardPattern);
		static double getSize (const File& file);

		static File getSpecialLocation(const File::SpecialLocationType type)
		{
			return (File::getSpecialLocation(type));
		}

		static bool replaceWithData (File& file, const LMemoryBlock &dataToWrite);

        static bool appendData (File& file, const LMemoryBlock &dataToAppend);

		static const String descriptionOfSizeInBytes(const double fileSize)
		{
			return (File::descriptionOfSizeInBytes ((juce::int64)fileSize));
		}

		static bool isValid(const File& file);
		static void wrapForLua (lua_State *L);
};

#endif
