
#ifndef __LIBFILE_H__
#define __LIBFILE_H__

#include <FS.h>
#include <libDebug.h>


void deleteRecursive(FS *fs, const char *path);
void makeDir(FS *fs, const char *path);
void writeFile(FS *fs, const char * path);
boolean existsOnFs(FS *fs, const char * path);
String humanReadableSize(const size_t bytes);

#endif