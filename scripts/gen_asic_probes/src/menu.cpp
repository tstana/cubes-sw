#include <iostream>
#include <string>
#include "probe.hpp"

Menu::Menu()
{}

Menu::~Menu()
{}

void Menu::displayMenu()
{
    std::cout << "\n";
    std::cout << "\n------- Probes Menu -------------------";
    std::cout << "\n1. Enter Analogue Probe choice";
    std::cout << "\n2. Enter Digital Probe choice";
    std::cout << "\n3. Enter DAC choice";
    std::cout << "\n4. Write to File";
    std::cout << "\n---------------------------------------\n";
    std::cout << "    Your choice : ";
    return ;
}

void Menu::displayChannel()
{
    std::cout << "\n---------------------------------------\n";
    std::cout << "    Enter Channel (1-32) : ";
}

void Menu::displayAnalog()
{
    std::cout << "\n";
    std::cout << "\n------- Analogue Probe choices --------";
    std::cout << "\n1. No Analogue Probe";
    std::cout << "\n2. Low gain preamplifier output";
    std::cout << "\n3. High gain preamplifier output";
    std::cout << "\n4. Low gain slow shaper output";
    std::cout << "\n5. High gain slow shaper output";
    std::cout << "\n6. Fast shaper output";
    std::cout << "\n---------------------------------------\n";
    std::cout << "    Your choice : ";
}

void Menu::displayDigital()
{
    std::cout << "\n";
    std::cout << "\n------- Digital Probe choices --------";
    std::cout << "\n1. No Digital Probe";
    std::cout << "\n2. Low gain peak detector mode";
    std::cout << "\n3. High gain peak detector mode";
    std::cout << "\n--------------------------------------\n";
    std::cout << "    Your choice : ";
}

void Menu::displayWriteMenu(AsicProbe& ch)
{
    std::cout << "\n";
    std::cout << "\n------- Write Menu --------------------\n";
    ch.PrintChoices();
    std::cout << "\n1. If this is okay, write to probe.cfg";
    std::cout << "\n2. If this is not okay, do config again\n";
    std::cout << "\n----------------------------------------\n";
    std::cout << "    Your choice : ";
}



int Menu::fetchpmenu()
{
    int pmenu;
    std::cin >> pmenu; 
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cout << "\n Incorrect input !" << std::endl;
        return -1;
    }
    if ((pmenu < 1) || (pmenu > 4))
    {
        std::cin.clear();
        std::cout << "\n Incorrect input !" << std::endl;
        return -1;
    }

    return pmenu;
}

AnalogProbe Menu::fetchAnalogMenu(int cmd)
{
    int ap;
    AnalogProbe ans;
    if (cmd ==-13)
    {
        std::cin >> ap;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cout << "\n Incorrect input !" << std::endl;
            return AnInvalid;
        }
    }
    else
    {
        ap = cmd;
    }
    ap = ap - 1;
    if ((ap < AnNone) || (ap > FSH))
    {
        std::cin.clear();
        std::cout << "\n Incorrect input !" << std::endl;
        return AnInvalid;
    }

    ans = (AnalogProbe)ap;
    return ans;
}

DigitalProbe Menu::fetchDigitalMenu(int cmd)
{
    int ap;
    DigitalProbe ans;
    if (cmd ==-13)
    {
        std::cin >> ap;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cout << "\n Incorrect input !" << std::endl;
            return DiInvalid;
        }
    }
    else
    {
        ap = cmd;
    }
    ap = ap - 1; // to account for enum starting from 0!
    if ((ap < DiNone) || (ap > HGPD))
    {
        std::cin.clear();
        std::cout << "\n Incorrect input !" << std::endl;
        return DiInvalid;
    }

    ans = (DigitalProbe)ap;
    return ans;
}

int Menu::fetchChannel(int cmd)
{
    int ch;
    if (cmd ==-13)
    {
        std::cin >> ch;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cout << "\n Incorrect input !" << std::endl;
            return -1;
        }
    }
    else
    {
        ch = cmd;
    }
    ch = ch - 1;    
    if ((ch < 0) || (ch > 31))
    {
        std::cin.clear();
        std::cout << "\n Incorrect input !" << std::endl;
        return -1;
    }

    return ch;
}

int Menu::fetchWriteMenu()
{
    int ch;
    std::cin >> ch;
    if (std::cin.fail())
    {
        std::cin.clear();
        std::cout << "\n Incorrect input !" << std::endl;
        return -1;
    }
    if ((ch < 1) || (ch > 2))
    {
        std::cin.clear();
        std::cout << "\n Incorrect input !" << std::endl;
        return -1;
    }

    return ch;
}


AsicProbe Menu::parseCmdLine(int argc, char ** argv)
{
    // Not implemented yet;
    AsicProbe ch;
    AnalogProbe ap=AnNone;
    DigitalProbe dp=DiNone;
    int achn=0,dchn=0,dachn=0;

    
    for(int i=1; i < argc; i++) // 0 is filename
    {
        std::string val = argv[i];
        if (val == "-a")
        {
            i++;
            ap = fetchAnalogMenu(std::stoi(argv[i]));
        }
        if (val == "-d")
        {
            i++;
            dp = fetchDigitalMenu(std::stoi(argv[i]));
        }
        if (val == "--ac")
        {
            i++;
            achn = fetchChannel(std::stoi(argv[i]));
        }
        if (val == "--dc")
        {
            i++;
            dchn = fetchChannel(std::stoi(argv[i]));
        }
        if (val == "--dac")
        {
            i++;
            dachn = fetchChannel(std::stoi(argv[i]));
            ch.setDAC(dachn);
        }
        if (val == "-h")
        {
            std::cout << "Usage : " << argv[0] << " <options>\n";
            std::cout << "\n If <options> are not specified, an interactive menu is displayed to choose the options\n";
            std::cout << "\n The <options> are (marked by a **): \n";
            std::cout << "** : -a <AnalogProbe> ";
            displayAnalog();
            std::cout << "\n\n** : -d <DigitalProbe> \n";
            displayDigital();
            std::cout << "\n\n** : --ac <AnalogChannel> (1 to 32)\n";
            std::cout << "** : --dc <DigitalChannel> (1 to 32)\n";
            std::cout << "** : --dac <DACChannel> (1 to 32)\n";
            std::cout << "\n--------------------------------------\n";
            ap = AnInvalid;
        }


    }
    if (ap != AnNone)
        ch.setAnalog(ap,achn);
    if (dp != DiNone)
        ch.setDigital(dp,dchn);
    

    return ch;
}
