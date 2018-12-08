#include "TFile.h"
#include "TH1F.h"
#include <utility>
#include "TChain.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <cstdlib>
#include <functional>
#include <math.h>
#include <iterator>
#include <TCanvas.h>

void PlotGEMData(int event)
//void PlotGEMData()
{
	gStyle->SetOptStat(0);
	Int_t NoOfDet = -10;
	Int_t argc =-10;//run number

	ifstream fh_infile;
	fh_infile.open("config.txt");
	if(!fh_infile)
	{
		cout<<"Cannot open input config file"<<endl;
	}
	fh_infile>>NoOfDet;
	const int NDet = (int)NoOfDet;

	vector<int> xstrip;
	vector<int> ystrip;
	int dumy1,dumy2;

	for(int pq=0;pq<NoOfDet;pq++)
	{
		fh_infile>>dumy1>>dumy2;
		//cout<<"Det "<<pq<<" xch "<<dumy1<<" ych "<<dumy2<<endl;
		xstrip.push_back(dumy1);
		ystrip.push_back(dumy2);
	}
	fh_infile>>argc;
	fh_infile.close();
	/*	
	for(int detnumber=0;detnumber<NDet;detnumber++)
	{
		cout<<"Det "<<detnumber<<" xstrip "<<xstrip[detnumber]<<" ystrip "<<ystrip[detnumber]<<endl;
	}*/	
    	cout<<"The run mumber: "<<argc<<endl;
    	TChain *T = new TChain("T");
    	//int argc=796;
	//cin>>argc;
	T->Add(Form("~/rootfiles/test_%d.root",argc));
      	//  cout<<"hi Chandan 1"<<endl;
	
	map<int, vector<TH1F*> > h_sample;
	map<int, TCanvas*> plot;
/*
	map<int, Double_t> x_strip;
	map<int, Double_t> y_strip;
   	map<int, map<int, Double_t> > xstripID; 
   	map<int, map<int, Double_t> > ystripID; 
	map<int, map<int, Double_t> > xadc;
	map<int, map<int, Double_t> > yadc;
*/
	//for(int ij=0;	
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

	int entries = T->GetEntries();
	if(event<entries)
    	{
		for(int detnumber = 0;detnumber<NDet;detnumber++)
		{
			
			h_sample[detnumber].clear();
	   	
      			cout<<"detnumber  "<<detnumber<<" xch "<<xstrip[detnumber]<<" ych "<<ystrip[detnumber]<<endl;	
			T->SetBranchAddress(Form("sbs.gems.x%d.nch",detnumber+1), &x_strip[detnumber]);
			T->SetBranchAddress(Form("sbs.gems.y%d.nch",detnumber+1), &y_strip[detnumber]);
	    		T->SetBranchAddress(Form("sbs.gems.x%d.strip",detnumber+1), xstripID[detnumber]);
			T->SetBranchAddress(Form("sbs.gems.y%d.strip",detnumber+1), ystripID[detnumber]);
	   	 
			for(int ij=0;ij<6;ij++)
	  		{
	      			T->SetBranchAddress(Form("sbs.gems.x%d.adc%d",detnumber+1,ij),xadc[detnumber][ij]);
		      		T->SetBranchAddress(Form("sbs.gems.y%d.adc%d",detnumber+1,ij),yadc[detnumber][ij]);
		  	}
	    	
			for(int ij=0;ij<(int)(1.*(xstrip[detnumber]+ystrip[detnumber])/128.);ij++)
			{
				//cout<<"det "<<detnumber<<" ij "<<ij<<endl;
				if(ij<(int)(xstrip[detnumber]/128.))
				{
					cout<<"det "<<detnumber<<"xapv ij "<<ij<<endl;
					h_sample[detnumber].push_back(new TH1F(Form("Det_%d_apv_%d",detnumber+1,ij+1),Form("Det %d X APV %d",detnumber+1,ij+1),793,0,793));
				}
				else
				{
					cout<<"det "<<detnumber<<"yapv  ij "<<ij<<endl;
		 			h_sample[detnumber].push_back(new TH1F(Form("Det_%d_apv_%d",detnumber+1,ij+1),Form("Det %d Y APV %d",detnumber+1,ij+1),793,0,793));
				}
			}
			cout<<"det "<<detnumber<<" hsample size "<<h_sample[detnumber].size()<<endl;
			//cout<<"Test "<<(int)((xstrip[detnumber]+ystrip[detnumber])/128.)<<endl;
		}
		
		T->GetEntry(event-1);
		//cout<<"test 2"<<endl;	
		//cout<<"x_strip "<<x_strip<<"  y_strip "<<y_strip<<endl;
		for(int detnumber=0;detnumber<NDet;detnumber++)
		{
			//map<int, vector<TH1F*> >::iterator it;
			//it=h_sample.begin();
			//it->first = detnumber;
			for(int pq=0;pq<xstrip[detnumber];pq++)
			{
			cout<<"pq "<<pq<<" test 2"<<endl;	
				if(pq<128)
				{
	      				for(int ik=0;ik<6;ik++)
	    				{
						h_sample[detnumber][0]->SetBinContent(xstripID[detnumber][pq]+131*ik,xadc[detnumber][ik][pq]);
	    				}
				}
				
				if(pq>=128 && pq <256)
				{
	      				for(int ik=0;ik<6;ik++)
	    				{
						h_sample[detnumber][1]->SetBinContent(xstripID[detnumber][pq]+131*ik-128,xadc[detnumber][ik][pq]);
	    				}
				}
			}
			//cout<<"pq "<<pq<<endl;
			for(int pq=0;pq<ystrip[detnumber];pq++)
			{
				if(pq<128)
				{
	      				for(int ik=0;ik<6;ik++)
	    				{
						h_sample[detnumber][2]->SetBinContent(ystripID[detnumber][pq]+131*ik,yadc[detnumber][ik][pq]);
	    				}
				}
				if(pq>=128 && pq <256)
				{
	      				for(int ik=0;ik<6;ik++)
	    				{
						h_sample[detnumber][3]->SetBinContent(ystripID[detnumber][pq]+131*ik-128,yadc[detnumber][ik][pq]);
	    				}
				}
				if(pq>=256 && pq <384)
				{
	      				for(int ik=0;ik<6;ik++)
	    				{
						h_sample[detnumber][4]->SetBinContent(ystripID[detnumber][pq]+131*ik-128*2,yadc[detnumber][ik][pq]);
	    				}
				}
				if(pq>=384 && pq <512)
				{
	      				for(int ik=0;ik<6;ik++)
	    				{
						h_sample[detnumber][5]->SetBinContent(ystripID[detnumber][pq]+131*ik-128*3,yadc[detnumber][ik][pq]);
	    				}
				}
			}

			plot[detnumber] = new TCanvas(Form("Det_%d_canvas",detnumber+1),Form("Det. %d single event",detnumber+1),800,600);
			plot[detnumber]->Divide(1,(int)((xstrip[detnumber]+ystrip[detnumber])/128));
			for(int pqr =1;pqr<=(int)((xstrip[detnumber]+ystrip[detnumber])/128);pqr++)
			{
				plot[detnumber]->cd(pqr);
				h_sample[detnumber][pqr-1]->GetXaxis()->SetLabelSize(0.11);
				h_sample[detnumber][pqr-1]->GetYaxis()->SetLabelSize(0.11);
				h_sample[detnumber][pqr-1]->GetYaxis()->SetTitle("ADC");
				h_sample[detnumber][pqr-1]->GetXaxis()->SetTitleOffset(0.31);
				h_sample[detnumber][pqr-1]->GetXaxis()->SetTitleSize(0.11);
				h_sample[detnumber][pqr-1]->GetXaxis()->SetTitle("stripNo.Sample");
				h_sample[detnumber][pqr-1]->GetYaxis()->SetTitleOffset(0.22);
				h_sample[detnumber][pqr-1]->GetYaxis()->SetTitleSize(0.11);
				h_sample[detnumber][pqr-1]->Draw();
			}
		}
	}
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
