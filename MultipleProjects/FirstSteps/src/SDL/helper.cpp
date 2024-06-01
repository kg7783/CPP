#include <iostream>
#include <fstream>
#include <random>
#include <sys/stat.h>
#include <filesystem>
#include <boost/filesystem.hpp>

#include <cmrc/cmrc.hpp>

#include "helper.h"

CMRC_DECLARE(FirstSteps);

#define SHOW_EXPR(expr) \
   std::cout << " =>> EXPR( " << #expr << " ) = " << (expr) << std::endl

/// Credits: https://stackoverflow.com/a/13059195
/// https://stackoverflow.com/questions/13059091/
struct membuf: std::streambuf 
{
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
    virtual ~membuf() = default;
};

/// Credits: https://stackoverflow.com/a/13059195
/// https://stackoverflow.com/questions/13059091/
struct memstream: virtual membuf, std::istream 
{

    memstream(char const* base, char* const end)
        : membuf(base, reinterpret_cast<uintptr_t>(end) - reinterpret_cast<uintptr_t>(base) )
        , std::istream(static_cast<std::streambuf*>(this)) { }

    memstream(char const* base, size_t size)
        : membuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) { }
};


using namespace std;

int dirExists(const char *path)
{
    struct stat info;

    if(stat( path, &info ) != 0)
        return 0;
    else if(info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}


string getFilePath(string file)
{
    
    auto fs = cmrc::FirstSteps::get_filesystem();

    std::cout << std::boolalpha;

    std::puts("\n ========= Experiment 1 ==>> Check resources ==========");

    string resource = "src/SDL/rc/"+file;
    SHOW_EXPR( fs.is_file(resource) );

    auto fd = fs.open(resource);

    auto is = memstream ( const_cast<char*>(fd.begin()),
                          const_cast<char*>(fd.end()) );

    size_t bmp = file.find(".bmp");
    size_t png = file.find(".png");
    size_t wav = file.find(".wav");

//    std::filesystem::path tmp{std::filesystem::temp_directory_path()};
//    std::filesystem::create_directories(tmp / "abcdef/example");
//    std::filesystem::remove_all(tmp / "abcdef")};
//    std::cout << "Deleted " << n << " files or directories\n";

    std::filesystem::path tmp{std::filesystem::temp_directory_path()};
    string path = tmp.append("FirstSteps/");
    if(dirExists(path.c_str()) == 0)
    {
        boost::filesystem::create_directories(path.c_str());
    }

    string outFile = path +"out" + std::to_string(rand());
    if(bmp != std::string::npos)
    {
        outFile += ".bmp";
    }
    else if(png != std::string::npos)
    {
        outFile += ".png";
    }
    else if(wav != std::string::npos)
    {
        outFile += ".wav";
    }

    auto ofs = std::ofstream(outFile);
    // Redirect is to ofs
    ofs << is.rdbuf();
    // Force writing
    ofs.flush();

    return string(outFile);


/*
	string fullFileName = "";

    std::string path = "";
    pid_t pid = getpid();
    char buf[20] = {0};
    sprintf(buf,"%",pid);
    std::string _link = "/proc/";
    _link.append( buf );
    _link.append( "/exe");
    char proc[512];
    int ch = readlink(_link.c_str(),proc,512);
    if (ch != -1) {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
    }

    fullFileName = path + string("/");
*/


/*
	char cwd[PATH_MAX];
   	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
   		printf("Current working dir: %s\n", cwd);
	} 
	else 
	{
       perror("getcwd() error");
   	}

	strcat(cwd, "/src/SDL/rc/");
	strcat(cwd, file.c_str());
	printf("Current file: %s\n", cwd);

    string s(cwd);

    return string(cwd);
*/

}

