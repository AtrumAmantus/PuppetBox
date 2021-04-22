#pragma once

#include <string>

#include "AssetArchive.h"
#include "Logger.h"

namespace PB
{
	class AssetLibrary
	{
	public:
		AssetLibrary(std::string archiveRoot) : archiveRoot_(archiveRoot) {};
		bool loadArchive(std::string archiveName)
		{
			bool error = false;

			if (assetLibraries_.find(archiveName) == assetLibraries_.end())
			{
				AssetArchive archive{ archiveName, archiveRoot_ };
				archive.init(&error);

				if (!error)
				{
					assetLibraries_.insert(
						std::pair<std::string, AssetArchive>{archiveName, archive}
					);

					LOGGER_DEBUG("'" + archiveName + "' loaded " + std::to_string(archive.assetCount()) + " assets");
				}
				else
				{
					LOGGER_ERROR("Failed to load archive '" + archiveName + "'");
				}
			}
			else
			{
				LOGGER_WARN("Archive '" + archiveName + "' is already loaded");
			}

			return !error;
		};
	private:
		std::string archiveRoot_;
		std::unordered_map<std::string, AssetArchive> assetLibraries_{};
	};
}