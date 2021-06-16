#include<iostream>
#include<fstream>
#include<ctime>
#include<algorithm>
#include<fstream>
#include<string>
#include<vector>
#include<chrono>
#include<windows.h>

using namespace std;

class Client{
public:

    string clientId;
    int noOfTrades;
    long totalBuy;
    long currentValue;
    vector <string> symbols;
    vector<string> quantities;
    vector<string> buyPrice;

    Client(string clientIdentity){
        clientId = clientIdentity;
        totalBuy = 0.0;
        noOfTrades = 0;
        currentValue = 0.0;
    }
};

void totalMTM(Client c, vector<string> futSymbols, vector<string> futQuant,vector<string> currSymbols,vector<string> currPrices,vector<string>nonEquitySymbols,vector<string> nonEquityLotsize){
    const int MAXLEN = 80;
    char s[MAXLEN];
    time_t t = time(0);
    strftime(s, MAXLEN, "%Y-%m-%d", localtime(&t));

    ofstream newFile1;

    string str1 = c.clientId + "_MTM_" + s + ".csv";
    newFile1.open(str1);

    newFile1 << "Time Stamp" << ",";
    for(int i = 0 ; i < c.noOfTrades ; i++){
        newFile1 << c.symbols[i] << ",";
    }
    newFile1 << "\n";
    newFile1 << s << ",";


    for(int i = 0 ; i < c.noOfTrades;i++){
        auto it = find(futSymbols.begin(),futSymbols.end(),c.symbols[i]);
        if (it != futSymbols.end()){
            auto it1 = find(nonEquitySymbols.begin(),nonEquitySymbols.end(),c.symbols[i]);
            if(it1 != nonEquitySymbols.end()){
                int index1 = it1 - nonEquitySymbols.begin();

                c.totalBuy += (stof(c.quantities[i])* stof(c.buyPrice[i])* stof(nonEquityLotsize[index1]));
                c.currentValue += (stof(c.quantities[i])*stof(currPrices[i])*stof(nonEquityLotsize[index1]));

                cout << currPrices[i] << " " << c.quantities[i] << " " << nonEquityLotsize[index1] << " " << c.buyPrice[i] << endl;
                newFile1 << (((stof(currPrices[i]) * stof(c.quantities[i]) * stof(nonEquityLotsize[index1]))-(stof(c.buyPrice[i])*stof(c.quantities[i]) * stof(nonEquityLotsize[index1])))/(stof(c.buyPrice[i])*stof(nonEquityLotsize[index1])*stof(c.quantities[i])))<< ",";
                
            }else{
                int index = it - futSymbols.begin();

                c.totalBuy += (stof(c.quantities[i]) * stof(c.buyPrice[i]) * stof(futQuant[index]));
                c.currentValue += (stof(c.quantities[i])*stof(currPrices[i])*stof(futQuant[index]));

                cout << currPrices[i] << " " << c.quantities[i] << " " << futQuant[index] << " " << c.buyPrice[i] << endl;
                newFile1 << (((stof(currPrices[i]) * stof(c.quantities[i]) * stof(futQuant[index]))-(stof(c.buyPrice[i])*stof(c.quantities[i]) * stof(futQuant[index])))/(stof(c.buyPrice[i])*stof(futQuant[index])*stof(c.quantities[i])))<< ",";
            }
        }
        else {cout << "-1" << endl;}
       
    }    
    cout << "Total buy for client: " << c.clientId << " " << c.totalBuy << endl;
    cout << "Current Value of stocks for client: " << c.clientId << " " << c.currentValue << endl << endl;

    ofstream newFile;

    string str = c.clientId + "_NET_" + s + ".csv";
    newFile.open(str);
    newFile << "Time Stamp,Buy Value,Absolute MTM,Percentage MTM\n";
    newFile << s << ",";
    newFile << c.totalBuy << "," << c.currentValue - c.totalBuy << "," << ((float)(c.currentValue - c.totalBuy)/c.totalBuy)*100;
    
    
}

int main(){
    const int MAXLEN = 80;
    char s[MAXLEN];
    time_t t = time(0);
    strftime(s, MAXLEN, "%Y-%m-%d", localtime(&t));

    string c1 = "14FD050";
    string c2 = "14FD051";
    string c3 = "14FD052";
    vector<Client> Clients;
    //vector<string> currentPrices;
    vector<string> nonEquity;
    vector<string> nonEquityMultipliers;
    nonEquity.push_back("CRUDEOIL21JUNFUT");
    nonEquity.push_back("EURINR21JUNFUT");
    nonEquity.push_back("GBPINR21JUNFUT");
    nonEquity.push_back("JPYINR21JUNFUT");
    nonEquity.push_back("USDINR21JUNFUT");
    nonEquity.push_back("GOLDM21AUGFUT");
    nonEquity.push_back("SILVERM21JUNFUT");
    nonEquityMultipliers.push_back("100");
    nonEquityMultipliers.push_back("1000");
    nonEquityMultipliers.push_back("1000");
    nonEquityMultipliers.push_back("1000");
    nonEquityMultipliers.push_back("1000");
    nonEquityMultipliers.push_back("10");
    nonEquityMultipliers.push_back("5");

    Client client1_1(c1);
    Client client2_1(c2);
    Client client3_1(c3);
    Clients.push_back(client1_1);
    Clients.push_back(client2_1);
    Clients.push_back(client3_1);

    ifstream total_order_file;
    total_order_file.open("C:\\Users\\DESKTOP\\Desktop\\Future\\total_orders2021-06-11.csv");

    if(!total_order_file.is_open()) std::cout << "ERROR: Can't open Total Order File" << '\n';

    string index;
    string symbol;
    string quantity;
    string client_id;
    string orderType;
    string price;
    string timeStamp;

    while(total_order_file.good()){
        getline(total_order_file, index, ',');
        getline(total_order_file, symbol, ',');
        getline(total_order_file, quantity, ',');
        getline(total_order_file, client_id, ',');
        getline(total_order_file, orderType, ',');
        getline(total_order_file, price, ',');
        getline(total_order_file, timeStamp, '\n');

        for(int i = 0; i < Clients.size();i++){
            if(client_id == Clients[i].clientId){
                Clients[i].symbols.push_back(symbol);
                Clients[i].quantities.push_back(quantity);
                Clients[i].buyPrice.push_back(price);
                Clients[i].noOfTrades ++;
            }
        }
    }
    total_order_file.close();
    // for(int i = 0 ; i < Clients.size();i++){
    //     cout << "Trade History for client: " << Clients[i].clientId << endl;
    //     for(int j = 0; j < Clients[i].noOfTrades ;j++){
    //         cout << Clients[i].symbols[j] << " " << Clients[i].quantities[j] << " " << Clients[i].buyPrice[j] << endl;
    //     }
    // }

    ifstream lot_size;
    lot_size.open("C:\\Users\\DESKTOP\\Desktop\\Future\\lot_size.csv");

    if(!lot_size.is_open()) std::cout << "ERROR: Can't open Total Order File" << '\n';

    vector<string> futureFileSymbols;
    vector<string> futureFileQuantities;

    string index1;
    string symbol1;
    string quantity1;

    while(lot_size.good()){
        getline(lot_size, index1, ',');
        getline(lot_size, symbol1, ',');
        getline(lot_size, quantity1, '\n');

        futureFileSymbols.push_back(symbol1);
        futureFileQuantities.push_back(quantity1);
    }

    lot_size.close();

    ifstream futures_current;
    futures_current.open("C:\\Users\\DESKTOP\\Desktop\\Future\\currentFile.csv");

    if(!futures_current.is_open()) std::cout << "ERROR: Can't open Futures Current FIle" << '\n';
    vector<string> currentSymbols;
    vector<string> currentPrices;
    
    string index2;
    string symbol2;
    string price1;

    while(futures_current.good()){
        getline(futures_current,index2,',');
        getline(futures_current, symbol2, ',');
        getline(futures_current,price1,'\n');

        currentSymbols.push_back(symbol2);
        currentPrices.push_back(price1);
    }

    ifstream margin_file;
    margin_file.open("C:\\Users\\DESKTOP\\Desktop\\Future\\margin.csv");

    if(!margin_file.is_open()) std:: cout << "ERROR: Can't open Margin File" << '\n';

    vector<string> marginSymbols;
    vector<string> marginVlaues;

    string index3;
    string symbol3;
    string value;

    //while(margin_file.good()){
    //    getline(margin_file,index3,',');
    //    getline(margin_file,)
    //}
    //cout << futureFileSymbols.size() << " " << futureFileQuantities.size() << endl;
    for(int i = 0 ; i < Clients.size();i++){
        totalMTM(Clients[i],futureFileSymbols,futureFileQuantities,currentSymbols,currentPrices,nonEquity,nonEquityMultipliers);
    }
    //totalMTM(Clients[1],futureFileSymbols,futureFileQuantities,currentSymbols,currentPrices);
    //totalMTM(Clients[2],futureFileSymbols,futureFileQuantities,currentSymbols,currentPrices);


    return 0;   
}