﻿// LinesCounter.cpp : Defines the entry point for the console application.
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
std::map<int , std::string > resultSorted;

std::map<std::string, std::string> colors = {
{ "c" , "#555555"} ,
{ "cs" , "#178600" } ,
{ "cpp" , "#f34b7d" } ,
{ "CMakeLists", "#cccccc" },
{ "css", "#563d7c" },
{ "Dockerfile", "#0db7ed" },
{ "yml", "#0db7ed" },
{ "html", "#e34c26" },
{ "js", "#f1e05a" },
{ "gitignore", "#f50000" },
{ "ignore", "#f50000" },
{ "md", "#02f88c" },
{ "jsx", "#d8bf10" },
{ "sln" , "#865fc5" },
{ "pyproj" , "#865fc5" },
{ "py" , "#2372a5" },
{ "sh" , "#89e051" },
{ "json" , "#d6ae39"},
};


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

template<typename A, typename B>
std::pair<B, A> flip_pair(const std::pair<A, B> &p)
{
	return std::pair<B, A>(p.second, p.first);
}

template<typename A, typename B>
std::map<B, A> flip_map(const std::map<A, B> &src)
{
	std::map<B, A> dst;
	std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
		flip_pair<A, B>);
	return dst;
}

std::string GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}

void addValue(fs::path FilePath, int count) {
	std::string key;


	if (FilePath.filename().string() == "CMakeLists.txt")
		key = "CMakeLists";
	else if (FilePath.filename().string() == "Dockerfile")
		key = "Dockerfile";
	else key = FilePath.extension().string().erase(0, 1);



	if (result.count(key) == 0)
		result.insert(std::make_pair(key, 0));
	result[key] += count;
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

	addValue(FilePath, count);

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
void createBadge(float x, float y, std::string color1, std::string color2, std::string value1, std::string value2, float width1, float width2, float height, int fontSize, std::ofstream& out) {
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
	out << "' font-size='";
	out << fontSize;
	out << "px'>";
	out << value1;
	out << "</text><text x='";
	out << width1 + width2 / 2;
	out << "' y='";
	out << height / 2 + 4;
	out << "' fill='#010101' fill-opacity='.3'></text><text x='";
	out << width1 + width2 / 2;
	out << "' y='";
	out << height / 2 + 4;
	out << "' font-size='";
	out << fontSize;
	out << "px'>";
	out << value2;
	out << "</text></g></svg>";

	BadgeID++;
}

std::string getColor(std::string FileKey) {
	if (colors.count(FileKey) == 0)
		return  "#9E9E9E";
	else return colors[FileKey];
}
int main()
{
	//get path from settings 
	std::vector<fs::path> temp;
	//debug ----------------
	//fs::path Path("D:\\lineCounter"); // empty path
	//std::vector<fs::path> Ignore; 

	//release  only ---------------------
	fs::path Path(GetCurrentWorkingDir()); // empty path
	std::vector<fs::path> Ignore = GetIgnorePaths(Path);

	int res = LineCount(Path, Ignore);
	std::ofstream out("lineCounter.svg");
	out << "<svg xmlns = 'http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'>";


	int height = 25;
	int mainW1 = 40, mainW2 = 100;
	int x = mainW1 + mainW2;
	int y = 0;
	int space = 2;
	int maxWidth = 890;
	createBadge(0, y, "#5b5b5b", "#ff69b4", "lines", std::to_string(res), mainW1, mainW2, height,12, out);


	resultSorted = flip_map(result);
	int count = 0;
	for (auto const &v : resultSorted) {
		count++;
		if (count < resultSorted.size()-2) 
			continue;

		int w1 = v.second.length() * 7 + 30, w2 = 70;
		x += space;
		if (x + w1 + w2 + space > maxWidth) { x = 0; y += height + space; }
		createBadge(x, y, "#5b5b5b", getColor(v.second), v.second, std::to_string(v.first), w1, w2, height,12, out);
		x += w1 + w2;
	
		
	}

	out << "</svg>";

	out.close();

	std::cout << "***" << res << "***  line of Code " << (res < 100 ? ":(" : ":)") << std::endl;

	return 0;
}