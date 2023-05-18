#pragma once

#include <string>
#include <FS.h>
#include <vector>
#include <map>
#include <libDebug.h>

typedef std::map<std::string, std::string> MapConfig;

class MyConfigServer
{
private:
	FS *fs;
	String configFile;
	MapConfig mapConfig;
	MapConfig *ptrMapConfig = &mapConfig;
	
	boolean loaded=false;
	boolean loadConfigMap(String content);

public:
	MyConfigServer();
	~MyConfigServer() = default;

	bool loadConfig(FS *_fs, const char *_filename);
	bool loadConfig(FS *_fs);

	void printConfig();
	bool isConfigLoaded(){return this->loaded;}

	void saveToConfigfile();
	bool saveConfig(MapConfig *_map);

 	bool containsKey(const char *_index); 
	std::string getElement( const char *_index );
	void putElement( std::string _index, std::string _value );
	void putElement( std::string _index, int _value );
};
