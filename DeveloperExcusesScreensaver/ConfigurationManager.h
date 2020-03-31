#pragma once

#include "framework.h"

struct Configuration
{
public:
	int timerPeriod = 0;
	//std::string fontData;
	std::vector<unsigned char> fontData;
};

class ConfigurationManager
{
public:
	ConfigurationManager(const Configuration& defaultConfig);

	Configuration& getConfiguration() { return m_config; }

	void load();
	void save() const;

private:
	Configuration m_config;
};

