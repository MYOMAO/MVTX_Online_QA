#include "TCanvas.h"

#include "TH2.h"

using namespace std;

TGaxis *newaxis;

void ReverseYAxis (TH2 *h)
{
   // Remove the current axis
   h->GetYaxis()->SetLabelOffset(999);
   h->GetYaxis()->SetTickLength(0);

   // Redraw the new axis
   gPad->Update();



   newaxis = new TGaxis(gPad->GetUxmax(),
                                gPad->GetUymin(),
                                gPad->GetUxmin(),
                                gPad->GetUymin(),
                                h->GetXaxis()->GetXmin(),
                                h->GetXaxis()->GetXmax(),
                                510,"-");
   newaxis->SetLabelOffset(-0.03);
   newaxis->Draw();
}

void Debug(){

	TH2D * Check = new TH2D("Check","",100,0,2,100,0,2);
	Check->Fill(1,1);
	Check->Fill(1,1);
	Check->Fill(1,1);


	TCanvas *c = new TCanvas("c","c",600,600);
	c->cd();

	Check->Draw("COLZ");

	gPad->Update();
   
	TPaletteAxis * palette = (TPaletteAxis*)Check->GetListOfFunctions()->FindObject("palette");
    palette->SetX1NDC(0.9);
    palette->SetX2NDC(0.95);
    palette->SetY1NDC(0.2);
    palette->SetY2NDC(0.8);


	ReverseYAxis(Check);


		
	return;
	
}


