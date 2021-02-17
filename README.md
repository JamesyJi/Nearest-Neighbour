# Nearest-Neighbour

In my spare time, I came across an interesting algorithm in [A Simple Algorithm for Nearest Neighbours in High Dimensions (October 1995) by Sameer A. Nene and Shree K. Nayar from the Department of Computer Science at Columbia University](https://www1.cs.columbia.edu/CAVE/publications/pdfs/Nene_TR95.pdf). In this readme, I will:
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
We want to try and ensure we end up with at least one point remaining within the area after we have trimmed our list, or we will come up empty handed, without finding a nearest neighbour. The paper does try to give some calculations on the relationship between p (the probability that there is at least one point) and e (the radius of the hypersphere/half the side of the hypercube we are searching). I will deal specifically with the hypercube algorithm here but the hypersphere one can also be optimised.

Let's deal with l = 1 and n = 100 000 for our examples. We will also use 10 000 test nodes.

The paper provides the following equation for e, given a probability p that at least one point will remain in the hypercube after trimming if there are n nodes in the data set and each node is of d dimensionality:
<a href="https://www.codecogs.com/eqnedit.php?latex=e=\frac{l}{2}(1-(1-p)^\frac{1}{n})^\frac{1}{d}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?e=\frac{l}{2}(1-(1-p)^\frac{1}{n})^\frac{1}{d}" title="e=\frac{l}{2}(1-(1-p)^\frac{1}{n})^\frac{1}{d}" /></a>
However, this is not correct. Let's do a test for d = 10, p = 0.99 which gives us e = 0.184201674. In otherwords, if we set the search margin to this value of e, we should expect that 99% of the 10 000 test nodes are non-empty or only 1% (100) nodes are empty. Running our code with these conditions, 2487 nodes come back as empty. Now this is very far off the 100 expected empty nodes we were meant to get. 

If I was to determine the reasoning for this mistake, I would point towards their analysis of the density function which can be found on page 12 of their paper. In particular, equation (7) gives <a href="https://www.codecogs.com/eqnedit.php?latex=P_c{\leq}\frac{2e}{l}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?P_c{\leq}\frac{2e}{l}" title="P_c{\leq}\frac{2e}{l}" /></a> which I believe is way too loose of a bound. They are essentially saying that the probability of a value being between +-e of another value is \frac{2e}{l}. Intuitively, this is incorrect. Let's take l = 1 for simplicity. Assume you have a number line from 0 to 1. Let's take a point 0.1 and e = 0.2. According to the mentioned formula, the probability that a point lies within 0.1 +- 0.2 should be 0.4. However, since our points only take value from [0,1], it is impossible for a value to fall between 0.1-0.2 and 0. Therefore, the real probablity is only 0.3. As you can see, the problem stems from sections being "cut off". In order to resolve this problem and tighten the bounds, we must determine a new formula which takes into account this "cut off" area.

### The distance between 2 random values on [0,1]
Consider the following problem: If we randomly pick 2 values Qi and Zi in this domain, what is the probability that their distance is greater than a certain value e? The answer is <a href="https://www.codecogs.com/eqnedit.php?latex=(1-e)^2" target="_blank"><img src="https://latex.codecogs.com/gif.latex?(1-e)^2" title="(1-e)^2" /></a> and it can be visualised [here](https://i.stack.imgur.com/ssHTf.png) where the shaded triangles where possible values to satisfy this criteria. The total area is 1 and so, the probabliity that 2 randomly picked points are **less than** e distance of each other is <a href="https://www.codecogs.com/eqnedit.php?latex=1-(1-e)^2" target="_blank"><img src="https://latex.codecogs.com/gif.latex?1-(1-e)^2" title="1-(1-e)^2" /></a>. 

Now, if Q is the test node with values Q1, Q2, Q3...Qd and likewise, Z is a node in the data set with values Z1, Z2, Z3... Zd and since the values of Qi and Zi are independent from each other... then the probability that all Zi lies within +-e of a corresponding Qi is just <a href="https://www.codecogs.com/eqnedit.php?latex=(1-(1-e)^2)^d" target="_blank"><img src="https://latex.codecogs.com/gif.latex?(1-(1-e)^2)^d" title="(1-(1-e)^2)^d" /></a>. In otherwords, this is the probability that Z lies within a hypercube of side length 2e centred at Q.

Then, with some simple binomial analysis (cutting out a lot of working out since markdown equations are a pain), the probability that k point lie inside this hypercube is:
<a href="https://www.codecogs.com/eqnedit.php?latex=P(k)=((2e-e^2)^d)^k(1-(2e-e^2)^d)^{n-k}\binom{n}{k}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?P(k)=((2e-e^2)^d)^k(1-(2e-e^2)^d)^{n-k}\binom{n}{k}" title="P(k)=((2e-e^2)^d)^k(1-(2e-e^2)^d)^{n-k}\binom{n}{k}" /></a>

Let p be the probability that **at least** one point lies inside this hypercube. Then:
<a href="https://www.codecogs.com/eqnedit.php?latex=p=1-P(0)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?p=1-P(0)" title="p=1-P(0)" /></a>
<a href="https://www.codecogs.com/eqnedit.php?latex==1-(1-(2e-e^2)^d)^n" target="_blank"><img src="https://latex.codecogs.com/gif.latex?=1-(1-(2e-e^2)^d)^n" title="=1-(1-(2e-e^2)^d)^n" /></a>

Subjecting e and we get:
<a href="https://www.codecogs.com/eqnedit.php?latex=e=1-\sqrt{1-(1-(1-p)^{\frac{1}{N}})^\frac{1}{D}}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?e=1-\sqrt{1-(1-(1-p)^{\frac{1}{N}})^\frac{1}{D}}" title="e=1-\sqrt{1-(1-(1-p)^{\frac{1}{N}})^\frac{1}{D}}" /></a>.

This time, for p = 0.99, we get e = 0.205269447. Let's run the code again with the same parameters as before. Again, we are expecting 100 hypercubes to be empty. Instead, we get 611 which is off, but a far greater improvement on the 2487 we got prior.

### What could be going wrong?
The problem is because we have dependency of events. Note that <a href="https://www.codecogs.com/eqnedit.php?latex=1-(1-e)^2" target="_blank"><img src="https://latex.codecogs.com/gif.latex?1-(1-e)^2" title="1-(1-e)^2" /></a> is the probability that 2 **random** value are within distance e of each other. Whilst this is all good for analysing one point Z with one point Q, it starts to break down with multiple Z and one point of Q. Basically, we are testing multiple data set points (Z) with the same test point (Q). Q is now no longer random for each Z and is fixed instead. Since the binomial distribution relies on the events to have no dependencies on each other, this leads to a breakdown in the analysis. 

### How can we fix this? Future research
I currently have a few investigations going on in order to try and resolve this problem. They are not polished yet (nor do I know if this if they will be since this could be an unsolved problem in mathematics as I have found nothing with my research so far regarding this issue). There are 2 main paths I am going down:
A) Hypercube line picking + square root of CLT to analyse the distribution of distances between 2 points in a hypercube.
B) Determining a non-piecewise function to approximate the distance between Z and a fixed point Q in terms of the coordinates of Q.

Option B has a lot of promise and I have created some graphs here: https://www.desmos.com/calculator/kteab1izwg. I have used c instead of e here (since e is a reserved variable on desmos). The gist is that x is a point in [0,1]. The red lines form a piecewise function giving the probability that a random value will lie below x-e. Similarly, the blue lines give the probability that a random value will lie below x+e. To get the probability that a random value lies between x-e and x+e, the blue function is subtracted from the red function to give the black function. My goal is to find a smooth function to approximate this piecewise function. This function is based on the values of the fixed test point so it should be fine to extend this analysis to cater to n data set points for a fixed test point, thus resolving the issue we had previously. Then the probability that a point Z lies within a point Q is now:
<a href="https://www.codecogs.com/eqnedit.php?latex=\prod_{i=1}^{d}P(x_i)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\prod_{i=1}^{d}P(x_i)" title="\prod_{i=1}^{d}P(x_i)" /></a>
Where x_1, x_2, x_3...x_d are the values of the coordinates of the test point in d dimensions.

Our problem has now shifted to resolving this product. What is its distribution? Expected value? Variance? etc.
