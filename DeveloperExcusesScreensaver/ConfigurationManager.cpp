#include "ConfigurationManager.h"

#include "framework.h"
#include "nlohmann/json.hpp"
#include "tstring.h"

using json = nlohmann::json;

namespace
{
	const auto settingsDirectory = tstring(TEXT("DeveloperExcusesScreensaver"));
	const auto settingsFilename = tstring(TEXT("settings.json"));

	const auto timerPeriodKey = std::string("timer_period");
	const auto fontDataKey = std::string("font_data");
	const auto separateQuoteKey = std::string("separate_quote");

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

	template<typename T>
	T tryGetValue(const json& j, const T& defaultValue)
	{
		if (j.is_null())
			return defaultValue;
		return j.get<T>();
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

	m_config.timerPeriod = tryGetValue(j[timerPeriodKey], m_config.timerPeriod);

	// TODO avoid encoding just to provide default value
	std::string defaultFontData = convert_to_string(encodeBase64(m_config.fontData));
	m_config.fontData = decodeBase64(convert_to_tstring(tryGetValue(j[fontDataKey], defaultFontData)));

	m_config.separateQuote = tryGetValue(j[separateQuoteKey], m_config.separateQuote);
}

void ConfigurationManager::save() const
{
	auto fontDataEncoded = encodeBase64(m_config.fontData);

	json j;
	j[timerPeriodKey] = m_config.timerPeriod;
	j[fontDataKey] = convert_to_string(fontDataEncoded);
    j[separateQuoteKey] = m_config.separateQuote;

	auto settingsPath = getSettingsPath();
	auto res = SHCreateDirectory(NULL, settingsPath.directoryPath.c_str());

	{
		std::ofstream ofs{ settingsPath.getFilePath() };
		ofs << j;
	}
}
