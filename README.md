# Nearest-Neighbour

An exploration of the algorithm in [A Simple Algorithm for Nearest Neighbours in High Dimensions (October 1995) by Sameer A. Nene and Shree K. Nayar from the Department of Computer Science at Columbia University](https://www1.cs.columbia.edu/CAVE/publications/pdfs/Nene_TR95.pdf). In this readme, I will:
1. Explain my attempts to optimise the algorithm
2. Delve into some of the more complicated analysis on the relationship between the minimum hypercube/hypersphere measurements and the probability that a point exists within the range **(VERY IN DEPTH)**

## How to use ##
Please use **./optimised** (./original takes way too long and is more for theoretical comparison rather than use and optimised also prints out more information). Run **python3 generator.py** to generate data set nodes, the dimensionality of the nodes and test set nodes. Please modify the defines at the top of **optimised.c** to ensure the same parameters as the data you have generated. Modify the margin "double e" as you see fit. Compile and run. It will run nearest neighbour searches on all the test nodes and print out some information regarding their results (Note, currently, I am focusing on simply checking the number of nodes in the hypercubes rather than finding the nearest neighbour and have modified the code making it slightly inaccurate if your purpose is to find the nearest neighbour).

## Before we begin
I would recommend reading Chapter 3 of the paper beginning on page 5. It is about 3 pages long, requires little to no understanding of mathematics and will provide a broad overview of how the algorithm works. It will suffice for part 1.

## Optimisations
The original algorithm as given by the paper is in original.c My optimisations to this are located in optimised.c.

### Preprocessing the data
Instead of a bubble sort with <a href="https://www.codecogs.com/eqnedit.php?latex=O(n^2)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?O(n^2)" title="O(n^2)" /></a> time complexity, we will use a mergesort with <a href="https://www.codecogs.com/eqnedit.php?latex=O(nlog(n))" target="_blank"><img src="https://latex.codecogs.com/gif.latex?O(nlog(n))" title="O(nlog(n))" /></a>. This will lead to drastically improved performance for high n (especially since we will be performing our point analysis with a data set of 100 000 or more points). Note that we actually sort the indexes rather than the actual array and to do this, we initialise an indexTrack[] array of size n numbered 0 to n-1 each time we wish to sort. Furthermore, mergesort requires more overhead due to its recursive nature, however even taking all this into account, this cost is still negligible when dealing with data sets on a scale this large.

### Ordering the dimensions
On page 11 of the paper, it briefly mentions that it is most advantageous to trim the dimensions in ascending order of sandwiched points (the intuition behind this is that we are more likely to trim a node when analysing it with the smaller set of points and thus, we do not have to analyse the same node again later on). We will use an insertion sort here since we are dealing with d dimensions which in reasonable scenarios, are very small. Although it has <a href="https://www.codecogs.com/eqnedit.php?latex=O(n^2)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?O(d^2)" title="O(d^2)" /></a> time complexity, the inplace nature of an insertion sort means it has no overhead, making it very efficient at small scales.

However, the paper does not dive into further analysis on the ordering of dimensions. Intuitively, this ordering will only increase time efficiency if the gap between the ascending dimensions is big enough to offset the cost of sorting the dimensions. A difference in the number of sandwiched points for a coordinate x if either (x-e) <= 0 or (x+e) >= 1. For example, let's take a point x and an e of 0.2. If x = 0.1, then we can see that x - e = -0.1. However, since there are no values below 0, we are "going out of bounds" and we can expect only 30% of nodes to be sandwiched here (between 0 and x + e). On the other hand, let's say x = 0.5. Then x - e = 0.3 and x + e = 0.7 (neither points are out of bounds), giving us about 40% of the nodes sandwiched between these 2 boundaries.

Now, imagine extreme worst case scenarios where e < 0.000...1 or e > 0.999... . In these respective cases, we will never go out of bounds/always go out of bounds and thus, there will be little variation in the number of sandwiched points, making the dimensional ordering not worth it.

Furthermore, let's say that our test point coordinates are essentially the same, e.g. (0.6, 0.6, 0.6, 0.6, 0.6). Again, there would not be much point ordering the dimensions since there would not be much difference between the number of points sandwiched between 0.6+-e in any of the dimensions. Such coordinates like in the example given could arise from a Gaussian distribution with very little variance. However, since we are dealing with uniform point distributions these occurences would be very rare.

## Analysis of the relationship between p and e and improving on the given equation
We want to try and ensure we end up with at least one point remaining within the area after we have trimmed our list, or we will come up empty handed, without finding a nearest neighbour. The paper does try to give some calculations on the relationship between p (the probability that there is at least one point)

TO BE CONTINUED WITH VERY IN DEPTH ANALYSIS. CURRENTLY COLLATING SHEETS IN ORDER AND TRANSCRIBING TO LATEX.
