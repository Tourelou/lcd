#include <iostream>
#include <iomanip>
#include "classLivreComptable.hpp"

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
