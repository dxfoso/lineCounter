// LinesCounter.cpp : Defines the entry point for the console application.
//
#include <string>
#include <iostream>
#include <experimental/filesystem> 
#include <algorithm> 
#include <iterator>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>



#if defined(_WIN32)
namespace fs = std::experimental::filesystem::v1;
namespace fs2 = std::experimental::filesystem::v1;
#elif defined(__linux__)
namespace fs = std::experimental::filesystem::v1::__cxx11;
namespace fs2 = std::experimental::filesystem;
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
	if (fs2::is_directory(FilePath))return 0;



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

	if (!fs2::is_directory(Path))return LineCountFile(Path, Ignore);
	int res = 0;
	for (auto & p : fs::directory_iterator(Path)) {
		if (!fs2::is_directory(p.path().string()))
			res += LineCountFile(p.path().string(), Ignore);
		else
			res += LineCount(p, Ignore);
	}
	std::cout << res << " ------folder " << Path << std::endl << std::endl;
	return res;
}
bool getFileContent(std::string fileName, std::vector<fs::path> & vecOfStrs, int lineCount = -1)
{

	// Open the File
	std::ifstream in(fileName.c_str());

	// Check if object is valid
	if (!in)
	{
		std::cerr << "no lineCounter.ignore file provided at " << fileName << "continue without ignore file" << std::endl;
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

	fs::path ignore(Parent);
	ignore.append("lineCounter.ignore");
	getFileContent(ignore.string(), temp);

	//combine with the parent path 
	for (fs::path const& value : temp) {
		fs::path t;
		if (value.string()[0] != '*')
			t.append(Parent.string());
		t.append(value.string());
		res.push_back(t);
	}
	return res;
}

int main()
{
	//get path from settings 
	std::vector<fs::path> temp;
	fs::path Path(GetCurrentWorkingDir()); // empty path

	std::vector<fs::path> Ignore = GetIgnorePaths(Path);

	int res = LineCount(Path, Ignore);
	std::ofstream out("lineCounter.svg");

	out << "<svg xmlns = 'http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='156' height='20'><linearGradient id='b' x2='0' y2='100%'><stop offset='0' stop-color='#bbb' stop-opacity='.1'/><stop offset='1' stop-opacity='.1'/></linearGradient><clipPath id='a'><rect width='156' height='20' rx='3' fill='#fff'/></clipPath><g clip-path='url(#a)'><path fill='";
	out << "#5b5b5b";
	out << "' d='M0 0h85v20H0z'/><path fill= '";
	out << "#9E9E9E";
	out << "' d='M85 0h71v20H85z'/><path fill='url(#b)' d='M0 0h156v20H0z'/></g><g fill='#fff' text-anchor='middle' font-family='DejaVu Sans,Verdana,Geneva,sans-serif' font-size='11'><text x='42.5' y='15' fill='#010101' fill-opacity='.3'>";
	out << "Lines Count";
	out << "</text><text x='42.5' y='14'>";
	out << "Lines Count :";
	out << "</text><text x='119.5' y='15' fill='#010101' fill-opacity='.3'>";
	out << res;
	out << "</text><text x='119.5' y='14'>";
	out << res;
	out << "</text></g></svg>";
	out.close();

	std::cout << "***" << res << "***" << std::endl;

	return 0;
}