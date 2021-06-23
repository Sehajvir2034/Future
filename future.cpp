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
    long totalSell;
    long buyCurrent;
    long sellCurrent;
    long currentValue;
    long marginValue;
    vector <string> symbols;
    vector<string> quantities;
    vector<string> buyPrice;

    Client(string clientIdentity){
        clientId = clientIdentity;
        totalBuy = 0.0;
        totalSell = 0.0;
        marginValue = 0.0;
        noOfTrades = 0;
        currentValue = 0.0;
    }
};

void totalMargin(Client c,vector<string> marginSymbols,vector<string> marginValues){
    for(int i = 0 ; i < c.noOfTrades;i++){
        auto it = find(marginSymbols.begin(),marginSymbols.end(),c.symbols[i]);
        if(it!=marginSymbols.end()){
            int index = it - marginSymbols.begin();
            c.marginValue += (stold(c.quantities[i])*stold(marginValues[index]));
            //cout << c.quantities[i] << " " << c.buyPrice[i] << " " << marginValues[index] << endl;
        }
    }
    cout << "Total margin for client " << c.clientId << " is:" << c.marginValue << endl;
}

void totalMTM(Client c, vector<string> futSymbols, vector<string> futQuant,vector<string> currSymbols,vector<string> currPrices,vector<string>nonEquitySymbols,vector<string> nonEquityLotsize,vector<string>marginSymbols,vector<string>marginValues){
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
                if(stold(c.quantities[i]) > 0){
                    c.totalBuy += (stold(c.quantities[i])* stold(c.buyPrice[i])* stold(nonEquityLotsize[index1]));
                    c.buyCurrent += (stold(c.quantities[i])*stold(currPrices[i])*stold(nonEquityLotsize[index1]));
                    auto symbolIndex = find(marginSymbols.begin(),marginSymbols.end(),c.symbols[i]);
                    if(symbolIndex != marginSymbols.end()){
                        int symbInd = symbolIndex - marginSymbols.begin();
                        cout << currPrices[i] << " " << c.quantities[i] << " " << nonEquityLotsize[index1] << " " << c.buyPrice[i] << " " << marginValues[symbInd] <<endl;
                        newFile1 << (((stold(currPrices[i]) * stold(c.quantities[i]) * stold(nonEquityLotsize[index1]))-(stold(c.buyPrice[i])*stold(c.quantities[i]) * stold(nonEquityLotsize[index1])))/(stold(marginValues[symbInd])*stold(c.quantities[i])))<<",";
                    }
                }else{
                    c.totalSell += (stold(c.quantities[i]) *stold(c.buyPrice[i])* stold(nonEquityLotsize[index1]));
                    c.sellCurrent += (stold(c.quantities[i])*stold(currPrices[i])*stold(nonEquityLotsize[index1]));
                    auto ind = find(marginSymbols.begin(),marginSymbols.end(),c.symbols[i]);
                    if(ind != marginSymbols.end()){
                        int index4 = ind - marginSymbols.begin();
                        cout << currPrices[i] << " " << c.quantities[i] << " " << nonEquityLotsize[index1] << " " << c.buyPrice[i] << " " << marginValues[index4] <<endl;
                        newFile1 << (((stold(currPrices[i]) * stold(c.quantities[i]) * stold(nonEquityLotsize[index1]))-(stold(c.buyPrice[i])*stold(c.quantities[i]) * stold(nonEquityLotsize[index1])))/(stold(marginValues[index4])*stold(c.quantities[i])))<<',';
                    }
                }

                //c.totalBuy += (stold(c.quantities[i])* stold(c.buyPrice[i])* stold(nonEquityLotsize[index1]));
                //c.currentValue += (stold(c.quantities[i])*stold(currPrices[i])*stold(nonEquityLotsize[index1]));

                //cout << currPrices[i] << " " << c.quantities[i] << " " << nonEquityLotsize[index1] << " " << c.buyPrice[i] << endl;
                //newFile1 << (((stold(currPrices[i]) * stold(c.quantities[i]) * stold(nonEquityLotsize[index1]))-(stold(c.buyPrice[i])*stold(c.quantities[i]) * stold(nonEquityLotsize[index1])))/(stold(c.buyPrice[i])*stold(nonEquityLotsize[index1])*stold(c.quantities[i])))<< ",";
                
            }else{
                int index = it - futSymbols.begin();
                if(stold(c.quantities[i]) > 0){
                    c.totalBuy += (stold(c.quantities[i])*stold(currPrices[i])*stold(futQuant[index]));
                    c.buyCurrent += (stold(c.quantities[i])*stold(currPrices[i])*stold(futQuant[index]));
                    auto indexx = find(marginSymbols.begin(),marginSymbols.end(),c.symbols[i]);
                    if(indexx != marginSymbols.end()){
                        int index5 = indexx - marginSymbols.begin();
                        cout << currPrices[i] << " " << c.quantities[i] << " " << futQuant[index] << " " << c.buyPrice[i] << " " << marginValues[index5] <<endl;
                        newFile1 << (((stold(currPrices[i]) * stold(c.quantities[i]) * stold(futQuant[index]))-(stold(c.buyPrice[i])*stold(c.quantities[i]) * stold(futQuant[index])))/(stold(marginValues[index5])*stold(c.quantities[i])))<<",";
                    }
                }else{
                    c.totalSell += (stold(c.quantities[i]) *stold(c.buyPrice[i])* stold(futQuant[index]));
                    c.sellCurrent += (stold(c.quantities[i])*stold(currPrices[i])*stold(futQuant[index]));
                    auto indexxx = find(marginSymbols.begin(),marginSymbols.end(),c.symbols[i]);
                    if(indexxx != marginSymbols.end()){
                        int index6 = indexxx - marginSymbols.begin();
                        cout << currPrices[i] << " " << c.quantities[i] << " " << futQuant[index] << " " << c.buyPrice[i] << " " << marginValues[index6] <<endl;
                        newFile1 << (((stold(currPrices[i]) * stold(c.quantities[i]) * stold(futQuant[index]))-(stold(c.buyPrice[i])*stold(c.quantities[i]) * stold(futQuant[index])))/(stold(marginValues[index6])*stold(c.quantities[i])))<<',';
                    }
                    
                }

                //c.totalBuy += (stold(c.quantities[i]) * stold(c.buyPrice[i]) * stold(futQuant[index]));
                //c.currentValue += (stold(c.quantities[i])*stold(currPrices[i])*stold(futQuant[index]));

                //cout << currPrices[i] << " " << c.quantities[i] << " " << futQuant[index] << " " << c.buyPrice[i] << endl;
                //newFile1 << (((stold(currPrices[i]) * stold(c.quantities[i]) * stold(futQuant[index]))-(stold(c.buyPrice[i])*stold(c.quantities[i]) * stold(futQuant[index])))/(stold(c.buyPrice[i])*stold(futQuant[index])*stold(c.quantities[i])))<< ",";
            }
        }
        else {cout << "-1" << endl;}
       
    }    
    cout << "Total buy for client: " << c.clientId << " " << c.totalBuy << endl;
    cout << "Total sell for client: " << c.clientId << " " << c.totalSell << endl;
    cout << "Current selling price :" << c.clientId << " " << c.sellCurrent << endl;
    totalMargin(c,marginSymbols,marginValues);
    //cout << "Current Value of stocks for client: " << c.clientId << " " << endl << endl;

    ofstream newFile;

    string str = c.clientId + "_NET_" + s + ".csv";
    newFile.open(str);
    newFile << "Time Stamp,Buy Value,Sell Value,Buy MTM,Sell MTM,Percentage MTM\n";
    newFile << s << ",";
    newFile << c.totalBuy << "," << abs(c.totalSell) << "," << float(c.buyCurrent-c.totalBuy)/(c.totalBuy) << "," << float(abs(c.sellCurrent)-abs(c.totalSell))/(c.totalSell);
    
    
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

    ifstream trade_order_file;
    trade_order_file.open("C:\\Users\\DESKTOP\\Desktop\\Future\\trade_file2021-06-11.csv");

    if(!trade_order_file.is_open()) std::cout << "ERROR: Can't open Trade File" << '\n';

    string index;
    string symbol;
    string price;
    string quantity;
    string client_id;
    string timeStamp;

    while(trade_order_file.good()){
        getline(trade_order_file, index, ',');
        getline(trade_order_file, symbol, ',');
        getline(trade_order_file, price, ',');
        getline(trade_order_file, quantity, ',');
        getline(trade_order_file, client_id, ',');
        getline(trade_order_file, timeStamp, '\n');

        for(int i = 0; i < Clients.size();i++){
            if(client_id == Clients[i].clientId){
                Clients[i].symbols.push_back(symbol);
                Clients[i].quantities.push_back(quantity);
                Clients[i].buyPrice.push_back(price);
                Clients[i].noOfTrades ++;
            }
        }
    }
    //trade_order_file.close();
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
    vector<string> marginValues;

    string index3;
    string symbol3;
    string value;

    while(margin_file.good()){
        getline(margin_file,index3,',');
        getline(margin_file,symbol3,',');
        getline(margin_file,value,'\n');

        marginSymbols.push_back(symbol3);
        marginValues.push_back(value);
    }

    //cout << futureFileSymbols.size() << " " << futureFileQuantities.size() << endl;
    for(int i = 0 ; i < Clients.size();i++){
        totalMTM(Clients[i],futureFileSymbols,futureFileQuantities,currentSymbols,currentPrices,nonEquity,nonEquityMultipliers,marginSymbols,marginValues);
    }
    //totalMargin(Clients[0],marginSymbols,marginValues);
    //totalMargin(Clients[1],marginSymbols,marginValues);
    //totalMargin(Clients[2],marginSymbols,marginValues);

    return 0;   
}