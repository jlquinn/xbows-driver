#include <fstream>
#include <iostream>

#include "driver_parse.hh"
#include "xbows.hh"

using namespace std;

int main(int ac, char* av[]) {
  ifstream ifs(av[1]);
  if (!ifs) {
    cerr << "Can't open file " << av[1] << endl;
    return 1;
  }

  init_xbows();
  
  read_config(ifs);

}
