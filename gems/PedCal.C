#include "TFile.h"
#include "TH1F.h"
#include <assert.h>
#include <utility>
#include "TF1.h"
#include "TChain.h"
#include "TMath.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>

#include <TCanvas.h>

Double_t fitf(Double_t *v, Double_t *par)
{
  	Double_t arg = 0;
  	if (par[2] != 0) arg = (v[0] - par[1])/par[2];
	Double_t fitval = par[0]*TMath::Exp(-0.5*arg*arg);
   	return fitval;
}


void PedCal()
{
	Int_t NoOfDet=-10;
	
	Int_t argc=-10;
	//cout<<"Enter run number"<<endl;
   	//cin>>argc;
	ifstream fh_infile;
    	fh_infile.open("config.txt");
    	if(!fh_infile)
	{
		cout<<"cannot open input file"<<endl;
    		return;
    	}
	
	fh_infile>>NoOfDet;
    	const int NDet = (int)NoOfDet;
	
    	vector<int> xstrip;
    	vector<int> ystrip;
    	int dumy1,dumy2;
    	for(int pq=0;pq<NoOfDet;pq++)
    	{
    		fh_infile>>dumy1>>dumy2;
		cout<<"xch "<<dumy1<<" ych "<<dumy2<<endl;
    		xstrip.push_back(dumy1);
    		ystrip.push_back(dumy2);
    	}
       // fh_infile>>dumy1; 
	fh_infile>>argc;
	cout<<"Run number "<<argc<<endl;
	fh_infile.close();
	TChain *T = new TChain("T");
    	T->Add(Form("~/rootfiles/run_%d.root",argc));
	Double_t* x_strip=new Double_t[NDet];
        Double_t* y_strip=new Double_t[NDet];//# of strips in X/Y
        Double_t*** xadc = new Double_t**[NDet];
        for(int detnumber=0;detnumber<NDet;detnumber++)
        {
                //const int xch = (int) xstrip[detnumber];
                xadc[detnumber]=new Double_t*[6];              
                for(int NoOfSample=0;NoOfSample<6;NoOfSample++)
                        xadc[detnumber][NoOfSample]=new Double_t[xstrip[detnumber]];
        }

        Double_t*** yadc = new Double_t**[NDet];
        for(int detnumber=0;detnumber<NDet;detnumber++)
        {
                yadc[detnumber]=new Double_t*[6];
                for(int NoOfSample=0;NoOfSample<6;NoOfSample++)
                        yadc[detnumber][NoOfSample]=new Double_t[ystrip[detnumber]];
        }

        Double_t** xstripID = new Double_t*[NDet];
        for(int detnumber=0;detnumber<NDet;detnumber++)
                xstripID[detnumber]=new Double_t[xstrip[detnumber]];

        Double_t** ystripID = new Double_t*[NDet];
        for(int detnumber=0;detnumber<NDet;detnumber++)
                ystripID[detnumber]=new Double_t[ystrip[detnumber]];

	map<int, vector<TH1F*> > hx;
	map<int, vector<TH1F*> > hy;

	for(int detnumber=0;detnumber<NoOfDet;detnumber++)
    	{

		T->SetBranchAddress(Form("sbs.gems.x%d.nch",detnumber+1), &x_strip[detnumber]);
    		T->SetBranchAddress(Form("sbs.gems.y%d.nch",detnumber+1), &y_strip[detnumber]);
	    	T->SetBranchAddress(Form("sbs.gems.x%d.strip",detnumber+1), xstripID[detnumber]);
	    	T->SetBranchAddress(Form("sbs.gems.y%d.strip",detnumber+1), ystripID[detnumber]);
		for(int ij=0;ij<6;ij++)
		{
	      		T->SetBranchAddress(Form("sbs.gems.x%d.adc%d",detnumber+1,ij),xadc[detnumber][ij]);
	      		T->SetBranchAddress(Form("sbs.gems.y%d.adc%d",detnumber+1,ij),yadc[detnumber][ij]);
	  	}
		
		for(int ij=0;ij<xstrip[detnumber];ij++)
	    	{
	       		hx[detnumber].push_back(new TH1F(Form("hx_%d_%d",detnumber+1,ij+1),Form("Det %d Pedestal x_%d",detnumber+1,ij+1),2000,0,2000));
	    	}	 
		
		for(int ij=0;ij<ystrip[detnumber];ij++)
	    	{
	       		hy[detnumber].push_back(new TH1F(Form("hy_%d_%d",detnumber+1,ij+1),Form("Det %d Pedestal y_%d",detnumber+1,ij+1),2000,0,2000));
	    	}	 
	}
	    
	int entries = T->GetEntries();
	    	
	for(int ij=0;ij<entries;ij++)
	//for(int ij=0;ij<10;ij++)
	//if(event<entries)
	{
		T->GetEntry(ij);
		for(int detnumber=0;detnumber<NDet;detnumber++)
		{
			Double_t dummy = 0;  
	    		//cout<<"x_strip "<<x_strip<<"  y_strip "<<y_strip<<endl;
			for(int pq=0;pq<xstrip[detnumber];pq++)
	    		{
	    			dummy=0;
	    			for(int ik=0;ik<6;ik++)
	    			{
	    				dummy += xadc[detnumber][ik][pq];
	    			}
	    			dummy = dummy/6.;
	    			hx[detnumber][pq]->Fill(dummy);
			}
	    		for(int pq=0;pq<ystrip[detnumber];pq++)
	    		{
	    			dummy = 0;
	    			for(int ik = 0;  ik<6; ik++)
	    			{
	    				dummy += yadc[detnumber][ik][pq];
	    			}
	    
				dummy = dummy/6;
	    			hy[detnumber][pq]->Fill(dummy);
	    		}
	    	}
	}
	    	
    	ofstream fh_out;
    	TString outfile="SelectPulse.txt";
    	fh_out.open(outfile);
    	if(!fh_out)
    	{
    		cout<<"Cannot open output file " <<outfile<<endl;
    		return;
    	}

	Double_t xcentroid;
	Double_t ycentroid;
	Double_t xsigma;
	Double_t ysigma;
	Double_t xamp;
	Double_t yamp;

	map<int, vector<TCanvas*> > plotx;
    	map<int, vector<TCanvas*> > ploty;
	for(int detnumber=0;detnumber<NDet;detnumber++)
	{
    		int xcanvasIndex=0;
		for(int ij=0; ij<xstrip[detnumber];ij++)
    		{
    			hx[detnumber][ij]->GetXaxis()->SetRangeUser(1,1500);
	    		xcentroid=(hx[detnumber][ij]->GetMaximumBin()-1);
    			xamp=(hx[detnumber][ij]->GetBinContent(xcentroid));
    			xsigma=20;
	    		TF1 *func = new TF1("fit",fitf,xcentroid-100,xcentroid+100,3);
    			/*if(ij%25==0)
			cout<<"  xch  "<<ij<<" xcentroid  "<<xcentroid[ij]<<
	    		"  xamp "<<xamp[ij]<<"  xsigma  "<<xsigma[ij]<<endl;*/
    			func->SetParameters(xamp,xcentroid,xsigma);
    			func->SetParNames("Constant","Mean_value","Sigma");
	    		hx[detnumber][ij]->Fit("fit","RQ0","",xcentroid-100,xcentroid+100);
    
			fh_out<<func->GetParameter(1)<<"\t"<<fabs(func->GetParameter(2))<<endl;
    			if(ij%50==0)
			{
				cout<<"After fitting for Det "<<detnumber<<"  xcentroid "<<func->GetParameter(1)<<"  xamplitude  "<<func->GetParameter(0)<<" xsigma  "<< func->GetParameter(2)<<endl;
	    			plotx[detnumber].push_back(new TCanvas(Form("xplot_%d_%d",detnumber+1,ij+1),Form("Det %d Ped x strip %d",detnumber+1,ij+1),800,800));
	    			plotx[detnumber][xcanvasIndex]->SetLogy();
		    		hx[detnumber][ij]->GetXaxis()->SetTitleSize(0.04);
		    		hx[detnumber][ij]->GetXaxis()->SetTitleOffset(1.0);
	    			hx[detnumber][ij]->GetXaxis()->SetTitle("ADC");
	    			hx[detnumber][ij]->GetYaxis()->SetTitleOffset(1.0);
		    		hx[detnumber][ij]->GetYaxis()->SetTitleSize(0.04);
		    		hx[detnumber][ij]->GetYaxis()->SetTitle("Counts");
				hx[detnumber][ij]->Draw();
	    			func->Draw("same");
		    		xcanvasIndex++;
    			}
    		}
	    	int ycanvasIndex=0;
    		for(int ij=0; ij<ystrip[detnumber];ij++)
	    	{
    			hy[detnumber][ij]->GetXaxis()->SetRangeUser(1,1500);
    			ycentroid=hy[detnumber][ij]->GetMaximumBin()-1;
	    		yamp=hy[detnumber][ij]->GetBinContent(ycentroid);
    			ysigma=20;
    			TF1 *func1 = new TF1("fit1",fitf,ycentroid-100,ycentroid+100,3);
	    		/*if(ij%50==0)
    			cout<<"  ych  "<<ij<<" ycentroid  "<<ycentroid[ij]<<
	        	"  yamp "<<yamp[ij]<<"  ysigma  "<<ysigma[ij]<<endl;*/
	    		func1->SetParameters(yamp,ycentroid,ysigma);
    			func1->SetParNames("Constant","Mean_value","Sigma");
			hy[detnumber][ij]->Fit("fit1","RQ0","",ycentroid-100,ycentroid+100);
    			//cout<<"After fitting  centroid "<<func1->GetParameter(1)<<"  yamplitude  "<<func1->GetParameter(0)<<" ysigma  "<< func1->GetParameter(2)<<endl;
    			fh_out<<func1->GetParameter(1)<<"\t"<<fabs(func1->GetParameter(2))<<endl;
		    	if(ij%50==0)
			{
	    			cout<<"After fitting for Det "<<detnumber<<" centroid "<<func1->GetParameter(1)<<"  yamplitude  "<<func1->GetParameter(0)<<" ysigma  "<< func1->GetParameter(2)<<endl;
	    			ploty[detnumber].push_back(new TCanvas(Form("Det %d yplot_%d",detnumber+1,ij+1),Form("Det %d Ped y strip %d",detnumber+1,ij+1),800,800));
		    		ploty[detnumber][ycanvasIndex]->SetLogy();
		    		hy[detnumber][ij]->GetXaxis()->SetTitleSize(0.04);
	    			hy[detnumber][ij]->GetXaxis()->SetTitleOffset(1.0);
	    			hy[detnumber][ij]->GetXaxis()->SetTitle("ADC");
		    		hy[detnumber][ij]->GetYaxis()->SetTitleOffset(1.0);
		    		hy[detnumber][ij]->GetYaxis()->SetTitleSize(0.04);
	    			hy[detnumber][ij]->GetYaxis()->SetTitle("Counts");
	   			hy[detnumber][ij]->Draw();
		    		func1->Draw("same");
	    			ycanvasIndex++;
	    		}
    		}
	}
	fh_out.close();
	
	for(int detnumber=0;detnumber<NDet;detnumber++)
	{
		for(int NoOfSample=0;NoOfSample<6;NoOfSample++)
		{
			delete [] xadc[detnumber][NoOfSample];
			delete [] yadc[detnumber][NoOfSample];
		}
		delete [] xadc[detnumber];
		delete [] yadc[detnumber];
	}
	delete [] xadc;
	delete [] yadc;
}

