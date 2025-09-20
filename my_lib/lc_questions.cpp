#include <iostream>
#include <iomanip>
#include <readline/readline.h>
#include <readline/history.h>
#include "global_vars.h"
#include "classLivreComptable.hpp"

void LivreComptable::questionBD() {
	int total_credit = 0;
	int total_debit = 0;
	int total_virement = 0;

	std::string requete = "SELECT * FROM 'Transactions' WHERE ";
	
	std::cout << "Formulez votre question à la base de données:" << std::endl;
	line_read = readline("Exemple: «Catégorie LIKE \"\%Épicerie\%\"» ? ");
	add_history(line_read);

	reponse = line_read;
	free(line_read);

	requete = requete + reponse + " ORDER BY Date;";
	std::cout << "--------------------------------------------------------" << std::endl;
	TMP_TRANSACTIONS.clear();
	incantationSQL(requete, "setQuestion2mem");
	printTransactions(TMP_TRANSACTIONS);

	for (auto t : TMP_TRANSACTIONS) {
		if (t.Type == "Dépôt" || t.Type == "Crédit") total_credit += atoi(t.Montant.c_str());
		if (t.Type == "Achat" || t.Type == "Débit") total_debit += atoi(t.Montant.c_str());
		if (t.Type == "Paiement" || t.Type == "Virement") total_virement += atoi(t.Montant.c_str());
	}
	if (total_credit != 0 || total_debit != 0 || total_virement != 0) {
		std::cout << "                                                   Crédit         Débit      Virement\n"
				<< "┌─────────────────────────────────────────────┬─────────────┬─────────────┬─────────────┐\n"
				<< "│ Total des éléments de la liste en requête   │ $ "
				<< std::right << std::setw(9) << cents2dollars(std::to_string(total_credit)) << " │ $ "
				<< std::right << std::setw(9) << cents2dollars(std::to_string(total_debit)) << " │ $ "
				<< std::right << std::setw(9) << cents2dollars(std::to_string(total_virement)) << " │\n"
				<< "└─────────────────────────────────────────────┴─────────────┴─────────────┴─────────────┘" << std::endl;
	}
}
void LivreComptable::fullQuestionBD() {

	std::cout << "Formulez votre requête complexe à la base de données:" << std::endl;
	std::cout << "Exemple: «UPDATE Transactions SET Date = \"2024-03-27\" WHERE Type = \"Crédit\";»" << std::endl;
	std::cout << "ATTENTION, IL N'Y A PAS DE DEUXIÈME CHANCE ? \n" << std::endl;
	line_read = readline(" ? : > ");
	add_history(line_read);

	reponse = line_read;
	free(line_read);

	std::cout << "--------------------------------------------------------" << std::endl;
	std::cout << "Lancement de la commande «" << reponse << "»" << std::endl;
	cbackPassage = false;
	incantationSQL(reponse, reponse);
}
