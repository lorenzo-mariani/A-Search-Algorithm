#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define ROW 7 	// the map must be consistent wrt ROW, COL
#define COL 8
#define CONNECTIVITY 8	// it must be 4 or 8
#define ALLOC 5		// allocazione dinamica iniziale dei vettori

typedef struct {
	int row, col;	// Row and column of a cell
	double f;	// Total cost of the cell
	double g;	// Distance between the current cell and the starting point
	double h;	// Estimated distance between the current cell and the goal point
	int parentRow, parentCol;	// Row and column of the parent cell 
} Cell;

// Check if the current cell is inside the map
bool check_position (int cell[]) {
	if (cell[0] >= 0 && cell[0] < ROW && cell[1] >= 0 && cell[1] < COL)
		return true;
	else
		return false; 
}

// Check if the goal point has been reached
bool is_goal (int start[], int goal[]) {
	if (start[0] == goal[0] && start[1] == goal[1])
		return true;
	else
		return false;
}

// Check if the current cell is free or if there is an obstacle
bool is_free (int cell[], bool map[][COL]) {
	if (map[cell[0]][cell[1]] == true)
		return true;
	else
		return false;
}

// Check the correctness of the start and goal cells
int check (int start[], int goal[], bool map[][COL]) {
	if ((!check_position(start)) || (!check_position(goal))) {
		printf("Start/goal point out of the map\n");
		return 0;
	}

	if ((!is_free(start, map)) || (!is_free(goal, map))) {
		printf("Start/goal point not reachable\n");
		return 0;
	}

	if (is_goal(start, goal)) {
		printf("Start and goal points are the same\n");
		return 0;
	}
	return 1;
}

// Computation of the heuristic
double heuristic (Cell a, Cell b) {
	
	double distance;

	if (CONNECTIVITY == 4) {
		distance = ((double)((abs(a.row - b.row))+(abs(a.col - b.col))));	// Manhattan distance
	}
	else if (CONNECTIVITY == 8) {
		distance = ((double)sqrt((a.row - b.row)*(a.row - b.row) + (a.col - b.col)*(a.col - b.col)));	// Euclidean distance
	}  
	
	return distance;
}

// Check if a neighbor is valid 
bool check_a_neighbor(int deltaRow, int deltaCol, int cell[], bool map[][COL]){
	cell[0] += deltaRow;
	cell[1] += deltaCol;
	if((check_position(cell)) && (is_free(cell, map)))
		return true;
	else
		return false;
}

// Function to find the shortest path between the starting point and the goal point
void search (bool map[][COL], int start[], int goal[]) {

	Cell arrayCells[ROW*COL];	// Array containining the details of each cell
	
	// Initialization of each cell
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			int pos = i*ROW + j;
			arrayCells[pos].row = i;
			arrayCells[pos].col = j;
			arrayCells[pos].f = 100000.0;	// very high number
			arrayCells[pos].g = 100000.0;	// very high number
			arrayCells[pos].h = 100000.0;	// very high number
			arrayCells[pos].parentRow = -1;	// invalid position
			arrayCells[pos].parentCol = -1;	// invalid position
		}
	}

	// Initialization of the starting cell
	int pos_start = start[0]*ROW + start[1];
	arrayCells[pos_start].f = 0.0;
	arrayCells[pos_start].g = 0.0;
	arrayCells[pos_start].h = 0.0;
	arrayCells[pos_start].parentRow = start[0];
	arrayCells[pos_start].parentCol = start[1];

	int *openSet;	// Priority queue
	int allocOpen = ALLOC;
	openSet = (int*)malloc(sizeof(int)*allocOpen);
	openSet[0] = start[0]*ROW + start[1];		// The first cell in the open set is the starting cell
	int openSetSize = 1;

	int *closedSet;	// Cells already visited
	int allocClosed = ALLOC;
	closedSet = (int*)malloc(sizeof(int)*allocClosed);
	int closedSetSize = 0;
	
	bool foundPath = false;
	
	int *path;		// Path between the starting point and the goal point
	int allocPath = ALLOC;
	path = (int*)malloc(sizeof(int)*allocPath);
	int pathSize = 0;
	
	int *bestPath;
	bestPath = (int*)malloc(sizeof(int)*allocPath);
	int bestPathSize = 0;

	while (openSetSize >= 0) {
		
		for (int i = 0; i < openSetSize; i++) {
			printf("\nCella open set: %d %d with f=%f, g=%f, h=%f, con genitore: %d %d...", arrayCells[openSet[i]].row, arrayCells[openSet[i]].col, arrayCells[openSet[i]].f, arrayCells[openSet[i]].g, arrayCells[openSet[i]].h), arrayCells[openSet[i]].parentRow, arrayCells[openSet[i]].parentCol;
		}
		
		int best = 0;	// Initial assumption: the cell having the lowest value of f is in the first position of the open set

		// Scan the open set to find the new best cell
		for (int i = 0; i < openSetSize; i++) {
			if (arrayCells[openSet[i]].f <= arrayCells[openSet[best]].f)
				if (arrayCells[openSet[i]].h < arrayCells[openSet[best]].h)
					best = i;
		}

		int c[2] = {arrayCells[openSet[best]].row, arrayCells[openSet[best]].col};		// c is the current cell
		int posC = c[0]*ROW+c[1];
		printf("\n\n>>> Cella corrente: %d %d, con genitore: %d %d\n", arrayCells[posC].row, arrayCells[posC].col, arrayCells[posC].parentRow, arrayCells[posC].parentCol);

		if (is_goal(c, goal) || openSetSize == 0) {
			
			Cell tmp = arrayCells[posC];	// Temporary cell initialized to the current cell (goal point)
			
			if (is_goal(c, goal)){
						
				path[0] = tmp.row*ROW + tmp.col;	// Adding the cell in the first position of the path
				pathSize = 1;
				
				// These instructions are executed every time a cell has a parent (the loop stops when the starting cell is evaluated, whose parent is the cell itself) 
				while ((tmp.parentRow != -1) && (tmp.row != tmp.parentRow || tmp.col != tmp.parentCol)) {
					
					// eventuale riallocazione vettori Path e BestPath
					if(pathSize >= allocPath){
						allocPath += ALLOC;
						path = (int*)realloc(path, allocPath*sizeof(int));
						bestPath = (int*)realloc(bestPath, allocPath*sizeof(int));
					}
					
					int posTmp = tmp.parentRow*ROW + tmp.parentCol; // Variable used to find the position of the cell within arrayCells 
					tmp = arrayCells[posTmp];	// Temporary cell value updated to the parent cell 
					path[pathSize] = posTmp;	// Adding the parent cell to the path
					pathSize++;
					printf("\nPathsize: %d, aggiunta: %d %d con genitore: %d %d\n", pathSize, arrayCells[posTmp].row, arrayCells[posTmp].col, arrayCells[posTmp].parentRow, arrayCells[posTmp].parentCol);
				}
				
				// update the BestPath if it is the first path I've found OR if the new path is better than the previous one.
				if(!foundPath || (foundPath && arrayCells[path[0]].f < arrayCells[bestPath[0]].f)) {
					for(int i = 0; i < pathSize; i++){
						bestPath[i] = path[i];
					}
					bestPathSize = pathSize;
				}
				
				foundPath = true;
	
				// 
				float pathF = arrayCells[path[0]].f;
				for(int i = 0; i < openSetSize; i++){
					if (arrayCells[openSet[i]].f >= pathF){
						for (int j = i; j < openSetSize; j++){
							
							// eventuale riallocazione vettore ClosedSet
							if(closedSetSize >= allocClosed){
								allocClosed += ALLOC;
								closedSet = (int*)realloc(closedSet, allocClosed*sizeof(int));
							}		
							// aggiungo la cella al ClosedSet
							closedSet[closedSetSize] = openSet[j];
							closedSetSize++;
							
							// rimuovo la cella da OpenSet
							openSet[j] = openSet[j+1];
							openSetSize--;
						}
					}
				}
			}
			
			if (openSetSize == 0){
				printf("Goal reached! The computed path is:\n");
				// Print the path
				for (int j = bestPathSize-1; j >= 0; j--) {
					printf("(%d, %d)\n", arrayCells[bestPath[j]].row, arrayCells[bestPath[j]].col);
				}
				
				/* ATTENZIONE: QUELLO TROVATO POTREBBE NON ESSERE IL PERCORSO MIGLIORE! */
	
				free(openSet);
				free(closedSet);
				free(path);
				free(bestPath);
				return;
			}
			
			
		}
		
		// Removing the current cell from the open set
		for (int i = best; i < openSetSize; i++) {
			openSet[i] = openSet[i + 1];
		}
		openSetSize--;

		// Adding the current cell inside the closed set
		// Eventuale riallocazione vettore ClosedSet
		if(closedSetSize >= allocClosed){
			allocClosed += ALLOC;
			closedSet = (int*)realloc(closedSet, allocClosed*sizeof(int));
		}
		closedSet[closedSetSize] = c[0]*ROW+c[1];
		closedSetSize++;
		
		int numNeighbors = 0;
		int neighbor[2];
		int tmp[CONNECTIVITY];
		
		// For cell "c", each possible neighbor (from high-left to low-right) is checked.
		int deltaRow, deltaCol;
		for (deltaRow=-1; deltaRow<=1; deltaRow++){
			for (deltaCol=-1; deltaCol<=1; deltaCol++){
				if (deltaRow != 0 || deltaCol != 0){	// this excludes the cell itself
					if (CONNECTIVITY == 8 || (deltaRow == 0 || deltaCol == 0)){		// this check the connectivity and works consequently
						if (check_a_neighbor(deltaRow, deltaCol, c, map)) {
							neighbor[0] = c[0] + deltaRow;
							neighbor[1] = c[1] + deltaCol;
							tmp[numNeighbors] = neighbor[0]*ROW + neighbor[1];
							numNeighbors++;
						}
					}
				}
			}
		}
	
		int neighbors[numNeighbors];
		for (int i=0; i<numNeighbors; i++){
			neighbors[i] = tmp[i];
		}
		
		//int neighborSize = sizeof(neighbors)/sizeof(neighbors[0]);
		//printf("\nTot: %d / %d = %d neighbors\n", sizeof(neighbors), sizeof(neighbors[0]), neighborSize);
		
		// Loop for checking every neighbor of the current cell
		for (int i = 0; i < numNeighbors; i++) {
			neighbor[0] = arrayCells[neighbors[i]].row;
			neighbor[1] = arrayCells[neighbors[i]].col;
			printf("   Neighbor: %d %d ", neighbor[0], neighbor[1]);
			
			// Check if the neighbor is already in the closed set.
			// If the neighbor is NOT in the closed set, it is evaluated, otherwise nothing is done.
			for (int j = 0; j < closedSetSize; j++) {
				if ((neighbor[0] != arrayCells[closedSet[j]].row || neighbor[1] != arrayCells[closedSet[j]].col) && (j == closedSetSize - 1)) {
						
					// If I am here, the neighbor is NOT in the closed set
					
					double tmpG = arrayCells[c[0]*ROW+c[1]].g + heuristic(arrayCells[c[0]*ROW+c[1]], arrayCells[neighbor[0]*ROW+neighbor[1]]);
					//printf("distance: %f. ClosedsetSize: %d. OpensetSize: %d\n", tmpG, closedSetSize, openSetSize);
					
					// If I am at the beginning (the cell is the start)
					if(c[0] == start[0] && c[1] == start[1]){

						arrayCells[neighbor[0]*ROW + neighbor[1]].g = tmpG;

						// Inserisco "neighbor" all'interno dell'open set
						// Eventuale riallocazione vettore OpenSet
						if(openSetSize >= allocOpen){
							allocOpen += ALLOC;
							openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
						}
						openSet[openSetSize] = neighbor[0]*ROW + neighbor[1];
						arrayCells[openSet[openSetSize]].h = heuristic(arrayCells[neighbor[0]*ROW+neighbor[1]], arrayCells[goal[0]*ROW+goal[1]]);
						arrayCells[openSet[openSetSize]].f = arrayCells[openSet[openSetSize]].g + arrayCells[openSet[openSetSize]].h;
						openSetSize++;
						printf(", now opensetSize=%d\n", openSetSize);
					}

					// Check if the neighbor is already in the open set. If it is NOT, it means that a new cell was discovered
					else {
						bool incr = false;
						for (int k = 0; k < openSetSize; k++) {
							if (neighbor[0] == arrayCells[openSet[k]].row && neighbor[1] == arrayCells[openSet[k]].col) {

									// If I am here, the neighbor is ALREADY in the open set

									// Check if the neighbor has been reached with a lower cost than before. 
									// If yes, its value of g is updated, otherwise nothing is done 
									if (tmpG < arrayCells[neighbor[0]*ROW + neighbor[1]].g) {
										arrayCells[neighbor[0]*ROW + neighbor[1]].g = tmpG;
										//openSet[k].g = tmpG;
										arrayCells[openSet[k]].h = heuristic(arrayCells[neighbor[0]*ROW+neighbor[1]], arrayCells[goal[0]*ROW+goal[1]]);
										//openSet[k].h = heuristic(neighbor, goal);
										arrayCells[openSet[k]].f = arrayCells[openSet[k]].g + arrayCells[openSet[k]].h;
										//openSet[k].f = openSet[k].g + openSet[k].h;
										//openSet[k].parentRow = c.row;
										//openSet[k].parentCol = c.col;
										arrayCells[neighbor[0]*ROW+neighbor[1]].parentRow = c[0];
										arrayCells[neighbor[0]*ROW+neighbor[1]].parentCol = c[1];
									}
									k = openSetSize;  // esce dal FOR e valuta un nuovo vicino.
													/*  ATTENZIONE: SICURI CHE ESCE SUBITO???  */	
							}
							else if (k == openSetSize - 1) {
								// If I am here, the neighbor is NOT in the open set: new cell discovered

								arrayCells[neighbor[0]*ROW + neighbor[1]].g = tmpG;
								//neighbor.g = tmpG;

								// Inserisco "neighbor" all'interno dell'open set
								// Eventuale riallocazione vettore OpenSet
								if(openSetSize >= allocOpen){
									allocOpen += ALLOC;
									openSet = (int*)realloc(openSet, allocOpen*sizeof(int));
								}
								openSet[openSetSize] = neighbor[0]*ROW + neighbor[1];
								arrayCells[openSet[openSetSize]].h = heuristic(arrayCells[neighbor[0]*ROW+neighbor[1]], arrayCells[goal[0]*ROW+goal[1]]);
								arrayCells[openSet[openSetSize]].f = arrayCells[openSet[openSetSize]].g + arrayCells[openSet[openSetSize]].h;
								incr = true;
								printf(", now opensetSize=%d. closedsetSize=%d\n", openSetSize+1, closedSetSize);
							}
						}
						if (incr)
							openSetSize++;
					}

					arrayCells[neighbor[0]*ROW + neighbor[1]].h = heuristic(arrayCells[neighbor[0]*ROW+neighbor[1]], arrayCells[goal[0]*ROW+goal[1]]);
					arrayCells[neighbor[0]*ROW + neighbor[1]].f = arrayCells[neighbor[0]*ROW + neighbor[1]].g + arrayCells[neighbor[0]*ROW + neighbor[1]].h;
					arrayCells[neighbor[0]*ROW + neighbor[1]].parentRow = c[0];
					arrayCells[neighbor[0]*ROW + neighbor[1]].parentCol = c[1];
				}
				else if (neighbor[0] == arrayCells[closedSet[j]].row && neighbor[1] == arrayCells[closedSet[j]].col) {
					j = closedSetSize;		// per uscire dal ciclo e cominciare con un nuovo neighbor
				}
			}
		}
	}
	
	free(openSet);
	free(closedSet);
	free(path);
	free(bestPath);

	return;
}

int main () {
	
	// false - There is an obstacle
	// true - The cell is free
	bool map[ROW][COL] = 
	{
		{true, true, true, true, true, true, true, false},
		{true, true, false, false, true, true, true, false},
		{true, true, false, false, true, true, true, false},
		{true, false, false, false, true, true, true, false},
		{true, true, false, false, true, true, true, false},
		{true, true, true, true, false, false, true, false},
		{true, false, true, true, true, true, true, false}
	};
	
	int start[] = {6, 2};
	int goal[] = {1, 6};
	
	if (check(start, goal, map) == 1) {
		// Execute the algorithm
		search(map, start, goal);
	}
	else {
		return 0;
	}

	return 1;
}
