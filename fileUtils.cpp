/*
 *  fileUtils.cpp
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#include "fileUtils.h"
#include <cstring>
#include <string.h>

#if !defined (_MSC_VER)
#include <unistd.h>
#include <linux/limits.h>
#else 
#include <io.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#define ASSET_PREFIX "ASSET_PREFIX"

#if defined (_MSC_VER)
static int setenv(const char* name, const char* value, int overwrite)
{
    int errcode = 0;
    if (!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if (errcode || envsize) return errcode;
    }
    return _putenv_s(name, value);
}
#endif

std::string FileUtils::getAt2XPngFile (const char *file)
{
    if (strlen(file) < 4) return {};
    
    if (strncmp (file + strlen(file) - 4, ".png", 4) != 0)
        return {}; 

    char ret[PATH_MAX] = { 0 };

#if !defined (_MSC_VER)    
    memcpy (ret, file, strlen(file) - 4);
    strcat (ret, "@2x.png");
#else 
    strncpy_s (ret, file, strlen(file) - 4);
    strcat_s (ret, "@2x.png");
#endif    

    return std::string (ret); 
}


std::string FileUtils::getPrefixedPath (const std::string s)
{
    const char *prefix = getenv (ASSET_PREFIX);

    if (!prefix) return s;

    auto ret = std::string(prefix) + "/" + s ;

    return ret;
}

void FileUtils::initPrefix (const char *s)
{
    setenv (ASSET_PREFIX, s, 1);
}

int FileUtils::printPermissions(const char *fname)
{
    struct stat fileStat = { 0 };

    if (stat(fname, &fileStat) < 0)
    {    
        printf ("no stats for %s\n", fname);
        return -1;
    }

    printf("Information for %s\n", fname);
    printf("---------------------------\n");
    printf("File Size: \t\t%ld bytes\n", fileStat.st_size);
    printf("Number of Links: \t%lu\n", fileStat.st_nlink);
    printf("File inode: \t\t%lu\n", fileStat.st_ino);
#if !defined (_MSC_VER)
    printf("File Permissions: \t");
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");

    printf("The file %s a symbolic link\n", (S_ISLNK(fileStat.st_mode)) ? "is" : "is not");
#endif 
    return 0;
}


bool FileUtils::file_exists (const char *fname, bool show_permissions, bool show_missing)
{
#if !defined (_MSC_VER)
    if (access(fname, 0) == 0)
#else 
    if (_access(fname, 0) == 0)
#endif
    {
        // file exists
        if (show_permissions)
        {
            printf ("File exists: %s\n", fname);
            printPermissions (fname);
        }
        return true;
    }
    else if (show_missing)
    {
        printf ("File NOT found: %s\n", fname);
    }

    return false;
}
