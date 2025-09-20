#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <ctime>
#include <sqlite3.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "amj_date.hpp"

/*
	Programme de comptabilité en ligne de commande
	Livre Comptable Daniel -> lcd version 1.1.0

	Tous les montants seront exprimés en cents dans la base de données.
*/

/*	Compilation g++ -std=c++11 lcd.cpp -o lcd -mmacosx-version-min=10.9 -l sqlite3 -l readline	*/

struct compte {
	std::string Nom;
	std::string Ref;
	std::string Type;	// Courant, Épargne, Crédit
	std::string Départ;
	std::string Présent;
};
/*
┌────┬──────────────────────────────────┬──────────────┐┌────┬──────────────────────────────────┬──────────────┐
│  1 │ Compte #1 | xxxxx-xxxxxx-x       │ $    2240,98 ││  2 │ Compte #2 | xxxxx-xxxxxx-x       │ $    3490,88 │
└────┤ -                                │ $    2240,98 │└────┤ -                                │ $    3490,88 │
	 └──────────────────────────────────┴──────────────┘     └──────────────────────────────────┴──────────────┘
*/
struct categorie {
	std::string Nom;
	int Utilisé;
	std::string Type;	// IN, OUT, Courant, Épargne, Crédit
};
/*
┌────┬───────────────────────────┐┌────┬───────────────────────────┐┌────┬───────────────────────────┐
│  1 │ Compte #1 | xxxxx-xxxxxx- ││  2 │ Compte #2 | xxxxx-xxxxxx- ││  3 │ Compte #3 | xxxxx-xxxxxx- │
└────┴───────────────────────────┘└────┴───────────────────────────┘└────┴───────────────────────────┘
*/
struct transaction {
	std::string Date;	// Sous forme: 2024-03-24
	std::string Description;
	std::string Type;	// Dépôt, Débit, Crédit, Achat, Virement, Paiement
	std::string Compte;
	std::string Catégorie;	// Nom de la catégorie
	std::string Montant;
};
/*
┌────────────┬────────────────────────────────┬─────────────┬─────────────────────────────────────────┐
│ 2024-05-31 │ Hypothèque                     │ Débit       │   De: Compte #2 | xxxxx-xxxxxx-x        │
└────────────┤                                │ $    234,56 │ Vers: Hypothèque                        │
			 └────────────────────────────────┴─────────────┴─────────────────────────────────────────┘
*/
int callback(void *, int, char **, char **);

//	Définition de la classe Livre Comptable

class LivreComptable
{
private:
	sqlite3* bd;
	AMJ_Date d;
	char *line_read = nullptr;
	std::vector<struct compte> COMPTES;
	std::vector<struct categorie> CATEGORIES;
	std::vector<struct transaction> TRANSACTIONS;
	std::vector<struct transaction> FAV_TRANSACTIONS;
/*
	Des vecteurs temporaires dépendant du type demandé
	Important de faire clear avant usage
*/
	std::vector<struct compte> TMP_COMPTES;
	std::vector<struct categorie> TMP_CATEGORIES;
	std::vector<struct transaction> TMP_TRANSACTIONS;
/*
	Type de transaction: Dépôt, Débit, Crédit, Achat, Virement, Paiement
	Les deux derniers impliquent une double transaction en une seule ligne.

	Opération :                                compte courant | Épargne | carte crédit
	----------------------------------------------------------------------------------
	Dépôt : Paye, remboursement d'impôt ...           +           N/A         N/A
	Débit : Hydro-Québec, Videotron ...               -           N/A          +
	Crédit : Remise dollar carte de crédit            +            +           -
	Achat : Achat avec une carte en magasin           -           N/A          +
	Virement : Transfert d'argent entre comptes     - / +        - / +        N/A
	Paiement : Remboursement de solde (carte)         -           N/A          -
	----------------------------------------------------------------------------------

	Les catégories sont déclarées dynamiquement + l'ajout des comptes
	pour permettre de faire des paiements et des virements.

	Dans le cas où le TYPE est Paiement ou Virement le logiciel présentera
	seulement les catégories appropriées pour ce type de transaction.
*/
	std::vector<std::string> typeTransaction = {"Dépôt", "Débit", "Crédit", "Achat", "Virement", "Paiement"};

	bool initLivre();
	void twistAccent(const char *, int &);
	bool testMontant(std::string &);
	std::string dollars2cents(const std::string &);	// Convert dollars to cents
	std::string cents2dollars(const std::string &);	// Convert cents to dollars
	void publishTransaction(struct transaction&);
	void incantationSQL(std::string, std::string);
public:
	LivreComptable();
	~LivreComptable();

	bool ouvreLivre(std::string, bool);

	bool ajoutCompte();
	void suppCompte();
	bool ajoutCategorie();
	void suppCategorie();
	void suppFavorite();
	void modifFavorite();

	void sommaireMois();

	int setMaster2mem(char **);
	int setCategories2mem(char **);
	int setFavorites2mem(char **);
	int setTransactions2mem(char **);
	int setQuestion2mem(char **);

	void questionBD();
	void fullQuestionBD();

	void printType();
	void printComptes(std::vector<struct compte>&);
	void printAllComptes();
	void printCategories(std::vector<struct categorie>&);
	void printAllCategories();
	void print1Transaction(struct transaction&);
	void printTransactions(std::vector<struct transaction>&, bool = false);
	void printAllFavorites();

	bool nouvelleTransaction(bool = false);

	void fermeLivre();
};
//	--------------------- Les variables globales ------------------------------

LivreComptable livre;	// LE livre de comptes
bool cbackPassage = false;	// Sommes-nous déjà passé dans callback
std::string reponse = "";	// Pour recueillir l'input usager
int choix = 0;	// atoi() de reponse

//	------------------------ Les 3 callbacks ----------------------------------
int callback(void *data, int argc, char **argv, char **azColName) {
	std::string message = (const char *)data;
	// Dépendant de la string passé en data, celà influencera la manipulation du retour.
	if (message == "setMaster2mem") return livre.setMaster2mem(argv);
	else if (message == "setCategories2mem") return livre.setCategories2mem(argv);
	else if (message == "setTransactions2mem") return livre.setTransactions2mem(argv);
	else if (message == "setFavorites2mem") return livre.setFavorites2mem(argv);
	else if (message == "setQuestion2mem") return livre.setQuestion2mem(argv);
	else {
		if (!cbackPassage) {
			std::cout << "La commande «" << message << "» répond: " << std::endl;
			std::cout << "--------------------------------------------------------\n" << std::endl;
			cbackPassage = true;
		}
		for(int i = 0; i < argc; i++){
			std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "N/A") << std::endl;
		}
		std::cout << std::endl;
	}
	return 0;
}
bool sortTransDateAsc(transaction t1, transaction t2) {
	return (t1.Date < t2.Date); 
}
bool sortTransDateDesc(transaction t1, transaction t2) {
	return (t1.Date > t2.Date); 
}
//	---------------------------------------------------------------------------
bool getReponse(const int nbEntree) {
	reponse = "";
	choix = 0;

	std::cout << "[0 ou enter pour annuler]: " << std::flush;
	std::getline(std::cin, reponse);
	if (reponse == "0" || reponse == "") return false;
	choix = atoi(reponse.c_str());
	if (choix == 0 || choix > nbEntree) return false;
	return true;
}

//	------------------- Implémentation de classe ------------------------------
//	private:
bool LivreComptable::initLivre() {

	incantationSQL("CREATE TABLE Master(Nom TEXT, Ref TEXT, Type TEXT, Départ INT, Présent INT);", "");
	incantationSQL("CREATE TABLE Catégories(Nom TEXT, Utilisé INT, Type TEXT);", "");
	incantationSQL("CREATE TABLE Transactions(Date TEXT, Description TEXT, Type TEXT, "\
														"Compte TEXT, Catégorie TEXT, Montant INT);", "");
	incantationSQL("CREATE TABLE Favorites(Date TEXT, Description TEXT, Type TEXT, "\
														"Compte TEXT, Catégorie TEXT, Montant INT);", "");

	std::cout << "\n--- Création des comptes --- | Retour seulement pour sortir" << std::endl;
	while (true) {
		if (!ajoutCompte()) break;
		std::cout << "-----------" << std::endl;
	}

	std::cout << "\n--- Création des catégories --- | Retour seulement pour sortir" << std::endl;
	while (true) {
		if (!ajoutCategorie()) break;
		std::cout << "-----------" << std::endl;
	}
	return true;
}

void LivreComptable::twistAccent(const char *chaine, int &width) {
	for (int i = 0; i < strlen(chaine); i++) {
		if ((unsigned int)chaine[i] >= '\x80') { width++; i++; }
		if ((unsigned int)chaine[i] >= '\xE0') { width++; i++; }
		if ((unsigned int)chaine[i] >= '\xF0') { width++; i++; }
	}
}

bool LivreComptable::testMontant(std::string &source) {
// Test la validité du montant en source: [-][::digit::][,|.::digit::]
	bool decimal = false;
	int compteur = 0;

	for (int i = 0; i < source.size(); i++) {
		if (source[i] >= '0' && source[i] <= '9') {
			if (decimal) compteur++;
			if (compteur > 2) return false;
			continue;
		}
		else if (source[i] == ',' || source[i] == '.') {
			if (!decimal) { decimal = true; continue; }
			else return false;
		}
		else if (source[i] == '-' && i == 0) continue;
		else return false;
	}
	return true;
}
std::string LivreComptable::cents2dollars(const std::string &cents) {
	// Converti la string xxxxx en xxx,xx pour l'affichage.
	std::string convString;
	int l = cents.size();

	if (l == 1) convString = "00" + cents;
	else if (l == 2) convString = "0" + cents;
	else convString = cents;
	convString.insert(convString.size() - 2, ",");

	int s = convString.size();
	if (s > 6) {	// Espace un peu si > 999,99
		if (convString[s - 7] == '-') return convString;	// Un -, on touche à rien
		else convString.insert(s - 6, " ");
	}
	return convString;
}
std::string LivreComptable::dollars2cents(const std::string &dollars) {
	//	Converti la string xxx,xx en cents pour la bd.
	std::string convString = dollars;
	int l = convString.size();
	int pos = -1;

	for (int i = 0; i < l; i++) {
		if (convString[i] == ',' || convString[i] == '.') {
			pos = i;
			break;
		}
	}
	if (pos == -1) convString += "00";	// Il n'y a pas de séparateur
	else {
		if (l - pos == 3) convString.erase(pos, 1);
		else if (l - pos == 2) { convString.erase(pos, 1); convString += "0"; }	
		else if (l - pos == 1) { convString.erase(pos, 1); convString += "00"; }	
	}
	return convString;
}

void LivreComptable::publishTransaction(transaction &trans) {

	std::string reqSQL = "";

	TRANSACTIONS.push_back(trans);
	std::sort(TRANSACTIONS.begin(), TRANSACTIONS.end(), sortTransDateAsc);

	reqSQL = "INSERT INTO Transactions(Date, Description, Type, Compte, Catégorie, Montant)";
	reqSQL += " VALUES(\"" + trans.Date + "\", \"" + trans.Description + "\", \"" + trans.Type + "\",";
	reqSQL += " \"" + trans.Compte + "\", \"" + trans.Catégorie + "\", " + trans.Montant + ");";

	incantationSQL(reqSQL,"");

	// Update Comptes et Catégories en mémoire «vector» et en base de données

	int cmteIndex;

	// lambda pour faire addition ou soustraction
	auto add_sub = [&reqSQL, &trans, this, &cmteIndex](bool add) mutable {

		int present = atoi(COMPTES[cmteIndex].Présent.c_str());
		int montant = atoi(trans.Montant.c_str());

		if (add) {
			reqSQL = "UPDATE Master SET Présent = Présent + " + trans.Montant + " WHERE Nom = '" + trans.Compte + "';";
			present += montant;
		}
		else {
			reqSQL = "UPDATE Master SET Présent = Présent - " + trans.Montant + " WHERE Nom = '" + trans.Compte + "';";
			present -= montant;
		}
		incantationSQL(reqSQL,"");

		COMPTES[cmteIndex].Présent = std::to_string(present);
	};

	// lambda pour trouver l'index du compte dans COMPTES
	auto findIDX = [this, &trans, &cmteIndex]() {
		for (size_t i = 0; i < COMPTES.size(); i++) {
			if (COMPTES[i].Nom == trans.Compte) {
				cmteIndex = i;
				break;  // Sortir de la boucle dès qu'on trouve le compte
			}
		}
	};

	findIDX();
	if (trans.Type == "Dépôt" || trans.Type == "Crédit") {
		if (COMPTES[cmteIndex].Type == "Crédit") add_sub(false);
		else add_sub(true);
	}

	else if (trans.Type == "Débit" || trans.Type == "Achat") {
		if (COMPTES[cmteIndex].Type == "Crédit") add_sub(true);
		else add_sub(false);
	}

	else if (trans.Type == "Virement") {
		add_sub(false);
		trans.Compte = trans.Catégorie;
		findIDX();
		add_sub(true);
	}
	else if (trans.Type == "Paiement") {
		add_sub(false);
		trans.Compte = trans.Catégorie;
		findIDX();
		add_sub(false);
	}

	// Update Catégories maintenant
	reqSQL = "UPDATE Catégories SET Utilisé = Utilisé + 1 WHERE Nom = '" + trans.Catégorie + "';";
	incantationSQL(reqSQL,"");
	for (auto i = 0; i < CATEGORIES.size(); i++) {
		if (CATEGORIES[i].Nom == trans.Catégorie) {
			CATEGORIES[i].Utilisé++;
			break;
		}
	}
}

void LivreComptable::incantationSQL(std::string sql, std::string message) {
	/* Execute SQL statement */
	int rc;
	char *zErrMsg = 0;

	rc = sqlite3_exec(bd, sql.c_str(), callback, (void *)message.c_str(), &zErrMsg);

	if(rc != SQLITE_OK) {
		std::cerr << "Erreur SQL: " << zErrMsg << std::endl;
		sqlite3_free(zErrMsg);
	}
}

//	------------------- Constructeur et Destructeur ---------------------------
LivreComptable::LivreComptable() {
	using_history();
}
LivreComptable::~LivreComptable() {
}
//	---------------------------------------------------------------------------
//	public:
bool LivreComptable::ouvreLivre(std::string nomLivre, bool nouveau) {
//	Ouvre la base de données
	int rc;
	std::cout << "Ouverture de la base de donnée «" << nomLivre << "»"<< std::endl;
	rc = sqlite3_open(nomLivre.c_str(), &bd);
	if(rc) {
		std::cerr << "Ne peut ouvrir la base de données: " << sqlite3_errmsg(bd) << std::endl;
		return false;
	}
	if (nouveau) {
		std::cout << "nouvelle base de données " << nomLivre << std::endl;
		if(!initLivre()) {
			std::cout << "Incapable d'initaliser la base de donnée: effacement de «" << nomLivre << "»" << std::endl;
			fermeLivre();
			std::remove(nomLivre.c_str());
			return false;
		}
	}
	else {
		incantationSQL("SELECT * FROM 'Master';", "setMaster2mem");
		incantationSQL("SELECT * FROM 'Catégories';", "setCategories2mem");
		incantationSQL("SELECT * FROM 'Transactions' ORDER BY Date;", "setTransactions2mem");
		incantationSQL("SELECT * FROM 'Favorites';", "setFavorites2mem");
	}
	return true;
}

bool LivreComptable::ajoutCompte() {
	std::string NOM = "";
	std::string NoREF = "";
	std::string compte = "";
	std::string TYPE = "";
	std::string DEPART = "";
	std::string CONCAT = "INSERT INTO Master(Nom, Ref, Type, Départ, Présent) VALUES(";

	line_read = readline("\tNom du compte     : ");
	NOM = line_read;
	free(line_read);
	if (NOM == "") return false;
	CONCAT += "\"" + NOM + "\", ";

	line_read = readline("\t# de référence    : ");
	NoREF = line_read;
	free(line_read);
	if (NoREF == "") return false;
	CONCAT += "\"" + NoREF + "\", ";

	do {
		std::cout << "\tType de compte    : 1- Compte courant, 2- Compte épargne, 3- Carte de crédit ? "
					<< std::flush;
		std::getline(std::cin, compte);
		if (compte == "1") TYPE = "Courant";
		else if (compte == "2") TYPE = "Épargne";
		else if (compte == "3") TYPE = "Crédit";
		else if (compte == "") return false;
		else TYPE = "";
	} while (TYPE == "");
	CONCAT += "\"" + TYPE + "\", ";

	do {
		std::cout << "\tMontant de départ : " << std::flush;
		std::cin >> DEPART;
		if (!testMontant(DEPART)) DEPART = "";
	} while (DEPART == "");
	std::cin.ignore(256, '\n');
	// Convertir DEPART 123,56 en cents -> 12356
	DEPART = dollars2cents(DEPART);

	CONCAT += DEPART + ", " + DEPART + ");";
	incantationSQL(CONCAT, "");
	COMPTES.push_back((struct compte) {NOM, NoREF, TYPE, DEPART, DEPART});

	// Création des catégories avec les comptes pour les virements et paiement.

	CONCAT = "INSERT INTO Catégories(Nom, Utilisé, Type) VALUES(\"" + NOM + "\", 1000, \"" + TYPE + "\");";
	incantationSQL(CONCAT, "");
	CATEGORIES.push_back((struct categorie) {NOM, 1000, TYPE});

	return true;
}
void LivreComptable::suppCompte() {
	TMP_COMPTES.clear();
	std::vector<int>cmpteIDX;
	std::vector<int>catIDX;

	for (auto i = 0; i < COMPTES.size(); i++) {
		if (COMPTES[i].Départ == COMPTES[i].Présent) {
			for (auto j = 0; j < CATEGORIES.size(); j++) {
				if (COMPTES[i].Nom == CATEGORIES[j].Nom && CATEGORIES[j].Utilisé == 1000) {
					TMP_COMPTES.push_back(COMPTES[i]);
					cmpteIDX.push_back(i);
					catIDX.push_back(j);
				}
			}
		}
	}
	if (TMP_COMPTES.size() > 0) {
		printComptes(TMP_COMPTES);
		if (TMP_COMPTES.size() == 1) {
			std::cout << "Supprimer ce compte ?" << std::endl;
			std::cout << "[n/N pour annuler]: " << std::flush;
			reponse = "";
			std::getline(std::cin, reponse);
			if (reponse == "n" || reponse == "N") return;
			else {
				std::string reqSQL = "DELETE FROM Master WHERE Nom = \"";
				reqSQL += COMPTES[cmpteIDX[0]].Nom + "\" AND Départ = ";
				reqSQL += COMPTES[cmpteIDX[0]].Départ + ";";
				incantationSQL(reqSQL, "");

				reqSQL = "DELETE FROM Catégories WHERE Nom = \"";
				reqSQL += CATEGORIES[catIDX[0]].Nom + "\" AND Utilisé = ";
				reqSQL += std::to_string(CATEGORIES[catIDX[0]].Utilisé) + ";";
				incantationSQL(reqSQL, "");

				COMPTES.erase(COMPTES.begin() + cmpteIDX[0]);
				CATEGORIES.erase(CATEGORIES.begin() + catIDX[0]);
			}
		}
		else {
			std::cout << "Choisir le compte à supprimer par son #" << std::endl;
			if (!getReponse(TMP_COMPTES.size())) return;

			std::string reqSQL = "DELETE FROM Master WHERE Nom = \"";
			reqSQL += COMPTES[cmpteIDX[choix - 1]].Nom + "\" AND Départ = ";
			reqSQL += COMPTES[cmpteIDX[choix - 1]].Départ + ";";
			incantationSQL(reqSQL, "");

			reqSQL = "DELETE FROM Catégories WHERE Nom = \"";
			reqSQL += CATEGORIES[catIDX[choix - 1]].Nom + "\" AND Utilisé = ";
			reqSQL += std::to_string(CATEGORIES[catIDX[choix - 1]].Utilisé) + ";";
			incantationSQL(reqSQL, "");

			COMPTES.erase(COMPTES.begin() + cmpteIDX[choix - 1]);
			CATEGORIES.erase(CATEGORIES.begin() + catIDX[choix -1]);
		}
	}
	else std::cout << "Aucun compte ne peut être supprimé pour l'instant" << std::endl;
}
bool LivreComptable::ajoutCategorie() {
	std::string SQLcommande = "";
	std::string NOM = "";
	std::string TYPE = "";

	line_read = readline("Nouvelle catégorie : ");
	NOM = line_read;
	free(line_read);

	if (NOM == "") return false;

	do {
		std::cout << "\tType de catégorie    : 1- En entrée, 2- En sortie ? " << std::flush;
		std::getline(std::cin, TYPE);
		if (TYPE == "1") TYPE = "IN";
		else if (TYPE == "2") TYPE = "OUT";
		else if (TYPE == "") return false;
		else TYPE = "";
	} while (TYPE == "");

	SQLcommande = "INSERT INTO Catégories(Nom, Utilisé, Type) VALUES(\"" + NOM + "\", 0, \"" + TYPE + "\");";
	incantationSQL(SQLcommande,"");
	CATEGORIES.push_back((struct categorie) {NOM, 0, TYPE});

	return true;
}
void LivreComptable::suppCategorie() {
	TMP_CATEGORIES.clear();
	std::vector<int>catIDX;

	for (auto i = 0; i < CATEGORIES.size(); i++) {
		if (CATEGORIES[i].Utilisé == 0) {
			TMP_CATEGORIES.push_back(CATEGORIES[i]);
			catIDX.push_back(i);
		}
	}
	if (TMP_CATEGORIES.size() > 0) {
		printCategories(TMP_CATEGORIES);
		if (TMP_CATEGORIES.size() == 1) {
			std::cout << "Supprimer cette catégorie ?" << std::endl;
			std::cout << "[n/N pour annuler]: " << std::flush;
			reponse = "";
			std::getline(std::cin, reponse);
			if (reponse == "n" || reponse == "N") return;
			else {
				std::string reqSQL = "DELETE FROM Catégories WHERE Nom = \"";
				reqSQL += CATEGORIES[catIDX[0]].Nom + "\" AND Utilisé = ";
				reqSQL += std::to_string(CATEGORIES[catIDX[0]].Utilisé) + ";";
				incantationSQL(reqSQL, "");

				CATEGORIES.erase(CATEGORIES.begin() + catIDX[0]);
			}
		}
		else {
			std::cout << "Choisir la catégorie à supprimer par son #" << std::endl;
			if (!getReponse(TMP_CATEGORIES.size())) return;

			std::string reqSQL = "DELETE FROM Catégories WHERE Nom = \"";
			reqSQL += CATEGORIES[catIDX[choix - 1]].Nom + "\" AND Utilisé = ";
			reqSQL += std::to_string(CATEGORIES[catIDX[choix - 1]].Utilisé) + ";";
			incantationSQL(reqSQL, "");

			CATEGORIES.erase(CATEGORIES.begin() + catIDX[choix -1]);
		}
	}
	else std::cout << "Aucune catégorie ne peut être supprimée pour l'instant" << std::endl;
}
void LivreComptable::suppFavorite() {
	std::cout << "Choisir une transaction favorite par son numéro," << std::endl;
	printTransactions(FAV_TRANSACTIONS, true);
	if (!getReponse(FAV_TRANSACTIONS.size())) return;

	choix--;
	std::string reqSQL = "DELETE FROM Favorites WHERE Description = \"";
				reqSQL += FAV_TRANSACTIONS[choix].Description + "\" AND Montant = ";
				reqSQL += FAV_TRANSACTIONS[choix].Montant + ";";
	incantationSQL(reqSQL, "");

	FAV_TRANSACTIONS.erase(FAV_TRANSACTIONS.begin() + choix);
}
void LivreComptable::modifFavorite() {
	std::cout << "Choisir une transaction favorite par son numéro," << std::endl;
	printTransactions(FAV_TRANSACTIONS, true);
	if (!getReponse(FAV_TRANSACTIONS.size())) return;

	choix--;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;
	while (true) {
		reponse = "";
		std::cout << "Nouveau montant pour «" << FAV_TRANSACTIONS[choix].Description << "» : " << std::flush;
		std::getline(std::cin, reponse);
		if (reponse == "") return;

		if (testMontant(reponse)) break;
		else std::cout << "Montant mal formatté, xxx,xx ..." << std::endl; 
	};
	std::cout << "-------------------------------------------------------------------------------" << std::endl;

	std::string reqSQL = "UPDATE Favorites SET Montant = " + dollars2cents(reponse) + " WHERE ";
				reqSQL += "Description = \"" + FAV_TRANSACTIONS[choix].Description + "\";";
	incantationSQL(reqSQL, "");

	FAV_TRANSACTIONS[choix].Montant = dollars2cents(reponse);
}

void LivreComptable::sommaireMois() {
	std::string mois[] = {"janvier", "février", "mars", "avril", "mai", "juin",
							"juillet", "août", "septembre", "octobre", "novembre", "décembre"};
	TMP_TRANSACTIONS.clear();
	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << "De quel compte voulez-vous le sommaire ?" << std::endl;
	printAllComptes();
	if (!getReponse(COMPTES.size())) return;
	std::cout << "--------------------------------------------------------" << std::endl;
	std::string selection = COMPTES[choix - 1].Nom;	

	for (int i = 0; i < TRANSACTIONS.size(); i++) {
		if (TRANSACTIONS[i].Compte == selection || TRANSACTIONS[i].Catégorie == selection) {
			TMP_TRANSACTIONS.push_back(TRANSACTIONS[i]);
		}
	}

	bool credit = COMPTES[choix - 1].Type == "Crédit";
	int moisCourant = 0;
	int start = atoi(COMPTES[choix - 1].Départ.c_str());
	int totalCredit, totalDebit, totalMois;
	totalCredit = totalDebit = totalMois = 0;

	int padDesc = 50;
	int padMontant = 10;
	std::string borderDesc = "──";
	std::string borderMontant = "──";
	for (int i = 0; i < padDesc; i++) borderDesc += "─";
	for (int i = 0; i < padMontant; i++) borderMontant += "─";

	auto pt = [&, this]() {
		if (credit) totalMois = totalDebit - totalCredit;
		else totalMois = totalCredit - totalDebit;
		std::cout << "             ┌────────────────────────────────────────────────────┬"
					<< "────────────┬────────────┬────────────┐" << std::endl;
		std::cout << "             │ " << std::setw(padDesc) << std::left << "Totaux du mois" << " │ " << std::flush;
		std::cout << std::setw(padMontant) << std::right << cents2dollars(std::to_string(totalCredit)) << " │ " << std::flush;
		std::cout << std::setw(padMontant) << std::right << cents2dollars(std::to_string(totalDebit)) << " │ " << std::flush;
		std::cout << std::setw(padMontant) << std::right << cents2dollars(std::to_string(totalMois)) << " │" << std::endl;
		std::cout << "             └────────────────────────────────────────────────────┴"
					<< "────────────┴────────────┴────────────┘\n" << std::endl;
		totalCredit = totalDebit = totalMois = 0;
	};

	for (auto& t : TMP_TRANSACTIONS) {
		int moisTransaction = atoi(t.Date.substr(5,2).c_str());
		if (moisCourant != moisTransaction) {	// Nous venons de changer de mois: print header
			if (moisCourant != 0) pt();		// Imprime le calcul seulement au premier changement
			moisCourant = moisTransaction;
			std::cout << "Sommaire de " << selection << " pour le mois de " << mois[moisCourant - 1]
						<< " " << t.Date.substr(0, 4) << std::endl;
			std::cout << "┌────────────┬" << borderDesc << "┬" << borderMontant << "┬"
						<< borderMontant << "┬" << borderMontant << "┐" << std::endl;
			std::cout << "│ Date       │ " << std::setw(padDesc) << std::left << "Description" << " │ "
						<< std::setw(padMontant + 1) << std::right << "Crédit"  << " │ "	// + 1 à cause
						<< std::setw(padMontant + 1) << std::right << "Débit"  << " │ "		//  de l'accent
						<< std::setw(padMontant) << std::right << "Total"  << " │" << std::endl;
			std::cout << "└────────────┴" << borderDesc << "┴" << borderMontant << "┴"
						<< borderMontant << "┴" << borderMontant << "┘" << std::endl;
		}
		int pad = padDesc;
		twistAccent(t.Description.c_str(), pad);

		std::cout << "│ " << t.Date << " │ " << std::setw(pad) << std::left << t.Description << " │ ";

		if (t.Type == "Virement" || t.Type == "Paiement") {
			if(selection == t.Catégorie) t.Type = "Crédit";
			else t.Type = "Débit";
		}
		int f_Montant = atoi(t.Montant.c_str());

		if (t.Type == "Crédit" || t.Type == "Dépôt") {
			std::cout << std::setw(padMontant) << std::right << cents2dollars(t.Montant) << " │ "
						<< std::setw(padMontant) << std::right << " " << " │ ";
			totalCredit += f_Montant;
			if (credit) start -= f_Montant;
			else start += f_Montant;
		}
		else {	// Un débit ou un achat puisqu'il n'y a plus de virement ni de paiement
			std::cout << std::setw(padMontant) << std::right << " " << " │ "
						<< std::setw(padMontant) << std::right << cents2dollars(t.Montant) << " │ ";
			totalDebit += f_Montant;
			if (credit) start += f_Montant;
			else start -= f_Montant;
		}

		std::cout << std::setw(padMontant) << std::right << cents2dollars(std::to_string(start))
																							<< " │" << std::endl;
		if (&t == &TMP_TRANSACTIONS.back()) pt();	// // Imprime le calcul si nous sommes au bout
	}
}

int LivreComptable::setMaster2mem(char **argv) {
	struct compte c;

	c.Nom = argv[0];
	c.Ref = argv[1];
	c.Type = argv[2];
	c.Départ = argv[3];
	c.Présent = argv[4];
	COMPTES.push_back(c);

	return 0;
}
int LivreComptable::setCategories2mem(char **argv) {
	struct categorie c;

	c.Nom = argv[0];
	c.Utilisé = atoi(argv[1]);
	c.Type = argv[2];
	CATEGORIES.push_back(c);

	return 0;
}
int LivreComptable::setFavorites2mem(char ** argv) {
	struct transaction t;

	t.Date = argv[0];
	t.Description = argv[1];
	t.Type = argv[2];
	t.Compte = argv[3];
	t.Catégorie = argv[4];
	t.Montant = argv[5];

	FAV_TRANSACTIONS.push_back(t);

	return 0;
}
int LivreComptable::setTransactions2mem(char ** argv) {
	struct transaction t;

	t.Date = argv[0];
	t.Description = argv[1];
	t.Type = argv[2];
	t.Compte = argv[3];
	t.Catégorie = argv[4];
	t.Montant = argv[5];

	TRANSACTIONS.push_back(t);

	return 0;
}
int LivreComptable::setQuestion2mem(char ** argv) {
	struct transaction t;

	t.Date = argv[0];
	t.Description = argv[1];
	t.Type = argv[2];
	t.Compte = argv[3];
	t.Catégorie = argv[4];
	t.Montant = argv[5];

	TMP_TRANSACTIONS.push_back(t);

	return 0;
}

void LivreComptable::questionBD() {
	int total = 0;

	std::string requete = "SELECT * FROM 'Transactions' WHERE ";
	
	std::cout << "Formulez votre question à la base de données:" << std::endl;
	line_read = readline("Exemple: «Catégorie LIKE \"\%Épicerie\%\"» ? ");
	add_history(line_read);

	reponse = line_read;
	free(line_read);

	requete = requete + reponse + " ORDER BY Date;";
	std::cout << "--------------------------------------------------------" << std::endl;
	TMP_TRANSACTIONS.clear();
	incantationSQL(requete, "setQuestion2mem");
	printTransactions(TMP_TRANSACTIONS);

	for (auto t : TMP_TRANSACTIONS) total += atoi(t.Montant.c_str());
	if (total != 0) {
		std::cout << "┌─────────────────────────────────────────────┬─────────────┐\n"
				<< "│ Total des éléments de la liste en requête   │ $ "
				<< std::right << std::setw(9) << cents2dollars(std::to_string(total)) << " │\n"
				<< "└─────────────────────────────────────────────┴─────────────┘" << std::endl;
	}
}
void LivreComptable::fullQuestionBD() {

	std::cout << "Formulez votre requête complexe à la base de données:" << std::endl;
	std::cout << "Exemple: «UPDATE Transactions SET Date = \"2024-03-27\" WHERE Type = \"Crédit\";»" << std::endl;
	std::cout << "ATTENTION, IL N'Y A PAS DE DEUXIÈME CHANCE ? \n" << std::endl;
	line_read = readline(" ? : > ");
	add_history(line_read);

	reponse = line_read;
	free(line_read);

	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << "Lancement de la commande «" << reponse << "»" << std::endl;
	cbackPassage = false;
	incantationSQL(reponse, reponse);
}

void LivreComptable::printType() {
	int nbreTypes = typeTransaction.size();
	int nbreParLigne = 3;
	int nbreRangee = nbreTypes / nbreParLigne;
	int nbreReste = nbreTypes % nbreParLigne;
	int seq = 0;

	// lambda pour éviter de réécrir du code	----------------
	auto pt = [seq, this](int iterateur) mutable {
		int namePad = 25;
		std::string border = "──";
		for (size_t i = 0; i < namePad; i++) border += "─";
		
		for ( int i = 0; i < iterateur; i++) {
			std::cout << "┌────┬" << border << "┐";
		}
		std::cout << std::endl;
		for ( int i = 0; i < iterateur; i++) {
			auto t = typeTransaction[seq];
			std::cout << "│ " << std::setw(2) << std::right << seq +1;
			int pad = namePad;
			twistAccent(t.c_str(), pad);
			std::cout << " │ " << std::setw(pad) << std::left << t.substr(0, pad) << " │";
			seq++;
		}
		std::cout << std::endl;
		for ( int i = 0; i < iterateur; i++) {
			std::cout << "└────┴" << border << "┘";
		}
		std::cout << std::endl;
	};
	//	--------------------------------------------------------
	for (int index = 0; index < nbreRangee; index++) pt(nbreParLigne);

	if (nbreReste) pt(nbreReste);
}
void LivreComptable::printComptes(std::vector<struct compte>&cmpt) {
	int nbreComptes = cmpt.size();
	int nbreParligne = 2;
	int nbreRangee = nbreComptes / nbreParligne;
	int nbreReste = nbreComptes % nbreParligne;
	int seq = 0;

	// lambda pour éviter de réécrir du code	----------------
	auto pt = [seq, cmpt, this](int iterateur) mutable {
		int namePad = 32;
		int dollarPad = 10;
		std::string borderNom = "──";
		std::string borderDollar = "──";
		for (int i = 0; i < namePad; i++) borderNom += "─";
		for (int i = 0; i < dollarPad + 2; i++) borderDollar += "─";	// À cause du $

		for ( int i = 0; i < iterateur; i++) {
			std::cout << "┌────┬" << borderNom << "┬" << borderDollar << "┐";
		}
		std::cout << std::endl;
		for ( int i = 0; i < iterateur; i++) {
			struct compte c = cmpt[seq];
			std::cout << "│ " << std::setw(2) << std::right << seq +1;
			int pad = namePad;
			twistAccent(c.Nom.c_str(), pad);
			std::cout << " │ " << std::setw(pad) << std::left << c.Nom.substr(0, pad)
					<< " │ $ " << std::right << std::setw(dollarPad) << cents2dollars(c.Départ) << " │";
			seq++;
		}
		std::cout << std::endl;
			seq-= iterateur;
		for ( int i = 0; i < iterateur; i++) {
			struct compte c = cmpt[seq];
			int pad = namePad;
			twistAccent(c.Ref.c_str(), pad);
			std::cout << "└────┤ " << std::setw(pad) << std::left << c.Ref.substr(0, pad)
						<< " │ $ " << std::right << std::setw(dollarPad) << cents2dollars(c.Présent) << " │";
			seq++;
		}
		std::cout << std::endl;
		for ( int i = 0; i < iterateur; i++) {
		std::cout << "     └" << borderNom << "┴" << borderDollar << "┘";
		}
		std::cout << std::endl;
	};
	//	--------------------------------------------------------
	for (int index = 0; index < nbreRangee; index++) {
		pt(nbreParligne);
	}
	if (nbreReste) pt(nbreReste);
}
void LivreComptable::printAllComptes() {
	printComptes(COMPTES);
}
void LivreComptable::printCategories(std::vector<struct categorie> &catgrie)
{
	int nbreCatgries = catgrie.size();
	int nbreParLigne = 3;
	int nbreRangee = nbreCatgries / nbreParLigne;
	int nbreReste = nbreCatgries % nbreParLigne;
	int seq = 0;

	// lambda pour éviter de réécrir du code	----------------
	auto pt = [seq, catgrie, this](int iterateur) mutable {
		int namePad = 25;
		std::string borderNom = "──";
		for (int i = 0; i < namePad; i++) borderNom += "─";

		for ( int i = 0; i < iterateur; i++) {
			std::cout << "┌────┬" << borderNom << "┐";
		}
		std::cout << std::endl;
		for ( int i = 0; i < iterateur; i++) {
			struct categorie c = catgrie[seq];
			std::cout << "│ " << std::setw(2) << std::right << seq + 1;
			int pad = namePad;
			twistAccent(c.Nom.c_str(), pad);
			std::cout << " │ " << std::setw(pad) << std::left << c.Nom.substr(0, pad) << " │";
			seq++;
		}
		std::cout << std::endl;
		for ( int i = 0; i < iterateur; i++) {
			std::cout << "└────┴" << borderNom << "┘";
		}
		std::cout << std::endl;
	};
	//	--------------------------------------------------------
	for (int index = 0; index < nbreRangee; index++) {
		pt(nbreParLigne);
	}
	if (nbreReste) pt(nbreReste);
}
void LivreComptable::printAllCategories() {
	printCategories(CATEGORIES);
}
void LivreComptable::print1Transaction(struct transaction &t) {
	std::vector<struct transaction> trans;
	trans.push_back(t);
	printTransactions(trans);
}
void LivreComptable::printTransactions(std::vector<struct transaction> &trans, bool count) {
	if (trans.size() > 0) {	// printCompactTransaction(trans);
		for (auto i =0; i < trans.size(); i++) {
			struct transaction t = trans[i];
			int padDate = 10;
			int padDesc = 30;
			int padType = 11;
			int padCompte = 33;
			int padMontant = padType - 2;
			int padCat = padCompte;

			if (count) padDate = 2;

			std::string borderDesc = "──";
			for (int i = 0; i < padDesc; i++) borderDesc += "─";
			std::string borderType = "──";
			for (int i = 0; i < padType; i++) borderType += "─";
			std::string borderCompte = "──";
			for (int i = 0; i < padCompte + 6; i++) borderCompte += "─";	// + 6 == De: - Vers:

			std::string desc1, desc2;
			int padD1 = padDesc, padD2 = padDesc;

			twistAccent(t.Description.c_str(), padD1);
			if (t.Description.length() > padD1) {	// "Chaîne trop longue: faut splitter"
				padD1 = padDesc;
				int index = 0;	// Pour trouver où couper: un hybride de twistAccent()
				for (auto i = 0; i < t.Description.length(); i++) {
					if (t.Description[i] == ' ') {
						if (i > padD1) break;
						else { index = i; continue; }
					}
					if ((unsigned int)t.Description[i] >= '\x80') { padD1++; i++; }
					if ((unsigned int)t.Description[i] >= '\xE0') { padD1++; i++; }
					if ((unsigned int)t.Description[i] >= '\xF0') { padD1++; i++; }
				}
				desc1 = t.Description.substr(0, index);
				padD1 = padDesc;
				twistAccent(desc1.c_str(), padD1);
				desc2 = t.Description.substr(index + 1);
				twistAccent(desc2.c_str(), padD2);
			}
			else desc1 = t.Description;
			
			twistAccent(t.Type.c_str(), padType);
			twistAccent(t.Compte.c_str(), padCompte);
			twistAccent(t.Catégorie.c_str(), padCat);

			if (i == 0 && count) std::cout << "┌────┬"
								<< borderDesc << "┬" << borderType << "┬" << borderCompte << "┐" << std::endl;
			if (i == 0 && !count) std::cout << "┌────────────┬"
								<< borderDesc << "┬" << borderType << "┬" << borderCompte << "┐" << std::endl;

			if (count) std::cout << "│ " << std::right << std::setw(padDate) << i + 1 << std::flush; 
			else std::cout << "│ " << std::setw(padDate) << t.Date << std::flush; 

			std::cout << " │ " << std::left << std::setw(padD1) << desc1 << " │ " << std::left
						<< std::setw(padType) << t.Type << " │   De: " << std::left << std::setw(padCompte)
						<< t.Compte << " │" << std::endl;
			if (count) std::cout << "└────┤ " << std::flush;
			if (!count) std::cout << "└────────────┤ " << std::flush;

			std::cout << std::left << std::setw(padD2) << desc2.substr(0, padD2) << " │ $ " << std::right
						<< std::setw(padMontant) << cents2dollars(t.Montant) << std::left
						<< " │ Vers: " << std::left << std::setw(padCat) << t.Catégorie << " │" << std::endl;

			if (i == trans.size() - 1 && count)  std::cout << "     └" << borderDesc << "┴" << borderType
																	<< "┴" << borderCompte << "┘" << std::endl;
			else if (i == trans.size() - 1 && !count)  std::cout << "             └" << borderDesc << "┴" << borderType
																	<< "┴" << borderCompte << "┘" << std::endl;

			else {
				if (count) std::cout << "┌────┤" << borderDesc << "┼" << borderType << "┼"
																		<< borderCompte << "│" << std::endl;
				if (!count) std::cout << "┌────────────┤" << borderDesc << "┼" << borderType << "┼"
																		<< borderCompte << "│" << std::endl;
			}
		}
	}
}
void LivreComptable::printAllFavorites() {
	printTransactions(FAV_TRANSACTIONS);
}

bool LivreComptable::nouvelleTransaction(bool favorite) {
	struct transaction t;
	bool copie = false;

	//	Implémentation de Date
	if (!favorite) {
		std::cout << "-------------------------------------------------------------------------------" << std::endl;
		while (true) {
			std::cout << "Entrez la date de la transaction,\n[0 pour annuler, enter pour accepter]: "
															<< d.derniereEntree << " ? " << std::flush;
			std::getline(std::cin, reponse);

			if (reponse == "0") return false;
			else if (reponse == "") break;
			if (!d.setCheckDate(reponse)) std::cout << "Date entrée invalide : [[AAAA-]MM-]JJ" << std::endl;
			else break;
		}
		t.Date = d.derniereEntree;
	}
	else t.Date = "----------";

	//	Implémentation de Description
	std::cout << "-------------------------------------------------------------------------------" << std::endl;
	if (favorite) std::cout << "Une courte description \n[0 ou enter pour annuler]: " << std::flush;
	else std::cout << "Une courte description ou @, pour appeler une transaction favorite\n"
															"[0 ou enter pour annuler]: " << std::flush;
	line_read = readline("");
	reponse = line_read;
	free(line_read);

	if (reponse == "0" || reponse == "") return false;
	if (!favorite && reponse == "@") {
		std::cout << "-------------------------------------------------------------------------------" << std::endl;
		std::cout << "Choisir une transaction favorite par son numéro," << std::endl;
		printTransactions(FAV_TRANSACTIONS, true);
		if (!getReponse(FAV_TRANSACTIONS.size())) return false;
		std::cout << "-------------------------------------------------------------------------------" << std::endl;

		choix--;
		t.Description = FAV_TRANSACTIONS[choix].Description;
		t.Type = FAV_TRANSACTIONS[choix].Type;
		t.Compte = FAV_TRANSACTIONS[choix].Compte;
		t.Catégorie = FAV_TRANSACTIONS[choix].Catégorie;
		t.Montant = FAV_TRANSACTIONS[choix].Montant;
		copie = true;
	}
	else {
		t.Description = reponse;

		// Implémentation de Type
		std::cout << "-------------------------------------------------------------------------------" << std::endl;
		std::cout << "Choisir le type par son numéro," << std::endl;
		printType();
		if (!getReponse(typeTransaction.size())) return false;
		t.Type = typeTransaction[choix - 1];

		//	Set les comptes et catégories à faire afficher
		TMP_COMPTES.clear();
		TMP_CATEGORIES.clear();

		//	En référence à typeTransaction = {"Dépôt", "Débit", "Crédit", "Achat", "Virement", "Paiement"};
		const char* compteTest[][3] = {{"Courant"}, {"Courant", "Crédit"}, {"Courant", "Épargne", "Crédit"},
										{"Courant", "Crédit"}, {"Courant", "Épargne"}, {"Courant"}};

		const char* categorieTest[][4] = {{"IN"}, {"OUT"}, {"IN", "Courant", "Épargne", "Crédit"},
											{"OUT"}, {"Courant", "Épargne"}, {"Crédit"}};

		for (auto i = 0; i < typeTransaction.size(); i++) {
			if (typeTransaction[i] == t.Type) {
				for (const char* element : compteTest[i]) {
					if (element != nullptr) {
						for (auto c : COMPTES) if (c.Type == element) TMP_COMPTES.push_back(c);
					}
					else break;	// Sortir de la boucle dès qu'on rencontre un nullptr
				}
				for (const char* element : categorieTest[i]) {
					if (element != nullptr) {
						for (auto c : CATEGORIES) if (c.Type == element) TMP_CATEGORIES.push_back(c);
					}
					else break;	// Sortir de la boucle dès qu'on rencontre un nullptr
				}
				break;	// Sortir de la boucle dès qu'on a trouvé le bon type
			}
		}
		//	Implémentation de Compte
		std::cout << "-------------------------------------------------------------------------------" << std::endl;
		std::cout << "Choisir un compte par son numéro," << std::endl;
		printComptes(TMP_COMPTES);
		if (!getReponse(TMP_COMPTES.size())) return false;
		t.Compte = TMP_COMPTES[choix - 1].Nom;

		//	Implémentation de Catégorie
		std::cout << "-------------------------------------------------------------------------------" << std::endl;
		std::cout << "Choisir une catégorie par son numéro," << std::endl; 
		printCategories(TMP_CATEGORIES);
		if (!getReponse(TMP_CATEGORIES.size())) return false;
		t.Catégorie = TMP_CATEGORIES[choix - 1].Nom;
	}
		//	Implémentation de Montant
	if (!copie || (copie && (t.Montant == "0" || t.Montant == "0.0"))) {
		if (!copie)
			std::cout << "-------------------------------------------------------------------------------" << std::endl;
		while (true) {
			reponse = "";
			std::cout << "Pour un montant de : " << std::flush;
			std::getline(std::cin, reponse);
			if (favorite && reponse == "0") {
				t.Montant = reponse;
				break;
			}
			if (reponse == "" || reponse == "0") return false;

			if (testMontant(reponse)) {
				t.Montant = dollars2cents(reponse);
				break;
			}
			else std::cout << "Montant mal formatté, xxx,xx ..." << std::endl; 
		};
		std::cout << "-------------------------------------------------------------------------------" << std::endl;
	}
	print1Transaction(t);
	if (favorite) {
		FAV_TRANSACTIONS.push_back(t);
		std::string pubFav = "INSERT INTO FAvorites(Date, Description, Type, Compte, Catégorie, Montant) VALUES(\"";
		pubFav += t.Date + "\", \"" + t.Description + "\", \"" + t.Type  + "\", \"" + t.Compte
													+ "\", \"" + t.Catégorie + "\", " + t.Montant + ");";
		incantationSQL(pubFav, "");
	}
	else publishTransaction(t);
	return true;
}

void LivreComptable::fermeLivre() {
//	Ferme la base de données
	sqlite3_close(bd);
}

//	Fin de définition de la classe Livre Comptable

int main(int argc, char *argv[]) {

	std::string lcdBookName = "";
	std::string reponse;
	bool nouveau = false;

	switch (argc) {
		case 1: {
			// Nous n'avons pas d'argument: set un defaut comme fileName
			// en rapport avec l'année courante
			std::time_t t = std::time(nullptr);
			std::tm *const pTInfo = std::localtime(&t);
			lcdBookName = "LivreComptable." + std::to_string(1900 + pTInfo->tm_year);
			break;
		}

		case 2:
			// Le nom du livre se trouve en argument
			lcdBookName = argv[1];
			break;

		default:
			std::cout << "Trop d'arguments" << std::endl;
			return 10;
			break;
	}

	std::ifstream file(lcdBookName, std::ios::binary);
	if (file.good()) {
		char buffer[16];
		file.read(buffer, sizeof(buffer));
		if (std::string(buffer) != "SQLite format 3\000") {
			std::cout << "«" << lcdBookName << "» n'est pas une base de données SQLite 3" << std::endl;
			return 10;
		}
	}
	else {
		std::cout << "«" << lcdBookName << "» n'existe pas, Création d'un nouveau livre ? " << std::flush;
		std::getline(std::cin, reponse);
		if (reponse[0] == 'n' || reponse[0] == 'N') return 0;
		nouveau = true;
	}
	if (!livre.ouvreLivre(lcdBookName, nouveau)) {
		std::cout << "Gros problème: Ne peut ouvrir ou initialiser la base de données" << std::endl;
		return 10;
	}

/*	C'est ici que tout commence	*/

	std::vector<std::string> options = {" 1- Entrer une nouvelle transaction",
										" 2- Afficher tous les comptes",
										" 3- Ajouter un compte",
										" 4- Supprimer un compte",
										" 5- Afficher toutes les catégories",
										" 6- Ajouter une catégorie",
										" 7- Supprimer une catégorie",
										" 8- Afficher toutes les favorites",
										" 9- Ajouter une transaction favorite",
										"10- Modifier une transaction favorite",
										"11- Supprimer une transaction favorite",
										"12- Interroger les transactions",
										"13- Accès complet à la bd [PRO ONLY]",
										"14- Sommaire de comptes"
										};

	std::cout << "\n*************** Livre comptable à Daniel ***************\n"
				<< "--------------------------------------------------------" << std::endl;
	livre.printAllComptes();

	while (true) {
		std::cout << "--------------------------------------------------------" << std::endl;
		for (auto o : options) std::cout << o << std::endl;

	std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "Choisir un item du menu par son numéro," << std::endl;
		if (!getReponse(options.size())) break;

//		std::cout << "--------------------------------------------------------" << std::endl;

		if (choix == 1) { while (livre.nouvelleTransaction()) livre.printAllComptes(); }
		else if (choix == 2) livre.printAllComptes();
		else if (choix == 3) livre.ajoutCompte();
		else if (choix == 4) livre.suppCompte();
		else if (choix == 5) livre.printAllCategories();
		else if (choix == 6) livre.ajoutCategorie();
		else if (choix == 7) livre.suppCategorie();
		else if (choix == 8) livre.printAllFavorites();
		else if (choix == 9) livre.nouvelleTransaction(true);
		else if (choix == 10) livre.modifFavorite();
		else if (choix == 11) livre.suppFavorite();
		else if (choix == 12) livre.questionBD();
		else if (choix == 13) livre.fullQuestionBD();
		else if (choix == 14) livre.sommaireMois();
		else break;
	}
	livre.fermeLivre();

	return 0;
}
