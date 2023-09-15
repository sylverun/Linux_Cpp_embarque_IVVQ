#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
//#include "EasyBMP/EasyBMP.h"


using namespace std;


string set_URL_coinbase(const string& symbole){
    return "https://api.coinbase.com/v2/exchange-rates?currency="+ symbole;
}

// Fonction de rappel pour gérer la réponse HTTP
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

double obtain_price(const string& URL){
    double price;

    // Initialisation de libcurl
    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Erreur lors de l'initialisation de libcurl." << endl;
        return 1.0;
    }

    // Configurer la requête HTTP GET pour to_convert
    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    // Stocker la réponse HTTP dans cette chaîne de caractères
    string response_data;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Effectuer la requête HTTP
    CURLcode res = curl_easy_perform(curl);

    // Vérifier si la requête s'est déroulée avec succès
    if (res != CURLE_OK) {
        cerr << "Erreur lors de la requête HTTP : " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return 1.0 ;
    }

    // Fermer libcurl
    curl_easy_cleanup(curl);

    // Analyser la réponse JSON to_convert
    try {
        nlohmann::json json_response = nlohmann::json::parse(response_data);
        string price_str = json_response["data"]["rates"]["USD"];
        price = stod(price_str);

    } catch (const exception& e) {
        cerr << "Erreur lors de l'analyse de la réponse JSON : " << e.what() << endl;
        return 1.0 ;
    }
    return price;
}



int main()
{

// Faire une map avec les symboles reconnus par l'api correspondant aux crypto et devises voulues.
    map<string, string> CryptoAndCurrencies;
    CryptoAndCurrencies["Bitcoin"] = "BTC";
    CryptoAndCurrencies["Ethereum"] = "ETH";
    CryptoAndCurrencies["BNB"] = "BNB";
    CryptoAndCurrencies["Cardano"] = "ADA";
    CryptoAndCurrencies["Dogecoin"] = "DOGE";
    CryptoAndCurrencies["Monero"] = "XMR";
    CryptoAndCurrencies["Dollar americain"] = "USD";
    CryptoAndCurrencies["Euro"] = "EUR";
    CryptoAndCurrencies["Yen japonais"] = "JPY";

// Faire une requete pour chaque elements de Crypto_list pour obtenir son prix en dollar et stocker les résultat dans une map
    map<string, double> Crypto_prices_USD;
    for (const auto &crypto : CryptoAndCurrencies){
        string cryptoSymbol = crypto.second;
        string URL_crypto= set_URL_coinbase(cryptoSymbol);
        double price_crypto= obtain_price(URL_crypto);
        Crypto_prices_USD[cryptoSymbol]= price_crypto;
    }


    // Créer un tableau de conversion pour toutes les cryptomonnaies et devises
    map<string, map<string, double>> ConversionTable;

    for (const auto &cryptoFrom : CryptoAndCurrencies) {
        const string &crypfromSymbol = cryptoFrom.second;
        map<string, double> conversions;

        for (const auto &cryptoTo : CryptoAndCurrencies) {
            const string &cryptoSymbol = cryptoTo.second;
            double cryptoFromPrice = Crypto_prices_USD[crypfromSymbol];
            double cryptoToPrice = Crypto_prices_USD[cryptoSymbol];

            // Calculer la conversion de cryptoFrom vers cryptoTo
            double conversion = cryptoFromPrice / cryptoToPrice;
            conversions[cryptoSymbol] = conversion;
        }

        ConversionTable[crypfromSymbol] = conversions;
    }

    // Pour le suivi, enregistrement de la date et de l'heure de la requete
            time_t currentTime = time(nullptr);
            tm *localTime = localtime(&currentTime);
            char dateTimeString[100];
            strftime(dateTimeString, sizeof(dateTimeString), "%Y-%m-%d_%H-%M-%S", localTime);



    // Créer un objet JSON à partir du tableau de conversion et ajouter la date et l'heure
        nlohmann::json ConversionJson;
        ConversionJson["DateTime"] = dateTimeString;
        for (const auto &crypto : CryptoAndCurrencies) {
            const string &cryptoFromName = crypto.second;
            ConversionJson[cryptoFromName] = ConversionTable[cryptoFromName];
        }

    // Enregistrer le JSON dans un fichier avec la date et l'heure dans le nom
        string jsonFileName = "conversion_" + string(dateTimeString) + ".json";
        ofstream jsonFile(jsonFileName);
        jsonFile << ConversionJson;
        jsonFile.close();

    //  Creation d'un tableau au format image

    return 0;
}

