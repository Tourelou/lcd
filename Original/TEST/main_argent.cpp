#include <iostream>
#include "argent.hpp"

void pt(Argent &objet) {
	std::cout << "Avec l'opérateur << " << objet << std::endl;
	std::cout << "Avec la méthode getInt " << objet.getInt() << std::endl;
	std::cout << "La valeur de l'attribut strCents " << objet.strCents << std::endl;
	std::cout << std::endl;
}

int main() {

	std::string input;
	int in;
	Argent argent1;

	//	Instance temporaire pour cout
	std::cout << "Une instance temporaire: " << Argent(123458) << std::endl;

	while (true) {
		std::cout << "------------------------------------" << std::endl;
		std::cout << "Entrez une valeur comme string en cents [xxxxxx] faire -5000 pour sortir: " << std::flush;
		std::cin >> input;
		std::cout << "------------------------------------" << std::endl;
		if (argent1.setFromStrCents(input)) {
			if (input == "-5000") break;
			std::cout << "---- objet #1 ----" << std::endl;
			pt(argent1);
		}
		else std::cout << "Valeur invalide" << std::endl;

		std::cout << "------------------------------------" << std::endl;
		std::cout << "Entrez une valeur comme string [xxx,xx] faire -5000 pour sortir: " << std::flush;
		std::cin >> input;
		std::cout << "------------------------------------" << std::endl;

		if (Argent::testMontant(input)) {
			if (input == "-5000") break;
			Argent argent2(input);
			std::cout << "---- objet #2 ----" << std::endl;
			pt(argent2);

			std::cout << "objet #1 - objet #2 = " << argent1 - argent2 << std::endl;
			argent1 -= argent2;
			std::cout << "---- objet #1 - objet #2 ----" << std::endl;
			pt(argent1);
		}
		else std::cout << "Valeur invalide" << std::endl;

		std::cout << "------------------------------------" << std::endl;
		std::cout << "Entrez une valeur comme int en cents [xxxxxx] faire -5000 pour sortir: " << std::flush;
		std::cin >> in;
		std::cout << "------------------------------------" << std::endl;

		if (in == -5000) break;
		if (std::cin.fail())  {
            std::cin.clear(); // Réinitialiser l'état du flux
            std::cin.ignore(1024, '\n'); // Ignorer les caractères restants
            std::cout << "Valeur invalide" << std::endl;
        }
		else {
			Argent argent3(in);
			std::cout << "---- objet #3 ----" << std::endl;
			pt(argent3);

			std::cout << "objet #1 + objet #3 = " << argent1 + argent3 << std::endl;
			argent1 += argent3;
			std::cout << "---- objet #1 + objet #3 ----" << std::endl;
			pt(argent1);
		}
	}
	return 0;
}
