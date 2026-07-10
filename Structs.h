#pragma once
#include <cstdint>
#include <string>
#include <vector>
struct ArchiveHeader
{
	char magic[6] = "SIGMA";
	uint32_t version = 1;
	uint64_t fileCount;
	uint64_t IndexofSet;
};
struct ChunkInfo
{
	uint64_t offset;
	uint32_t size;
	uint32_t crc;
	uint32_t originalSize;
};
struct FileEntry
{
	std::string name;
	uint64_t size;
	uint64_t offset;

	bool isDirectory = false;

	uint64_t compresedSize;
	bool compresed;
	uint32_t crc;

	std::vector<ChunkInfo> chunks;
};