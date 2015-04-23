/*!
 * \file prod_file_DB.cpp
 * \brief Prepare wafer test file to be uploaded to the database.
 * \author Valeria Botta (University of Pisa, INFN Pisa)
 *
 * Copyright 2015 Valeria Botta
 *
 * This file is part of PixelTestTools.
 *
 * PixelTestTools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * PixelTestTools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PixelTestTools.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <limits>


using namespace std;

void produci_file_info_n (string, string, string, vector<double>, vector<double>, float, int, string, bool, string );
void prod_file_DB (string, string, bool, bool, bool, string);

int main (int argc, char *argv[]) {
	
	if (argc!=6) {
		cout << "ERROR: Wrong number of input arguments. " << endl;
		cout << "Please enter BATCH_ID WAFER_ID SENSOR1_flag SENSOR2_flag SENSOR3_flag " << endl;
		cout << "Dummy example: 112233 00 true false true" << endl;
		cout << "Exiting without doing any action." << endl;
		return 1; 
	}

	
	string batchID = (string) argv[1];	
	string waferID = (string) argv[2];

	string sensor1 = (string) argv[3];
	bool isSensor1=false;
	if (sensor1=="true" || sensor1== "t") isSensor1 = true;

	string sensor2 = (string) argv[4];
	bool isSensor2=false;
	if (sensor2=="true" || sensor2== "t") isSensor2 = true;

	string sensor3 = (string) argv[5];
	bool isSensor3=false;
	if (sensor3=="true" || sensor3== "t" ) isSensor3 = true;
	

	cout << "isSensor1 = " << isSensor1 <<endl;
        cout << "isSensor2 = " << isSensor2 <<endl;
	cout << "isSensor3 = " << isSensor3 <<endl;

	string getDateNow = "date +%s >> timestamp.txt";
	int getDateNow_err = system(getDateNow.c_str());
	
	ifstream file_timestamp ("timestamp.txt");
	//file_timestamp = fopen("timestamp.txt","r");

	string timestamp;

	if (file_timestamp.is_open()) {

		getline (file_timestamp, timestamp); //cout << "timestamp from file : " << timestamp << endl;

		file_timestamp.close();
	}

	cout << "timestamp from file : " << timestamp << endl;
	
	//remove timestamp.txt
	string rm_timestamp = "rm timestamp.txt";
	int rm_timestamp_err = system(rm_timestamp.c_str());

	prod_file_DB(batchID, waferID, isSensor1, isSensor2, isSensor3, timestamp);


    //prod_file_DB(batchID, waferID, isSensor1, isSensor2, isSensor3);
	return 0;
} //end of main



void prod_file_DB  (string batch, string wafer, bool flag1, bool flag2, bool flag3, string TIMESTAMP ) {
  
  string nome_filein = batch+"-"+wafer;   // assemble input file name

// DATA AND DB FOLDER PATHS HERE!!!!!!!!!!
//  string nome_dataFolder = "/home/pixel/workspace/sensorIV/pixel-production-phase-I-transfer/"+batch+"/"+batch+"-"+wafer;
//  string nome_DBfolder = "/home/pixel/workspace/sensorIV/toDataBase";

  string nome_dataFolder = "/home/valeria/pixel/misure_lab/sim_folderStructure/uploadCode/pixel-production-phase-I-transfer/"+batch+"/"+batch+"-"+wafer;
  string nome_DBfolder = "/home/valeria/pixel/misure_lab/sim_folderStructure/uploadCode/toDataBase";
 	 


  if (flag1) nome_filein = nome_filein+"-01";
  if (flag2) nome_filein = nome_filein+"-02";
  if (flag3) nome_filein = nome_filein+"-03";

  nome_filein = nome_dataFolder+"/"+nome_filein+"-IV.dat";

  cout << "nome file input = " << nome_filein << endl;

  ifstream filein;

  filein.open (nome_filein.c_str());
 
  string line, a_line;
  string line_meas;

  vector<double> mis_100V, mis_150V;   //vectors to store the 100 and 150V lines, needed in the .inf file


  float temperature = -1.1; // other stuff for the .inf file
  int Hum = -1;
  string Date; //Date format: elapsed seconds from 1/1/1970
  cout << "Date after declaration: Date = " << Date << endl;
  bool foundDate = false;  // if no "Date" line is found in the input file, the date will be set to current date.


  filein.close();
     

  ofstream fileout01, fileout02, fileout03;   // output files to store IV measurements (.tab.txt)

  filein.open (nome_filein.c_str());

  bool isOpenFileIn=false;	   
 
  if (filein.is_open())
  {
    isOpenFileIn=true;

    if (flag1) { // output files to store IV measurements (.tab.txt)
    cout << "Crosscheck Date: '" << Date << "'.\n";
    string nome_fileout = "S"+batch+"-"+wafer+"-1"+"_new_"+TIMESTAMP+".tab.txt";
    fileout01.open(nome_fileout.c_str());
    cout << "nome file out : " << nome_fileout << endl;
  
    }

    if (flag2) {
  
    string nome_fileout = "S"+batch+"-"+wafer+"-2"+"_new_"+TIMESTAMP+".tab.txt";
    fileout02.open(nome_fileout.c_str());

    cout << "nome file out : " << nome_fileout << endl;
   
    }

    if (flag3) {

    string nome_fileout = "S"+batch+"-"+wafer+"-3"+"_new_"+TIMESTAMP+".tab.txt";
    fileout03.open(nome_fileout.c_str());

    cout << "nome file out : " << nome_fileout << endl;
    
    }


    int n_line=0;

    while (!filein.eof()) {
    
    getline (filein,line);

    n_line =  n_line +1;
    //cout << "n_line: " << n_line << endl; 

    // extracting lines containing Temperature, Humidity

    if ( line.find("Temperature") !=std::string::npos ) {
      
      string interesting_line = line;
      string sbuf;
      vector<string> Temper_line;
      stringstream sstr(interesting_line);

      while (sstr >> sbuf) 
	Temper_line.push_back(sbuf);
    
      temperature = (strtof(Temper_line[1].c_str(), NULL));
  
    }

    if ( line.find("Humidity") !=std::string::npos ) {
      
      string interesting_line = line;
      string sbuf;
      vector<string> Hum_line;
      stringstream sstr(interesting_line);

      while (sstr >> sbuf) 
	Hum_line.push_back(sbuf);
    
      Hum = (int) (strtof(Hum_line[1].c_str(), NULL));
  
    }

    static const string date_label = "Date";

    if ( line.find(date_label) != std::string::npos ) {
      foundDate = true;
      const size_t pos = line.find(date_label) + date_label.size() + 1;
      const string date_string = line.substr(pos);
      tm time_descriptor;
      cout << "Date string: '" << date_string << "'.\n"; 
      strptime(date_string.c_str(), "%F %T", &time_descriptor);
      const time_t posix_seconds = mktime(&time_descriptor);
      ostringstream s_posix_time;
      s_posix_time << posix_seconds;
      Date = s_posix_time.str();
      cout << "Date: '" << Date << "'.\n"; 
    }
    


    if (n_line>2 && n_line<124) { //selection of lines containing IV values (line_meas), always lines number 3 to 123
      
        string buf;  	       //Buffer string
	vector<string> misure; // Create vector to hold words

	line_meas = line;
	
	stringstream ss(line_meas); // Insert the string into a stream

		
	while (ss >> buf)
	  misure.push_back(buf);


	double iv_01 [2], iv_02 [2], iv_03 [2];


	iv_01[0]= (strtod(misure[0].c_str(), NULL)); //First coloumn is always Voltage, same for all three sensors
	iv_02[0] = iv_01[0];
	iv_03[0] = iv_01[0];

	if ( misure[1] != "NaN") { 

	     iv_01 [1]= (strtod(misure[1].c_str(), NULL));
	     iv_02 [1]= (strtod(misure[2].c_str(), NULL));
	     iv_03 [1]= (strtod(misure[3].c_str(), NULL));
	    
	     
	}

	else { // compliance current is 100 uA 
	  
	    iv_01 [1]=  (-100.0E-6);
	    iv_02 [1]=  (-100.0E-6);
	    iv_03 [1]=  (-100.0E-6);
	}

	iv_01[1]= - iv_01 [1]; //store currents with positive sign, not necessary for Database requirements
	iv_02[1]= - iv_02 [1];
	iv_03[1]= - iv_03 [1];


	if ( iv_01[0] == 100 ) { // hold 100 and 150 V IV measurements to be used later for grading and .inf file 
						
	                         mis_100V.push_back(iv_01[0]); // stored in a vector of double, eg: mis_100V = (100,i1,i2,i3)
				 mis_100V.push_back(iv_01[1]);
				 mis_100V.push_back(iv_02[1]);
				 mis_100V.push_back(iv_03[1]);
	
		    }

	if ( iv_01[0] == 150 ) { 

	                         mis_150V.push_back(iv_01[0]); // same for 150 V measurement
				 mis_150V.push_back(iv_01[1]);
				 mis_150V.push_back(iv_02[1]);
				 mis_150V.push_back(iv_03[1]);
	
		    }


	// write .tab.txt files containing IV data for the selected sensors (check input flags)

	if (flag1) { 

	     if (fileout01.is_open())  {
	                                fileout01 << iv_01[0] <<"\t" << iv_01[1] << "\n";
	     }
	     else cout << "Unable to open file 01"<< endl;
	}

	if (flag2) {
	     if (fileout02.is_open())  {
	                                 fileout02 << iv_02[0] <<"\t" << iv_02[1] << "\n";
	     }
	     else cout << "Unable to open file 02"<< endl;
	     
	}     
	
	if (flag3) {
	     if (fileout03.is_open())  {
	                                 fileout03 << iv_03[0] <<"\t" << iv_03[1] << "\n";
	     }
	     else cout << "Unable to open file 03" << endl;
	     
	}     
	    
     } //end of measurement lines loop (line 3 to 123)
  
    
    } //end of while loop on input file lines
 
 
    // creating info file (.inf.txt) for selected sensors 

    if (flag1) { 
      
      string sensor = "1";
      produci_file_info_n(batch, wafer, sensor, mis_100V, mis_150V, temperature, Hum, Date, foundDate, TIMESTAMP);

      }
    

    if (flag2) {
      string sensor = "2";
      produci_file_info_n( batch, wafer, sensor, mis_100V, mis_150V, temperature, Hum, Date, foundDate, TIMESTAMP);
    }

    if (flag3) {
      string sensor = "3";
      produci_file_info_n(batch, wafer, sensor, mis_100V, mis_150V, temperature, Hum, Date, foundDate, TIMESTAMP);
    }

	
  filein.close(); // Close all open files
  if (fileout01.is_open()) fileout01.close();
  if (fileout02.is_open()) fileout02.close();
  if (fileout03.is_open()) fileout03.close();

  }
    
  
  else cout << "Unable to open input file" << endl; 


  // Create folder to be filled with .inf and .tab files
  // One folder for each sensor, naming convention is : SensorTest_S280253-16-03_new_01.tar.gz
  // Compressed as tar archive and moved to the folder containing data to be uploaded in the DB
  
  if (isOpenFileIn) {

  if (flag1) {

    string OutFolder = "SensorTest_S"+batch+"-"+wafer+"-1"+"_new_"+TIMESTAMP;

    string makeOutFolder = "mkdir -p "+ OutFolder; // -p option to overwrite the folder when it already exists 
    int makeOutFolder_err = system(makeOutFolder.c_str());

    string nome_fileout = "S"+batch+"-"+wafer+"-1"+"_new_"+TIMESTAMP+".tab.txt"; 
    string moveFileOut = "mv "+nome_fileout+" "+OutFolder;
    int moveFileOut_err = system(moveFileOut.c_str());

    string nome_fileinf = "S"+batch+"-"+wafer+"-1"+"_new_"+TIMESTAMP+".inf.txt";
    string moveFile_inf = "mv "+nome_fileinf+" "+OutFolder;
    int moveFile_inf_err = system(moveFile_inf.c_str());

    string create_tar_arch = "tar -zcf "+OutFolder+".tar.gz "+ OutFolder;
    int create_tar_arch_err= system(create_tar_arch.c_str());

    string move_arch = "mv "+OutFolder+".tar.gz "+nome_DBfolder;
    int move_arch_err = system(move_arch.c_str());

    if (move_arch_err==0) { // if moving the archive folder was successful, remove it from current folder 
			string rm_folder = "rm -r "+OutFolder;
			int rm_folder_err = system(rm_folder.c_str());
			} 
  }

  if (flag2) {
 
    string OutFolder = "SensorTest_S"+batch+"-"+wafer+"-2"+"_new_"+TIMESTAMP;

    string makeOutFolder = "mkdir -p "+ OutFolder;
    int makeOutFolder_err = system(makeOutFolder.c_str());

    string nome_fileout = "S"+batch+"-"+wafer+"-2"+"_new_"+TIMESTAMP+".tab.txt"; 
    string moveFileOut = "mv "+nome_fileout+" "+OutFolder;
    int moveFileOut_err = system(moveFileOut.c_str());

    string nome_fileinf = "S"+batch+"-"+wafer+"-2"+"_new_"+TIMESTAMP+".inf.txt";
    string moveFile_inf = "mv "+nome_fileinf+" "+OutFolder;
    int moveFile_inf_err = system(moveFile_inf.c_str());

    string create_tar_arch = "tar -zcf "+OutFolder+".tar.gz "+ OutFolder;
    int create_tar_arch_err= system(create_tar_arch.c_str());

    string move_arch = "mv "+OutFolder+".tar.gz "+nome_DBfolder;
    int move_arch_err = system(move_arch.c_str());

    if (move_arch_err==0) {
			string rm_folder = "rm -r "+OutFolder;
			int rm_folder_err = system(rm_folder.c_str());
			} 
  }

  if (flag3) {

    string OutFolder = "SensorTest_S"+batch+"-"+wafer+"-3"+"_new_"+TIMESTAMP;
    
    string makeOutFolder = "mkdir -p "+ OutFolder;
    int makeOutFolder_err = system(makeOutFolder.c_str());

    string nome_fileout = "S"+batch+"-"+wafer+"-3"+"_new_"+TIMESTAMP+".tab.txt";
    string moveFileOut = "mv "+nome_fileout+" "+OutFolder;
    int moveFileOut_err = system(moveFileOut.c_str());


    string nome_fileinf = "S"+batch+"-"+wafer+"-3"+"_new_"+TIMESTAMP+".inf.txt";
    string moveFile_inf = "mv "+nome_fileinf+" "+OutFolder;
    int moveFile_inf_err = system(moveFile_inf.c_str());

    string create_tar_arch = "tar -zcf "+OutFolder+".tar.gz "+ OutFolder;
    int create_tar_arch_err= system(create_tar_arch.c_str());

    string move_arch = "mv "+OutFolder+".tar.gz "+nome_DBfolder;
    int move_arch_err = system(move_arch.c_str());

    if (move_arch_err==0) {
			string rm_folder = "rm -r "+OutFolder;
			int rm_folder_err = system(rm_folder.c_str());
			} 


  }

  }

} // End of function prod_DB



// Function to create the info file

void produci_file_info_n( string Nbatch, string Nwafer, string Nsensor, vector<double> iv_100, vector<double> iv_150, float temp, int humi, string date, bool thereisDate, string processDate) {





  string fileout_name = "S"+Nbatch+"-"+Nwafer+"-"+Nsensor+"_new_"+processDate+".inf.txt";
 
  FILE * file_commenti;
  file_commenti = fopen(fileout_name.c_str(),"w");

  // fixed lines
  
  fprintf(file_commenti, "CENTRE\t PISA \n");
  fprintf(file_commenti, "STEP\t NEW \n");

  fprintf(file_commenti, "BATCH\t %s \n", Nbatch.c_str());
  fprintf(file_commenti, "WAFER\t %s \n", Nwafer.c_str());
  fprintf(file_commenti, "SENSOR\t %s \n",Nsensor.c_str());

  unsigned int col = 0; // Select the right coloumn containing the current measurement of this sensor
  if ( Nsensor == "1" ) col =1;
  if ( Nsensor == "2" ) col =2;
  if ( Nsensor == "3" ) col =3;

  fprintf(file_commenti, "V1\t %.2f \n", iv_100[0]);
  fprintf(file_commenti, "V2\t %.2f \n", iv_150[0]);

  fprintf(file_commenti, "I1\t %E \n", iv_100[col]);
  fprintf(file_commenti, "I2\t %E \n", iv_150[col]);


  double slope =  iv_150[col] / iv_100[col] ; // Slope := i(at 150V)/i(at 100V)
  fprintf (file_commenti, "SLOPE\t %.2f \n", slope);
 
 fprintf (file_commenti, "TEMPERATURE\t %.1f \n", temp);


  // GRADE ASSIGNMENT :

  	// I(-150V) < 2uA: Grade 1, otherwise Grade 0
  	// slope I(-150V)/I(-100V) < 2: Grade 1, otherwise Grade 0. 
  	// Final Grade is AND of 1 and 2. 
	// Currents values at 20C - need to rescale from room temperature to 20C

  float E_g = 1.12; //Silicon energy gap at room temperature [eV]
  float kB = 8.6173324*pow(10,-5); // Boltzmann constant [eV/K]
  float T_17 = 20  +273.15; // changed to 20C 
  float T_amb = temp+273.15; // Room temperature [k]
  float i_20 = iv_150[col];
  float espon = (-E_g/(2*kB))*( (1/T_17) - (1/T_amb) );  
  i_20 = i_20 *pow((T_17 / T_amb),1.5); 
  i_20 = i_20 *exp(espon);
 // i_20 = - i_20; //reverse current sign, that is always negative

  string grade;
  cout << " SUMMARY INF FILE " << endl;
  cout << "i_20 =" <<  i_20 << "\tSlope = " << slope << endl;

  if (i_20 < 0.000002 && slope < 2) grade = "1";
  else grade = "0";
  
  cout << "Grade : " << grade << endl;
  fprintf(file_commenti, "GRADE\t %s \n", grade.c_str());
     
  // Write comments
  fprintf(file_commenti,"COMMENT\t Two probes, both on p-side. Waiting time: 3s. Humidity  %i \n" ,humi);

  // Date 

  fprintf(file_commenti, "DATE \t");
  if (thereisDate) fprintf(file_commenti, " %s \n", date.c_str()); 


  else 	{

	cout << "No Date info in measurement file." << endl;
	
	}

  fclose(file_commenti);
 
} //end of this function 
