#include <iostream>
#include <fstream>
#include <unistd.h>	// chdir, getpid
#include <sys/ioctl.h>
#include <libgen.h>	// dirname
#include <ctime>
#ifdef __APPLE__
	#include <libproc.h>	// proc_pidpath
#endif
#ifdef __linux__
	#include <linux/limits.h>
#endif
#include "my_lib/classLivreComptable.hpp"

LivreComptable livre;	// LE livre de comptes
bool cbackPassage = false;	// Sommes-nous déjà passé dans callback
std::string reponse = "";	// Pour recueillir l'input usager
int choix = 0;	// atoi() de reponse

std::string ver = "2025-02-18";

int chCurrDir(const char *path) {
	char resolved_path[PATH_MAX];
	
	if (realpath(path, resolved_path) == nullptr) {
		std::cerr << "Erreur lors de la résolution du chemin" << std::endl;
		return 10;
	}
	char* dir = dirname(resolved_path);

	if (chdir(dir) != 0) {
		std::cerr << "Erreur lors du changement de répertoire" << std::endl;
		return 10;
	}
	return 0;
}

int get_cmd_path(char *path) {
	ssize_t len = -1;

#ifdef __APPLE__
	// macOS specific code
	pid_t pid = getpid();
	len = proc_pidpath(pid, path, PATH_MAX);
	if (len > 0) path[len] = '\0';
#else
	// Linux specific code
	len = readlink("/proc/self/exe", path, PATH_MAX - 1);
	if (len != -1) path[len] = '\0';
#endif

	if (len > 0) {
		if (chCurrDir(path)) return 10;
		else return 0;
	}
	else return 10;
}

bool termSize() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	if (w.ws_col >= 112) return true;

	std::cout << "Largeur: " << w.ws_col << ", Hauteur: " << w.ws_row << std::endl;
	std::cout << "Un peu court pour afficher sur le terminal.\nUn minimum de 112 carctères est necessaire.\n";
	std::cout << "Appuyez sur «Enter» une fois fait. ";
	std::cin.get();

	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	std::cout << "Largeur: " << w.ws_col << ", Hauteur: " << w.ws_row << std::endl;
	if (w.ws_col >= 112) return true;
	else return false;
}

int main(int argc, char const *argv[]) {

	std::string lcdBookName = "";
	std::string reponse;
	bool nouveau = false;
	char current_dir[PATH_MAX];

	if(!termSize()) return 2;

	std::cout << "--------------------------------------------------------\n";
	std::cout << "--- lcd  par Daniel Vaillancourt, version " << ver  << " ---\n";
	std::cout << "--------------------------------------------------------\n";

	switch (argc) {
		case 1: {
			// Nous n'avons pas d'argument: set un defaut comme fileName
			// en rapport avec l'année courante
			std::string cmdPath = argv[0];
			std::time_t t = std::time(nullptr);
			std::tm *const pTInfo = std::localtime(&t);
			lcdBookName = "LivreComptable." + std::to_string(1900 + pTInfo->tm_year);
			// Pas d'argument, donc le currDir sera le répertoire de la commande
			bool dirPath = false;
			for (char c : cmdPath) if (c == '/') { dirPath = true; break; }
			// Si nous avons une commande avec son path
			if (dirPath) {
				if (chCurrDir(argv[0])) return 10;
			}
			// Sinon
			else {
				if (get_cmd_path(current_dir)) return 10;
			}
			break;
		}

		case 2: {
			// Le nom du livre se trouve en argument
			lcdBookName = argv[1];
			if (lcdBookName == "-h") {
				std::cout << "\nlcd [nom du fichier]\n"
				"\n3 façon d'utiliser lcd:\n"
				"\n1- lcd sans argument: Ouvre LivreComptable.xxxx situé dans le même dossier que l'application.\n"
				"2- lcd 'fichier': Sans path «/», Ouvre «fichier» situé dans le même dossier que l'application.\n"
				"3- lcd '~/Desktop/fichier': Avec path, Ouvre «fichier» du dossier demandé [absolu ou relatif].\n"
																										<< std::endl;
				return 0;
			}
			bool dirPath = false;
			for (char c : lcdBookName) if (c == '/') { dirPath = true; break; }
			if (dirPath) {
				if (lcdBookName[lcdBookName.size() - 1] == '/') {	// En dernier ... c'est un répertoire
					std::cout << "Il faut un fichier en argument" << std::endl;
					return 10;
				}
				int found = lcdBookName.find_last_of("/\\");
				std::string dirName = lcdBookName.substr(0,found);
				if (!(lcdBookName[0] == '/')) {	// Path relatif
					if (getcwd(current_dir, sizeof(current_dir)) != nullptr) {
						dirName = std::string(current_dir) + "/" + dirName;
					}
				}
				if (chdir(dirName.c_str()) != 0) {
					std::cerr << "Erreur lors du changement de répertoire" << std::endl;
					return 10;
				}
				lcdBookName = lcdBookName.substr(found + 1);
			}
			else {	// Cas de figure ou argv[1] est juste un nom sans «/»
				if (get_cmd_path(current_dir)) return 10;
			}
			break;
		}

		default:
			std::cout << "Trop d'arguments" << std::endl;
			return 10;
			break;
	}

	std::ifstream file(lcdBookName, std::ios::binary);
	if (file.good()) {
		char buffer[16];
		file.read(buffer, sizeof(buffer));
		if (std::string(buffer) != "SQLite format 3\000") {
			std::cout << "«" << lcdBookName << "» n'est pas une base de données SQLite 3" << std::endl;
			return 10;
		}
	}
	else {
		std::cout << "«" << lcdBookName << "» n'existe pas, Création d'un nouveau livre ? " << std::flush;
		std::getline(std::cin, reponse);
		if (reponse[0] == 'n' || reponse[0] == 'N') return 0;
		nouveau = true;
	}
	if (!livre.ouvreLivre(lcdBookName, nouveau)) {
		std::cout << "Gros problème: Ne peut ouvrir ou initialiser la base de données" << std::endl;
		return 10;
	}

/*	C'est ici que tout commence	*/

	std::vector<std::string> options = {" 1- Entrer une nouvelle transaction",
										" 2- Afficher tous les comptes",
										" 3- Ajouter un compte",
										" 4- Supprimer un compte",
										" 5- Afficher toutes les catégories",
										" 6- Ajouter une catégorie",
										" 7- Supprimer une catégorie",
										" 8- Afficher toutes les favorites",
										" 9- Ajouter une transaction favorite",
										"10- Modifier une transaction favorite",
										"11- Supprimer une transaction favorite",
										"12- Interroger les transactions",
										"13- Accès complet à la bd [PRO ONLY]",
										"14- Sommaire de comptes"
										};

	std::cout << "--------------------------------------------------------" << std::endl;
	livre.printAllComptes();

	while (true) {
		std::cout << "--------------------------------------------------------" << std::endl;
		for (auto o : options) std::cout << o << std::endl;

	std::cout << "--------------------------------------------------------" << std::endl;
		std::cout << "Choisir un item du menu par son numéro," << std::endl;
		if (!getReponse(options.size())) break;

//		std::cout << "--------------------------------------------------------" << std::endl;

		if (choix == 1) { while (livre.nouvelleTransaction()) livre.printAllComptes(); }
		else if (choix == 2) livre.printAllComptes();
		else if (choix == 3) livre.ajoutCompte();
		else if (choix == 4) livre.suppCompte();
		else if (choix == 5) livre.printAllCategories();
		else if (choix == 6) livre.ajoutCategorie();
		else if (choix == 7) livre.suppCategorie();
		else if (choix == 8) livre.printAllFavorites();
		else if (choix == 9) livre.nouvelleTransaction(true);
		else if (choix == 10) livre.modifFavorite();
		else if (choix == 11) livre.suppFavorite();
		else if (choix == 12) livre.questionBD();
		else if (choix == 13) livre.fullQuestionBD();
		else if (choix == 14) livre.sommaireMois();
		else break;
	}
	livre.fermeLivre();

	return 0;
}
