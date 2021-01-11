/*! 
 * @file Cubes-Reader.hh
 * The header file containing structure declaration for reading Cubes binary into 
 * a local data vector and return it as ROOT / ASCII histogram files
 *
 * File is part of the Cubes project 
*/

#ifndef __CUBES_RDR_H_
#define __CUBES_RDR_H_

#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <TH1F.h>

#include "Error.hh"

enum gchain_e
{
    LG,
    HG
};

struct data_s 
{
    unsigned int nbytes;
    const char* fmt;
};

/*! The main Class for reading Cubes generated binary files. It consists of
 * useful functions to get histograms and headers from the binary file and write
 * them to ROOT files. A useful feature is the 'Append' function, which can
 * append contents of multiple binary files to the same ROOT file.
 *
 * A typical usage is shown :
 * 
 * CubesReader cc;
 * cc.SetWriteFile("Output.root"); // will write the headers and histograms to this file
 * // Note that if SetWriteFile is not specified, will write to Cubes.root instead 
 * cc.ReadFile("Name_of_binary_file");
 *
 * Other useful utilities include a function to change/set the binary file format by 
 * reading from a tab separated formatted ASCII file and functions to Get annd Draw individual 
 * MPPC histograms for inspection.
 *
 */ 
class CubesReader
{
    public :
        CubesReader();
        ~CubesReader();

        int ReadFile(const char* filename);
        int SetWriteFile(const char* filename);
        int Append(const char* filename);
        int SetBinaryDataFormat(const char* filename);
        TH1F* GetHist(int mppc, gchain_e amplr);
        int Draw(int mppc,gchain_e amplr,Option_t *option="");

    private :
        int readfile(TString filename);
        unsigned int intval(char* buff, unsigned int nbytes, unsigned int offs=0);
        TString asciival(char*buff,unsigned  int nbytes);
        void printraw(char* buff,unsigned  int nbytes);
        void SetBinaryDataFormat();
        TFile* rootfl;
        TString rootflname;
        unsigned int num_mppc;
        unsigned int totbytes; 
        std::map<unsigned int,data_s> datastruct;
};
        


#endif
/* --------------------------------------------- End of File ----------------------------------*/
