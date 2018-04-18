#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <iomanip>

using namespace std;

std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();
    
    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.
    
    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();
    
    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
            case '\n':
                return is;
            case '\r':
                if(sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case EOF:
                // Also handle the case when the last line has no line ending
                if(t.empty())
                    is.setstate(std::ios::eofbit);
                return is;
            default:
                t += (char)c;
        }
    }
}

int main(){
    cout << fixed;
    cout.precision(2);
    
    ifstream unFile, inFile, KunFile, KarFile, KpaFile;
    string name;
    
    cout << "Enter patient name: ";
    cin >> name;
    
    //////// Unenhanced
    string un_name = "Ao_Unenhanced_" + name;
    unFile.open(un_name);
    string k;
    int number_of_scan_un = 0;
    while(!safeGetline(unFile, k).eof()) {
        number_of_scan_un ++;
    }
    unFile.clear();
    unFile.seekg(0);
    
    double unenhanced[number_of_scan_un];
    double un_sum = 0;
    double densityUn;
    for (int i=0; i < number_of_scan_un; i++) {
        unFile >> unenhanced[i];
        un_sum = un_sum + unenhanced[i];
    }
    densityUn = un_sum / number_of_scan_un;
    //cout << "Mean unenhanced density: " << densityUn <<"\n";
    
    /////////////
    
    string en_name = "Ao_Enhanced_" + name;
    inFile.open(en_name);
    
    string s;
    int number_of_scan = 0;
    while(!safeGetline(inFile, s).eof()) { // count number of scans
        number_of_scan ++;
    }
    inFile.clear();
    inFile.seekg(0);
    
    int number_of_data = number_of_scan + 1;
    
    double rawdata[number_of_data][2];
    for (int i=1; i < number_of_data; i++) {
        inFile >> rawdata[i][0] >> rawdata[i][1];
    }
    
    
    double data[number_of_data][2];
    data[0][0] = 0;
    data[0][1] = 0;
    
    for (int i=1; i < number_of_data; i++) {
        data[i][0] = rawdata[i][0];
        data[i][1] = rawdata[i][1] - densityUn;
    }
    
    
    cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\ \n \n";
    ///////////////////////////
    int count=1;
    for (int i=0; i < number_of_data-1; i++) { // count number of different scan times
        if (data[i][0] != data[i+1][0]) {
            count++;
        }
    }
    double time[count];
    int j=1;
    time[0] = data[0][0];
    for (int i=0; i < number_of_data-1; i++) { // take different scan times
        if (data[i][0] != data[i+1][0]) {
            time[j] = data[i+1][0];
            j++;
        }
    }
    
    cout << "\n \n";
    
    /////////////////////
    double density[count];
    
    density[0] = data[0][1];
    
    double sum[count];
    double dem[count];
    
    for (int i=1; i < count; i++) {
        sum[i]=0;
        dem[i]=0;
        for (int k=1; k < number_of_data; k++) { // take avarage density of scans having same times
            if (data[k][0] == time[i]) {
                sum[i] += data[k][1];
                dem[i] ++;
            }
        }
        density[i] = sum[i] / dem[i];
        //cout << time[i] << " | " << density[i]  << "\n";

    }
    
    cout << "Processed data: \n";
    cout << "Time | Mean density \n";
    for (int i=0; i < count; i++) {
        cout << time[i] << " | " << density[i]  << "\n";
    }
    cout << "\n";
    
    
    //////////////////// Calculate integral
    double ArBgn, ArEnd, PaBgn, PaEnd; // Beginning and Ending times of each kidney scan.
    int ArBgnIndex = 0, ArEndIndex = 0, PaBgnIndex = 0, PaEndIndex = 0;
    
    cout << "Time of first Arterial phase scan: ";
    cin >> ArBgn;
    cout << "Time of last Arterial phase scan: ";
    cin >> ArEnd;
    cout << "Time of first Parenchymal phase scan: ";
    cin >> PaBgn;
    cout << "Time of last Parenchymal phase scan: ";
    cin >> PaEnd;
    
    for (int i=0; i < count; i++){ // take index of scan times.
        if (time[i] == ArBgn) {
            ArBgnIndex = i;
        }
        else if (time[i] == ArEnd) {
            ArEndIndex = i;
        }
        else if (time[i] == PaBgn) {
            PaBgnIndex = i;
        }
        else if (time[i] == PaEnd) {
            PaEndIndex = i;
        }
    }
    
    ////////////// Take mean density of each kidney scan
    double ArIntDens=0, PaIntDens=0, sum1=0, sum2=0, count1=0, count2=0, intsum1=0, intsum2=0;
    double timeNew[count], densityNew[count];
    
    for (int i=0; i < count; i++){
        if ( (time[i] < ArBgn) || ((time[i] > ArEnd) && (time[i] < PaBgn)) || (time[i] > PaEnd) ) {
            timeNew[i] = time[i];
        }
        else if ((time[i] >= ArBgn) && (time[i] <= ArEnd)) {
            sum1 += density[i];
            count1++;
            timeNew[i] = time[i];
        }
        else if ((time[i] >= PaBgn) && (time[i] <= PaEnd)) {
            sum2 += density[i];
            count2++;
            timeNew[i] = time[i];
        }
    }
    ArIntDens = sum1 / count1;
    PaIntDens = sum2 / count2;
    
    //////////////
    for (int i=0; i < count; i++){
        if ( ((time[i] < ArBgn) || (time[i] > ArEnd)) && ((time[i] < PaBgn) || (time[i] > PaEnd)) ) {
            densityNew[i] = density[i];
        }
        else if ((time[i] >= ArBgn) && (time[i] <= ArEnd)) {
            densityNew[i] = ArIntDens;
        }
        else if ((time[i] >= PaBgn) && (time[i] <= PaEnd)) {
            densityNew[i] = PaIntDens;
        }
    }
    
    
    ///////////////// Linear interpolation
    double a[count-1], b[count-1];
    for (int i = 0; i < count-1; i++){
        a[i] = (densityNew[i+1] - densityNew[i]) / (timeNew[i+1] - timeNew[i]);
        b[i] = -a[i] * timeNew[i] + densityNew[i];
    }
    
    cout << "\n";
    cout << "Linear interpolation: \n";
    cout << "b(t) = \n";
    for (int i = 0; i < ArBgnIndex; i++){
        cout << a[i] << "t + " << b[i] << " if " << timeNew[i] << " <= t < " << timeNew[i+1] << "\n";
    }
    cout << ArIntDens << " if " << ArBgn << " <= t < " << ArEnd << "\n";
    for (int i = ArEndIndex; i < PaBgnIndex; i++){
        cout << a[i] << "t + " << b[i] << " if " << timeNew[i] << " <= t < " << timeNew[i+1] << "\n";
    }
    cout << PaIntDens << " if " << PaBgn << " <= t < " << PaEnd << "\n";
    for (int i = PaEndIndex; i < count-1; i++){
        cout << a[i] << "t + " << b[i] << " if " << timeNew[i] << " <= t < " << timeNew[i+1] << "\n";
    }
    
    double t1 = (ArBgn + ArEnd) / 2, t2 = (PaBgn + PaEnd) / 2; // define t1, t2
    
    cout << "\n";
    cout << "t1 = " << t1 << "\n";
    cout << "t2 = " << t2 << "\n \n";
    cout << "b(t1) = " << ArIntDens << "\n";
    cout << "b(t2) = " << PaIntDens << "\n \n";
    
    /////// Calculate integral from t=0 to t=t1
    for (int i = 0; i < ArBgnIndex; i++){
        intsum1 += (densityNew[i] + densityNew[i+1]) * (timeNew[i+1] - timeNew[i]) * 0.5;
    }
    intsum1 = intsum1 + (t1 - ArBgn)*ArIntDens;
    cout << "The integral of b(t) from t0 to t1: " << intsum1 << "\n";
    
    /////// Calculate integral from t=0 to t=t2
    for (int i = ArEndIndex; i < PaBgnIndex; i++){
        intsum2 += (densityNew[i] + densityNew[i+1]) * (timeNew[i+1] - timeNew[i]) * 0.5;
    }
    intsum2 = intsum2 + (ArEnd - t1)*ArIntDens + (t2 - PaBgn)*PaIntDens + intsum1;
    cout << "The integral of b(t) from t0 to t2: " << intsum2 << "\n";
    cout << "The integral of b(t) from t1 to t2: " << intsum2 - intsum1 << "\n \n";
    
    
    ////////// K(t)
    double thickness;
    
    cout << "Slice thickness: ";
    cin >> thickness;
    
    //////// K unenhanced
    string name1 = "Kunenhanced_" + name;
    KunFile.open(name1);
    string s1;
    int number_of_scan1 = 0;
    while(!safeGetline(KunFile, s1).eof()) {
        number_of_scan1 ++;
    }
    KunFile.clear();
    KunFile.seekg(0);
    
    
    double rawdata1[number_of_scan1][2]; // rawdata1[i][0]: density; rawdata[i][1]: area
    for (int i=0; i < number_of_scan1; i++) {
        KunFile >> rawdata1[i][0] >> rawdata1[i][1];
    }
    
    double KUn[number_of_scan1], KUnSum=0;
    
    for (int i=0; i < number_of_scan1; i++) {
        KUn[i] = rawdata1[i][0] * rawdata1[i][1] * thickness;
    }
    
    cout << "K unenhanced \n";
    for (int i=0; i < number_of_scan1; i++) {
        cout << KUn[i] << "\n";
    }
    
    for (int i=0; i < number_of_scan1; i++) {
        KUnSum = KUnSum + KUn[i];
    }
    
    
    //////// K arterial
    string name2 = "Karterial_" + name;
    KarFile.open(name2);
    string s2;
    int number_of_scan2 = 0;
    while(!safeGetline(KarFile, s2).eof()) {
        number_of_scan2 ++;
    }
    KarFile.clear();
    KarFile.seekg(0);
    
    double rawdata2[number_of_scan2][2];
    for (int i=0; i < number_of_scan2; i++) {
        KarFile >> rawdata2[i][0] >> rawdata2[i][1];
    }
    
    double KAr[number_of_scan2], KArSum=0;
    
    for (int i=0; i < number_of_scan2; i++) {
        KAr[i] = rawdata2[i][0] * rawdata2[i][1] * thickness;
    }
    
    cout << "\n";
    cout << "K Arterial \n";
    for (int i=0; i < number_of_scan2; i++) {
        cout << KAr[i] << "\n";
    }
    
    for (int i=0; i < number_of_scan2; i++) {
        KArSum = KArSum + KAr[i]; // volumn data
    }
    
    
    //////// K Parenchymal
    string name3 = "Kparenchymal_" + name;
    KpaFile.open(name3);
    string s3;
    int number_of_scan3 = 0;
    while(!safeGetline(KpaFile, s3).eof()) { // count number of scans
        number_of_scan3 ++;
    }
    KpaFile.clear();
    KpaFile.seekg(0);
    
    double rawdata3[number_of_scan3][2]; //
    for (int i=0; i < number_of_scan3; i++) {
        KpaFile >> rawdata3[i][0] >> rawdata3[i][1];
    }
    
    double KPa[number_of_scan3], KPaSum=0;
    
    for (int i=0; i < number_of_scan3; i++) {
        KPa[i] = rawdata3[i][0] * rawdata3[i][1] * thickness;
    }
    
    cout << "\n";
    cout << "K Parenchymal \n";
    for (int i=0; i < number_of_scan3; i++) {
        cout << KPa[i] << "\n";
    }
    
    for (int i=0; i < number_of_scan3; i++) {
        KPaSum = KPaSum + KPa[i];
    }
    
    cout << "\n";
    
    /////////// Volumn data
    cout << "Parenchymal Volumn Data Unenhanced: " << KUnSum << "\n";
    cout << "Parenchymal Volumn Data in Arterial Phase: " << KArSum << "\n";
    cout << "Parenchymal Volumn Data in Parenchymal Phase: " << KPaSum << "\n \n";
    
    //////////// K(t1), K(t2)
    double Kt1 = KArSum - KUnSum;
    double Kt2 = KPaSum - KUnSum;
    cout << "K(t1) and K(t2): \n";
    cout << "K(t1): " << Kt1 << "\n";
    cout << "K(t2): " << Kt2 << "\n \n";
    
    /////////// Volumn
    double VolUn = 0, VolAr = 0, VolPa = 0; //volumn
    
    for (int i=0; i < number_of_scan1; i++) {
        VolUn = VolUn + rawdata1[i][1] * thickness;
    }
    for (int i=0; i < number_of_scan2; i++) {
        VolAr = VolAr + rawdata2[i][1] * thickness;
    }
    for (int i=0; i < number_of_scan3; i++) {
        VolPa = VolPa + rawdata3[i][1] * thickness;
    }
    
    cout << "Parenchymal Volumn Unenhanced: " << VolUn << " mm^3 \n";
    cout << "Parenchymal Volumn in Arterial Phase: " << VolAr << " mm^3 \n";
    cout << "Parenchymal Volumn in Parenchymal Phase: " << VolPa << " mm^3 \n";
    cout << "Mean Parenchymal Volumn: " << (VolUn + VolAr + VolPa) / 3 << " mm^3 \n \n";
    
    ////////////// c2
    double c2 = (Kt2 - Kt1*(PaIntDens/ArIntDens)) / (intsum2 - (PaIntDens/ArIntDens)*intsum1);
    
    cout << "c2 = " << c2 << "\n";
    
    /////////////////// GFR
    double Hct;
    
    cout << "Hematocrit level: ";
    cin >> Hct;
    cout << "GFR(CT) = " << (1-Hct)*c2*0.06 << " mL/min \n";
    
    
    unFile.close();
    inFile.close();
    KunFile.close();
    KarFile.close();
    KpaFile.close();
    return 0;
}
