#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WHITE 'w'
#define BLACK 'b'
#define GRAY 'g'

typedef struct vertex{
	char c;
	char color;
	int s_time;
	int f_time;
	struct vertex* next;
}Vertex;

typedef struct vgraph{
	int numVertex;
	struct vertex* adjList;
}VGraph;

int timestamp = 0; //global timestamp

FILE * readFile();
VGraph* makeAdjList(FILE * fp);
void addNode(Vertex* adjList, char v);
void DFS(VGraph * graph);
void DFS_visit(VGraph* graph, char c);
void printTime(VGraph* graph);

//Main
int main(int argc, char * argv[])
{
	FILE *fp = NULL;
	VGraph* graph = NULL;

	fp = readFile();
	graph = makeAdjList(fp);
	DFS(graph);
	printTime(graph);



	fclose(fp);
	free(graph);
}


//Functions
FILE * readFile() {
	int c = 0;
	char str[20];
	FILE *fp = NULL;

	while (!fp) {
		printf("Enter the file name: ");
		if (fgets(str, sizeof(str), stdin) == NULL)
			fprintf(stderr, "Error. NULL received\n");
		else {
			char * pos;
			if ((pos = strchr(str, '\n')) != NULL) {//remove the trailing '\n'
				*pos = '\0';
			}
		}
		fp = fopen(str, "r");

		if (!fp) {
			perror("File open failed ");
			continue;
		}
		else {
			printf("<File open successful>\n");
			break;
		}
	}

	return fp;
}

VGraph* makeAdjList(FILE * fp) {
	int len = 0;
	int row = 0, col = 0;
	char * str[100];
	const char* delim = "\t\n ";
	char *tok = NULL;
	VGraph* graph = NULL;

	int i = 0, j = 1;
	char adjMat[50][50] = { '\0' }; //Allocate 2d array for adjacent matrix

	while (!feof(fp)) {
		fgets(&str[row], sizeof(str), fp);
		char * pos;
		if ((pos = strchr(&str[row], '\n')) != NULL) {//remove the trailing '\n'
			*pos = '\0';
		}

		//Tokenize the received file and create an ajdacency matrix
		tok = strtok(&str[row], delim);
		while (tok != NULL) {
			adjMat[i][j] = tok[0]; //Store the tokens in adjacent matrix
			tok = strtok(NULL, delim);
			if (row == 0)
				col++; //Counting the number of columns of adjacent matrix
			j++;
		}

		row++;
		j = 0;
		i++;
	}
	row -= 1;
	len = row;

	printf("\n# of Vertices: %d\n", len);

	for (i = 0; i < row+1; i++) {
		for (j = 0; j < col+1; j++) {
			printf("%c ", adjMat[i][j]);
		}
		printf("\n");
	}
	printf("\n");

	graph = (VGraph*)malloc(sizeof(VGraph)); //Adj array pointer
	graph->numVertex = len; //number of vertices
	graph->adjList = (Vertex*)malloc(sizeof(Vertex) * len); //array for lists
	memset(graph->adjList, '\0', sizeof(Vertex) * len);

	for (i = 1; i < row + 1; i++) {
		graph->adjList[i - 1].c = adjMat[i][0]; //copy each vertex to adjList array
	}

	for (i = 1; i < row + 1; i++) {
		for (j = 1; j < col + 1; j++) {
			if (adjMat[i][j] == '1') { //If adjacent
				addNode(&(graph->adjList[i-1]), adjMat[0][j]); //add a node to the adjacency list
			}
		}
	}
	for (i = 0; i < row; i++) { //print the adjacency list
		Vertex * cur = &(graph->adjList[i]);
		while (cur != NULL) {
			printf("%c -> ", cur->c);
			cur = cur->next;
		}
		printf("[NULL]\n");
	}

	return graph;
}

void addNode(Vertex* adjList, char v) {
	Vertex * newNode = (Vertex*)malloc(sizeof(Vertex));
	Vertex * cur = adjList;
	newNode->c = v;
	newNode->next = NULL;

	if (cur->next == NULL) { //linked list is empty
		adjList->next = newNode;
	}
	else {
		while (cur->next != NULL) {//linked list is not empty
			cur = cur->next;
		}
		cur->next = newNode;
	}
}

void DFS(VGraph * graph) {
	int numVert = graph->numVertex;
	int i = 0;
	for (i = 0; i < numVert; i++) {
		graph->adjList[i].color = WHITE;
	}
	timestamp = 0;
	for (i = 0; i < numVert; i++) {
		if (graph->adjList[i].color == WHITE)
			DFS_visit(graph, graph->adjList[i].c);
	}

}

void DFS_visit(VGraph* graph, char c) {
	int i = 0, idx = 0;
	int numVert = graph->numVertex;
	Vertex *cur = NULL, *head = NULL;
	for (i = 0; i < numVert; i++) {
		if (graph->adjList[i].c == c) {
			idx = i;
			break;
		}
	}
	graph->adjList[idx].color = GRAY;
	timestamp += 1;
	graph->adjList[idx].s_time = timestamp;
	cur = &(graph->adjList[idx]);
	head = cur; //keep the head of the linked list in the adj array

	while (cur != NULL) {
		for (i = 0; i < numVert; i++) {
			if (cur->c == graph->adjList[i].c) {
				idx = i;
				break;
			}
		}
		if(graph->adjList[idx].color == WHITE)
			DFS_visit(graph, graph->adjList[idx].c);
		cur = cur->next;
	}
	head->color = BLACK;
	timestamp += 1;
	head->f_time = timestamp;
}

void printTime(VGraph* graph) {
	int numVert = graph->numVertex;
	int i = 0;
	printf("\n[start / finish]\n");
	for (i = 0; i < numVert; i++) {
		printf("<%c> %d / %d\n", graph->adjList[i].c, graph->adjList[i].s_time, graph->adjList[i].f_time);
	}
}