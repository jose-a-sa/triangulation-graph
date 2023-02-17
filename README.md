# Triangulation Flip Graph

Small CLI binary that computes all the convex triangulations on a 2D plane connected.

## Usage

Provide an single string as the argument for the set of points being triangulated.

Take for example a set of points given by _(-1,-1) (-1,0) (-1,1) (0,-1) (0,0) (0,1) (1,-1)_.

```bash
./triangulation-graph "(-1,-1) (-1,0) (-1,1) (0,-1) (0,0) (0,1) (1,-1)"
```

## Output

The program outputs all the possible triangulations in WKT (well-known text) format.
The last line in the output is the edges of the graph connecting the triangulation by a flip operation (labelled by 0,
1, ..., in the same order).

```
MULTIPOLYGON((-1 -1,0 -1,0 0), (-1 -1,-1 0,0 0), (0 -1,1 -1,1 0), (0 -1,0 0,1 0), (-1 0,0 0,-1 1), (0 0,1 0,-1 1))
MULTIPOLYGON((-1 -1,0 -1,-1 0), (0 -1,1 -1,1 0), (0 -1,-1 0,0 0), (0 -1,0 0,1 0), (-1 0,0 0,-1 1), (0 0,1 0,-1 1))
MULTIPOLYGON((-1 -1,0 -1,1 0), (-1 -1,-1 0,0 0), (-1 -1,0 0,1 0), (0 -1,1 -1,1 0), (-1 0,0 0,-1 1), (0 0,1 0,-1 1))
MULTIPOLYGON((-1 -1,0 -1,0 0), (-1 -1,-1 0,0 0), (0 -1,1 -1,0 0), (1 -1,0 0,1 0), (-1 0,0 0,-1 1), (0 0,1 0,-1 1))
MULTIPOLYGON((-1 -1,0 -1,-1 0), (0 -1,1 -1,0 0), (0 -1,-1 0,0 0), (1 -1,0 0,1 0), (-1 0,0 0,-1 1), (0 0,1 0,-1 1))
MULTIPOLYGON((-1 -1,0 -1,-1 0), (0 -1,1 -1,-1 0), (1 -1,-1 0,0 0), (1 -1,0 0,1 0), (-1 0,0 0,-1 1), (0 0,1 0,-1 1))
MULTIPOLYGON((-1 -1,0 -1,-1 0), (0 -1,1 -1,0 0), (0 -1,-1 0,-1 1), (0 -1,0 0,-1 1), (1 -1,0 0,1 0), (0 0,1 0,-1 1))
MULTIPOLYGON((-1 -1,0 -1,-1 0), (0 -1,1 -1,1 0), (0 -1,-1 0,-1 1), (0 -1,0 0,1 0), (0 -1,0 0,-1 1), (0 0,1 0,-1 1))
{1,0} {2,0} {7,1} {6,4} {6,7} {4,1} {4,3} {3,0} {5,4} {4,6} {1,4} {0,3} {0,2} {3,4} {1,7} {7,6} {4,5} {0,1}
```

Graphically, the result is the following triangulations,

<img width="900" alt="image" src="https://user-images.githubusercontent.com/13206784/219547026-8ec9fb8d-bb8e-4b17-b1ba-c44d34fd0660.png">

including the graph of flip operations.

<img width="420" alt="image" src="https://user-images.githubusercontent.com/13206784/219548006-0d5d4fb6-c3a7-4632-9be0-49e744dc966e.png">
