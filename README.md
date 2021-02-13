# Nearest-Neighbour

An exploration of the algorithm in [A Simple Algorithm for Nearest Neighbours in High Dimensions (October 1995) by Sameer A. Nene and Shree K. Nayar from the Department of Computer Science at Columbia University](https://www1.cs.columbia.edu/CAVE/publications/pdfs/Nene_TR95.pdf). In this readme, I will:
1. Explain my attempts to optimise the algorithm
2. Delve into some of the more complicated analysis on the relationship between the minimum hypercube/hypersphere measurements and the probability that a point exists within the range

## Before we begin
I would recommend reading Chapter 3 of the paper beginning on page 5. It is about 3 pages long, requires little to no understanding of mathematics and will provide a broad overview of how the algorithm works. It will suffice for part 1.

## Optimisations
The original algorithm as given by the paper is in original.c My optimisations to this are located in optimised.c.

### Preprocessing the data
Instead of a bubble sort with <a href="https://www.codecogs.com/eqnedit.php?latex=O(n^2)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?O(n^2)" title="O(n^2)" /></a> time complexity, we will use a mergesort with <a href="https://www.codecogs.com/eqnedit.php?latex=O(nlog(n))" target="_blank"><img src="https://latex.codecogs.com/gif.latex?O(nlog(n))" title="O(nlog(n))" /></a>. This will lead to drastically improved performance for high n (especially since we will be performing our point analysis with a data set of 100 000 or more points). Note that we actually sort the indexes rather than the actual array and to do this, we initialise an indexTrack[] array of size n numbered 0 to n-1 each time we wish to sort. Furthermore, mergesort requires more overhead due to its recursive nature, however even taking all this into account, this cost is still negligible when dealing with data sets on a scale this large.

### Ordering the dimensions
On page 11 of the paper, it briefly mentions that it is most advantageous to trim the dimensions in ascending order of sandwiched points (the intuition behind this is that we are more likely to trim a node when analysing it with the smaller set of points and thus, we do not have to analyse the same node again later on). We will use an insertion sort here since we are dealing with d dimensions which in reasonable scenarios, are very small. Although it has <a href="https://www.codecogs.com/eqnedit.php?latex=O(n^2)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?O(d^2)" title="O(d^2)" /></a> time complexity, the inplace nature of an insertion sort means it has no overhead, making it very efficient at small scales.

However, the paper does not dive into further analysis on the ordering of dimensions. Intuitively, this ordering will only increase time efficiency if the gap between the ascending dimensions is big enough to offset the cost of sorting the dimensions. Imagine a worst case scenario where our e = 1. That is, when we binarysearch, we end up with an lower index of 0 and an upper index of n-1 (i.e. we did not cut off any nodes). This would mean that there is no difference whether or not we order the dimensions since they all contain the same, maximum number of sandwiched points. 

Furthermore, let's say that our test point coordinates are essentially the same, e.g. (0.6, 0.6, 0.6, 0.6, 0.6). Again, there would not be much point ordering the dimensions since there would not be much difference between the number of points sandwiched between 0.6+-e in any of the dimensions.
