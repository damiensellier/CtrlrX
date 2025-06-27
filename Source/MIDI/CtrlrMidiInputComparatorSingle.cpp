#include "stdafx.h"
#include "CtrlrMidiInputComparatorSingle.h"
#include "CtrlrLog.h"
#include "CtrlrPanel/CtrlrPanel.h"
#include "JuceClasses/LMemoryBlock.h"

CtrlrMidiInputComparatorSingle::CtrlrMidiInputComparatorSingle(CtrlrPanel &_owner, const CtrlrMIDIDeviceType _source)
	: owner(_owner), cacheSize(32), source(_source)
{
	clear();
}

CtrlrMidiInputComparatorSingle::~CtrlrMidiInputComparatorSingle()
{
	clear();
}

void CtrlrMidiInputComparatorSingle::clear()
{
	mapCC.clear();
	mapAftertouch.clear();
	mapPitchWheel.clear();
	mapProgramChange.clear();
	mapNoteOn.clear();
	mapNoteOff.clear();
	mapChannelPressure.clear();
	mapSysEx.clear();
	mapNull.clear();
	cacheCC.clear();
	cacheAftertouch.clear();
	cachePitchWheel.clear();
	cacheProgramChange.clear();
	cacheNoteOn.clear();
	cacheNoteOff.clear();
	cacheChannelPressure.clear();
	cacheSysEx.clear();
	cacheNull.clear();
	messageContainer.clear();
}

CtrlrMidiMap &CtrlrMidiInputComparatorSingle::getMap(const CtrlrMidiMessageType t)
{
	switch (t)
	{
	case CC:
		return (mapCC);
	case PitchWheel:
		return (mapPitchWheel);
	case NoteOn:
		return (mapNoteOn);
	case NoteOff:
		return (mapNoteOff);
	case Aftertouch:
		return (mapAftertouch);
	case ProgramChange:
		return (mapProgramChange);
	case ChannelPressure:
		return (mapChannelPressure);
	default:
		return (mapNull);
	}
}

Array<CtrlrCacheDataSingle> &CtrlrMidiInputComparatorSingle::getCache(const CtrlrMidiMessageType t)
{
	switch (t)
	{
	case CC:
		return (cacheCC);
	case PitchWheel:
		return (cachePitchWheel);
	case NoteOn:
		return (cacheNoteOn);
	case NoteOff:
		return (cacheNoteOff);
	case Aftertouch:
		return (cacheAftertouch);
	case ProgramChange:
		return (cacheProgramChange);
	case ChannelPressure:
		return (cacheChannelPressure);
	default:
		return (cacheNull);
	}
}

void CtrlrMidiInputComparatorSingle::addMatchTarget (CtrlrModulator *m)
{
	const CtrlrMidiMessageType type = getMidiTypeFromModulator(m, 0, source);

	if (type == SysEx)
	{
		addMatchTargetSysEx(m);
		return;
	}

	CtrlrMidiMap &map		= getMap(type);
	CtrlrMidiMapIterator it = map.find(getMidiNumberFromModulator(m, source));

	if (it == map.end())
	{
		map.insert (CtrlrMidiMapPair(getMidiNumberFromModulator(m, source),m));
	}
	else
	{
		map[getMidiNumberFromModulator(m, source)].targets.add (m);
	}
}

void CtrlrMidiInputComparatorSingle::addMatchTargetSysEx (CtrlrModulator *m)
{
	BigInteger bi = memoryToBits(m->getMidiMessage(source).getMidiPattern());

	CtrlrMultiMidiMapIterator it = mapSysEx.find(bi);

	if (it == mapSysEx.end())
	{
		mapSysEx.insert (CtrlrMultiMidiMapPair(bi,m));
	}
	else
	{
		mapSysEx[bi].targets.add (m);
	}
}

void CtrlrMidiInputComparatorSingle::match (const MidiMessage &m) // Updated v5.6.34. Comparator was only updating the first modulator if several have the same CC index.
{
    _DBG("SINGLE_COMPARATOR_DBG: Entered match(MidiMessage) for: " + m.getDescription());

    messageContainer = m;
    CtrlrMidiMessageType type = midiMessageToType(m);
    int channel = m.getChannel();
    int number = getMidiNumberFromMidiMessage(m);

    // --- Debugging cacheMatch call ---
    _DBG("SINGLE_COMPARATOR_DBG: Calling cacheMatch for type=" + String(type) + ", number=" + String(number) + ", channel=" + String(channel));
    if (cacheMatch(type, number, channel))
    {
        _DBG("SINGLE_COMPARATOR_DBG: Cache matched, returning early. No further map lookup will occur for this message.");
        return; // THIS IS THE SUSPECTED EARLY EXIT POINT
    }
    _DBG("SINGLE_COMPARATOR_DBG: Cache did NOT match, proceeding to map lookup.");

    if (type == SysEx)
    {
        matchSysEx(m);
        return;
    }

    CtrlrMidiMap &map = getMap(type);

    if (map.size() != 0)
    {
        CtrlrMidiMapIterator it = map.find (number);

        if (it != map.end())
        {
            _DBG("SINGLE_COMPARATOR_DBG: Map found " + String((*it).second.targets.size()) + " targets for number " + String(number) + ".");
            for (int i=0; i < (*it).second.targets.size(); i++)
            {
                _DBG("SINGLE_COMPARATOR_DBG: Checking target " + (*it).second.targets[i]->getName() + " (Mod Channel: " + String((*it).second.targets[i]->getMidiMessage().getChannel()) + " vs Msg Channel: " + String(m.getChannel()) + ")");
                if (m.getChannel() == (*it).second.targets[i]->getMidiMessage().getChannel())
                {
                    _DBG("SINGLE_COMPARATOR_DBG: CHANNEL MATCH for (MAP) " + (*it).second.targets[i]->getName() + ". DISPATCHING.");
                    (*it).second.targets[i]->getProcessor().setValueFromMIDI (messageContainer);
                    updateCache (type, it); // updateCache might be moving the target to front, affecting future cacheMatch
                }
                else
                {
                    _DBG("SINGLE_COMPARATOR_DBG: Channel MISMATCH for (MAP) " + (*it).second.targets[i]->getName());
                }
            }
        }
        else
        {
            _DBG("SINGLE_COMPARATOR_DBG: No entry found in map for number " + String(number) + ".");
        }
    }
    else
    {
        _DBG("SINGLE_COMPARATOR_DBG: Map is empty for type " + String(type) + ".");
    }
    _DBG("SINGLE_COMPARATOR_DBG: Exiting match(MidiMessage).");
}

void CtrlrMidiInputComparatorSingle::matchSysEx(const MidiMessage &m)
{
	BigInteger bi = memoryToBits(MemoryBlock(m.getRawData(), m.getRawDataSize()));

	CtrlrMultiMidiMapIterator it;

	for (it=mapSysEx.begin(); it != mapSysEx.end(); it++)
	{
		if (compareMemory ((*it).first.toMemoryBlock(), messageContainer.getData()))
		{
			for (int i=0; i < (*it).second.targets.size(); i++)
			{
				(*it).second.targets[i]->getProcessor().setValueFromMIDI (messageContainer, source);
			}

			updateCacheSysEx (it);
			break;
		}
	}
}

void CtrlrMidiInputComparatorSingle::updateCache (const CtrlrMidiMessageType t, CtrlrMidiMapIterator &it)
{
	CtrlrCacheDataSingle c((*it).first, (*it).second);
	Array<CtrlrCacheDataSingle>	&cache = getCache(t);

	if (cache.contains (c))
	{
		if (cache.indexOf (c) == 0)
			return;
		else
		{
			cache.swap (cache.indexOf(c), 0);
		}
	}
	else
	{
		cache.insert (0, c);
		cache.resize (cacheSize);
	}
}

void CtrlrMidiInputComparatorSingle::updateCacheSysEx (CtrlrMultiMidiMapIterator &it)
{
	CtrlrCacheDataMulti c((*it).first, (*it).second);
	if (cacheSysEx.contains (c))
	{
		if (cacheSysEx.indexOf (c) == 0)
			return;
		else
		{
			cacheSysEx.swap (cacheSysEx.indexOf(c), 0);
		}
	}
	else
	{
		cacheSysEx.insert (0, c);
		cacheSysEx.resize (cacheSize);
	}
}

bool CtrlrMidiInputComparatorSingle::cacheMatch(CtrlrMidiMessageType type, const int number, const int channel)  // Updated v5.6.34. Comparator was only updating the first modulator if several have the same CC index.
{
    _DBG("CACHE_DBG: Entered cacheMatch for type=" + String(type) + ", number=" + String(number) + ", channel=" + String(channel));

    if (type == SysEx)
    {
        bool sysExMatch = cacheMatchSysEx();
        _DBG("CACHE_DBG: SysEx cache match result: " + String((int)sysExMatch)); // Using the confirmed working fix
        return (sysExMatch);
    }

    Array<CtrlrCacheDataSingle> &cache = getCache(type);
    _DBG("CACHE_DBG: Cache size for type " + String(type) + ": " + String(cache.size()));

    bool foundAndDispatchedAny = false; // Flag to track if any modulator was dispatched from this cache entry

    for (int i = 0; i < cache.size(); i++)
    {
        _DBG("CACHE_DBG: Checking cache entry [" + String(i) + "] - Key: " + String(cache[i].key) + " (Target Number: " + String(number) + ")");
        if (cache[i].key == number)
        {
            _DBG("CACHE_DBG: Cache key MATCH for number " + String(number) + " at index " + String(i) + ". Targets in cache entry: " + String(cache[i].mapData.targets.size()));
            for (int j = 0; j < cache[i].mapData.targets.size(); j++)
            {
                _DBG("CACHE_DBG: Checking cache target [" + String(j) + "] " + cache[i].mapData.targets[j]->getName() + " (Mod Channel: " + String(cache[i].mapData.targets[j]->getMidiMessage().getChannel()) + " vs Msg Channel: " + String(channel) + ")");
                if (cache[i].mapData.targets[j]->getMidiMessage().getChannel() == channel)
                {
                    _DBG("CACHE_DBG: CHANNEL MATCH for cache target " + cache[i].mapData.targets[j]->getName() + ". DISPATCHING.");
                    cache[i].mapData.targets[j]->getProcessor().setValueFromMIDI (messageContainer);
                    foundAndDispatchedAny = true; // Set flag if a dispatch occurred
                    // IMPORTANT: DO NOT return true here. Continue the inner loop
                    // to process all other modulators associated with this cache entry.
                }
                else
                {
                    _DBG("CACHE_DBG: Channel MISMATCH for cache target " + cache[i].mapData.targets[j]->getName());
                }
            }
            // After checking ALL targets within this specific cache entry for the matching key:
            if (foundAndDispatchedAny) {
                _DBG("CACHE_DBG: Processed all targets for cache key " + String(number) + ". Returning TRUE from cacheMatch.");
                return true; // Now it's safe to return true, as all relevant modulators for this key have been processed.
            }
        }
    }
    _DBG("CACHE_DBG: No match found in cache. Returning FALSE.");
    return (false);
}


bool CtrlrMidiInputComparatorSingle::cacheMatchSysEx ()
{
	for (int i=0; i<cacheSysEx.size(); i++)
	{
		if (compareMemory(cacheSysEx[i].key.toMemoryBlock(), messageContainer.getData()))
		{
			for (int j=0; j<cacheSysEx[i].mapData.targets.size(); j++)
			{
				cacheSysEx[i].mapData.targets[j]->getProcessor().setValueFromMIDI (messageContainer);
			}

			return (true);
		}
	}
	return (false);
}

const String CtrlrMidiInputComparatorSingle::dumpTableContents()
{
	String ret;

	for (int i=0; i<kMidiMessageType; i++)
	{
		CtrlrMidiMap &map = getMap((const CtrlrMidiMessageType)i);

		if (&map != &mapNull)
		{
			ret << "\n*****************************************************************************\n";
			ret << "\t\tMAP type: " << midiMessageTypeToString((const CtrlrMidiMessageType)i) << ", size=" << STR((uint32)map.size());
			ret << "\n*****************************************************************************\n";

			for(CtrlrMidiMapIterator itr = map.begin(); itr != map.end(); ++itr)
			{
				ret << "\n\tindex=" << String(itr->first) << " targets=" << String(itr->second.targets.size());
				for (int j=0; j<itr->second.targets.size(); j++)
				{
					ret << "\n\t\ttarget=" << itr->second.targets[j]->getName();
				}
			}
			ret << "\n*****************************************************************************\n\n";
		}
	}

	ret << "\n*****************************************************************************\n";
	ret << "\t\tMAP SysEx " << "size=" << STR((uint32)mapSysEx.size());
	ret << "\n*****************************************************************************\n";
	for(CtrlrMultiMidiMapIterator itr = mapSysEx.begin(); itr != mapSysEx.end(); ++itr)
	{
		MemoryBlock bl = itr->first.toMemoryBlock();
		ret << "\n\tindex=" << String::toHexString (bl.getData(), (int)bl.getSize(), 1) << " targets=" << String(itr->second.targets.size());
		for (int j=0; j<itr->second.targets.size(); j++)
		{
			ret << "\n\t\ttarget=" << itr->second.targets[j]->getName();
		}
	}
	ret << "\n*****************************************************************************\n\n";
	return (ret);
}
