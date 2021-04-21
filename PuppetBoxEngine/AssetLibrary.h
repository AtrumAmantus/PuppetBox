#pragma once

#include <string>

#include "AssetArchive.h"
#include "Logger.h"

class AssetLibrary
{
public:
	bool loadArchive(std::string archiveName)
	{
		bool success;

		if (assetLibraries_.find(archiveName) == assetLibraries_.end())
		{
			AssetArchive archive{ archiveName };
			archive.init();

			assetLibraries_.insert(
				std::pair<std::string, AssetArchive>{archiveName, archive}
			);

			LOGGER_DEBUG("'" + archiveName + "' loaded " + std::to_string(archive.assetCount()) + " assets");
			success = true;
		}
		else
		{
			success = false;
		}

		return success;
	};
private:
	std::unordered_map<std::string, AssetArchive> assetLibraries_{};
};