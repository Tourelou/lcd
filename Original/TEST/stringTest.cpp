#include <iostream>
#include <vector>

std::vector<std::string> typeTransaction = {"Dépôt", "Débit", "Crédit", "Achat", "Virement", "Paiement"};

int main() {
/*	struct compteTest {
		std::string depot[1] = {"Courant"};
		std::string debit[2] = {"Courant", "Crédit"};
		std::string credit[3] = {"Courant", "Épargne", "Crédit"};
		std::string achat[2] = {"Courant", "Crédit"};
		std::string virement[2] = {"Courant", "Épargne"};
		std::string paiement[2] = {"Courant", "Crédit"};
	} ct;
*/

	const char* compteTest[][3] = {
		{"Courant"},
		{"Crédit", "Débit"},
		{"Épargne", "Livret A", "PEL"},
		{"Investissement"}
	};

	std::cout << "Allo " << typeTransaction.size() << std::endl;

	for (auto i = 0; i < typeTransaction.size(); i++) {
		std::cout << "type: " << typeTransaction[i] << std::endl;
		if (typeTransaction[i] == "Crédit") {
// Imprimer chaque élément du troisième array (index 2)
		    std::cout << "Éléments du troisième array :" << std::endl;
			for (const char* element : compteTest[i]) {
				if (element != nullptr) {
					std::cout << element << std::endl;
				} else {
					break; // Sortir de la boucle dès qu'on rencontre un nullptr
				}
			}
		}
	}
}
