/*! 
 * @file probe.hpp
 * The header file containing structure declaration for reading probe choices 
 *
 * File is part of the Cubes project 
*/

#ifndef __PROBES_H_
#define __PROBES_H_ 1

#include <string>

enum AnalogProbe
{
    AnInvalid=-1,
    AnNone=0,
    PALG,
    PAHG,
    SSHLG,
    SSHHG,
    FSH
};

enum DigitalProbe
{
    DiInvalid=-1,
    DiNone=0,
    LGPD,
    HGPD
};

enum DacProbe
{
    DcInvalid =-1,
    DcNone=0,
    Enable
};
   
class AsicProbe
{
    public :
        AsicProbe();
        ~AsicProbe();
        std::string bitstream();
        void PrintChoices();
        void setAnalog(AnalogProbe ap, int chn);
        void setDigital(DigitalProbe dp, int chn);
        void setDAC(int chn);
        bool hasInvalid();

    private :
        AnalogProbe aval;
        DigitalProbe dval;
        DacProbe dcval;
        int achan;
        int dchan;
        int dacchan;

};


/*! 
 * Dummy class with loads of cout and cin
 * Can be replaced with button Qt classes later
 */ 
class Menu
{
    public :
        Menu();
        ~Menu();
        void displayMenu();
        void displayChannel();
        void displayAnalog();
        void displayDigital();
        void displayWriteMenu(AsicProbe& ch);
        int fetchpmenu();
        AnalogProbe fetchAnalogMenu(int cmdline=-13);
        DigitalProbe fetchDigitalMenu(int cmdline=-13);
        int fetchChannel(int cmdline=-13);
        int fetchWriteMenu();
        AsicProbe parseCmdLine(int argc, char** argv);
};

#endif
/* --------------------------------------------- End of File ----------------------------------*/
