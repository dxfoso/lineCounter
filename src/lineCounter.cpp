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
#include <map> 
int BadgeID = 0;
std::map<std::string, int > result;


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
int   LineCountFile(fs::path FilePath, std::vector<fs::path>& Ignore) {
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

	std::string  ext = FilePath.extension().string().erase(0, 1);
	if (result.count(ext) == 0)
		result.insert(std::make_pair(ext, 0));
	result[ext] += count;

	//only for command line 
	std::string c = std::to_string(count);
	std::cout << " " << c << std::setw(10 - c.length()) << " : " << FilePath << std::endl;
	//only for command line
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

	if (!fs2::is_directory(Path)) return LineCountFile(Path, Ignore);
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
void createBadge(float x, float y, std::string color1, std::string color2, std::string value1, std::string value2, float width1, float width2, float height, std::ofstream& out) {
	int totalWidth = width1 + width2;


	out << "<svg xmlns = 'http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' width='";
	out << totalWidth;
	out << "' height='";
	out << height;
	out << "' ";
	out << "x='";
	out << x;
	out << "' ";
	out << "y='";
	out << y;
	out << "' ";
	out << "><linearGradient id='b' x2='0' y2='100%'><stop offset='0' stop-color='#bbb' stop-opacity='.1'/><stop offset='1' stop-opacity='.1'/></linearGradient><clipPath id='a";
	out << BadgeID;
	out << "'><rect width='";
	out << totalWidth;
	out << "' height='";
	out << height;
	out << "'";
	out << " rx='3' fill='#fff'/></clipPath><g clip-path='url(#a";
	out << BadgeID;
	out << ")'><path fill='";
	out << color1;
	out << "' d='M0 0h";
	out << width1;
	out << "v";
	out << height;
	out << "H0z'/><path fill= '";
	out << color2;
	out << "' d='M";
	out << width1;
	out << " 0h";
	out << width2;
	out << "v";
	out << height;
	out << "H";
	out << width1;
	out << "z'/><path fill='url(#b)' d='M0 0h";
	out << totalWidth;
	out << "v";
	out << height;
	out << "H0z'/></g><g fill='#fff' text-anchor='middle' font-family='DejaVu Sans,Verdana,Geneva,sans-serif' font-size='11'><text x='";
	out << width1 / 2;
	out << "' y='";
	out << height / 2 + 4;
	out << "' fill='#010101' fill-opacity='.3'></text><text x='";
	out << width1 / 2;
	out << "' y='";
	out << height / 2 + 4;
	out << "'>";
	out << value1;
	out << "</text><text x='";
	out << width1 + width2 / 2;
	out << "' y='";
	out << height / 2 + 4;
	out << "' fill='#010101' fill-opacity='.3'></text><text x='";
	out << width1 + width2 / 2;
	out << "' y='";
	out << height / 2 + 4;
	out << "'>";
	out << value2;
	out << "</text></g></svg>";

	BadgeID++;
}
int main()
{
	//get path from settings 
	std::vector<fs::path> temp;
	fs::path Path(GetCurrentWorkingDir()); // empty path

	std::vector<fs::path> Ignore = GetIgnorePaths(Path);

	int res = LineCount(Path, Ignore);
	std::ofstream out("lineCounter.svg");
	out << "<svg xmlns = 'http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'>";



	createBadge(0, 0, "#5b5b5b", "#9E9E9E", "Line Code Total", std::to_string(res), 100, 80, 20, out);
	int x = 180;
	for (auto const &v : result) {
		int w1 = 40, w2 = 50;
		createBadge(x, 0, "#5b5b5b", "#9E9E9E", v.first, std::to_string(v.second), w1, w2, 20, out);

		x += w1 + w2 + 1;
	}

	out << "</svg>";

	out.close();

	std::cout << "***" << res << "***" << std::endl;

	return 0;
}