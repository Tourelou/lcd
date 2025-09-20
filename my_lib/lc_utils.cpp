#include <iostream>
#include <cstring>
#include "global_vars.h"
#include "classLivreComptable.hpp"

void LivreComptable::twistAccent(const char *chaine, int &width) {
	for (int i = 0; i < strlen(chaine); i++) {
		if ((unsigned int)chaine[i] >= '\x80') { width++; i++; }
		if ((unsigned int)chaine[i] >= '\xE0') { width++; i++; }
		if ((unsigned int)chaine[i] >= '\xF0') { width++; i++; }
	}
}

bool LivreComptable::testMontant(std::string &source) {
// Test la validité du montant en source: [-][::digit::][,|.::digit::]
	bool decimal = false;
	int compteur = 0;

	for (int i = 0; i < source.size(); i++) {
		if (source[i] >= '0' && source[i] <= '9') {
			if (decimal) compteur++;
			if (compteur > 2) return false;
			continue;
		}
		else if (source[i] == ',' || source[i] == '.') {
			if (!decimal) { decimal = true; continue; }
			else return false;
		}
		else if (source[i] == '-' && i == 0) continue;
		else return false;
	}
	return true;
}
std::string LivreComptable::cents2dollars(const std::string &cents) {
	// Converti la string xxxxx en xxx,xx pour l'affichage.
	std::string convString;
	int l = cents.size();

	if (l == 1) convString = "00" + cents;
	else if (l == 2) convString = "0" + cents;
	else convString = cents;
	convString.insert(convString.size() - 2, ",");

	int s = convString.size();
	if (s > 6) {	// Espace un peu si > 999,99
		if (convString[s - 7] == '-') return convString;	// Un -, on touche à rien
		else convString.insert(s - 6, " ");
	}
	return convString;
}
std::string LivreComptable::dollars2cents(const std::string &dollars) {
	//	Converti la string xxx,xx en cents pour la bd.
	std::string convString = dollars;
	int l = convString.size();
	int pos = -1;

	for (int i = 0; i < l; i++) {
		if (convString[i] == ',' || convString[i] == '.') {
			pos = i;
			break;
		}
	}
	if (pos == -1) convString += "00";	// Il n'y a pas de séparateur
	else {
		if (l - pos == 3) convString.erase(pos, 1);
		else if (l - pos == 2) { convString.erase(pos, 1); convString += "0"; }	
		else if (l - pos == 1) { convString.erase(pos, 1); convString += "00"; }	
	}
	return convString;
}

bool sortTransDateAsc(transaction t1, transaction t2) {
	return (t1.Date < t2.Date); 
}
bool sortTransDateDesc(transaction t1, transaction t2) {
	return (t1.Date > t2.Date); 
}
//	---------------------------------------------------------------------------
bool getReponse(const int nbEntree) {
	reponse = "";
	choix = 0;

	std::cout << "[0 ou enter pour annuler]: " << std::flush;
	std::getline(std::cin, reponse);
	if (reponse == "0" || reponse == "") return false;
	choix = atoi(reponse.c_str());
	if (choix == 0 || choix > nbEntree) return false;
	return true;
}
