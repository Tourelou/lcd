#include <iostream>
#include <iomanip>
#include "global_vars.h"
#include "classLivreComptable.hpp"

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
