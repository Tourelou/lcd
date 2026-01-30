#ifndef __CLASS_LIVRE_COMPTABLE__
	#define __CLASS_LIVRE_COMPTABLE__

	#include <vector>
	#include <sqlite3.h>
	#include "global.h"
	#include "class_AMJ_Date.hpp"

	class LivreComptable
	{
	private:
		sqlite3* bd;
		AMJ_Date d;
		char *line_read = nullptr;
		std::vector<struct compte> COMPTES;
		std::vector<struct categorie> CATEGORIES;
		std::vector<struct transaction> TRANSACTIONS;
		std::vector<struct transaction> FAV_TRANSACTIONS;
	/*
		Des vecteurs temporaires dépendant du type demandé
		Important de faire clear avant usage
	*/
		std::vector<struct compte> TMP_COMPTES;
		std::vector<struct categorie> TMP_CATEGORIES;
		std::vector<struct transaction> TMP_TRANSACTIONS;
	/*
		Type de transaction: Dépôt, Débit, Crédit, Achat, Virement, Paiement
		Les deux derniers impliquent une double transaction en une seule ligne.

		Opération :                                compte courant | Épargne | carte crédit
		----------------------------------------------------------------------------------
		Dépôt : Paye, remboursement d'impôt ...           +           N/A         N/A
		Débit : Hydro-Québec, Videotron ...               -           N/A          +
		Crédit : Remise dollar carte de crédit            +            +           -
		Achat : Achat avec une carte en magasin           -           N/A          +
		Virement : Transfert d'argent entre comptes     - / +        - / +        N/A
		Paiement : Remboursement de solde (carte)         -           N/A          -
		----------------------------------------------------------------------------------

		Les catégories sont déclarées dynamiquement + l'ajout des comptes
		pour permettre de faire des paiements et des virements.

		Dans le cas où le TYPE est Paiement ou Virement le logiciel présentera
		seulement les catégories appropriées pour ce type de transaction.
	*/
		std::vector<std::string> typeTransaction = {"Dépôt", "Débit", "Crédit", "Achat", "Virement", "Paiement"};

	// lc_initLivre.cpp
		bool initLivre();
		void incantationSQL(std::string, std::string);
	// lc_utils.cpp
		void twistAccent(const char *, int &);
		bool testMontant(std::string &);
		std::string dollars2cents(const std::string &);	// Convert dollars to cents
		std::string cents2dollars(const std::string &);	// Convert cents to dollars
	// lc_publish.cpp
		void publishTransaction(struct transaction&);
	public:
	// lc_const_dest.cpp
		LivreComptable();
		~LivreComptable();
	// lc_ouvre_ferme.cpp
		bool ouvreLivre(std::string, bool);
		void valideFavoris();
		void fermeLivre();
	// lc_ajoute.cpp
		bool ajoutCompte();
		bool ajoutCategorie();
	// lc_supprime.cpp
		void suppCompte();
		void suppCategorie();
		void suppFavorite();
		void modifFavorite();
	// lc_sommaire_mois.cpp
		void sommaireMois();

	// lc_set2mem.cpp
		int setMaster2mem(char **);
		int setCategories2mem(char **);
		int setFavorites2mem(char **);
		int setTransactions2mem(char **);
		int setQuestion2mem(char **);
	// lc_questions.cpp
		void questionBD();
		void fullQuestionBD();

	// lc_print_comptes.cpp
		void printComptes(std::vector<struct compte>&);
		void printAllComptes();
	// lc_print_categories.cpp
		void printType();
		void printCategories(std::vector<struct categorie>&);
		void printAllCategories();
	// lc_print_transactions.cpp
		void print1Transaction(struct transaction&);
		void printTransactions(std::vector<struct transaction>&, bool = false);
		void printAllFavorites();
	// lc_nouvelle_trans.cpp
		bool nouvelleTransaction(bool = false);
	};
	// lc_utils.cpp
	bool sortTransDateAsc(transaction, transaction);
	bool sortTransDateDesc(transaction, transaction); 

	bool getReponse(const int);

#endif // __CLASS_LIVRE_COMPTABLE__
