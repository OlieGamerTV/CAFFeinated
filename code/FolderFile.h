#pragma once
#ifndef _FOLDERFILEH
#define _FOLDERFILEH
#include <vector>

struct FolderFile
{
    int fileCount = 0;
    std::vector<char*> fileNames;
    
    void ClearActiveData()
    {
        fileCount = 0;
        fileNames.clear();
    };
};

#endif