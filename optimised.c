/* The optimised algorithm. It will presort all dimensions before starting to trim
which will require multiple backward maps instead of just one.
It also uses a mergesort which is far quicker on large scale and an insertion sort
to order the dimensions for searching which is more effective on small scale */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#define N_NODES 100000
#define N_DIM 10
#define TEST_NODES 1000

typedef struct tuple {
    int index;
    double value;
} tuple;

void preprocess(double pointSet[N_DIM][N_NODES], double orderedSet[N_DIM][N_NODES], int bMap[N_DIM][N_NODES], int fMap[N_DIM][N_NODES]);
void sort(double unsorted[N_NODES], int indexTrack[N_NODES]);
void mergeSortIndex(double unsorted[N_NODES], int indexTrack[N_NODES]);
void mergeSort(tuple unsorted[N_NODES], int start, int end);
void merge(tuple unsorted[N_NODES], int start, int centre, int end);
void insertionSort(int unsorted[N_DIM], int indexTrack[N_DIM]);

int nearestNeighbourSearch(double point[N_DIM], double orderedSet[N_DIM][N_NODES], int bMap[N_DIM][N_NODES], int fMap[N_DIM][N_NODES], double e, double pointSet[N_DIM][N_NODES]);
int binarySearchLeftMost(double ordered[N_NODES], double target);
int binarySearchRightMost(double ordered[N_NODES], double target);

int main(void) {
    // Initialise pointSet, orderedSet, backward and forward maps and testing set
    static double pointSet[N_DIM][N_NODES];
    static double orderedSet[N_DIM][N_NODES];
    static int fMap[N_DIM][N_NODES];
    static int bMap[N_DIM][N_NODES];

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

    double e = 39.4;

    // For each node, test for the nearest neighbour
    int noNeighbour = 0;
    for (int n = 0; n < TEST_NODES; n++) {
        int index = nearestNeighbourSearch(testSet[n], orderedSet, bMap, fMap, e, pointSet);
        if (index == -1) {
            noNeighbour += 1;
        }
    }

    printf("%d nodes had no nearest neighbour in the hypercube of side length 2e with e = %lf\n", noNeighbour, e);

    return 0;
}

/* Preprocess the points, creating an ordered set and a backwards and forwards map.
O[i][F[i][j]] = P[i][j] and P[i][B[i][j]] = O[i][j] */
void preprocess(double pointSet[N_DIM][N_NODES], double orderedSet[N_DIM][N_NODES], int bMap[N_DIM][N_NODES], int fMap[N_DIM][N_NODES]) {    
    int indexTrack[N_NODES];

    // Create backward and forward maps for all dimensions
    for (int d = 0; d < N_DIM; d++) {
        mergeSortIndex(pointSet[d], indexTrack);
        for (int n = 0; n < N_NODES; n++) {
            orderedSet[d][n] = pointSet[d][indexTrack[n]];
            bMap[d][n] = indexTrack[n];
            fMap[d][indexTrack[n]] = n;
        }
    }
}

/* Given an unsorted array, will keep it unsorted but modify indexTrack to keep
track of the position of the original indexes in ascending order of their corresponding
values. */
void mergeSortIndex(double unsorted[N_NODES], int indexTrack[N_NODES]) {
    // Create structs of index, value pairs
    tuple unsortedTuples[N_NODES];
    for (int i = 0; i < N_NODES; i++) {
        unsortedTuples[i].index = i;
        unsortedTuples[i].value = unsorted[i];
    }

    mergeSort(unsortedTuples, 0, N_NODES - 1);

    for (int i = 0; i < N_NODES; i++) {
        indexTrack[i] = unsortedTuples[i].index;
    }

    return;
}

void mergeSort(tuple unsorted[N_NODES], int start, int end) {
    if (start < end) {
        int centre = start + (end - start) / 2;
        mergeSort(unsorted, start, centre);
        mergeSort(unsorted, centre + 1, end);
        merge(unsorted, start, centre, end);
    }
    
    return;
}

void merge(tuple unsorted[N_NODES], int start, int centre, int end) {
    int sizeLeft = centre - start + 1;
    int sizeRight = end - centre;

    tuple *leftElems = (tuple*)malloc(sizeLeft * sizeof(tuple));
    tuple *rightElems = (tuple*)malloc(sizeRight * sizeof(tuple));

    for (int i = 0; i < sizeLeft; i++) {
        leftElems[i] = unsorted[i + start];
    }
    for (int i = 0; i < sizeRight; i++) {
        rightElems[i] = unsorted[i + centre + 1];
    }

    int i = 0;
    int j = 0;
    int k = start;

    while (i < sizeLeft && j < sizeRight) {
        if (leftElems[i].value < rightElems[j].value) {
            unsorted[k] = leftElems[i];
            k++;
            i++;
        } else {
            unsorted[k] = rightElems[j];
            k++;
            j++;
        }
    }

    while (i < sizeLeft) {
        unsorted[k] = leftElems[i];
        k++;
        i++;
    }
    while (j < sizeRight) {
        unsorted[k] = rightElems[j];
        k++;
        j++;
    }

    free(leftElems);
    free(rightElems);

    return;
}
/* Given an unsorted list, will keep it unsorted by modify indexTrack to keep track
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

/* Given an unsorted array, will keep it unsorted but modify indexTrack to keep
track of the position of the original indexes in ascending order of their corresponding
values. */
void insertionSort(int unsorted[N_DIM], int indexTrack[N_DIM]) {
    for (int i = 0; i < N_DIM; i++) {
        indexTrack[i] = i;
    }

    int key, j;
    for (int i = 1; i < N_DIM; i++) {
        key = indexTrack[i];
        j = i - 1;
        while (j >= 0 && unsorted[indexTrack[j]] > unsorted[key]) {
            indexTrack[j + 1] = indexTrack[j];
            j--;
        }
        indexTrack[j + 1] = key;
    }

    return;
}

/* Performs the closest point search. It returns an index into the point set array
which represents the cloeset point. Returns -1 if no nearest poitn was found within
the given margin */
int nearestNeighbourSearch(double point[N_DIM], double orderedSet[N_DIM][N_NODES], int bMap[N_DIM][N_NODES], int fMap[N_DIM][N_NODES], double e, double pointSet[N_DIM][N_NODES]) {
    // Perform binary searches on all dimensions and order them in ascending order
    // based on number of points between the indexes.
    int lowerIndexes[N_DIM]; // lowerIndexes[d] = lower bound index of the dth dimension
    int upperIndexes[N_DIM]; // upperIndexes[d] = upper bound index of the dth dimension
    int nPoints[N_DIM]; // nPoints[d] = number of sandwiched points of the dth dimension

    for (int d = 0; d < N_DIM; d++) {
        lowerIndexes[d] = binarySearchLeftMost(orderedSet[d], point[d] - e);
        upperIndexes[d] = binarySearchRightMost(orderedSet[d], point[d] + e);
        nPoints[d] = upperIndexes[d] - lowerIndexes[d];
    }

    // Sort the dimensions in ascending order based on number of sandwiched points
    // sortedDimension[0] = dimension with lest number of sandwiched points
    int sortedDimensions[N_DIM];
    insertionSort(nPoints, sortedDimensions);

    // Create list of candidates to be trimmed
    int candidates[N_NODES];
    int nCandidates = 0;
    for (int i = lowerIndexes[sortedDimensions[0]]; i <= upperIndexes[sortedDimensions[0]]; i++) {
        candidates[nCandidates] = bMap[sortedDimensions[0]][i];
        nCandidates++;
    }

    // Trim the candidates in dimension order of ascending number of sandwiched points
    for (int d = 1; d < N_DIM; d++) {
        int dimension = sortedDimensions[d];
        int lower = lowerIndexes[dimension];
        int upper = upperIndexes[dimension];
        
        // Track how many candidates before and after the round
        int beforeTrim = nCandidates;
        nCandidates = 0;
        
        for (int n = 0; n < beforeTrim; n++) {
            if (fMap[dimension][candidates[n]] >= lower && fMap[dimension][candidates[n]] <= upper) {
                candidates[nCandidates] = candidates[n];
                nCandidates++;
            }
        }
    }

    printf("Nodes after trim = %d\n", nCandidates);
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