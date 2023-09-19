#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <CImg.h>
#include <gnuplot-iostream.h>

using namespace std;
using namespace cimg_library;

int compteur() {
    ifstream configFile("compteur_crypto.txt");

    int compte = 0;
    if (configFile.is_open()) {
        configFile >> compte;
        configFile.close();
    }

    // Incrémenter le compteur
    compte++;
    compte = compte % 10;

    // Enregistrer le nouveau compteur dans le fichier de configuration
    ofstream outFile("compteur_crypto.txt");
    if (outFile.is_open()) {
        outFile << compte;
        outFile.close();
    }
    return compte;
}

string set_URL_coinbase(const string &symbole) {
    return "https://api.coinbase.com/v2/exchange-rates?currency=" + symbole;
}

// Fonction de rappel pour gérer la réponse HTTP
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char *>(contents), total_size);
    return total_size;
}

double obtain_price(const string &URL) {
    double price;

    // Initialisation de libcurl
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Erreur lors de l'initialisation de libcurl." << endl;
        return 1.0;
    }

    // Configurer la requête HTTP GET pour to_convert
    curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    // Stocker la réponse HTTP dans cette chaîne de caractères
    string response_data;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Effectuer la requête HTTP
    CURLcode res = curl_easy_perform(curl);

    // Vérifier si la requête s'est déroulée avec succès
    if (res != CURLE_OK) {
        cerr << "Erreur lors de la requête HTTP : " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        return 1.0;
    }

    // Fermer libcurl
    curl_easy_cleanup(curl);

    // Analyser la réponse JSON to_convert
    try {
        nlohmann::json json_response = nlohmann::json::parse(response_data);
        string price_str = json_response["data"]["rates"]["USD"];
        price = stod(price_str);

    } catch (const exception &e) {
        cerr << "Erreur lors de l'analyse de la réponse JSON : " << e.what() << endl;
        return 1.0;
    }
    return price;
}

int main() {
    // Faire un appel  au compteur et récuperer la valeur
    int comptage = compteur();

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

    // Faire une requete pour chaque élément de Crypto_list pour obtenir son prix en dollar et stocker les résultat dans une map
    map<string, double> Crypto_prices_USD;
    for (const auto &crypto : CryptoAndCurrencies) {
        string cryptoSymbol = crypto.second;
        string URL_crypto = set_URL_coinbase(cryptoSymbol);
        double price_crypto = obtain_price(URL_crypto);
        Crypto_prices_USD[cryptoSymbol] = price_crypto;
    }

    // Créer un tableau de conversion pour toutes les cryptomonnaies et devises (utilisé  pour générer  le json)
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

    // Pour le suivi, enregistrement de la date et de l'heure de la requête
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

    // Enregistrer le JSON dans un fichier avec la date et l'heure dans le nom pour le garder sans l'effacer après x_itération
    string jsonFileName = "conversion_" + string(dateTimeString) + "_" + to_string(comptage) + ".json";
    ofstream jsonFile(jsonFileName);
    jsonFile << ConversionJson;
    jsonFile.close();

    // Enregistrer le JSON dans un fichier pour le récupérer lors de l'appel pour le graphique toutes les x_itérations
    string jsonFileName2 = "conversion_" + to_string(comptage) + ".json";
    ofstream jsonFile2(jsonFileName2);
    jsonFile2 << ConversionJson;
    jsonFile2.close();

    // Créez une liste de symboles de crypto triés par ordre alphabétique
    vector<string> symbols;
    for (const auto &pair : CryptoAndCurrencies) {
        symbols.push_back(pair.second);
    }
    sort(symbols.begin(), symbols.end());

    // Obtenez la taille de la map (nombre de symboles)
    int mapSize = symbols.size();

    // Créez une image CImg pour le tableau
    CImg<unsigned char> image((mapSize + 1) * 100, (mapSize + 1) * 30, 1, 3, 255);

    // Remplissez la première ligne avec les symboles de crypto triés
    for (int i = 0; i < mapSize; i++) {
        image.draw_text((i + 1) * 100, 0, symbols[i].c_str(), "black");
    }

    // Remplissez la première colonne avec les symboles de crypto triés
    for (int i = 0; i < mapSize; i++) {
        string Symb = "1 " + symbols[i] + " =";
        image.draw_text(0, (i + 1) * 30, Symb.c_str(), "black");
    }

    // Remplir l'image avec les données de ConversionTable
    for (int i = 0; i < mapSize; i++) {
        for (int j = 0; j < mapSize; j++) {
            char valueStr[15];
            snprintf(valueStr, sizeof(valueStr), "%e", ConversionTable[symbols[i]][symbols[j]]);
            image.draw_text((j + 1) * 100, (i + 1) * 30, valueStr, "black");
        }
    }

    // Sauvegarder l'image en PNG ou JPEG
    const string Namepng = "conversion_table_" + to_string(comptage) + ".png";
    image.save(Namepng.c_str());

    cout << "Tableau de conversion enregistré en tant qu'image." << endl;

    if (comptage == 0) {
        /*
        // Créer un map pour stocker les données pour chaque datetime
        map<string, map<string, double>> data_datetime;

        // Aller chercher les valeurs pour chaque datetime dans les fichiers JSON
        for (int i = 0; i < 10; i++) {
            string jsonFileName = "conversion_" + to_string(i) + ".json";
            ifstream jsonFile(jsonFileName);

            if (!jsonFile) {
                cerr << "Erreur lors de l'ouverture du fichier JSON : " << jsonFileName << endl;
                continue; // Passer au fichier suivant en cas d'erreur
            }

            nlohmann::json jsonData;
            jsonFile >> jsonData;

            // Extraire la date et l'heure du JSON
            string datetime = jsonData["DateTime"];

            // Extraire les valeurs en USD pour chaque crypto-monnaie
            map<string, double> usdValues;
            for (auto &symbol : symbols) {
                double usdValue = jsonData[symbol]["USD"];
                usdValues[symbol] = usdValue;
            }

            // Stocker les données dans le map des datetimes
            data_datetime[datetime] = usdValues;*/

        // graph bitcoin /USD en fonction du temps
        map<string, double> data_BTC;

        for (int i = 0; i < 10; i++) {
            string jsonFileName = "conversion_" + to_string(i) + ".json";
            ifstream jsonFile(jsonFileName);

            if (!jsonFile) {
                cerr << "Erreur lors de l'ouverture du fichier JSON : " << jsonFileName << endl;
                continue; // Passer au fichier suivant en cas d'erreur
            }
            nlohmann::json jsonData;
            jsonFile >> jsonData;

            // Extraire la date et l'heure du JSON
            string datetime = jsonData["DateTime"];

            // Extraire les valeurs en USD pour bitcooin
            double usdValue = jsonData["BTC"]["USD"];
            data_BTC[datetime] = usdValue;
            }

        // Créez un objet Gnuplot
    Gnuplot gp;

    // Définissez le terminal de sortie au format PNG
    gp << "set terminal postscript" << endl;

    // Spécifiez le nom du fichier de sortie PNG
    gp << "set output 'graph_suivi_BTC.ps'" << endl;

    // Définissez le style du tracé (lignes, points, etc.)
    gp << "set style data linespoints" << endl;
    gp << "set timefmt \"%Y-%m-%d_%H-%M-%S\"" << endl;
    gp << "set xdata time" << endl;
    gp << "set xtics rotate by -45" << endl;

    // Nommez les axes
    gp << "set xlabel 'Datetime'" << endl;
    gp << "set ylabel 'Valeur en USD'" << endl;

    // Titre du graphique
    gp << "set title 'Cours du Bitcoin (BTC) en fonction du Datetime'" << endl;

    // Affichez la légende
    gp << "set key top left" << endl;

    // Créez un vecteur de couleurs pour les courbes (facultatif)
    vector<string> colors = {"red"};

    // Traçage des données pour Bitcoin (BTC)
    gp << "plot '-' using 1:2 title 'BTC' with linespoints linecolor rgb 'red'" << endl;

    // Préparation des données au format Gnuplot
    for (const auto &data : data_BTC) {

        gp << data.first << " " << data.second << endl;
    }

    gp << "e" << endl; // Terminer le tracé
  }

    return 0;
}

