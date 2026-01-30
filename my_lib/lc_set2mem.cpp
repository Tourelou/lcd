#include "classLivreComptable.hpp"

int LivreComptable::setMaster2mem(char **argv) {
	struct compte c;

	c.Nom = argv[0];
	c.Ref = argv[1];
	c.Type = argv[2];
	c.Départ = argv[3];
	c.Présent = argv[4];
	COMPTES.push_back(c);

	return 0;
}
int LivreComptable::setCategories2mem(char **argv) {
	struct categorie c;

	c.Nom = argv[0];
	c.Utilisé = atoi(argv[1]);
	c.Type = argv[2];
	CATEGORIES.push_back(c);

	return 0;
}
int LivreComptable::setTransactions2mem(char ** argv) {
	struct transaction t;

	t.Date = argv[0];
	t.Description = argv[1];
	t.Type = argv[2];
	t.Compte = argv[3];
	t.Catégorie = argv[4];
	t.Montant = argv[5];

	TRANSACTIONS.push_back(t);

	return 0;
}
int LivreComptable::setFavorites2mem(char ** argv) {
	struct transaction t;

	t.Date = argv[0];
	t.Description = argv[1];
	t.Type = argv[2];
	t.Compte = argv[3];
	t.Catégorie = argv[4];
	t.Montant = argv[5];

	FAV_TRANSACTIONS.push_back(t);

	return 0;
}
int LivreComptable::setQuestion2mem(char ** argv) {
	struct transaction t;

	t.Date = argv[0];
	t.Description = argv[1];
	t.Type = argv[2];
	t.Compte = argv[3];
	t.Catégorie = argv[4];
	t.Montant = argv[5];

	TMP_TRANSACTIONS.push_back(t);

	return 0;
}
