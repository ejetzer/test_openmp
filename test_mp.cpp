/* Fichier test_mp.cpp
Copyright 2021 Émile Jetzer
*/
#include <omp.h>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

using std::cout,
      std::endl;

// -----
// Tiré directement du code de MPS
// void
// Unstructured(uint        ThetaLength,
//             uint        PhiLength,
//             complex128 *array0,
//             double     *array1,
//             complex128  scalar,
//             complex128 *output)
// {
//  for (uint p=0; p < PhiLength; p++ )
//  {
//    *output   = scalar * array0[p] * array1[p];
//     output++; // Se comporte comme un tableau, bouge avec p?
//  }
// }
// -----

void une_seule_boucle(int n, int X[], int Y[], int sortie[]) {
    for (int i = 0; i < n; i++) {
        sortie[i] = X[i] * Y[i];
    }
}

void une_seule_boucle_omp(int n, int X[], int Y[], int sortie[]) {
    int i;

    #pragma omp parallel shared(n, X, Y, sortie)\
                         private(i)
    #pragma omp for
    for (i = 0; i < n; i++) {
        sortie[i] = X[i] * Y[i];
    }
}

// -----
// void
// Structured(uint        ThetaLength,
//           uint        PhiLength,
//           complex128 *array0,
//           double     *array1,
//           complex128  scalar,
//           complex128 *output)
// {
//  for (uint p=0; p < PhiLength; p++ )
//  {
//    for (uint t=0; t < ThetaLength; t++ )
//    {
//      *output   = scalar * array0[p] * array1[t];
//       output++;
//    }
//  }
// }
// -----

void boucle_imbriquee(int n, int m, int X[], int Y[], int sortie[]) {
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            sortie[(i+1)*j] = X[i] + Y[j];
        }
    }
}

void boucle_imbriquee_omp(int n, int m, int X[], int Y[], int sortie[]) {
    int i, j;

    #pragma omp parallel shared(n, m, X, Y, sortie)\
                         private(i, j)
    #pragma omp for collapse(2)
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            sortie[(i+1)*j] = X[i] + Y[j];
        }
    }
}

void initialiser(int tableau[], int n, unsigned int *graine) {
    for (int i = 0; i < n; i++) {
        tableau[i] = rand_r(graine) % 100;
    }
}

void afficher(int tableau[], int a, int b) {
    for (int i = a; i < b; i++) {
        cout << i << ")\t" << tableau[i] << endl;
    }
}

bool tout(int X[], int Y[], int n) {
    for (int i = 0; i < n; i++) {
        if (X[i] != Y[i]) {
            return false;
        }
    }

    return true;
}

int main() {
    int n = 1000, m = 500;
    int X[n], Y[n];
    int s[n] = {0}, s_omp[n] = {0};
    int s2[n*m] = {0}, s2_omp[n*m] = {0};
    unsigned int graine1 = 10, graine2 = 20;

    // Donner des valeurs de départ aléatoires aux tableaux
    initialiser(X, n, &graine1);
    initialiser(Y, n, &graine2);

    cout << "une_seule_boucle(n, X[], Y[], s[])" << endl;
    une_seule_boucle(n, X, Y, s);
    cout << "une_seule_boucle_omp(x, X[], Y[], s[])" << endl;
    une_seule_boucle_omp(n, X, Y, s_omp);
    cout << "Comparaison: " << tout(s, s_omp, n) << endl;

    cout << "boucle_imbriquee(n, m, X[], Y[], s[])" << endl;
    boucle_imbriquee(n, m, X, Y, s2);
    cout << "boucle_imbriquee_omp(n, m, X[], Y[], s[])" << endl;
    boucle_imbriquee_omp(n, m, X, Y, s2_omp);
    cout << "Comparaison: " << tout(s2, s2_omp, n*m) << endl;

    return 0;
}
