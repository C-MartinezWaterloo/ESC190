#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "autocomplete.h"


int compare_forQSORTterms(const void *a, const void *b) {
    const term *Firstterm = (const term *)a;
    const term *Secondterm = (const term *)b;
    return strcmp(Firstterm->term, Secondterm->term);  
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


int starts_with(const char *term, const char *substr) {
    return strncmp(term, substr, strlen(substr)) == 0;
}

int lowest_match(term *terms, int nterms, char *substr) {
    int left = 0;
    int right = nterms - 1;
    int result = -1;  

    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        
        if (starts_with(terms[mid].term, substr)) {
            result = mid; 
            right = mid - 1;
        } else if (strcmp(terms[mid].term, substr) < 0) {
            left = mid + 1; 
        } else {
            right = mid - 1; 
        }
    }
    
    return result;
}

int highest_match(term *terms, int nterms, char *substr) {
    int left = 0;
    int right = nterms - 1;
    int result = -1;  

    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        
        if (starts_with(terms[mid].term, substr)) {
            result = mid; 
            left = mid + 1;
        } else if (strcmp(terms[mid].term, substr) < 0) {
            left = mid + 1; 
        } else {
            right = mid - 1; 
        }
    }
    
    return result;
}


int compare(const void *a, const void *b) {
    const term *t1 = (const term *) a;
    const term *t2 = (const term *) b;
    if (t1->weight > t2->weight) return -1;
    if (t1->weight < t2->weight) return 1;
    return 0;
}



void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr) {
    
    int low  = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);

    
    if (low == -1 || high == -1 || low > high) {

        *n_answer = 0;
        *answer   = NULL;
        return;
    }

  
    *n_answer = high - low + 1;


    *answer = malloc((*n_answer) * sizeof(term));
    

    for (int i = 0; i < *n_answer; i++) {


        (*answer)[i].weight = terms[low + i].weight;
        strcpy((*answer)[i].term, terms[low + i].term);
    }

    qsort(*answer, *n_answer, sizeof(term), compare);
}




// void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr);
// The function takes terms (assume it is sorted lexicographically in increasing order), the number of
// terms nterms, and the query string substr, and places the answers in answer, with *n_answer being the
// number of answers. The answers should be sorted by weight in non-decreasing order.
// Sorting with qsort
// See here: https://www.tutorialspoint.com/c_standard_library/c_function_qsort.htm
// You must use qsort for this question.


//Helper function to print an array of terms: */
// void print_terms(const term *arr, int n) {
//     for (int i = 0; i < n; i++) {
//         /* Print index, weight, and the term string */
//         printf("%2d) weight = %.2f, term = \"%s\"\n", 
//                i+1, arr[i].weight, arr[i].term);
//     }
// }
