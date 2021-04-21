#pragma once

#include <string>
#include <unordered_set>

#include <ZipLib/ZipFile.h>

class AssetLibrary
{
public:
	AssetLibrary(std::string archivePath) : archivePath_(archivePath) {};
	void init()
	{
		auto archive = ZipFile::Open(archivePath_);
		
		for (uint32_t i = 0; i < archive->GetEntriesCount(); ++i)
		{
			archiveAssets_.insert(archive->GetEntry(i)->GetName());
		}
	};
	bool hasAsset(std::string assetName)
	{
		return archiveAssets_.find(assetName) != archiveAssets_.end();
	};
	uint32_t assetCount()
	{
		return archiveAssets_.size();
	};
private:
	std::string archivePath_;
	std::unordered_set<std::string> archiveAssets_{};
};