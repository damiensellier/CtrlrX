// LRSAKey.h
#ifndef L_RSA_KEY
#define L_RSA_KEY

extern "C"
{
    #include "lua.h"
}

#include "JuceHeader.h"
#include <utility> // For std::pair

class LRSAKey // This class will *only* contain static helper methods that don't belong to juce::RSAKey
{
public:
    // Static method to create key pairs. Returns a pair of RSAKey objects.
    static std::pair<juce::RSAKey, juce::RSAKey> createKeyPairWrapper(int numBits);

    // Static dummy function for testing
    static juce::String getDummyString();

    static void wrapForLua (lua_State *L);
};

#endif // L_RSA_KEY
