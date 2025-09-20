#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include "classLivreComptable.hpp"

bool LivreComptable::ajoutCompte() {
	std::string NOM = "";
	std::string NoREF = "";
	std::string compte = "";
	std::string TYPE = "";
	std::string DEPART = "";
	std::string CONCAT = "INSERT INTO Master(Nom, Ref, Type, Départ, Présent) VALUES(";

	line_read = readline("\tNom du compte     : ");
	NOM = line_read;
	free(line_read);
	if (NOM == "") return false;
	CONCAT += "\"" + NOM + "\", ";

	line_read = readline("\t# de référence    : ");
	NoREF = line_read;
	free(line_read);
	if (NoREF == "") return false;
	CONCAT += "\"" + NoREF + "\", ";

	do {
		std::cout << "\tType de compte    : 1- Compte courant, 2- Compte épargne, 3- Carte de crédit ? "
					<< std::flush;
		std::getline(std::cin, compte);
		if (compte == "1") TYPE = "Courant";
		else if (compte == "2") TYPE = "Épargne";
		else if (compte == "3") TYPE = "Crédit";
		else if (compte == "") return false;
		else TYPE = "";
	} while (TYPE == "");
	CONCAT += "\"" + TYPE + "\", ";

	do {
		std::cout << "\tMontant de départ : " << std::flush;
		std::cin >> DEPART;
		if (!testMontant(DEPART)) DEPART = "";
	} while (DEPART == "");
	std::cin.ignore(256, '\n');
	// Convertir DEPART 123,56 en cents -> 12356
	DEPART = dollars2cents(DEPART);

	CONCAT += DEPART + ", " + DEPART + ");";
	incantationSQL(CONCAT, "");
	COMPTES.push_back((struct compte) {NOM, NoREF, TYPE, DEPART, DEPART});

	// Création des catégories avec les comptes pour les virements et paiement.

	CONCAT = "INSERT INTO Catégories(Nom, Utilisé, Type) VALUES(\"" + NOM + "\", 1000, \"" + TYPE + "\");";
	incantationSQL(CONCAT, "");
	CATEGORIES.push_back((struct categorie) {NOM, 1000, TYPE});

	return true;
}

bool LivreComptable::ajoutCategorie() {
	std::string SQLcommande = "";
	std::string NOM = "";
	std::string TYPE = "";

	line_read = readline("Nouvelle catégorie : ");
	NOM = line_read;
	free(line_read);

	if (NOM == "") return false;

	do {
		std::cout << "\tType de catégorie    : 1- En entrée, 2- En sortie ? " << std::flush;
		std::getline(std::cin, TYPE);
		if (TYPE == "1") TYPE = "IN";
		else if (TYPE == "2") TYPE = "OUT";
		else if (TYPE == "") return false;
		else TYPE = "";
	} while (TYPE == "");

	SQLcommande = "INSERT INTO Catégories(Nom, Utilisé, Type) VALUES(\"" + NOM + "\", 0, \"" + TYPE + "\");";
	incantationSQL(SQLcommande,"");
	CATEGORIES.push_back((struct categorie) {NOM, 0, TYPE});

	return true;
}
