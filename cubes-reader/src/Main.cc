#include "Cubes-Reader.hh"
#include <iostream>


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Please specify file to read !" << std::endl;
        std::cerr << "Usage : " << argv[0] << " <filename>" << std::endl;
        return 0;
    }

    const char* fname = argv[1];
    const char* fname2 = argv[2];
    //char read[100];


    CubesReader cc;
    if (argc >= 3)
        cc.SetWriteFile(argv[2]);
    else
        cc.SetWriteFile("Outhist.root");
    cc.ReadFile(fname);
    //cc.Draw(3,HG,"L");
    //cc.Append(fname2);

    //std::cout << "Press return to continue ...";
    //std::cin.get();
    
    return 0;
}
