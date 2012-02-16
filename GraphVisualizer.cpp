/*************************************************************************
 * File: GraphVisualizer.cpp (PC/Mac Version)
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of the GraphVisualizer.h interface.  You should not need
 * to modify the contents of this file.
 *
 * Comments have been added to relevant sections.
 */

#include "GraphVisualizer.h"
#include "graphics.h"
#include "extgraph.h"
#include <limits>     // For numeric_limits
#include <algorithm>  // For min, max
using namespace std;

/* A struct containing information about the viewport. */
struct Viewport {
	double minX, minY, maxX, maxY;
	double width, height;
};

/* Clears the display by drawing a large, white rectangle over the display
 * window.
 */
void ClearDisplay() {
	/* Draw a large, white rectangle over everything. */
	SetPenColor("White");
	StartFilledRegion(1.0);

	/* Trace the outline. */
	MovePen(0, 0);
	DrawLine(GetWindowWidth(), 0);
	DrawLine(0, GetWindowHeight());
	DrawLine(-GetWindowWidth(), 0);
	DrawLine(0, -GetWindowHeight());

	/* Complete the region. */
	EndFilledRegion();
}

/* Given a graph, returns a Viewport that can see that graph. */
Viewport ComputeViewport(SimpleGraph& graph) {
	Viewport result;
	result.minX = result.minY = numeric_limits<double>::max();
	result.maxX = result.maxY = -numeric_limits<double>::max();

	/* Find the minimum and maximum X and Y values. */
	for (size_t i = 0; i < graph.nodes.size(); ++i) {
		result.minX = min(result.minX, graph.nodes[i].x);
		result.minY = min(result.minY, graph.nodes[i].y);
		result.maxX = max(result.maxX, graph.nodes[i].x);
		result.maxY = max(result.maxY, graph.nodes[i].y);
	}

	/* Cache the width and height. */
	result.width = GetWindowWidth();
	result.height = GetWindowHeight();

	return result;
}

/* A constant controlling the percent of the viewport dimension to dedicate
 * on each side to padding.  Increasing this value adds more of a margin to
 * the screen.
 */
const double kMarginPaddingPercent = 0.025;

/* Given an X or Y coordinate, the minimum and maximum values for the coordinate,
 * and the scale factor (i.e. the maximum possible X or Y value in the display),
 * scales the coordinate so that it fits in the display with the proper padding.
 */
double TransformCoordinate(double pt, double min, double max, double scaleMax) {
	/* Rescale so that we go from [0, max - min] instead of from [min, max],
	 * then multiply by the conversion factor scaleMax / (max - min).  Also,
	 * we want to pad this value by a small margin on each side, so we take the
	 * resulting coordinate, scale it down by (1 - 2a), then add a * scaleMax.
	 */
	return (1 - 2 * kMarginPaddingPercent) * (pt - min) * scaleMax / (max - min) +
		   kMarginPaddingPercent * scaleMax;
}

/* Transforms a global X coordinate to a graphics X coordinate. */
double TransformX(double x, Viewport& viewport) {
	return TransformCoordinate(x, viewport.minX, viewport.maxX, viewport.width);
}

/* Transforms a global Y coordinate to a graphics Y coordinate. */
double TransformY(double x, Viewport& viewport) {
	return TransformCoordinate(x, viewport.minY, viewport.maxY, viewport.height);
}

/* Draws all of the arcs. */
void DrawArcs(SimpleGraph& graph, Viewport& viewport) {
	SetPenColor("Black");

	for (size_t i = 0; i < graph.edges.size(); ++i) {
		/* Figure out where all of the points should lie. */
		const double startX = TransformX(graph.nodes[graph.edges[i].start].x, viewport);
		const double startY = TransformY(graph.nodes[graph.edges[i].start].y, viewport);
		const double endX   = TransformX(graph.nodes[graph.edges[i].end].x, viewport);
		const double endY   = TransformY(graph.nodes[graph.edges[i].end].y, viewport);

		/* Draw a line connecting them. */
		MovePen(startX, startY);
		DrawLine(endX - startX, endY - startY);
	}
}

/* Size of each node when drawn. */
const double kNodeRadius = 1 / 16.0;

/* Draws all of the nodes. */
void DrawNodes(SimpleGraph& graph, Viewport& viewport) {
	SetPenColor("Blue");
	for (size_t i = 0; i < graph.nodes.size(); ++i) {
		StartFilledRegion(1.0);
		MovePen(TransformX(graph.nodes[i].x, viewport) + kNodeRadius,
			    TransformY(graph.nodes[i].y, viewport));
		DrawArc(kNodeRadius, 0, 360);
		EndFilledRegion();
	}
}

/* Renders the graph based on the x and y coordinates of its points. */
void DrawGraph(SimpleGraph& graph) {
	/* Clear the screen so we don't clutter up the display. */
	ClearDisplay();

	/* Figure out the maximum resolution in each direction. */
	Viewport viewport = ComputeViewport(graph);

	/* Draw all of the lines between nodes. */
	DrawArcs(graph, viewport);

	/* Draw all of the nodes on top of those lines. */
	DrawNodes(graph, viewport);

	/* Draw everything so it renders correctly. */
	UpdateDisplay();
}

/* Initializing this module just sets up the graphics window to a comfortable size. */
void InitGraphVisualizer() {
	SetWindowSize(4, 4);
	InitGraphics();
}
