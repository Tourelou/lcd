#include <iostream>
#include "global_vars.h"
#include "classLivreComptable.hpp"

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
