#include "stdafx.h"
#include "stdafx_luabind.h"
#include "CtrlrLuaObjectWrapper.h"
#include "CtrlrLuaMemoryBlock.h"
#include "CtrlrUtilities.h"
#include "CtrlrLog.h"

CtrlrLuaMemoryBlock::CtrlrLuaMemoryBlock()
{
}

CtrlrLuaMemoryBlock::CtrlrLuaMemoryBlock(const int initialSize, bool zeroData) : mb(initialSize, zeroData)
{
}

CtrlrLuaMemoryBlock::CtrlrLuaMemoryBlock(const void *dataToInitialiseFrom, int sizeInBytes) : mb(dataToInitialiseFrom, sizeInBytes)
{
}

CtrlrLuaMemoryBlock::CtrlrLuaMemoryBlock(const MemoryBlock &other) : mb(other)
{
}

CtrlrLuaMemoryBlock::CtrlrLuaMemoryBlock(luabind::object const &initialData)
{
	for(luabind::iterator i(initialData), end; i != end; i++)
	{
		const uint8 v = luabind::object_cast<uint8>(*i);
		mb.append (&v, 1);
	}
}

CtrlrLuaMemoryBlock::~CtrlrLuaMemoryBlock()
{
}

const CtrlrLuaMemoryBlock CtrlrLuaMemoryBlock::getRange(const int startByte, const int numBytes) const
{
	MemoryBlock ret(numBytes,true);

	for (int i=0; i<numBytes; i++)
	{
		if (i+startByte < mb.getSize())
			ret[i] = mb[i+startByte];
		else
			break;
	}

	return (ret);
}

const int CtrlrLuaMemoryBlock::getByte(const int bytePosition) const
{
	if (bytePosition >= mb.getSize())
	{
		_LERR("CtrlrLuaMemoryBlock::getByte position: "+String((unsigned int)bytePosition)+" is beyond bounds of this memory block with size: "+String((unsigned int)mb.getSize()));
		return (0);
	}

	return ((uint8)mb[bytePosition]);
}

void CtrlrLuaMemoryBlock::setByte(const unsigned int bytePosition, const int byteValue)
{
	if (bytePosition < mb.getSize())
	{
		mb[bytePosition] = byteValue;
	}
	else
	{
		_LERR("CtrlrLuaMemoryBlock::getByte position: "+String((unsigned int)bytePosition)+" is beyond bounds of this memory block with size: "+String((unsigned int)mb.getSize()));
	}
}

void CtrlrLuaMemoryBlock::append(const CtrlrLuaObjectWrapper &dataToAppend)
{
	for(luabind::iterator i(dataToAppend.getObject()), end; i != end; i++)
	{
		const uint8 v = luabind::object_cast<uint8>(*i);
		mb.append (&v, 1);
	}
}

void CtrlrLuaMemoryBlock::append(const uint8 byte)
{
	mb.append (&byte, 1);
}

const int CtrlrLuaMemoryBlock::getSize() const
{
	return ((int)mb.getSize());
}

const String CtrlrLuaMemoryBlock::toString ()
{
	return (mb.toString());
}

const String CtrlrLuaMemoryBlock::toSafeString ()
{
	return (removeInvalidChars(mb.toString()));
}

const String CtrlrLuaMemoryBlock::toHexString(const int groupSize) const 
{
	return (String::toHexString (mb.getData(), (int)mb.getSize(), groupSize));
}

void CtrlrLuaMemoryBlock::loadFromHexString(const String &string)
{
	mb.loadFromHexString (string);
}

void CtrlrLuaMemoryBlock::copyFrom (CtrlrLuaMemoryBlock &sourceData, int destinationOffset, int numBytes)
{
	mb.copyFrom (sourceData.getData(), destinationOffset, numBytes);
}

void CtrlrLuaMemoryBlock::copyTo (CtrlrLuaMemoryBlock &destinationData, int sourceOffset, int numBytes)
{
	mb.copyTo (destinationData.getData(), sourceOffset, numBytes);
}

void CtrlrLuaMemoryBlock::insert (CtrlrLuaMemoryBlock &dataToInsert, int numBytesToInsert, int insertPosition)
{
	mb.insert (dataToInsert.getData(), numBytesToInsert, insertPosition);
}

void CtrlrLuaMemoryBlock::removeSection (int startByte, int numBytesToRemove)
{
	mb.removeSection (startByte, numBytesToRemove);
}
/*
* 
* This code is worth keeping because it can decompress gzip files, which are common.
* Unfortunately, JUCE does not provide a way of compressing to gzip.
* However, zlib is more common for in-memory compression.
* 
String CtrlrLuaMemoryBlock::decompressGzip()
{
	try
	{
		if (getSize() == 0)
			return String("Error: Empty memory block");

		// Check GZIP header
		if (getSize() < 2)
			return String("Error: File too small");

		uint8 byte1 = ((uint8*)getData())[0];
		uint8 byte2 = ((uint8*)getData())[1];

		if (byte1 != 0x1f || byte2 != 0x8b)
			return String("Error: Invalid GZIP header");

		_DBG("decompressGzip: Creating stream with gzipFormat");

		// Try with explicit gzipFormat
		GZIPDecompressorInputStream gzipStream(
			new MemoryInputStream(getData(), getSize(), false),
			true,
			GZIPDecompressorInputStream::gzipFormat
		);

		_DBG("decompressGzip: Stream created, exhausted=" + String(gzipStream.isExhausted() ? "true" : "false"));
		_DBG("decompressGzip: Stream total length=" + String((int64)gzipStream.getTotalLength()));

		// Read all at once
		String result = gzipStream.readEntireStreamAsString();

		_DBG("decompressGzip: Result length=" + String(result.length()));

		if (result.isEmpty())
		{
			// Try other formats
			_DBG("decompressGzip: Trying zlibFormat");
			GZIPDecompressorInputStream gzipStream2(
				new MemoryInputStream(getData(), getSize(), false),
				true,
				GZIPDecompressorInputStream::zlibFormat
			);
			result = gzipStream2.readEntireStreamAsString();

			if (result.isEmpty())
			{
				_DBG("decompressGzip: Trying deflateFormat");
				GZIPDecompressorInputStream gzipStream3(
					new MemoryInputStream(getData(), getSize(), false),
					true,
					GZIPDecompressorInputStream::deflateFormat
				);
				result = gzipStream3.readEntireStreamAsString();
			}
		}

		if (result.isEmpty())
			return String("Error: All decompression formats failed");

		return result;
	}
	catch (const std::exception& e)
	{
		return String("Error: ") + e.what();
	}
	catch (...)
	{
		return String("Error: Unknown exception");
	}
}

// Standalone function for Lua
String luaDecompressGzip(CtrlrLuaMemoryBlock& mb)
{
	return mb.decompressGzip();
}
*/


String CtrlrLuaMemoryBlock::decompressZlib()
{
	try
	{
		if (getSize() == 0)
			return String("Error: Empty memory block");

		// --- REMOVED: Strict ZLIB Header Check (byte1 != 0x1f || byte2 != 0x8b) ---
		// We assume the user might be opening a ZLIB stream (which starts with 0x78)
		// or a ZLIB file. We will rely on the GZIPDecompressorInputStream to handle it.

		// Check for minimum size for any compressed data
		if (getSize() < 4)
			return String("Error: Data stream too small");

		// --- 1. ATTEMPT DECOMPRESSION WITH ZLIB FORMAT (Most Likely Format from Your Save) ---
		_DBG("decompressZlib: Attempting with zlibFormat");

		// Note: The 'true' argument deletes the MemoryInputStream when done.
		GZIPDecompressorInputStream zlibStream(
			new MemoryInputStream(getData(), getSize(), false),
			true,
			GZIPDecompressorInputStream::zlibFormat
		);

		String result = zlibStream.readEntireStreamAsString();

		if (result.isEmpty())
		{
			// --- 2. FALLBACK TO ZLIB FORMAT ---
			_DBG("decompressZlib: zlibFormat failed. Trying gzipFormat for standard ZLIB files.");
			GZIPDecompressorInputStream gzipStream(
				new MemoryInputStream(getData(), getSize(), false),
				true,
				GZIPDecompressorInputStream::gzipFormat
			);
			result = gzipStream.readEntireStreamAsString();
		}

		// --- 3. FALLBACK TO RAW DEFLATE FORMAT ---
		if (result.isEmpty())
		{
			_DBG("decompressZlib: gzipFormat failed. Trying raw deflateFormat.");
			GZIPDecompressorInputStream deflateStream(
				new MemoryInputStream(getData(), getSize(), false),
				true,
				GZIPDecompressorInputStream::deflateFormat
			);
			result = deflateStream.readEntireStreamAsString();
		}


		if (result.isEmpty())
			return String("Error: All decompression formats failed");

		_DBG("decompressZlib: Successfully decompressed data. Result length=" + String(result.length()));

		return result;
	}
	catch (const std::exception& e)
	{
		return String("Error: ") + e.what();
	}
	catch (...)
	{
		return String("Error: Unknown exception");
	}
}

// Standalone function for Lua (No change needed here)
String luaDecompressZlib(CtrlrLuaMemoryBlock& mb)
{
	return mb.decompressZlib();
}


// Standalone function for Lua
CtrlrLuaMemoryBlock luaCompressZlib(CtrlrLuaMemoryBlock& mb)
{
	return mb.compressZlib();
}

CtrlrLuaMemoryBlock CtrlrLuaMemoryBlock::compressZlib()
{
	// 1. INPUT CHECK (Should be the first operation)
	if (getSize() == 0)
	{
		_DBG("compressZlib: Empty input");
		return CtrlrLuaMemoryBlock(); // Return an empty block immediately
	}

	_DBG("compressZlib: Compressing " + String(getSize()) + " bytes");

	// 2. DECLARE OUTPUT BLOCK (Only once)
	MemoryBlock outputBlock;

	// 3. EXECUTE COMPRESSION IN A SCOPE BLOCK
	{
		// Create a memory output stream that writes to outputBlock (does NOT take ownership of outputBlock)
		MemoryOutputStream outputStream(outputBlock, false);

		// Create ZLIB compressor. The 3rd argument 'false' means it does NOT delete outputStream.
		GZIPCompressorOutputStream gzipStream(&outputStream, 9, false);

		// Write the input data to the compressor
		gzipStream.write(getData(), getSize());

		// When 'gzipStream' goes out of scope here, its destructor luaDecompressZlibis called, 
		// which finalizes and writes the ZLIB footer to 'outputStream'/'outputBlock'.
	} // <-- Streams are destroyed, outputBlock is now fully populated.

	_DBG("compressZlib: Compressed to " + String((int)outputBlock.getSize()) + " bytes");

	// 4. RETURN RESULT
	// Return a new Lua MemoryBlock wrapping the now-populated outputBlock
	return CtrlrLuaMemoryBlock(outputBlock);
}


String CtrlrLuaMemoryBlock::decompressGzip()
{
	try
	{
		if (getSize() < 4)
			return String("Error: Data stream too small");

		// --- 1. ATTEMPT DECOMPRESSION WITH GZIP FORMAT (Priority) ---
		_DBG("decompressGzip: Attempting with gzipFormat");

		GZIPDecompressorInputStream gzipStream(
			new MemoryInputStream(getData(), getSize(), false),
			true,
			GZIPDecompressorInputStream::gzipFormat // <-- GZIP PRIORITY
		);

		String result = gzipStream.readEntireStreamAsString();

		if (result.isEmpty())
		{
			// --- 2. FALLBACK TO ZLIB FORMAT ---
			_DBG("decompressGzip: gzipFormat failed. Trying zlibFormat.");
			GZIPDecompressorInputStream zlibStream(
				new MemoryInputStream(getData(), getSize(), false),
				true,
				GZIPDecompressorInputStream::zlibFormat
			);
			result = zlibStream.readEntireStreamAsString();
		}

		// --- 3. FALLBACK TO RAW DEFLATE FORMAT ---
		if (result.isEmpty())
		{
			_DBG("decompressGzip: Fallbacks failed. Trying raw deflateFormat.");
			GZIPDecompressorInputStream deflateStream(
				new MemoryInputStream(getData(), getSize(), false),
				true,
				GZIPDecompressorInputStream::deflateFormat
			);
			result = deflateStream.readEntireStreamAsString();
		}

		if (result.isEmpty())
			return String("Error: All decompression formats failed");

		_DBG("decompressGzip: Successfully decompressed data. Result length=" + String(result.length()));
		return result;
	}
	catch (const std::exception& e)
	{
		return String("Error: ") + e.what();
	}
	catch (...)
	{
		return String("Error: Unknown exception");
	}
}

// Standalone function for Lua
String luaDecompressGzip(CtrlrLuaMemoryBlock& mb)
{
	return mb.decompressGzip();
}

// Your compressZlib:
// GZIPCompressorOutputStream gzipStream(&outputStream, 9, false); // false = Zlib format

// NEW compressGzip:
CtrlrLuaMemoryBlock CtrlrLuaMemoryBlock::compressGzip()
{
	if (getSize() == 0)
	{
		_DBG("compressGzip: Empty input");
		return CtrlrLuaMemoryBlock();
	}

	_DBG("compressGzip: Compressing " + String(getSize()) + " bytes (Gzip format)");

	MemoryBlock outputBlock;
	{
		MemoryOutputStream outputStream(outputBlock, false);

		// The third parameter 'true' tells JUCE to output a GZIP-formatted stream.
		GZIPCompressorOutputStream gzipStream(&outputStream, 9, true);

		gzipStream.write(getData(), getSize());
	}

	_DBG("compressGzip: Compressed to " + String((int)outputBlock.getSize()) + " bytes");

	return CtrlrLuaMemoryBlock(outputBlock);
}

// Standalone function for Lua
CtrlrLuaMemoryBlock luaCompressGzip(CtrlrLuaMemoryBlock& mb)
{
	return mb.compressGzip();
}


void CtrlrLuaMemoryBlock::wrapForLua(lua_State* L)
{
	using namespace luabind;

	module(L)
		[
			class_<CtrlrLuaMemoryBlock>("CtrlrLuaMemoryBlock")
				.def(constructor<>())
				.def(constructor<const int, bool>())
				.def(constructor<luabind::object const&>())
				.def("getByte", &CtrlrLuaMemoryBlock::getByte)
				.def("setByte", &CtrlrLuaMemoryBlock::setByte)
				.def("append", (void(CtrlrLuaMemoryBlock::*)(const uint8)) & CtrlrLuaMemoryBlock::append)
				.def("append", (void(CtrlrLuaMemoryBlock::*)(const CtrlrLuaObjectWrapper&)) & CtrlrLuaMemoryBlock::append)
				.def("getRange", &CtrlrLuaMemoryBlock::getRange)
				.def("toHexString", &CtrlrLuaMemoryBlock::toHexString)
				.def("toString", &CtrlrLuaMemoryBlock::toString)
				.def("getSize", &CtrlrLuaMemoryBlock::getSize)
				.def("loadFromHexString", &CtrlrLuaMemoryBlock::loadFromHexString)
				.def("toSafeString", &CtrlrLuaMemoryBlock::toSafeString)
				.def("copyFrom", (void(CtrlrLuaMemoryBlock::*)(CtrlrLuaMemoryBlock&, int, int)) & CtrlrLuaMemoryBlock::copyFrom)
				.def("copyTo", (void(CtrlrLuaMemoryBlock::*)(CtrlrLuaMemoryBlock&, int, int)) & CtrlrLuaMemoryBlock::copyTo)
				.def("insert", &CtrlrLuaMemoryBlock::insert)
				.def("removeSection", &CtrlrLuaMemoryBlock::removeSection),
				def("decompressZlib", &luaDecompressZlib), // Zlib support
				def("compressZlib", &luaCompressZlib), //  Zlib support
				def("decompressGzip", &luaDecompressGzip),//  Gzip support
				def("compressGzip", &luaCompressGzip)//  Gzip support

		];
}