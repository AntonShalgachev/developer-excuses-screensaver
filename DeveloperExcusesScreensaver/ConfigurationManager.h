#pragma once

#include "framework.h"

class ConfigurationManager
{
public:
	struct Configuration
	{
	public:
		int timerPeriod = 0;
		std::vector<unsigned char> fontData;
	};

	ConfigurationManager(const Configuration& defaultConfig);

	Configuration& getConfiguration() { return m_config; }

	void load();
	void save() const;

private:
	Configuration m_config;
};

