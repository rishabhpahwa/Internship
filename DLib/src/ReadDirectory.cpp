/*
     * Copyright: Vehant Technologies (P) Ltd
     */

    /*
     * ReadDirectory.cpp
     * (Source File)
     * ------------------
     * Reads a directory and all sub-directories under the input base directory.
     * The function listFiles takes as input a baseDir path and a boolean value 
     * indicating whether or not to traverse the sub-directories as well. It 
     * returns a vector containg names of files. Additionally a user may also
     * sort the file names in lexicographic order by calling the function,
     * sortFileNames().
     *
     * Initial Author: Rishabh Pahwa
*/

#include "ReadDirectory.h"

using namespace std;

// An internal function used by listFiles to check whether the item corresponing
// to a path is a valid directory or not.
bool ReadDirectory::isDir(string dir)
{
	struct stat fileInfo;
	stat(dir.c_str(), &fileInfo);
	if (S_ISDIR(fileInfo.st_mode)) 
	{
		return true;
	} 
	else 
	{
		return false;
	}
}

// Sorts the given input vector of strings (Filenames in this case)
// and returns the sorted list.
std::vector<string> ReadDirectory::sortFileNames()
{
	std::sort(m_fileList.begin(), m_fileList.end());
	return m_fileList;
}

// Creates a list of complete file names with paths relative to the baseDirectory.
// Stores the paths in m_fileList.
std::vector<string> ReadDirectory::listFiles(string baseDir, bool recursive)
{
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(baseDir.c_str())) == NULL) 
	{
		cout << "[ERROR: " << errno << " ] Couldn't open " << baseDir << "." << endl;
		// return EXIT_FAILURE;
	}
	else
	{
		while ((dirp = readdir(dp)) != NULL) 
		{
			if (dirp->d_name != string(".") && dirp->d_name != string("..")) 
			{
				if (isDir(baseDir + dirp->d_name) == true && recursive == true) 
				{
					listFiles(baseDir + dirp->d_name + "/", true);
				}
				else 
				{
					m_fileList.push_back(string(baseDir + dirp->d_name));
				}
			}
		}
		closedir(dp);
	}
	return m_fileList;
}