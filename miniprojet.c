// projet gestion d'etudiants
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_ETUDIANTS     50
#define MAX_MODULES       50
#define MAX_NAME_LEN      30
#define MAX_ADDR_LEN      30
#define MAX_MODNAME_LEN   50
#define MAX_BUFFER_FENETRE 8192

//======================================================================
// Structures
//======================================================================

typedef struct {
    int matricule;
    int jour, mois, annee;
    char nom[MAX_NAME_LEN];
    char prenom[MAX_NAME_LEN];
    char lieu_de_naissance[MAX_NAME_LEN];
    char adresse[MAX_ADDR_LEN];
} Etudiant;

typedef struct {
    int matricule;
    int jour, mois, annee;
    char nom[MAX_NAME_LEN];
    char prenom[MAX_NAME_LEN];
    char lieu_de_naissance[MAX_NAME_LEN];
    char adresse[MAX_ADDR_LEN];
    float moy; // moyenne générale
} EtudiantView;

typedef struct {
    int coef;
    char nom[MAX_MODNAME_LEN];
    int code;
    float moy; // moyenne du module
} Module;

typedef struct {
    int coef;
    char nom[MAX_MODNAME_LEN];
    int code;
    float moy;
} ModuleView;

// Structure temporaire pour échanges (tri)
typedef struct {
    int matricule;
    int jour, mois, annee;
    char nom[MAX_NAME_LEN];
    char prenom[MAX_NAME_LEN];
    char lieu[MAX_NAME_LEN];
    char adresse[MAX_ADDR_LEN];
    int coef;
    int code;
    char nomModule[MAX_MODNAME_LEN];
    float moy;
} Tmp;

//======================================================================
// Données globales
//======================================================================

Etudiant     etudiants[MAX_ETUDIANTS + 1];      // indexés à partir de 1
EtudiantView etudiants2[MAX_ETUDIANTS + 1];     // vue pour tris/affichage

Module       modules[MAX_MODULES + 1];          // indexés à partir de 1
ModuleView   modules2[MAX_MODULES + 1];         // vue pour tris/affichage

float        notes[MAX_ETUDIANTS + 1][MAX_MODULES + 1]; // notes[i][j]

int n  = 0;  // nombre d'étudiants
int nm = 0;  // nombre de modules

int choix = 0, choixx = 0, choixxx = 0, choixxxx = 0;

//======================================================================
// Prototypes
//======================================================================

void reglage(void);

// gestion étudiants
void saisir_des_etudiant(void);
void afficher_les_etudiants(void);
void modifier(void);
void supprimer(void);
void ajouter(void);

// tri et moyennes étudiants
void trier_ordre_alphabetique_ascendent(void);
void trier_ordre_alphabetique_descendant(void);
void afficher_moy_decroi(void);
void afficher_moy_CROIS(void);

// gestion modules
void saisir_les_modules(void);
void ajouter_modules(void);
void modifier_module(void);
void supprimer_module(void);
void afficher_les_modules(void);
void trier_module_par_nom_ascendent(void);
void trier_module_par_nom_descendant(void);
void moyenne_module_crois(void);
void moyenne_module_decroi(void);

// gestion notes
void saisir_les_notes(void);
void afficher_les_notes(void);

// divers
void quitter(void);

// helpers internes
static void sync_etudiants_view(void);
static void sync_modules_view(void);
static int  lire_int_sure(const char *msg);
static void lire_chaine(const char *msg, char *dest, int taille);
static void show_text_window(const char *title, const char *text);
static void append_line(char *buffer, size_t size, const char *fmt, ...);

//======================================================================
// Helpers
//======================================================================

static void sync_etudiants_view(void) {
    for (int i = 1; i <= n; ++i) {
        etudiants2[i].matricule          = etudiants[i].matricule;
        etudiants2[i].jour               = etudiants[i].jour;
        etudiants2[i].mois               = etudiants[i].mois;
        etudiants2[i].annee              = etudiants[i].annee;
        strcpy(etudiants2[i].nom,        etudiants[i].nom);
        strcpy(etudiants2[i].prenom,     etudiants[i].prenom);
        strcpy(etudiants2[i].lieu_de_naissance, etudiants[i].lieu_de_naissance);
        strcpy(etudiants2[i].adresse,    etudiants[i].adresse);
        etudiants2[i].moy                = 0.0f;
    }
}

static void sync_modules_view(void) {
    for (int i = 1; i <= nm; ++i) {
        modules2[i].code = modules[i].code;
        modules2[i].coef = modules[i].coef;
        strcpy(modules2[i].nom, modules[i].nom);
        modules2[i].moy  = modules[i].moy;
    }
}

static int lire_int_sure(const char *msg) {
    int x;
    int ret;
    printf("%s", msg);
    while ((ret = scanf("%d", &x)) != 1) {
        int c;
        if (ret == EOF) {
            exit(EXIT_FAILURE);
        }
        while ((c = getchar()) != '\n' && c != EOF) {}
        printf("Entrée invalide, réessayez : ");
    }
    return x;
}

static void lire_chaine(const char *msg, char *dest, int taille) {
    int c;
    printf("%s", msg);
    // vider le buffer précédent
    while ((c = getchar()) != '\n' && c != EOF) {}
    if (fgets(dest, taille, stdin) != NULL) {
        char *p = strchr(dest, '\n');
        if (p) *p = '\0';
    } else {
        dest[0] = '\0';
    }
}

static void show_text_window(const char *title, const char *text) {
#ifdef _WIN32
    MessageBoxA(NULL, text, title, MB_OK | MB_ICONINFORMATION);
#else
    // Sur autres systèmes : on ne fait rien, pour éviter les warnings
    (void)title;
    (void)text;
#endif
}

static void append_line(char *buffer, size_t size, const char *fmt, ...) {
    size_t len = strlen(buffer);
    if (len >= size - 1) return;

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer + len, size - len, fmt, ap);
    va_end(ap);
}

//======================================================================
// Ecriture du fichier des notes
//======================================================================

void reglage(void) {
    FILE *nt = fopen("notes.txt", "w");
    if (!nt) {
        printf("Erreur lors de l'ouverture de notes.txt\n");
        return;
    }

    fprintf(nt, "les notes des etudiants : \n\n");
    fprintf(nt, "matricule | nom prenom | notes \n\n");

    for (int i = 1; i <= n; ++i) {
        fprintf(nt, "%d | %s %s | ",
                etudiants[i].matricule,
                etudiants[i].nom,
                etudiants[i].prenom);

        for (int j = 1; j <= nm; ++j) {
            fprintf(nt, "%s : %0.2f  ", modules[j].nom, notes[i][j]);
        }
        fprintf(nt, "\n");
    }

    fclose(nt);
}

//======================================================================
// Etudiants
//======================================================================

void saisir_des_etudiant(void) {
    FILE *f = fopen("etudiants.txt", "a");
    if (!f) {
        printf("Erreur d'ouverture du fichier etudiants.txt.\n");
        return;
    }

    int nb = lire_int_sure("Entrer le nombre des etudiants que vous voulez saisir : ");
    printf("\n");

    fprintf(f, "la liste des etudiants : \n\n");
    fprintf(f, "matricule | nom | prenom | date de naissance | lieu de naissance | adresse \n\n");

    for (int k = 0; k < nb && n < MAX_ETUDIANTS; ++k) {
        int idx = n + 1;
        int matricule_ok = 0;

        printf("Saisir les informations de l'etudiant numero %d :\n\n", idx);

        // Matricule unique
        while (!matricule_ok) {
            etudiants[idx].matricule = lire_int_sure("matricule : ");
            matricule_ok = 1;
            for (int j = 1; j <= n; ++j) {
                if (etudiants[idx].matricule == etudiants[j].matricule) {
                    printf("Ce matricule est déjà utilisé, réessayez.\n");
                    matricule_ok = 0;
                    break;
                }
            }
        }

        // Nom / Prenom
        printf("Nom : ");
        scanf("%29s", etudiants[idx].nom);

        printf("Prenom : ");
        scanf("%29s", etudiants[idx].prenom);

        // Date de naissance
        do {
            etudiants[idx].jour = lire_int_sure("Jour de naissance [1-31] : ");
        } while (etudiants[idx].jour < 1 || etudiants[idx].jour > 31);

        do {
            etudiants[idx].mois = lire_int_sure("Mois de naissance [1-12] : ");
        } while (etudiants[idx].mois < 1 || etudiants[idx].mois > 12);

        do {
            etudiants[idx].annee = lire_int_sure("Annee de naissance (< 2024) : ");
        } while (etudiants[idx].annee <= 0 || etudiants[idx].annee >= 2024);

        // Lieu / Adresse
        printf("Lieu de naissance (un seul mot) : ");
        scanf("%29s", etudiants[idx].lieu_de_naissance);

        lire_chaine("Adresse : ", etudiants[idx].adresse, MAX_ADDR_LEN);

        fprintf(f, "%d | %s | %s | %d/%d/%d | %s | %s\n",
                etudiants[idx].matricule,
                etudiants[idx].nom,
                etudiants[idx].prenom,
                etudiants[idx].jour,
                etudiants[idx].mois,
                etudiants[idx].annee,
                etudiants[idx].lieu_de_naissance,
                etudiants[idx].adresse);

        ++n;
    }

    fclose(f);
    sync_etudiants_view();
}

void afficher_les_etudiants(void) {
    system("cls");
    if (n == 0) {
        printf("La liste est vide.\n");
        return;
    }

    char buffer[MAX_BUFFER_FENETRE];
    buffer[0] = '\0';
    append_line(buffer, sizeof(buffer), "Liste des etudiants :\n");

    for (int i = 1; i <= n; ++i) {
        printf("\n");
        printf("Etudiant :\n");
        printf("Matricule : %d\n", etudiants[i].matricule);
        printf("Nom       : %s\n", etudiants[i].nom);
        printf("Prenom    : %s\n", etudiants[i].prenom);
        printf("Naissance : %d/%d/%d\n",
               etudiants[i].jour, etudiants[i].mois, etudiants[i].annee);
        printf("Lieu      : %s\n", etudiants[i].lieu_de_naissance);
        printf("Adresse   : %s\n", etudiants[i].adresse);

        append_line(buffer, sizeof(buffer),
                    "\nMatricule: %d\nNom: %s\nPrenom: %s\nNaissance: %d/%d/%d\nLieu: %s\nAdresse: %s\n",
                    etudiants[i].matricule,
                    etudiants[i].nom,
                    etudiants[i].prenom,
                    etudiants[i].jour,
                    etudiants[i].mois,
                    etudiants[i].annee,
                    etudiants[i].lieu_de_naissance,
                    etudiants[i].adresse);
    }

    // Fenêtre avec la liste
    show_text_window("Liste des etudiants", buffer);
}

void modifier(void) {
    if (n == 0) {
        printf("La liste est vide.\n");
        return;
    }

    FILE *f;
    int mat, k = 0;

    printf("Liste des etudiants enregistres :\n");
    for (int i = 1; i <= n; ++i) {
        printf("Matricule: %d  Etudiant: %s %s\n",
               etudiants[i].matricule, etudiants[i].nom, etudiants[i].prenom);
    }

    while (k == 0) {
        mat = lire_int_sure("\nSaisir le matricule de l'etudiant a modifier : ");
        for (int i = 1; i <= n; ++i) {
            if (etudiants[i].matricule == mat) {
                k = i;
                break;
            }
        }
        if (k == 0) {
            printf("Aucun etudiant avec ce matricule. Reessayez.\n");
        }
    }

    // Nouveau matricule unique
    int ok = 0;
    while (!ok) {
        etudiants[k].matricule = lire_int_sure("Nouveau matricule : ");
        ok = 1;
        for (int i = 1; i <= n; ++i) {
            if (i != k && etudiants[i].matricule == etudiants[k].matricule) {
                printf("Matricule deja utilise, reessayez.\n");
                ok = 0;
                break;
            }
        }
    }

    printf("Nouveau Nom : ");
    scanf("%29s", etudiants[k].nom);

    printf("Nouveau Prenom : ");
    scanf("%29s", etudiants[k].prenom);

    do {
        etudiants[k].jour = lire_int_sure("Jour de naissance [1-31] : ");
    } while (etudiants[k].jour < 1 || etudiants[k].jour > 31);

    do {
        etudiants[k].mois = lire_int_sure("Mois de naissance [1-12] : ");
    } while (etudiants[k].mois < 1 || etudiants[k].mois > 12);

    do {
        etudiants[k].annee = lire_int_sure("Annee de naissance (< 2024) : ");
    } while (etudiants[k].annee <= 0 || etudiants[k].annee >= 2024);

    printf("Nouveau lieu de naissance (un seul mot) : ");
    scanf("%29s", etudiants[k].lieu_de_naissance);

    lire_chaine("Nouvelle adresse : ", etudiants[k].adresse, MAX_ADDR_LEN);

    sync_etudiants_view();

    int y = lire_int_sure("Mettre a jour le fichier etudiants.txt ? (0=oui / 1=non) : ");
    if (y == 0) {
        f = fopen("etudiants.txt", "w");
        if (!f) {
            printf("Erreur d'ouverture du fichier etudiants.txt.\n");
        } else {
            fprintf(f, "la liste des etudiants : \n\n");
            fprintf(f, "matricule | nom | prenom | date de naissance | lieu de naissance | adresse \n\n");
            for (int i = 1; i <= n; ++i) {
                fprintf(f, "%d | %s | %s | %d/%d/%d | %s | %s\n",
                        etudiants[i].matricule,
                        etudiants[i].nom,
                        etudiants[i].prenom,
                        etudiants[i].jour,
                        etudiants[i].mois,
                        etudiants[i].annee,
                        etudiants[i].lieu_de_naissance,
                        etudiants[i].adresse);
            }
            fclose(f);
        }
    }

    reglage();
}

void supprimer(void) {
    if (n == 0) {
        printf("La liste est vide.\n");
        return;
    }

    int mat, k = 0;

    printf("Liste des etudiants enregistres :\n");
    for (int i = 1; i <= n; ++i) {
        printf("Matricule: %d  Etudiant: %s %s\n",
               etudiants[i].matricule, etudiants[i].nom, etudiants[i].prenom);
    }

    mat = lire_int_sure("\nSaisir le matricule de l'etudiant a supprimer : ");

    for (int i = 1; i <= n; ++i) {
        if (etudiants[i].matricule == mat) {
            k = i;
            break;
        }
    }

    if (k == 0) {
        printf("Aucun etudiant avec ce matricule.\n");
        return;
    }

    for (int j = k; j < n; ++j) {
        etudiants[j] = etudiants[j + 1];
    }
    --n;

    sync_etudiants_view();

    int y = lire_int_sure("Supprimer aussi dans le fichier etudiants.txt ? (0=oui / 1=non) : ");
    if (y == 0) {
        FILE *f = fopen("etudiants.txt", "w");
        if (!f) {
            printf("Erreur d'ouverture du fichier etudiants.txt.\n");
        } else {
            fprintf(f, "la liste des etudiants : \n\n");
            fprintf(f, "matricule | nom | prenom | date de naissance | lieu de naissance | adresse \n\n");
            for (int i = 1; i <= n; ++i) {
                fprintf(f, "%d | %s | %s | %d/%d/%d | %s | %s\n",
                        etudiants[i].matricule,
                        etudiants[i].nom,
                        etudiants[i].prenom,
                        etudiants[i].jour,
                        etudiants[i].mois,
                        etudiants[i].annee,
                        etudiants[i].lieu_de_naissance,
                        etudiants[i].adresse);
            }
            fclose(f);
        }
    }

    reglage();
}

void ajouter(void) {
    if (n >= MAX_ETUDIANTS) {
        printf("Nombre maximal d'etudiants atteint.\n");
        return;
    }

    int idx = n + 1;
    int matricule_ok = 0;

    printf("\nSaisir les informations de l'etudiant :\n\n");

    // Matricule unique
    while (!matricule_ok) {
        etudiants[idx].matricule = lire_int_sure("matricule : ");
        matricule_ok = 1;
        for (int j = 1; j <= n; ++j) {
            if (etudiants[idx].matricule == etudiants[j].matricule) {
                printf("Ce matricule est déjà utilise, reessayez.\n");
                matricule_ok = 0;
                break;
            }
        }
    }

    printf("Nom : ");
    scanf("%29s", etudiants[idx].nom);

    printf("Prenom : ");
    scanf("%29s", etudiants[idx].prenom);

    do {
        etudiants[idx].jour = lire_int_sure("Jour de naissance [1-31] : ");
    } while (etudiants[idx].jour < 1 || etudiants[idx].jour > 31);

    do {
        etudiants[idx].mois = lire_int_sure("Mois de naissance [1-12] : ");
    } while (etudiants[idx].mois < 1 || etudiants[idx].mois > 12);

    do {
        etudiants[idx].annee = lire_int_sure("Annee de naissance (< 2024) : ");
    } while (etudiants[idx].annee <= 0 || etudiants[idx].annee >= 2024);

    printf("Lieu de naissance (un seul mot) : ");
    scanf("%29s", etudiants[idx].lieu_de_naissance);

    lire_chaine("Adresse : ", etudiants[idx].adresse, MAX_ADDR_LEN);

    ++n;
    sync_etudiants_view();

    int y = lire_int_sure("Ajouter aussi dans le fichier etudiants.txt ? (0=oui / 1=non) : ");
    if (y == 0) {
        FILE *f = fopen("etudiants.txt", "a");
        if (!f) {
            printf("Erreur d'ouverture du fichier etudiants.txt.\n");
        } else {
            fprintf(f, "%d | %s | %s | %d/%d/%d | %s | %s\n",
                    etudiants[idx].matricule,
                    etudiants[idx].nom,
                    etudiants[idx].prenom,
                    etudiants[idx].jour,
                    etudiants[idx].mois,
                    etudiants[idx].annee,
                    etudiants[idx].lieu_de_naissance,
                    etudiants[idx].adresse);
            fclose(f);
        }
    }

    printf("\n");
}

//======================================================================
// Tri des étudiants par nom + fenêtre
//======================================================================

void trier_ordre_alphabetique_ascendent(void) {
    if (n == 0) {
        printf("La liste est vide.\n");
        return;
    }

    sync_etudiants_view();

    for (int i = 1; i <= n - 1; ++i) {
        for (int j = 1; j <= n - i; ++j) {
            if (strcoll(etudiants2[j].nom, etudiants2[j + 1].nom) > 0) {
                EtudiantView tmp = etudiants2[j];
                etudiants2[j] = etudiants2[j + 1];
                etudiants2[j + 1] = tmp;
            }
        }
    }

    char buffer[MAX_BUFFER_FENETRE];
    buffer[0] = '\0';
    append_line(buffer, sizeof(buffer), "Etudiants (ordre alphabetique ascendant) :\n");

    printf("Les etudiants tries par nom (ascendant) :\n\n");
    for (int i = 1; i <= n; ++i) {
        printf("\nMatricule : %d\n", etudiants2[i].matricule);
        printf("Nom       : %s\n", etudiants2[i].nom);
        printf("Prenom    : %s\n", etudiants2[i].prenom);
        printf("Naissance : %d/%d/%d\n",
               etudiants2[i].jour, etudiants2[i].mois, etudiants2[i].annee);
        printf("Lieu      : %s\n", etudiants2[i].lieu_de_naissance);
        printf("Adresse   : %s\n", etudiants2[i].adresse);

        append_line(buffer, sizeof(buffer),
                    "\nMatricule: %d\nNom: %s\nPrenom: %s\nNaissance: %d/%d/%d\nLieu: %s\nAdresse: %s\n",
                    etudiants2[i].matricule,
                    etudiants2[i].nom,
                    etudiants2[i].prenom,
                    etudiants2[i].jour,
                    etudiants2[i].mois,
                    etudiants2[i].annee,
                    etudiants2[i].lieu_de_naissance,
                    etudiants2[i].adresse);
    }

    show_text_window("Etudiants - ordre alphabetique ascendant", buffer);
}

void trier_ordre_alphabetique_descendant(void) {
    if (n == 0) {
        printf("La liste est vide.\n");
        return;
    }

    sync_etudiants_view();

    for (int i = 1; i <= n - 1; ++i) {
        for (int j = 1; j <= n - i; ++j) {
            if (strcoll(etudiants2[j].nom, etudiants2[j + 1].nom) < 0) {
                EtudiantView tmp = etudiants2[j];
                etudiants2[j] = etudiants2[j + 1];
                etudiants2[j + 1] = tmp;
            }
        }
    }

    char buffer[MAX_BUFFER_FENETRE];
    buffer[0] = '\0';
    append_line(buffer, sizeof(buffer), "Etudiants (ordre alphabetique descendant) :\n");

    printf("Les etudiants tries par nom (descendant) :\n\n");
    for (int i = 1; i <= n; ++i) {
        printf("\nMatricule : %d\n", etudiants2[i].matricule);
        printf("Nom       : %s\n", etudiants2[i].nom);
        printf("Prenom    : %s\n", etudiants2[i].prenom);
        printf("Naissance : %d/%d/%d\n",
               etudiants2[i].jour, etudiants2[i].mois, etudiants2[i].annee);
        printf("Lieu      : %s\n", etudiants2[i].lieu_de_naissance);
        printf("Adresse   : %s\n", etudiants2[i].adresse);

        append_line(buffer, sizeof(buffer),
                    "\nMatricule: %d\nNom: %s\nPrenom: %s\nNaissance: %d/%d/%d\nLieu: %s\nAdresse: %s\n",
                    etudiants2[i].matricule,
                    etudiants2[i].nom,
                    etudiants2[i].prenom,
                    etudiants2[i].jour,
                    etudiants2[i].mois,
                    etudiants2[i].annee,
                    etudiants2[i].lieu_de_naissance,
                    etudiants2[i].adresse);
    }

    show_text_window("Etudiants - ordre alphabetique descendant", buffer);
}

//======================================================================
// Modules
//======================================================================

void saisir_les_modules(void) {
    FILE *m = fopen("modules.txt", "w");
    if (!m) {
        printf("Erreur d'ouverture de modules.txt.\n");
        return;
    }

    int nb = lire_int_sure("Entrer le nombre de modules que vous voulez saisir : ");
    printf("\n");

    fprintf(m, "la liste des modules : \n\n");
    fprintf(m, "code | Nom | coefficient \n\n");

    for (int k = 0; k < nb && nm < MAX_MODULES; ++k) {
        int idx = nm + 1;
        int code_ok = 0;

        printf("Saisir les informations du module numero %d :\n\n", idx);

        while (!code_ok) {
            modules[idx].code = lire_int_sure("code : ");
            code_ok = 1;
            for (int j = 1; j <= nm; ++j) {
                if (modules[idx].code == modules[j].code) {
                    printf("Code deja utilise, reessayez.\n");
                    code_ok = 0;
                    break;
                }
            }
        }

        printf("Nom : ");
        scanf("%49s", modules[idx].nom);

        modules[idx].coef = lire_int_sure("coefficient : ");
        modules[idx].moy  = 0.0f;

        fprintf(m, "%d | %s | %d\n",
                modules[idx].code, modules[idx].nom, modules[idx].coef);

        ++nm;
    }

    fclose(m);
    sync_modules_view();
}

void ajouter_modules(void) {
    if (nm >= MAX_MODULES) {
        printf("Nombre maximal de modules atteint.\n");
        return;
    }

    FILE *m = fopen("modules.txt", "a");
    if (!m) {
        printf("Erreur d'ouverture de modules.txt.\n");
        return;
    }

    int idx = nm + 1;
    int code_ok = 0;

    printf("Saisir les informations du module numero %d :\n\n", idx);

    while (!code_ok) {
        modules[idx].code = lire_int_sure("code : ");
        code_ok = 1;
        for (int j = 1; j <= nm; ++j) {
            if (modules[idx].code == modules[j].code) {
                printf("Code deja utilise, reessayez.\n");
                code_ok = 0;
                break;
            }
        }
    }

    printf("Nom : ");
    scanf("%49s", modules[idx].nom);

    modules[idx].coef = lire_int_sure("coefficient : ");
    modules[idx].moy  = 0.0f;

    fprintf(m, "%d | %s | %d\n",
            modules[idx].code, modules[idx].nom, modules[idx].coef);

    ++nm;

    fclose(m);
    sync_modules_view();
}

void modifier_module(void) {
    if (nm == 0) {
        printf("La liste des modules est vide.\n");
        return;
    }

    printf("Liste des modules :\n");
    for (int i = 1; i <= nm; ++i) {
        printf("code : %d   nom : %s\n", modules[i].code, modules[i].nom);
    }

    int cod = lire_int_sure("\nSaisir le code du module a modifier : ");
    int k = 0;

    for (int i = 1; i <= nm; ++i) {
        if (modules[i].code == cod) {
            k = i;
            break;
        }
    }

    if (k == 0) {
        printf("Aucun module avec ce code.\n");
        return;
    }

    // nouveau code unique
    int ok = 0;
    while (!ok) {
        modules[k].code = lire_int_sure("Nouveau code : ");
        ok = 1;
        for (int j = 1; j <= nm; ++j) {
            if (j != k && modules[j].code == modules[k].code) {
                printf("Code deja utilise, reessayez.\n");
                ok = 0;
                break;
            }
        }
    }

    printf("Nouveau nom : ");
    scanf("%49s", modules[k].nom);

    modules[k].coef = lire_int_sure("Nouveau coefficient : ");

    sync_modules_view();

    FILE *md = fopen("modules.txt", "w");
    if (!md) {
        printf("Erreur d'ouverture de modules.txt.\n");
        return;
    }

    fprintf(md, "la liste des modules : \n\n");
    fprintf(md, "code | Nom | coefficient \n\n");
    for (int i = 1; i <= nm; ++i) {
        fprintf(md, "%d | %s | %d\n",
                modules[i].code, modules[i].nom, modules[i].coef);
    }

    fclose(md);
}

void supprimer_module(void) {
    if (nm == 0) {
        printf("La liste des modules est vide.\n");
        return;
    }

    printf("Liste des modules :\n");
    for (int i = 1; i <= nm; ++i) {
        printf("code : %d   nom : %s\n", modules[i].code, modules[i].nom);
    }

    int cod = lire_int_sure("\nSaisir le code du module a supprimer : ");
    int k = 0;

    for (int i = 1; i <= nm; ++i) {
        if (modules[i].code == cod) {
            k = i;
            break;
        }
    }

    if (k == 0) {
        printf("Aucun module avec ce code.\n");
        return;
    }

    for (int j = k; j < nm; ++j) {
        modules[j] = modules[j + 1];
    }
    --nm;

    sync_modules_view();

    FILE *m = fopen("modules.txt", "w");
    if (!m) {
        printf("Erreur d'ouverture de modules.txt.\n");
        return;
    }

    fprintf(m, "la liste des modules : \n\n");
    fprintf(m, "code | Nom | coefficient \n\n");
    for (int i = 1; i <= nm; ++i) {
        fprintf(m, "%d | %s | %d\n",
                modules[i].code, modules[i].nom, modules[i].coef);
    }

    fclose(m);
}

void afficher_les_modules(void) {
    if (nm == 0) {
        printf("La liste des modules est vide.\n");
        return;
    }

    for (int i = 1; i <= nm; ++i) {
        printf("\nNom du module: %s\tCode: %d\tCoefficient: %d\n",
               modules[i].nom, modules[i].code, modules[i].coef);
    }
}

//======================================================================
// Tri modules par nom
//======================================================================

void trier_module_par_nom_ascendent(void) {
    if (nm == 0) {
        printf("La liste des modules est vide.\n");
        return;
    }

    sync_modules_view();

    for (int i = 1; i <= nm - 1; ++i) {
        for (int j = 1; j <= nm - i; ++j) {
            if (strcoll(modules2[j].nom, modules2[j + 1].nom) > 0) {
                ModuleView tmp = modules2[j];
                modules2[j] = modules2[j + 1];
                modules2[j + 1] = tmp;
            }
        }
    }

    printf("Modules tries par nom (ascendant) :\n");
    for (int i = 1; i <= nm; ++i) {
        printf("\nNom : %s\tCode : %d\tCoefficient : %d\n",
               modules2[i].nom, modules2[i].code, modules2[i].coef);
    }
}

void trier_module_par_nom_descendant(void) {
    if (nm == 0) {
        printf("La liste des modules est vide.\n");
        return;
    }

    sync_modules_view();

    for (int i = 1; i <= nm - 1; ++i) {
        for (int j = 1; j <= nm - i; ++j) {
            if (strcoll(modules2[j].nom, modules2[j + 1].nom) < 0) {
                ModuleView tmp = modules2[j];
                modules2[j] = modules2[j + 1];
                modules2[j + 1] = tmp;
            }
        }
    }

    printf("Modules tries par nom (descendant) :\n");
    for (int i = 1; i <= nm; ++i) {
        printf("\nNom : %s\tCode : %d\tCoefficient : %d\n",
               modules2[i].nom, modules2[i].code, modules2[i].coef);
    }
}

//======================================================================
// Notes
//======================================================================

void saisir_les_notes(void) {
    if (n == 0 || nm == 0) {
        printf("Veuillez d'abord saisir etudiants et modules.\n");
        return;
    }

    printf("Liste des etudiants :\n");
    for (int i = 1; i <= n; ++i) {
        printf("Matricule: %d  %s %s\n",
               etudiants[i].matricule,
               etudiants[i].nom,
               etudiants[i].prenom);
    }

    int continuer = 1;
    while (continuer == 1) {
        int mat = lire_int_sure("\nDonnez le matricule de l'etudiant : ");
        int pos = 0;

        for (int i = 1; i <= n; ++i) {
            if (etudiants[i].matricule == mat) {
                pos = i;
                break;
            }
        }

        if (!pos) {
            printf("Etudiant inexistant.\n");
        } else {
            printf("Saisie des notes pour %s %s :\n",
                   etudiants[pos].nom, etudiants[pos].prenom);

            for (int j = 1; j <= nm; ++j) {
                float note_val;
                do {
                    printf("Module %s : ", modules[j].nom);
                    int ret = scanf("%f", &note_val);
                    if (ret != 1) {
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF) {}
                        note_val = -1.0f;
                    }
                    if (note_val < 0.0f || note_val > 20.0f) {
                        printf("La note doit etre entre 0 et 20.\n");
                    }
                } while (note_val < 0.0f || note_val > 20.0f);

                notes[pos][j] = note_val;
            }
        }

        continuer = lire_int_sure("Saisir les notes d'un autre etudiant ? (1=oui / 0=non) : ");
    }

    reglage();
}

void afficher_les_notes(void) {
    if (n == 0 || nm == 0) {
        printf("Aucune note disponible.\n");
        return;
    }

    printf("Liste des etudiants :\n");
    for (int i = 1; i <= n; ++i) {
        printf("Matricule: %d  %s %s\n",
               etudiants[i].matricule,
               etudiants[i].nom,
               etudiants[i].prenom);
    }

    int continuer = 1;
    while (continuer == 1) {
        int mat = lire_int_sure("\nDonnez le matricule de l'etudiant : ");
        int pos = 0;

        for (int i = 1; i <= n; ++i) {
            if (etudiants[i].matricule == mat) {
                pos = i;
                break;
            }
        }

        if (!pos) {
            printf("Etudiant inexistant.\n");
        } else {
            printf("Notes de %s %s :\n",
                   etudiants[pos].nom, etudiants[pos].prenom);
            for (int j = 1; j <= nm; ++j) {
                printf("-> %s : %0.2f\n", modules[j].nom, notes[pos][j]);
            }
        }

        continuer = lire_int_sure("Afficher les notes d'un autre etudiant ? (1=oui / 0=non) : ");
    }
}

//======================================================================
// Moyennes étudiants (tri croissant / décroissant) + fenêtres
//======================================================================

void afficher_moy_decroi(void) {
    if (n == 0 || nm == 0) {
        printf("Impossible de calculer les moyennes.\n");
        return;
    }

    sync_etudiants_view();

    for (int i = 1; i <= n; ++i) {
        float s = 0.0f;
        int coef_sum = 0;
        for (int j = 1; j <= nm; ++j) {
            s += notes[i][j] * modules[j].coef;
            coef_sum += modules[j].coef;
        }
        etudiants2[i].moy = (coef_sum > 0) ? (s / coef_sum) : 0.0f;
    }

    for (int i = 1; i <= n - 1; ++i) {
        for (int j = 1; j <= n - i; ++j) {
            if (etudiants2[j].moy < etudiants2[j + 1].moy) {
                EtudiantView tmp = etudiants2[j];
                etudiants2[j] = etudiants2[j + 1];
                etudiants2[j + 1] = tmp;
            }
        }
    }

    char buffer[MAX_BUFFER_FENETRE];
    buffer[0] = '\0';
    append_line(buffer, sizeof(buffer), "Etudiants par moyenne decroissante :\n");

    printf("Etudiants tries par moyenne decroissante :\n");
    for (int i = 1; i <= n; ++i) {
        printf("\nMatricule : %d\n", etudiants2[i].matricule);
        printf("Nom       : %s\n", etudiants2[i].nom);
        printf("Prenom    : %s\n", etudiants2[i].prenom);
        printf("Moyenne   : %0.2f\n", etudiants2[i].moy);

        append_line(buffer, sizeof(buffer),
                    "\nMatricule: %d\nNom: %s\nPrenom: %s\nMoyenne: %0.2f\n",
                    etudiants2[i].matricule,
                    etudiants2[i].nom,
                    etudiants2[i].prenom,
                    etudiants2[i].moy);
    }

    show_text_window("Etudiants - Moyenne decroissante", buffer);
}

void afficher_moy_CROIS(void) {
    if (n == 0 || nm == 0) {
        printf("Impossible de calculer les moyennes.\n");
        return;
    }

    sync_etudiants_view();

    for (int i = 1; i <= n; ++i) {
        float s = 0.0f;
        int coef_sum = 0;
        for (int j = 1; j <= nm; ++j) {
            s += notes[i][j] * modules[j].coef;
            coef_sum += modules[j].coef;
        }
        etudiants2[i].moy = (coef_sum > 0) ? (s / coef_sum) : 0.0f;
    }

    for (int i = 1; i <= n - 1; ++i) {
        for (int j = 1; j <= n - i; ++j) {
            if (etudiants2[j].moy > etudiants2[j + 1].moy) {
                EtudiantView tmp = etudiants2[j];
                etudiants2[j] = etudiants2[j + 1];
                etudiants2[j + 1] = tmp;
            }
        }
    }

    char buffer[MAX_BUFFER_FENETRE];
    buffer[0] = '\0';
    append_line(buffer, sizeof(buffer), "Etudiants par moyenne croissante :\n");

    printf("Etudiants tries par moyenne croissante :\n");
    for (int i = 1; i <= n; ++i) {
        printf("\nMatricule : %d\n", etudiants2[i].matricule);
        printf("Nom       : %s\n", etudiants2[i].nom);
        printf("Prenom    : %s\n", etudiants2[i].prenom);
        printf("Moyenne   : %0.2f\n", etudiants2[i].moy);

        append_line(buffer, sizeof(buffer),
                    "\nMatricule: %d\nNom: %s\nPrenom: %s\nMoyenne: %0.2f\n",
                    etudiants2[i].matricule,
                    etudiants2[i].nom,
                    etudiants2[i].prenom,
                    etudiants2[i].moy);
    }

    show_text_window("Etudiants - Moyenne croissante", buffer);
}

//======================================================================
// Moyennes modules (tri croissant / décroissant)
//======================================================================

void moyenne_module_crois(void) {
    if (n == 0 || nm == 0) {
        printf("Impossible de calculer les moyennes de modules.\n");
        return;
    }

    sync_modules_view();

    for (int j = 1; j <= nm; ++j) {
        float s = 0.0f;
        int c = 0;
        for (int i = 1; i <= n; ++i) {
            s += notes[i][j];
            ++c;
        }
        modules2[j].moy = (c > 0) ? (s / c) : 0.0f;
    }

    for (int i = 1; i <= nm - 1; ++i) {
        for (int j = 1; j <= nm - i; ++j) {
            if (modules2[j].moy > modules2[j + 1].moy) {
                ModuleView tmp = modules2[j];
                modules2[j] = modules2[j + 1];
                modules2[j + 1] = tmp;
            }
        }
    }

    printf("Modules tries par moyenne croissante :\n");
    for (int i = 1; i <= nm; ++i) {
        printf("\nNom : %s\tCode : %d\tCoeff : %d\tMoyenne : %0.2f\n",
               modules2[i].nom, modules2[i].code, modules2[i].coef, modules2[i].moy);
    }
}

void moyenne_module_decroi(void) {
    if (n == 0 || nm == 0) {
        printf("Impossible de calculer les moyennes de modules.\n");
        return;
    }

    sync_modules_view();

    for (int j = 1; j <= nm; ++j) {
        float s = 0.0f;
        int c = 0;
        for (int i = 1; i <= n; ++i) {
            s += notes[i][j];
            ++c;
        }
        modules2[j].moy = (c > 0) ? (s / c) : 0.0f;
    }

    for (int i = 1; i <= nm - 1; ++i) {
        for (int j = 1; j <= nm - i; ++j) {
            if (modules2[j].moy < modules2[j + 1].moy) {
                ModuleView tmp = modules2[j];
                modules2[j] = modules2[j + 1];
                modules2[j + 1] = tmp;
            }
        }
    }

    printf("Modules tries par moyenne decroissante :\n");
    for (int i = 1; i <= nm; ++i) {
        printf("\nNom : %s\tCode : %d\tCoeff : %d\tMoyenne : %0.2f\n",
               modules2[i].nom, modules2[i].code, modules2[i].coef, modules2[i].moy);
    }
}

//======================================================================
// Quitter
//======================================================================

void quitter(void) {
    system("cls");
    printf("\n\n\t------------ FIN -----------\n\n");
    printf("\t------------ MERCI -----------\n\n");
}

//======================================================================
// MAIN
//======================================================================

int main(void) {
menu:
    do {
        printf("\n\033[1;34m   *******      Gestion des etudiants    ****** \033[0m\n\n");
        printf("=================================================================== \n\n");
        printf("  Total des etudiants : %d\n", n);
        printf("  Total des modules   : %d\n\n", nm);
        printf("-------------------Etudiants---------------------------------------- \n");
        printf("  1.  Saisir les etudiants\n");
        printf("  2.  Afficher la liste des etudiants\n");
        printf("  3.  Gérer les etudiants\n\n");
        printf("-------------------Modules------------------------------------------ \n");
        printf("  4.  Saisir les modules\n");
        printf("  5.  Gérer les modules\n");
        printf("  6.  Afficher les modules\n\n");
        printf("-------------------Notes-------------------------------------------- \n");
        printf("  7.  Saisir les notes\n");
        printf("  8.  Afficher les notes\n\n");
        printf("  9.  Quitter\n");
        printf("=================================================================== \n\n");
        printf("\t\033[1;33mPS : en cas d'espace, utilisez '_' \033[0m\n\n");
        printf("Saisissez votre choix : ");

        if (scanf("%d", &choix) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            choix = -1;
        }

        printf("\n");

        switch (choix) {
            case 1:
                system("cls");
                saisir_des_etudiant();
                system("cls");
                break;

            case 2: {
                system("cls");
                printf("\n******* Choisissez l'ordre d'affichage ******\n\n");
                printf(" 1. Ordre d'enregistrement\n");
                printf(" 2. Ordre alphabetique\n");
                printf(" 3. Ordre de leur moyenne\n");
                printf(" 0. Retour au menu\n\n");
                printf("Votre choix : ");
                scanf("%d", &choixxx);

                switch (choixxx) {
                    case 1:
                        system("cls");
                        afficher_les_etudiants();
                        break;
                    case 2:
                        system("cls");
                        printf("1. Ascendant\n2. Descendant\nVotre choix : ");
                        scanf("%d", &choixxx);
                        if (choixxx == 1) {
                            system("cls");
                            trier_ordre_alphabetique_ascendent();
                        } else if (choixxx == 2) {
                            system("cls");
                            trier_ordre_alphabetique_descendant();
                        }
                        break;
                    case 3:
                        system("cls");
                        printf("1. Ordre croissant\n2. Ordre decroissant\nVotre choix : ");
                        scanf("%d", &choixxx);
                        if (choixxx == 1) {
                            system("cls");
                            afficher_moy_CROIS();
                        } else if (choixxx == 2) {
                            system("cls");
                            afficher_moy_decroi();
                        }
                        break;
                    case 0:
                        system("cls");
                        goto menu;
                    default:
                        break;
                }
                break;
            }

            case 3: {
                system("cls");
                printf("\n******* Choisissez l'action ******\n\n");
                printf("1. Supprimer\n");
                printf("2. Modifier\n");
                printf("3. Ajouter\n");
                printf("0. Retour au menu\n\n");
                printf("Votre choix : ");
                scanf("%d", &choixx);

                switch (choixx) {
                    case 1:
                        system("cls");
                        supprimer();
                        system("cls");
                        break;
                    case 2:
                        system("cls");
                        modifier();
                        system("cls");
                        break;
                    case 3:
                        system("cls");
                        ajouter();
                        system("cls");
                        break;
                    case 0:
                        system("cls");
                        goto menu;
                    default:
                        break;
                }
                break;
            }

            case 4:
                system("cls");
                saisir_les_modules();
                system("cls");
                break;

            case 5: {
                system("cls");
                printf("\n******* Choisissez l'action ******\n\n");
                printf("1. Supprimer\n");
                printf("2. Modifier\n");
                printf("3. Ajouter\n");
                printf("0. Retour au menu\n\n");
                printf("Votre choix : ");
                scanf("%d", &choixxxx);

                switch (choixxxx) {
                    case 1:
                        system("cls");
                        supprimer_module();
                        system("cls");
                        break;
                    case 2:
                        system("cls");
                        modifier_module();
                        system("cls");
                        break;
                    case 3:
                        system("cls");
                        ajouter_modules();
                        system("cls");
                        break;
                    case 0:
                        system("cls");
                        goto menu;
                    default:
                        break;
                }
                break;
            }

            case 6: {
                system("cls");
                printf("\n******* Choisissez l'ordre d'affichage ******\n\n");
                printf("1. Ordre d'enregistrement\n");
                printf("2. Ordre alphabetique\n");
                printf("3. Ordre par moyenne\n");
                printf("0. Retour au menu\n\n");
                printf("Votre choix : ");
                scanf("%d", &choixxx);

                switch (choixxx) {
                    case 1:
                        system("cls");
                        afficher_les_modules();
                        break;
                    case 2:
                        system("cls");
                        printf("1. Ascendant\n2. Descendant\nVotre choix : ");
                        scanf("%d", &choixxx);
                        if (choixxx == 1) {
                            system("cls");
                            trier_module_par_nom_ascendent();
                        } else if (choixxx == 2) {
                            system("cls");
                            trier_module_par_nom_descendant();
                        }
                        break;
                    case 3:
                        system("cls");
                        printf("1. Moyenne croissante\n2. Moyenne decroissante\nVotre choix : ");
                        scanf("%d", &choixxx);
                        if (choixxx == 1) {
                            system("cls");
                            moyenne_module_crois();
                        } else if (choixxx == 2) {
                            system("cls");
                            moyenne_module_decroi();
                        }
                        break;
                    case 0:
                        system("cls");
                        goto menu;
                    default:
                        break;
                }
                break;
            }

            case 7:
                system("cls");
                saisir_les_notes();
                system("cls");
                break;

            case 8:
                system("cls");
                afficher_les_notes();
                break;

            case 9:
                system("cls");
                quitter();
                break;

            default:
                printf("Choix errone !!! le choix doit etre entre [1 - 9]\n");
                break;
        }

    } while (choix != 9);

    return 0;
}
