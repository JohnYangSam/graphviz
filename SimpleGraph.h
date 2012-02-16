/*************************************************************************
 * File: SimpleGraph.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * A header file defining a simple graph that has a collection of nodes
 * storing their position in two-dimensional space, along with a list of
 * edges linking up various edges.  The nodes are stored in a vector, and
 * edges identify their endpoints by describing the indices of the linked
 * nodes in the vector.
 */

#ifndef SimpleGraph_Included // Include guard
#define SimpleGraph_Included

/* Include vector so that it's accessible in the graph class. */
#include <vector>
using namespace std;

/**
 * Type: Node
 * -----------------------------------------------------------------------
 * A type representing a node in a graph.  Each node stores only the x and
 * y coordinates of where the node is in the plane; all other information
 * about the node is stored implicitly by its position in the SimpleGraph
 * list of nodes.
 */
struct Node {
	double x, y;
};

/**
 * Type: Edge
 * -----------------------------------------------------------------------
 * A type representing an edge in the graph.  It stores its endpoints by
 * the indices in which they occur in the SimpleGraph's list of Nodes.
 */
struct Edge {
	size_t start, end;
};

/**
 * Type: SimpleGraph
 * -----------------------------------------------------------------------
 * A type representing a simple graph of nodes and edges.
 */
struct SimpleGraph {
	vector<Node> nodes;
	vector<Edge> edges;
};

#endif
