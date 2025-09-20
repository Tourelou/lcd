#include <iostream>
#include <iomanip>
#include "classLivreComptable.hpp"

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
