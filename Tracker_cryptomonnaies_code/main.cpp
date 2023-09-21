#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include "CImg.h"
#include "gnuplot-iostream.h"

using namespace std;
using namespace cimg_library;


///
/// \brief compteur créé puis modifie un fichier texte qui contiendra un chiffre de 0 à 9 permettant tout les dix lancements du programme(modifié le modulo pour changer le nombre de lancements) de lancer une création d'un autre graphique montrant l'évolution du bitcoin au cours de ses dix lancements
/// \return un chiffre permettant de suivre le compte pour le programme
///
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

    // Enregistrer le nouveau compteur dans le fichier
    ofstream outFile("compteur_crypto.txt");
    if (outFile.is_open()) {
        outFile << compte;
        outFile.close();
    }
    return compte;
}

///
/// \brief set_URL_coinbase permet de créer l'url pour chaque devise ou crypto pour aller checher l'information voulu
/// \param symbole est le symbole de la devise ou crypto ex "USD" pour le dollar sur coinbase, il peut changer suivant l'API que l'on veut consulter-modifier la map CryptoAndCurrencies suivant les besoins.
/// \return l'adresse pour aller chercher la donnée voulue
///
string set_URL_coinbase(const string &symbole) {
    return "https://api.coinbase.com/v2/exchange-rates?currency=" + symbole;
}

///
/// \brief WriteCallback est une fonction de rappel pour la réception de données lors d'une requête HTTP.
/// \param contents est un pointeur vers les données reçues depuis la réponse HTTP
/// \param size est la taille en octets d'un élément des données.
/// \param nmemb est le nombre d'éléments de données reçus.
/// \param output est un pointeur vers une instance de la classe "string" où les données reçues seront stockées.
/// \return la taille totale des données reçues en octets
///
size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char *>(contents), total_size);
    return total_size;
}

///
/// \brief obtain_price permet d'obtenir le prix en USD depuis une URL en effectuant une requête HTTP en utilisant la bibliothèque libcurl
///  Elle récupère la réponse HTTP, l'analyse en tant que JSON pour extraire le prix, et renvoie ce prix en tant que double.
/// \param URL est l'URL à partir de laquelle récupérer le prix.
/// \return Le prix extrait de la réponse JSON. En cas d'erreur, renvoie 1.0.
///
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

///
/// \brief obtain_table est une fonction qui permet d'obtenir une table de conversion entre différentes cryptomonnaies.
/// \param List_crypto est une map associant le nom de la crypto ou devise à son symbole utilisé dans l'api
/// \param prices_USD est une map associant le symbole de la crypto / devise à son prix en USD
/// \return La table de conversion représentée par une carte imbriquée, où chaque entrée indique le taux de  conversion entre deux cryptomonnaies.
///
map<string, map<string, double>> obtain_table(const map<string, string>& List_crypto, map<string, double>& prices_USD){
    map<string, map<string, double>> ConversionTable;
    for (const auto &cryptoFrom : List_crypto) {
        const string &crypfromSymbol = cryptoFrom.second;
        map<string, double> conversions;

        for (const auto &cryptoTo : List_crypto) {
            const string &cryptoSymbol = cryptoTo.second;
            double cryptoFromPrice = prices_USD[crypfromSymbol];
            double cryptoToPrice = prices_USD[cryptoSymbol];

            // Calculer la conversion de cryptoFrom vers cryptoTo
            double conversion = cryptoFromPrice / cryptoToPrice;
            conversions[cryptoSymbol] = conversion;
        }

        ConversionTable[crypfromSymbol] = conversions;
    }
    return ConversionTable;
}

///
/// \brief create_json crée un objet JSON à partir des données de conversion
/// \param dateheure est la date et l'heure associées aux données de conversion.
/// \param List_crypto est une map associant le nom de la crypto ou devise à son symbole utilisé dans l'api
/// \param ConversionTable est une map représentant la table de conversion entre les cryptomonnaies
/// \return un JSON contenant les données de conversion, y compris la date et l'heure
///
nlohmann::json create_json(const char* dateheure,const map<string, string>& List_crypto,map<string, map<string, double>>& ConversionTable){
    nlohmann::json ConversionJson;
    ConversionJson["DateTime"] = dateheure;
    for (const auto &crypto : List_crypto) {
        const string &cryptoFromName = crypto.second;
        ConversionJson[cryptoFromName] = ConversionTable[cryptoFromName];
    }
    return ConversionJson;
    }

///
/// \brief save_json sauvegarde un JSON dans deux fichiers avec deux noms spécifiés différent
/// \param Conversion est le JSON à sauvegarder contenant toutes les conversions
/// \param dateheure est la date et l'heure associées à la requete http pour la ssauvegarde persistante
/// \param compte est l'état du compteur pour le rappeler facilement lors du tracé historique
///
void save_json(nlohmann::json& Conversion, const char* dateheure, int& compte){
    string jsonFileName = "conversion_" + string(dateheure) + "_" + to_string(compte) + ".json";
    ofstream jsonFile(jsonFileName);
    jsonFile << Conversion;
    jsonFile.close();

    // Enregistrer le JSON dans un fichier pour le récupérer lors de l'appel pour le graphique toutes les x_itérations et sera écraser lors d'un nouveau comptage
    string jsonFileName2 = "conversion_" + to_string(compte) + ".json";
    ofstream jsonFile2(jsonFileName2);
    jsonFile2 << Conversion;
    jsonFile2.close();
    }

///
/// \brief create_save_png_table crée et sauvegarde le tableau de conversion sous forme d'image PNG
/// \param SYMBOLE est une liste de symboles des cryptomonnaie et devises triés.
/// \param ConversionTable est une table de conversion entre les cryptomonnaies.
/// \param compte est l'état du compteur pour garder le fichier jusqu'au prochain compte
///
void create_save_png_table(vector<string>& SYMBOLE,map<string, map<string, double>>& ConversionTable, int& compte){
   // Obtenez la taille de la map (nombre de symboles)
   int mapSize = SYMBOLE.size();

   // Créez une image CImg pour le tableau
   CImg<unsigned char> image((mapSize + 1) * 100, (mapSize + 1) * 30, 1, 3, 255);

   // Remplissez la première ligne avec les symboles de crypto triés
   for (int i = 0; i < mapSize; i++) {
       image.draw_text((i + 1) * 100, 0, SYMBOLE[i].c_str(), "black");
   }

   // Remplissez la première colonne avec les symboles de crypto triés
   for (int i = 0; i < mapSize; i++) {
       string Symb = "1 " + SYMBOLE[i] + " =";
       image.draw_text(0, (i + 1) * 30, Symb.c_str(), "black");
   }

   // Remplir l'image avec les données de ConversionTable
   for (int i = 0; i < mapSize; i++) {
       for (int j = 0; j < mapSize; j++) {
           char valueStr[15];
           snprintf(valueStr, sizeof(valueStr), "%e", ConversionTable[SYMBOLE[i]][SYMBOLE[j]]);
           image.draw_text((j + 1) * 100, (i + 1) * 30, valueStr, "black");
       }
   }
   // Sauvegarder l'image en PNG
   const string Namepng = "conversion_table_" + to_string(compte) + ".png";
   image.save(Namepng.c_str());
   cout << "Tableau de conversion enregistré en tant qu'image." << endl;
}

///
/// \brief create_save_history_graph crée et sauvegarde un graphique de l'évolution d'une cryptomonnaie en fonction du temps et d'une monnaie de référence.
///  Le graphique est sauvegardé au format PostScript (PS).
/// \param crypto_a_suivre est le nom de la cryptomonnaie à suivre et afficher sur le graphique.
/// \param crypto_ref est le nom de la cryptomonnaie de référence pour l'évolution.
///
void create_save_history_graph(string& crypto_a_suivre, string& crypto_ref){
    // graph bitcoin /USD en fonction du temps
    map<string, double> data_crypto;

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

       // Extraire les valeurs en USD pour bitcoin
       double refValue = jsonData[crypto_a_suivre][crypto_ref];
       data_crypto[datetime] = refValue;
    }

   // Créez un objet Gnuplot
   Gnuplot gp;
   // Définissez le terminal de sortie au format PNG
   gp << "set terminal postscript" << endl;

   // Spécifiez le nom du fichier de sortie PNG
   gp << "set output 'graph_suivi_" << crypto_a_suivre << "_en_fct_de_" << crypto_ref << ".ps'" << endl;

   // Définissez le style du tracé (lignes, points, etc.)
   gp << "set style data linespoints" << endl;
   gp << "set timefmt \"%Y-%m-%d_%H-%M-%S\"" << endl;
   gp << "set xdata time" << endl;
   gp << "set xtics rotate by -45" << endl;

   // Nommez les axes
   gp << "set xlabel 'Datetime'" << endl;
   gp << "set ylabel 'Valeur en "<< crypto_ref << "'" << endl;

   // Titre du graphique
   gp << "set title 'Cours du " << crypto_a_suivre << " en fonction du temps'" << endl;

   // Affichez la légende
   gp << "set key top left" << endl;

   // Traçage des données pour Bitcoin (BTC)
   gp << "plot '-' using 1:2 title '"<< crypto_a_suivre <<"' with linespoints linecolor rgb 'red'" << endl;

   // Préparation des données au format Gnuplot
   for (const auto &data : data_crypto) {
       gp << data.first << " " << data.second << endl;
   }
   gp << "e" << endl; // Terminer le tracé
 }

///
/// \brief main appelle les différentes fonctions pour faire le tracqueur de cryptomonnaies
/// \return 0 si tout fonctionne commme prévu
///
int main() {
    // Faire un appel au compteur et récuperer la valeur
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

    // Faire une requete pour chaque élément de CryptoAndCurrencies pour obtenir son prix en dollar et stocker les résultat dans une map
    map<string, double> Crypto_prices_USD;
    for (const auto &crypto : CryptoAndCurrencies) {
        string cryptoSymbol = crypto.second;
        string URL_crypto = set_URL_coinbase(cryptoSymbol);
        double price_crypto = obtain_price(URL_crypto);
        Crypto_prices_USD[cryptoSymbol] = price_crypto;
    }

    // Créer un tableau de conversion pour toutes les cryptomonnaies et devises
    map<string, map<string, double>> Conversion_Table = obtain_table(CryptoAndCurrencies,Crypto_prices_USD);

    // Pour le suivi, enregistrement de la date et de l'heure de la requête
    time_t currentTime = time(nullptr);
    tm *localTime = localtime(&currentTime);
    char dateTimeString[100];
    strftime(dateTimeString, sizeof(dateTimeString), "%Y-%m-%d_%H-%M-%S", localTime);

    // Créer un objet JSON à partir du tableau de conversion et ajouter la date et l'heure
    nlohmann::json ConversionJson = create_json(dateTimeString,CryptoAndCurrencies,Conversion_Table);

    // Enregistrer le JSON dans deux fichiers dont un sera garder meme aapress la remise à 0 du compteur
    save_json(ConversionJson,dateTimeString,comptage);

    // Créez une liste de symboles de crypto triés par ordre alphabétique
    vector<string> symbols;
    for (const auto &pair : CryptoAndCurrencies) {
        symbols.push_back(pair.second);
    }
    sort(symbols.begin(), symbols.end());

    //Faire la table de conversion en PNG
    create_save_png_table(symbols,Conversion_Table,comptage);

    if (comptage == 0) {
        string Crypto1= "BTC";
        string Crypto2= "USD";
        string Crypto3= "XMR";
        create_save_history_graph(Crypto1, Crypto2);
        create_save_history_graph(Crypto3, Crypto2);
    }
  return 0;
}

