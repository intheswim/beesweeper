/*
 *  fileUtils.h
 *  BeeSweeper
 *
 *  Created by Yuriy Yakimenko on 10/15/22.
 *  Copyright 2022. All rights reserved.
 *
 */

#pragma once
#include <string>

class FileUtils
{
    public:
    static std::string getAt2XPngFile (const char *file);
    static std::string getPrefixedPath (const std::string s);
    static void initPrefix (const char *s);
    static bool file_exists (const char *fname, bool show_permissions, bool show_missing);
    
    private:
    static int printPermissions(const char *fname);
};