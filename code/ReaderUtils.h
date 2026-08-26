#pragma once

const int32_t SRC_ENDIANLITTLE = 0;
const int32_t SRC_ENDIANBIG = 1;

// Start of setting up the ZLIB stuff.
#include "zlib.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) _setmode(_fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384
// End of setting up the ZLIB stuff.

/// <summary>
/// Reads all of the data from the file passed in.
/// </summary>
/// <param name="fileName">The path and name of the file.</param>
/// <returns>If the file is valid, the data of the file. Otherwise, a null pointer.</returns>
static char* ReadContentsFromFile(char* fileName) {
	FILE* currentFile = fopen(fileName, "rb");

	if (ferror(currentFile) != 0) {
		printf("Error occured while trying to open the file.\n");
		return nullptr;
	}

	fseek(currentFile, 0L, SEEK_END);
	int32_t length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	return data;
}

/// <summary>
/// Reads all of the data from the file passed in.
/// </summary>
/// <param name="fileName">The path and name of the file.</param>
/// <returns>If the file is valid, the data of the file. Otherwise, a null pointer.</returns>
static char* ReadContentsFromFile(char* fileName, size_t* outSize) {
	FILE* currentFile = fopen(fileName, "rb");

	if (ferror(currentFile) != 0) {
		printf("Error occured while trying to open the file.\n");
		return nullptr;
	}

	fseek(currentFile, 0L, SEEK_END);
	int32_t length = ftell(currentFile);
	fseek(currentFile, 0L, SEEK_SET);

	char* data = (char*)malloc(length);

	fread(data, sizeof(char), length, currentFile);

	fclose(currentFile);

	if (outSize != nullptr) {
		*outSize = length;
	}

	return data;
}

/// <summary>
/// Decompresses the given data within <paramref name="inputData"/>.
/// </summary>
/// <param name="inputData">The data that is to be decompressed.</param>
/// <param name="level">The level of the decompression.</param>
/// <param name="compedSize">The size of the compressed data.</param>
/// <param name="uncompedSize">The size of the decompressed data.</param>
/// <returns>If decompression is successful, the decompressed data. Otherwise, it returns what was passed in.</returns>
static char* InflateData(char* inputData, int32_t level, size_t compedSize, size_t uncompedSize) {
	char* outputData = (char*)malloc(uncompedSize);

	//Initialize ZLIB decompression in the event that we come across a compressed file.
	int32_t ret;
	unsigned have;
	z_stream strm;
	unsigned char* in = (unsigned char*)malloc(compedSize);
	unsigned char* out = (unsigned char*)malloc(uncompedSize);

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	if (inflateInit(&strm, level) != Z_OK) {
		printf("An error occured while trying to initialize inflate.\n");
		free(in);
		return inputData;
	}

	memcpy(in, inputData, compedSize);

	strm.avail_in = compedSize;
	strm.next_in = in;

	strm.avail_out = uncompedSize;
	strm.next_out = out;

	ret = inflate(&strm, Z_NO_FLUSH);

	if (ret != Z_OK && ret != Z_STREAM_END) {
		printf("An error occured while trying to decompress the inputted data. Error code %d\n", ret);
		inflateEnd(&strm);

		// Just return the original data if we fail to decompress. Probably just means we don't need to decompress it.
		memcpy(outputData, inputData, compedSize);
		return outputData;
	}

	if (ret == Z_OK) {
		printf("The inputted data decompressed successfully.\n");
	}

	if (ret == Z_STREAM_END) {
		printf("The inputted data decompressed successfully and hit the end of the stream.\n");
	}

	memcpy(outputData, out, uncompedSize);

	free(in);
	free(out);

	inflateEnd(&strm);

	return outputData;
}

/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
char* inf(FILE* source)
{
	int32_t ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return nullptr;

	char* dest = (char*)malloc(CHUNK);
	int32_t pos = 0;

	/* decompress until deflate stream ends or end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			return dest;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			if (ret == Z_STREAM_ERROR) {
				return dest;
			}
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return dest;
			}
			have = CHUNK - strm.avail_out;

			dest = (char*)realloc(dest, pos + have);

			memcpy(dest + pos, out, have);

			pos += have;
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);

	return dest;
}

static void glStringReplace(char* str, char c, char rep) {
	for (int32_t i = 0; i < strlen(str); i++) {
		if (str[i] == c) str[i] = rep;
	}
}

/// Taken directly from Mumbo's Motors, developed by XephyrCraft. Generates a new header checksum for R1 Bundles.
/// CREDIT TO MOJOBOJO FOR THIS AWESOME, WORKING HEADER CHECKSUM RECALCULATOR.
/// Calculate the new header checksum if pointers change.
static uint32_t checksum32(char* data, size_t length) {
	uint32_t r11 = 0;

	uint32_t r8 = 0;
	uint32_t r10 = 0;

	for (int32_t i = 0; i < length; i++)
	{
		r8 = data[i];
		r10 = r11 << 4;

		if ((r8 & 0x80) > 0)
		{
			r11 = 0xFFFFFF80 | r8;
		}
		else
		{
			r11 = r8;
		}

		r11 = r11 + r10;
		r10 = r11 & 0xF0000000;

		if (r10 != 0)
		{
			r8 = r10 >> 24;
			r10 = r8 | r10;
			r11 = r10 ^ r11;
		}
	}
	return r11;
}

/// <summary>
/// Converts the wide character array provided to a regular character array.
/// </summary>
/// <param name="ws">- The wide character array.</param>
/// <param name="size">- The size of the character array.</param>
/// <returns>The character array equivalent of the supplied wide character array.</returns>
static char* compileWSToS(wchar_t* ws, size_t size) {
	char* buf = (char*)malloc(size);
	memset(buf, 0, size);

	for (int32_t i = 0; i < size / 2; i++) {
		buf[i] = (char)(ws[i] & 0xFF);
	}

	return buf;
}

/// <summary>
/// Converts the regular character array provided to a wide character array.
/// </summary>
/// <param name="s">- the regular character array.</param>
/// <param name="size">- The size of the character array.</param>
/// <returns>The wide character array equivalent of the supplied character array.</returns>
static wchar_t* compileSToWS(char* s, size_t size) {
	wchar_t* buf = (wchar_t*)malloc(size);
	memset(buf, 0, size);

	for (int32_t i = 0; i < size / 2; i++) {
		buf[i] = (wchar_t)(s[i]);
	}

	return buf;
}

static int32_t* compileChar3ToIntArray(char val1, char val2, char val3) {
	int32_t* finalVal = (int32_t*)malloc(12);
	memset(finalVal, 0, 12);

	finalVal[0] += (int32_t)val1;
	finalVal[1] += (int32_t)val2;
	finalVal[2] += (int32_t)val3;

	return finalVal;
}

static char* compileIntArrayToChar3(int32_t* arr) {
	char* finalVal = (char*)malloc(3);
	memset(finalVal, 0, 3);

	finalVal[0] += (char)arr[0];
	finalVal[1] += (char)arr[1];
	finalVal[2] += (char)arr[2];

	return finalVal;
}

static int32_t compileU8ColToU32Col(char r, char g, char b, char a) {
	char* tmpVal = (char*)malloc(4);
	uint32_t finalVal = 0;

	tmpVal[0] = r;
	tmpVal[1] = g;
	tmpVal[2] = b;
	tmpVal[3] = a;

	memcpy(&finalVal, tmpVal, 4);

	free(tmpVal);
	return finalVal;
}

static char* compileU32ColToU8Col(uint32_t rgba) {
	char finalVal[4];

	finalVal[0] = rgba & 0xFF;
	finalVal[1] = (rgba >> 8) & 0xFF;
	finalVal[2] = (rgba >> 16) & 0xFF;
	finalVal[3] = (rgba >> 24) & 0xFF;

	return finalVal;
}

/// <summary>
/// Flips the endian of the inputted value.
/// </summary>
/// <param name="val"></param>
/// <returns><paramref name="val"/>, opposite of the current endian.</returns>
static uint64_t flipEndian(uint64_t val) {
	return ((0xFF00000000000000 & val) >> 56) | ((0x00FF000000000000 & val) >> 40) | ((0x0000FF0000000000 & val) >> 24) | ((0x000000FF00000000 & val) >> 8) | ((0x00000000ff000000 & val) << 8) | ((0x0000000000ff0000 & val) << 24) | ((0x000000000000ff00 & val) << 40) | ((0x00000000000000ff & val) << 56);
}

/// <summary>
/// Flips the endian of the inputted value.
/// </summary>
/// <param name="val"></param>
/// <returns><paramref name="val"/>, opposite of the current endian.</returns>
static int64_t flipEndian(int64_t val) {
	return ((0xFF00000000000000 & val) >> 56) | ((0x00FF000000000000 & val) >> 40) | ((0x0000FF0000000000 & val) >> 24) | ((0x000000FF00000000 & val) >> 8) | ((0x00000000ff000000 & val) << 8) | ((0x0000000000ff0000 & val) << 24) | ((0x000000000000ff00 & val) << 40) | ((0x00000000000000ff & val) << 56);
}

/// <summary>
/// Flips the endian of the inputted value.
/// </summary>
/// <param name="val"></param>
/// <returns><paramref name="val"/>, opposite of the current endian.</returns>
static int32_t flipEndian(int32_t val) {
	return ((0xFF000000 & val) >> 24) | ((0x00FF0000 & val) >> 8) | ((0x0000FF00 & val) << 8) | ((0x000000FF & val) << 24);
}

/// <summary>
/// Flips the endian of the inputted value.
/// </summary>
/// <param name="val"></param>
/// <returns><paramref name="val"/>, opposite of the current endian.</returns>
static uint32_t flipEndian(uint32_t val) {
	return ((0xFF000000 & val) >> 24) | ((0x00FF0000 & val) >> 8) | ((0x0000FF00 & val) << 8) | ((0x000000FF & val) << 24);
}

/// <summary>
/// Flips the endian of the inputted value. <paramref name="endian"/> specifies the endianness of the incoming data.
/// </summary>
/// <param name="val"></param>
/// <param name="endian"></param>
/// <returns>Outputs a float, opposite of the specified endian.</returns>
static float flipEndian_f32(char* val, int32_t endian) {
	char* tmpVal = (char*)malloc(4);
	float finalVal = 0;

	tmpVal[0] = val[3];
	tmpVal[1] = val[2];
	tmpVal[2] = val[1];
	tmpVal[3] = val[0];

	memcpy(&finalVal, tmpVal, 4);

	free(tmpVal);
	return finalVal;
}

/// <summary>
/// Flips the endian of the inputted value.
/// </summary>
/// <param name="val"></param>
/// <returns><paramref name="val"/>, opposite of the current endian.</returns>
static int16_t flipEndian(int16_t val) {
	return ((0xFF00 & val) >> 8) | ((0x00FF & val) << 8);
}

/// <summary>
/// Flips the endian of the inputted value.
/// </summary>
/// <param name="val"></param>
/// <returns><paramref name="val"/>, opposite of the current endian.</returns>
static uint16_t flipEndian(uint16_t val) {
	return ((0xFF00 & val) >> 8) | ((0x00FF & val) << 8);
}

/// <summary>
/// Flips the endian of the inputted value.
/// </summary>
/// <param name="val"></param>
/// <returns><paramref name="val"/>, opposite of the current endian.</returns>
static wchar_t flipEndian(wchar_t val) {
	return ((0xFF00 & val) >> 8) | ((0x00FF & val) << 8);
}