#include <boost/program_options.hpp>
#include "../user_map.h"

using namespace boost;
namespace po = boost::program_options;

#include <iostream>
#include <algorithm>
#include <iterator>
using namespace std;
using namespace user_map;


// A helper function to simplify the main part.
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
    copy(v.begin(), v.end(), ostream_iterator<T>(os, " ")); 
    return os;
}

void print_help(po::options_description & desc,char * program)
{
    cout << "Usage: "<<program<<" [options]\n";
    cout << desc;
}

int main(int ac, char* av[])
{
    try {
        unsigned long long mac;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("mac,m", po::value<unsigned long long>(&mac), "mac address of the user ")
        ;

        po::positional_options_description p;
        p.add("mac",-1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
            options(desc).positional(p).run(), vm);
        po::notify(vm);


        if (vm.count("help")) {
            print_help(desc,av[0]);
            return 0;
        }

        if (vm.count("mac"))
        {
            cout << "user's mac is: " 
                 << mac << "\n";
        }
        else
        {
            print_help(desc,av[0]);
            return 0;
        }

        int user_id=user_get_id(mac);
        cout << "user's id is: "<< user_id<<endl;
    }
    catch(std::exception& e)
    {
        cout << e.what() << "\n";
        return 1;
    }    
    return 0;
}

