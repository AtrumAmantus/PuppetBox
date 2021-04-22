#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <STBI/stb_image.h>
#include <ZipLib/ZipFile.h>

#include "Logger.h"
#include "Utilities.h"

namespace PB
{
	class AssetArchive
	{
	public:
		AssetArchive(std::string archiveName, std::string archiveRoot) : archiveName_(archiveName), archiveRoot_(archiveRoot) {};
		void init(bool* error)
		{
			auto archive = ZipFile::Open(archiveRoot_ + archiveName_, false);
			
			if (archive != nullptr)
			{
				auto manifest = archive->GetEntry("manifest.info");

				if (manifest != nullptr)
				{
					loadManifest(*manifest);

					for (uint32_t i = 0; i < archive->GetEntriesCount(); ++i)
					{
						archiveAssets_.insert(archive->GetEntry(i)->GetName());
					}
				}
				else
				{
					*error = true;
					LOGGER_ERROR("Invalid/corrupt archive '" + archiveName_ + "'");
				}
			}
			else
			{
				*error = true;
				LOGGER_ERROR("Missing archive '" + archiveName_ + "'");
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
		uint8_t* readImageData(std::string assetName, bool* error)
		{
			if (hasAsset(assetName))
			{
				stbi_set_flip_vertically_on_load(true);

				auto archive = ZipFile::Open(archiveRoot_ + archiveName_);
				auto asset = archive->GetEntry(assetName);
				auto stream = asset->GetDecompressionStream();

				stream->seekg(0, stream->end);
				int32_t streamLength = stream->tellg();
				stream->seekg(0, stream->beg);

				char* buffer = new char[streamLength];

				stream->read(buffer, streamLength);

				int32_t x, y, nChan;

				return stbi_load_from_memory((uint8_t*)buffer, streamLength, &x, &y, &nChan, 0);
			}
			else
			{
				*error = true;
				return nullptr;
			}
		};
	private:
		std::string archiveName_;
		std::string archiveRoot_;
		std::unordered_set<std::string> archiveAssets_{};
		std::unordered_map<std::string, std::string> archiveAssetIds_{};
	private:
		void loadManifest(ZipArchiveEntry& manifest)
		{
			auto stream = manifest.GetDecompressionStream();
			std::string line;

			while (std::getline(*stream, line))
			{
				StringUtils::trim(&line);

				std::string* values = nullptr;
				uint32_t valueCount;
				StringUtils::split(line, &values, &valueCount);

				if (valueCount == 2)
				{
					archiveAssetIds_.insert(
						std::pair<std::string, std::string>{ values[0], values[1] }
					);
				}
				else
				{
					LOGGER_ERROR("Invalid manifest entry '" + line + "'");
				}
			}
		};
	};
}