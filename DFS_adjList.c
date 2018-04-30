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
	struct vertex* transposeList;
}VGraph;

int timestamp = 0; //global timestamp
int timestamp_t = 0; //global timestamp for transposed graph
char * timestack = NULL;
char ** scc_stack = NULL;
int scc_cnt = 0, scc_elem = 0;
int top = 0; //global "top" for stack

FILE * readFile();
VGraph* makeAdjList(FILE * fp);
void addNode(Vertex* adjList, char v);
void DFS(VGraph * graph);
void DFS_t(VGraph * graph);
void DFS_visit(VGraph* graph, char c);
void DFS_visit_t(VGraph* graph, char c);
void printTime(Vertex* adjList, int len);
char** transpose(char Mat[][50], int len);
void printSCC();
void printadjListT(VGraph* graph, int row);
void printadjList(VGraph* graph, int row);
void free_all(VGraph * graph);

//Main
int main(int argc, char * argv[])
{
	FILE *fp = NULL;
	VGraph* graph = NULL;
	int i = 0;

	fp = readFile();
	graph = makeAdjList(fp);
	DFS(graph);
	printadjList(graph, graph->numVertex);
	printf("\n\n[ Original adjacency list (Time) ]\n");
	printTime(graph->adjList, graph->numVertex);
	DFS_t(graph);
	printadjListT(graph, graph->numVertex);
	printf("\n\n[ Transposed adjacency list (Time) ]\n");
	printTime(graph->transposeList, graph->numVertex);
	printSCC();


	free_all(graph);
	fclose(fp);
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
	char str[100][100];
	const char* delim = "\t\n ";
	char *tok = NULL;
	VGraph* graph = NULL;

	int i = 0, j = 1;
	char adjMat[50][50] = { '\0' }; //Allocate 2d array for adjacent matrix
	char ** transMat = NULL;

	while (!feof(fp)) {
		fgets(str[row], sizeof(str), fp);
		char * pos;

		if ((pos = strchr(str[row], '\n')) != NULL) {//remove the trailing '\n'
			*pos = '\0';
		}

		//Tokenize the received file and create an ajdacency matrix
		tok = strtok(str[row], delim);
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
	
	transMat = transpose(adjMat, len); //transpose the adjacency matrix

	graph = (VGraph*)malloc(sizeof(VGraph)); //Adj array pointer
	graph->numVertex = len; //number of vertices
	graph->adjList = (Vertex*)malloc(sizeof(Vertex) * len); //array for lists
	graph->transposeList = (Vertex*)malloc(sizeof(Vertex) * len); //array for transposed lists

	memset(graph->adjList, '\0', sizeof(Vertex) * len);
	memset(graph->transposeList, '\0', sizeof(Vertex) * len);

	timestack = (char*)malloc(sizeof(char) * len); //allocate stack for each vertex's finished times
	scc_stack = (char**)malloc(sizeof(char*) * len); //allocate stack to store the SCCs
	for (i = 0; i < len; i++) {
		scc_stack[i] = (char*)malloc(sizeof(char) * len);
		memset(scc_stack[i], '\0', sizeof(char) * len);
	}

	for (i = 1; i < row + 1; i++) {
		graph->adjList[i - 1].c = adjMat[i][0]; //copy each vertex to adjList array
		graph->transposeList[i - 1].c = transMat[i][0]; //copy each vertex to transposed list
	}

	for (i = 1; i < row + 1; i++) {
		for (j = 1; j < col + 1; j++) {
			if (adjMat[i][j] == '1') { 
				addNode(&(graph->adjList[i-1]), adjMat[0][j]); //add a node to the adjacency list
			}
			if (transMat[i][j] == '1') {
				addNode(&(graph->transposeList[i - 1]), transMat[0][j]); //add a node to the transposed adj list
			}
		}
	}

	for (i = 0; i < len + 1; i++) {
		char* cur = transMat[i];
		free(cur);
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
	timestack[top++] = head->c;
}

void printTime(Vertex* adjList, int len) {
	int numVert = len;
	int i = 0;
	printf("\n[start / finish]\n\n");
	for (i = 0; i < numVert; i++) {
		printf("<%c> %d / %d\n", adjList[i].c, adjList[i].s_time, adjList[i].f_time);
	}
}

char** transpose(char Mat[][50], int len) {
	int i = 0, j = 0;
	char **transMat = (char**)malloc(sizeof(char*) * (len+1));
	for (i = 0; i < len + 1; i++) {
		transMat[i] = (char*)malloc(sizeof(char) * (len + 1));
		memset(transMat[i], '\0', sizeof(char) * (len + 1));
	}

	for (i =0; i < len+1; i++) {
		for (j = 0; j < len+1; j++) {
			if (i == 0 || j == 0)
				transMat[i][j] = Mat[i][j];
			else
				transMat[j][i] = Mat[i][j];
		}
	}
	return transMat;
}

void DFS_t(VGraph * graph) {
	int numVert = graph->numVertex;
	int i = 0, j = 0, k = 0;
	for (i = 0; i < numVert; i++) {
		graph->transposeList[i].color = WHITE;
	}
	timestamp_t = 0;

	for (i = top - 1; i >= 0; i--) {
		for (j = 0; j < numVert; j++) {
			if (graph->transposeList[j].color == WHITE && graph->transposeList[j].c == timestack[i]) {
				DFS_visit_t(graph, graph->transposeList[j].c);
				scc_cnt++;
				scc_elem = 0;
			}
		}
	}
}

void DFS_visit_t(VGraph* graph, char c) {
	int i = 0, idx = 0;
	int numVert = graph->numVertex;
	Vertex *cur = NULL, *head = NULL;
	for (i = 0; i < numVert; i++) {
		if (graph->transposeList[i].c == c) {
			idx = i;
			break;
		}
	}
	graph->transposeList[idx].color = GRAY;
	timestamp_t += 1;
	graph->transposeList[idx].s_time = timestamp_t;
	cur = &(graph->transposeList[idx]);
	head = cur; //keep the head of the linked list in the adj array

	while (cur != NULL) {
		for (i = 0; i < numVert; i++) {
			if (cur->c == graph->transposeList[i].c) {
				idx = i;
				break;
			}
		}
		if (graph->transposeList[idx].color == WHITE)
			DFS_visit_t(graph, graph->transposeList[idx].c);
		cur = cur->next;
	}
	head->color = BLACK;
	timestamp_t += 1;
	head->f_time = timestamp_t;

	scc_stack[scc_cnt][scc_elem++] = head->c;
}

void printSCC() {
	printf("\n\n< Strongly-Connected Component Result >\n\n");
	int i = 0, j = 0;
	for (i = 0; i < scc_cnt; i++) {
		printf("SCC%d: ", i+1);
		for (j = scc_cnt; j >= 0; j--){
			if (scc_stack[i][j] == '\0')
				continue;
			printf("%c ", scc_stack[i][j]);
		}
		printf("\n\n");
	}
}

void printadjListT(VGraph* graph, int row) {
	int i = 0;
	printf("\n\n< Transposed adjacency list >\n\n");
	for (i = 0; i < row; i++) { //print the transposed adjacency list
		Vertex * cur = &(graph->transposeList[i]);
		while (cur != NULL) {
			printf("%c -> ", cur->c);
			cur = cur->next;
		}
		printf("[NULL]\n");
	}
}

void printadjList(VGraph* graph, int row) {
	int i = 0;
	printf("\n< Original adjacency list >\n\n");
	for (i = 0; i < row; i++) { //print the adjacency list
		Vertex * cur = &(graph->adjList[i]);
		while (cur != NULL) {
			printf("%c -> ", cur->c);
			cur = cur->next;
		}
		printf("[NULL]\n");
	}
}

void free_all(VGraph * graph) {
	int i = 0;
	Vertex* cur = NULL, *prev = NULL;
	for (i = 0; i < graph->numVertex; i++) {
		char* cur = scc_stack[i];
		free(cur);
	}
	free(scc_stack);
	for (i = 0; i < graph->numVertex; i++) {
		cur = &(graph->adjList[i]);
		cur = cur->next;
		while (cur != NULL) {
			prev = cur->next;
			free(cur);
			cur = prev;
		}
	}
	free(graph->adjList);

	for (i = 0; i < graph->numVertex; i++) {
		cur = &(graph->transposeList[i]);
		cur = cur->next;
		while (cur != NULL) {
			prev = cur->next;
			free(cur);
			cur = prev;
		}
	}
	free(graph->transposeList);
	free(timestack);
	free(graph);
}