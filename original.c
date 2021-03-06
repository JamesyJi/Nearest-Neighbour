/* The original algorithm from https://www1.cs.columbia.edu/CAVE/publications/pdfs/Nene_TR95.pdf
This algorithm arbitrarily starts trimming with the first dimension instead of
presorting the points.
It also uses a bubble sort instead of a mergesort. */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>

// Modify these parameters as you see fit
#define N_NODES 100000
#define N_DIM 10
#define TEST_NODES 1000
double e = 39.4;

double searchTime = 0;


void preprocess(double pointSet[N_DIM][N_NODES], double orderedSet[N_DIM][N_NODES], int bMap[N_NODES], int fMap[N_DIM][N_NODES]);
void sort(double unsorted[N_NODES], int indexTrack[N_NODES]);
int nearestNeighbourSearch(double point[N_DIM], double orderedSet[N_DIM][N_NODES], int bMap[N_NODES], int fMap[N_DIM][N_NODES], double e, double pointSet[N_DIM][N_NODES]);
int binarySearchLeftMost(double ordered[N_NODES], double target);
int binarySearchRightMost(double ordered[N_NODES], double target);

int main(void) {
    // Initialise pointSet, orderedSet, backward and forward maps and testing set
    static double pointSet[N_DIM][N_NODES];
    static double orderedSet[N_DIM][N_NODES];
    static int fMap[N_DIM][N_NODES];
    static int bMap[N_NODES];

    static double testSet[N_NODES][N_DIM];

    // Read in data from the file into the pointSet
    FILE *dataSetFile;
    dataSetFile = fopen("dataset.txt", "r");
    for (int n = 0; n < N_NODES; n++) {
        for (int d = 0; d < N_DIM; d++) {
            fscanf(dataSetFile, "%lf", &pointSet[d][n]);
        }
    }
    fclose(dataSetFile);
    printf("=====Finished processing pointset=====\n");

    // Preprocess the orderedset and maps
    preprocess(pointSet, orderedSet, bMap, fMap);
    printf("=====Finished processing orderedset and maps=====\n");

    // Read in data from the testing set
    FILE *testSetFile;
    testSetFile = fopen("testset.txt", "r");
    for (int n = 0; n < TEST_NODES; n++) {
        for (int d = 0; d < N_DIM; d++) {
            fscanf(testSetFile, "%lf", &testSet[n][d]);
        }
    }
    fclose(testSetFile);

    printf("=====Finished processing data=====\n");

    // For each node, test for the nearest neighbour
    int noNeighbour = 0;
    for (int n = 0; n < TEST_NODES; n++) {
        int index = nearestNeighbourSearch(testSet[n], orderedSet, bMap, fMap, e, pointSet);
        if (index == -1) {
            noNeighbour += 1;
        }
    }

    printf("%d test nodes had no data nodes in the hypercube of side length 2e with e = %lf\n", noNeighbour, e);
    printf("Took %lf seconds to search\n", searchTime);
    return 0;
}

/* Preprocess the points, creating an ordered set and a backwards and forwards map.
O[i][F[i][j]] = P[i][j] and P[i][B[i][j]] = O[i][j] */
void preprocess(double pointSet[N_DIM][N_NODES], double orderedSet[N_DIM][N_NODES], int bMap[N_NODES], int fMap[N_DIM][N_NODES]) {    
    int indexTrack[N_NODES];

    // Create backward map for the first dimension by sorting first dimension in point set
    sort(pointSet[0], indexTrack);
    for (int i = 0; i < N_NODES; i++) {
        // printf("i = %d, indexTrack = %d, pointSetIndexTrack = %lf\n", i, indexTrack[i], pointSet[0][indexTrack[i]]);
        orderedSet[0][i] = pointSet[0][indexTrack[i]];
        bMap[i] = indexTrack[i];
        fMap[0][indexTrack[i]] = i;
    }

    printf("Created one backward map\n");
    // printf("Printing the ordered set of first dimension\n");
    // for (int i = 0; i < N_NODES; i++) {
    //     printf("%lf\n", orderedSet[0][i]);
    // }

    // Create forward map for each dimension by sorting corresponding coordinate
    // in the point set
    for (int i = 1; i < N_DIM; i++) {
        sort(pointSet[i], indexTrack);
        for (int j = 0; j < N_NODES; j++) {
            orderedSet[i][j] = pointSet[i][indexTrack[j]];
            fMap[i][indexTrack[j]] = j;
        }
    }
}

/* Given an unsorted list, will keep it unsorted but modify indexTrack to keep track
of the position of the original indexes in ascending order */
/* Sorts an unsorted list and modifies indexTrack to keep track of the position
of the original indexes in the newly sorted array.*/
void sort(double unsorted[N_NODES], int indexTrack[N_NODES]) {
    // printf("Starting a bubble sort\n");
    // Initialise the temp map
    for (int i = 0; i < N_NODES; i++) {
        indexTrack[i] = i;
    }

    // Bubble sort whilst keeping track of the new indexes
    for (int i = 0; i < N_NODES - 1; i++) {
        for (int j = 0; j < N_NODES - i - 1; j++) {
            if (unsorted[indexTrack[j]] > unsorted[indexTrack[j + 1]]) {
                // double t = unsorted[j + 1];
                // unsorted[j + 1] = unsorted[j];
                // unsorted[j] = t;
                
                double t = indexTrack[j + 1];
                indexTrack[j + 1] = indexTrack[j];
                indexTrack[j] = t;
            }
        }
    }

    // printf("Finished a bubble sort\n");
    return;
}

/* Performs the closest point search. It returns an index into the point set array
which represents the cloeset point. Returns -1 if no nearest poitn was found within
the given margin */
int nearestNeighbourSearch(double point[N_DIM], double orderedSet[N_DIM][N_NODES], int bMap[N_NODES], int fMap[N_DIM][N_NODES], double e, double pointSet[N_DIM][N_NODES]) {
    clock_t start = clock();

    // Perform binary search on first dimension
    int bottom = binarySearchLeftMost(orderedSet[0], point[0] - e);
    int top = binarySearchRightMost(orderedSet[0], point[0] + e);
    // printf("First dimension has lower %d and upper %d\n", bottom, top);

    // Create list of candidates to be trimmed
    int candidates[N_NODES];
    int nCandidates = 0;
    for (int i = bottom; i <= top; i++) {
        candidates[nCandidates] = bMap[i];
        nCandidates++;
    }

    // printf("After first trim, these candidates are within range\n");
    // for (int i = 0; i < nCandidates; i++) {
    //     printf("%lf\n", pointSet[0][candidates[i]]);
    // }

    // Trim list with binary searches on other dimensions along with lookups
    for (int d = 1; d < N_DIM; d++) {
        bottom = binarySearchLeftMost(orderedSet[d], point[d] - e);
        top = binarySearchRightMost(orderedSet[d], point[d] + e);
        // printf("Dimension %d has lower %d and upper %d\n", d, bottom, top);

        // Track how many candidates before and after the round
        int beforeTrim = nCandidates;
        nCandidates = 0;

        for (int n = 0; n < beforeTrim; n++) {
            if (fMap[d][candidates[n]] >= bottom && fMap[d][candidates[n]] <= top) {
                candidates[nCandidates] = candidates[n];
                nCandidates++;
            }
        }
    }

    // printf("Nodes after trim = %d\n", nCandidates);
    // Perform an exhaustive search on the remaining points
    double maximum = DBL_MAX;
    int index = -1;
    for (int n = 0; n < nCandidates; n++) {
        double distance = 0;
        for (int d = 0; d < N_DIM; d++) {
            distance += pow(point[d] - orderedSet[d][fMap[d][candidates[n]]], 2);
        }
        if (distance < maximum) {
            maximum = distance;
            index = candidates[n];
        }
    }

    clock_t end = clock();
    searchTime += (double)(end - start) / CLOCKS_PER_SEC;

    return index;
}

/* Determines the leftmost element via binary search on an ordered list and returns it.
If target is not in the array, Returns the index of the smallest element larger than target.
Note that this index will range between 0 to n(out of bounds). However for this program,
our target will always exist in the ordered list and so index will never = n */
int binarySearchLeftMost(double ordered[N_NODES], double target) {
    int lower = 0;
    int upper = N_NODES;
    while (lower < upper) {
        int centre = (lower + upper) / 2;
        if (ordered[centre] < target) {
            lower = centre + 1;
        } else {
            upper = centre;
        }
    }
    return lower;
}

/* Determines the rightmost element via binary search on an ordered list and returns it.
If the target is not in the array, Returns the index of the largest element smaller than target.
Note that this index will range between -1(out of bounds) to n-1. However for this program,
our target will always exist in the ordered list and so index will never = -1 */
int binarySearchRightMost(double ordered[N_NODES], double target) {
    int lower = 0;
    int upper = N_NODES;
    while (lower < upper) {
        int centre = (lower + upper) / 2;
        if (ordered[centre] > target) {
            upper = centre;
        } else {
            lower = centre + 1;
        }
    }
    return upper - 1;
}