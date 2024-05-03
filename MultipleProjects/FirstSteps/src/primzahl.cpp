#include <math.h>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

static void check_file(string file);

static bool prim(unsigned long n);
static bool prim_(unsigned long n);


void check_file(string file)
{
    try 
    {
        if (std::filesystem::remove(file))
        {
            std::cout << "file " << file << " deleted.\n";
        }
        else
        {
            std::cout << "file " << file << " not found.\n";
        }
    }
    catch(const std::filesystem::filesystem_error& err) 
    {
        std::cout << "filesystem error: " << err.what() << '\n';
    }
}

static bool prim(unsigned long n)
{

	int x;

	//////////////////////

    if(n<2)
    {
        return false;
    } 

	if(n==2)
    {
        return true;
    }

	if(n%2==0)
    {
        return false;
    }

	for(x=3;x<=sqrt(n);x+=2)
    {
		if(n%x==0)
        {
            return false;
        }
	}

	return true;

	//////////////////////

}

static bool prim_(unsigned long n)
{

	int x;

	//////////////////////

    if(n<2)
    {
        return false;
    } 

	if(n==2)
    {
        return true;
    }

    for(unsigned long int i=2; i<n; i++)
    {
        if(n%i == 0)
        {
            return false;
        }
    }

	return true;

	//////////////////////

}

void manage_primzahl(unsigned long lower_limit, unsigned long upper_limit)
{
    string file = "test.txt";
    check_file(file);

    std::ofstream outfile;
    outfile.open(file, std::ios_base::app); // append instead of overwrite

    for(unsigned long i=lower_limit; i<= upper_limit; i++)
    {
        bool result = prim_(i);
        if(result == true)
        {
            outfile << i << endl;
            cout << i << endl;    
        }
    }

    outfile.close();
}