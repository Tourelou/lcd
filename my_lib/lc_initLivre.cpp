#include <iostream>
#include "global_vars.h"
#include <sqlite3.h>
#include "classLivreComptable.hpp"

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
