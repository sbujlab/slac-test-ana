#include "TFile.h"
#include "TH1F.h"
#include "TChain.h"
#include "TH2F.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <functional>
#include <math.h>
#include <utility>
#include <map>
#include <TCanvas.h>
#include <TStyle.h>
#include <iterator>
map<int, vector <double> > xcentroid;
map<int, vector <double> > xsigma;
map<int, vector <double> > ycentroid;
map<int, vector <double> > ysigma;
bool fsampleplot = 0;
void Sorting()
{
	//gStyle->SetOptStat(0);
	cout<<" Start time "<<time(NULL)<<endl;
        Int_t argc=-10;
        ifstream fh_infile;
        fh_infile.open("config.txt");
        if(!fh_infile)
        {
                cout<<"cannot open input file"<<endl;
                return;
        }
	int NoOfDet,ch_qdc;//No of GEM detector and no of QDC channel used
        fh_infile>>NoOfDet;
        const int NDet = (int)NoOfDet;

        vector<int> xstrip;
        vector<int> ystrip;
        int dumy1,dumy2;
        for(int pq=0;pq<NoOfDet;pq++)
        {
                fh_infile>>dumy1>>dumy2;
                cout<<"xstrip "<<dumy1<<" ystrip "<<dumy2<<endl;
                xstrip.push_back(dumy1);
                ystrip.push_back(dumy2);
        }

        fh_infile>>ch_qdc;
        fh_infile>>argc;
        cout<<"Run number "<<argc<<endl;
        fh_infile.close();

	int ij;
	Double_t xcentroid1;
	Double_t ycentroid1;
	Double_t xcharge;
	Double_t ycharge;
	//Pedestal information
	TString infile = "SelectPulse.txt";
	ifstream fh_infile1;
	fh_infile1.open(infile);
	if(!fh_infile1)
	{
		cout<<"Cannot open pedestal file : "<<infile<<endl;
		return;
	}
	Double_t temp1,temp2;
	for(int detnumber=0;detnumber<NDet;detnumber++)
	{
		for(int ij=0;ij<xstrip[detnumber];ij++)
		{
			fh_infile1>>temp1>>temp2;
			xcentroid[detnumber].push_back(temp1);
			xsigma[detnumber].push_back(temp2);
		}
		for(int ij=0;ij<ystrip[detnumber];ij++)
		{
			fh_infile1>>temp1>>temp2;
			ycentroid[detnumber].push_back(temp1);
			ysigma[detnumber].push_back(temp2);
		}
	}
/*	
	map<int, vector<double> >::iterator it1,it2;
	for(it1=xcentroid.begin();it1!=xcentroid.end();++it1)
	{
		int DetNumber = it1->first;
		vector<double> testxcentroid = it1->second;
		for(int pq=0;pq<(int) testxcentroid.size();pq++)
		{
			cout<<"Detector number "<<DetNumber<<" xcentroid "<<xcentroid[DetNumber][pq]<<" xsigma "<<xsigma[DetNumber][pq]<<endl;
		}
	}
	for(it1=ycentroid.begin();it1!=ycentroid.end();++it1)
	{
		int DetNumber = it1->first;
		vector<double> testycentroid = it1->second;
		for(int pq=0;pq<(int) testycentroid.size();pq++)
		{
			cout<<"Detector number "<<DetNumber<<" ycentroid "<<ycentroid[DetNumber][pq]<<" ysigma "<<ysigma[DetNumber][pq]<<endl;
		}
	}
*/

	fh_infile1.close();
	
    	TChain *T = new TChain("T");
    	T->Add(Form("~/rootfiles/run_%d.root",argc));
		
	Double_t* x_strip=new Double_t[NDet];
       	Double_t* y_strip=new Double_t[NDet];//# of strips in X/Y
	Double_t*** xadc = new Double_t**[NDet];
	for(int detnumber=0;detnumber<NDet;detnumber++)
	{
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


	vector<double> xintegral;//raw xintegral
	vector<double> yintegral;//raw yintegral
    	vector<double> xcmn;//x common-mode-noise
    	vector<double> ycmn; //y common-mode-noise
	map<int, TH1D* > h_xcharge;//total x charge for an event
	map<int, TH1D* > h_ycharge;//total y charge for an event
	map<int, TH1D* > h_totcharge;//total charge (x+y) for an event
	map<int, TH1D* > h_xstriphit;//(xtrip - xstrip_max) distribution
	map<int, TH1D* > h_ystriphit;//(ystrip - ystrip_max) distribution
	map<int, TH1D* > h_xstriphitgated;//(xtrip - xstrip_max) distribution
	map<int, TH1D* > h_ystriphitgated;//(ystrip - ystrip_max) distribution
	map<int, vector<TH1D*> > h_xcmn;//hist for x common-mode-noise
    	map<int, vector<TH1D*> > h_ycmn;//hist for y common-mode-noise
	map<int, TH1I*> h_xmultiplicity;//hits in GEM plane
	map<int, TH1I*> h_ymultiplicity;//hits in GEM plane
	map<int, TH1I*> h_xmultiplicitygated;//hits in GEM plane
	map<int, TH1I*> h_ymultiplicitygated;//hits in GEM plane
	map<int, TH2D*> hits;//hits in GEM plane
	map<int, TH2D*> h_charge;//x-y charge correlation

	map<int, TH1D* > h_lqdc;//total x charge for an event
	map<int, TH1D* > h_hqdc;//total x charge for an event
	for(int ij_qdc=0;ij_qdc<ch_qdc;ij_qdc++)
	{
		h_lqdc[ij_qdc] = new TH1D(Form("h_lqdc_%d",ij_qdc),Form("QDC (L) ch %d",ij_qdc),5000,0,5000);	
		h_hqdc[ij_qdc] = new TH1D(Form("h_hqdc_%d",ij_qdc),Form("QDC (H) ch %d",ij_qdc),5000,0,5000);
	}	
	Double_t* lqdc =new Double_t[ch_qdc];
	Double_t* hqdc =new Double_t[ch_qdc];
	
	for(int ij_qdc=0;ij_qdc<ch_qdc;ij_qdc++)
	{
    		T->SetBranchAddress(Form("sbs.sbuscint.ladc%d",ij_qdc), &lqdc[ij_qdc]);
    		T->SetBranchAddress(Form("sbs.sbuscint.hadc%d",ij_qdc), &hqdc[ij_qdc]);
	}


	for(int detnumber=0;detnumber<NDet;detnumber++)
	{
    		//const int xapv = xstrip[detnumber]/128;
		//const int yapv = ystrip[detnumber]/128;
    		cout<<"Det "<<detnumber+1<<"  xapv  "<<xstrip[detnumber]/128<<" yapv "<<ystrip[detnumber]/128<<endl;
   
	     	// Double_t integral[6][128] = {-1e10};
		cout<<"test 1"<<endl;
    		for(int pk=0;pk<(int)xstrip[detnumber]/128.;pk++)
	    	{
    			h_xcmn[detnumber].push_back(new TH1D(Form("h_xcmn_%d_%d",detnumber+1,pk),Form("Det %d CMN for xapv %d",detnumber+1,pk),200,-200,200));
    		}
    		for(int pk=0;pk<(int)ystrip[detnumber]/128.;pk++)
	    	{
    			h_ycmn[detnumber].push_back( new TH1D(Form("h_ycmn_%d_%d",detnumber+1,pk),Form("Det %d CMN for yapv %d",detnumber+1,pk),200,-200,200));
	    	}
		h_xcharge[detnumber]=new TH1D(Form("xcharge_Det_%d",detnumber+1),Form("Det %d xcharge",detnumber+1),100,-0.5,10000-0.5); 
		h_ycharge[detnumber]=new TH1D(Form("ycharge_Det_%d",detnumber+1),Form("Det %d ycharge",detnumber+1),100,-0.5,10000-0.5); 
		h_totcharge[detnumber]=new TH1D(Form("charge_Det_%d",detnumber+1),Form("Det %d charge",detnumber+1),200,-0.5,20000-0.5); 
		h_xstriphit[detnumber]=new TH1D(Form("xstriphit_Det_%d",detnumber+1),Form("Det %d (xstrip - xstripMax)",detnumber+1),xstrip[detnumber]+50,-1.*xstrip[detnumber]/2.-25-0.5,xstrip[detnumber]/2.+25-0.5); 
		h_ystriphit[detnumber]=new TH1D(Form("ystriphit_Det_%d",detnumber+1),Form("Det %d (ystrip - ystripMax)",detnumber+1),ystrip[detnumber]+50,-1.*ystrip[detnumber]/2.-25-0.5,ystrip[detnumber]/2.+25-0.5); 
		h_xstriphitgated[detnumber]=new TH1D(Form("xstriphitgated_Det_%d",detnumber+1),Form("Det %d (xstrip - xstripMax) gated",detnumber+1),xstrip[detnumber]+50,-1.*xstrip[detnumber]/2.-25-0.5,xstrip[detnumber]/2.+25-0.5); 
		h_ystriphitgated[detnumber]=new TH1D(Form("ystriphitgated_Det_%d",detnumber+1),Form("Det %d (ystrip - ystripMax) gated",detnumber+1),ystrip[detnumber]+50,-1.*ystrip[detnumber]/2.-25-0.5,ystrip[detnumber]/2.+25-0.5); 
		h_xmultiplicity[detnumber]=new TH1I(Form("xmultiplicity_Det_%d",detnumber+1),Form("Det %d xmultiplicity",detnumber+1),20,-0.5,20-0.5); 
		h_ymultiplicity[detnumber]=new TH1I(Form("ymultiplicity_Det_%d",detnumber+1),Form("Det %d ymultiplicity",detnumber+1),20,-0.5,20-0.5); 
		h_xmultiplicitygated[detnumber]=new TH1I(Form("xmultiplicitygated_Det_%d",detnumber+1),Form("Det %d xmultiplicitygated",detnumber+1),20,-0.5,20-0.5); 
		h_ymultiplicitygated[detnumber]=new TH1I(Form("ymultiplicitygated_Det_%d",detnumber+1),Form("Det %d ymultiplicitygated",detnumber+1),20,-0.5,20-0.5); 
		hits[detnumber]=new TH2D(Form("Det %d XY hits",detnumber+1),Form("Det %d XY hits",detnumber+1),xstrip[detnumber],-0.5,xstrip[detnumber]-0.5,ystrip[detnumber],-0.5,ystrip[detnumber]-0.5);
		h_charge[detnumber]=new TH2D(Form("Det %d YX charge",detnumber+1),Form("Det %d YX charge", detnumber+1),100,-0.5,9999.5,100,-0.5,9999.5);
	
    		T->SetBranchAddress(Form("sbs.gems.x%d.nch",detnumber+1), &x_strip[detnumber]);
		T->SetBranchAddress(Form("sbs.gems.y%d.nch",detnumber+1), &y_strip[detnumber]);
    		T->SetBranchAddress(Form("sbs.gems.x%d.strip",detnumber+1), xstripID[detnumber]);
    		T->SetBranchAddress(Form("sbs.gems.y%d.strip",detnumber+1), ystripID[detnumber]);
    		for(int ij=0;ij<6;ij++)
		{
      			T->SetBranchAddress(Form("sbs.gems.x%d.adc%d",detnumber+1,ij),xadc[detnumber][ij]);
      			T->SetBranchAddress(Form("sbs.gems.y%d.adc%d",detnumber+1,ij),yadc[detnumber][ij]);
  		}
	}

	cout<<"Enter event number "<<endl;
        int nevent = 2;
	cin>>nevent;
  	Double_t dummy = 0;  
	int entries = T->GetEntries();
	//for(int ij=0;ij<20;ij++)
	for(int ij=0;ij<entries;ij++)
	{
		T->GetEntry(ij);
		for(int ij_qdc=0;ij_qdc<ch_qdc;ij_qdc++)
		{
			h_lqdc[ij_qdc]->Fill(lqdc[ij_qdc]);
			h_hqdc[ij_qdc]->Fill(hqdc[ij_qdc]);
		}

	    	//cout<<"x_strip "<<x_strip<<"  y_strip "<<y_strip<<endl;
		for(int detnumber=0;detnumber<NDet;detnumber++)
		{
			for(int pq=0;pq<xstrip[detnumber];pq++)
    			{
    				dummy=0;
    				for(int ik=0;ik<6;ik++)
    				{
					//cout<<"pq "<<pq<<" ik  "<<ik<<" xadc "<<xadc[detnumber][ik][pq]<<endl;
    					dummy += xadc[detnumber][ik][pq];
    				}
    				dummy = dummy/6.;
				xintegral.push_back(dummy);
				//cout<<"pq "<<pq<<" xdummy "<<dummy<<endl;
	     			// hx[pq]->Fill(dummy);
			}

	    		vector <double> xadc_temp;
			for(int pk=0;pk<xstrip[detnumber];pk++)
    			{
    				xadc_temp.push_back(xintegral[pk]-xcentroid[detnumber][pk]);
				//cout<<"pk "<<pk<<"X Integral "<<xintegral[pk]<<" xcentroid "<<xcentroid[pk]<<"  xadc_temp "<<xadc_temp[pk]<<endl;	        
			}
	
    			for(int pk=0;pk<xstrip[detnumber]/128.;pk++)//loop over apvs
    			{
				double dumy_xcmn=0;
    				vector<double> xadc_temp_sort;
    				//xadc_temp_sort.insert(xadc_temp_sort.end(),&xadc_temp[128*pk],&xadc_temp[128*(pk+1)]);
    				xadc_temp_sort.insert(xadc_temp_sort.begin(),&xadc_temp[128*pk],&xadc_temp[128*(pk+1)]);
    			//	int xyz=0;
			//	for(std::vector<float>::iterator itv = xadc_temp_sort.begin();itv!=xadc_temp_sort.end();++itv)
			//	{
			//		cout<<"pk "<<pk<<" xyz "<<xyz<<" xadc "<<*itv<<endl;
			//		xyz++;
			//	}
				sort(xadc_temp_sort.begin(),xadc_temp_sort.end());
    				//for(int ip=0;ip<128;ip++)
			 	//{
				//	 cout<<"ip  "<<ip<<" adc_temp_sort "<<adc_temp_sort[ip]<<endl;
				// }
			
    				for(int ik=28;ik<100;ik++)
    				{
    					dumy_xcmn +=xadc_temp_sort[ik];
    				}
    				xcmn.push_back(dumy_xcmn/72.);
    				h_xcmn[detnumber][pk]->Fill(xcmn[pk]);
    				//cout<<"x   apv  "<<pk<<"  cmn "<<xcmn[pk]<<endl;
				vector<double>().swap(xadc_temp_sort);
	    		}

    			for(int pq=0;pq<ystrip[detnumber];pq++)
    			{
				dummy = 0;
    				for(int ik = 0;  ik<6; ik++)
    				{
					//cout<<"pq "<<pq<<" ik  "<<ik<<" yadc "<<yadc[ik][pq]<<endl;
    					dummy += yadc[detnumber][ik][pq];
    				}
    				dummy = dummy/6;
	    			yintegral.push_back(dummy);
    				//hy[pq]->Fill(dummy);
		        }

	    		vector <double> yadc_temp;
			for(int pk=0;pk<ystrip[detnumber];pk++)
    			{
    				yadc_temp.push_back(yintegral[pk]-ycentroid[detnumber][pk]);
	    			//cout<<"pk "<<pk<<"Y Integral "<<yintegral[pk]<<" ycentroid "<<ycentroid[pk]<<"  yadc_temp "<<yadc_temp[pk]<<endl;
		        }
    			for(int pk=0;pk<ystrip[detnumber]/128.;pk++)
	    		{
    				double dumy_ycmn=0; 
    				vector<double> yadc_temp_sort;
    				yadc_temp_sort.insert(yadc_temp_sort.begin(),&yadc_temp[128*pk],&yadc_temp[128*(pk+1)]);
	    			sort(yadc_temp_sort.begin(),yadc_temp_sort.end());
    				//for(int ip=0;ip<128;ip++)
				 //{
				//	 cout<<"ip  "<<ip<<" adc_temp_sort "<<yadc_temp_sort[ip]<<endl;
				// }
			
    				for(int ik=28;ik<100;ik++)
    				{
    					dumy_ycmn +=yadc_temp_sort[ik];
	    			}
    				ycmn.push_back(dumy_ycmn/72.);
    				h_ycmn[detnumber][pk]->Fill(ycmn[pk]);
    				//cout<<"y apv  "<<pk<<"  cmn "<<cmn[pk]<<endl;
				vector<double>().swap(yadc_temp_sort);
		    	}
			
    			map<int, double>  hitx;
    			map<int, double>  hitxgated;
			map<int, map<int, TH1D*> > xhitHisto;
			map<int, map<int, TCanvas*> > C_xhits;
			int nxhits=0;
    			for(int pk=0;pk<xstrip[detnumber]/128.;pk++)
    			{
    				double corrch = 0;
    				for(int ik=0;ik<128;ik++)
    				{
    					corrch = xadc_temp[128*pk+ik]-xcmn[pk];
					//cout<<"pk "<<pk<<" ik "<<ik<<" xadc "<<xadc_temp[128*pk+ik]<<" xcmn "<<xcmn[pk]<<endl;
    					//if(-1*corrch>5*xsigma[128*pk+ik])
    					if(xintegral[128*pk+ik]>(xcentroid[detnumber][128*pk+ik]+5*xsigma[detnumber][128*pk+ik]))
    					//if(corrch>0)
    					{
						if(ij==nevent-1){
						xhitHisto[detnumber][nxhits] = new TH1D(Form("hx_%d_%d_%d",detnumber+1,ij,(Int_t)xstripID[detnumber][128*pk+ik]),Form("Det %d XEvent %d strip %d",detnumber+1,ij,(Int_t)xstripID[detnumber][128*pk+ik]),6,0,6); 
						for(int mn = 0;mn<6;mn++)
						{
							//cout<<"Event "<<ij<<" xapv "<<xapv<<" strip "<<ik<<" cor strip "<<xstripID[128*pk+ik]<<" sample no "<<mn<<" amplitude "<<xadc[mn][128*pk+ik]<<endl;
							xhitHisto[detnumber][nxhits]->SetBinContent(mn+1,xadc[detnumber][mn][128*pk+ik]-xcentroid[detnumber][128*pk+ik]-xcmn[pk]);
						}
						C_xhits[detnumber][nxhits] = new TCanvas(Form("Cx_%d_%d_%d",detnumber+1,ij,(Int_t)xstripID[detnumber][128*pk+ik]),Form("Det %d Canvas XEvent %d strip %d",detnumber+1, ij,(Int_t)xstripID[detnumber][128*pk+ik]),500,500); 
						xhitHisto[detnumber][nxhits]->SetLineWidth(2);
						xhitHisto[detnumber][nxhits]->GetXaxis()->SetTitleSize(0.04);
						xhitHisto[detnumber][nxhits]->GetXaxis()->SetTitleOffset(1.0);
						xhitHisto[detnumber][nxhits]->GetXaxis()->SetTitle("Sample");
						xhitHisto[detnumber][nxhits]->GetYaxis()->SetTitleSize(0.04);
						xhitHisto[detnumber][nxhits]->GetYaxis()->SetTitleOffset(1.28);
						xhitHisto[detnumber][nxhits]->GetYaxis()->SetTitle("ADC");
						xhitHisto[detnumber][nxhits]->Draw("");
						nxhits++;}
						hitx.insert(pair <int, double> ((Int_t)xstripID[detnumber][128*pk+ik],1.*corrch));
			        		//cout<<"Event "<<ij<<" X corrch "<<corrch<<" sigma  "<<xsigma[128*pk+ik]<<"  strip no "<<xstripID[128*pk+ik]<<endl; 
    					}
    				}
    			}
			map<int, double>::iterator itm,itm1;
			xcentroid1=0;
			ycentroid1=0;
			xcharge = 0;
			ycharge = 0;
			double chargemax=0;
			double stripdiff = 0;
			int stripmax=0;
			int previousstrip=0;
			int multiplicity = 0;
			int dumy_variable=0;
			for(itm= hitx.begin(); itm!=hitx.end();++itm)
			{
				//cout<<"Event "<<ij<<"  inside iterator loop map first element "<<itm->first<<"  charge  "<<itm->second<<endl;
				//xcharge +=itm->second; 
				//xcentroid1 += (itm->first)*(itm->second);
				if((itm->second)>chargemax)
				{
					chargemax=itm->second;
					stripmax=itm->first;
				}
    				//cout<<"x strip id "<<itm->first<<" charge "<<itm->second<<endl;
	    		}
			multiplicity = (int)hitx.size();
			//cout<<"size of hitx "<<(int) hitx.size()<<endl;
			if(multiplicity>0)
				h_xmultiplicity[detnumber]->Fill(multiplicity);
	
			for(itm= hitx.begin(); itm!=hitx.end();++itm)
			{
				h_xstriphit[detnumber]->Fill(itm->first - stripmax);
			}
			for(itm=hitx.begin();itm!=hitx.end();++itm)
			{
				//dumy_variable=0;
				int teststrip;
				double testcharge;
				//itm1 = hitx.begin();
				//int ittest = (int) itm;
				//std::advance(itm1,1);
				for(itm1=itm;itm1!=hitx.end();++itm1)
				{
					//cout<<"For strip "<<itm->first<<" Inside compare loop strip "<<itm1->first<<" charge "<<itm1->second<<endl; 
					if(abs(itm->first - itm1->first)==32 && (itm1->second > itm->second))
					{
						teststrip = itm1->first;
						testcharge = itm1->second;
						//hitxgated.insert(pair <int, double> (itm1->first,itm1->second));
						//cout<<"1st Cond. variable_dumy "<<dumy_variable<<" map first "<<itm1->first<<" second "<<itm1->second<<endl; 
						break;
					}
					else
					{
						teststrip = itm->first;
						testcharge = itm->second;

						//hitxgated.insert(pair <int, double> (itm->first, itm->second));
						//cout<<"2nd Cond. variable_dumy "<<dumy_variable<<" map first "<<itm->first<<" second "<<itm->second<<endl; 
					}
					//dumy_variable++;
				}

				hitxgated.insert(pair <int, double> (teststrip, testcharge));
				//cout<<"size of hitxgated "<<hitxgated.size()<<endl;
			}
			/*
			cout<<"size of hitxgated "<<hitxgated.size()<<endl;
			
			for(itm=hitxgated.begin();itm!=hitxgated.end();++itm)
			{
				cout<<"Event "<<ij<<" hitxgated strip "<<itm->first<<" charge "<<itm->second<<endl;
			}*/
			
			
			for(itm=hitxgated.begin(); itm!=hitxgated.end();++itm)
			{
				stripdiff = itm->first - stripmax;
				h_xstriphitgated[detnumber]->Fill(stripdiff);
				xcharge +=itm->second; 
				xcentroid1 += (itm->first)*(itm->second);
			}
			multiplicity = (int) hitxgated.size();
			if(multiplicity>0)
				h_xmultiplicitygated[detnumber]->Fill(multiplicity);
			if(xcharge>0)
				h_xcharge[detnumber]->Fill(xcharge);

			xcentroid1 = xcentroid1/xcharge;

    			map<int, double> hity;
    			map<int, double> hitygated;
			map<int, map<int, TH1D*> > yhitHisto;
                	map<int, map<int, TCanvas*> > C_yhits;
                	int nyhits=0;
	    		for(int pk=0;pk<(int)ystrip[detnumber]/128.;pk++)
    			{
    				double corrch = 0;
    				for(int ik=0;ik<128;ik++)
	    			{
    					corrch = yadc_temp[128*pk+ik]-ycmn[pk];
    					//if(-1*corrch>5*ysigma[128*pk+ik])
    					if(yintegral[128*pk+ik]>(ycentroid[detnumber][128*pk+ik]+5*ysigma[detnumber][128*pk+ik]))
    					//if(corrch>0)
	    				{
						if(ij==nevent-1){
						yhitHisto[detnumber][nyhits] = new TH1D(Form("hy_%d_%d_%d",detnumber+1,ij,(Int_t)ystripID[detnumber][128*pk+ik]),Form("Det %d YEvent %d strip %d",detnumber+1,ij,(Int_t)ystripID[detnumber][128*pk+ik]),6,0,6); 
						for(int mn = 0;mn<6;mn++)
						{
						//	yhitHisto[nyhits]->GetYaxis()->SetRangeUser(0,1200);
							//cout<<"Event "<<ij<<" yapv "<<yapv<<" strip "<<ik<<" cor strip "<<ystripID[128*pk+ik]<<" sample no "<<mn<<" amplitude "<<yadc[mn][128*pk+ik]<<endl;
							//yhitHisto[nyhits]->SetBinContent(mn+1,ycentroid[128*pk+ik]-yadc[mn][128*pk+ik]+ycmn[pk]);
							yhitHisto[detnumber][nyhits]->SetBinContent(mn+1,yadc[detnumber][mn][128*pk+ik]-ycentroid[detnumber][128*pk+ik]-ycmn[pk]);
						}
						C_yhits[detnumber][nyhits] = new TCanvas(Form("Cy_%d_%d_%d",detnumber+1,ij,(Int_t)ystripID[detnumber][128*pk+ik]),Form("Det %d Canvas YEvent %d strip %d",detnumber+1,ij,(Int_t)ystripID[detnumber][128*pk+ik]),500,500);
						yhitHisto[detnumber][nyhits]->SetLineWidth(2);
						yhitHisto[detnumber][nyhits]->GetXaxis()->SetTitleSize(0.04);
						yhitHisto[detnumber][nyhits]->GetXaxis()->SetTitleOffset(1.0);
						yhitHisto[detnumber][nyhits]->GetXaxis()->SetTitle("Sample");
						yhitHisto[detnumber][nyhits]->GetYaxis()->SetTitleSize(0.04);
						yhitHisto[detnumber][nyhits]->GetYaxis()->SetTitleOffset(1.28);
						yhitHisto[detnumber][nyhits]->GetYaxis()->SetTitle("ADC");
						yhitHisto[detnumber][nyhits]->Draw("");
						nyhits++;}
	
						hity.insert(pair <int, double> ((Int_t)ystripID[detnumber][128*pk+ik],1*corrch));
						//cout<<"Event "<<ij<<" y corrch "<<corrch<<" sigma  "<<ysigma[128*pk+ik]<<"  strip no "<<ystripID[128*pk+ik]<<endl; 
    					}
	    			}	
    			}

			stripmax=0;
			chargemax=0;
			for(itm= hity.begin(); itm!=hity.end();++itm)
			{
				//ycharge += itm->second;
				//ycentroid1 += (itm->first)*(itm->second);
				if((itm->second)>chargemax)
				{
					chargemax=itm->second;
					stripmax=itm->first;
				}
    				//cout<<"y strip id "<<itm->first<<" charge "<<itm->second<<endl;
	    		}
			multiplicity=(int) hity.size();
			if(multiplicity>0)
				h_ymultiplicity[detnumber]->Fill(multiplicity);
			for(itm=hity.begin(); itm!=hity.end();++itm)
			{
				h_ystriphit[detnumber]->Fill(itm->first - stripmax);
			}

			for(itm=hity.begin();itm!=hity.end();++itm)
			{
				//dumy_variable=0;
				int teststrip;
				double testcharge;
				//itm1 = hitx.begin();
				//int ittest = (int) itm;
				//std::advance(itm1,1);
				for(itm1=itm;itm1!=hity.end();++itm1)
				{
					//cout<<"For strip "<<itm->first<<" Inside compare loop strip "<<itm1->first<<" charge "<<itm1->second<<endl; 
					if(abs(itm->first - itm1->first)==32 && (itm1->second > itm->second))
					{
						teststrip = itm1->first;
						testcharge = itm1->second;
						//hitxgated.insert(pair <int, double> (itm1->first,itm1->second));
						//cout<<"1st Cond. variable_dumy "<<dumy_variable<<" map first "<<itm1->first<<" second "<<itm1->second<<endl; 
						break;
					}
					else
					{
						teststrip = itm->first;
						testcharge = itm->second;

						//hitxgated.insert(pair <int, double> (itm->first, itm->second));
						//cout<<"2nd Cond. variable_dumy "<<dumy_variable<<" map first "<<itm->first<<" second "<<itm->second<<endl; 
					}
					//dumy_variable++;
				}

				hitygated.insert(pair <int, double> (teststrip, testcharge));
				//cout<<"size of hitxgated "<<hitxgated.size()<<endl;
			}
			/*
			cout<<"size of hitxgated "<<hitxgated.size()<<endl;
			
			for(itm=hitxgated.begin();itm!=hitxgated.end();++itm)
			{
				cout<<"Event "<<ij<<" hitxgated strip "<<itm->first<<" charge "<<itm->second<<endl;
			}*/
			for(itm=hitygated.begin(); itm!=hitygated.end();++itm)
			{
				h_ystriphitgated[detnumber]->Fill(itm->first - stripmax);
				ycharge +=itm->second; 
				ycentroid1 += (itm->first)*(itm->second);
			}
			multiplicity = (int) hitygated.size();
			if(multiplicity>0)
				h_ymultiplicitygated[detnumber]->Fill(multiplicity);

			if(ycharge>0)
				h_ycharge[detnumber]->Fill(ycharge);
			if(xcharge>0 && ycharge>0)
				h_totcharge[detnumber]->Fill(xcharge+ycharge);
			ycentroid1 = ycentroid1/ycharge;
			if(xcentroid1>0 && ycentroid1>0)
			{
			//	cout<<"Entries  "<<ij<<"  xcentroid "<<xcentroid1<<"  ycentroid "<<ycentroid1<<" xcharge "<<xcharge<<" ycharge "<<ycharge<<endl;
			        hits[detnumber]->Fill(xcentroid1,ycentroid1);
				h_charge[detnumber]->Fill(xcharge,ycharge);
			}
			
			vector<double>().swap(xcmn);
	    		vector<double>().swap(ycmn);
    			vector<double>().swap(xadc_temp);
    			vector<double>().swap(xintegral);
	    		vector<double>().swap(yadc_temp);
    			vector<double>().swap(yintegral);
    			for(map<int,double>::iterator it=hitx.begin();it!=hitx.end();++it)
			{
				hitx.erase(it);
			}
    			for(map<int,double>::iterator it=hity.begin();it!=hity.end();++it)
			{
				hity.erase(it);
			}
    			for(map<int,double>::iterator it=hitxgated.begin();it!=hitxgated.end();++it)
			{
				hitxgated.erase(it);
			}
    			for(map<int,double>::iterator it=hitygated.begin();it!=hitygated.end();++it)
			{
				hitygated.erase(it);
			}
			
			//cout<<endl;
		}
	}
	map<int, TCanvas*> C_qdc;
	for(int ij_qdc=0;ij_qdc<ch_qdc;ij_qdc++)
	{
		C_qdc[ij_qdc] = new TCanvas(Form("QDC_ch%d",ij_qdc),Form("QDC channel %d",ij_qdc),900,600);
		C_qdc[ij_qdc]->Divide(2,1);
		C_qdc[ij_qdc]->cd(1);
		h_lqdc[ij_qdc]->Draw();

		C_qdc[ij_qdc]->cd(2);
		h_hqdc[ij_qdc]->Draw();
	}

	map<int, TCanvas*> C_strip;
	map<int, TCanvas*> C_cmn;
	map<int, TCanvas*> C_hits;
	map<int, TCanvas*> C_charge;
	map<int, TCanvas*> C_multiplicity;
	for(int detnumber=0;detnumber<NDet;detnumber++)
	{
		
		C_strip[detnumber] = new TCanvas(Form("Det_%d_HitStripDistribution",detnumber+1),Form("Det %d Strip Hit distribution",detnumber+1),800,800);
		C_cmn[detnumber] = new TCanvas(Form("Det_%d_CMN",detnumber+1),Form("Det %d CMN",detnumber+1),900,600);
		C_hits[detnumber] = new TCanvas(Form("Det_%d_hits",detnumber+1),Form("Det %d hits",detnumber+1),900,600);
		C_charge[detnumber] = new TCanvas(Form("Det_%d_charge",detnumber+1),Form("Det %d charge",detnumber+1),900,600);
		C_multiplicity[detnumber] = new TCanvas(Form("Det_%d_multiplicity",detnumber+1),Form("Det %d multiplicity",detnumber+1),800,800);
	    	
		C_cmn[detnumber]->Divide(1,(int)((xstrip[detnumber]+ystrip[detnumber])/128.));
    		for(int ip=0;ip<(int)((xstrip[detnumber]+ystrip[detnumber])/128.);ip++)
    		{
    			C_cmn[detnumber]->cd(ip+1);
	    		C_cmn[detnumber]->cd(ip+1)->SetLogy();
    			if(ip<xstrip[detnumber]/128.)
			{
				h_xcmn[detnumber][ip]->SetLineWidth(2);
				h_xcmn[detnumber][ip]->GetXaxis()->SetLabelSize(0.07);
				h_xcmn[detnumber][ip]->GetYaxis()->SetLabelSize(0.07);
				h_xcmn[detnumber][ip]->GetXaxis()->SetTitleSize(0.07);
				h_xcmn[detnumber][ip]->GetXaxis()->SetTitleOffset(0.68);
				h_xcmn[detnumber][ip]->GetXaxis()->SetTitle("common-mode-noise");
				h_xcmn[detnumber][ip]->GetYaxis()->SetTitleSize(0.07);
				h_xcmn[detnumber][ip]->GetYaxis()->SetTitleOffset(0.77);
				h_xcmn[detnumber][ip]->GetYaxis()->SetTitle("Counts");
				h_xcmn[detnumber][ip]->Draw();
			}
			else
			{
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->SetLineWidth(2);
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetXaxis()->SetLabelSize(0.07);
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetYaxis()->SetLabelSize(0.07);
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetXaxis()->SetTitleSize(0.07);
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetXaxis()->SetTitleOffset(0.68);
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetXaxis()->SetTitle("common-mode-noise");
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetYaxis()->SetTitleSize(0.07);
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetYaxis()->SetTitleOffset(0.77);
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->GetYaxis()->SetTitle("Counts");
			
				h_ycmn[detnumber][ip-xstrip[detnumber]/128.]->Draw();
			
			}

	    	}

		C_strip[detnumber]->Divide(1,2);
		C_strip[detnumber]->cd(1);
		C_strip[detnumber]->cd(1)->SetLogy();
		h_xstriphit[detnumber]->GetXaxis()->SetTitle("Xstrip-XstripMax");
		h_xstriphit[detnumber]->GetYaxis()->SetTitle("Count");
		h_xstriphit[detnumber]->Draw();
		h_xstriphitgated[detnumber]->SetLineColor(kRed);
		h_xstriphitgated[detnumber]->Draw("same");

		C_strip[detnumber]->cd(2);
		C_strip[detnumber]->cd(2)->SetLogy();
		h_ystriphit[detnumber]->GetXaxis()->SetTitle("Ystrip-YstripMax");
		h_ystriphit[detnumber]->GetYaxis()->SetTitle("Count");
		h_ystriphit[detnumber]->Draw();
		h_ystriphitgated[detnumber]->SetLineColor(kRed);
		h_ystriphitgated[detnumber]->Draw("same");

		C_hits[detnumber]->cd();
		C_hits[detnumber]->SetLogz();
		hits[detnumber]->GetXaxis()->SetTitle("Xstrip");
		hits[detnumber]->GetYaxis()->SetTitle("Ystrip");
		hits[detnumber]->Draw("COLZ");
		
		C_charge[detnumber]->Divide(2,2);
		C_charge[detnumber]->cd(1);
		h_xcharge[detnumber]->GetXaxis()->SetTitle("XCharge");
		h_xcharge[detnumber]->GetYaxis()->SetTitle("Count");
		h_xcharge[detnumber]->Draw();
		C_charge[detnumber]->cd(2);
		h_ycharge[detnumber]->GetXaxis()->SetTitle("XCharge");
		h_ycharge[detnumber]->GetYaxis()->SetTitle("Count");
		h_ycharge[detnumber]->Draw();
		C_charge[detnumber]->cd(3);
		h_totcharge[detnumber]->GetXaxis()->SetTitle("Toatal Charge");
		h_totcharge[detnumber]->GetYaxis()->SetTitle("Count");
		h_totcharge[detnumber]->Draw();
		C_charge[detnumber]->cd(4);
		C_charge[detnumber]->cd(4)->SetLogz();
		h_charge[detnumber]->GetXaxis()->SetTitle("XCharge");
		h_charge[detnumber]->GetYaxis()->SetTitle("YCharge");
		h_charge[detnumber]->GetYaxis()->SetTitleOffset(1.24);
		h_charge[detnumber]->Draw("COLZ");
		
		C_multiplicity[detnumber]->Divide(1,2);
		C_multiplicity[detnumber]->cd(1);
		C_multiplicity[detnumber]->cd(1)->SetLogy();
		h_xmultiplicity[detnumber]->GetXaxis()->SetTitle("Hit xstrip  multiplicity");
		h_xmultiplicity[detnumber]->GetYaxis()->SetTitle("Count");
		h_xmultiplicity[detnumber]->Draw();
		h_xmultiplicitygated[detnumber]->SetLineColor(kRed);
		h_xmultiplicitygated[detnumber]->Draw("same");
		C_multiplicity[detnumber]->cd(2);
		C_multiplicity[detnumber]->cd(2)->SetLogy();
		h_ymultiplicity[detnumber]->GetXaxis()->SetTitle("Hit ystrip multiplicity");
		h_ymultiplicity[detnumber]->GetYaxis()->SetTitle("Count");
		h_ymultiplicity[detnumber]->Draw();
		h_ymultiplicitygated[detnumber]->SetLineColor(kRed);
		h_ymultiplicitygated[detnumber]->Draw("same");
	}
}
