#ifndef __VARS__
	#define __VARS__

	#include <string>
	#include "classLivreComptable.hpp"

	extern LivreComptable livre;	// LE livre de comptes
	extern bool cbackPassage;	// Sommes-nous déjà passé dans callback
	extern std::string reponse;	// Pour recueillir l'input usager
	extern int choix;	// atoi() de reponse

#endif // __VARS__
