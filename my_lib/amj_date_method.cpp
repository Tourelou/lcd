	#include <ctime>
	#include <iostream>
	#include <sstream>
	#include <iomanip>
	#include "class_AMJ_Date.hpp"

	/*	------- */
	/*	constructor */
	AMJ_Date::AMJ_Date() {
		setAujourdhui();
	}

	AMJ_Date::~AMJ_Date() {
	}

	/*	private */
	inline void AMJ_Date::setAujourdhui() {
		std::time_t t = std::time(0);   // get time maintenant
		std::tm* now = std::localtime(&t);
		std::stringstream ss;

		annee = now->tm_year + 1900;
		mois = now->tm_mon + 1;
		jour = now->tm_mday;

		ss << std::setw(4) << std::setfill('0') << annee << "-"
			<< std::setw(2) << std::setfill('0') << mois << "-"
			<< std::setw(2) << std::setfill('0') << jour << std::flush;
		aujourdhui = derniereEntree = ss.str();
	}

//	Teste une date sans avoir à créer une instance grâce à static
		bool AMJ_Date::testDate(std::string_view date2check_sv) {
			int ndjs[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
			int a, m, j = 0;
			std::string date2check{date2check_sv};

			// Il nous faut une string du genre: yyyy-mm-jj ou yyyy/mm/jj
			if ((date2check.length() == 10) && ((date2check[4] == '-' && date2check[7] == '-')\
											|| (date2check[4] == '/' && date2check[7] == '/'))) {
				for (auto i = 0; i < date2check.size(); i++) {
					if (i == 7 || i == 4) continue;		// Déjà testé pour «-» ou «/»
					// Autrement doit être absolument un chiffre
					else if (date2check[i] >= '0' && date2check[i] <= '9') continue;
					else return false;
				}
			
				a = atoi(date2check.substr(0, 4).c_str());
				m = atoi(date2check.substr(5, 2).c_str());
				j = atoi(date2check.substr(8, 2).c_str());

				ndjs[2] = (a % 4 == 0) ? (a % 100 == 0) ? (a % 400 == 0) ? 29 : 28 : 29 : 28;

				if (a > 9999 || a < 1970) return false;
				if (m > 12 || m < 1) return false;
				if (j > ndjs[m] || j < 1) return false;
				return true;
			}
			else return false;
		}

	/*	----------- */

	/* public functions */
	bool AMJ_Date::setCheckDate(std::string_view date2check) {
	/*	analyse une string genre YYYY-MM-DD pour sa validité	*/
	/*	Possibilités: 	1- YYYY-MM-DD
						2- MM-DD
						3- DD
	*/
	std::string s = "";
		if (date2check.length() == 10) {	// AAAA-MM-JJ
			s = date2check;
		}
		else if (date2check.length() == 5) {	// MM-JJ
			s = derniereEntree.substr(0, 4) + "-" + (std::string)date2check;
		}
		else if (date2check.length() == 2) {	// JJ
			s = derniereEntree.substr(0, 4) + "-" +\
				derniereEntree.substr(5, 2)\
				+ "-" + (std::string)date2check;
		}
		else return false;	// La string ne ressemble pas à aucune possibilité

		if (!testDate(s)) return false;
		// Si tout est OK on peut setter l'instance
		annee = atoi(s.substr(0, 4).c_str());
		mois = atoi(s.substr(5, 2).c_str());
		jour = atoi(s.substr(8, 2).c_str());
		derniereEntree = s;
		return true;
	}
