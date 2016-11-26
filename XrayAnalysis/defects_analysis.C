#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include "TROOT.h"
#include "TH1F.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TF1.h"
struct Pos{
  Pos(): row(-1),col(-1){} 
  Pos(int row_, int col_): row(row_), col(col_) {}
  int row;
  int col;
  bool operator< (const Pos& other ) const{
    if(row != other.row)
      return row < other.row;
    return col < other.col;
  }
};
using PosSet = std::set<Pos>;
using RocDefects = std::map<int,PosSet>;
using boost::property_tree::ptree;
using boost::property_tree::read_json;

RocDefects Parse_XrayFailures(const std::string& xray_failure){
  RocDefects xrayFailures;
  size_t n = 0;
  while(n < xray_failure.size()){
    if (xray_failure[n] != '('){
      n++;
      continue;
    }
    const size_t blockEnd = xray_failure.find(")",n);
    if(blockEnd == std::string::npos) throw std::runtime_error(" Can't Find the ')' ");
    const std::string block = xray_failure.substr(n+1,blockEnd-n -1);
    std::istringstream is(block);
    int Roc_id;
    Pos pos;
    char c;
    is >> Roc_id >> c >> pos.row >> c >> pos.col ;
    if(is.fail())throw std::runtime_error(" Can't parse block ");
    xrayFailures[Roc_id].insert(pos);
    n=blockEnd+1;  
  }
  return xrayFailures;
}
RocDefects Parse_Failures(const std::string& failure){
  RocDefects Failures;
  ptree pt;
  std::istringstream is(failure);
  read_json(is,pt);
  for (ptree::const_iterator it = pt.begin(); it !=pt.end(); ++it)
  {
    std::string Roc_Id = it->first;
    Roc_Id.erase(0,3);
    Pos pos;
    for(const auto& item:it->second){
      auto iter = item.second.begin();
      pos.row =iter ->second.get_value<int>();
      iter++;
      pos.col =iter ->second.get_value<int>();
      Failures[atoi(Roc_Id.c_str())].insert(pos);
    }
  }
  return Failures;
}

void defects_analysis(){
  TFile* f_out = new TFile("defects_analysis_Pisa.root","RECREATE");
  f_out->mkdir("A");
  f_out->mkdir("B");
  
  TH1F* pa_xray_diffH_[16];
  TH2D* pa_xray_diff2DH_[16];
  TH2D* pa_bb_vs_xray2DH_[16];
  TH1F* pa_xray_diffTotal_GradeAH_ = new TH1F("pa_xray_diffTotal_GradeA","pa -Xray (Total)",71,-0.5,70.5);
  TH2D* pa_xray_diffTotal2D_GradeAH_ = new TH2D("pa_xray_diffTotal2D_GradeA","pa -Xray vs pa (Total)",71,-0.5,70.5,71,-0.5,70.5);
  pa_xray_diffTotal2D_GradeAH_->GetXaxis()->SetTitle("Total no. of PA Failures");
  pa_xray_diffTotal2D_GradeAH_->GetYaxis()->SetTitle("No. of PA Failures not in X Ray Faiure");
  TH2D* pa_bb_vs_xrayTotal2D_GradeAH_ = new TH2D("pa_bb_vs_xrayTotal2D_GardeA","pa+bb vs xray (total)",71,-0.5,70.5,71,-0.5,70.5);
  pa_bb_vs_xrayTotal2D_GradeAH_->GetYaxis()->SetTitle("no. of pa+bb  failures");
  pa_bb_vs_xrayTotal2D_GradeAH_->GetXaxis()->SetTitle("no. of xray failures");
  
  TH1F* pa_xray_diffTotal_GradeBH_ = new TH1F("pa_xray_diffTotal_GradeB","pa -Xray (Total)",71,-0.5,70.5);
  TH2D* pa_xray_diffTotal2D_GradeBH_ = new TH2D("pa_xray_diffTotal2D_GradeB","pa -Xray vs pa (Total)",71,-0.5,70.5,71,-0.5,70.5);
  pa_xray_diffTotal2D_GradeBH_->GetXaxis()->SetTitle("Total no. of PA Failures");
  pa_xray_diffTotal2D_GradeBH_->GetYaxis()->SetTitle("No. of PA Failures not in X Ray Faiure");
  TH2D* pa_bb_vs_xrayTotal2D_GradeBH_ = new TH2D("pa_bb_vs_xrayTotal2D_GardeB","pa+bb vs xray (total)",71,-0.5,70.5,71,-0.5,70.5);
  pa_bb_vs_xrayTotal2D_GradeBH_->GetYaxis()->SetTitle("no. of pa+bb  failures");
  pa_bb_vs_xrayTotal2D_GradeBH_->GetXaxis()->SetTitle("no. of xray failures");
  
  for(int i=0;i<16;i++){
    std::ostringstream convert;
    convert << i;
    std::string b = convert.str();
    std::string diff_histo_name = "paxray_diff_" + b;
    std::string diff_histo_name_2D = diff_histo_name + "_2D";
    std::string diff_histo_title = "pa - Xray in RocId" + b;
    std::string diff_histo_title_2D = "pa -Xray vs pa in RocId" + b;
    pa_xray_diffH_[i] = new TH1F(diff_histo_name.c_str() , diff_histo_title.c_str(),71,-0.5,70.5);
    pa_xray_diff2DH_[i] = new TH2D(diff_histo_name_2D.c_str() , diff_histo_title_2D.c_str(),71,-0.5,70.5,71,-0.5,70.5);
    pa_xray_diff2DH_[i]->GetXaxis()->SetTitle("Total no. of PA Failures");
    pa_xray_diff2DH_[i]->GetYaxis()->SetTitle("No. of PA Failures not in X Ray Faiure");

    std::string vs_histo_name_2D = "pa_bb_vs_xray_"+ b +"_2D";
    std::string vs_histo_title_2D = "pa+bb vs xray";
    pa_bb_vs_xray2DH_[i] = new TH2D(vs_histo_name_2D.c_str(),vs_histo_title_2D.c_str(),71,-0.5,70.5,71,-0.5,70.5);
    pa_bb_vs_xray2DH_[i]->GetYaxis()->SetTitle("no. of pa+bb  failures");
    pa_bb_vs_xray2DH_[i]->GetXaxis()->SetTitle("no. of xray failures");

  }
  std::ifstream in_file("defects.txt");
  std::string line;
  int line_no=1;
  if(in_file.is_open()){ 
    while(getline(in_file,line)){
      //if(line_no >1 ) break;
      //std::cout<<" Reading line no"<<line_no<<std::endl;
      line_no++;
      std::vector<std::string> columns;
      boost::trim_if(line, boost::is_any_of(" \t"));
      boost::split(columns, line, boost::is_any_of("\t"), boost::token_compress_off);
      if(columns.size() != 8) {
	std::cerr<<" Bad No. Columns "<<columns.size()<<std::endl;
	continue;
      }
      const std::string& center_bare = columns.at(0);
      if(center_bare != "Pisa") continue;
      const std::string& center_xray = columns.at(1);
      if(center_xray != "PERUGIA") continue;
      const std::string& full_module_id = columns.at(2);
      const std::string& bare_module_id = columns.at(3);
      const std::string& final_grade = columns.at(4);
      if(final_grade != "A" && final_grade != "B") continue;
      std::string xray_failures = columns.at(5);
      std::string bb_failures = columns.at(6);
      std::string pa_failures = columns.at(7); 
      size_t found_1 = xray_failures.find("{");
      size_t found_2 = xray_failures.find("}",found_1);
      if(found_1 == std::string::npos || found_2 ==std::string::npos){ 
	std::cerr<<" Can't Find Xray Failure Block "<<std::endl;;
	continue;
      }
      xray_failures = xray_failures.substr(found_1+1,found_2-found_1-1);
      found_1 = bb_failures.find("{");
      found_2 = bb_failures.find("}",found_1);
      if(found_1 == std::string::npos || found_2 ==std::string::npos){
	std::cerr<<" Can't Find BB Failure Block "<<std::endl;
	continue;
      }
      bb_failures = bb_failures.substr(found_1,found_2-found_1+1);
      found_1 = pa_failures.find("{");
      found_2 = pa_failures.find("}",found_1);
      if(found_1 == std::string::npos || found_2 ==std::string::npos){
	std::cerr<<" Can't Find PA Failure Block "<<std::endl;
	continue;
      }
      pa_failures = pa_failures.substr(found_1,found_2-found_1+1);

      //std::cout<<"All blocks found"<<std::endl;

      //finding x_ray failures
      const RocDefects xrayFailures = Parse_XrayFailures(xray_failures);
      //std::cout<<"x ray Map created"<<std::endl;
    
      //finding bb_failures
      RocDefects bbFailures = Parse_Failures(bb_failures);
      //std::cout<<"bb Map created"<<std::endl;

      //finding pa_failures
      RocDefects paFailures = Parse_Failures(pa_failures);
      //std::cout<<"pa Map created"<<std::endl;
    
      for(const auto& entry:paFailures){
	int Roc_id =entry.first;
	const PosSet& pa_set = entry.second;
	if(!xrayFailures.count(Roc_id)){
	  pa_xray_diffH_[Roc_id]->Fill(pa_set.size());
	  pa_xray_diff2DH_[Roc_id]->Fill(pa_set.size(),pa_set.size());
	  
	  if(final_grade == "A"){
	    pa_xray_diffTotal_GradeAH_->Fill(pa_set.size());
	    pa_xray_diffTotal2D_GradeAH_->Fill(pa_set.size(),pa_set.size());
	  }
	  else if(final_grade == "B"){
	    pa_xray_diffTotal_GradeBH_->Fill(pa_set.size());
	    pa_xray_diffTotal2D_GradeBH_->Fill(pa_set.size(),pa_set.size());
	  }
	  continue;
        }
	const PosSet& xray_set = xrayFailures.at(Roc_id);
	std::vector<Pos> v(pa_set.size());                      // 0  0  0  0  0  0  0  0  0  0
	std::vector<Pos>::iterator it;
	it = std::set_difference(pa_set.begin(),pa_set.end(),xray_set.begin(),xray_set.end(),v.begin());
	v.resize(it-v.begin());
	if(v.size()>0) std::cout<<" Module Id = "<<full_module_id<<" Bare Module Id = "<<bare_module_id<<" ROC ID = "<<Roc_id<<" No. of Pa Failure = "<<pa_set.size()<<" No. of Xray Failure = "<<xray_set.size()<<" No. of difference = "<<v.size()<<std::endl;
	pa_xray_diffH_[Roc_id]->Fill(v.size());
	pa_xray_diff2DH_[Roc_id]->Fill(pa_set.size(),v.size());

	if(final_grade == "A"){
	  pa_xray_diffTotal_GradeAH_->Fill(v.size());
	  pa_xray_diffTotal2D_GradeAH_->Fill(pa_set.size(),v.size());
	}
        else if(final_grade == "B"){
	  pa_xray_diffTotal_GradeBH_->Fill(v.size());
	  pa_xray_diffTotal2D_GradeBH_->Fill(pa_set.size(),v.size());
	}
      }
      for(int i=0;i<16;i++){
	int nXrayFailure;
        PosSet bareFailures;
	if(paFailures.count(i))
	  bareFailures.insert(paFailures.at(i).begin(),paFailures.at(i).end());
	nXrayFailure = xrayFailures.count(i) ? xrayFailures.at(i).size() : 0;
	if(bbFailures.count(i))
	  bareFailures.insert(bbFailures.at(i).begin(),bbFailures.at(i).end());
	pa_bb_vs_xray2DH_[i]->Fill(nXrayFailure,bareFailures.size());
	if(final_grade == "A") pa_bb_vs_xrayTotal2D_GradeAH_->Fill(nXrayFailure,bareFailures.size());
	else if(final_grade == "B") pa_bb_vs_xrayTotal2D_GradeBH_->Fill(nXrayFailure,bareFailures.size()); 
      }
    }
  }
  f_out->cd();
  for(int i=0;i<16;i++){
    pa_xray_diffH_[i]->Write();
    pa_xray_diff2DH_[i]->Write();
    pa_bb_vs_xray2DH_[i]->Write();
  }

  f_out->cd("A");
  pa_xray_diffTotal_GradeAH_->Write();
  pa_xray_diffTotal2D_GradeAH_->Write();
  pa_bb_vs_xrayTotal2D_GradeAH_->Write();

  f_out->cd("B");
  pa_xray_diffTotal_GradeBH_->Write();
  pa_xray_diffTotal2D_GradeBH_->Write();
  pa_bb_vs_xrayTotal2D_GradeBH_->Write();
  
  f_out->Close();
}
int main(){
  defects_analysis();
  return 0;
}
