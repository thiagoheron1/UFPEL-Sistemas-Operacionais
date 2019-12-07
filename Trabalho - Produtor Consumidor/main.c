#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 100
#define NUMBER_OF_PRODUCER 2000
#define NUMBER_OF_CONSUMER 2000
#define ITERATIONS_OF_PRODUCER 10000

typedef struct node{
   int value;
   struct node *next;
}Node;

typedef struct Queue{
   int nItems;
   struct node *first;
}Queue;
struct Queue *myBuffer;

typedef struct{
    struct Queue mB;
    int iterationsProducer;
} thread_arg, *ptr_thread_arg;

pthread_t idsProducer[NUMBER_OF_PRODUCER];
pthread_t idsConsumers[NUMBER_OF_CONSUMER];
thread_arg argumentsProducer[NUMBER_OF_PRODUCER];
pthread_mutex_t bufferMutex;
pthread_cond_t canProduce;
pthread_cond_t canConsume;


Queue* create(){
	Queue *myBuffer;
	myBuffer = malloc(sizeof(Node));
	if (myBuffer == NULL) {
		printf("It's not possible to malloc linked list!\n");
	}
	myBuffer->nItems = 0;
	myBuffer->first = NULL;
	return myBuffer;
}

int isFull(){
	if (myBuffer == NULL) {
        printf("Error: methos isFull() is not work because LinkedList(myBuffer) is NULL\n");
		return -1;
	}else if (myBuffer->nItems == BUFFER_SIZE){
		return 1;
	}
	return 0;
}

int isEmpty(){
    
	if (myBuffer == NULL) {
        printf("Error: method isEmpty() is not work because LinkedList(myBuffer) is NULL\n");
		return -1;
	} else {
        if (myBuffer->nItems == 0){
		    return 1;
	    }
    }
    return 0;
}

int length(){
    if (myBuffer == NULL) {
        printf("Error: method lenght() is not work because LinkedList(myBuffer) is NULL\n");
		return -1;
    } else{
	    return myBuffer->nItems;
    }
}

void show(){
    if (myBuffer != NULL){

        Node *node = myBuffer->first;
        if (isEmpty()){
            printf("It's not possible to show(), because LinkedList(myBuffer) is Empty!\n");
        } else {
            while (node->next != NULL){
                printf("[%d]\n", node->value);
                node = node->next;
            }
            printf("[%d]\n", node->value);
        }
    } else {
        printf("Error: method show() is not work because LinkedList(myBuffer) is NULL\n");
    }
}

int insert(int value){

	Node *newNode, *node;

    if (isFull()) {
        printf("Its not possible to insert(): Queue is Full!\n");
        return -1;

    } else {
		newNode = malloc(sizeof(Node));
		if (newNode == NULL) {
			printf("Its not possible to malloc (Insert method!)\n");
            return -1;
		}
		newNode->value = value;
        newNode->next = NULL;



        if (myBuffer->first == NULL){
            myBuffer->first = newNode;


        } else {
            node = myBuffer->first;
            while(node->next != NULL){
                node = node->next; 
            }

            node->next = newNode;
        }
        myBuffer->nItems += 1;
    }

	return 1;
}

int pop(){
    int value;
	Node *firstNode, *node;

    if (isEmpty()){
        printf("Its not possible to remove: Queue is Empty!\n");
        return -1;
    } else {
        firstNode = myBuffer->first;
        value = firstNode->value;
        myBuffer->first = firstNode->next;
        free(firstNode);
        myBuffer->nItems -= 1;
        return value;

    }
}

void destroy(){
	Node *node;

	while(myBuffer->first != NULL){
		node = myBuffer->first;
		myBuffer->first = node->next;
		free(node);
	}
    free(myBuffer);
}

int isPrimeNumber(int number){
    int count = 0;
    int i;
    
    if (number == 0 || number == 1){
        return 0;
    } else {
        for (i = 2; i <= number / 2; i++) {
            if (number % i == 0) {
                count++;
            break;
            }
        }
        if (count == 0){
            return 1;
        } else {
            return 0;
        }
    }
}



void *producer(void *arg){
    ptr_thread_arg argument = (ptr_thread_arg)arg;

    
    for(int i=0; i < argument->iterationsProducer; i++){
        pthread_mutex_lock(&bufferMutex);

        // Wait until some element be consumed
        // Bloqueia a thread em questão até ter espaço no buffer para a inserção de novos elementos.
        while(isFull()){
            //printf("Producer Blocked - Buffer is Full!\n");
            pthread_cond_wait(&canProduce, &bufferMutex);
        }
            
        int number = (int)(drand48() * 1234.0);
        insert(number);
        //printf("Thread: %lu - Insert: %d - Buffer Size: %d\n", pthread_self(), number, myBuffer->nItems);
        
        // Signal to canConsume that has some element to be consume
        // Através das variáveis de condição, informa à condição do consumidor que há elementos para ser consumido.
        pthread_cond_signal(&canConsume);
        pthread_mutex_unlock(&bufferMutex);    
    }   
}   

void *consumer(){
    int numberRemoved;

    while(1){

        pthread_mutex_lock(&bufferMutex);
        // Wait until some element be produced
        // Bloqueia a thread em questão até ter elementos no buffer para serem consumidos.
        while(isEmpty()){
            //printf("Consumer Blocked - Buffer is Empty!\n");
            pthread_cond_wait(&canConsume, &bufferMutex);
        }

        numberRemoved = pop();
        //printf("Number Removed: %d\n", numberRemoved);

        // Signal to canProduce that the buffer is empty, so can produce
        // Através das variáveis de condição, informa à condição do produtor que há espaço no buffer para produzir.
        pthread_cond_signal(&canProduce);
        pthread_mutex_unlock(&bufferMutex);


        // A verificação do número primo / igual à -1 é feito fora da sessão crítica.
        if (numberRemoved == -1){
            break;
        } else if (isPrimeNumber(numberRemoved)){
            printf("[%lu:%d]\n", pthread_self(), numberRemoved);
        }   
    }
};

int main(){

    int i;
    srand(time(NULL));

    // Initialize Mutex
    pthread_mutex_init(&bufferMutex, NULL);
    
    // Initialize Condition Variables
    pthread_cond_init(&canProduce, NULL); 
    pthread_cond_init(&canConsume, NULL);

    // Initialize LinkedList(Queue==Buffer)
    myBuffer = create();

    // Create Threads Producer
    for(i=0; i<NUMBER_OF_PRODUCER; i++){
        argumentsProducer[i].iterationsProducer = ITERATIONS_OF_PRODUCER;
        pthread_create(&(idsProducer[i]), NULL, producer, (void *) &(argumentsProducer[i]));
    }

    // Create Threads Consumer
    for(i=0; i<NUMBER_OF_CONSUMER; i++)
        pthread_create(&(idsConsumers[i]), NULL, consumer, (void *) &myBuffer);


    // Join Threads Producer
    for(i=0; i<NUMBER_OF_PRODUCER; i++)
            pthread_join(idsProducer[i], NULL);

    // Insert -1 in buffer to end consumer's threads
    for(i=0; i<NUMBER_OF_CONSUMER; i++){
        pthread_mutex_lock(&bufferMutex);
        while(isFull()){
            //printf("Producer Blocked - Buffer is Full!\n");
            pthread_cond_wait(&canProduce, &bufferMutex);
        }
        //printf("Inserted = -1\n");
        insert(-1);
        
        pthread_cond_signal(&canConsume);
        pthread_mutex_unlock(&bufferMutex);
    }

    //show();

    // Join Threads Consumer
    for(i=0; i<NUMBER_OF_CONSUMER; i++)
        pthread_join(idsConsumers[i], NULL);

    // Clean LinkedList
    destroy();

    // Clean Mutex
    pthread_mutex_destroy(&bufferMutex); 
    
    // Clean Condition Variables
    pthread_cond_destroy(&canProduce); 
    pthread_cond_destroy(&canConsume); 

    return 0;

}