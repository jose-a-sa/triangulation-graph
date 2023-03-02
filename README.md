# Triangulation Flip Graph

Small CLI binary that computes all the convex triangulations on a 2D plane connected.

## Usage

Provide an single string as the argument for the set of points being triangulated.

Take for example a set of points given by _(-1,-1) (-1,0) (-1,1) (0,-1) (0,0) (0,1) (1,-1)_.

```bash
./triangulation-graph "(-1,-1) (-1,0) (-1,1) (0,-1) (0,0) (0,1) (1,-1)"
```

***Note:*** The number of triangulations of a set of $n$ points in the plane growns exponentially with $n$.

<img width="618" alt="Untitled" src="https://user-images.githubusercontent.com/13206784/222421996-70b2c408-6252-4147-bb75-7a4c2e7f56d6.png">
The average counts and time are obtained by taking a sample of uniformly distributed points $(x, y)$, for $0 \le x,y < 1$.


## Output

The program outputs all the possible triangulations as a list of 3-tuples of points.
Each triangulation is printed in a new line.
The last line in the output is the edges of the graph connecting the triangulation by a flip operation (labelled by 0,
1, ..., in the same order).

```
((-1 -1,-1 0,0 -1), (-1 0,-1 1,0 0), (-1 0,0 -1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 -1), (0 0,1 -1,1 0))
((-1 -1,-1 0,0 0), (-1 -1,0 -1,0 0), (-1 0,-1 1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 -1), (0 0,1 -1,1 0))
((-1 -1,-1 0,0 -1), (-1 0,-1 1,0 0), (-1 0,0 -1,1 -1), (-1 0,0 0,1 -1), (-1 1,0 0,1 0), (0 0,1 -1,1 0))
((-1 -1,-1 0,0 -1), (-1 0,-1 1,0 0), (-1 0,0 -1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 0), (0 -1,1 -1,1 0))
((-1 -1,-1 0,0 -1), (-1 0,-1 1,0 -1), (-1 1,0 -1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 -1), (0 0,1 -1,1 0))
((-1 -1,-1 0,0 0), (-1 -1,0 -1,0 0), (-1 0,-1 1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 0), (0 -1,1 -1,1 0))
((-1 -1,-1 0,0 -1), (-1 0,-1 1,0 -1), (-1 1,0 -1,0 0), (-1 1,0 0,1 0), (0 -1,0 0,1 0), (0 -1,1 -1,1 0))
((-1 -1,-1 0,0 0), (-1 -1,0 -1,1 0), (-1 -1,0 0,1 0), (-1 0,-1 1,0 0), (-1 1,0 0,1 0), (0 -1,1 -1,1 0))
{7,5} {6,4} {6,3} {5,3} {5,1} {4,6} {3,0} {5,7} {3,6} {3,5} {2,0} {1,5} {1,0} {0,4} {0,2} {0,3} {4,0} {0,1}
```

Graphically, the result is the following triangulations,

<img width="960" alt="image" src="https://user-images.githubusercontent.com/13206784/221058089-98475a90-fab0-446f-ab20-6a7805ea728d.png">

including the graph of flip operations.

<img width="480" alt="image" src="https://user-images.githubusercontent.com/13206784/221057170-d016f272-b2c8-4eb4-a41b-b15f09094dbb.png">
