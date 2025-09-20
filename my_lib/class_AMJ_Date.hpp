#ifndef __AMJ_DATE__
	#define __AMJ_DATE__

	#include <string>
	#include <string_view>

	class AMJ_Date {
	private:
		int annee, mois, jour;
		void setAujourdhui();

	public:
		std::string aujourdhui;
		std::string derniereEntree;

		AMJ_Date();
		~AMJ_Date();

		//	Teste une date sans avoir à créer une instance grâce à static
		static bool testDate(std::string_view);

		// Un string_view permet de passer un const char * comme une std::string
		// en autant qu'on essaie pas de le modifier.
		bool setCheckDate(std::string_view);
	};

#endif
