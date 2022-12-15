// Updated by Xiaochun He on May 31, 2019 following Martin Purschke's
// suggestion correction
//
#include "mvtxOnlineQA.h"
#include "TPaveText.h"
#include "TEllipse.h"
#include <chrono>
#include <ctime> 


#include <TCanvas.h>
#include <TF1.h>
#include <TGaxis.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TLine.h>
#include <TStyle.h>
#include <TFile.h>
#include "TPaletteAxis.h"
#include <fstream>
#include <iostream>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define IDMVTXV1_MAXRUID       4
#define IDMVTXV1_MAXRUCHN      28

TFile * fout;

int init_done = 0;


unsigned int mvtx_refresh;

using namespace std;

const int NSTAVE = 4;
const bool chip_expected[4] = {true, true, true, true};
string stave_name[4] = {"E103", "C105", "C104", "A105"};

vector<TLine*> chip_edges, dead_chip_forward, dead_chip_backward;

string outHitLocations = "/home/maps/meeg/felix/daq/felix_rcdaq/online_monitoring/hitLocations/locations.txt";
ofstream write_outHitLocations(outHitLocations.c_str());

map<pair<int,int>,pair<int,int>> chipmap = {
	{{1,1}, {0,0}},
	{{1,2}, {0,1}},
	{{1,3}, {0,2}},
	{{1,4}, {0,3}},
	{{1,5}, {0,4}},
	{{1,6}, {0,5}},
	{{1,7}, {0,6}},
	{{1,8}, {0,7}},
	{{1,9}, {3,8}},
	{{1,10}, {3,7}},
	{{1,11}, {3,6}},
	{{1,12}, {3,5}},
	{{1,13}, {3,4}},
	{{1,14}, {3,3}},
	{{1,15}, {3,2}},
	{{1,16}, {3,1}},
	{{1,17}, {3,0}},
	{{1,18}, {2,8}},
	{{1,19}, {2,7}},
	{{1,20}, {2,6}},
	{{1,21}, {2,5}},
	{{1,22}, {2,4}},
	{{1,23}, {2,3}},
	{{1,24}, {2,2}},
	{{1,25}, {2,1}},
	{{1,26}, {2,0}},
	{{1,27}, {0,8}},
	{{2,1}, {1,2}},
	{{2,2}, {1,1}},
	{{2,3}, {1,0}},
	{{2,4}, {1,3}},
	{{2,5}, {1,4}},
	{{2,6}, {1,5}},
	{{2,7}, {1,6}},
	{{2,8}, {1,7}},
	{{2,9}, {1,8}}
}; //<ruid, ruchn> to <stave, chipID>


int InitFlag;

//My Plots

TH1D* EvtHitDis;
TH1D* EvtHitChip;
TH1D* ChipFiredHis;

TH2I* ChipStave;
TH2D* ChipStaveOcc;
TH1D* ChipStave1D;
TH1D * InfoCanvas;
TEllipse *bulb;
const int NRowMax = 512;
const int NColMax = 1024;
const int NBins = 30;

const int NCHIP = 9;


int mvtx_evnts = 0;

TH2D * HitMap[NSTAVE][NCHIP]; 


const int NMaps =NSTAVE *  NCHIP;


int NAllHits;
TCanvas *cMy; 
//TCanvas *cMyReverse; 


int HitPerChip[NSTAVE][NCHIP];
float OccPerChip[NSTAVE][NCHIP];

int NChipFired;

int mvtx_verbose;

int FileCounts;

int RunID;
int TotalFiles;
string RunIDString;
string FileNameProcess;
TString FNameString;

TPaveText * ptFileName;
TPaveText * ptNFile;
TPaveText * ptNEvent;
TPaveText * bulbGreen;
TPaveText * bulbRed;
TPaveText * bulbYellow;


// The following line caused some problem of running ROOT6 in macro
//TF1* fg = new TF1("fg", "gaus", 0, 1024);

TF1* fg;

//-- some constants for different chips
int chipColor[] = {kBlue, kRed, kGreen+2, kMagenta+2};
int chipMarker[] = {kFullCircle, kFullSquare, kFullDiamond, kFullCross};

// Show fit to beam center
TCanvas* cBeamCenter = nullptr;
const bool show_beam_fit = true;

unsigned short decode_row(int hit)
{
	return hit >> 16;
}

unsigned short decode_col(int hit)
{
	return hit & 0xffff;
}


//============================================================//

void set_verbose(int v)
{
	mvtx_verbose = v;
}

//============================================================//

void set_refresh(int r)
{
	mvtx_refresh = r;
}

//============================================================//

int pinit()
{
	/*
	   cout << "Now START" << endl;

	   RunInfo.open("RunInfo.txt");

	   RunInfo >> RunIDString;

	   cout << "Now Processing Run " << RunIDString << endl;

*/
	cout << "Now START" << endl;


	std::ifstream RunInfo("RunInfo.txt");
	RunInfo >> RunID;

	cout << "RunID = " << RunID << endl;

	cout << "Here" << endl;


	fout = new TFile(Form("SaveFiles/Run_%d.root",RunID),"RECREATE");
	fout->cd();
	
	std::ifstream FileInfo("FileInfo.txt");
	
	FileInfo >> FNameString;

//	FNameString = FileNameProcess;

	
	cout << "Pass 1 " << endl; 

	std::ifstream FileCount("FileCount.txt");

	FileCount >> TotalFiles;


	std::ofstream FileCount2("FileCount.txt");

	FileCount2 << TotalFiles;
	cout << "Pass 2 " << endl; 


	bulb = new TEllipse(0.2,0.75,0.30,0.20);



	ptFileName = new TPaveText(0.20,0.40,0.85,0.50,"NDC");
	ptFileName->SetTextSize(0.04);
	ptFileName->SetFillColor(0);
	ptFileName->SetTextAlign(12);
	ptFileName->AddText(Form("Current File Proccessing: %s",FNameString.Data()));

	ptNFile = new TPaveText(0.20,0.30,0.85,0.40,"NDC");
	ptNFile->SetTextSize(0.04);
	ptNFile->SetFillColor(0);
	ptNFile->SetTextAlign(12);
	ptNFile->AddText(Form("File Processed: %d",TotalFiles));


	ptNEvent = new TPaveText(0.20,0.20,0.85,0.30,"NDC");
	ptNEvent->SetTextSize(0.04);
	ptNEvent->SetFillColor(0);
	ptNEvent->SetTextAlign(12);
	ptNEvent->AddText("Event Processed: 0");


	bulbRed = new TPaveText(0.55,0.65,0.85,0.75,"NDC");
	bulbRed->SetTextSize(0.04);
	bulbRed->SetFillColor(0);
	bulbRed->SetTextAlign(12);
	bulbRed->SetTextColor(kRed);
	bulbRed->AddText("Red = QC Waiting");


	bulbYellow = new TPaveText(0.5,0.65,0.85,0.75,"NDC");
	bulbYellow->SetTextSize(0.04);
	bulbYellow->SetFillColor(0);
	bulbYellow->SetTextAlign(12);
	bulbYellow->SetTextColor(kYellow);
	bulbYellow->AddText("Yellow = QC Pausing");



	bulbGreen = new TPaveText(0.55,0.55,0.85,0.65,"NDC");
	bulbGreen->SetTextSize(0.04);
	bulbGreen->SetFillColor(0);
	bulbGreen->SetTextAlign(12);
	bulbGreen->SetTextColor(kGreen);
	bulbGreen->AddText("GREEN = QC Processing");

	InfoCanvas	= new TH1D("InfoCanvas","InfoCanvas",3,-0.5,2.5);
	InfoCanvas->SetTitle("QC Process Information Canvas");
	InfoCanvas->GetListOfFunctions()->Add(ptFileName);
	InfoCanvas->GetListOfFunctions()->Add(ptNFile);
	InfoCanvas->GetListOfFunctions()->Add(ptNEvent);
	InfoCanvas->GetListOfFunctions()->Add(bulb);
	InfoCanvas->GetListOfFunctions()->Add(bulbRed);	
	//	InfoCanvas->GetListOfFunctions()->Add(bulbYellow);
	InfoCanvas->GetListOfFunctions()->Add(bulbGreen);


	ChipStave = new TH2I("ChipStave","",NCHIP,-0.5,NCHIP-0.5,NSTAVE,-0.5,NSTAVE-0.5);
	ChipStave->GetXaxis()->SetTitle("Chip Number");
	ChipStave->GetYaxis()->SetTitle("Stave ID");
	ChipStave->GetXaxis()->CenterTitle();
	ChipStave->GetYaxis()->CenterTitle();
	ChipStave->GetYaxis()->SetTitleOffset(1.3);
	ChipStave->SetTitle(Form("Occupancy: Run %d Stave Number and Chip Number",RunID));



	ChipStaveOcc = new TH2D("ChipStaveOcc","",NCHIP,-0.5,NCHIP-0.5,NSTAVE,-0.5,NSTAVE-0.5);
	ChipStaveOcc->GetXaxis()->SetTitle("Chip Number");
	ChipStaveOcc->GetYaxis()->SetTitle("Stave ID");
	ChipStaveOcc->GetXaxis()->CenterTitle();
	ChipStaveOcc->GetYaxis()->CenterTitle();
	ChipStaveOcc->GetYaxis()->SetTitleOffset(1.3);
	ChipStaveOcc->SetTitle(Form("Average Occupancy: Run %d Stave Number and Chip Number",RunID));




	ChipStave1D = new TH1D("ChipStave1D","",NCHIP*NSTAVE,-0.5,NCHIP * NSTAVE-0.5);
	ChipStave1D->GetXaxis()->SetTitle("Chip Number + Stave ID * 9");
	ChipStave1D->GetYaxis()->SetTitle("Occupancy (%)");
	ChipStave1D->GetXaxis()->CenterTitle();
	ChipStave1D->GetYaxis()->CenterTitle();
	ChipStave1D->GetYaxis()->SetTitleOffset(1.3);
	ChipStave1D->SetTitle(Form("Average Occupancy: Run %d Stave Number and Chip Number",RunID));


	EvtHitDis = new TH1D("EvtHitDis","",25,-0.5,24.5);
	EvtHitDis->GetXaxis()->SetTitle("Number of Hits Per Event");
	EvtHitDis->GetYaxis()->SetTitle("Number of Events");
	EvtHitDis->GetXaxis()->CenterTitle();
	EvtHitDis->GetYaxis()->CenterTitle();
	EvtHitDis->GetYaxis()->SetTitleOffset(1.3);
	EvtHitDis->SetTitle(Form("Number of Hits Per Event Distribution: Run %d",RunID));


	EvtHitChip = new TH1D("EvtHitChip","",25,-0.5,24.5);
	EvtHitChip->GetXaxis()->SetTitle("Number of Hits Per Event Per Chip");
	EvtHitChip->GetYaxis()->SetTitle("Number of Events #times Chips");
	EvtHitChip->GetXaxis()->CenterTitle();
	EvtHitChip->GetYaxis()->CenterTitle();
	EvtHitChip->GetYaxis()->SetTitleOffset(1.3);
	EvtHitChip->SetTitle(Form("Number of Hits Per Event Per Chip Distribution: Run %d",RunID));


	ChipFiredHis = new TH1D("ChipFiredHis","",NCHIP*NSTAVE,-0.5,NCHIP*NSTAVE - 0.5);
	ChipFiredHis->GetXaxis()->SetTitle("Number of Chips Fired in Each Event");
	ChipFiredHis->GetYaxis()->SetTitle("Number of Events");
	ChipFiredHis->GetXaxis()->CenterTitle();
	ChipFiredHis->GetYaxis()->CenterTitle();
	ChipFiredHis->GetYaxis()->SetTitleOffset(1.3);
	ChipFiredHis->SetTitle(Form("Number of Chips Fired in Each Event Distribution: Run %d",RunID));


	//HitMap Initiation//



	for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){

			HitMap[i][j] = new TH2D(Form("HitMap_%d_%d",i,j),"",NBins,-0.5,NColMax+0.5,NBins,-0.5,NRowMax+0.5); 

			HitMap[i][j]->GetXaxis()->SetTitle("Col");
			HitMap[i][j]->GetYaxis()->SetTitle("Row");
			HitMap[i][j]->GetXaxis()->CenterTitle();
			HitMap[i][j]->GetYaxis()->CenterTitle();
			HitMap[i][j]->GetYaxis()->SetTitleOffset(1.4);
			HitMap[i][j]->SetTitle(Form("2D HitMap: Run %d Stave %d and Chip ID %d",RunID,i,j));


	

			//ReverseYAxis(HitMap[i][j]);
		}
	}






	bulb->SetFillColor(kGreen);

	cout << "DONE INIT" << endl;


	return 0;

}

//============================================================//

int process_event (Event * e)
{

	// Added by Xiaochu He following Martin's recommedation
	//cout << "START process" << endl;
	NAllHits = 0;


	for(int i = 0; i < NSTAVE; i++){

		for(int j = 0; j < NCHIP; j++){

			HitPerChip[i][j] = 0;

		}

	}


	Packet *p = e->getPacket(2000);
	if (p)
	{

		bool evnt_err = false;



		int npixels[NSTAVE] = {0};
		double mrow[NSTAVE] = {0};
		double mcol[NSTAVE] = {0};
		double mrow_refstave = -1;
		double mcol_refstave = -1;

		if ( !evnt_err ) {
			for (int ruid=0; ruid<IDMVTXV1_MAXRUID+1; ruid++)
			{
				if (p->iValue(ruid)!=-1)
				{
					for ( int ruchn = 0; ruchn < IDMVTXV1_MAXRUCHN+1; ruchn++)
					{
						if (p->iValue(ruid,ruchn)>0)
						{
							for (int i=0;i<p->iValue(ruid,ruchn);i++)
							{
								int hit = p->iValue(ruid,ruchn,i);
								int irow = decode_row(hit);
								int icol = decode_col(hit);
								//cout << "(ruid " << ruid << ", ruchn " << ruchn << ") ";
								//cout << "(row " << irow << ", col " << icol << ") ";
								if (chipmap.count({ruid,ruchn}) != 1) {
									cout << "invalid: (ruid " << ruid << ", ruchn " << ruchn << ") " << endl;
								} else {
									pair<int, int> chiplocation = chipmap[{ruid,ruchn}];
									int istave = chiplocation.first;
									int ichip = chiplocation.second;
									HitPerChip[istave][ichip] = HitPerChip[istave][ichip] + 1;

									HitMap[istave][ichip]->Fill(icol,irow);



									ChipStave->Fill(ichip,istave);
									NAllHits = NAllHits + 1;

								}
							}
							//cout << endl;
						}
					}
				}
			}

			for(int i = 0; i < NSTAVE; i++){

				for(int j = 0; j < NCHIP; j++){

					int Index = NCHIP * i + j;

					OccPerChip[i][j] = float(HitPerChip[i][j])/(float(NRowMax) * float(NColMax));

					ChipStaveOcc->Fill(j,i,OccPerChip[i][j]);
					ChipStave1D->Fill(Index,OccPerChip[i][j]);
				}
			}



			EvtHitDis->Fill(NAllHits);
			NChipFired = 0;

			for(int i = 0; i < NSTAVE; i++){

				for(int j = 0; j < NCHIP; j++){

					if(HitPerChip[i][j] > 0){
						EvtHitChip->Fill(HitPerChip[i][j]);
						NChipFired = NChipFired + 1;
					}


				}

			}

			ChipFiredHis->Fill(NChipFired);

		}



		//hnevnt->Fill(0);
		delete p;

	}

	//cout << "NAllHits = " << NAllHits << endl;


	if ( mvtx_refresh > 0 &&  mvtx_evnts%mvtx_refresh == 0 ) OM();
	if ( mvtx_evnts%100000 == 0 ) cout << " processing event " << mvtx_evnts << endl;

	mvtx_evnts++;
	return 0;
}




//============================================================//

int mask_pixels(float thresh)
{
}

//============================================================//



TPad * MyPad[3];

TPad * MyPad0;


TPad * MyPad2[NSTAVE][NCHIP];
TPad * MyPad3;
TPad * MyPad4[NSTAVE][NCHIP];

int Cycle = 0;


TCanvas * cOM0;
TCanvas * cOM2; 
TCanvas * cOM3; 
TCanvas * cOM4; 

TCanvas * cOM5; 
TCanvas * cOM6; 

int OM(){


	gStyle->SetOptStat(0);


	ptNEvent->Clear();
	ptNEvent->AddText(Form("Event Processed: %d",mvtx_evnts));




    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);


	TLatex * lat = new TLatex(0.55,0.13,Form("%s",std::ctime(&end_time)));
	lat->SetNDC();
	lat->SetTextAlign(32);
	lat->SetTextFont(42);
	lat->SetTextSize(0.05);
	lat->SetLineWidth(2);


	cOM0 = new TCanvas("cOM0","cOM0",900,600);
	cOM0->cd();
	MyPad0 = new TPad("MyPad0", "MyPad0",0,0,0.95,0.95);
	MyPad0->SetMargin(0.15,0.15,0.20,0.15);
	MyPad0->Draw();
	MyPad0->cd();

	InfoCanvas->Draw();
	lat->Draw("SAME");


	cOM0->Modified();

	MyPad0->Modified();
	MyPad0->Update();
	cOM0->Update();



	cOM2 = new TCanvas("cOM2","cOM2",1800,600);


	cout << "Cycle " << Cycle  << endl;


	//	cOM2->cd();



	if(InitFlag == 0){


		cout << "START INITIALIZING" << endl;
		// cOM2->cd();
		for(int i = 0; i < 3; i++){
			cOM2->cd();


			MyPad[i] = new TPad(Form("MyPad_%d",i), Form("MyPad_%d",i),0.33*i, 0.10, 0.33*(i+1),0.90 );
			MyPad[i]->SetMargin(0.15,0.15,0.22,0.15);
			gPad->SetLogy();
			MyPad[i]->Draw();

		}

		MyPad[0]->cd();
		EvtHitDis->Draw("hist");
		lat->Draw("SAME");
		MyPad[0]->Update();

		MyPad[1]->cd();
		lat->Draw("SAME");	
		EvtHitChip->Draw("hist");
		MyPad[1]->Update();


		MyPad[2]->cd();
		lat->Draw("SAME");	
		ChipFiredHis->Draw("hist");
		MyPad[2]->Update();

		//	InitFlag = 1;


	}

	cOM2->Modified();

	for (int i = 0; i < 3; i++){


		MyPad[i]->Modified();
	}



	cOM2->Update();


	cOM3 = new TCanvas("cOM3","cOM3",1800,800);


	for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){
			cOM3->cd();




			MyPad2[i][j] = new TPad(Form("MyPad2_%d_%d",i,j), Form("MyPad2_%d_%d",i,j), 0.11*j, 0.24*i, 0.11*(j+1), 0.24*(i+1) );
			MyPad2[i][j]->SetMargin(0.15,0.05,0.15,0.10);
			MyPad2[i][j]->Draw();
			lat->Draw("SAME");	
			MyPad2[i][j]->cd();






			HitMap[i][j]->Draw("COLZ");
			/*
			   MyPad2[i][j]->Update();

			   MyPad2[i][j]->Modified();
			   MyPad2[i][j]->Update();
			   */



			//ConfirmXAxis(HitMap[i][j]);
			ReverseYAxis(HitMap[i][j]);

			cout << "STAVE: " << i << "   CHIP: " << j << "  Total Hits: " << HitMap[i][j]->Integral() << endl;


		}
	}

	cOM3->Modified();

	for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){

			MyPad2[i][j]->Modified();
		}
	}
	cOM3->Update();



	//Ave Occ



	cOM5 = new TCanvas("cOM5","cOM5",1800,800);


	for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){
			cOM5->cd();




			MyPad4[i][j] = new TPad(Form("MyPad4_%d_%d",i,j), Form("MyPad4_%d_%d",i,j), 0.11*j, 0.24*i, 0.11*(j+1), 0.24*(i+1) );
			MyPad4[i][j]->SetMargin(0.15,0.05,0.15,0.10);
			MyPad4[i][j]->Draw();
			lat->Draw("SAME");	
			MyPad4[i][j]->cd();

		
		
			TH2D * HitMapAveOcc = (TH2D *) HitMap[i][j]->Clone(Form("HitMapAveOcc_%d_%d",i,j));

			HitMapAveOcc->Scale(1.0/mvtx_evnts);
		

			HitMapAveOcc->Draw("COLZ");
			/*
			   MyPad2[i][j]->Update();

			   MyPad2[i][j]->Modified();
			   MyPad2[i][j]->Update();
			   */



			//ConfirmXAxis(HitMap[i][j]);
			ReverseYAxis(HitMapAveOcc);

			cout << "STAVE: " << i << "   CHIP: " << j << "  Total Hits: " << HitMap[i][j]->Integral() << endl;


		}
	}

	cOM5->Modified();

	for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){

			MyPad4[i][j]->Modified();
		}
	}
	cOM5->Update();















	cOM4 = new TCanvas("cOM4","cOM4",600,600);

	cOM4->cd();
	MyPad3 = new TPad("MyPad3", "MyPad3",0,0,0.95,0.95);
	MyPad3->SetMargin(0.15,0.15,0.15,0.15);
	MyPad3->Draw();
	lat->Draw("SAME");	
	MyPad3->cd();
	ChipStave->Draw("COLZ");

	cOM4->Modified();

	MyPad3->Modified();
	MyPad3->Update();
	cOM4->Update();


	cOM6 = new TCanvas("cOM6","cOM6",600,600);
	cOM6->cd();
	lat->Draw("SAME");
	ChipStaveOcc->Draw("COLZ");
	cOM6->Modified();
	cOM6->Update();



	Cycle = Cycle + 1;


	return 0;
}

//TPad * MyPad4[NSTAVE][NCHIP];


int print_my_canvas()
{

	TCanvas * cMy = new TCanvas("cMy","cMy",600,600);

	cMy->cd();
	cMy->SetRightMargin(0.1);





	mkdir(Form("QAPlots/HitMaps/%d/",RunID),0777);
	mkdir(Form("QAPlots/ChipOccupancy/%d/",RunID),0777);



	//EvtHitDis->Draw("hist");
	//cMy->SaveAs("EvtHitDis.png");
	for(int i = 0; i < NSTAVE; i++){
		for(int j = 0; j < NCHIP; j++){

			gPad->Update();

			gPad->SetRightMargin(0.15);

			gPad->Draw();
			gPad->cd();

			HitMap[i][j]->Draw("COLZ");



			ReverseYAxis(HitMap[i][j]);
			

			cMy->SaveAs(Form("QAPlots/HitMaps/%d/HitMap_%d_%d.png",RunID,i,j));


		}
	}





	ChipStave->Draw("COLZ");

	cMy->SaveAs(Form("QAPlots/ChipOccupancy/%d/ChipStave.png",RunID));



	ChipStaveOcc->Scale(1.0/mvtx_evnts);
	ChipStaveOcc->Draw("COLZ");
	cMy->SaveAs(Form("QAPlots/ChipOccupancy/%d/ChipStaveOcc.png",RunID));

	gPad->Update();

	gPad->SetRightMargin(0.10);
	gPad->SetLeftMargin(0.16);

	gPad->Draw();
	gPad->cd();

	EvtHitDis->GetYaxis()->SetTitleOffset(2.4);
	EvtHitDis->Draw("hist");

	cMy->SaveAs(Form("QAPlots/ChipOccupancy/%d/EvtHitDis.png",RunID));

	EvtHitChip->GetYaxis()->SetTitleOffset(2.4);
	EvtHitChip->Draw("hist");

	cMy->SaveAs(Form("QAPlots/ChipOccupancy/%d/EvtHitChip.png",RunID));
	ChipFiredHis->GetYaxis()->SetTitleOffset(2.4);
	ChipFiredHis->Draw("hist");

	cMy->SaveAs(Form("QAPlots/ChipOccupancy/%d/ChipFiredHis.png",RunID));





	ChipStave1D->Scale(1.0/mvtx_evnts * 100.0);
	ChipStave1D->Draw("hist");
	cMy->SaveAs(Form("QAPlots/ChipOccupancy/%d/ChipStave1D.png",RunID));



	fout->cd();

	HitMap[1][4]->Write();
	ChipStaveOcc->Write();
	ChipStave1D->Write();
	ChipFiredHis->Write();
	EvtHitChip->Write();
	EvtHitDis->Write();

	InfoCanvas->Write();

	fout->Close();



	return 0;
}


//============================================================//


int pstop()
{

	cout << "mvtx_evnts = " << mvtx_evnts << endl;
	TotalFiles = TotalFiles + 1;


	bulb->SetFillColor(kRed);
	ptNEvent->Clear();
	ptNEvent->AddText(Form("Event Processed: %d",mvtx_evnts));


	ptNFile->Clear();
	ptNFile->AddText(Form("File Processed: %d",TotalFiles));


    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);


	TLatex * lat = new TLatex(0.55,0.13,Form("%s",std::ctime(&end_time)));
	lat->SetNDC();
	lat->SetTextAlign(32);
	lat->SetTextFont(42);
	lat->SetTextSize(0.05);
	lat->SetLineWidth(2);


	cOM0 = new TCanvas("cOM0","cOM0",900,600);
	cOM0->cd();
	MyPad0 = new TPad("MyPad0", "MyPad0",0,0,0.95,0.95);
	MyPad0->SetMargin(0.15,0.15,0.20,0.15);
	MyPad0->Draw();
	MyPad0->cd();

	InfoCanvas->Draw();
	lat->Draw("SAME");


	cOM0->Modified();

	MyPad0->Modified();
	MyPad0->Update();
	cOM0->Update();



	cOM0->SaveAs("FileDoneTest.png");

	return 0;


}

void reset_histos()
{
}

//============================================================//


int XTicks;
int DivisionStep = 32;


void ConfirmXAxis(TH2 *h)
{
	// Remove the current axis
	h->GetXaxis()->SetLabelOffset(999);
	h->GetXaxis()->SetTickLength(0);
	// Redraw the new axis
	gPad->Update();
	XTicks = (h->GetXaxis()->GetXmax()-h->GetXaxis()->GetXmin())/DivisionStep;

	TGaxis *newaxis = new TGaxis(gPad->GetUxmin(),
			gPad->GetUymin(),
			gPad->GetUxmax(),
			gPad->GetUymin(),
			h->GetXaxis()->GetXmin(),
			h->GetXaxis()->GetXmax(),
			XTicks,"N");
	newaxis->SetLabelOffset(0.0);
	newaxis->Draw();
	//	h->GetListOfFunctions()->Add(newaxis);	
}

void ReverseYAxis (TH2 *h)
{
	// Remove the current axis
	h->GetYaxis()->SetLabelOffset(999);
	h->GetYaxis()->SetTickLength(0);


	XTicks = (h->GetYaxis()->GetXmax()-h->GetYaxis()->GetXmin())/DivisionStep;


	//Redraw the new axis

	gPad->Update();




	TGaxis *newaxis = new TGaxis(gPad->GetUxmin(),
			gPad->GetUymax(),
			gPad->GetUxmin()-0.001,
			gPad->GetUymin(),
			h->GetYaxis()->GetXmin(),
			h->GetYaxis()->GetXmax(),
			XTicks,"+");
	newaxis->SetLabelOffset(-0.03);
	newaxis->Draw();
	//	h->GetListOfFunctions()->Add(newaxis);	

}
