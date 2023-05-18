#include "MyConfigServer.h"

/**
 * @brief  split() - splits std::string s with std::string delimiter in its elemts
 * @note   the retunging vectors contains the elements
 * @param  s: std::string 
 * @param  delimiter: std::string 
 * @retval std::vector
 */
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) 
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

/**
 * @brief  MyConfigServer() constructor
 * @note   
 * @retval 
 */
MyConfigServer::MyConfigServer()
{
}

/**
 * @brief  loadConfog(FS *_fs)
 * @note   calls loadConfig(_fs, "/config.json", FileFormat::JSON)
 * @param  *_fs: pointer to filesystem
 * @retval 
 */
boolean MyConfigServer::loadConfig(FS *_fs)
{
    return this->loadConfig(_fs, "/config.map");
}

/**
 * @brief  loadConfig(FS *_fs, const char *_filename, FileFormat _format)
 * @note   load confog file and stores the key value pairs in member variables
 * @param  *_fs: pointer to filesystem
 * @param  *_filename: name of config file
 * @param  _format: FileFormat::JSON or FileFormat::MAP
 * @retval true|false success of loading
 */
boolean MyConfigServer::loadConfig(FS *_fs, const char *_filename)
{
    /* ------------------------------------------------- */
    /* init                                              */
    this->fs         = _fs;
    this->configFile = _filename;
    /* ------------------------------------------------- */
    /* filesystem mounted                                */
    if(this->fs->exists(this->configFile)) 
    {
      /* ----------------------------------------------- */ 
      /* file exists, reading and loading                */
      File file = this->fs->open(this->configFile, FILE_READ);
      if(file) 
      {
        String content = file.readString();
        this->loaded = this->loadConfigMap(content);  
      }
      file.close();
    }
    else
    {
      this->loaded = false;
    }
    return this->loaded;
}

/**
 * @brief  printConfig()
 * @note   
 * @retval None
 */
void MyConfigServer::printConfig()
{
  for (auto const& x : this->mapConfig)
  {
    dbSerialPrintf( "%s -> %s", x.first.c_str(), x.second.c_str());
  }
}

/**
 * @brief  saveToConfigFile()
 * @note   saves the config-data to its file
 * @retval None
 */
void MyConfigServer::saveToConfigfile()
{
  dbSerialPrintf("save to file");
  int ret = 0;
  File file = this->fs->open(this->configFile, FILE_WRITE);
  if (file) 
  {
    for (auto const& x : this->mapConfig)
    {
      file.print(x.first.c_str());file.print(":");file.print(x.second.c_str());file.println(",");
    }
    file.close();
  }
  else
  {
    dbSerialPrintf("can not open file:[%s]", this->configFile);
  }    
}


/**
 * @brief  saveConfig(MapConfig *_map)
 * @note   saves a map configuration
 * @param  *_map: pointer to MapConfig
 * @retval true
 */
boolean MyConfigServer::saveConfig(MapConfig *_map)
{
  for (auto itr = _map->begin(); itr != _map->end(); itr++)
  {
    this->mapConfig[itr->first] = itr->second;
  }
  this->saveToConfigfile();
  return true;
}

/**
 * @brief  loadConfigMap(String content)
 * @note   loads a map config from String content 
 * @param  content: String content
 * @retval true
 */
boolean MyConfigServer::loadConfigMap(String content)
{
  std::string test = content.c_str();
  // remove \n\r from content
  test.erase(std::remove(test.begin(), test.end(), '\n'), test.cend());
  test.erase(std::remove(test.begin(), test.end(), '\r'), test.cend());
  test.erase(std::remove(test.begin(), test.end(), ' ' ), test.cend());
  std::vector<std::string> vecHelper;
  std::vector<std::string> elemt;
  std::string key;
  std::string value;
  // here we have somthing like 
  // B5:/03 IT WON'T TAKE LONG.MP3,B5:/03 IT WON'T TAKE LONG.MP3,B5:/03 IT WON'T TAKE LONG.MP3,....
  vecHelper = split(test, ",");  
  for( int i=0; i<vecHelper.size(); i++)
  {
    // now we something like 
    // B5:/03 IT WON'T TAKE LONG.MP3 in each element
    std::string strElemt = vecHelper[i].c_str();
    elemt = split( strElemt, ":" );
    // now we have 
    // B5 and /03 IT WON'T TAKE LONG.MP3 separated
    if( elemt.size() == 2)
    {
      key   = elemt[0];
      value = elemt[1];
      // finaly we put it in the map
      this->mapConfig[key] = value;
    }    
  }
  return true;
}

/**
 * @brief  containsKey(const char *_index)
 * @note   check if a key exists in json or map
 * @param  *_index: const char pointer to key
 * @retval true|false
 */
bool MyConfigServer::containsKey(const char *_index)
{
  return (this->mapConfig.find(_index) != this->mapConfig.end());
}

/**
 * @brief  getElement( const char *_index )
 * @note   returns a value to a given key
 * @param  *_index: const char pointer to key
 * @retval 
 */
std::string MyConfigServer::getElement( const char *_index )
{
  if( !this->containsKey(_index) )
  {
    dbSerialPrintf("key[%s] not in config", _index);
    return "";
  }
  return this->mapConfig[_index];
}

/**
 * @brief  putElement( const char *_index, const char *_value )
 * @note   puts an key value pair in a map or json document
 * @param  *_index: std::string
 * @param  *_value: std::string
 * @retval None
 */
void MyConfigServer::putElement( std::string _index, std::string _value )
{ 
  this->mapConfig[_index]=_value;
}

/**
 * @brief  putElement( const char *_index, const char *_value )
 * @note   puts an key value pair in a map or json document
 * @param  *_index: std::string
 * @param  *_value: int
 * @retval None
 */
void MyConfigServer::putElement(  std::string _index, int _value )
{ 
  this->mapConfig[_index]=std::to_string(_value);
}
