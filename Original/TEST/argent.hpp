#ifndef __ARGENT__
	#define __ARGENT__

	#include <iostream>
	#include <sstream>
	#include <iomanip>
	#include <string>
	#include <cstring>

/*
	Deux façons d'inialiser une instance:

		1- Argent a(int &a)				Avec un integer

		2- Argent a(std::string &a)		Avec une string xxx,xx

	Bonnes pratiques de la classe pour initialisation ace une string :

		1- Tester d'abord la validité de la string avec testMontant()
			avant d'instancier un objet.
		2- Instancier avec la valeur testé un objet Argent a("montant");

		Sinon, si la string est invalide, l'objet sera initialisé à 0.
*/
	class Argent {
	private:
		int cents;	// Variable pour le calcul
		std::string dollars;	// Variable pour impression

		void input2cents(const std::string &);
		void input2dollars(const int &);
	public:
		std::string strCents = "0";	// Variable pour la bd
		Argent();
		Argent(const int &);
		Argent(const char *);
		Argent(const std::string &);
		~Argent();

		int getInt();
		bool setFromStrCents(std::string &);

		static bool testMontant(const std::string &source) {
		// Test la validité du montant en source: [-][::digit::][,|.::digit::]
			bool decimal = false;
			int compteur = 0;
			bool num = false;

			for (int i = 0; i < source.size(); i++) {
			// Faire sûr d'avoir des chiffres > 0
				if (source[i] > '0' && source[i] <= '9') num = true;
			}
			if (!num) return false;

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

		friend std::ostream& operator<<(std::ostream&, const Argent&);

		Argent& operator+=(const Argent &);
		Argent& operator-=(const Argent &);
		friend Argent operator+(Argent, const Argent&);
		friend Argent operator-(Argent, const Argent&);
	};

	// Private function -----------------------------------
	void Argent::input2cents(const std::string &input) {
	// #include <algorithm>
		bool neg, num;
		neg = num = false;
		int compteur, sep;
		compteur = sep = 0;
		dollars = strCents = "";

		for (int i = 0; i < input.size(); i++) {
			// Faire sûr d'avoir des chiffres > 0
			if (input[i] > '0' && input[i] <= '9') num = true;
		}
		if (num == false)
			{ cents = 0; dollars = "0,00"; strCents = "0"; return; }
		num = false;

		for (int i = 0; i < input.size(); i++) {
			if (input[i] == '-' && i == 0 && neg == false)
				{ dollars += "-"; neg = true; }

			else if ((input[i] == '.' || input[i] == ',') && sep == 0) {
				if (num) { dollars += ','; sep = i; }
				else { dollars += "0,"; num = sep = true; }
			}

			else if (input[i] >= '0' && input[i] <= '9' && compteur < 3)
			{ dollars += input[i]; num = true; if (sep) compteur++; }

			else { cents = 0; dollars = "0,00"; strCents = "0"; return; }
		}
		if (num) {
			if (sep == 0) dollars += ",00";
			else {
				int dl = dollars.size();
				if (dl - sep == 2) dollars += "0";	// Seulement 1 digit après le séparateur
				else if (dl -sep == 1) dollars += "00";	// separateur à la fin
			}
		}
		else { cents = 0; dollars = "0,00"; strCents = "0"; return; }
		// Si on se rend ici, on devrait avoir une belle string [-]xxx,xx

		num = false;	// on se resert de num pour illiminer les zéros de tête
		for (int i = 0; i < dollars.size(); i++) {
			if (dollars[i] == '0' && num == true) strCents += dollars[i];
			else if (dollars[i] == '-') strCents += dollars[i];
			else if (dollars[i] > '0' && dollars[i] <= '9')
				{ strCents += dollars[i]; num = true; }
		}
		cents = std::stoi(strCents);
	}

	void Argent::input2dollars(const int &input) {
		// Ajuste les valeur strCents et dollars pour matcher cents.
		cents = input;	// Un integer, pas trop de test à faire.
		strCents = std::to_string(cents);	// Une évidence.
		std::stringstream ss;
		int c = std::abs(cents);	// valeur absolue sinon ça fait -5,-05

		if(cents < 0) ss << "-";
		ss << c / 100 << "," << std::setfill('0') << std::setw(2)
										<< c % 100 << std::flush;
		dollars = ss.str();
	}

	// Public function ------------------------------------
	Argent::Argent() {
	// Constructeur par défaut
		cents = 0;
		dollars = "0,00";
		strCents = "0";
	}

	Argent::Argent(const int &init) {
	// Constructeur avec paramètres
		input2dollars(init);
	}

	Argent::Argent(const char *c) {
	// Constructeur avec paramètres
		input2cents(c);
	}

	Argent::Argent(const std::string &init) {
	// Constructeur avec paramètres
		input2cents(init);
	}

	Argent::~Argent() {
	}

	inline int Argent::getInt() {
		return cents;
	}

	bool Argent::setFromStrCents(std::string &input) {
		bool num = false;
		for (int i = 0; i < input.size(); i++) {
			if (input[i] == '-' && i == 0) continue;
			if (input[i] >= '0' && input[i] <= '9') { num = true; continue; }
			else return false;
		}
		if (!num) return false;

		cents = std::stoi(input);
		input2dollars(cents);
		return true;
	}

	std::ostream& operator<<(std::ostream& flux, const Argent &a) {
		int s = a.dollars.size();
		if (s > 6) {
			if (a.dollars[s - 7] == '-') flux << a.dollars;
			else {
				std::string t = a.dollars;
				t.insert(s - 6, " ");
				flux << t;
			}
		}
		else flux << a.dollars;
		return flux;
	}

	Argent& Argent::operator+=(const Argent &autre) {
		this->cents += autre.cents;
		input2dollars(cents);
		return *this;
	}

	Argent operator+(Argent lhs, const Argent& rhs) {
	lhs += rhs;  // Utilise operator+=
	return lhs;
	}

	Argent& Argent::operator-=(const Argent &autre) {
		this->cents -= autre.cents;
		input2dollars(cents);
		return *this;
	}

	Argent operator-(Argent lhs, const Argent& rhs) {
	lhs -= rhs;  // Utilise operator+=
	return lhs;
	}

#endif
