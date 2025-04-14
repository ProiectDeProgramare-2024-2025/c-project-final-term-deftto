#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROPERTIES 100
#define MAX_FAVORITES 100

// ANSI color codes pentru evidențiere
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

// Structură pentru un imobil
typedef struct {
    int id;
    char titlu[50];         // Titlu
    char tip[20];           // tip imobil
    float pret;
    float dimensiune;       // metri 2
    int nrCamere;
    int nrBai;
    char parcare[50];       //  numele străzii
    int esteFavorite;       // 0 = nu, 1 = da
    int achizitionat;       // 0 = disponibil, 1 = achiziționat
    char dataAchizitie[11]; // format "zz/ll/aaaa". Dacă nu e achiziționat, rămâne ""
} Property;

// Funcție pentru validarea formatului datei (simplu, folosind sscanf)
int validateDate(const char* date) {
    int day, month, year;
    if (sscanf(date, "%2d/%2d/%4d", &day, &month, &year) != 3) {
        return 0;
    }
    // Validări de bază: zi între 1 și 31, lună între 1 și 12
    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900)
        return 0;
    return 1;
}

// Funcție pentru citirea unui șir de caractere (salvând eventual '\n')
void readString(const char* prompt, char* buffer, int size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin)) {
        // Elimina caracterul newline, dacă există
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
    }
}

// Funcție pentru citirea datelor din fișierul de imobile
int readProperties(Property properties[], int *count, const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        // Nu a reușit deschiderea fișierului, poate fi prima rulare
        *count = 0;
        return 0;
    }
    int cnt = 0;
    while (cnt < MAX_PROPERTIES && fscanf(fp, "%d\n", &properties[cnt].id) == 1) {
        fgets(properties[cnt].titlu, sizeof(properties[cnt].titlu), fp);
        properties[cnt].titlu[strcspn(properties[cnt].titlu, "\n")] = '\0';
        fgets(properties[cnt].tip, sizeof(properties[cnt].tip), fp);
        properties[cnt].tip[strcspn(properties[cnt].tip, "\n")] = '\0';
        fscanf(fp, "%f\n", &properties[cnt].pret);
        fscanf(fp, "%f\n", &properties[cnt].dimensiune);
        fscanf(fp, "%d\n", &properties[cnt].nrCamere);
        fscanf(fp, "%d\n", &properties[cnt].nrBai);
        fgets(properties[cnt].parcare, sizeof(properties[cnt].parcare), fp);
        properties[cnt].parcare[strcspn(properties[cnt].parcare, "\n")] = '\0';
        fscanf(fp, "%d\n", &properties[cnt].esteFavorite);
        fscanf(fp, "%d\n", &properties[cnt].achizitionat);
        fgets(properties[cnt].dataAchizitie, sizeof(properties[cnt].dataAchizitie), fp);
        properties[cnt].dataAchizitie[strcspn(properties[cnt].dataAchizitie, "\n")] = '\0';
        cnt++;
    }
    *count = cnt;
    fclose(fp);
    return 1;
}

// Funcție pentru scrierea datelor în fișierul de imobile
int writeProperties(Property properties[], int count, const char* filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Eroare la deschiderea fișierului pentru scriere!\n");
        return 0;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d\n", properties[i].id);
        fprintf(fp, "%s\n", properties[i].titlu);
        fprintf(fp, "%s\n", properties[i].tip);
        fprintf(fp, "%.2f\n", properties[i].pret);
        fprintf(fp, "%.2f\n", properties[i].dimensiune);
        fprintf(fp, "%d\n", properties[i].nrCamere);
        fprintf(fp, "%d\n", properties[i].nrBai);
        fprintf(fp, "%s\n", properties[i].parcare);
        fprintf(fp, "%d\n", properties[i].esteFavorite);
        fprintf(fp, "%d\n", properties[i].achizitionat);
        fprintf(fp, "%s\n", properties[i].dataAchizitie);
    }
    fclose(fp);
    return 1;
}

// Funcție pentru afișarea listei de imobile (doar cele disponibile)
void listProperties(Property properties[], int count) {
    printf("\n=== Lista Imobilelor Disponibile ===\n");
    for (int i = 0; i < count; i++) {
        if (properties[i].achizitionat == 0) {
            // Evidențiem titlul imobilului în culoare (GREEN)
            printf("ID: %d | Titlu: " GREEN "%s" RESET " | Tip: %s | Pret: " YELLOW "%.2f" RESET "\n",
                   properties[i].id, properties[i].titlu, properties[i].tip, properties[i].pret);
        }
    }
}

// Funcție pentru afișarea detaliilor unui imobil (cu evidențiere a elementelor importante)
void viewProperty(Property prop) {
    printf("\n--- Detalii imobil ---\n");
    printf("ID: %d\n", prop.id);
    printf("Titlu: " GREEN "%s" RESET "\n", prop.titlu);
    printf("Tip: %s\n", prop.tip);
    printf("Pret: " YELLOW "%.2f" RESET "\n", prop.pret);
    printf("Dimensiune: %.2f m^2\n", prop.dimensiune);
    printf("Numar camere: %d\n", prop.nrCamere);
    printf("Numar bai: %d\n", prop.nrBai);
    // Evidențiem numele străzii/parcării în albastru
    printf("Loc parcare: " BLUE "%s" RESET "\n", prop.parcare);
    if (prop.achizitionat) {
        printf("Achizitionat la data: %s\n", prop.dataAchizitie);
    }
    printf("Favorite: %s\n", prop.esteFavorite ? "Da" : "Nu");
}

// Funcție pentru adăugarea unui imobil nou în listă
void addProperty(Property properties[], int *count) {
    if (*count >= MAX_PROPERTIES) {
        printf("Nu se mai pot adauga imobile!\n");
        return;
    }
    Property p;
    p.id = (*count == 0 ? 1 : properties[*count - 1].id + 1);

    readString("Introduceti titlul imobilului: ", p.titlu, sizeof(p.titlu));
    readString("Introduceti tipul imobilului (casa, apartament, etc.): ", p.tip, sizeof(p.tip));

    printf("Introduceti pretul imobilului: ");
    scanf("%f", &p.pret);
    printf("Introduceti dimensiunea (in m^2): ");
    scanf("%f", &p.dimensiune);
    printf("Introduceti numarul de camere: ");
    scanf("%d", &p.nrCamere);
    printf("Introduceti numarul de bai: ");
    scanf("%d", &p.nrBai);
    getchar(); // Consum newline-ul

    readString("Introduceti locul de parcare (numele strazii/adresa): ", p.parcare, sizeof(p.parcare));

    p.esteFavorite = 0;
    p.achizitionat = 0;
    strcpy(p.dataAchizitie, "");

    properties[*count] = p;
    (*count)++;

    printf("Imobilul a fost adaugat cu succes!\n");
}

// Funcție pentru adăugarea unui imobil la favorite
void addToFavorites(Property properties[], int count, int id) {
    for (int i = 0; i < count; i++) {
        if (properties[i].id == id && properties[i].achizitionat == 0) {
            properties[i].esteFavorite = 1;
            printf("Imobilul '%s' a fost adaugat la favorite.\n", properties[i].titlu);
            return;
        }
    }
    printf("Imobilul cu ID-ul %d nu a fost gasit sau nu este disponibil.\n", id);
}

// Funcție pentru achiziționarea unui imobil (introducerea datei de achiziție)
void purchaseProperty(Property properties[], int *count, int id) {
    for (int i = 0; i < *count; i++) {
        if (properties[i].id == id && properties[i].achizitionat == 0) {
            // Solicităm data de achiziție, oferind un exemplu cu formatul corect
            char data[20];
            int valid = 0;
            do {
                // Mesajul explicit, conform cerinței
                readString("✅ Data achizitionarii (zz/ll/aaaa): ", data, sizeof(data));
                if (validateDate(data)) {
                    valid = 1;
                } else {
                    printf("Format invalid! Va rugam sa introduceti data in formatul zz/ll/aaaa.\n");
                }
            } while (!valid);

            strcpy(properties[i].dataAchizitie, data);
            properties[i].achizitionat = 1;
            printf("Felicitari! Ati achizitionat imobilul '%s'.\n", properties[i].titlu);

            // Eliminăm imobilul din lista de imobile disponibile prin mutarea ultimei intrări în pozitia curentă
            // (Aceasta este o metodă simplă de ștergere dintr-un array nesortat)
            properties[i] = properties[(*count) - 1];
            (*count)--;
            return;
        }
    }
    printf("Imobilul cu ID-ul %d nu a fost gasit sau nu este disponibil pentru achizitie.\n", id);
}

// Funcție pentru afișarea listei de imobile favorite
void listFavorites(Property properties[], int count) {
    printf("\n=== Lista Imobilelor Favorite ===\n");
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (properties[i].esteFavorite && properties[i].achizitionat == 0) {
            printf("ID: %d | Titlu: " GREEN "%s" RESET " | Pret: " YELLOW "%.2f" RESET "\n",
                   properties[i].id, properties[i].titlu, properties[i].pret);
            found = 1;
        }
    }
    if (!found) {
        printf("Nu aveti imobile favorite.\n");
    }
}

// Meniul principal al aplicației
int main() {
    Property properties[MAX_PROPERTIES];
    int count = 0;
    const char* filename = "properties.txt";

    // Citire din fișier
    readProperties(properties, &count, filename);

    int optiune;
    do {
        printf("\n========================\n");
        printf("1. Adauga imobil\n");
        printf("2. Afiseaza imobile disponibile\n");
        printf("3. Vizualizeaza detalii imobil\n");
        printf("4. Adauga imobil la favorite\n");
        printf("5. Achizitioneaza imobil\n");
        printf("6. Afiseaza imobile favorite\n");
        printf("0. Iesire\n");
        printf("========================\n");
        printf("Selectati o optiune: ");
        scanf("%d", &optiune);
        getchar();  // Consum newline-ul

        int id;
        switch (optiune) {
            case 1:
                addProperty(properties, &count);
                break;
            case 2:
                listProperties(properties, count);
                break;
            case 3:
                printf("Introduceti ID-ul imobilului pentru vizualizare: ");
                scanf("%d", &id);
                getchar();
                {
                    int found = 0;
                    for (int i = 0; i < count; i++) {
                        if (properties[i].id == id) {
                            viewProperty(properties[i]);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        printf("Imobilul cu ID-ul %d nu a fost gasit.\n", id);
                    }
                }
                break;
            case 4:
                printf("Introduceti ID-ul imobilului pe care doriti sa il adaugati la favorite: ");
                scanf("%d", &id);
                getchar();
                addToFavorites(properties, count, id);
                break;
            case 5:
                printf("Introduceti ID-ul imobilului pe care doriti sa il achizitionati: ");
                scanf("%d", &id);
                getchar();
                purchaseProperty(properties, &count, id);
                break;
            case 6:
                listFavorites(properties, count);
                break;
            case 0:
                printf("Iesire din aplicatie...\n");
                break;
            default:
                printf("Optiune invalida! Incercati din nou.\n");
        }

    } while (optiune != 0);

    // Salvare in fișier la iesire
    writeProperties(properties, count, filename);

    return 0;
}
