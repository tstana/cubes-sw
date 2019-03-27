#include "probe.hpp"
#include <iostream>
#include <map>
#include <algorithm> // for std::reverse

AsicProbe::AsicProbe()
    : achan(0),dchan(0),dacchan(0)
{
    aval=AnNone;
    dval=DiNone;
    dcval=DcNone;
}

AsicProbe::~AsicProbe()
{}

void AsicProbe::setAnalog(AnalogProbe ap, int chn)
{
    aval = ap;
    achan = chn;
}

void AsicProbe::setDigital(DigitalProbe dp, int chn)
{
    dval = dp;
    dchan = chn;
}

void AsicProbe::setDAC(int chn)
{
    dacchan = chn;
    dcval = Enable;
}

void AsicProbe::PrintChoices()
{
    static const std::map<AnalogProbe,std::string> apstr{
        {AnNone,"No Analogue Probe"},
        {PALG,"Low gain preamplifier output"},
        {PAHG,"High gain preamplifier output"},
        {SSHLG,"Low gain slow shaper output"},
        {SSHHG,"High gain slow shaper output"},
        {FSH,"Fast shaper output"},
        {AnInvalid,"INVALID"}
    };
    static const std::map<DigitalProbe,std::string> dpstr{
        {DiNone,"No Analogue Probe"},
        {LGPD,"Low gain peak detector mode"},
        {HGPD,"High gain peak detector mode"},
        {DiInvalid,"INVALID"}
    };
    static const std::map<DacProbe,std::string> dcstr{
        {DcInvalid,"INVALID"},
        {DcNone,"disabled"},
        {Enable,"enabled"}
    };

    std::cout << "Choices are \n";
    std::cout << "------------------------------------\n";
    auto iter = apstr.find(aval);
    if (aval == AnNone)
        std::cout << "\nAnalogue Probe Out is : " << iter->second;
    else
        std::cout << "\nAnalogue Probe Out is : " << iter->second << " for chn " << achan+1;
    auto iter2 = dpstr.find(dval);
    if (dval == DiNone)
        std::cout << "\nDigital Probe Out is : " << iter2->second;
    else
        std::cout << "\nDigital Probe Out is : " << iter2->second << " for chn " << dchan+1;
    auto iter3 = dcstr.find(dcval);
    if (dcval == Enable)
        std::cout << "\nDAC Out is " << iter3->second << " for chn " << dacchan+1;
    else
        std::cout << "\nDAC Out is " << iter3->second;

    std::cout << "\n\n----------------------------------\n";
}

std::string AsicProbe::bitstream()
{
    std::string out(256,'0');
    static const std::map<AnalogProbe,int> aoff{
        {FSH,0},
        {SSHLG,32},
        {SSHHG,96},
        {PAHG,160},
        {PALG,160}
    };
    static const std::map<DigitalProbe,int> doff{
        {LGPD,64},
        {HGPD,128}
    };
    static const std::map<DacProbe,int> dcoff{
        {Enable,224}
    };

    if (aval != AnNone)
    {
        auto itr = aoff.find(aval);
        int offs = itr->second + achan;
        if (aval == PALG)
            offs += achan + 1;
        if (aval == PAHG)
            offs += achan;
        out[offs] = '1';
    }
    if (dval != DiNone)
    {
        auto itr = doff.find(dval);
        int offs = itr->second + dchan;
        out[offs] = '1';
    }
    if (dcval != DcNone)
    {
        auto itr = dcoff.find(dcval);
        int offs = itr->second + dacchan;
        out[offs] = '1';
    }

    //std::reverse(out.begin(),out.end()); // reverse for endianness
    return out;
}

bool AsicProbe::hasInvalid()
{
    bool inv = false;

    inv = (aval == AnInvalid) || ( dval == DiInvalid) || (dcval == DcInvalid) 
        || (achan == -1) || (dchan == -1) || (dacchan == -1) ;

    return inv;
}
