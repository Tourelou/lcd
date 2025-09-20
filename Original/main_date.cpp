#include <iostream>
#include "amj_date.hpp"

int main() {

	if (AMJ_Date::testDate("2000-02-29")) std::cout << "2000-02-29 check OK" << std::endl;
	else std::cout << "Désolé, la date est invalide ..." << std::endl;
	AMJ_Date d;

	while (true) {
		std::cout << "Date: " << d.derniereEntree << " ? " << std::flush;
		std::string reponse;
		std::getline(std::cin, reponse);

		if (reponse == "0") break;
		if (reponse == "") std::cout << "Date proposée acceptée" << std::endl;
		else {
			if (d.setCheckDate(reponse)) std::cout << "Date modifiée: " << d.derniereEntree << std::endl;
			else std::cout << "Date entrée invalide : [[AAAA-]MM-]JJ : 0 Pour sortir" << std::endl;
		}
	}
	if (d.setCheckDate("2024/12/31")) std::cout << d.derniereEntree << std::endl;
	std::cout << d.aujourdhui << std::endl;

	return 0;
}
