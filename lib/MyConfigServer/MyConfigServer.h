#pragma once

#include <string>
#include <FS.h>
#ifdef CONFIG_WITH_JSON
	#include <ArduinoJson.h>
#endif
#include <vector>
#include <map>

#ifdef CONFIG_WITH_JSON
	#define CONFIG_JSON_SIZE 512
#endif

typedef enum FileFormat {JSON, MAP} fileFormat;

typedef std::map<std::string, std::string> MapConfig;

class MyConfigServer
{
private:
	FS *fs;
	String configFile;
	FileFormat format;
#ifdef CONFIG_WITH_JSON
	StaticJsonDocument<CONFIG_JSON_SIZE> jsonConfig;
#endif
	MapConfig mapConfig;
	MapConfig *ptrMapConfig = &mapConfig;
	
	boolean loaded=false;

#ifdef CONFIG_WITH_JSON
	boolean loadConfigJson(String content);
#endif
	boolean loadConfigMap(String content);

public:
	MyConfigServer();
	~MyConfigServer() = default;

	bool loadConfig(FS *_fs, const char *_filename, FileFormat _format);
	bool loadConfig(FS *_fs);

	void printConfig();
	bool isConfigLoaded(){return this->loaded;}

	void saveToConfigfile();
#ifdef CONFIG_WITH_JSON
	bool saveConfig(const JsonDocument& jsonDoc);
#endif
	bool saveConfig(MapConfig *_map);

 	bool containsKey(const char *_index); 
	std::string getElement( const char *_index );
	void putElement( const char *_index, const char *_value );
};
