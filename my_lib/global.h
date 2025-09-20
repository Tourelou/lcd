#ifndef __GLOBAL__
	#define __GLOBAL__

	#include <string>
	#include <vector>
	#include <sqlite3.h>

	#include "classLivreComptable.hpp"

	struct compte {
		std::string Nom;
		std::string Ref;
		std::string Type;	// Courant, Épargne, Crédit
		std::string Départ;
		std::string Présent;
	};
	/*
	┌────┬──────────────────────────────────┬──────────────┐┌────┬──────────────────────────────────┬──────────────┐
	│  1 │ Compte #1 | xxxxx-xxxxxx-x       │ $    2240,98 ││  2 │ Compte #2 | xxxxx-xxxxxx-x       │ $    3490,88 │
	└────┤ -                                │ $    2240,98 │└────┤ -                                │ $    3490,88 │
		 └──────────────────────────────────┴──────────────┘     └──────────────────────────────────┴──────────────┘
	*/

	struct categorie {
		std::string Nom;
		int Utilisé;
		std::string Type;	// IN, OUT, Courant, Épargne, Crédit
	};
	/*
	┌────┬───────────────────────────┐┌────┬───────────────────────────┐┌────┬───────────────────────────┐
	│  1 │ Compte #1 | xxxxx-xxxxxx- ││  2 │ Compte #2 | xxxxx-xxxxxx- ││  3 │ Compte #3 | xxxxx-xxxxxx- │
	└────┴───────────────────────────┘└────┴───────────────────────────┘└────┴───────────────────────────┘
	*/
	struct transaction {
		std::string Date;	// Sous forme: 2024-03-24
		std::string Description;
		std::string Type;	// Dépôt, Débit, Crédit, Achat, Virement, Paiement
		std::string Compte;
		std::string Catégorie;	// Nom de la catégorie
		std::string Montant;
	};
	/*
	┌────────────┬────────────────────────────────┬─────────────┬─────────────────────────────────────────┐
	│ 2024-05-31 │ Hypothèque                     │ Débit       │   De: Compte #2 | xxxxx-xxxxxx-x        │
	└────────────┤                                │ $    234,56 │ Vers: Hypothèque                        │
				 └────────────────────────────────┴─────────────┴─────────────────────────────────────────┘
	*/

#endif // __GLOBAL__
