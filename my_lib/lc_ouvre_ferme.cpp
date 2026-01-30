#include <iostream>
#include <algorithm>
#include <unistd.h>	// chdir, getpid, getcwd
#ifdef __linux__
	#include <linux/limits.h>
#endif
#include <sqlite3.h>
#include "classLivreComptable.hpp"

// Vérifie si un nom de compte existe dans le vecteur COMPTES et CATÉGORIES

template <typename T>
bool itemExiste(const std::vector<T>& ITEMS, const std::string& nom) {
	return std::any_of(ITEMS.begin(), ITEMS.end(), [&](const T& i) { 
		return i.Nom == nom; 
	});
}

void LivreComptable::valideFavoris() {
	auto it = FAV_TRANSACTIONS.begin();
	while (it != FAV_TRANSACTIONS.end()) {
		bool okCompte = itemExiste(COMPTES, it->Compte);
		bool okCat = itemExiste(CATEGORIES, it->Catégorie);

		if (!okCompte || !okCat) {
			// Affichage de l'erreur spécifique
			if (!okCompte) std::cout << "--------------------------------------------------------\n"
										<< "Compte invalide : " << it->Compte << std::endl;
			if (!okCat)    std::cout << "--------------------------------------------------------\n"
										<< "Catégorie invalide : " << it->Catégorie << std::endl;
			
			print1Transaction(*it);

			// code DELETE pour SQLite avant l'erase
			// Remplace temporairement les ' par '' (syntaxe SQLite pour l'échappement)
			std::string descSecurisee = it->Description;
			size_t pos = 0;

			while ((pos = descSecurisee.find("'", pos)) != std::string::npos) {
				descSecurisee.replace(pos, 1, "''");
				pos += 2;
			}
			std::string sql = "DELETE FROM Favorites WHERE Description = '" + descSecurisee + 
								"' AND Montant = " + it->Montant + ";";

			incantationSQL(sql, "");
			it = FAV_TRANSACTIONS.erase(it);
			// On ne fait pas ++it ici car erase() nous place déjà sur le suivant
		} 
		// C'est ici que l'incrémentation doit se faire
		else { ++it; }
	}
}

bool LivreComptable::ouvreLivre(std::string nomLivre, bool nouveau) {
//	Ouvre la base de données
	int rc;
	rc = sqlite3_open(nomLivre.c_str(), &bd);
	if(rc) {
		std::cerr << "Ne peut ouvrir la base de données: " << sqlite3_errmsg(bd) << std::endl;
		return false;
	}
	std::cout << "Ouverture de la base de donnée «" << nomLivre << "»"<< std::endl;
	char currDir[PATH_MAX];
	getcwd(currDir, PATH_MAX);
	std::cout << "Du répertoire «" << currDir << "»" << std::endl;
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

		valideFavoris();
	}
	return true;
}

void LivreComptable::fermeLivre() {
//	Ferme la base de données
	sqlite3_close(bd);
}
