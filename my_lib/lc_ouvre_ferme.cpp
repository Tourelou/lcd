#include <iostream>
#include <unistd.h>	// chdir, getpid, getcwd
#ifdef __linux__
	#include <linux/limits.h>
#endif
#include <sqlite3.h>
#include "classLivreComptable.hpp"

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
	}
	return true;
}

void LivreComptable::fermeLivre() {
//	Ferme la base de données
	sqlite3_close(bd);
}
