#include "ConfigurationManager.h"

#include "framework.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace
{
	const auto settingsDirectory = tstring(TEXT("DeveloperExcusesScreensaver"));
	const auto settingsFilename = tstring(TEXT("settings.json"));

	const auto timerPeriodKey = std::string("timer_period");
	const auto fontDataKey = std::string("font_data");

	struct SettingsPath
	{
		std::string getFilePath()
		{
			auto fullPath = directoryPath + TEXT("\\") + settingsFilename;

			return convert_to_string(fullPath);
		}

		tstring directoryPath;
	};

	tstring encodeBase64(const std::vector<unsigned char>& binaryData)
	{
		auto binaryDataSize = static_cast<DWORD>(binaryData.size());
		DWORD encodedDataLength = 0;
		CryptBinaryToString(binaryData.data(), binaryDataSize, CRYPT_STRING_BASE64, nullptr, &encodedDataLength);
		std::vector<TCHAR> encodedData;
		encodedData.resize(encodedDataLength);
		auto result = CryptBinaryToString(binaryData.data(), binaryDataSize, CRYPT_STRING_BASE64, encodedData.data(), &encodedDataLength);

		if (!result)
		{
			std::stringstream ss;
			ss << "Failed to encode binary string with the error 0x" << std::hex << GetLastError() << std::dec;
			throw std::runtime_error(ss.str());
		}

		return tstring(encodedData.begin(), encodedData.end());
	}

	std::vector<unsigned char> decodeBase64(const tstring& encodedData)
	{
		auto encodedDataSize = static_cast<DWORD>(encodedData.size());
		DWORD binaryDataLength = 0;
		CryptStringToBinary(encodedData.data(), encodedDataSize, CRYPT_STRING_BASE64, nullptr, &binaryDataLength, nullptr, nullptr);
		std::vector<unsigned char> binaryData;
		binaryData.resize(binaryDataLength);
		CryptStringToBinary(encodedData.data(), encodedDataSize, CRYPT_STRING_BASE64, binaryData.data(), &binaryDataLength, nullptr, nullptr);

		return binaryData;
	}

	SettingsPath getSettingsPath()
	{
		tstring directoryFullPath = {};

		LPTSTR path = nullptr;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path)))
		{
			directoryFullPath = tstring(path) + TEXT("\\") + settingsDirectory;

			CoTaskMemFree(path);
			path = nullptr;
		}

		return SettingsPath{ directoryFullPath };
	}
}

ConfigurationManager::ConfigurationManager(const Configuration& defaultConfig)
{
	m_config = defaultConfig;
	load();
}

void ConfigurationManager::load()
{
	json j;

	{
		std::ifstream ifs{ getSettingsPath().getFilePath() };

		if (!ifs.is_open())
			return;

		ifs >> j;
	}

	auto timerPeriod = j[timerPeriodKey].get<int>();
	auto fontData = decodeBase64(convert_to_tstring(j[fontDataKey].get<std::string>()));

	m_config = Configuration{
		timerPeriod,
		fontData,
	};
}

void ConfigurationManager::save() const
{
	auto fontDataEncoded = encodeBase64(m_config.fontData);

	json j;
	j[timerPeriodKey] = m_config.timerPeriod;
	j[fontDataKey] = convert_to_string(fontDataEncoded);

	auto settingsPath = getSettingsPath();
	auto res = SHCreateDirectory(NULL, settingsPath.directoryPath.c_str());

	{
		std::ofstream ofs{ settingsPath.getFilePath() };
		ofs << j;
	}
}
