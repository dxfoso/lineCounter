// LinesCounter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>
#include <filesystem> 
#include <algorithm> 
#include <iterator>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>

#if defined(_WIN32)
namespace fs = std::experimental::filesystem::v1;
#elif defined(__linux__)
namespace fs = std::filesystem;
#endif

/////////////////GetCurrentWorkingDir///////////////////////////////////////////////
#include <stdio.h>  /* defines FILENAME_MAX */
#if defined(_WIN32)
#include <direct.h>
#define GetCurrentDir _getcwd
#elif defined(__linux__)
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


std::string GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}
/////////////////GetCurrentWorkingDir///////////////////////////////////////////////



int LineCountFile(fs::path FilePath, std::vector<fs::path>& Ignore) {
	if (fs::is_directory(FilePath))return 0;



	//check Ignore file list first 
	for (fs::path const& value : Ignore)
	{
		if (!value.string()[0] == '*')continue;
		if (FilePath.extension().compare(value.extension()) == 0)
			return 0;
	}

	std::ifstream inFile(FilePath);
	int count = std::count(std::istreambuf_iterator<char>(inFile),
		std::istreambuf_iterator<char>(), '\n');

	std::string c = std::to_string(count);


	std::cout << " " << c << std::setw(10 - c.length()) << " : " << FilePath << std::endl;
	return count;
}
int LineCount(fs::path Path, std::vector<fs::path>& Ignore) {
	//check Ignore file list first 
	for (fs::path const& value : Ignore)
	{
		if (value.string()[0] == '*')continue;
		if (Path.compare(value) == 0)
			return 0;
	}

	if (!fs::is_directory(Path))return LineCountFile(Path, Ignore);
	int res = 0;
	for (auto & p : fs::directory_iterator(Path)) {
		if (!fs::is_directory(p.path().string()))
			res += LineCountFile(p.path().string(), Ignore);
		else
			res += LineCount(p, Ignore);
	}
	std::cout <<  res << " ------  " << Path << std::endl << std::endl;
	return res;
}

bool getFileContent(std::string fileName, std::vector<fs::path> & vecOfStrs, int lineCount = -1)
{

	// Open the File
	std::ifstream in(fileName.c_str());

	// Check if object is valid
	if (!in)
	{
		std::cerr << "Cannot open the File : " << fileName << std::endl;
		return false;
	}

	std::string str;
	// Read the next line from File untill it reaches the end.

	if (lineCount == -1) {//not limit of line count 
		while (std::getline(in, str))
		{
			if (str.size() > 0)
				vecOfStrs.push_back(str);
		}
	}
	else {//apply limit on line count
		int count = 0;
		while (std::getline(in, str) && count < lineCount)
		{
			if (str.size() > 0)
				vecOfStrs.push_back(str);
			count++;
		}
	}
	//Close The File
	in.close();
	return true;
}




std::vector<fs::path> GetIgnorePaths(fs::path Parent) {

	std::vector<fs::path> temp;
	std::vector<fs::path> res;

	std::string currentPath = GetCurrentWorkingDir();

	fs::path p1; // empty path
	p1 += currentPath; // does not insert a separator
	p1.append("ignore.txt"); // does not insert a separator

	getFileContent(p1.string(), temp);


	//combine with the parent path 
	for (fs::path const& value : temp) {
		fs::path t;
		if (value.string()[0] != '*')
			t.append(Parent);
		t.append(value);
		res.push_back(t);
	}
	return res;
}

int main()
{
	//get path from settings 
	std::vector<fs::path> temp;
	fs::path p1(GetCurrentWorkingDir()); // empty path
	p1.append("path.txt"); // does not insert a separator
	getFileContent(p1.string(), temp, 1);
	fs::path Path = temp[0];


	std::vector<fs::path> Ignore = GetIgnorePaths(Path);




	int res = 0;
	res = LineCount(Path, Ignore);
	//std::cout << res;
	std::cin >> res;
	return 0;
}





