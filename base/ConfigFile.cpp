#include <string.h>

#include "ConfigFile.h"
#include "util.h"


CConfigFile::CConfigFile(const std::string& configFileName)
	:m_strFileName(configFileName)
{
	loadConfig();
}

CConfigFile::~CConfigFile() 
{
}

void CConfigFile::loadConfig() 
{
	FILE* fp = ::fopen(m_strFileName.c_str(), "r");
	if (!fp) 
	{
		printf("open config file failed. err=%s\n", strerror(errno));
		return;
	}
	char buf[256];
	while (fgets(buf, 256, fp)) {
		//配置格式每行形如：listenport=20000

		if (buf[0] == '#' || buf[0] == '\n')
			continue;
		char* equal = strchr(buf, '=');
		if (!equal)
			continue;
		//将等号变为\0, 即将一行分割为key和value两部分
		*equal = 0;
		std::string strKey = trim(buf);
		std::string strValue = trim(equal+1);
		m_mapConfig[strKey] = strValue;
	}
}

std::string CConfigFile::getValue(const std::string& key)
{
	auto it = m_mapConfig.find(key);
	if (it == m_mapConfig.end()) {
		return "";
	}
	return it->second;
}

//void CConfigFile::printAll()
//{
//	for (auto it = m_mapConfig.begin(); it != m_mapConfig.end(); ++it) {
//		printf("%s : %s\n", it->first.c_str(), it->second.c_str());
//	}
//}