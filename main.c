#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const int ALPHABET_SIZE=26;
const int TOP_N=3;
const char filename[]="alfabet.in";


void read_distribution(const char *filename, double distribution[ALPHABET_SIZE]);
void compute_histogram(const char *text, double histogram[ALPHABET_SIZE]);
double chi_squared_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]);
double euclidean_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]);
double cosine_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]);
void encrypt(unsigned char s[], int n);
void decrypt(unsigned char s[], int n);
void break_caesar_cipher(const char* text, int top_shifts[TOP_N], double top_distances[TOP_N],
                         double (*distance_function)(const double[], const double[]),
                         const double target_distribution[ALPHABET_SIZE]);
void print_menu();
int take_option();

int main(void) {
    double alfabet[ALPHABET_SIZE];
    char text[1000]="";
    char txt[1000];
    int top_shifts[TOP_N];
    double top_distances[TOP_N];
    double histogram[ALPHABET_SIZE];
    int n, option_distance;
    read_distribution(filename,alfabet);
    print_menu();
    int option=take_option();
    while (option != 7) {
        switch (option) {
            case 1:
                getchar();
                fgets(text, sizeof(text), stdin);
                break;

            case 2:
                char file[100];
                printf("Enter the filename: ");
                scanf("%s", file);
                FILE *fp = fopen(file, "r");

                if (fp == NULL) {
                    printf("Eroare la deschiderea fisierului %s", file);
                    break;
                }

                fgets(text, sizeof(text), fp);
                printf("%s", text);
                fclose(fp);
                break;

            case 3:
                if (strcmp(text, "")==0)
                    printf("The string is empty. Use the no. 1 and 2 option to fill it.\n");
                else {
                    printf("Provide the shift value: ");
                    scanf("%d", &n);
                    printf("\n");
                    encrypt(text, n);
                    printf("%s", text);
                }
                break;
            case 4:
                getchar();
                printf("Enter the text you want to be decripted: ");
                fgets(txt, sizeof(txt), stdin);
                printf("With how many spaces was the text shifted? ");
                scanf("%d", &n);
                n=n%26;
                decrypt(txt, n);
                printf("%s", txt);
                break;
            case 5:
                if (strcmp(text, "")==0)
                    printf("The string is empty. Use the no. 1 and 2 option to fill it.\n");
                else {
                    compute_histogram(text, histogram);
                    for (int i = 0; i < ALPHABET_SIZE; i++) {
                        printf("%c: %.2f%%\n", (char)(i+97), histogram[i]);
                    }
                }
                break;
            case 6:
                printf("Choose your option to calculate distance: \n");
                printf(" 1.chi-squared distance\n 2.Cosine distance\n 3.Euclidian distance\n");
                printf("Your option is: ");
                scanf("%d", &option_distance);
                if (option_distance==1) {
                    break_caesar_cipher(text, top_shifts, top_distances, chi_squared_distance, alfabet);
                }
                else if (option_distance==2) {
                    break_caesar_cipher(text, top_shifts, top_distances, cosine_distance, alfabet);
                }
                else if (option_distance==3) {
                    break_caesar_cipher(text, top_shifts, top_distances, euclidean_distance, alfabet);
                }
                else {
                    printf("Optiune invalida.");
                    break;
                }
                decrypt(text, top_shifts[0]);
                printf("%s", text);

                break;

            default:
                printf("Invalid option. Please enter another option.");
                break;
        }
        printf("\n");
        print_menu();
        option = take_option();
    }

    return 0;
}

void print_menu() {
    printf("1.Reading a text from the keyboard\n");
    printf("2.Reading a text from a file\n");
    printf("3.Encrypting a text with a specified shif\n");
    printf("4.Decrypting a text with a known shift\n");
    printf("5.Computing and displaying the frequency distribution of a text\n");
    printf("6.Breaking the encrypted text using frequency analysis\n");
    printf("7.Exit\n");
}

int take_option() {
    int n;
    printf("Your option is: ");
    scanf("%d", &n);
    printf("\n");
    return n;
}

void read_distribution(const char *filename, double distribution[ALPHABET_SIZE]) {
    FILE *fp = fopen(filename, "r");
    char linie[256];

    if (fp == NULL) {
        perror("Eroare la deschiderea fisierului");
    }

    int i=0;
    while (fgets(linie, sizeof(linie), fp) != NULL) {
        distribution[i] = atof(linie);
        i++;
    }

    fclose(fp);
}

void compute_histogram(const char *text, double histogram[ALPHABET_SIZE]) {
    int len = strlen(text);
    int litere=0;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        histogram[i] = 0.0;
    }

    for (int i = 0; i < len; i++) {
        char c = toupper((unsigned char)text[i]);

        if (c >= 'A' && c <= 'Z') {
            histogram[c - 'A']++;
            litere++;
        }
    }

    if (len > 0) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            histogram[i] =100.00*histogram[i]/litere;
        }
    }
}

double chi_squared_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double chi = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        chi += (hist1[i] - hist2[i])*(hist1[i] - hist2[i])/hist2[i];
    }
    return chi;
}

double euclidean_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double euclidean = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        euclidean+=(hist1[i] - hist2[i])*(hist1[i]-hist2[i]);
    }
    euclidean = sqrt(euclidean);
    return euclidean;
}

double cosine_distance(const double hist1[ALPHABET_SIZE], const double hist2[ALPHABET_SIZE]) {
    double cosine = 1.0, sum_hist1=0.0, sum_hist2=0.0, sum_hists=0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        sum_hist1 += hist1[i]*hist1[i];
        sum_hist2 += hist2[i]*hist2[i];
        sum_hists += hist1[i]*hist2[i];
    }
    sum_hist1 = sqrt(sum_hist1);
    sum_hist2 = sqrt(sum_hist2);
    cosine -= sum_hists/(sum_hist1*sum_hist2);
    return cosine;
}

void encrypt(unsigned char s[], int n) {
    int l = strlen((char*)s);
    n = (n % 26 + 26) % 26;
    for (int i = 0; i < l; i++) {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = 'a' + (s[i] - 'a' + n) % 26;
        else if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] = 'A' + (s[i] - 'A' + n) % 26;
    }
}

void decrypt(unsigned char s[], int n) {
    encrypt(s, 26 - (n % 26));
}

void break_caesar_cipher(const char* text, int top_shifts[TOP_N], double top_distances[TOP_N],
                         double (*distance_function)(const double[], const double[]),
                         const double target_distribution[ALPHABET_SIZE]) {

    double current_histogram[ALPHABET_SIZE];
    char temp_text[1000];

    for (int i = 0; i < TOP_N; i++) {
        top_distances[i] = 1e9;
    }

    for (int s = 0; s < 26; s++) {
        strcpy(temp_text, text);

        decrypt((unsigned char*)temp_text, s);

        compute_histogram(temp_text, current_histogram);

        double d = distance_function(current_histogram, target_distribution);

        for (int i = 0; i < TOP_N; i++) {
            if (d < top_distances[i]) {
                for (int j = TOP_N - 1; j > i; j--) {
                    top_distances[j] = top_distances[j-1];
                    top_shifts[j] = top_shifts[j-1];
                }
                top_distances[i] = d;
                top_shifts[i] = s;
                i=TOP_N;
            }
        }
    }
}



