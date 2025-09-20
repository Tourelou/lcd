#include <iostream>
#include <iomanip>
#include "classLivreComptable.hpp"

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
