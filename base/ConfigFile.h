
#include <string>
#include <unordered_map>

class CConfigFile
{
public:
    CConfigFile(const std::string& configFilename);
    ~CConfigFile();
    std::string getValue(const std::string& key);
    //void printAll();

private:
    void loadConfig();
    std::string m_strFileName;
    std::unordered_map<std::string, std::string> m_mapConfig;
};