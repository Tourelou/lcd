#include <algorithm>
#include "classLivreComptable.hpp"

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
