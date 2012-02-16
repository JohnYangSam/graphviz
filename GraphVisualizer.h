/*************************************************************************
 * File: GraphVisualizer.h (Mac Version)
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * A header file defining a set of functions which can be used to
 * visualize a simple graph.  To initialize the visualizer, you should
 * call the function InitGraphVisualizer() to set up internal state.  You
 * can then invoke DrawGraph() to have the graph visualizer render the
 * graph.
 *
 * The graph visualizer works by computing the size of the bounding
 * rectangle which holds the entire graph, then drawing all the nodes and
 * edges scaled by the size of the bounding rectangle.  In this way, no
 * matter how much space is used by the graph, it always renders in the
 * same window.
 */

#ifndef GraphVisualizer_Included // Include guard
#define GraphVisualizer_Included

#include "SimpleGraph.h" // For the SimpleGraph type.

/**
 * Function: InitGraphVisualizer()
 * -----------------------------------------------------------------------
 * Initializes the internal state used by the graph visualizer.  You
 * should call this function exactly once in your program and before any
 * calls to DrawGraph.
 */
void InitGraphVisualizer();

/**
 * Function: DrawGraph(SimpleGraph& graph)
 * -----------------------------------------------------------------------
 * Draws the specified graph.  This function will only work if you have
 * made a previous call to InitGraphVisualizer().
 */
void DrawGraph(SimpleGraph& graph);

/**
 * Redefinition: main
 * -----------------------------------------------------------------------
 * Due to a quirk in the way that the CS106B/X graphics package works,
 * we need to redefine the main function to be something other than main.
 * This is unsightly, but regrettably the workaround is necessary in order
 * for the visualizer to work.
 */
#define main Main

#endif