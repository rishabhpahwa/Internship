#ifndef _READ_DIR_
#define _READ_DIR_

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <vector>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

using namespace std;

class ReadDirectory
{
    public:

    // Stores the list of files read.
    std::vector<string> m_fileList;

    // Checks if the input directory address corresponds to a directory
    // or not. Returns true if it is a directory.
    bool isDir(string dir);

    // Sorts the given input vector of strings (Filenames in this case)
    // and returns the sorted list.
    std::vector<string> sortFileNames();

    // Creates a list of complete file names with paths relative to the baseDirectory.
    // Stores the paths in m_fileList.
    std::vector<string> listFiles(string baseDir, bool recursive);

};

#endif
