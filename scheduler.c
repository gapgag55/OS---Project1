#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void output();
void FCFS();
void SJF();
void RR(int quantum);

struct Candidate {
   int  pid;
   int  burstTime;
   int  arrivalTime;
   int  priority;
   //
   int  startTime;
   int  finishTime;
   //
   int responseTime;
   int waitingTime;
   int turnaroundTime;
};

/*
 * Initiate global variables
 * We have processors, Size
 */
struct Candidate processors[999];
int size = 0;

int main(int argc, char *argv[])
{

  // Read file and set variables
  FILE* file;
  char buffer[255];

  /*
   * Read file from arguments
   * Params at below:
   * - argv[0] = ./scheduler
   * - argv[1] = Filename
   * - argv[2] = Algorithm for process scheduling
   */

  file = fopen(argv[1], "r");

  if (file) {
    // Initial signification variables;
    int lineIndexer = 0;

    // Read file each line
    while(fgets(buffer, 255, (FILE*) file)) {

      // Check first line is a number of processors
      if ( lineIndexer == 0 ) {
        size = atoi(buffer);
        lineIndexer++;
        continue;
      }

      // Split information for each line
      char *token = strtok( buffer, "," );
      int indexer = 0;
      int temp[4];

      while( token != NULL ) {
        /* While there are tokens in "string" */
        temp[indexer++] = atoi(token);
        /* Get next token: */
        token = strtok( NULL, "," );
      }

      /*
       * Read Information each line
       * Add signification information to processors array
       * 3 - 1 = 2 --> 0 = (size - 1) - (size - lineIndexer)
       * 3 - 2 = 1 --> 1
       * 3 - 3 = 0 --> 2
       */
       indexer = (size - 1) - (size - lineIndexer);
       processors[indexer].pid = temp[0];
       processors[indexer].burstTime = temp[1];
       processors[indexer].arrivalTime = temp[2];
       processors[indexer].priority = temp[3];

       lineIndexer++;
    }

    fclose(file);

  } else {

    fprintf(stderr, "Error opening file");

  }

  // Print output before Algorithm processing
  output(0);


  // Check wheter what is algorithm user might user - FCFS, SJF, 10 (RR)
  // If argv[2] > 0, we predict that it is a number of quantum
  if (atoi(argv[2]) > 0) {
    RR(atoi(argv[2]));
  } else if ( strcmp(argv[2], "FCFS") == 0 ) {
    FCFS();
  } else if ( strcmp(argv[2], "SJF") == 0 ) {
    SJF();
  } else {
    fprintf(stderr, "You input out of bound algorithms");
  }

  return 0;
}

/*
 * First Come First Serve Algorithm
 */
void FCFS()
{

  struct Candidate minTime;
  // selection sort to fine the startTime and finishTime
  for (int i = 0; i < size-1; i++) {
    int minTime = i;
    for (int j = i+1; j < size; j++) {

      int checkTime = processors[j].arrivalTime < processors[minTime].arrivalTime;
      int checkPriority = processors[j].priority < processors[minTime].priority;
      int checkBrustTime = processors[j].burstTime < processors[minTime].burstTime;

      if (checkTime) {
        minTime = j;
      } else if ( processors[j].arrivalTime == processors[minTime].arrivalTime ) {
        if (checkTime){
          minTime = j;
        }else if (processors[j].priority == processors[minTime].priority){
          if(checkBrustTime){
            minTime = j;
          }
        }
      }
    }

    struct Candidate temp = processors[minTime];
    processors[minTime] = processors[i];
    processors[i] = temp;

    //set startTime
    if(i == 0) {
      // speacial case when i = 0;
      processors[i].startTime = 0;
    } else {
      // normal case
      processors[i].startTime = processors[i-1].finishTime;
    }
    // set finishTime
    processors[i].finishTime = processors[i].startTime + processors[i].burstTime;
  }
  processors[size-1].startTime = processors[size-2].finishTime;
  processors[size-1].finishTime = processors[size-1].startTime + processors[size-1].burstTime;

  //find total brust time
  for (int i = 0; i < size; i++) {
    processors[i].responseTime = processors[i].startTime - processors[i].arrivalTime;
    processors[i].waitingTime = processors[i].startTime - processors[i].arrivalTime;
    processors[i].turnaroundTime = processors[i].finishTime - processors[i].arrivalTime;

  }
  output(1);
}

/*
 * Shortest Job First Algorithm - nonpreemtive
 * @param {}
 */
void SJF()
{
  // for(int i = 0 ; i < size-1; i++) {
  //   for (int j = i+1; j < size; j++) {
  //
  //   }
  // }
}

/*
 * Round Robin Algorithm
 * @param {}
 */
void RR(int quantum)
{
  /*
   * Use Selection sort to sort firstest arrive
   */
  for (int i = 0; i < size-1; i++) {
    for (int j = i+1; j < size; j++) {
      if ( processors[i].arrivalTime > processors[j].arrivalTime ) {
        struct Candidate temp = processors[i];
        processors[i] = processors[j];
        processors[j] = temp;
      }
    }
  }

  /*
   * Initiate variables and processors struct value
   */
  int tempBurstTime[size];

  for (int i = 0; i < size; i++) {
    tempBurstTime[i] = processors[i].burstTime;

    processors[i].startTime = -1;
    processors[i].finishTime = -1;
  }

  /*
   * Initiate variables to use soon
   */
  int index = 0;
  int count = 0;
  int i = 0;
  
  /*
   * Use Like-infinite loop to run the whole procecss
   */
  while (1) {
    
    /*
     * Indexer will point to next process to run when count variable 
     * equals quantum time
     */
    if (count == quantum) {

      // Reset count when run if instruction
      count = 0;

      /*
       * Check next process that is it arrived or not?
       * If so, next process ready to running
       * else, previous processs will run next
       */
      if (processors[index+1].arrivalTime <= i) {
        index++;
      } else {
        index--;
      }

      // If index out of bound reset it to be first process
      if (index >= size || index < 0) index = 0;
    }
    
    // Find process that remains of burstTime
    while(processors[index].burstTime == 0) {
      index++; count = 0;

      /* Detect and Prevent in case of process does not arrive after 
       * Previous process running finished
       * If there is not arrived, we will wait until it arrives 
       */
      if (processors[index].arrivalTime > i) {
        index--; i++;
        continue;
      }

      // If index out of bound reset it to be first process
      if (index >= size) index = 0;
    }

    // Running process
    processors[index].burstTime -= 1;
    count++;

    // Find Finished Time 
    if (processors[index].burstTime == 0 && processors[index].finishTime == -1) {
      processors[index].finishTime = i+1;
    } 

    // Find Started Time
    if (processors[index].startTime == -1) {
      processors[index].startTime = i;
    } 

    // Increase Process time
    i++;


    /*
     * This instructions is used to break the process
     * when every process running finished 
     */
    int sum = 0;
    for (int j = 0; j < size; j++)
      if (processors[j].burstTime <= 0) sum += 1;

    if (sum == size) break;

  }

  /*
   * Use this as well in case of first arrival time is zero
   */
  int firstArrive = processors[0].arrivalTime;

  for (int i = 0; i < size; i++) {
    /*
     * Algorithm to calculate the results
     */
     processors[i].responseTime = processors[i].startTime - processors[i].arrivalTime;
     processors[i].burstTime = tempBurstTime[i];
     processors[i].turnaroundTime = processors[i].finishTime - processors[i].arrivalTime;
     processors[i].waitingTime = processors[i].turnaroundTime - processors[i].burstTime;
  }

  output(1);
}

/*
 * Output Function for tracking process works
 * @param {}
 */
void output(int condition)
{
  // PID, process’s response time, total waiting time and its turnaround time
  if (condition == 0) {

    printf("------------------------\n");
    printf("|   Before Processing  |\n");
    printf("------------------------\n");

    // Heading
    printf("%s\t%-5s\t%-5s\t%s\n",
      "PID",
      "Burst Time",
      "Arrival time",
      "Priority"
    );

    // Body processors
    for (int i = 0; i < size; i++) {
      printf("%d\t%-15d\t%-15d\t%d\n",
        processors[i].pid,
        processors[i].burstTime,
        processors[i].arrivalTime,
        processors[i].priority
      );
    }

    return;
  }

  float sumBurstTime = 0;
  float sumResponseTime = 0;
  float sumWaitingTime = 0;
  float sumTurnaroundTime = 0;

  printf("------------------------\n");
  printf("|   After Processing   |\n");
  printf("------------------------\n");

  printf("%s\t%-5s\t%-5s\t%s\n",
    "PID",
    "Response Time",
    "Waiting time",
    "Turnaround time"
  );
  for (int i = 0; i < size; i++) {

    printf("%d\t%-15d\t%-15d\t%d\n",
      processors[i].pid,
      processors[i].responseTime,
      processors[i].waitingTime,
      processors[i].turnaroundTime
    );

    sumBurstTime += processors[i].burstTime;
    sumResponseTime += processors[i].responseTime;
    sumWaitingTime += processors[i].waitingTime;
    sumTurnaroundTime += processors[i].turnaroundTime;
  }

  /* you should report the system’s throughput,
   * the average values of the response time, waiting time, and turnaround time
   */
   printf("Throughput: %.2f  %.2f  %.2f  %.2f\n",
     size/sumBurstTime,
     sumResponseTime/size,
     sumWaitingTime/size,
     sumTurnaroundTime/size
   );
}
