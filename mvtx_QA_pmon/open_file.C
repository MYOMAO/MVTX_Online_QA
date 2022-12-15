#if !defined(__CINT__) || defined(__MAKECINT__) || defined(__CLING__)

#include "mvtxOnlineQA.h"
#include <fstream>
#include <string>
#include "ctype.h"
R__LOAD_LIBRARY(libmvtxonline2019)
#include <sstream>
#include <string>
#endif






//void open_file(int runnum, int nevents = 10001, const char* ftype = "longrun")
void open_file()
{
	
//	cout << "ISTARTNOW" << endl;
	

	gSystem->Load("libmvtxonline2019.so");
	set_refresh(100000);

	int nevents = -1; 
	
	//set_verbose(1);
	char filein[500];
	string infile;
	//int runnum=atoi(filename.Data());
	//sprintf(filein,"../beamtest2019/%s/%s_%08d-0000.prdf",ftype,ftype,runnum);
	//sprintf(filein,"/mnt/databkup_1/MVTX_testbeam2019/%s/%s_%08d-0000.prdf",ftype,ftype,runnum);
	//sprintf(filein,"/sphenix/data/data01/mvtx/%s/%s_%08d-0000.prdf",ftype,ftype,runnum);

	std::stringstream iss;

	std::string line;

	ifstream NewFile;

	NewFile.open("Diff.txt");

    NewFile.clear();
    NewFile.seekg(0);
	std::string runfilename;

	while (std::getline(NewFile, line)){
		string Run;

		NewFile >> filein;
		infile = filein;
//		runfilename = infile;
		cout << "Now Running QA on File " << filein << endl;

/*
		std::size_t const n = infile.find_first_of(filein);

		char const* digits = "123456789";
		if (n != std::string::npos)
		{
			std::size_t const m = infile.find_first_not_of(digits, n);
			Run.substr(n, m != std::string::npos ? m-n : m);
		}

*/

		for (int i = 0; i < infile.length(); i++)
		{	
		
			if (isdigit (infile[i]))
			{

				if(infile[i] > '0'){

					Run += (infile[i]);
					//cout << "infile[i] = " << infile[i] << "  Run = " << Run << endl;
				}
			
			}

		}

		
		//

		cout << "Which belongs to Run " << Run << endl;
		
		ofstream RunInfo("RunInfo.txt");

		RunInfo << Run << endl;

		ofstream FileInfo("FileInfo.txt");

	    std::string runfilename = infile.substr (8,100);     // "think"
		

	//	runfilename.substr(runfilename.find("/") + 1); 

		FileInfo << runfilename << endl;
		
		cout << "runfilename = " << runfilename << endl;

		//pinit();
		pfileopen(filein);
		prun(nevents);
		print_my_canvas();
		//

	}

	pstop();

	/*


	// sprintf(filein,"/sphenix/data/data01/mvtx/tb-1441-052019/longrun/longrun_00000957-0000.prdf");


*/


	//return;
}
