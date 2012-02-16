/******************************************************
 * File: main.cpp
 * Author: John Yang-Sammataro
 *
 * This is the main.ccp implementation of GraphViz.  
 * The program prompts the user for a file that contains 
 * the information for a simple graph of connected nodes.
 * It uses this information to create a graph of the nodes.
 * It then prompts the user for the number of seconds to run 
 * a force algorithm on the graph.  This algorithm calculates
 * repulsive and attractive forces based on the Fruchterman-
 * Reingold algorithm to make an aesthetically pleasing graph.
 * It then asks the user if they want to try another graph.
 *
 * References: CS106L coursereader was referenced for Getline,
 * GetInteger functions, and the force algorithm.
 */

/* Include libraries and header files */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <ctime>
#include "SimpleGraph.h"
#include "GraphVisualizer.h"
using namespace std;

/* Constants */
const double kPi = 3.14159265358979323;
const double kRepel = 10e-3;
const double kAttract = 10e-3;

/* Function prototypes */
void Welcome();
string GetLine();
int GetInteger();
int GetPositiveInteger();
string PromptForFileName();
int PromptForTime();
Node CreateInitialNode(size_t nodeNumber, size_t totalNumberOfNodes);
SimpleGraph LoadGraph();
double GetElapsedTime(time_t startTime);
double CalculateFRepel(double x0, double x1, double y0, double y1);
double CalculateFAttract(double x0, double x1, double y0, double y1);
double CalculateRadiansAngle(double x0, double x1, double y0, double y1);
double CalculateXForce(double fRepel, double radiansAngle);
double CalculateYForce(double fRepel, double radiansAngle);
void CalculateRepulsiveForces(SimpleGraph& graph, vector<Node>& nodeChanges);
vector<Node> InitializeNodeChanges(SimpleGraph& graph);
void CalculateAttractiveForces(SimpleGraph& graph, vector<Node>& nodeChanges);
void UpdateNodeMovements(SimpleGraph& graph, vector<Node>& nodeChanges);

/* Functions */

/*
 * Welcome
 * Writes a welcome message to the console.
 */
void Welcome() {
	cout << "Welcome to CS106L GraphViz!" << endl;
	cout << "This program uses a force-directed graph layout algorithm" << endl;
	cout << "to render sleek, snazzy pictures of various graphs." << endl;
	cout << endl;
}

/* 
 * GetLine:
 * Takes in a line from the user and returns a string.
 * Safer than just using a getline(cin, str); function
 */
string GetLine() {
    string buffer;
    getline(cin, buffer);
    return buffer;
}

/* 
 * GetInteger:
 * Takes in an integer and prompts until a valid integer is inputed
 * by the user.
 */
int GetInteger() {
    while(true) {
        stringstream converter;
        converter << GetLine();
        int resultInt;
        /* Check if an int is entered correctly */
        if(converter >> resultInt) {
            /*Check for extraneous input */
            char remaining;
            if(converter >> remaining) {
                cout << "Unexpected character: " << remaining << endl;
            } else {
                    return resultInt;
            }
        } else {
            cout << "Please enter an integer." << endl;
        }
        cout << "Retry: ";
    }    
}

/*
 * GetPositiveInteger
 * Prompts the user until they enter
 * a positive integer and returns it.
 */

int GetPositiveInteger() {
    while(true) {
        int integer = GetInteger();
        if(integer > 0) {
            return integer;
        } else {
            cout << "Not a positive integer." << endl;
            cout << "Please enter a positive integer: ";
        }
    }
}

/*
 * PromptForFile
 * Prompts the user for a file name until they enter an 
 * existing file then returns the name of that file as a string.
 */
string PromptForFileName() {
    while(true) {
        cout << "Please enter a graph file to import: ";
        string fileName = GetLine();
        //Need to convert strings to C strings for file reading in C++
        ifstream graphFile(fileName.c_str());
        if (graphFile.is_open()) {
            graphFile.close();
            return fileName;
        } else {
            cout << fileName << " is an invalid file name." << endl;
        }
    }
}

/*
 * LoadGraph
 * Loads a graph specified by the user and returns the
 * loaded graph.
 */
SimpleGraph LoadGraph() {
    //Initialize graph
    SimpleGraph graph;
    
    //Prompt user for graph
    ifstream graphFileStream (PromptForFileName().c_str());
    
    //Get the number of nodes
    size_t numberOfNodes;
    graphFileStream >> numberOfNodes;
    
    //Error checking
    if(graphFileStream.fail()) {
        cout << "Corrupted file on number of nodes." << endl;
        return graph;
    }
    
    //Add the nodes with unit circle positions to the graph
    for(size_t n = 0; n < numberOfNodes; n++) {
        Node graphNode = CreateInitialNode(n, numberOfNodes);
        graph.nodes.push_back(graphNode);
    }
    
    size_t start, end;
    while(graphFileStream >> start >> end) {
        Edge graphEdge;
        graphEdge.start = start;
        graphEdge.end = end;
        graph.edges.push_back(graphEdge);
    }

#if 0
    //*****USE THE ABOVE OR A LOOP AND A HALF RULE AS DESCRIBED 
    //BELOW FOR FILE READING*****
     
    //Add edges to the graph
    while (true) {
        Edge graphEdge;
        graphFileStream >> graphEdge.start;
        //Check for end of file
        if (graphFileStream.fail()) {
            graphFileStream.close();
            break;
        }
        graphFileStream >> graphEdge.end;
        //Error checking
        if (graphFileStream.fail()) {
            cout << "Corrupted file on edges." << endl;
            break;
        }
        graph.edges.push_back(graphEdge);
        
    }
#endif
    
    return graph;
}
                          
/*
 * CreateInitialNode
 * Creates a node with a position on the unit circle
 * based on the node number and the total number of
 * of nodes in the simple graph.
 */
Node CreateInitialNode(size_t nodeNumber, size_t totalNumberOfNodes) {
    Node node;
    node.x = cos(2 * kPi * (double) nodeNumber / (double) totalNumberOfNodes);
    node.y = sin(2 * kPi * (double) nodeNumber / (double) totalNumberOfNodes);
    return node;

}

/* 
 * PromptForTime 
 * Prompts the user for time and integer
 * (GetInteger wrapper with a prompt)
 */
int PromptForTime () {
    cout << "Enter an integer number of seconds to run the algorithm: ";
    return GetPositiveInteger();
}

/*
 * GetElapsedTime
 * Takes in a startTime and returns the time
 * elapsed since that point.
 */
double GetElapsedTime(time_t startTime) {
    return difftime(time(NULL), startTime);
}

/*
 * TransformGraph
 * Takes in a graph by reference, calculates the repulsive and
 * attractive forces acting on the nodes in the graph, and
 * then updates node positions accordingly.
 */
void TransformGraph(SimpleGraph& graph) {
    vector<Node> nodeChanges = InitializeNodeChanges(graph);
    CalculateRepulsiveForces(graph, nodeChanges);
    CalculateAttractiveForces(graph, nodeChanges);
    UpdateNodeMovements(graph, nodeChanges);
}

/*
 * InitializeNodeChanges
 * Takes in a graph by referenceand initializes and returns an 
 * nodeChange vector according to the number of nodes in the graph.
 * The node change vector is a vector holding a number of Node
 * structs equal to the number of nodes in the graph.  Each node
 * struct has an x and y which are 0.
 */
vector<Node> InitializeNodeChanges(SimpleGraph& graph) {
    vector<Node> nodeChanges;
    for (size_t index = 0; index < graph.nodes.size(); index++) {
        Node node;
        node.x = 0;
        node.y = 0;
        nodeChanges.push_back(node);
    }
    return nodeChanges;
}

/*
 * CalculateRepulsiveForces
 * Takes in a simple graph and a vector of Node structs both by
 * by reference. Calculates the repulsive forces on the nodes 
 * and stores them in the vector of nodes.
 */
void CalculateRepulsiveForces(SimpleGraph& graph, vector<Node>& nodeChanges) {
    for (size_t nodeIndex0 = 0; nodeIndex0 < graph.nodes.size() - 1; nodeIndex0++) {
        for (size_t nodeIndex1 = nodeIndex0 + 1; nodeIndex1 < graph.nodes.size(); nodeIndex1++) {
            
            //Get node positions
            double x0 = graph.nodes[nodeIndex0].x;
            double x1 = graph.nodes[nodeIndex1].x;
            double y0 = graph.nodes[nodeIndex0].y;
            double y1 = graph.nodes[nodeIndex1].y;
            
            //Calculate repulsive force and angle between the two nodes
            double fRepel = CalculateFRepel(x0, x1, y0, y1);
            double radiansAngle = CalculateRadiansAngle(x0, x1, y0, y1);
            
            //Update the x and y changes based on the forces.
            nodeChanges[nodeIndex0].x -= CalculateXForce(fRepel, radiansAngle);
            nodeChanges[nodeIndex0].y -= CalculateYForce(fRepel, radiansAngle);
            nodeChanges[nodeIndex1].x += CalculateXForce(fRepel, radiansAngle);
            nodeChanges[nodeIndex1].y += CalculateYForce(fRepel, radiansAngle);
        }
    }
}


/*
 * CalculateAttractiveForces
 * Takes in a graph and vector of changes by reference
 * Calculates the attractive forces and updates the
 * vector of node changes appropriately.
 */
void CalculateAttractiveForces(SimpleGraph& graph, vector<Node>& nodeChanges) {
    for (size_t edgeIndex = 0; edgeIndex < graph.edges.size(); edgeIndex++) {
        
        //Get nodes
        Node node0 = graph.nodes[graph.edges[edgeIndex].start];
        Node node1 = graph.nodes[graph.edges[edgeIndex].end];
        
        //Get node positions
        double x0 = node0.x;
        double x1 = node1.x;
        double y0 = node0.y;
        double y1 = node1.y;
        
        //Calculate attractive force and radian angle
        double fAttract = CalculateFAttract(x0, x1, y0, y1);
        double radiansAngle = CalculateRadiansAngle(x0, x1, y0, y1);
        
        //Update x and y changes based on graph
        nodeChanges[graph.edges[edgeIndex].start].x += CalculateXForce(fAttract, radiansAngle);
        nodeChanges[graph.edges[edgeIndex].start].y += CalculateYForce(fAttract, radiansAngle);
        nodeChanges[graph.edges[edgeIndex].end].x   -= CalculateXForce(fAttract, radiansAngle);
        nodeChanges[graph.edges[edgeIndex].end].y   -= CalculateYForce(fAttract, radiansAngle);
    }
}

/*
 * CalculateFRepel
 * Calculates and returns the repeling force from the
 * input of four double coordinates of two node locations
 */
double CalculateFRepel(double x0, double x1, double y0, double y1) {
    return kRepel / sqrt( (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

/*
 * CalculateFAttract
 * Calculates and returns the attracting force from the
 * input of four double coordinates of two node locations
 */
double CalculateFAttract(double x0, double x1, double y0, double y1) {
    return kAttract * ((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

/*
 * CalculateRadiansAngle
 * Calculates and returns the radians angle between nodes with
 * two different positions
 */
double CalculateRadiansAngle(double x0, double x1, double y0, double y1) {
    return atan2 (y1-y0, x1-x0);
}
                                         
/*
 * CalculateXForce
 * Takes in a force and an angle in radians and computes the appropriate
 * x component of that force.
 */
double CalculateXForce(double fRepel, double radiansAngle){
    return fRepel * cos(radiansAngle);
}

/*
 * CalculateYForce
 * Takes in a force and an angle in radians and computes the appropriate
 * Y component of that force.
 */
double CalculateYForce(double fRepel, double radiansAngle){
    return fRepel * sin(radiansAngle);
}

void UpdateNodeMovements(SimpleGraph& graph, vector<Node>& nodeChanges) {
    for(size_t nodeIndex = 0; nodeIndex < graph.nodes.size(); nodeIndex++) {
       //Update the node positions
        graph.nodes[nodeIndex].x +=nodeChanges[nodeIndex].x;
        graph.nodes[nodeIndex].y +=nodeChanges[nodeIndex].y;
        //Reset the node changes
        nodeChanges[nodeIndex].x = 0;
        nodeChanges[nodeIndex].y = 0;
    }
}

/* Main function */

int main() {
	Welcome();
    InitGraphVisualizer();
    
    do {
        //Load graph
        SimpleGraph graph = LoadGraph();
        DrawGraph(graph);
        //Get algorithm time
        int algorithmTime = PromptForTime();
    
        //Start transformation
        time_t startTime = time(NULL);
        while (true) {
            TransformGraph(graph);
            DrawGraph(graph);
            if(GetElapsedTime(startTime) > algorithmTime) break;
        }
        
        //Allow for multiple graphs
        cout << "Type \"yes\" and hit ENTER to load a new graph or press ENTER to finish the program: ";
    } while (GetLine() == "yes");
    return 0;
}
