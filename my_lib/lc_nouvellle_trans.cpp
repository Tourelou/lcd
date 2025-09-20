#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include "global_vars.h"
#include "classLivreComptable.hpp"

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

