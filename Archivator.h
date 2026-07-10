#pragma once
#include "RLE.h"
#include "Archivator.h"
#include "CRCCompresR.h"
#include "ัสั.h"
#include "FileWriter.h"
#include "PositionWriter.h"
#include "ScopeControl.h"
#include "Structs.h"
#include "FormatWriter.h"
#include "FileProcesor.h"
#include <iostream>
#include <string>
#include <vector>
class ArchivatorControl
{
public:
	/*void  Archive(const std::string& fileName, std::vector<std::string>& files);*/
	void Archive(
		const std::string& archiveName,
		const std::string& folder);
	void  Extract(const std::string& filename);
private:
	void AddFolder(
		const fs::path& folder,
		const fs::path& root,
		std::vector<std::string>& files
	);
};