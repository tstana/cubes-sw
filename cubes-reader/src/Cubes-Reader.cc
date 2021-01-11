/*! 
 * @file Cubes-Reader.cc
 * File containing structure definition for reading Cubes binary into 
 * a local data vector and return it as ROOT / ASCII histogram files
 *
 * File is part of the Cubes project 
*/
#include "Cubes-Reader.hh"

#include <ios> // remove
#include <iomanip>
#include <cstdio> // rename
#include <fstream>
#include <cmath> // pow
#include <cstring> // memcpy

#include <TRandom3.h>
#include <TFileMerger.h>

#ifndef MAXBITS
#define MAXBITS 4
#endif

CubesReader::CubesReader() :
    rootfl(0),rootflname("Cubes.root")
{
    SetBinaryDataFormat();
}


CubesReader::~CubesReader()
{
    if (rootfl)
        if(rootfl->IsOpen())
        {
            rootfl->Close();
            rootfl->Delete();
        }
}


void CubesReader::SetBinaryDataFormat()
{
    datastruct.insert(										// comment this line if the file obtained from SD card as the files on SD card will not have unixtime
      std::pair<unsigned int,data_s>(1,{23,"UNIXTIME"}));  // comment this line if the file obtained from SD card 
    datastruct.insert(
      std::pair<unsigned int,data_s>(2,{2,"BOARDID"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(3,{4,"TIME"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(4,{2,"TEMP_CITI_START"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(5,{2,"TEMP_HVPS_START"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(6,{2,"HVPS_VOLT_START"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(7,{2,"HVPS_CURR_START"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(8,{114,"RESERVED"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(9,{2,"DAQ_TIME"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(10,{2,"ACT_DAQ_TIME"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(11,{4,"TRIG_COUNT_CH0"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(12,{4,"TRIG_COUNT_CH16"}));    
    datastruct.insert(
      std::pair<unsigned int,data_s>(13,{4,"TRIG_COUNT_CH31"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(14,{4,"TRIG_COUNT_OR32"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(15,{2,"TEMP_CITI_END"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(16,{2,"TEMP_HVPS_END"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(17,{2,"HVPS_VOLT_END"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(18,{2,"HVPS_CURR_END"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(19,{98,"RESERVED"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(20,{2,"NUMBINS"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(21,{4096,"HIST_MPPC3_HG"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(22,{4096,"HIST_MPPC3_LG"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(23,{4096,"HIST_MPPC2_HG"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(24,{4096,"HIST_MPPC2_LG"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(25,{4096,"HIST_MPPC1_HG"}));
    datastruct.insert(
      std::pair<unsigned int,data_s>(26,{4096,"HIST_MPPC1_LG"}));

    num_mppc=3;
    // totbytes=24832; // use this for SD card file
    totbytes=25000; // comment this line when SD card file is used
}

inline void CubesReader::printraw(char* buff,unsigned int nbytes)
{
    if(DBGLEV >= 3)
    {
        std::cout << "Buffer contents are " << std::endl;
        for(int i=0;i<nbytes;i++)
        {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (buff[i]&0xff);
        }
        std::cout << std::endl;
    }
    return;
}

unsigned int CubesReader::intval(char *buff,unsigned  int nbytes, unsigned int offs)
{
    unsigned int ans=0;
    int min = offs;
    int max = nbytes + offs;
    for(int ii=min;ii<max;ii++)
    {
        ans += pow(16,((max-1-ii)*2))*(buff[ii]&0xff); // big endian
    }
    return ans;
}
   
TString CubesReader::asciival(char *buff,unsigned  int nbytes)
{
    TString ans;
    for(int ir=0;ir<nbytes;ir++)
    {
        ans += char(buff[ir]&0xff);
    }
    return ans;
}

int CubesReader::SetWriteFile(const char* filename)
{
    rootflname = filename;
    return SUCCESS;
}

int CubesReader::ReadFile(const char* filename)
{
    TString fln = filename;
    if (rootfl)
        if (rootfl->IsOpen())
        {
            DBGMSG(2,"## WARNING ## : Resetting histograms and saving freshly to " << rootflname);
            rootfl->Close();
            rootfl = 0;
        }
    return readfile(fln);
}

TH1F* CubesReader::GetHist(int mppc, gchain_e amplr)
{
    rootfl = new TFile(rootflname);
    if (not rootfl->IsOpen())
    {
        DBGMSG(1,"## ERROR ## : Could not access histogram");
        return 0;
    }

    TString ampl;
    if (amplr == LG) ampl = "LG";
    else if (amplr == HG) ampl = "HG";

    TH1F* hist = (TH1F*)rootfl->Get(Form("HIST_MPPC%i_%s",mppc,ampl.Data()));
    if (not hist)
    {
        DBGMSG(1,"## ERROR ## : Could not access histogram");
        return hist;
    }

    hist->SetDirectory(0); // detach histogram from file to preven crashing on close
    rootfl->Close();

    return hist;
}

int CubesReader::Draw (int mppc, gchain_e amplr, Option_t *option)
{
    TH1F* hist = GetHist(mppc,amplr);
    if (not hist)
    {
        DBGMSG(1,"## ERROR : Could not retrieve histogram");
        return FAILURE;
    }
    hist->Draw(option);

    return SUCCESS;
}


int CubesReader::Append(const char* filename)
{
    TString fln = filename;
    bool closefl=false;
    if (not rootfl)
    {
        closefl=true;
    }
    else if(not rootfl->IsOpen())
    {
        closefl = true;
    }

    if (closefl)
    {
        rootfl = new TFile(rootflname);
        if (not rootfl->IsOpen())
        {
            DBGMSG(2,"## WARNING ## : Cannot find file to append to. Creating a new root file");
        }
    }

    return readfile(fln);
}

int CubesReader::readfile(TString filename)
{
    if (datastruct.empty())
    {
        DBGMSG(1,"## ERROR ## : Reading data format not set !");
        return FAILURE;
    }
 
    //Open file to read
    std::ifstream fl(filename.Data(),std::ios::in | std::ios::binary);
    if (not fl.is_open())
    {
        DBGMSG(1,"## ERROR ## : Could not open file " << filename);
        return FAILURE;
    }
    fl.seekg(0,std::ios::end);
    int ln = fl.tellg();
    fl.seekg(0,std::ios::beg);
    if( ln != totbytes)
    {
        if (ln < totbytes)
        {
            DBGMSG(1,"## ERROR ## : Expected " << totbytes << 
                    " bytes, but file " << filename << 
                    " has " << ln << " bytes.");
            fl.close();
            return FAILURE;
        }
        DBGMSG(2,"## WARNING ## : Expected " << totbytes << " bytes, but file " <<
                    filename << " has " << ln << " bytes. Reading only the first "
                    << totbytes << " bytes.");
    }


    TH1F* histset[2*num_mppc]={0};
    //Open file to write
    bool newfl = true;
    if (rootfl)
        if (rootfl->IsOpen())
            newfl = false;

    if (newfl)
        rootfl = new TFile(rootflname,"RECREATE");
    else
    {
        rootfl->Close();
        TRandom3 rn;
        rootfl = new TFile(Form("tmpfile_%i.root",(int)(100*rn.Rndm())),"RECREATE");
    }

    if (not rootfl->IsOpen())
    {
        DBGMSG(1,"## ERROR ## : Could not open output ROOT file " << rootflname);
        fl.close();
        return FAILURE;
    }
    TTree *info = new TTree("Header","Auxillary information");
 
    TString boardid; // I need the instances outside for TTree->Fill to work
    unsigned int unixtime, time,daq_time,act_daq_time,numbins=0;
    float temp_hvps_start, temp_hvps_end, temp_citi_start, temp_citi_end, 
            hvps_volt_start, hvps_volt_end, hvps_curr_start, hvps_curr_end,
            trig_count_ch0, trig_count_ch16, trig_count_ch31, trig_count_or32;

    // Begin reading
    for(auto itr = datastruct.begin(); itr != datastruct.end(); itr++)
    {
        unsigned int nbytes = (itr->second).nbytes;
        std::string fmt = (itr->second).fmt;
        char buffer[nbytes];
        if (! fl.read(buffer,nbytes) )
        {
            DBGMSG(1," ## ERROR ## : Corrupted read ! Could not read " << (itr->second).fmt << " Exiting.");
            fl.close();
            return FAILURE;
        }
        DBGMSG(3,"## READING " << nbytes << " bytes for " << fmt);
        printraw(buffer, nbytes);
        if(fmt == "UNIXTIME")
        {
            unixtime = intval(buffer, nbytes); 
            DBGMSG(3," ## STATUS ## : unix time = " << unixtime);
            info->Branch("UNIXTIME", &unixtime,"UNIXTIME/i");
        }
        else if(fmt == "BOARDID")
        {
            boardid = asciival(buffer, nbytes); 
            DBGMSG(3," ## STATUS ## : Histoid = " << boardid);
            info->Branch("BOARDID",(void *)boardid.Data(),"BOARDID/C");
        }
        else if(fmt == "TIME")
        {
            time = intval(buffer, nbytes);
            DBGMSG(3," ## STATUS ## : time = " << time);
            info->Branch("TIME",&time,"TIME/i");
        }
        else if(fmt == "TEMP_CITI_START")
        {
            temp_citi_start = intval(buffer, nbytes);
            DBGMSG(3," ## STATUS ## : citiroc temp = " << temp_citi_start);
            info->Branch("TEMP_CITI_START",&temp_citi_start);
        }
        else if(fmt == "TEMP_HVPS_START")
        {
            unsigned int val = intval(buffer, nbytes);
            temp_hvps_start = (val*1.907e-5 - 1.035)/(-5.5e-3);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << temp_hvps_start);
            info->Branch("TEMP_HVPS_START",&temp_hvps_start);
        }

        else if(fmt == "HVPS_VOLT_START")
        {
            unsigned int val = intval(buffer, nbytes);
            hvps_volt_start = (val*1.812e-3); 
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << hvps_volt_start);
            info->Branch("HVPS_VOLT_START",&hvps_volt_start);
        }

        else if(fmt == "HVPS_CURR_START")
        {
            unsigned int val = intval(buffer, nbytes);
            hvps_curr_start = (val*5.194e-3);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << hvps_curr_start);
            info->Branch("HVPS_CURR_START",&hvps_curr_start);
        }
        else if(fmt == "RESERVED")
        {
            // do nothing !
            DBGMSG(3," ## STATUS ## : " << fmt << ". Doing nothing ! ");
        }
        else if(fmt =="DAQ_TIME")
        {
        	unsigned int val = intval(buffer, nbytes);
            daq_time = (val*1/256);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << daq_time);
            info->Branch("DAQ_TIME",&daq_time);
        }
        else if(fmt == "ACT_DAQ_TIME")
        {
        	unsigned int val = intval(buffer, nbytes);
            act_daq_time = (val*1/256); 
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << act_daq_time);
            info->Branch("ACT_DAQ_TIME",&act_daq_time);
        }
        else if(fmt == "TRIG_COUNT_CH0")
        {
            trig_count_ch0 = intval(buffer, nbytes);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << trig_count_ch0);
            info->Branch("TRIG_COUNT_CH0",&trig_count_ch0);
        }
        else if(fmt == "TRIG_COUNT_CH16")
        {
            trig_count_ch16 = intval(buffer, nbytes);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << trig_count_ch16);
            info->Branch("TRIG_COUNT_CH16",&trig_count_ch16);
        }
        else if(fmt == "TRIG_COUNT_CH31")
        {
            trig_count_ch31 = intval(buffer, nbytes);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << trig_count_ch31);
            info->Branch("TRIG_COUNT_CH31",&trig_count_ch31);
        }
        else if(fmt == "TRIG_COUNT_OR32")
        {
            trig_count_or32 = intval(buffer, nbytes);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << trig_count_or32);
            info->Branch("TRIG_COUNT_OR32",&trig_count_or32);
        }    
        else if(fmt == "TEMP_CITI_END")
        {
            temp_citi_end = intval(buffer, nbytes);
            DBGMSG(3," ## STATUS ## : citiroc temp end = " << temp_citi_end);
            info->Branch("TEMP_CITI_END",&temp_citi_end);
        }
        else if(fmt == "TEMP_HVPS_END")
        {
            unsigned int val = intval(buffer, nbytes);
            temp_hvps_end = (val*1.907e-5 - 1.035)/(-5.5e-3);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << temp_hvps_end);
            info->Branch("TEMP_HVPS_END",&temp_hvps_end);
        }

        else if(fmt == "HVPS_VOLT_END")
        {
            unsigned int val = intval(buffer, nbytes);
            hvps_volt_end = (val*1.812e-3);
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << hvps_volt_end);
            info->Branch("HVPS_VOLT_END",&hvps_volt_end);
        }

        else if(fmt == "HVPS_CURR_END")
        {
            unsigned int val = intval(buffer, nbytes);
            hvps_curr_end = (val*5.194e-3); 
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << hvps_curr_end);
            info->Branch("HVPS_CURR_END",&hvps_curr_end);
        }   
        else if(fmt == "RESERVED")
        {
            // do nothing !
            DBGMSG(3," ## STATUS ## : " << fmt << ". Doing nothing ! ");
        }
        else if(fmt == "NUMBINS")
        {
            numbins = intval(buffer, nbytes); 
            DBGMSG(3," ## STATUS ## : " << fmt << " = " << numbins);
            numbins=2048; //overriding for debug purposes. TODO remove later.
            info->Branch("NUMBINS",&numbins,"NUMBINS/i");

        }
        else if(fmt.find("HIST_MPPC") != std::string::npos)
        {
            int i = std::stoi(fmt.substr(fmt.find("MPPC")+4,1)); // mppc number
            int offs = (fmt.find("LG") != std::string::npos); // 0 or 1
            if (numbins == 0)
            {
                DBGMSG(2,"## WARNING ## : NUMBINS not found in header, setting to 2048 for the histogram");
                numbins = 2048;
            }
            histset[2*(i-1) + offs] = new TH1F(fmt.data(),fmt.data(),numbins,0,numbins-1);
            unsigned int nbytesperbin = nbytes/numbins;
            int tothits=0;
            for (unsigned int j=0;j<numbins;j++)
            {
                unsigned int hits = intval(buffer, nbytesperbin, j*nbytesperbin);
                (histset[2*(i-1) + offs])->SetBinContent(j+1,hits); // histogram fill
                // std::ios init(NULL);
                // init.copyfmt(std::cout);
                // if ((j > 729) && (j < 739) && (fmt.find("HIST_MPPC2_HG") != std::string::npos))
                // {
                    
                //     std::cout << "Value at " << j+1 << " = " << hits << " hx = " 
                //     << std::hex << std::setw(2) << std::setfill('0') << (unsigned int)(unsigned char)buffer[j*nbytesperbin]
                //     << " " << std::hex << std::setw(2) << (unsigned int)(unsigned char)buffer[j*nbytesperbin + 1] << std::endl;
                //     std::cout.copyfmt(init);

                // }
                tothits += hits;
            }
            DBGMSG(3," ## STATUS ## : " << fmt << ". For i=" << i << 
                    ", offs = " << offs << ", Nbytes =  " 
                    << nbytes << ", nbytesperbin = " << nbytesperbin
                    << ", tothits = " << tothits << ", numbins = " << numbins);
            (histset[2*(i-1) + offs])->SetEntries(tothits);
            (histset[2*(i-1) + offs])->Sumw2(); // for errors
            //(histset[2*(i-1) + offs])->ComputeIntegral(); // for getrandom if used !
               
        }
    }// for loop read

    DBGMSG(3," ## STATUS ## : File read done. Filling root file !") ;
    info->Branch("NUM_MPPC",&num_mppc,"NUM_MPPC/i");
    info->Fill();

    //Write to rootfl
    info->Write();
    for(int hi=0;hi<num_mppc;hi++)
    {
        if(histset[2*hi])
            (histset[2*hi])->Write();
        else
            DBGMSG(2,"## WARNING ## : Histogram " << 2*hi << " not found in data !"); 
        if(histset[2*hi+1])
            (histset[2*hi+1])->Write();
        else
            DBGMSG(2,"## WARNING ## : Histogram " << 2*hi+1 << " not found in data !"); 
    }
 

    rootfl->Flush();
    rootfl->Close();
    fl.close();

    if (not newfl)
    {
        TString tmpname = rootfl->GetName();
        TFileMerger mg;
        mg.AddFile(rootflname);
        mg.AddFile(tmpname);
        mg.OutputFile("MergeFile.root");
        if(not mg.Merge())
        {
            DBGMSG(1,"## ERROR ## : Could not append file ");
            return FAILURE;
        }
        if((std::remove(tmpname) != 0) || (std::remove(rootflname) != 0))
        {
            DBGMSG(2,"## WARNING ## : Could not rename file. Appended file is MergeFile.root");
            return FAILURE;
        }
        std::rename("MergeFile.root",rootflname);
    }


    return SUCCESS;
}

int CubesReader::SetBinaryDataFormat(const char* filename)
{
    std::ifstream fl(filename);

    if (not fl.is_open())
    {
        DBGMSG(1,"## ERROR ## : Could not open file " << filename);
        return FAILURE;
    }

    int i=1,sumbytes=0,mcount=0;
    unsigned int numbytes;
    std::string type; 
    datastruct.clear();
    while (true)
    {
        if (fl.eof()) break;

        fl >> numbytes;
        fl >> type;
        sumbytes += numbytes;
        datastruct.insert(
            std::pair<unsigned int,data_s>(i,{numbytes,type.data()}));
        i++;
        if (type.find("MPPC") != std::string::npos)
            mcount++;
    }
    totbytes = sumbytes;
    num_mppc = mcount/2;

    return SUCCESS;
}









/* --------------------------------------------- End of File ----------------------------------*/
