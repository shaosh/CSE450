/*Version Information
//
//Version 1.0
//Version 1.1
//Version 1.2
//	Delete Adjacency list for G, only keep that for Gf.
//	Almost done except the 2nd bonus
//Version 1.3
//	Begin to do bonus 2
//	Treat value and rvalue equally: mainly in findRedge() and updateFlow()
//Version 1.4
//	Done with width part
*/	
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define TRUE 1
#define FALSE 0
#define WHITE 0
#define GRAY 1
#define BLACK 2

struct edge{
	int start; 
	int end;
	int capacity;
};

struct redge{ //r = reverse
	int start; 
	int end;
	int value;
	int rvalue;
};

struct flow{
	int start; 
	int end;
	int value;
};

struct node{
	int num;
	int connected;
	struct node* next;
};

struct vertex{
	int head;
	struct node* nodeList;
	int color;
	int distance;
	struct vertex* parent;
};

struct edge* graph;
struct redge* rgraph;
struct redge* rmaxgraph;
struct flow* currentFlow;
struct vertex* rvertexList;
int edgeNum;
int vertexNum;
int startVertex;
int endVertex;
int knum;
int totalValue;
int maxCapacity;
int maxCount;

int main(int, char**);
void readFile(char*);
struct redge* madeResidual(struct edge*, struct flow*);
struct vertex* bfs();
struct vertex* bfsByWidth(struct redge*);
struct redge** findAugment(struct redge*);
struct redge** findAugmentByWidth(struct redge*);
struct redge* findRedge(struct vertex*, struct vertex*, struct redge*);
struct redge* findRedgeByWidth(struct vertex*, struct vertex*, struct redge*);
struct flow* updateFlow(struct redge**, struct edge*, struct flow*);
int findEdge(int, int, struct edge*);
struct vertex** findCut();
int calCapacity(struct vertex**, int);
void fordfulkerson(struct edge*, struct redge*, struct flow*);
int findRedgeForWidth(int, int, struct redge*);
struct redge* mademaxresidual(struct redge*, int);


//void enqueue(struct vertex**, struct vertex);
//struct vertex* dequeue(struct vertex**);

int main(int argc, char* argv[]){
	int i = 0;
	if (argc != 2){
		printf("Error: invalid commands!");
		return;
	}
	readFile(argv[1]);
	fordfulkerson(graph, rgraph, currentFlow);
}

void readFile(char* filename){
	int i = 0, j = 0;
	struct node* tempNode;
	int tempStart, tempEnd, tempCapacity;
	FILE *file = fopen(filename, "r");
	fscanf(file, "%d", &vertexNum);
	fscanf(file, "%d", &edgeNum);
	fscanf(file, "%d", &startVertex);
	fscanf(file, "%d", &endVertex);
	fscanf(file, "%d", &knum);

	graph = (struct edge*)malloc(sizeof(struct edge) * edgeNum);
	currentFlow = (struct flow*)malloc(sizeof(struct flow) * edgeNum);
	rvertexList = (struct vertex*)malloc(sizeof(struct vertex) * vertexNum);
	totalValue = 0;

	for(i = 0; i < vertexNum; i++){
		rvertexList[i].nodeList = (struct node*)malloc(sizeof(struct node));
		rvertexList[i].head = i + 1;
		rvertexList[i].nodeList->num = 0;
		rvertexList[i].nodeList->next = NULL;
		rvertexList[i].nodeList->connected = -1;
		rvertexList[i].color = WHITE;
		rvertexList[i].distance = -1;
		rvertexList[i].parent = NULL;
	}

	maxCapacity = 0;
	for(i = 0; i < edgeNum; i++){
		fscanf(file, "%d", &tempStart);
		fscanf(file, "%d", &tempEnd);
		fscanf(file, "%d", &tempCapacity);
		graph[i].start = tempStart;
		graph[i].end = tempEnd;
		graph[i].capacity = tempCapacity;
		currentFlow[i].start = tempStart;
		currentFlow[i].end = tempEnd;
		currentFlow[i].value = 0;

		if(tempCapacity > maxCapacity)
			maxCapacity = tempCapacity;
		//Add end node to start node's list
		if(rvertexList[tempStart - 1].nodeList->num == 0){ 
			rvertexList[tempStart - 1].nodeList->num = tempEnd;
			rvertexList[tempStart - 1].nodeList->next = NULL;
			rvertexList[tempStart - 1].nodeList->connected = TRUE;
		}
		else{
			tempNode = (struct node*)malloc(sizeof(struct node));
			tempNode->num = tempEnd;
			rvertexList[tempStart - 1].nodeList->connected = TRUE;
			tempNode->next = rvertexList[tempStart - 1].nodeList;
			rvertexList[tempStart - 1].nodeList = tempNode;
		}

		//Add start node to end node's list
		if(rvertexList[tempEnd - 1].nodeList->num == 0){ 
			rvertexList[tempEnd - 1].nodeList->num = tempStart;
			rvertexList[tempEnd - 1].nodeList->next = NULL;
			rvertexList[tempEnd - 1].nodeList->connected = FALSE;
		}
		else{
			tempNode = (struct node*)malloc(sizeof(struct node));
			tempNode->num = tempStart;
			rvertexList[tempEnd - 1].nodeList->connected = FALSE;
			tempNode->next = rvertexList[tempEnd - 1].nodeList;
			rvertexList[tempEnd - 1].nodeList = tempNode;
		}
	}
	rgraph = madeResidual(graph, currentFlow);
	//********************************Test readFile***********************************
	/*printf("Test: \n");
	printf("%d %d\n", vertexNum, edgeNum);
	printf("%d %d %d\n", startVertex, endVertex, knum);

	printf("Test graph: \n");
	for(i = 0; i < edgeNum; i++){
		printf("%d %d %d\n", graph[i].start, graph[i].end, graph[i].capacity);
	}

	printf("Test flows: \n");
	for(i = 0; i < edgeNum; i++){
		printf("%d %d %d\n", currentFlow[i].start, currentFlow[i].end, currentFlow[i].value);
	}

	printf("Test rgraph: \n");
	for(i = 0; i < edgeNum; i++){
		printf("%d %d %d %d\n", rgraph[i].start, rgraph[i].end, rgraph[i].value, rgraph[i].rvalue);
	}

	printf("Test vertices: \n");
	for(i = 0; i < vertexNum; i++){
		printf("%d: ", i + 1);
		for(tempNode = rvertexList[i].nodeList; tempNode != NULL; tempNode = tempNode->next){
			printf("%d: %d --> ", tempNode->num, tempNode->connected);
		}
		printf("\n");
	}*/
	//********************************Test readFile end***********************************
}

struct redge* madeResidual(struct edge* graph, struct flow* currentFlow){
	int i = 0, j = 0;
	struct node* tempNode;
	struct redge* rgraph = (struct redge*)malloc(sizeof(struct redge) * edgeNum);
	//printf("=====================================\n");
	for(i = 0; i < edgeNum; i++){
		rgraph[i].start = graph[i].start;
		rgraph[i].end = graph[i].end;
		rgraph[i].value = graph[i].capacity - currentFlow[i].value;
		rgraph[i].rvalue = currentFlow[i].value;

		//Maintain the direction in the adjacency list
		if(rgraph[i].value > 0){
			for(tempNode = rvertexList[rgraph[i].start - 1].nodeList; tempNode != NULL; tempNode = tempNode->next){
				if(tempNode->num == rgraph[i].end){
					tempNode->connected = TRUE;
					break;
				}
			}
		}
		else{
			for(tempNode = rvertexList[rgraph[i].start - 1].nodeList; tempNode != NULL; tempNode = tempNode->next){
				if(tempNode->num == rgraph[i].end){
					tempNode->connected = FALSE;
					break;
				}
			}
		}

		if(rgraph[i].rvalue > 0){
			for(tempNode = rvertexList[rgraph[i].end - 1].nodeList; tempNode != NULL; tempNode = tempNode->next){
				if(tempNode->num == rgraph[i].start){
					tempNode->connected = TRUE;
					break;
				}
			}
		}
		else{
			for(tempNode = rvertexList[rgraph[i].end - 1].nodeList; tempNode != NULL; tempNode = tempNode->next){
				if(tempNode->num == rgraph[i].start){
					tempNode->connected = FALSE;
					break;
				}
			}
		}

		//printf("%d %d %d %d\n", rgraph[i].start, rgraph[i].end, rgraph[i].value, rgraph[i].rvalue);
	}
	//printf("=====================================\n");
	return rgraph;
}

struct vertex* bfs(){
	int i = 0, count = 0, head;
	struct vertex* tempVertex;
	struct node* tempNode;
//	struct vertex** queue = (struct vertex**)malloc(sizeof(struct vertex*) * vertexNum);
	int* queue = (int*)malloc(sizeof(int) * vertexNum);
	queue[0] = 0;
	for(i = 1; i < vertexNum; i++){
		queue[i] = 0;
		rvertexList[i].color = WHITE;
		rvertexList[i].distance = -1;
		rvertexList[i].parent = NULL;
	}
	rvertexList[startVertex - 1].color = GRAY;
	rvertexList[startVertex - 1].distance = 0;
	rvertexList[startVertex - 1].parent = NULL;
	
//	enqueue(queue, rvertexList[0]);

	for (i = 0; i < vertexNum; i++){
		if (queue[i] == 0){
			queue[i] = rvertexList[startVertex - 1].head;
			break;
		}
	}

	count++;
	while(count != 0){
//		tempVertex = dequeue(queue);
		tempVertex = &(rvertexList[queue[0] - 1]);	
		for(i = 0; i < vertexNum - 1; i++){
			queue[i] = queue[i + 1];
		}
		queue[i] = 0;
		
		count--;
		tempNode = tempVertex->nodeList;
		for(; tempNode != NULL; tempNode = tempNode->next){
			if(tempNode->connected == TRUE){
				head = tempNode->num;
				
				if(rvertexList[head - 1].color == WHITE){
					rvertexList[head - 1].color = GRAY;
					rvertexList[head - 1].distance = tempVertex->distance + 1;
					rvertexList[head - 1].parent = tempVertex;
					if(head == endVertex){
						return &(rvertexList[head - 1]);
						/*printf("%d ", head);
						for(; tempVertex != NULL; tempVertex = tempVertex->parent){
							printf("%d ", tempVertex->head);
						}*/
					//	return tempVertex;
					}

					//enqueue(queue, rvertexList[head - 1]);
					for (i = 0; i < vertexNum; i++){
						if (queue[i] == 0){
							queue[i] = rvertexList[head - 1].head;
							break;
						}
					}
					count++;
				}
			}
		}
		tempVertex->color = BLACK;
	}
	return NULL;
}

struct redge** findAugment(struct redge* rgraph){
	struct vertex* v;
	struct vertex* temp;
	struct redge** augment;
	int count = 0, i = 0;
	v = bfs();
	if(v == NULL)
		return NULL;
	temp = v;
	for(; temp != NULL; temp = temp->parent){
		//printf("%d ", temp->head);
		count++;
	}
	temp = v;
	augment = (struct redge**)malloc(sizeof(struct redge*) * (count - 1));
	i = count - 2;
	for(; temp != NULL, i >= 0; temp = temp->parent, i--){
		augment[i] = findRedge(temp->parent, temp, rgraph);
	}
	//********************************Test augment***********************************
	/*for(i = 0; i < count - 1; i++){
		printf("start = %d, end = %d\n", augment[i]->start, augment[i]->end);
	}
	printf("augment over\n");*/
	//********************************Test augment end***********************************
	return augment;
}

struct redge* findRedge(struct vertex* start, struct vertex* end, struct redge* rgraph){
	int i = 0, temp;
	for(; i < edgeNum; i++){
		if(rgraph[i].start == start->head && rgraph[i].end == end->head) {
			//if(rgraph[i].value > 0)
				return &rgraph[i];
		}
	}

	for(i = 0; i < edgeNum; i++){
		if(rgraph[i].start == end->head && rgraph[i].end == start->head){
			//if(rgraph[i].rvalue > 0 && rgraph[i].value == 0){
			temp = rgraph[i].start;
			rgraph[i].start = rgraph[i].end;
			rgraph[i].end = temp;
			temp = rgraph[i].value;
			rgraph[i].value = rgraph[i].rvalue;
			rgraph[i].rvalue = temp;
			return &rgraph[i];
			//}
		}
	}
}

struct flow* updateFlow(struct redge** augment, struct edge* graph, struct flow* currentFlow){
	int i = 0, min = 0, index;
	struct redge* tempRedge = augment[0];
	min = tempRedge->value;
	/*if(tempRedge->value > 0)
		min = tempRedge->value;
	else
		min = tempRedge->rvalue;*/
	if(tempRedge->end != endVertex){
		for(i = 1; i < edgeNum; i++){
			if(augment[i]->value < min){
				min = augment[i]->value;
			}
			/*if(augment[i]->value < min && augment[i]->value > 0){
				min = augment[i]->value;
			}
			else if(augment[i]->rvalue < min && augment[i]->value == 0){
				min = augment[i]->rvalue;
			}*/
			if(augment[i]->end == endVertex)
				break;
		}
	}
	tempRedge = augment[0];
	if(tempRedge->end == endVertex){
		index = findEdge(tempRedge->start, tempRedge->end, graph);
		if(index >= 0){
			currentFlow[index].value += min;
			totalValue += min;
		}
		else{
			index = findEdge(tempRedge->end, tempRedge->start, graph);
			currentFlow[index].value -= min;
			totalValue += min;
		}
	}
	else{
		for(i = 0; i < edgeNum; i++){
			index = findEdge(augment[i]->start, augment[i]->end, graph);
			if(index >= 0){
				currentFlow[index].value += min;
				//totalValue += min;
			}
			else{
				index = findEdge(augment[i]->end, augment[i]->start, graph);
				currentFlow[index].value -= min;
				//totalValue -= min;
			}
			if(augment[i]->end == endVertex)
				break;
		}
		totalValue += min;
	}
	//if(tempRedge->end == endVertex){
	//	index = findEdge(tempRedge->start, tempRedge->end, graph);
	//	if(tempRedge->value > 0){
	//		currentFlow[index].value += min;
	//		totalValue += min;
	//	}
	//	else{
	//		currentFlow[index].value -= min;
	//		totalValue += min;
	//	}
	//}
	//else{
	//	for(i = 0; i < edgeNum; i++){
	//		index = findEdge(augment[i]->start, augment[i]->end, graph);
	//		if(augment[i]->value > 0){
	//			currentFlow[index].value += min;
	//			//totalValue += min;
	//		}
	//		else{
	//			currentFlow[index].value -= min;
	//			//totalValue -= min;
	//		}
	//		if(augment[i]->end == endVertex)
	//			break;
	//	}
	//	totalValue += min;
	//}
	return currentFlow;
}

int findEdge(int start, int end, struct edge* graph){
	int i = 0;
	for(; i < edgeNum; i++){
		if(graph[i].start == start && graph[i].end == end){
			return i;
		}
		/*else if(graph[i].start == end && graph[i].end == start){
			return (-1) * i;
		}*/
	}
	return -1;
	/*for(; i < edgeNum; i++){
		if(graph[i].start == end && graph[i].end == start){
			return i;
		}
	}*/
}

struct vertex** findCut(){
	int i = 0, count = 0, head;
	struct vertex** vertexCut;
	struct vertex* tempVertex;
	struct node* tempNode;
//	struct vertex** queue = (struct vertex**)malloc(sizeof(struct vertex*) * vertexNum);
	int* queue = (int*)malloc(sizeof(int) * vertexNum);
	queue[0] = 0;
	for(i = 1; i < vertexNum; i++){
		queue[i] = 0;
		rvertexList[i].color = WHITE;
		rvertexList[i].distance = -1;
		rvertexList[i].parent = NULL;
	}
	rvertexList[startVertex - 1].color = GRAY;
	rvertexList[startVertex - 1].distance = 0;
	rvertexList[startVertex - 1].parent = NULL;
	
//	enqueue(queue, rvertexList[0]);

	for (i = 0; i < vertexNum; i++){
		if (queue[i] == 0){
			queue[i] = rvertexList[startVertex - 1].head;
			break;
		}
	}

	count++;
	while(count != 0){
//		tempVertex = dequeue(queue);
		tempVertex = &(rvertexList[queue[0] - 1]);	
		for(i = 0; i < vertexNum - 1; i++){
			queue[i] = queue[i + 1];
		}
		queue[i] = 0;
		
		count--;
		tempNode = tempVertex->nodeList;
		for(; tempNode != NULL; tempNode = tempNode->next){
			if(tempNode->connected == TRUE){
				head = tempNode->num;
				
				if(rvertexList[head - 1].color == WHITE){
					rvertexList[head - 1].color = GRAY;
					rvertexList[head - 1].distance = tempVertex->distance + 1;
					rvertexList[head - 1].parent = tempVertex;

					//enqueue(queue, rvertexList[head - 1]);
					for (i = 0; i < vertexNum; i++){
						if (queue[i] == 0){
							queue[i] = rvertexList[head - 1].head;
							break;
						}
					}
					count++;
				}
			}
		}
		tempVertex->color = BLACK;
	}

	count = 0;
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color == BLACK)
			count++;
	}
	vertexCut = (struct vertex**)malloc(sizeof(struct vertex*) * count);
	count = 0;
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color == BLACK){
			vertexCut[count] = &rvertexList[i];
			count++;
		}
	}
	return vertexCut;
}

int calCapacity(struct vertex** cut, int count){
	int i = 0, cutNode, j = 0, capacity = 0;
	for(i = 0; i < count; i++){
		cutNode = cut[i]->head;
		for(j = 0; j < edgeNum; j++){
			if(graph[j].start == cutNode && rvertexList[graph[j].end - 1].color != BLACK){
				capacity += graph[j].capacity;
			}
		}
	}
	return capacity;
}

struct redge* mademaxresidual(struct redge* rgraph, int maxcapacity){
	int i = 0, count = 0;
	for(i = 0; i < edgeNum; i++){
		if(rgraph[i].value >= maxcapacity || rgraph[i].rvalue >= maxcapacity){
			count++;
		}
		/*else if(rgraph[i].value == 0 && rgraph[i].rvalue >= maxcapacity){
			count++;
		}*/
	}
	maxCount = count;
	rmaxgraph = (struct redge*)malloc(sizeof(struct redge) * count);
	count = 0;
	for(i = 0; i < edgeNum; i++){
		if(rgraph[i].value >= maxcapacity || rgraph[i].rvalue >= maxcapacity){
			rmaxgraph[count] = rgraph[i];
			count++;
		}
	}
	return rmaxgraph;
}

struct vertex* bfsByWidth(struct redge* rmaxgraph){
	int i = 0, count = 0, start, end, index;
	int flag = FALSE;
	struct vertex* tempVertex;
	struct node* tempNode;
//	struct vertex** queue = (struct vertex**)malloc(sizeof(struct vertex*) * vertexNum);
	int* queue = (int*)malloc(sizeof(int) * vertexNum);
	queue[0] = 0;
	for(i = 1; i < vertexNum; i++){
		queue[i] = 0;
		rvertexList[i].color = WHITE;
		rvertexList[i].distance = -1;
		rvertexList[i].parent = NULL;
	}
	rvertexList[startVertex - 1].color = GRAY;
	rvertexList[startVertex - 1].distance = 0;
	rvertexList[startVertex - 1].parent = NULL;
	
//	enqueue(queue, rvertexList[0]);

	for (i = 0; i < vertexNum; i++){
		if (queue[i] == 0){
			queue[i] = rvertexList[startVertex - 1].head;
			break;
		}
	}

	count++;
	while(count != 0){
//		tempVertex = dequeue(queue);
		tempVertex = &(rvertexList[queue[0] - 1]);	
		start = tempVertex->head;
		for(i = 0; i < vertexNum - 1; i++){
			queue[i] = queue[i + 1];
		}
		queue[i] = 0;
		
		count--;
		tempNode = tempVertex->nodeList;
		for(; tempNode != NULL; tempNode = tempNode->next){
			if(tempNode->connected == TRUE){
				end = tempNode->num;
				
				if(rvertexList[end - 1].color == WHITE){
					rvertexList[end - 1].color = GRAY;
					rvertexList[end - 1].distance = tempVertex->distance + 1;
					rvertexList[end - 1].parent = tempVertex;

					if(findRedgeForWidth(start, end, rmaxgraph) == TRUE)
						flag = TRUE;
					if(end == endVertex && flag == TRUE){
						return &(rvertexList[end - 1]);
					}
					//enqueue(queue, rvertexList[head - 1]);
					for (i = 0; i < vertexNum; i++){
						if (queue[i] == 0){
							queue[i] = rvertexList[end - 1].head;
							break;
						}
					}
					count++;
				}
			}
		}
		tempVertex->color = BLACK;
	}
	return NULL;
}

int findRedgeForWidth(int start, int end, struct redge* rgraph){
	int i = 0;
	for(; i < maxCount; i++){
		if(rgraph[i].start == start && rgraph[i].end == end) {
			return TRUE;
		}
	}
	return FALSE;
}


struct redge** findAugmentByWidth(struct redge* rmaxgraph){
	struct vertex* v;
	struct vertex* temp;
	struct redge** augment;
	int count = 0, i = 0;
	v = bfsByWidth(rmaxgraph);
	if(v == NULL){
		return NULL;
	}
	temp = v;
	for(; temp != NULL; temp = temp->parent){
		//printf("%d ", temp->head);
		count++;
	}
	temp = v;
	augment = (struct redge**)malloc(sizeof(struct redge*) * (count - 1));
	i = count - 2;
	for(; temp != NULL, i >= 0; temp = temp->parent, i--){
		augment[i] = findRedge(temp->parent, temp, rgraph);
	}
	//********************************Test augment***********************************
	/*for(i = 0; i < count - 1; i++){
		printf("start = %d, end = %d\n", augment[i]->start, augment[i]->end);
	}
	printf("augment over\n");*/
	//********************************Test augment end***********************************
	return augment;
}

struct redge* findRedgeByWidth(struct vertex* start, struct vertex* end, struct redge* rgraph){
	int i = 0, temp;
	for(; i < maxCount; i++){
		if(rgraph[i].start == start->head && rgraph[i].end == end->head) {
			//if(rgraph[i].value > 0)
				return &rgraph[i];
		}
	}

	for(i = 0; i < maxCount; i++){
		if(rgraph[i].start == end->head && rgraph[i].end == start->head){
			//if(rgraph[i].rvalue > 0 && rgraph[i].value == 0){
			temp = rgraph[i].start;
			rgraph[i].start = rgraph[i].end;
			rgraph[i].end = temp;
			temp = rgraph[i].value;
			rgraph[i].value = rgraph[i].rvalue;
			rgraph[i].rvalue = temp;
			return &rgraph[i];
			//}
		}
	}
}

void fordfulkerson(struct edge* graph, struct redge* rgraph, struct flow* currentFlow){
	FILE *file = fopen("output.txt", "w");
	struct redge** r;
	struct flow* f;
	struct vertex** cut;
	int i = 0, count = 0, capacity = 0, j = 0, tempMax = maxCapacity;
	fprintf(file, "Use shortest path in the residual graph as the augmenting path\n");
	printf("Use shortest path in the residual graph as the augmenting path\n");
	r = findAugment(rgraph);
	while(r != NULL){
		count++;
		f = updateFlow(r, graph, currentFlow);
		rgraph = madeResidual(graph, currentFlow);
		if(count == knum){
			fprintf(file, "The augmenting path at iteration %d is\n", count);
			printf("The augmenting path at iteration %d is\n", count);
			for(i = 0; ; i++){
				if(i == 0){
					fprintf(file, "%d ", r[i]->start);
					fprintf(file, "%d ", r[i]->end);
					printf("%d ", r[i]->start);
					printf("%d ", r[i]->end);
				}
				else{
					fprintf(file, "%d ", r[i]->end);
					printf("%d ", r[i]->end);
				}
				if(r[i]->end == endVertex)
					break;
			}
		}

		/*for(j = 0; j < edgeNum; j++){
			printf("start = %d, end = %d, value = %d\n", rgraph[j].start, rgraph[j].end, rgraph[j].value);
		}*/

		r = findAugment(rgraph);
	}
	cut = findCut();
	count = 0;
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color == BLACK)
			count++;
	}
	//printf("Cut: \n");
	//for(i = 0; i < vertexNum; i++){
	//	if(rvertexList[i].color == BLACK)
	//		printf("%d ", rvertexList[i].head);
	//}
	//printf("\n");
	//for(i = 0; i < vertexNum; i++){
	//	if(rvertexList[i].color != BLACK)
	//		printf("%d ", rvertexList[i].head);
	//}
	//printf("\n");
	capacity = calCapacity(cut, count);
	//printf("Capacity = %d\n", capacity); 
	//printf("done\n");
	//printf("final total value = %d\n", totalValue);
	fprintf(file, "\n\nFlow value = %d\n", totalValue);
	printf("\n\nFlow value = %d\n", totalValue);
	fprintf(file, "The max-flow is\n");
	printf("The max-flow is\n");
	for(i = 0; i < edgeNum; i++){
		fprintf(file, "%d %d %d\n", currentFlow[i].start, currentFlow[i].end, currentFlow[i].value);
		printf("%d %d %d\n", currentFlow[i].start, currentFlow[i].end, currentFlow[i].value);
	}
	fprintf(file, "\n\nMin-cut capacity = %d\n", capacity);
	fprintf(file, "The min-cut is\n");
	printf("\n\nMin-cut capacity = %d\n", capacity);
	printf("The min-cut is\n");
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color == BLACK){
			fprintf(file, "%d ", rvertexList[i].head);
			printf("%d ", rvertexList[i].head);
		}
	}
	printf("\n");
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color != BLACK){
			fprintf(file, "%d ", rvertexList[i].head);
			printf("%d ", rvertexList[i].head);
		}
	}
	printf("\n==================================================================\n");

	for(i = 0; i < edgeNum; i++){
		currentFlow[i].value = 0;
	}
	totalValue = 0;
	rgraph = madeResidual(graph, currentFlow);
	rmaxgraph = mademaxresidual(rgraph, tempMax);
	count = 0;
	/*
	for(i = 0; i < edgeNum; i++){
		printf("%d - %d - %d\n", rmaxgraph[i].start, rmaxgraph[i].end, rmaxgraph[i].value );
	}*/
	fprintf(file, "\n==================================================================\n");
	fprintf(file, "\n\n\n\nUse widest path in the residual graph as the augmenting path\n");
	printf("Use shortest path in the residual graph as the augmenting path\n");
	while(tempMax > 1){
		r = findAugmentByWidth(rmaxgraph);
		if(r != NULL)
		while(r != NULL){
			count++;
			f = updateFlow(r, graph, currentFlow);
			rgraph = madeResidual(graph, currentFlow);
			if(count == knum){
				fprintf(file, "The augmenting path at iteration %d is\n", count);
				printf("The augmenting path at iteration %d is\n", count);
				for(i = 0; ; i++){
					if(i == 0){
						fprintf(file, "%d ", r[i]->start);
						fprintf(file, "%d ", r[i]->end);
						printf("%d ", r[i]->start);
						printf("%d ", r[i]->end);
					}
					else{
						fprintf(file, "%d ", r[i]->end);
						printf("%d ", r[i]->end);
					}
					if(r[i]->end == endVertex)
						break;
				}
			}
			r = findAugment(rgraph);
		}
		tempMax /= 2;
		rmaxgraph = mademaxresidual(rgraph, tempMax);
	}
	cut = findCut();
	count = 0;
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color == BLACK)
			count++;
	}
	/*printf("Cut: \n");
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color == BLACK)
			printf("%d ", rvertexList[i].head);
	}
	printf("\n");
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color != BLACK)
			printf("%d ", rvertexList[i].head);
	}
	printf("\n");*/
	capacity = calCapacity(cut, count);
	/*printf("Capacity = %d\n", capacity); 
	printf("done\n");
	printf("final total value = %d\n", totalValue);*/

	

	fprintf(file, "\n\nFlow value = %d\n", totalValue);
	fprintf(file, "The max-flow is\n");
	printf("\n\nFlow value = %d\n", totalValue);
	printf("The max-flow is\n");
	for(i = 0; i < edgeNum; i++){
		fprintf(file, "%d %d %d\n", currentFlow[i].start, currentFlow[i].end, currentFlow[i].value);
		printf("%d %d %d\n", currentFlow[i].start, currentFlow[i].end, currentFlow[i].value);
	}
	fprintf(file, "\n\nMin-cut capacity = %d\n", capacity);
	fprintf(file, "The min-cut is\n");
	printf("\n\nMin-cut capacity = %d\n", capacity);
	printf("The min-cut is\n");
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color == BLACK){
			fprintf(file, "%d ", rvertexList[i].head);
			printf("%d ", rvertexList[i].head);
		}
	}
	printf("\n");
	for(i = 0; i < vertexNum; i++){
		if(rvertexList[i].color != BLACK){
			fprintf(file, "%d ", rvertexList[i].head);
			printf("%d ", rvertexList[i].head);
		}
	}
}