#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "autocomplete.h"


int compare_forQSORTterms(const void *a, const void *b) {
    const term *termA = (const term *)a;
    const term *termB = (const term *)b;
    return strcmp(termA->term, termB->term);  
}

void read_in_terms(term **terms, int *pnterms, char *filename) {
    char linesBuffer[200];
    int in = 0;

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("File not found error\n");
        *pnterms = 0;
        return;
    }

    // read first line to get the number of terms
    if (!fgets(linesBuffer, sizeof(linesBuffer), fp)) {
        printf("Error reading file\n");
        fclose(fp);
        *pnterms = 0;
        return;
    }

    //this is beacuse the size of the file is an integer at the beginning

    int total_terms = atoi(linesBuffer);

    // Allocate memory for all terms
    *terms = (term *)malloc(sizeof(term) * total_terms);

    while (fgets(linesBuffer, sizeof(linesBuffer), fp) && in < total_terms) {
        double num;
        char str[200];

        if (sscanf(linesBuffer, "%lf%*c%199[^\n]", &num, str) == 2) {
            (*terms)[in].weight = num;
            strcpy((*terms)[in].term, str);
            in++;
        }
    }
    

    fclose(fp);
    *pnterms = in; // Store the number of terms read

    qsort(*terms, *pnterms, sizeof(term), compare_forQSORTterms);

    
}


int main() {
    term *terms = NULL;
    int num_terms;
    char *file = "cities.txt";

    read_in_terms(&terms, &num_terms, file);

    if (num_terms > 0) {
        printf("First term: %lf - %s\n", terms[10].weight, terms[10].term);
    } else {
        printf("No terms read.\n");
    }

    free(terms); 

    return 0;
}
