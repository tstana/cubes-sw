#include <iostream>
#include <fstream>
#include "probe.hpp"


int main(int argc, char *argv[])
{
    AsicProbe ch;
    AnalogProbe ap; 
    DigitalProbe dp;
    int chn=0;
    Menu mn;

    if (argc < 2) // interactive mode on couts
    {
        bool loop=true;
        int pmenu;
        while (loop)
        {
            mn.displayMenu();
            pmenu = mn.fetchpmenu();
            switch (pmenu)
            {
                case 1:
                    mn.displayAnalog();
                    if( (ap = mn.fetchAnalogMenu()) == AnInvalid)
                        return -1;
                    if (ap != AnNone)
                    {
                        mn.displayChannel();
                        if( (chn = mn.fetchChannel()) == -1)
                            return -1;
                        ch.setAnalog(ap,chn);
                    }

                    break;
                case 2:
                    mn.displayDigital();
                    if( (dp = mn.fetchDigitalMenu()) == DiInvalid)
                        return -1;
                    if (dp != DiNone)
                    {
                        mn.displayChannel();
                        if( (chn = mn.fetchChannel()) == -1)
                            return -1;
                        ch.setDigital(dp,chn);
                    } 
                    break;

                case 3:
                    mn.displayChannel();
                    if( (chn = mn.fetchChannel()) == -1)
                        return -1;
                    ch.setDAC(chn);

                    break;

                case 4:
                    mn.displayWriteMenu(ch);
                    if( (chn = mn.fetchWriteMenu()) == -1)
                        return -1;
                    if (chn == 1)
                        loop = false;

                    break;

                default :
                    return -1;
            } // switch case
        } // while (loop)
    } // interactive mode

    else // batch mode based on flags
    {
        ch = mn.parseCmdLine(argc,argv);
    }

    if ( not ch.hasInvalid())
    {
        std::ofstream opfl;
        opfl.open("Probe.conf");
        if (not opfl.is_open())
        {
            std::cout << "Could not open file Probe.conf to write config" << std::endl;
            return -1;
        }
        opfl << ch.bitstream();
        opfl.close();
    }
    return 0;
}





        
    
