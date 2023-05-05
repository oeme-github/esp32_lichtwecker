#include "filelib.h"

/**
 * @brief  filelib:listDir
 * @note   list content of directory
 * @param  &fs: 
 * @param  dirname: 
 * @param  levels: 
 * @retval None
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
  dbSerialPrintf("*** Listing directory: %s ***", dirname);

  File root = fs.open(dirname);
  if(!root){
    dbSerialPrintln("ERROR: Failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    dbSerialPrintln("ERROR: Not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file)
  {
    if(file.isDirectory())
    {
      dbSerialPrint("  DIR : ");
      dbSerialPrintln(file.name());
      if(levels)
      {
        listDir(fs, file.name(), levels -1);
      }
    } 
    else 
    {
      dbSerialPrint("  FILE: ");
      dbSerialPrint(file.name());
      dbSerialPrint("  SIZE: ");
      dbSerialPrintln(file.size());
    }
    file = root.openNextFile();
  }
}

/**
 * @brief  createDir(fs::FS &fs, const char * path)
 * @note   create directory
 * @param  &fs: 
 * @param  path: 
 * @retval None
 */
void createDir(fs::FS &fs, const char * path)
{
  dbSerialPrintf("*** Creating Dir: %s ***", path);
  if(!fs.mkdir(path))
  {
    dbSerialPrintln("ERROR: mkdir failed");
  }
}

/**
 * @brief  void removeDir(fs::FS &fs, const char * path)  
 * @note   removes directory and its content
 * @param  &fs: 
 * @param  path: 
 * @retval None
 */
void removeDir(fs::FS &fs, const char * path)
{
  dbSerialPrintf("*** Removing Dir: %s ***", path);
  if(!fs.rmdir(path))
  {
    dbSerialPrintln("ERROR: rmdir failed");
  }
}

/**
 * @brief  void renameFile(fs::FS &fs, const char * path1, const char * path2)
 * @note   renames file
 * @param  &fs: 
 * @param  path1: 
 * @param  path2: 
 * @retval None
 */
void renameFile(fs::FS &fs, const char * path1, const char * path2)
{
  dbSerialPrintf("*** Renaming file %s to %s ***", path1, path2);
  if(!fs.rename(path1, path2)) 
  {
    dbSerialPrintln("ERROR: Rename failed");
  }
}

/**
 * @brief  void deleteFile(fs::FS &fs, const char * path)
 * @note   delete file
 * @param  &fs: 
 * @param  path: 
 * @retval None
 */
void deleteFile(fs::FS &fs, const char * path)
{
  dbSerialPrintf("*** Deleting file: %s ***", path);
  if(!fs.remove(path))
  {
    dbSerialPrintln("ERROR: Delete failed");
  }
}
