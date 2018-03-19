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
  int allBurstTime = 0;
  int tempBurstTime[size];

  // Sort by ArrivalTime
  for (int i = 0; i < size-1; i++) {
    for (int j = i+1; j < size; j++) {
      if ( processors[i].arrivalTime > processors[j].arrivalTime ) {
        struct Candidate temp = processors[i];
        processors[i] = processors[j];
        processors[j] = temp;
      }
    }
  }

  /* Find all burst time
   * easy for processing next Time
   */
  for (int i = 0; i < size; i++) {
    allBurstTime += processors[i].burstTime;

    tempBurstTime[i] = processors[i].burstTime;

    // Set default value
    processors[i].responseTime = -1;
    processors[i].turnaroundTime = 0;

    processors[i].startTime = 0;
    processors[i].waitingTime = 0;
  }

  int count = 1, index = 0;
  for (int i = 0; i < allBurstTime; i++) {

    // printf("Index: %d, PID: %d, Burst Time: %d\n", index, processors[index].pid, processors[index].burstTime);
    processors[index].burstTime -= 1;

    // Set ResponsTime when first time process
    if (processors[index].responseTime == -1) {
      processors[index].responseTime = i;
    }
    count++;

    /*
     * Change the processor when quantum process
     * or burstTime of that processor is zero (Terminate)
     */
    if (count-1 == quantum || processors[index].burstTime <= 0) {
      index++; count = 1;

      processors[index-1].finishTime = i;

      /*
       * If the next processor also has burstTime equal 0
       * We will find the next one that remain of allBurstTime
       * Otherwise, we end of processing.
       */
      int terminate = 0;
      while (1) {

        // Restore index when out of bound.
        if (index >= size) index = 0;

        // If all burstTime of processors is zero we terminate system.
        if (terminate++ == size) break;

        // printf("Terminate: %d\n", terminate);

        /*
         * If burstTime is still 0
         * We find index again.
         */
        if (processors[index].burstTime == 0) {
          index++; continue;
        } else {
          // We found the next processors.
          break;
        }
      }

      // We woould like to make sure it is actual finished.
      if (terminate > size) break;

      /* If at the first time of processing,
       * Let start with waitingTime equal currentTime (or i);
       */
       processors[index].startTime = i;

      if (processors[index].finishTime == 0) {
        processors[index].waitingTime += i - processors[index].arrivalTime;
      } else {
        /*
         * Calculate waiting Time
         * StartTime is currentTime
         * FinishTime is lastest finished processing of processor.
         */
        processors[index].waitingTime += processors[index].startTime - processors[index].finishTime;
      }
      // if ( processors[index].pid == 3) {
        // printf("PID %d, LastTime: %d, startTime: %d, Remain: %d\n", processors[index].pid, processors[index].finishTime, processors[index].startTime, processors[index].startTime - processors[index].finishTime);
      // }
    }
  }

  /*
   * Restore burstTime by tempBurstTime
   * and calculate turnaroundTime
   */
  for(int i = 0; i < size; i++) {
    processors[i].burstTime = tempBurstTime[i];
    processors[i].turnaroundTime =  processors[i].waitingTime + processors[i].burstTime;
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
     sumBurstTime/size,
     sumResponseTime/size,
     sumWaitingTime/size,
     sumTurnaroundTime/size
   );
}
