# Triangulation Flip Graph

Small CLI binary that computes all the convex triangulations on a 2D plane and their connectivity graph.

## Motivation

During my PhD, I encountered the problem of enumerating and exploring the connectivity of triangulations of a convex polytope in 2D (points in $\mathbb{Z}^2$). The connectivity of these triangulations is defined via a flip operation: given a triangulation, if we select two adjacent triangles that form a convex quadrilateral, we can replace their shared diagonal edge with the other diagonal of the quadrilateral. This operation results in a new triangulation, and repeating this process explores an undirected graph of possible triangulations connected by flips.

Computing these triangulations is computationally expensive, as the number of triangulations grows exponentially with the number of points. Even in the best-case scenario, where all $n$ points form a convex polygon, the number of triangulations follows the Catalan number sequence:
```math
T(n) = C_{n-2} = \frac{1}{n-1} \binom{2n-4}{n-2}
```
For a general set of $n$ points in the plane, the number of triangulations is at least exponential:
```math
T(n) \geq 2^{\Omega(n)}
```

## Approach

Currently, to efficiently handle this combinatorial explosion, this project employs high-performance concurrency techniques. A multi-threaded approach with concurrent queues distributes work efficiently across multiple cores, reducing contention and improving scalability. Each thread processes triangulations independently, applying flip operations and storing meshes in a concurrent hash set.

This project is a playground for experimenting with all possible optimization techniques to accelerate graph exploration. It is designed as a testbed for evaluating different concurrency models, data structures, and memory access patterns.


## Usage (in development)

Provide a single string as the argument for the set of points being triangulated.

For example, given a set of points:

```
(-1,-1) (-1,0) (-1,1) (0,-1) (0,0) (0,1) (1,-1)
```

Run the command:

```bash
./triangulation-graph "(-1,-1) (-1,0) (-1,1) (0,-1) (0,0) (0,1) (1,-1)"
```

![Triangulation Growth](https://user-images.githubusercontent.com/13206784/222421996-70b2c408-6252-4147-bb75-7a4c2e7f56d6.png)
The average counts and time are obtained by taking a sample of uniformly distributed points.

## Output (in development)

The program outputs all possible triangulations as a list of 3-tuples of points.
Each triangulation is printed on a new line.
The last line in the output represents the edges of the graph connecting the triangulations by a flip operation (labeled sequentially: 0, 1, ...).

```plaintext
((-1 -1,-1 0,0 -1), (-1 0,-1 1,0 0), (-1 0,0 -1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 -1), (0 0,1 -1,1 0))
((-1 -1,-1 0,0 0), (-1 -1,0 -1,0 0), (-1 0,-1 1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 -1), (0 0,1 -1,1 0))
((-1 -1,-1 0,0 -1), (-1 0,-1 1,0 0), (-1 0,0 -1,1 -1), (-1 0,0 0,1 -1), (-1 1,0 0,1 0), (0 0,1 -1,1 0))
...
{7,5} {6,4} {6,3} {5,3} {5,1} {4,6} {3,0} {5,7} {3,6} {3,5} {2,0} {1,5} {1,0} {0,4} {0,2} {0,3} {4,0} {0,1}
```

Graphically, the result is the following triangulations:

![Triangulation Graph](https://user-images.githubusercontent.com/13206784/221058089-98475a90-fab0-446f-ab20-6a7805ea728d.png)

Including the graph of flip operations:

![Flip Graph](https://user-images.githubusercontent.com/13206784/221057170-d016f272-b2c8-4eb4-a41b-b15f09094dbb.png)
