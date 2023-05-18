#include <libFile.h>

void deleteRecursive(FS *fs, const char *path)
{
  File file = fs->open((char *)path);
  if (!file.isDirectory()) {
    file.close();
    fs->remove((char *)path);
    return;
  }
  file.rewindDirectory();
  while (true) {
    File entry = file.openNextFile();
    if (!entry) {
      break;
    }
    String entryPath = (String)path + "/" + entry.name();
    if (entry.isDirectory()) {
      entry.close();
      deleteRecursive(fs, entryPath.c_str());
    } else {
      entry.close();
      fs->remove((char *)entryPath.c_str());
    }
    yield();
  }
  fs->rmdir((char *)path);
  file.close();
}

void makeDir(FS *fs, const char * path)
{
  dbSerialPrintf("*** Creating Dir: %s ***", path);
  if(!fs->mkdir(path))
  {
    dbSerialPrintln("ERROR: mkdir failed");
  }
}


void writeFile(FS *fs, const char * path)
{
  dbSerialPrintf("*** writeFile: %s ***", path);
  File file = fs->open((char *)path, FILE_WRITE);
  if (file)
  {
    file.write(0);
    file.close();
  }
}

boolean existsOnFs(FS *fs, const char * path)
{
    return fs->exists(path);
}

/**
 * @brief  humanReadableSize(const size_t bytes)
 * @note   returns a Sting with humanreadable size of memory
 * @param  bytes: const size 
 * @retval String
 */
String humanReadableSize(const size_t bytes) 
{
    if (bytes < 1024) 
        return String(bytes) + " B";
    else if (bytes < (1024 * 1024)) 
        return String(bytes / 1024.0) + " kB";
    else if (bytes < (1024 * 1024 * 1024)) 
        return String(bytes / 1024.0 / 1024.0) + " MB";
    else 
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}
