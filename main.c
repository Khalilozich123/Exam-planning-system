#include <hpdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>

#define MAX_MODULES 100
#define MAX_PROFS 100
#define MAX_SALLES 100

//Structure des données


typedef struct {
    char filiere[100];
    char semestre[100];
    char nom[100];
    char identifiant[20];
} Module;

typedef struct {
    char nom[100];
    char identifiant[20];
} Professeur;

typedef struct {
    char nom[100];
} Salle;

typedef struct{
    char nom[50];
    char prenom[50];
    char password[20];
    char email[50];
} User;

//Décoration

void gotoxy(int x, int y) {     //Fonction pour controler la position du curseur
    COORD coord;
    coord.X = x - 1;    // SetConsoleCursorPosition expects 0-based coordinates
    coord.Y = y - 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void getConsoleSize(int *columns, int *lines) {   // Fonction pour calculer la taille du console
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        *columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *columns = 0; // Error case
        *lines = 0;   // Error case
    }
}

void setConsoleColor(int textColor, int bgColor) {    // Fonction pour changer les couleurs
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int color = (bgColor << 4) | textColor;  
    SetConsoleTextAttribute(hConsole, color);
}

void resetConsoleColor() {    // Fonction qui nous rend à la couleur initiale
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);  
}

void displayTitle() {   // Fonction pour afficher le titre du programme
    int columns, lines;
    getConsoleSize(&columns, &lines);
    int titleX;
    titleX = (columns - 52) / 2; // Centre du console
    if (titleX < 0) titleX = 0;

    setConsoleColor(9, 0);  // Light blue text
    gotoxy(titleX, 3);
    printf("+--------------------------------------------------+");
    gotoxy(titleX, 4);
    printf("|            PLANIFICATION DES EXAMENS             |");
    gotoxy(titleX, 5);
    printf("+--------------------------------------------------+");

    resetConsoleColor();
}

void displayMenu(int titleX) {   // Fonction qui affiche le menu principal
    gotoxy(titleX, 7);
    printf("═════════ Veuillez sélectionner votre rôle ═════════");
    
    gotoxy(titleX, 12);
    setConsoleColor(10, 0); // Green text
    printf("1. Admin");
    gotoxy(titleX, 15);
    printf("2. Étudiant");
    resetConsoleColor();

    gotoxy(titleX, 18);
    setConsoleColor(14, 0); // Yellow text
    printf("Votre choix: ");
    resetConsoleColor();
}

void displayMenu2(int *choix) {  // Fonction qui affiche les choix d'Admin
    system("cls");
    displayTitle();

    SetConsoleOutputCP(CP_UTF8);  

    int columns, lines;
    getConsoleSize(&columns, &lines);

    int titleX = (columns - 52) / 2;


    
    gotoxy(titleX, 12);
    setConsoleColor(10, 0); // Green text
    printf("1. Generer par session");
    gotoxy(titleX, 15);
    printf("2. Generer par filiere");
    resetConsoleColor();

    gotoxy(titleX, 18);
    setConsoleColor(14, 0); // Yellow text
    printf("Votre choix: ");
    resetConsoleColor();
    scanf("%d",choix);
    resetConsoleColor();
}

void clearscreen(){   // Fonction qui réinitialise le console
    system("cls");
    displayTitle();
}

void clearLine(int x, int y, int length) {   // Fonction qui efface une ligne
    gotoxy(x, y);
    for (int i = 0; i < length; i++) {
        printf(" ");  // Remplacer la ligne avec des espaces
    }
}

void error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data) {      // Fonction qui gère les erreurs du PDF
    printf("Error: %04X, Detail: %u\n", (unsigned int)error_no, (unsigned int)detail_no);
    exit(1);
}

void displayHomePage(int titleX, int* choix) {
    displayTitle();
    displayMenu(titleX);

    while (*choix != 1 && *choix != 2) {
        gotoxy(titleX + 12, 12); // Position for input
        printf(" "); // Clear previous input
        gotoxy(titleX + 12, 18);
        
        // Read input and check if it's valid
        if (scanf("%d", choix) != 1) {
            // Clear invalid input
            while(getchar() != '\n'); // Clear the input buffer
            *choix = 0; // Set to an invalid choice
        }

        // Provide feedback for invalid choices
        if (*choix != 1 && *choix != 2) {
            gotoxy(titleX, 20);
            setConsoleColor(12, 0); // Red text
            printf("Choix invalide. Veuillez choisir 1 ou 2.\n");
            resetConsoleColor();
            Sleep(2000); // Wait for 2 seconds before clearing the message
            gotoxy(titleX, 20);
            printf("                                        "); // Clear the message
        }
    }
}

void removeSpaces(char *str) {      // Fonction qui efface les espaces dans une chaine de caratères
    char *src = str, *dst = str;
    while (*src) {
        if (!isspace((unsigned char)*src)) { // Vérifie si le caractère n'est pas un espace
            *dst++ = *src; // Copie le caractère non-espace
        }
        src++;
    }
    *dst = '\0'; // Terminer la chaîne
}

void toLowerCase(char *str) {       // Fonction qui transforme le majuscule en minuscule
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]); // Convertir chaque caractère en minuscule
    }
}

//Lecture des données

int lireModules(const char *filename, Module modules[]) {       // Fonction qui récupère les données des modules
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }

    int count = 0;
    char buffer[200];

    while (fgets(buffer,sizeof(buffer),file)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Supprime le saut de ligne

        // Faire une copie de la ligne pour préserver la chaîne originale
        char tempBuffer[200];
        strcpy(tempBuffer, buffer);

        // Découper la ligne avec strtok
        char *token = strtok(tempBuffer, ",");
        if (token != NULL) strcpy(modules[count].filiere, token);

        token = strtok(NULL, ",");
        if (token != NULL) strcpy(modules[count].semestre, token);

        token = strtok(NULL, ",");
        if (token != NULL) strcpy(modules[count].nom, token);

        token = strtok(NULL, ",");
        if (token != NULL) strcpy(modules[count].identifiant, token);

        // Vérifie si tous les champs sont remplis
        if (modules[count].filiere[0] && modules[count].semestre[0] &&
            modules[count].nom[0] && modules[count].identifiant[0]) {
            count++; // Incrémente le compteur uniquement si tous les champs sont valides
        } else {
            printf("Erreur: Ligne mal formatée : %s\n", buffer);
        }
    }

    fclose(file);
    return count;
}

int lireProfesseurs(const char *filename, Professeur professeurs[]) {       // Fonction qui récupère les données des professeurs
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }

    int count = 0;
    char buffer[200];

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Supprime le saut de ligne

        // Faire une copie de la ligne pour préserver la chaîne originale
        char tempBuffer[200];
        strcpy(tempBuffer, buffer);

        // Découper la ligne avec strtok
        char *token = strtok(tempBuffer, ",");
        if (token != NULL) {
            strcpy(professeurs[count].nom, token);
        }

        token = strtok(NULL, ",");
        if (token != NULL) {
            strcpy(professeurs[count].identifiant, token);
        }

        // Vérifie si les deux champs sont remplis
        if (professeurs[count].nom[0] && professeurs[count].identifiant[0]) {
            count++;
        } else {
            printf("Erreur: Ligne mal formatée : %s\n", buffer);
        }
    }

    fclose(file);
    return count;
}

int lireSalles(const char *filename, Salle salles[]) {      // Fonction qui récupère les données des salles
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", filename);
        return 0;
    }
    int count = 0;
    while (fgets(salles[count].nom, sizeof(salles[count].nom), file)) {
        salles[count].nom[strcspn(salles[count].nom, "\n")] = '\0';
        count++;
    }
    fclose(file);
    return count;
}

void Registration(User *e){     // Fonction d'inscription d'étudiant
    system("cls");
    displayTitle();

    SetConsoleOutputCP(CP_UTF8);  // Optionnel : Configurer en UTF-8 pour meilleure compatibilité

    int columns, lines;
    getConsoleSize(&columns, &lines);
    

    int titleX = (columns - 52) / 2;

    FILE *rpt;

    setConsoleColor(10, 0);
    gotoxy(titleX,12);
    printf("Entrer votre nom : ");
    resetConsoleColor();
    fgets(e->nom, 50, stdin);
    e->nom[strcspn(e->nom, "\n")] = 0;
    removeSpaces(e->nom); // Supprimer les espaces

    setConsoleColor(10, 0);
    gotoxy(titleX,15);
    printf("Entrer votre prenom : ");
    resetConsoleColor();
    fgets(e->prenom, 50, stdin);
    e->prenom[strcspn(e->prenom, "\n")] = 0;
    removeSpaces(e->prenom); // Supprimer les espaces

    snprintf(e->email, 50, "%s.%s@etu.uae.ac.ma", e->prenom, e->nom);
    toLowerCase(e->email);

    setConsoleColor(10, 0);
    gotoxy(titleX,18);
    printf("Entrer votre mot de passe : ");
    resetConsoleColor();
    fgets(e->password, 20, stdin);
    e->password[strcspn(e->password, "\n")] = 0;

    rpt=fopen("Data/users.txt","a");
    if (rpt==NULL){
        printf("error");
        exit(1);
    }
    else{
        fprintf(rpt, "%s %s %s %s\n", e->nom , e->prenom , e->password , e->email);
        gotoxy(titleX,22);
        setConsoleColor(4,0);
        printf("Vous etes inscrit !\n");
        Sleep(1500);
        resetConsoleColor();
        system("cls");
    }
    fclose(rpt);
    resetConsoleColor();
}

int checkuser(User *e){     // Fonction qui vérifie si l'utilisateur existe
    FILE *cpt;
    char line[100],nom[50],prenom[50],password[20],email[50];
    cpt=fopen("Data/users.txt","r");
    if (cpt==NULL){
        printf("Error");
        exit(1);
    }
    while(fgets(line,sizeof(line),cpt)){
        sscanf(line, "%s %s %s %s", nom,prenom,password,email);
        if (strcmp(password, e->password) == 0 && strcmp(email, e->email) == 0) {
        fclose(cpt);
        return 1; 
        }
    }
    fclose(cpt);
    return 0;
}

void connexion() {      // Fonction de connexion d'étudiant
    User user;
    int result;
    do {
        system("cls");
        displayTitle();

        SetConsoleOutputCP(CP_UTF8);  // Optionnel : Configurer en UTF-8 pour meilleure compatibilité

        int columns, lines;
        getConsoleSize(&columns, &lines);

        int titleX = (columns - 52) / 2;

        setConsoleColor(10, 0);
        gotoxy(titleX,12);
        printf("Entrer votre email: ");
        resetConsoleColor();
        fgets(user.email, 50, stdin);
        user.email[strcspn(user.email, "\n")] = 0;

        setConsoleColor(10, 0);
        gotoxy(titleX, 15);
        printf("Entrer votre mot de passe: ");
        resetConsoleColor();

        int i = 0;
        char ch;
        while (1) {
            ch = getch(); // Lire un caractère sans l'afficher
            if (ch == 13) { // Si la touche 'Entrée' est pressée
                user.password[i] = '\0'; // Terminer la chaîne
                break;
            } else if (ch == 8) { // Si la touche 'Retour arrière' est pressée
                if (i > 0) {
                    i--;
                    printf("\b \b"); // Effacer le dernier astérisque
                }
            } else {
                user.password[i] = ch; // Ajouter le caractère au mot de passe
                i++;
                printf("*"); // Afficher un astérisque
            }
        }
        resetConsoleColor();

        result = checkuser(&user);

        if (result) {
            gotoxy(titleX, 20);
            setConsoleColor(2, 0);  // Vert pour le succès
            printf("Succès!\n");
            resetConsoleColor();
            Sleep(1000);
            break;
        } else {
            gotoxy(titleX, 20);
            setConsoleColor(4, 0);  // Rouge pour l'erreur
            printf("Email ou mot de passe invalide\n");
            resetConsoleColor();
            Sleep(1000);  // Attendre 2 secondes avant de nettoyer l'écran
        }
    } while (!result);
}

void Login(int *Loginresult) {      // Fonction du choix entre inscription ou connexion d'étudiant
    system("cls");
    displayTitle();

    SetConsoleOutputCP(CP_UTF8);  // Optionnel : Configurer en UTF-8 pour meilleure compatibilité

    int columns, lines;
    getConsoleSize(&columns, &lines);

    int titleX = (columns - 52) / 2;

    User user;
    int choice;
    gotoxy(titleX, 12);
    setConsoleColor(10, 0); // Texte vert
    printf("1. Inscription");
    gotoxy(titleX, 15);
    printf("2. Connexion");
    resetConsoleColor();
    gotoxy(titleX, 18);
    setConsoleColor(14, 0); // Texte jaune
    printf("Votre choix: ");
    scanf("%d", &choice);
    resetConsoleColor();
    getchar();  // Consomme le retour à la ligne résiduel

    if (choice == 1) {
        Registration(&user);
        *Loginresult = 1;  // Retour à la page d'accueil
    } else if (choice == 2) {
        connexion();
        *Loginresult = 0;  // Continuer avec Étudiant
    } else {
        printf("Choix invalide. Retour à la page d'accueil.\n");
        *Loginresult = 1;  // Retour par défaut à la page d'accueil
        Sleep(1000);       // Petite pause pour laisser le message s'afficher
    }
}

void LoginAdmin(){      // Fonction de connexion d'administrateur
    char id[10];
    char password[50];
    do {
        system("cls");
        displayTitle();
        
        SetConsoleOutputCP(CP_UTF8);  // Optionnel : Configurer en UTF-8 pour meilleure compatibilité

        int columns, lines;
        getConsoleSize(&columns, &lines);

        int titleX = (columns - 52) / 2;
        gotoxy(titleX, 12);
        setConsoleColor(10, 0); // Green text
        printf("Entre l'ID : ");
        resetConsoleColor();
        scanf("%s", id);
        
        setConsoleColor(10, 0);
        gotoxy(titleX, 15);
        printf("Entrer le pass : ");
        resetConsoleColor();
        
        int i = 0;
        char ch;
        while (1) {
            ch = getch(); // Lire un caractère sans l'afficher
            if (ch == 13) { // Si la touche 'Entrée' est pressée
                password[i] = '\0'; // Terminer la chaîne
                break;
            } else if (ch == 8) { // Si la touche 'Retour arrière' est pressée
                if (i > 0) {
                    i--;
                    printf("\b \b"); // Effacer le dernier astérisque
                }
            } else {
                password[i] = ch; // Ajouter le caractère au mot de passe
                i++;
                printf("*"); // Afficher un astérisque
            }
        }
        resetConsoleColor();
        
        if (strcmp(id, "Adm") != 0 || strcmp(password, "Adm1456") != 0) {
            gotoxy(titleX, 20);
            setConsoleColor(12, 0); // Red text
            printf("Id ou password incorrecte");
            resetConsoleColor();
            Sleep(2000); // Wait for 2 seconds before clearing the screen
        }
    } while (strcmp(id, "Adm") != 0 || strcmp(password, "Adm1456") != 0);
}

//Les espaces

void AdminTout(const char *filiere,const char *semestre) {      // Fonction qui génère le planning en fonction de la session
    Module modules[MAX_MODULES];
    Professeur professeurs[MAX_PROFS];
    Salle salles[MAX_SALLES];
    int nbModules, nbProfs, nbSalles;

    // Seed the random number generator to ensure different results each time
    srand(time(NULL));

    // Génération des données
    char filename[100];
    snprintf(filename, sizeof(filename), "Data/%s.txt", filiere);
    nbModules = lireModules(filename, modules);
    nbProfs = lireProfesseurs("Data/Professeurs.txt", professeurs);
    nbSalles = lireSalles("Data/Salles.txt", salles);



    // Filtrer les modules selon la filière et le semestre
    Module modulesFiltres[MAX_MODULES];
    int nbModulesFiltres = 0;

    for (int i = 0; i < nbModules; i++) {
        if (strcmp(modules[i].filiere, filiere) == 0 && strcmp(modules[i].semestre, semestre) == 0) {
            modulesFiltres[nbModulesFiltres++] = modules[i];
        }
    }

    // Vérifier si des modules ont été trouvés
    if (nbModulesFiltres == 0) {
        printf("Aucun module trouvé pour la filière %s et le semestre %s.\n", filiere, semestre);
    }

    // Mélange des modules au hasard
    for (int i = nbModulesFiltres - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Module temp = modulesFiltres[i];
        modulesFiltres[i] = modulesFiltres[j];
        modulesFiltres[j] = temp;
    }

    for (int i = 0; i < nbModulesFiltres; i++) {
        const char *module = modulesFiltres[i].nom;
        const char *identifiant = modulesFiltres[i].identifiant;
        const char *professeur = NULL;

        // Associer un professeur au module
        for (int j = 0; j < nbProfs; j++) {
            if (strcmp(professeurs[j].identifiant, identifiant) == 0) {
                professeur = professeurs[j].nom;
                break;
            }
        }

        if (!professeur) {
            printf("Erreur : Aucun professeur trouve pour le module %s\n", module);
            continue;
        }
    }

    // Création de la source du PDF

    char file_path[100];
    snprintf(file_path, sizeof(file_path), "Planning/%s/%s.pdf", filiere,semestre);
    

    // Step 1: Create a new PDF document
    HPDF_Doc pdf = HPDF_New(error_handler, NULL);
    if (!pdf) {
        printf("Failed to create PDF object\n");
    }

    // Step 2: Add a page to the document
    HPDF_Page page = HPDF_AddPage(pdf);

    // Step 3: Set custom page size (297 mm x 210 mm, A4 Landscape)
    HPDF_Page_SetWidth(page, 841.89);  // 297 mm in points
    HPDF_Page_SetHeight(page, 595.28); // 210 mm in points

    // Step 4: Set font and write text to the page
    HPDF_Font font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
    HPDF_Page_SetFontAndSize(page, font, 12);

    // Step 5: Draw the images (LogoEnsah and LogoUAE)
    HPDF_Image image1 = HPDF_LoadPngImageFromFile(pdf, "Resources/LogoEnsah.png");
    if (!image1) {
        printf("Failed to load LogoEnsah.png\n");
        HPDF_Free(pdf);
    }

    HPDF_Image image2 = HPDF_LoadPngImageFromFile(pdf, "Resources/LogoUAE.png");
    if (!image2) {
        printf("Failed to load LogoUAE.png\n");
        HPDF_Free(pdf);
    }

    HPDF_REAL resized_width = 75;
    HPDF_REAL resized_height = 75;

    HPDF_Page_DrawImage(page, image1, 50, 595.28 - resized_height - 20, resized_width, resized_height);
    HPDF_Page_DrawImage(page, image2, 841.89 - resized_width - 50, 595.28 - resized_height - 20, resized_width, resized_height);

    // Step 6: Add text between the two images, centered
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, font, 12); // Use the default font and size
    HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0); // Black text

    // Calculate the width of the text block
    const char* text = "Université Abdelmalek Essaâdi\nEcole Nationale des Sciences Appliquées\nAl Hoceima";
    HPDF_REAL text_width = HPDF_Page_TextWidth(page, text);

    // Center the text between the two images
    HPDF_REAL text_x = (350 + (841.89 - resized_width - 50) - text_width) / 2;

    // Set a position slightly below the logos
    HPDF_REAL text_y = 595.28 - resized_height + 20; // Slightly below the images

    // Write the text in multiple lines
    HPDF_Page_TextOut(page, text_x+20, text_y, "Universite Abdelmalek Essaadi");
    HPDF_Page_TextOut(page, text_x+20, text_y - 15, "Ecole Nationale des Sciences Appliquees");
    HPDF_Page_TextOut(page, text_x+20, text_y - 30, "Al Hoceima");

    HPDF_Page_EndText(page);
    

    // Step 6: Draw the table
    HPDF_REAL table_start_x = 100;
    HPDF_REAL table_start_y = 350;
    HPDF_REAL first_column_width = 80;
    HPDF_REAL second_column_width = 290;
    HPDF_REAL third_column_width = 290;
    HPDF_REAL header_height = 20;
    HPDF_REAL row_height = 50;

    // Step 7: Draw the bold blue line above the table
    HPDF_Page_SetRGBFill(page, 0.0, 0.32, 0.75); // Dark blue color
    HPDF_Page_SetLineWidth(page, 2);  // Set line width to make it bold
    HPDF_Page_MoveTo(page, table_start_x, table_start_y + 50);  // Starting point of the line
    HPDF_Page_LineTo(page, table_start_x + first_column_width + second_column_width + third_column_width, table_start_y + 50); // Ending point of the line
    HPDF_Page_Stroke(page);

    // Step 7: Write the labels above the line
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, font, 12); // Use the default font and size

    // Write "Filiere" on the left side of the line
    HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0); // Black text
    HPDF_REAL filiere_text_x = table_start_x ; // Position the text a little left of the line
    HPDF_REAL filiere_text_y = table_start_y + 55; // Position text slightly below the line
    HPDF_Page_TextOut(page, filiere_text_x, filiere_text_y, "Filiere");

    // Write "Semestre" in the center of the line
    HPDF_REAL semestre_text_x = table_start_x + (first_column_width + second_column_width + third_column_width) / 2 - (HPDF_Page_TextWidth(page, "Semestre") / 2);
    HPDF_REAL semestre_text_y = table_start_y + 55;
    HPDF_Page_TextOut(page, semestre_text_x, semestre_text_y, "Semestre");

    // Write "Salle" on the right side of the line
    HPDF_REAL salle_text_x = table_start_x + first_column_width + second_column_width + third_column_width - HPDF_Page_TextWidth(page, "Salle") - 20;
    HPDF_REAL salle_text_y = table_start_y + 55;
    HPDF_Page_TextOut(page, salle_text_x, salle_text_y, "Salle");
    HPDF_Page_EndText(page);

    // Step 8: Write the selected Filiere, Semestre, and Salle under their respective labels
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, font, 12); // Use the default font and size

    // Write the chosen filiere
    HPDF_REAL filiere_value_x = table_start_x ; // Same X as "Filiere"
    HPDF_REAL filiere_value_y = table_start_y + 35; // Place it below "Filiere" label
    HPDF_Page_TextOut(page, filiere_value_x, filiere_value_y, filiere);

    // Write the chosen semestre
    HPDF_REAL semestre_value_x = table_start_x + (first_column_width + second_column_width + third_column_width) / 2 - (HPDF_Page_TextWidth(page, semestre) / 2);
    HPDF_REAL semestre_value_y = table_start_y + 35; // Place it below "Semestre" label
    HPDF_Page_TextOut(page, semestre_value_x, semestre_value_y, semestre);

    // Write the chosen salle
    const char *salle = salles[rand() % nbSalles].nom; // Random salle
    HPDF_REAL salle_value_x = table_start_x + first_column_width + second_column_width + third_column_width - HPDF_Page_TextWidth(page, salle) - 20;
    HPDF_REAL salle_value_y = table_start_y + 35; // Place it below "Salle" label
    HPDF_Page_TextOut(page, salle_value_x, salle_value_y, salle);
    HPDF_Page_EndText(page);

    // Draw header row with very light purple fill
    HPDF_Page_SetRGBFill(page, 0.95, 0.9, 1.0);
    HPDF_Page_Rectangle(page, table_start_x, table_start_y, first_column_width, -header_height);
    HPDF_Page_Rectangle(page, table_start_x + first_column_width, table_start_y, second_column_width, -header_height);
    HPDF_Page_Rectangle(page, table_start_x + first_column_width + second_column_width, table_start_y, third_column_width, -header_height);
    HPDF_Page_FillStroke(page);

    // Add header text in black
    HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
    HPDF_Page_BeginText(page);

    HPDF_REAL date_text_x = table_start_x + (first_column_width / 2) - (HPDF_Page_TextWidth(page, "Date") / 2);
    HPDF_REAL date_text_y = table_start_y - header_height / 2 - 5;
    HPDF_Page_TextOut(page, date_text_x, date_text_y, "Date");

    HPDF_Page_TextOut(page, table_start_x + first_column_width + 10, table_start_y - header_height + 5, "9:00");
    HPDF_Page_TextOut(page, table_start_x + first_column_width + second_column_width - 40, table_start_y - header_height + 5, "11:00");
    HPDF_Page_TextOut(page, table_start_x + first_column_width + second_column_width + 10, table_start_y - header_height + 5, "15:00");
    HPDF_Page_TextOut(page, table_start_x + first_column_width + second_column_width + third_column_width - 40, table_start_y - header_height + 5, "17:00");

    HPDF_Page_EndText(page);

    // Draw the remaining rows
    const char* days[] = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
    const char* dates1[] = {"30/12/2024", "31/12/2024", "01/01/2025", "02/01/2025", "03/01/2025", "04/01/2025"};
    const char* dates2[] = {"26/05/2025", "27/05/2025", "28/05/2025", "29/05/2025", "30/05/2025", "31/05/2025"};
    const char** dates = NULL; // Pointer to an array of const char*
    if (strcmp(semestre, "S1") == 0 || strcmp(semestre, "S3") == 0 || strcmp(semestre, "S5") == 0) {
        dates = dates1;
    } else if (strcmp(semestre, "S2") == 0 || strcmp(semestre, "S4") == 0) {
        dates = dates2;
    }

    int moduleIndex = 0;
    for (int i = 0; i < 6; ++i) {
        HPDF_REAL current_y = table_start_y - header_height - (i * row_height);

        // Fill first column with very light purple color
        HPDF_Page_SetRGBFill(page, 0.95, 0.9, 1.0);
        HPDF_Page_Rectangle(page, table_start_x, current_y, first_column_width, -row_height);
        HPDF_Page_FillStroke(page);

        // Draw the remaining row rectangles without fill
        HPDF_Page_SetRGBFill(page, 1.0, 1.0, 1.0);
        HPDF_Page_Rectangle(page, table_start_x + first_column_width, current_y, second_column_width, -row_height);
        HPDF_Page_Rectangle(page, table_start_x + first_column_width + second_column_width, current_y, third_column_width, -row_height);
        HPDF_Page_Stroke(page);

        // Add day and date to the first column
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);

        // Day name
        HPDF_REAL day_text_x = table_start_x + (first_column_width / 2) - (HPDF_Page_TextWidth(page, days[i]) / 2);
        HPDF_REAL day_text_y = current_y - 15;
        HPDF_Page_TextOut(page, day_text_x, day_text_y, days[i]);

        // Date
        HPDF_REAL date_text_x = table_start_x + (first_column_width / 2) - (HPDF_Page_TextWidth(page, dates[i]) / 2);
        HPDF_REAL date_text_y = current_y - 35;
        HPDF_Page_TextOut(page, date_text_x, date_text_y, dates[i]);

        HPDF_Page_EndText(page);

        // Insert module names alternately in the second and third columns
        for (int col = 0; col < 2; ++col) {
            if (moduleIndex < nbModulesFiltres) {
                HPDF_Page_BeginText(page);

                // Set the text color to a darker blue for the module name
                HPDF_Page_SetRGBFill(page, 0.32, 0.52, 0.75);  // Darker blue color

                // Use a bold font for the module name
                HPDF_Font boldFont = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
                HPDF_Page_SetFontAndSize(page, boldFont, 12);

                HPDF_REAL col_x = (col == 0)
                    ? table_start_x + first_column_width              // Start of second column
                    : table_start_x + first_column_width + second_column_width; // Start of third column;

                // Center module name horizontally
                HPDF_REAL text_width = HPDF_Page_TextWidth(page, modulesFiltres[moduleIndex].nom);
                HPDF_REAL text_x = col_x + ((col == 0 ? second_column_width : third_column_width) - text_width) / 2;

                // Adjust vertical position to move the text higher in the cell
                HPDF_REAL text_y = current_y - (row_height / 2) + 5;  // Adjusted (move up by increasing this value)

                HPDF_Page_TextOut(page, text_x, text_y, modulesFiltres[moduleIndex].nom);
                HPDF_Page_EndText(page);

                // Find and display the associated professor under the module name
                const char *professeur = NULL;
                for (int j = 0; j < nbProfs; j++) {
                    if (strcmp(professeurs[j].identifiant, modulesFiltres[moduleIndex].identifiant) == 0) {
                        professeur = professeurs[j].nom;
                        break;
                    }
                }

                if (professeur) {
                    HPDF_Page_BeginText(page);

                    // Use the italic font for the professor's name, and set the color to red
                    HPDF_Font italicFont = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
                    HPDF_Page_SetFontAndSize(page, italicFont, 10);  // Smaller size for the professor's name
                    HPDF_Page_SetRGBFill(page, 1.0, 0.0, 0.0);  // Red text

                    // Center professor name horizontally
                    text_width = HPDF_Page_TextWidth(page, professeur);
                    text_x = col_x + ((col == 0 ? second_column_width : third_column_width) - text_width) / 2;

                    // Adjust vertical position for the professor's name (move it higher)
                    HPDF_REAL professor_text_y = text_y - 10; // Move professor's name higher by adjusting this value

                    HPDF_Page_TextOut(page, text_x, professor_text_y, professeur);
                    HPDF_Page_EndText(page);
                }

                moduleIndex++;
            }
        }


    }
    // Step 7: Save the PDF to a file
    HPDF_SaveToFile(pdf, file_path);

    // Step 8: Free resources
    HPDF_Free(pdf);

    
}

void Admin() {      // Fonction qui génère le planning d'une seule filière
    Module modules[MAX_MODULES];
    Professeur professeurs[MAX_PROFS];
    Salle salles[MAX_SALLES];
    char filiere[100], semestre[100];
    int nbModules, nbProfs, nbSalles;

    int columns, lines;
    getConsoleSize(&columns, &lines);

    int titleX = (columns - 52) / 2;

    // Seed the random number generator to ensure different results each time
    srand(time(NULL));
    

   // Main logic starts here
    while (1) {
        displayTitle();
        gotoxy(titleX, 12);
        setConsoleColor(10,0);
        printf("Entrez la filiere (ID,GI,TDIA) : ");
        resetConsoleColor();
        scanf("%s", filiere);
        char filename[100];
        snprintf(filename, sizeof(filename), "Data/%s.txt", filiere);

        FILE *file = fopen(filename, "r");
        if (file) {
            fclose(file);
            break;  // File exists; exit this loop
        }else {
            gotoxy(titleX, 14);
            setConsoleColor(4, 0);        
            printf("Le fichier %s.txt n'existe pas. Veuillez reessayer.\n", filiere);
            resetConsoleColor();
            Sleep(1500);
            clearscreen();
        }
    }

    // Continue to load data
    char filename[100];
    snprintf(filename, sizeof(filename), "Data/%s.txt", filiere);
    nbModules = lireModules(filename, modules);
    nbProfs = lireProfesseurs("Data/Professeurs.txt", professeurs);
    nbSalles = lireSalles("Data/Salles.txt", salles);

    setConsoleColor(10,0);
    gotoxy(titleX, 15);
    printf("Entrez le semestre (S1, S2, S3,S4,S5) : ");
    resetConsoleColor();
    scanf("%s", semestre);

    while(strcmp(semestre, "S1") != 0 &&
        strcmp(semestre, "S2") != 0 &&
        strcmp(semestre, "S3") != 0 &&
        strcmp(semestre, "S4") != 0 &&
        strcmp(semestre, "S5") != 0) {
        // If semester is invalid, clear the line and prompt again
        clearLine(titleX, 15, 70);
        resetConsoleColor();  // Clear the semester input line
        setConsoleColor(10,0);
        gotoxy(titleX, 15);  // Move to the same line
        printf("Entrez le semestre (S1, S2, S3,S4,S5) : ");
        scanf("%s", semestre);
    }

    // Filtrer les modules selon la filière et le semestre
    Module modulesFiltres[MAX_MODULES];
    int nbModulesFiltres = 0;

    for (int i = 0; i < nbModules; i++) {
        if (strcmp(modules[i].filiere, filiere) == 0 && strcmp(modules[i].semestre, semestre) == 0) {
            modulesFiltres[nbModulesFiltres++] = modules[i];
        }
    }

    // Vérifier si des modules ont été trouvés
    if (nbModulesFiltres == 0) {
        printf("Aucun module trouvé pour la filière %s et le semestre %s.\n", filiere, semestre);
    }

    // Mélange des modules au hasard
    for (int i = nbModulesFiltres - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Module temp = modulesFiltres[i];
        modulesFiltres[i] = modulesFiltres[j];
        modulesFiltres[j] = temp;
    }

    for (int i = 0; i < nbModulesFiltres; i++) {
        const char *module = modulesFiltres[i].nom;
        const char *identifiant = modulesFiltres[i].identifiant;
        const char *professeur = NULL;

        // Associer un professeur au module
        for (int j = 0; j < nbProfs; j++) {
            if (strcmp(professeurs[j].identifiant, identifiant) == 0) {
                professeur = professeurs[j].nom;
                break;
            }
        }

        if (!professeur) {
            printf("Erreur : Aucun professeur trouve pour le module %s\n", module);
            continue;
        }
    }

    // Création de la source du PDF

    char file_path[100];
    snprintf(file_path, sizeof(file_path), "Planning/%s/%s.pdf", filiere,semestre);
    

    // Step 1: Create a new PDF document
    HPDF_Doc pdf = HPDF_New(error_handler, NULL);
    if (!pdf) {
        printf("Failed to create PDF object\n");
    }

    // Step 2: Add a page to the document
    HPDF_Page page = HPDF_AddPage(pdf);

    // Step 3: Set custom page size (297 mm x 210 mm, A4 Landscape)
    HPDF_Page_SetWidth(page, 841.89);  // 297 mm in points
    HPDF_Page_SetHeight(page, 595.28); // 210 mm in points

    // Step 4: Set font and write text to the page
    HPDF_Font font = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
    HPDF_Page_SetFontAndSize(page, font, 12);

    // Step 5: Draw the images (LogoEnsah and LogoUAE)
    HPDF_Image image1 = HPDF_LoadPngImageFromFile(pdf, "Resources/LogoEnsah.png");
    if (!image1) {
        printf("Failed to load LogoEnsah.png\n");
        HPDF_Free(pdf);
    }

    HPDF_Image image2 = HPDF_LoadPngImageFromFile(pdf, "Resources/LogoUAE.png");
    if (!image2) {
        printf("Failed to load LogoUAE.png\n");
        HPDF_Free(pdf);
    }

    HPDF_REAL resized_width = 75;
    HPDF_REAL resized_height = 75;

    HPDF_Page_DrawImage(page, image1, 50, 595.28 - resized_height - 20, resized_width, resized_height);
    HPDF_Page_DrawImage(page, image2, 841.89 - resized_width - 50, 595.28 - resized_height - 20, resized_width, resized_height);

    // Step 6: Add text between the two images, centered
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, font, 12); // Use the default font and size
    HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0); // Black text

    // Calculate the width of the text block
    const char* text = "Université Abdelmalek Essaâdi\nEcole Nationale des Sciences Appliquées\nAl Hoceima";
    HPDF_REAL text_width = HPDF_Page_TextWidth(page, text);

    // Center the text between the two images
    HPDF_REAL text_x = (350 + (841.89 - resized_width - 50) - text_width) / 2;

    // Set a position slightly below the logos
    HPDF_REAL text_y = 595.28 - resized_height + 20; // Slightly below the images

    // Write the text in multiple lines
    HPDF_Page_TextOut(page, text_x+20, text_y, "Universite Abdelmalek Essaadi");
    HPDF_Page_TextOut(page, text_x+20, text_y - 15, "Ecole Nationale des Sciences Appliquees");
    HPDF_Page_TextOut(page, text_x+20, text_y - 30, "Al Hoceima");

    HPDF_Page_EndText(page);
    

    // Step 6: Draw the table
    HPDF_REAL table_start_x = 100;
    HPDF_REAL table_start_y = 350;
    HPDF_REAL first_column_width = 80;
    HPDF_REAL second_column_width = 290;
    HPDF_REAL third_column_width = 290;
    HPDF_REAL header_height = 20;
    HPDF_REAL row_height = 50;

    // Step 7: Draw the bold blue line above the table
    HPDF_Page_SetRGBFill(page, 0.0, 0.32, 0.75); // Dark blue color
    HPDF_Page_SetLineWidth(page, 2);  // Set line width to make it bold
    HPDF_Page_MoveTo(page, table_start_x, table_start_y + 50);  // Starting point of the line
    HPDF_Page_LineTo(page, table_start_x + first_column_width + second_column_width + third_column_width, table_start_y + 50); // Ending point of the line
    HPDF_Page_Stroke(page);

    // Step 7: Write the labels above the line
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, font, 12); // Use the default font and size

    // Write "Filiere" on the left side of the line
    HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0); // Black text
    HPDF_REAL filiere_text_x = table_start_x ; // Position the text a little left of the line
    HPDF_REAL filiere_text_y = table_start_y + 55; // Position text slightly below the line
    HPDF_Page_TextOut(page, filiere_text_x, filiere_text_y, "Filiere");

    // Write "Semestre" in the center of the line
    HPDF_REAL semestre_text_x = table_start_x + (first_column_width + second_column_width + third_column_width) / 2 - (HPDF_Page_TextWidth(page, "Semestre") / 2);
    HPDF_REAL semestre_text_y = table_start_y + 55;
    HPDF_Page_TextOut(page, semestre_text_x, semestre_text_y, "Semestre");

    // Write "Salle" on the right side of the line
    HPDF_REAL salle_text_x = table_start_x + first_column_width + second_column_width + third_column_width - HPDF_Page_TextWidth(page, "Salle") - 20;
    HPDF_REAL salle_text_y = table_start_y + 55;
    HPDF_Page_TextOut(page, salle_text_x, salle_text_y, "Salle");
    HPDF_Page_EndText(page);

    // Step 8: Write the selected Filiere, Semestre, and Salle under their respective labels
    HPDF_Page_BeginText(page);
    HPDF_Page_SetFontAndSize(page, font, 12); // Use the default font and size

    // Write the chosen filiere
    HPDF_REAL filiere_value_x = table_start_x ; // Same X as "Filiere"
    HPDF_REAL filiere_value_y = table_start_y + 35; // Place it below "Filiere" label
    HPDF_Page_TextOut(page, filiere_value_x, filiere_value_y, filiere);

    // Write the chosen semestre
    HPDF_REAL semestre_value_x = table_start_x + (first_column_width + second_column_width + third_column_width) / 2 - (HPDF_Page_TextWidth(page, semestre) / 2);
    HPDF_REAL semestre_value_y = table_start_y + 35; // Place it below "Semestre" label
    HPDF_Page_TextOut(page, semestre_value_x, semestre_value_y, semestre);

    // Write the chosen salle
    const char *salle = salles[rand() % nbSalles].nom; // Random salle
    HPDF_REAL salle_value_x = table_start_x + first_column_width + second_column_width + third_column_width - HPDF_Page_TextWidth(page, salle) - 20;
    HPDF_REAL salle_value_y = table_start_y + 35; // Place it below "Salle" label
    HPDF_Page_TextOut(page, salle_value_x, salle_value_y, salle);
    HPDF_Page_EndText(page);

    // Draw header row with very light purple fill
    HPDF_Page_SetRGBFill(page, 0.95, 0.9, 1.0);
    HPDF_Page_Rectangle(page, table_start_x, table_start_y, first_column_width, -header_height);
    HPDF_Page_Rectangle(page, table_start_x + first_column_width, table_start_y, second_column_width, -header_height);
    HPDF_Page_Rectangle(page, table_start_x + first_column_width + second_column_width, table_start_y, third_column_width, -header_height);
    HPDF_Page_FillStroke(page);

    // Add header text in black
    HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);
    HPDF_Page_BeginText(page);

    HPDF_REAL date_text_x = table_start_x + (first_column_width / 2) - (HPDF_Page_TextWidth(page, "Date") / 2);
    HPDF_REAL date_text_y = table_start_y - header_height / 2 - 5;
    HPDF_Page_TextOut(page, date_text_x, date_text_y, "Date");

    HPDF_Page_TextOut(page, table_start_x + first_column_width + 10, table_start_y - header_height + 5, "9:00");
    HPDF_Page_TextOut(page, table_start_x + first_column_width + second_column_width - 40, table_start_y - header_height + 5, "11:00");
    HPDF_Page_TextOut(page, table_start_x + first_column_width + second_column_width + 10, table_start_y - header_height + 5, "15:00");
    HPDF_Page_TextOut(page, table_start_x + first_column_width + second_column_width + third_column_width - 40, table_start_y - header_height + 5, "17:00");

    HPDF_Page_EndText(page);

    // Draw the remaining rows
    const char* days[] = {"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};
    const char* dates1[] = {"30/12/2024", "31/12/2024", "01/01/2025", "02/01/2025", "03/01/2025", "04/01/2025"};
    const char* dates2[] = {"26/05/2025", "27/05/2025", "28/05/2025", "29/05/2025", "30/05/2025", "31/05/2025"};
    const char** dates = NULL; // Pointer to an array of const char*
    if (strcmp(semestre, "S1") == 0 || strcmp(semestre, "S3") == 0 || strcmp(semestre, "S5") == 0) {
        dates = dates1;
    } else if (strcmp(semestre, "S2") == 0 || strcmp(semestre, "S4") == 0) {
        dates = dates2;
    }

    int moduleIndex = 0;
    for (int i = 0; i < 6; ++i) {
        HPDF_REAL current_y = table_start_y - header_height - (i * row_height);

        // Fill first column with very light purple color
        HPDF_Page_SetRGBFill(page, 0.95, 0.9, 1.0);
        HPDF_Page_Rectangle(page, table_start_x, current_y, first_column_width, -row_height);
        HPDF_Page_FillStroke(page);

        // Draw the remaining row rectangles without fill
        HPDF_Page_SetRGBFill(page, 1.0, 1.0, 1.0);
        HPDF_Page_Rectangle(page, table_start_x + first_column_width, current_y, second_column_width, -row_height);
        HPDF_Page_Rectangle(page, table_start_x + first_column_width + second_column_width, current_y, third_column_width, -row_height);
        HPDF_Page_Stroke(page);

        // Add day and date to the first column
        HPDF_Page_BeginText(page);
        HPDF_Page_SetRGBFill(page, 0.0, 0.0, 0.0);

        // Day name
        HPDF_REAL day_text_x = table_start_x + (first_column_width / 2) - (HPDF_Page_TextWidth(page, days[i]) / 2);
        HPDF_REAL day_text_y = current_y - 15;
        HPDF_Page_TextOut(page, day_text_x, day_text_y, days[i]);

        // Date
        HPDF_REAL date_text_x = table_start_x + (first_column_width / 2) - (HPDF_Page_TextWidth(page, dates[i]) / 2);
        HPDF_REAL date_text_y = current_y - 35;
        HPDF_Page_TextOut(page, date_text_x, date_text_y, dates[i]);

        HPDF_Page_EndText(page);

        // Insert module names alternately in the second and third columns
        for (int col = 0; col < 2; ++col) {
            if (moduleIndex < nbModulesFiltres) {
                HPDF_Page_BeginText(page);

                // Set the text color to a darker blue for the module name
                HPDF_Page_SetRGBFill(page, 0.32, 0.52, 0.75);  // Darker blue color

                // Use a bold font for the module name
                HPDF_Font boldFont = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
                HPDF_Page_SetFontAndSize(page, boldFont, 12);

                HPDF_REAL col_x = (col == 0)
                    ? table_start_x + first_column_width              // Start of second column
                    : table_start_x + first_column_width + second_column_width; // Start of third column;

                // Center module name horizontally
                HPDF_REAL text_width = HPDF_Page_TextWidth(page, modulesFiltres[moduleIndex].nom);
                HPDF_REAL text_x = col_x + ((col == 0 ? second_column_width : third_column_width) - text_width) / 2;

                // Adjust vertical position to move the text higher in the cell
                HPDF_REAL text_y = current_y - (row_height / 2) + 5;  // Adjusted (move up by increasing this value)

                HPDF_Page_TextOut(page, text_x, text_y, modulesFiltres[moduleIndex].nom);
                HPDF_Page_EndText(page);

                // Find and display the associated professor under the module name
                const char *professeur = NULL;
                for (int j = 0; j < nbProfs; j++) {
                    if (strcmp(professeurs[j].identifiant, modulesFiltres[moduleIndex].identifiant) == 0) {
                        professeur = professeurs[j].nom;
                        break;
                    }
                }

                if (professeur) {
                    HPDF_Page_BeginText(page);

                    // Use the italic font for the professor's name, and set the color to red
                    HPDF_Font italicFont = HPDF_GetFont(pdf, "Helvetica-Bold", NULL);
                    HPDF_Page_SetFontAndSize(page, italicFont, 10);  // Smaller size for the professor's name
                    HPDF_Page_SetRGBFill(page, 1.0, 0.0, 0.0);  // Red text

                    // Center professor name horizontally
                    text_width = HPDF_Page_TextWidth(page, professeur);
                    text_x = col_x + ((col == 0 ? second_column_width : third_column_width) - text_width) / 2;

                    // Adjust vertical position for the professor's name (move it higher)
                    HPDF_REAL professor_text_y = text_y - 10; // Move professor's name higher by adjusting this value

                    HPDF_Page_TextOut(page, text_x, professor_text_y, professeur);
                    HPDF_Page_EndText(page);
                }

                moduleIndex++;
            }
        }


    }
    // Step 7: Save the PDF to a file
    HPDF_SaveToFile(pdf, file_path);

    // Step 8: Free resources
    HPDF_Free(pdf);


    gotoxy(titleX,20);
    setConsoleColor(4,0);
    printf("PDF created successfully: PDFtest.pdf\n");
    Sleep(500);
    resetConsoleColor();
    
}

void Admin2() {     // Fonction qui donne le choix de session
    int choix;
    const char* filiere[] = {"GI","ID","TDIA"};
    const char* semestreAutomne[] = {"S1", "S3", "S5"};
    const char* semestrePrintemps[] = {"S2", "S4"};
    int sizeFiliere = sizeof(filiere) / sizeof(filiere[0]);
    int sizeSemestreAutomne = sizeof(semestreAutomne) / sizeof(semestreAutomne[0]);
    int sizeSemestrePrintemps = sizeof(semestrePrintemps) / sizeof(semestrePrintemps[0]);

    do {
        system("cls");
        displayTitle();

        SetConsoleOutputCP(CP_UTF8);  // Optionnel : Configurer en UTF-8 pour meilleure compatibilité

        int columns, lines;
        getConsoleSize(&columns, &lines);

        int titleX = (columns - 52) / 2;
        gotoxy(titleX, 12);
        setConsoleColor(10, 0); // Green text
        printf("1. Session Automne");
        gotoxy(titleX, 15);
        printf("2. Session printemps");
        resetConsoleColor();
        gotoxy(titleX, 18);
        setConsoleColor(14, 0); // Yellow text
        printf("Votre choix: ");
        resetConsoleColor();
        scanf("%d",&choix);
        

        if (choix != 1 && choix != 2) {
            gotoxy(titleX, 20);
            setConsoleColor(12, 0); // Red text
            printf("Choix invalide. Veuillez choisir 1 ou 2.");
            resetConsoleColor();
            Sleep(2000); 
        }
    } while (choix != 1 && choix != 2);

    int i, j;
    if (choix == 1) {
        for (i = 0; i < sizeFiliere; i++) {
            for (j = 0; j < sizeSemestreAutomne; j++) {
                AdminTout(filiere[i], semestreAutomne[j]);
            }
        }
    } else {
        for (i = 0; i < sizeFiliere; i++) {
            for (j = 0; j < sizeSemestrePrintemps; j++) {
                AdminTout(filiere[i], semestrePrintemps[j]);
            }
        }
    }
}

void Etudiant() {       // Fonction d'espace étudiant
    int columns, lines;
    getConsoleSize(&columns, &lines);

    int titleX = (columns - 52) / 2;

    char filiere[100];
    char semestre[100];

    while (1) {
        displayTitle();
        setConsoleColor(10,0);
        gotoxy(titleX, 12);
        printf("Entrer la filière : ");
        resetConsoleColor();
        scanf("%s", filiere); // Pas d'espace attendu dans le nom

        setConsoleColor(10,0);
        gotoxy(titleX, 15);
        printf("Entrer le semestre : ");
        resetConsoleColor();
        scanf("%s", semestre);

        // Construct the file path
        char file_path[100];
        snprintf(file_path, sizeof(file_path), "Planning/%s/%s.pdf", filiere, semestre);

        // Check if the file exists
        FILE *file = fopen(file_path, "r");
        if (!file) {
            gotoxy(titleX, 20);
            setConsoleColor(4, 0);        
            printf("Ce planning n'existe pas. Veuillez réessayer.\n"); // Correction de l'orthographe
            resetConsoleColor();
            Sleep(1500);
            clearscreen();
            continue; // Recommencer la boucle
        }
        fclose(file); // Fermer correctement le fichier si ouvert
        break; // Sortir de la boucle si le fichier existe
    }

    // Ouvrir le fichier après validation
    char file_path[100];
    snprintf(file_path, sizeof(file_path), "Planning/%s/%s.pdf", filiere, semestre);

    gotoxy(titleX, 20);
    setConsoleColor(4, 0);
    printf("Ouverture de votre fichier...\n"); // Correction de l'orthographe
    resetConsoleColor(); // Réinitialisation de la couleur
    Sleep(2000);   

    char command[256];
#ifdef _WIN32
    snprintf(command, sizeof(command), "start %s", file_path); // Commande pour Windows
#elif __APPLE__
    snprintf(command, sizeof(command), "open %s", file_path); // Commande pour macOS
#elif __linux__
    snprintf(command, sizeof(command), "xdg-open %s", file_path); // Commande pour Linux
#else
    printf("Plateforme non prise en charge : impossible d'ouvrir le fichier.\n");
    return;
#endif
    system(command); // Exécution de la commande
}


int main() {
    SetConsoleOutputCP(CP_UTF8);  // Optionnel : Configurer en UTF-8 pour meilleure compatibilité

    int columns, lines;
    getConsoleSize(&columns, &lines);
    

    int titleX = (columns - 52) / 2;
    

    while (1) {
        
        system("cls");  // Effacer la console à chaque boucle pour éviter des affichages indésirables
        int choix = 0;
        displayHomePage(titleX, &choix);  // Affiche la page d'accueil

        if (choix == 1) {
            setConsoleColor(10, 0);  // Texte vert
            gotoxy(titleX, 21);
            printf("Chargement de l'interface Admin...");
            resetConsoleColor();
            Sleep(1000);  // Délai pour une meilleure expérience utilisateur
            system("cls");  // Effacer la console après le message
            LoginAdmin();
            system("cls");
            int choix2;
            displayMenu2(&choix2);
            if (choix2 == 1) {     // Si Inscription a été choisi
                Admin2();
                continue;       // Retourner à la page d'accueil
            } else {
                system("cls");  // Effacer la console après le message
                Admin();  // Appel à la fonction Admin
            }
        } else if (choix == 2) {
            setConsoleColor(10, 0);  // Texte vert
            gotoxy(titleX, 21);
            printf("Chargement de l'interface Étudiant...");
            Sleep(1000);   // Délai pour une meilleure expérience utilisateur
            resetConsoleColor();
            system("cls");

            int loginresult;
            Login(&loginresult);
            if (loginresult == 1) {     // Si Inscription a été choisi
                continue;               // Retourner à la page d'accueil
            } else {
                resetConsoleColor();
                system("cls");  // Effacer la console après le message
                Etudiant();  // Appel à la fonction Etudiant
            }
            
        }
        else{
            // Entrée invalide
            gotoxy(titleX, 14);
            setConsoleColor(12, 0);  // Texte rouge
            printf("Entrée invalide. Veuillez réessayer.");
            resetConsoleColor();
            Sleep(1500);
        }
        // Vous pouvez ajouter une option de quitter ici si nécessaire
    }

    return 0;
}
