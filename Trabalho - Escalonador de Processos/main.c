#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECREASESING 1
#define INCREMENTING -1
#define IGNORE 0

int memorySize;
int memoryAvailable;
int numberCPU;
char *nameFile;
int timeSlice = 0;


typedef struct process{
    int timeArrival;
    int slicesTotal;
    int slicesSize;
    int slicesToChangePriority;
    int sizeMemory;
    int priorityInitial;
    int priorityCurrent;
    int priorityStatus;
    int priorityPrevious;
    int isSearched;
    int isBlocked;
    int sliceThrown;
    int durationObserved;
    int flagThrown;
} Process;

typedef struct node{
    Process process;
    struct node *next;
} Node;

typedef struct LinkedListPriority{
    int nItems;
    struct node *first;
} LinkedListPriority;
struct LinkedListPriority *linkedListBlockedProcesses, *linkedListPriorityZero, *linkedListPriorityOne, *linkedListPriorityTwo, *linkedListPriorityThree, *linkedListPriorityFour;

int isEmpty(LinkedListPriority *linkedListPriorityX) {

    if (linkedListPriorityX == NULL){
        ////printf("Error: method isEmpty() is not work because LinkedList(myBuffer) is NULL\n");
        return -1;
    } else {
        if (linkedListPriorityX->nItems == 0) {
            return 1;
        }
    }
    return 0;
}

int isArrived(Process *process){
    if (process == NULL){
        ////printf("(ARRIVED) --> Process Null\n");
        return -1;
    } else{
        if (process->timeArrival > timeSlice){
            ////printf("(ARRIVED) --> Process [%d] not Arrived! - Time Slice = %d\n", process->timeArrival, timeSlice);
            return 0;
        } else {
            ////printf("(ARRIVED) --> Process [%d] Already Arrived - Time Slice = %d\n", process->timeArrival, timeSlice);
            return 1;
        }
    }
}

int isSearch(Process *process){
    if (process == NULL){
        ////printf("(SEARCHED) --> Process Null\n");
        return -1;
    } else {
        if (process->isSearched == 1){
            ////printf("(SEARCHED) --> Is searched! Process Searched: %d\n", process->isSearched);
            return 1;
        } else {
            ////printf("(SEARCHED) --> Is NOT searched! Process Searched: %d\n", process->isSearched);
            return 0;
        }
    }
}

int hasProcess(){
    if(isEmpty(linkedListPriorityZero) && isEmpty(linkedListPriorityOne) && isEmpty(linkedListPriorityTwo) && isEmpty(linkedListPriorityThree) && isEmpty(linkedListPriorityFour)  && isEmpty(linkedListBlockedProcesses)){
        ////printf("(hasProcess) --> All lists is empty!\n");
        return 0;
    } else {
        ////printf("(hasProcess) --> All lists is NOT empty!\n");
        return 1;
    }
}

int hasMemory(Process *process) {
    if (process == NULL){
        ////printf("(HAS MEMORY) --> Process is NULL!\n");
        return -1;
    } else{
        if (process->sizeMemory <= memoryAvailable){
            ////printf("(HAS MEMORY) --> Has Memory! Process Memory: %d - Memory Available: %d\n", process->sizeMemory, memoryAvailable);
            return 1;
        } else {
           // //printf("(HAS MEMORY) --> Not has Memory! Process Memory: %d - Memory Available: %d\n", process->sizeMemory, memoryAvailable);
            return 0;
        }
    }
}

LinkedListPriority *create(){
    LinkedListPriority *myBuffer;
    myBuffer = malloc(sizeof(Node));
    if (myBuffer == NULL) {
        //printf("It's not possible to malloc linked list!\n");
    }
    myBuffer->nItems = 0;
    myBuffer->first = NULL;
    return myBuffer;
}

void destroy(LinkedListPriority *linkedListPriorityX){
    Node *node;

    while (linkedListPriorityX->first != NULL) {
        node = linkedListPriorityX->first;
        linkedListPriorityX->first = node->next;
        free(node);
    }
    free(linkedListPriorityX);
}

int insert(Process process, LinkedListPriority *linkedListPriorityX){

    Node *newNode, *node;

    newNode = malloc(sizeof(Node));
    if (newNode == NULL) {
        ////printf("Its not possible to malloc (Insert method!)\n");
        return -1;
    }
    newNode->process = process;
    newNode->next = NULL;

    if (linkedListPriorityX->first == NULL) {
        linkedListPriorityX->first = newNode;
    } else {
        node = linkedListPriorityX->first;
        while (node->next != NULL) {
            node = node->next;
        }
        node->next = newNode;
    }
    linkedListPriorityX->nItems += 1;
    ////printf("(INSERT) --> %d\n", newNode->process.timeArrival);
    return 1;
}

void show(LinkedListPriority *linkedListPriorityX) {
    if (linkedListPriorityX != NULL) {
        Node *node = linkedListPriorityX->first;
        if (isEmpty(linkedListPriorityX)) {
            printf("(SHOW) --> Linked List is Empty!");
        } else {
            while (node->next != NULL) {
                printf("[%d-%d] ", node->process.timeArrival, node->process.slicesSize);
                node = node->next;
            }
            printf("[%d-%d] ", node->process.timeArrival, node->process.slicesSize);
        }
    } else {
        printf("Error: method show() is not work because LinkedList(myBuffer) is NULL!");
    }
    printf("\n");
}


Process* pop(LinkedListPriority *linkedListPriorityX) {
    Process *value = NULL;
    Node *firstNode, *node;
     if (isEmpty(linkedListPriorityX)) {
        //printf("(POP) -> Its not possible to remove: LinkedListPriority is Empty!\n");
        return NULL;
    } else {
        firstNode = linkedListPriorityX->first;
        value = &(firstNode->process);
        linkedListPriorityX->first = firstNode->next;
        //free(firstNode);
        linkedListPriorityX->nItems -= 1;
        ////printf("(POP) -> Process POP: %d\n", value->timeArrival);
        return value;
    }
}


void readFile() {

    FILE *file;
    int inputTimeArrival, inputSizeSlices, inputSizeMemory, inputPriority;
    Process process;

    file = fopen(nameFile, "r");
    if (file == NULL) {
        //printf("It's not possible to open this file, try again!\n");
        system("pause");
        return;
    }

    while ((fscanf(file, "%d, %d, %d, %d", &inputTimeArrival, &inputSizeSlices, &inputSizeMemory, &inputPriority)) != EOF) {
        process.timeArrival = inputTimeArrival;
        process.slicesSize = inputSizeSlices;
        process.slicesToChangePriority = 0;
        process.sizeMemory = inputSizeMemory;
        process.priorityInitial = inputPriority;
        process.priorityCurrent = inputPriority;
        process.isSearched = 0;
        process.isBlocked = 0;
        process.priorityPrevious = -2;
        process.sliceThrown = -1;
        process.durationObserved = -1;
        process.slicesTotal = inputSizeSlices;
        process.flagThrown = 0;

        if (process.priorityInitial >= 1 && process.priorityCurrent <= 3) {
            process.priorityStatus = DECREASESING;
        } else {
            process.priorityStatus = IGNORE;
        }

        if (inputPriority == 0) {
            insert(process, linkedListPriorityZero);
        } else if (inputPriority == 1) {
            insert(process, linkedListPriorityOne);
        } else if (inputPriority == 2) {
            insert(process, linkedListPriorityTwo);
        } else if (inputPriority == 3) {
            insert(process, linkedListPriorityThree);
        } else if (inputPriority == 4) {
            insert(process, linkedListPriorityFour);
        }
    }

    file = fopen("results.txt", "w+");
    fclose(file);
}


void infoProcess(Process *th) {
    if (th != NULL){
        //printf("!!!!!!!!!!!!!PROCESS!!!!!!!!!!!!!!!\n");
        //printf("Time Arrival: %d \n", th->timeArrival);
        //printf("Slices Size: %d \n", th->slicesSize);
        //printf("Slices To Change Priority: %d \n", th->slicesToChangePriority);
        //printf("Size Memory: %d \n", th->sizeMemory);
        //printf("Priority Initial: %d \n", th->priorityInitial);
        //printf("Priority Current: %d \n", th->priorityCurrent);
        //printf("Priority Status: %d \n", th->priorityStatus);
        //printf("Is Searched: %d\n", th->isSearched);
        //printf("Is Blocked: %d\n", th->isBlocked);
        //printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    } else {
        //printf("(INFO PROCESS) -- >Process is NULL!\n");
    }
    
}

Process *getFirst(LinkedListPriority *linkedListPriorityX) {
    Process *value = NULL;
    Node *firstNode, *node;

    if (isEmpty(linkedListPriorityX)) {
        ////printf("(GET FIRST) --> LinkedListPriority is Empty!\n");
        return value;
    } else {
        ////printf("(GET FIRST) --> First Process: %d\n", linkedListPriorityX->first->process.timeArrival);
        return &linkedListPriorityX->first->process;
    }
}

void rotateRoundRobin(LinkedListPriority *linkedListPriorityX) {

    //printf("Rotate Roubin-------------\n");
    Process *process = pop(linkedListPriorityX);
    if (process == NULL){
        ////printf("(ROTATE ROUND ROBIN) --> Process is NULL!\n");
    } else{
        insert(*process, linkedListPriorityX);
    }
    //printf("End Rotate------------------------\n");

}


void moveProcessToBlocked(LinkedListPriority *linkedListPriorityX){
    Process *processBlocked;
    processBlocked = pop(linkedListPriorityX);
    processBlocked->isBlocked = 1;
    processBlocked->isSearched = 1;
    insert(*processBlocked, linkedListBlockedProcesses);
}


int hasProcessUnsearched(LinkedListPriority *linkedListPriorityX){
    int i;
    Process *process;
    for(i = 0; i < linkedListPriorityX->nItems; i++){
        process = getFirst(linkedListPriorityX);
        if (process->isSearched == 0){
            return 1;
        }
        rotateRoundRobin(linkedListPriorityX);
    }
    return 0;
}

Process *findProcessAvailable(){
    Process *process;

    do{ 
        //printf("------------------L0--------------------\n");
        process = getFirst(linkedListPriorityZero);
        //printf("First Process: %d % d\n", process->timeArrival, process->slicesSize);

        if (isArrived(process) && !isSearch(process)){
            //printf("Chegou\n");
            if (hasMemory(process)){
                process->isSearched = 1;
                return process;
            } else {
                process->isSearched = 1;
                process->isBlocked = 1;
                moveProcessToBlocked(linkedListPriorityZero);
            }
        } else if (process != NULL){
            process->isSearched = 1;
        }
        rotateRoundRobin(linkedListPriorityZero);
    } while(hasProcessUnsearched(linkedListPriorityZero));


    do{
        //printf("------------------L1--------------------\n");
        process = getFirst(linkedListPriorityOne);
        if (isArrived(process) && !isSearch(process)){
            if (hasMemory(process)){
                process->isSearched = 1;
                return process;
            } else {
                moveProcessToBlocked(linkedListPriorityOne);
            }
        } else if (process != NULL){
            process->isSearched = 1;
        }
        rotateRoundRobin(linkedListPriorityOne);
    } while(hasProcessUnsearched(linkedListPriorityOne));

    do{
        //printf("------------------L2--------------------\n");
        process = getFirst(linkedListPriorityTwo);
        if (isArrived(process) && !isSearch(process)){
            if (hasMemory(process)){
                process->isSearched = 1;
                return process;
            } else {
                moveProcessToBlocked(linkedListPriorityTwo);
            }
        } else if (process != NULL){
            process->isSearched = 1;
        }
        rotateRoundRobin(linkedListPriorityTwo);
    } while(hasProcessUnsearched(linkedListPriorityTwo));

    do{
        //printf("------------------L3--------------------\n");
        process = getFirst(linkedListPriorityThree);
        if (isArrived(process) && !isSearch(process)){
            if (hasMemory(process)){
                process->isSearched = 1;
                return process;
            } else {
                moveProcessToBlocked(linkedListPriorityThree);
            }
        } else if (process != NULL){
            process->isSearched = 1;
        }
        rotateRoundRobin(linkedListPriorityThree);
    } while(hasProcessUnsearched(linkedListPriorityThree));


    do{
        //printf("------------------L4--------------------\n");
        process = getFirst(linkedListPriorityFour);
        if (isArrived(process) && !isSearch(process)){
            if (hasMemory(process)){
                process->isSearched = 1;
                return process;
            } else {
                moveProcessToBlocked(linkedListPriorityFour);
            }
        } else if (process != NULL){
            process->isSearched = 1;
        }
        rotateRoundRobin(linkedListPriorityFour);
    } while(hasProcessUnsearched(linkedListPriorityFour));



    return NULL;

}

LinkedListPriority *getLinkedListOfProcess(Process *process){
    if (process == NULL){
        ////printf("(GET LL) --> Process is NULL!\n");
        return NULL;
    } else {
        ////printf("(GET LL) --> Process is NOT NULL!\n");
        if (process->priorityCurrent == 0){
            return linkedListPriorityZero;
        } else if (process->priorityCurrent == 1){
            return linkedListPriorityOne;
        } else if (process->priorityCurrent == 2){
            return linkedListPriorityTwo;
        } else if (process->priorityCurrent == 3){
            return linkedListPriorityThree;
        } else if (process->priorityCurrent == 4){
            return linkedListPriorityFour;
        } else if (process->priorityCurrent == -1){
            return linkedListBlockedProcesses;
        }
    }
}

LinkedListPriority *getLinkedListOfProcessPrevious(Process *process){
    if (process == NULL){
        ////printf("(GET LL) --> Process is NULL!\n");
        return NULL;
    } else {
        ////printf("(GET LL) --> Process is NOT NULL!\n");
        if (process->priorityPrevious == 0){
            return linkedListPriorityZero;
        } else if (process->priorityPrevious == 1){
            return linkedListPriorityOne;
        } else if (process->priorityPrevious == 2){
            return linkedListPriorityTwo;
        } else if (process->priorityPrevious == 3){
            return linkedListPriorityThree;
        } else if (process->priorityPrevious == 4){
            return linkedListPriorityFour;
        } else if (process->priorityPrevious == -1){
            return linkedListBlockedProcesses;
        }
    }
}

Process *checkWithBlockedProcesses(Process *process){
    int i;
    Process *processBlocked;

    
    if (process == NULL){
        ////printf("(checkWithBlockedProcess) --> Process is NULL!\n");
        return NULL;
    } else {
        for(i = 0; i < linkedListBlockedProcesses->nItems; i++){
            processBlocked = getFirst(linkedListBlockedProcesses);
            if (processBlocked->priorityCurrent < process->priorityCurrent){
                if(processBlocked->sizeMemory <= memoryAvailable){
                    //printf("here!!! GETTT\n");
                    process->isSearched = 0;
                    processBlocked->isSearched = 1;
                    processBlocked->isBlocked = 1;
                    return processBlocked;
                }
            }
        }
    }
    return process;
}

void updateProcess(Process *process){
    if (process == NULL){
        //printf("(Update Process) --> Process is NULL!\n");
    } else{
        //printf("(Update Process) --> Updating\n");
        process->slicesSize -= 1;
        process->slicesToChangePriority += 1;
    }
}

void updateSystemResources(Process *process){
    memoryAvailable -= process->sizeMemory;
    //printf("(Update System) --> Updating, Memory: %d\n", memoryAvailable);
}

int hasChangePriority(Process *process) {
    if (process->priorityInitial == 0 || process->priorityInitial == 4) {
        process->slicesToChangePriority = 0;
        //printf("(hasChangePriority) --> NOT CHANGE (0-4)\n");
        return 0;
    } else {
        if (process->slicesToChangePriority == 10) {
            process->slicesToChangePriority = 0;

            //printf("(hasChangePriority) --> CHANGE\n");
            if(process->priorityInitial == process->priorityCurrent && process->priorityStatus == DECREASESING){
               process->priorityCurrent += 1;
            } else if(process->priorityInitial != process->priorityCurrent && process->priorityStatus == DECREASESING){
                process->priorityCurrent += 1;
            } else if(process->priorityInitial != process->priorityCurrent && process->priorityStatus == INCREMENTING){
                process->priorityCurrent -= 1;
            }

            if (process->priorityCurrent == 4){
                process->priorityStatus = INCREMENTING;
            }
            return 1;
        }
    }

    
    return 0;
}

void moveProcessPriority(Process *process){
    LinkedListPriority *llPrevious, *llCurrent;
    Process *processRemoved;
    llPrevious = getLinkedListOfProcessPrevious(process);
    llCurrent = getLinkedListOfProcess(process);
    processRemoved = pop(llPrevious);
    insert(*process, llCurrent);
}


int hasSlices(Process *process) {
    if (process->slicesSize == 0){
        //printf("(hasSlices) --> NOT\n");
        return 0;
    } else {
        //printf("(hasSlices) --> YES\n");
        return 1;
    }
}

void moveOutBlockedList(){
    Process *processBlocked;
    LinkedListPriority *ll;
    processBlocked = pop(linkedListBlockedProcesses);
    processBlocked->isBlocked = 0;
    
    ll = getLinkedListOfProcess(processBlocked);
    insert(*processBlocked, ll);
}

void updateIsSearch(){
    Process *process;
    for(int i= 0; i < linkedListPriorityZero->nItems; i++){
        process = pop(linkedListPriorityZero);
        process->isSearched = 0;
        insert(*process, linkedListPriorityZero);
    }

    for(int i= 0; i < linkedListPriorityOne->nItems; i++){
        process = pop(linkedListPriorityOne);
        process->isSearched = 0;
        insert(*process, linkedListPriorityOne);
    }

    for(int i= 0; i < linkedListPriorityTwo->nItems; i++){
        process = pop(linkedListPriorityTwo);
        process->isSearched = 0;
        insert(*process, linkedListPriorityTwo);
    }

    for(int i= 0; i < linkedListPriorityThree->nItems; i++){
        process = pop(linkedListPriorityThree);
        process->isSearched = 0;
        insert(*process, linkedListPriorityThree);
    }

    for(int i= 0; i < linkedListPriorityFour->nItems; i++){
        process = pop(linkedListPriorityFour);
        process->isSearched = 0;
        insert(*process, linkedListPriorityFour);
    }


}


void writeFile(Process *process){
FILE *fp;

    fp = fopen("results.txt", "a+");
    fprintf(fp,"%d, %d, %d, %d\n", process->timeArrival, process->sliceThrown, process->slicesTotal, process->durationObserved);


    fclose(fp);
}

void CPU(){
    LinkedListPriority *ll;


    // Encontra um processo que esteja disponível para ser executado(tempo de chegada, memória suficiente)
    Process *process = findProcessAvailable();

    if (process != NULL){

        // Verifica se há algum processo na fila de processos bloqueados com preferência(prioridade maior e  memória suficiente), trocando caso seja possível.
        process = checkWithBlockedProcesses(process);
        if (process != NULL){

            // Flag para controlar o output do tempo de lançamento.
            if (process->flagThrown == 0){
                process->sliceThrown = timeSlice;
                process->flagThrown = 1;
            }

            // Atualização de informações do processo e sistema(Número de Slices Restantes, Número de Slices Passados na Prioridade, Memória Restante Sistema)
            infoProcess(process);
            ll = getLinkedListOfProcess(process);
            updateProcess(process);
            updateSystemResources(process);


            // Verifica se ainda há slices para o processo.
            if (hasSlices(process)){
                process->priorityPrevious = process->priorityCurrent;

                // Caso tenha slices, verifica se é necessário trocar a prioridade.
                if (hasChangePriority(process)){
                    moveProcessPriority(process);
                } else {
                    // Caso contrário, verifica se o processo era um bloqueado
                    if(process->isBlocked){

                        // Se for bloqueado, remove da fila de bloqueados e recoloca ele na sua lista de prioridades.
                        process->isBlocked = 0;
                        //printf("Moved Process Blocked!\n");
                        moveOutBlockedList();
                        infoProcess(process);
                    } else {

                        // Caso não seja um processo bloqueado, realiza um round robin
                        //printf("Rotate Roubin!\n");
                        rotateRoundRobin(ll);
                    }
                }
            } else {
                // Caso não tenha mais slices, o processo é terminado.
                process->durationObserved = timeSlice+1;
                writeFile(process);
                Process *p = pop(ll);
                free(p);
            }
        }
    } 


    // Caso não tenha encontrado um processo nas listas de proriodades, encontra processos na lista de bloqueados.
    if (process == NULL){

        if(!isEmpty(linkedListBlockedProcesses)){
            moveOutBlockedList();

        }
    }


}



int main(int argc, char *argv[]) {

    numberCPU = atoi(argv[1]);
    memorySize = atoi(argv[2]);
    nameFile = argv[3];

    memoryAvailable = memorySize;
    printf("%d %d %s", numberCPU, memoryAvailable, nameFile);


    // Inicializa as Linked List das Filas de Prioridades executando em Round Robin (Controlado pelo Insert/Pop)
    linkedListPriorityZero = create();
    linkedListPriorityOne = create();
    linkedListPriorityTwo = create();
    linkedListPriorityThree = create();
    linkedListPriorityFour = create();
    linkedListBlockedProcesses = create();

    // Lê o arquivo, preeenchendo as filas de prioridades com os processos.
    readFile();

    // Enquanto houver processos em alguma fila de prioridade, irá printar o estado atual de todas as listas em um slice da unidade de tempo.
    while (hasProcess()) {
        printf("-------------------------------------\n");
        printf("Time Slice: %d\n", timeSlice);
        printf("Memory Available: %d\n", memoryAvailable);
        printf("LL Zero: ");
        show(linkedListPriorityZero);
        printf("LL One: ");
        show(linkedListPriorityOne);
        printf("LL Two: ");
        show(linkedListPriorityTwo);
        printf("LL Three: ");
        show(linkedListPriorityThree);
        printf("LL Four: ");
        show(linkedListPriorityFour);
        printf("LL Blocked: ");
        show(linkedListBlockedProcesses);
        printf("-------------------------------------\n");

        // Estrutura de Repetição para simular a quantidade de CPU's executada em uma unidade de tempo(slice)
        for (int i = 0; i < numberCPU; ++i){
            //printf("here\n");
            CPU();

        }

        // Atualização do tempo, memória e flags disponíveis para o próximo slice.
        ++timeSlice;
        memoryAvailable = memorySize;
        updateIsSearch();
        //printf("\n");
    }


    printf("-------------------------------------\n");
    printf("Time Slice: %d\n", timeSlice);
    printf("Memory Available: %d\n", memoryAvailable);
    printf("LL Zero: ");
    show(linkedListPriorityZero);
    printf("LL One: ");
    show(linkedListPriorityOne);
    printf("LL Two: ");
    show(linkedListPriorityTwo);
    printf("LL Three: ");
    show(linkedListPriorityThree);
    printf("LL Four: ");
    show(linkedListPriorityFour);
    printf("LL Blocked: ");
    show(linkedListBlockedProcesses);
    printf("-------------------------------------\n");


    // Clean LinkedList
    destroy(linkedListPriorityZero);
    destroy(linkedListPriorityOne);
    destroy(linkedListPriorityTwo);
    destroy(linkedListPriorityThree);
    destroy(linkedListPriorityFour);


    return 0;
}
