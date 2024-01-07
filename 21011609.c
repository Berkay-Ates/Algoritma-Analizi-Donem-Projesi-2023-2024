#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE_SIZE 1000
#define MAX_TOUR_TIME 15

// BFS ile graf uzerinde dolanirken yardimci olmasi icin queue structu
typedef struct QUEUE {
  int items[MAX_QUEUE_SIZE];
  int front;
  int rear;
} QUEUE;

// Traverse tree uzerindeki nodelar 
typedef struct node {
  int intValue;
  char charValue;
  struct node* root; 
  struct node* leaf;
  struct node* next;
}NODE;

// Traverse tree olusturmak icin BFS ile graf uzerinde dolanirken yardimici olmasi icin nodeQueue structu
typedef struct nodeQueue{
  struct node** items;
  int front;
  int rear;
}NODEQUEUE;

//BFS sonucu elde ettigimiz graph bilgilerini saklayabilmek icin graph structu 
typedef struct graph {
  int graphSize;
  int graphElements[MAX_QUEUE_SIZE];
}GRAPH;

// queue ye eleman ekleme fonksiyonu
void enqueue(QUEUE *q, int value);

// queue den eleman cekme fonksiyonu 
int dequeue(QUEUE *q);

// queue nin bos olup olmadigini kontrol eden fonksiyon 
int isEmpty(QUEUE *q);

// graph uzerinde BFS algoritmasi calistiran fonksiyon
void bfs(int** graph, int start, int n,int* visited);

// Node queue ye eleman ekleyen fonksiyon
void enqueueNode(NODEQUEUE* nodeQueue,NODE* value);

// Nodequeue nin bos olup olmadigini kontrol eden fonksiyon 
int isEmptyNodeQueue(NODEQUEUE* nodeQueue);

// nodequeue den eleman ceken fonksiyon 
NODE* dequeueNode(NODEQUEUE* nodeQueue);

// Traverse tree olusturmamiz icin kullandigimiz node malloclayan fonksiyon 
NODE* mallocNode(char charValue,int intValue);

// maksimum edge degerini bulmak icin calisan fonksiyon 
int** calculateEdgeBetwennes(int** graph, int length,char* elements);

// Her bir node icin path Tree (TRAVERSE TREE) olusturan fonksiyon 
NODE* createPathTreeOneNode(int** graph,int start,int length,char* elements);

// Her bir nodun tree sini bastiran fonksiyon 
void printPathTree( NODE* root,NODE** path,int length,int ** pathThree,char* elements);

// Grafin maksimum edge degerini bulup koparan fonksiyon 
void handleGraphEdges(int** graph, int graphSize,char* elements);

// Grafin edge degerlerini bastiran fonksiyon 
int printEdgeMatrixInfo(int** edgeMatrix,int graphSize,char* elements);

// maksimum degere sahip olan edge i koparan fonksiyon 
void breakOffStrongestEdge(int** graph,int** edgeMatrix,int graphSize,int max,char* elements);

// edgelerin degerlerini bastiran fonksiyon 
void printEdgeConnectnessMatrix(int** graph, int graphSize,char* elements);

// tum fonksiyonlari cagiran ve kendini de recursive bir sekilde yineleyen asil is ustlenici fonksiyon 
void handleEveryThing(int** graph, int graphSize,int counter,char* elements,int groupSize,int prevGraphSize);

// BFS ile gruplari ve grup eleman sayilarini bulan fonksiyon 
GRAPH** findGroupsAndGroupSize(int** graph, int graphSize);

// BFS ile bulunan graflari yazdiran fonksiyon 
void printFoundedGraphs(GRAPH** graphs,int graphSize,char* elements);

// BFS ile bulunan elemanlari freeleyen fonksiyon 
void freeBFSGraphs(GRAPH** graphs,int graphSize);

// Elde edilen TRAVERSE TREE (path tree) freeleyen fonksiyon 
void freePathTree(NODE* root);

// dosyadan baglanti matrisini okuyan fonksiyon 
int readFromFileMatrix( char *filename, int ***matrix);

// baglanti matrisini ekrana yazdiran fonksiyon
void printMatrix(int **matrix, int size);

int main() {
  char elements[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
  int i = 0;

  // dosya ismimiz
  char fileName[10] = "test5.txt";

  // adjacency matris ve matris boyutu
  int** graph;
  int graphSize = readFromFileMatrix(fileName, &graph); 

  // t degerimiz
  int groupSize = 3;

  printf("Desired graph size (t): %d\n",groupSize);
  printf("Matrix Size: %d\n", graphSize);
  printf("Readed Matrix From File:\n");
  printMatrix(graph, graphSize);
  
  printf("EDGE betweennes is running\n");
  handleEveryThing(graph,graphSize,0,elements,groupSize,0);
  printf("done");

  for(i=0;i<graphSize;i++)
    free(graph[i]);
  
  free(graph);
  return 0;
}

int readFromFileMatrix( char *filename, int ***matrix) {
    printf("File name: %s\n",filename);
    FILE *file = fopen(filename, "r");
    int size;
    int i;
    int j;

    if (file == NULL) {
        perror("File openning error\n");
        exit(-1);
    }
    
    // Dosyanin ilk satirindaki degeri matris boyutu olarak alir.
    fscanf(file, "%d", &size);

	 // Matris i�in bellek ayristirma islemi.
    *matrix = (int **)malloc(size * sizeof(int *));
    for (i = 0; i < size; i++) {
      (*matrix)[i] = (int *)malloc(size * sizeof(int));
    }
    

 	// Dosyadan matris elemanlarini okuyarak matrise atar.
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++){ 
          fscanf(file, "%d", &(*matrix)[i][j]);
        }
    }

	 // Matris boyutunu d�nd�r�r.
    fclose(file);

    return size;
}

void printMatrix(int **matrix, int size) {
  int i,j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void handleEveryThing(int** graph, int graphSize,int counter,char* elements,int groupSize,int prevGraphSize){

  GRAPH** graphs = findGroupsAndGroupSize(graph,graphSize);
  printf("\n");

  int i = 0;
  int counter2 = counter;

	// Graf eleman sayisi belirli bir s�re boyunca degismezse islemleri sonlandir.
  if(counter==MAX_TOUR_TIME){
    printf("The count of the graph elements did not  changed for the last %d steps so operations ending\n",MAX_TOUR_TIME);
    printFoundedGraphs(graphs,graphSize,elements);
    freeBFSGraphs(graphs,graphSize);
    return;
  }
	
	  // Belirli bir grup boyutuna ulasildiginda islemleri sonlandir.
  while(graphs[i] != NULL && i < graphSize){
    if(graphs[i]->graphSize <= groupSize){
      printf("We reached a graph which have desired number of elements which is %d, last groups are such that:\n",groupSize);
      printFoundedGraphs(graphs,graphSize,elements);
      freeBFSGraphs(graphs,graphSize);
      return;
    }
    i++;
  }

  //* eger suanki graf sayisi ile onceki graf sayisi ayni ise counteri arttiralim
  //* eger graftaki alt gruplarin sayisi degismis ise bu defa counteri sifirlayalim
  if(i==prevGraphSize){
    counter2++;
  }else{
    counter2=0;
  }

  printFoundedGraphs(graphs,graphSize,elements);
  freeBFSGraphs(graphs,graphSize);
  handleGraphEdges(graph,graphSize,elements);

	// yeni grup ile eski grup ayni elemana sahipse counter i arttir 
  handleEveryThing(graph,graphSize,counter2,elements,groupSize,i);
}

void freeBFSGraphs(GRAPH** graphs,int graphSize){
  int i =0;
  while(graphs[i] != NULL && i < graphSize){
    free(graphs[i]);
    i++;
  }
  free(graphs);
}

void printFoundedGraphs(GRAPH** graphs,int graphSize,char* elements){
  int i =0;
  int j = 0;
  while(graphs[i] != NULL && i < graphSize){
    printf("Graph %d is consist from : ",i+1);
    for(j=0;j<graphs[i]->graphSize;j++){
      printf(" %c, ",elements[graphs[i]->graphElements[j]]);
    }
    printf("\n");
    i++;
  }
}

GRAPH** findGroupsAndGroupSize(int** graph, int graphSize){
  int* visited = (int*) malloc(sizeof(int)*graphSize);
  int i = 0 ; 
  int j = 0;
  int k = 0;
  int counter = 0;

  GRAPH* tmpGraph;

	// Graf dizisi i�in bellek ayristirma islemi.
  GRAPH** graphs = (GRAPH**) malloc(sizeof(GRAPH*)*MAX_QUEUE_SIZE);
  if(graphs==NULL){
    printf("There was error while allocating memory\n");
  }

    // Graf dizisini baslangi�ta bos olarak ayarla.
  for(i=0; i<graphSize; i++)
    graphs[i] = NULL;

  // Ziyaret edilen d�g�mleri baslangi�ta sifira ayarla.
  for(i=0; i<graphSize; i++)
    visited[i] = 0;

  for(i=0; i<graphSize; i++){
  	// Ziyaret edilmemis d�g�m bulundugunda BFS ile yeni bir grup olustur.
    if(visited[i] == 0 ){
      bfs(graph,i,graphSize,visited);

      tmpGraph = (GRAPH*) malloc(sizeof(GRAPH));
      graphs[counter] = tmpGraph;
      counter++;
      tmpGraph->graphSize = 0;
       // Her grup i�in bos GRAPH yapisi olustur.
      for(k=0;k<graphSize;k++)
        tmpGraph->graphElements[k] = 0;

		// BFS tarafindan ziyaret edilen d�g�mleri grup elemanlarina ekle.
      for(j=0;j<graphSize;j++){
        if(visited[j] == i+1){
          tmpGraph->graphElements[tmpGraph->graphSize] = j;
          tmpGraph->graphSize+=1;
        }
      }
    }
  }
  
  return graphs;
}


void handleGraphEdges(int** graph, int graphSize,char* elements){
  int** edgeMatrix;
  int maxEdgeValue;

  //* once bfs ile kac grup oldugu ve gruplarin eleman sayilari tespit edilmeli 
  //* eger herhangi bir grubun eleman sayisi belli bir degerin altinda ise islem bitirilmeli 
  //* eger eleman sayisi belirli bir grubun altinda degilse her grup icin ayni islem tekrarlanmali
  //* gruplardan birinde bulunan eleman sayisi belli bir degerin altindaysa islem sonlandirilmali

  edgeMatrix = calculateEdgeBetwennes(graph,graphSize,elements);
  maxEdgeValue = printEdgeMatrixInfo(edgeMatrix,graphSize,elements);
  printf("%d is max edge value\n",maxEdgeValue);
  printf("BEFORE ADJACENCY MATRIX\n");
  printEdgeConnectnessMatrix(graph,graphSize,elements);
  breakOffStrongestEdge(graph,edgeMatrix,graphSize,maxEdgeValue,elements);
  printf("AFTER ADJACENCY MATRIX\n");
  printEdgeConnectnessMatrix(graph,graphSize,elements);


}

void printEdgeConnectnessMatrix(int** graph, int graphSize,char* elements){
  int i=0 , j=0;
  
    printf("     ");
  for(j=0;j<graphSize;j++)
    printf("  %c  ",elements[j]);

  printf("\n");

  for(i=0;i<graphSize;i++){
    printf("  %c  ",elements[i]);
    for(j=0;j<graphSize;j++)
        printf("  %d  ",graph[i][j]);

    printf("\n");
  }

}

void breakOffStrongestEdge(int** graph,int** edgeMatrix,int graphSize,int max,char* elements){

  int i =0;
  int j =0;

  for(i=0;i<graphSize;i++){
    for(j=0;j<graphSize;j++){
      if(edgeMatrix[i][j] == max){
       printf("Edge between %c and %c has been broke off.\n",elements[i],elements[j]);
       graph[i][j] = 0;
       graph[j][i] = 0;
      }
    }
  }

}

int printEdgeMatrixInfo(int** edgeMatrix,int graphSize,char* elements){
  int i = 0;
  int j = 0;
  int max = 0;

  printf("     ");
  for(j=0;j<graphSize;j++)
    printf("  %c  ",elements[j]);

  printf("\n");

  for(i=0;i<graphSize;i++){
    printf("  %c  ",elements[i]);
    for(j=0;j<graphSize;j++){
      if(edgeMatrix[i][j] < 10)
        printf("  %d  ",edgeMatrix[i][j]);
      else 
        printf(" %d  ",edgeMatrix[i][j]);

    }

    printf("\n");
  }

  for(i=0;i<graphSize;i++){
    for(j=0;j<graphSize;j++){
      if(edgeMatrix[i][j] != 0){
        printf("EDGE ('%c','%c'):%d times\n",elements[i],elements[j],edgeMatrix[i][j]);
        edgeMatrix[j][i]=0;
        if(max < edgeMatrix[i][j])
          max = edgeMatrix[i][j];
      }
    }
  }
  return max;
}

void enqueue(QUEUE *q, int value) {
  if (q->rear == MAX_QUEUE_SIZE - 1) {
    printf("Queue full\n");
  } else {
    q->items[++(q->rear)] = value;
  }
}

int dequeue(QUEUE *q) {
  if (q->front > q->rear) {
    printf("Queue empty\n");
    return -1;
  } else {
    return q->items[(q->front)++];
  }
}

int isEmpty(QUEUE *q) {
  return q->front > q->rear;
}

void enqueueNode(NODEQUEUE* nodeQueue,NODE* value){
  if(nodeQueue->rear == MAX_QUEUE_SIZE-1){
    printf("Queue is full\n");
  }else{
    nodeQueue->items[++(nodeQueue->rear)]=value;
  }
}

int isEmptyNodeQueue(NODEQUEUE* nodeQueue){
  //* 1 donerse empty, 0 donerse empty degil. 
  return nodeQueue->front > nodeQueue->rear;
}

NODE* dequeueNode(NODEQUEUE* nodeQueue){
  if(nodeQueue->front>nodeQueue->rear){
    printf("Queue is empty\n");
    return NULL;
  }else{
    return nodeQueue->items[(nodeQueue->front)++];
  }
}

int** calculateEdgeBetwennes(int** graph, int length,char* elements){
  //* path tree yi tum nodelar icin hesaplayip diziye kaydedelim 
  NODE** pathTree = (NODE**) malloc(sizeof(NODE*)*length);
  if(pathTree == NULL){
    printf("Error while allocating memory.. exitting\n");
    exit(-1);
  } 
  int i,j = 0;
   // D�g�mler arasindaki kenarlari kaydedecegimiz matris.
  int** edgeMatrix = (int**) malloc(sizeof(int*)*length);
  if(edgeMatrix == NULL){
    printf("There was an error while allocating edgeMatrix\n");
    exit(-1);
  }

  for(i=0;i<length;i++){
    edgeMatrix[i] = (int*) malloc(sizeof(int)*length);
    for(j=0;j<length;j++)
      edgeMatrix[i][j] = 0;
  }


  //* her node icin gezinti matrisini olusturuyoruz 
  for(i=0;i<length;i++){
    pathTree[i] = createPathTreeOneNode(graph,i,length,elements);
  }

  //* tum pathi yazdirirken ayni zamanda nodelar arasindaki gecisleri de kaydedelim
  NODE** path;

  for(i=0;i<length;i++){
    path = (NODE**) malloc(sizeof(NODE*)*length);
    
    for(j=0;j<length;j++)
      path[j] = NULL;

    path[0] = pathTree[i];
    printPathTree(pathTree[i],path,length,edgeMatrix,elements);
    freePathTree(pathTree[i]);
    printf("\n");
  }
  
  return edgeMatrix;
}

void freePathTree(NODE* root){
  NODE* tmp = root->leaf;
  if(tmp == NULL){
    free(root);
    return;
  }
  while(tmp != NULL){
    freePathTree(tmp);
    tmp=tmp->next;
  }
}

NODE* createPathTreeOneNode(int** graph,int start,int length,char* elements){      
  //* NODEQUEUE olusup yok olabilir ama startNode path tree de yer alacagi icin olusturulup geri dondurulmeli

  NODEQUEUE* nodeQueue = (NODEQUEUE*) malloc(sizeof(NODEQUEUE));
  if(nodeQueue == NULL){
    printf("Error while allocating memory.. exitting\n");
    exit(-1);
  }

  nodeQueue->items = (NODE**) malloc(sizeof(NODE*)*MAX_QUEUE_SIZE);
  if(nodeQueue->items == NULL){
    printf("There is memory allocation error\n");
    exit(-1);
  }

  nodeQueue->front=0;
  nodeQueue->rear=-1;


  int i,j;
  int visitedNodes[length];
  for(i=0;i<length;i++)
    visitedNodes[i] = 0;

  //* start node ayni zamanda bizim icin root node
  NODE* startNode = mallocNode(elements[start],start);
  enqueueNode(nodeQueue,startNode);
  visitedNodes[start] = 1;

  NODE* tmpNode;
  NODE* tmpNode2;

  int queuedCount=0;
  int count =1;
  int tour=1;

  while(!isEmptyNodeQueue(nodeQueue)){
    queuedCount = count;
    count = 0;
    tour++;
    // queue ye bir turda kac tane eleman atilmissa hepsini temizlememiz gerekir dolayisiyla 2. for bundan kaynakli
    for(j=0;j<queuedCount;j++){
      tmpNode = dequeueNode(nodeQueue);
      for(i=0;i<length;i++){
        if((visitedNodes[i]==0 || visitedNodes[i]==tour) && graph[tmpNode->intValue][i]){
          count++;
          tmpNode2 = mallocNode(elements[i],i);
          enqueueNode(nodeQueue,tmpNode2);
          visitedNodes[i] = tour;
          tmpNode2->root=tmpNode;
          tmpNode2->next=tmpNode->leaf;
          tmpNode->leaf=tmpNode2;
        }
      }
    }
  }
  return startNode;
}

void bfs(int** graph, int start, int length,int* visited){
  QUEUE queue;
  int i;

  queue.front = 0;
  queue.rear = -1;

  enqueue(&queue, start);
  visited[start] = start+1;
  printf("BFS Traverse Node Indexes: ");
  while (!isEmpty(&queue)) {
    int current = dequeue(&queue);
    printf("%d ", current);

    for (i = 0; i < length; i++) {
      if (graph[current][i] && !visited[i]) {
        enqueue(&queue, i);
        visited[i] = start+1;
      }
    }
  }
  printf("\n");
}

void printPathTree(NODE* root,NODE** path,int length,int** edgeMatrix,char* elements){
  NODE** newPath;
  NODE* tmp = root->leaf;
  int i =0;
  while(tmp != NULL){
    printf("Shorthest path from %c to %c:[",path[0]->charValue,tmp->charValue);
    i = 0;
     // Her bir yolun baslangi� d�g�m� ile son d�g�m� arasindaki kenarlari yazdir.
    while(path[i] != NULL){
      printf("'%c',",path[i]->charValue);
      if(path[i+1] != NULL)
        edgeMatrix[path[i]->intValue][path[i+1]->intValue]+=1;

      i++;
    }
    
     // Son d�g�m ile biten kenari g�ncelle.
    printf("'%c']\n",tmp->charValue);
    edgeMatrix[path[i-1]->intValue][tmp->intValue]+=1;
    tmp = tmp->next;
  }
   // Agacin alt d�g�mleri i�in ayni islemi tekrarla.
  tmp = root->leaf;
  while(tmp!=NULL){
    newPath = (NODE**) malloc(sizeof(NODE*)*length);
    for(i=0;i<length;i++)
      newPath[i] = NULL;

    i=0;
    while(path[i] != NULL){
      newPath[i] = path[i];
      i++;
    }
     // Agacin altindaki d�g�mleri yazdirma ve kenar matrisini g�ncelleme.
    newPath[i]=tmp;
    printPathTree(tmp,newPath,length,edgeMatrix,elements);
    tmp = tmp->next;
    free(newPath);
  }
}

NODE* mallocNode(char charValue,int intValue){
  NODE* tmpNode = (NODE*) malloc(sizeof(NODE));
  if(tmpNode == NULL){
    printf("Error while allocating memory.. exitting\n");
    exit(-1);
  }
  tmpNode->intValue=intValue;
  tmpNode->charValue=charValue;
  tmpNode->root=NULL;
  tmpNode->leaf=NULL;
  tmpNode->next = NULL;

  return tmpNode;
}
