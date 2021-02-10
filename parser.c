//parses the input file into Process and event
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define QUEUE_CAPACITY 20
#define MAX_NAME_LENGTH 5

// Struct of process variables
struct Process{
    char name[3]; // Number of process
    char status[20]; // ready, blocked, or running
    int changed; // 1 if just changed, 0 if same
    char task[15];
};

// Struct for each of the queues
struct Queue{
    int front, rear, size; // First and last in the queue, and amount of 
    char** array; // Array of strings
    char name[10]; // Name of queue itself
};

// Function prototypes
struct Queue* createQueue(const char queueName[10]); // Initializes a queue of max length capacity
void addToQueue(struct Queue* queue, struct Process* process); // Adds a process to the rear of the queue
void removeFromQueue(struct Queue*, struct Process* process); // Removes a process from the front of the queue
void printQueue(struct Queue* queue); // Prints queue to output file
void writeQueue(struct Queue* queue, FILE* outFile); // Writes to output file
void printProcessStatus(struct Process* processes[20], int numProcesses); // Prints each process and status to terminal
void writeProcessStatus(struct Process* processes[20], int numProcesses, FILE* outFile); // Writes each process/state to output file
int findProcessLocation(struct Process* processes[20], char* processName);
void resetChanged(struct Process* processes[20], int numProcesses);

int main(int args, char* kwargs[])
{
	//int i;
	char* rch;
	char str[200];
	char LineInFile[40][300]; // 40 words, 300 letters each
	int lineP, lineQ;
	char* sch;
	char tokenizedLine[10][10];
    char *header; // Array of strings from first line of each input file
    int numProcesses = 0; // Number of processes initialized
    struct Process *processes[20]; // Holds each process as they are initialized
    int index = 0;
    
	FILE* fp1;
	FILE* fp2;
	fp1 = fopen("inp1.txt", "r");			//open the original input file
	fp2 = fopen("inp1_parsed.txt", "w");	//output the Process ID and event to another file. 
											//You can store in variables instead of printing to file

	lineP = 0;
	//i = 0;

    // Instantiate queues
    struct Queue* keyboard;
    struct Queue* disk;
    struct Queue* printer;
    printer = createQueue((char*)"Printer");
    keyboard = createQueue((char*)"Keyboard");
    disk = createQueue((char*)"Disk");

	printf("Started parsing...\n");

	//copy first line of the original file to the new file
	fgets(str, sizeof(str), fp1); // Line 0	

    /*
     * Parse first line for processes 
     */   
	header = strtok(str, " ");
    while(memcmp(header, "end", 3) != 0){
        
        if(header[0] != 'P') // Check for process
        { 
            printf("Compare return val: %i\n", strcmp(header, "end"));
            printf("An error occcurred parsing the first line.\n");
            printf("%s \n", header);
            printf("Characters: %i\n", strlen(header));
            return -1;
        }
        else
        {
            // Initialize new struct pointer, save in memory
            struct Process *newEntry; 
            newEntry = (struct Process *) malloc(sizeof(struct Process));
            // Enter struct data, parse through name and status in header
            strcpy(newEntry->name, header);
            header = strtok(NULL, " "); // Advance token to status
            strcpy(newEntry->status, header);
            newEntry->changed = 0;
            strcpy(newEntry->task, "");
            header = strtok(NULL, " "); // Advance token to next process
            // Check initialization
            printf("Name: %s, Status: %s, Changed: %i, Current task: %s\n",
                    newEntry->name, 
                    newEntry->status, 
                    newEntry->changed, 
                    newEntry->task);
            // Add to file output
            fprintf(fp2, "%s %s ", newEntry->name, newEntry->status);
            // Add to pointer array
            processes[numProcesses] = newEntry; 
            numProcesses++; //Increment total processes
            
        }
        
           
    }
     
    fprintf(fp2, "\n"); // Newline in file, first line complete
    
// Test code 
// This is working as of 2/10/21
//=====================================================================================
    // Find location in processes array of P8
    /*
    int loc = 0;
    
    loc = findProcessLocation(processes, "P8");
    printf("P8 is supposed to be in index [4], is actually in index [%i]\n", loc);
    // Add the first two items to the disk queue
    for (int i = 0; i < 2; i++)
    {
        addToQueue(disk, processes[i]);
    }
    printQueue(disk);
    // Find P1 and remove from disk
    int index;
    index = findProcessLocation(processes, "P1");
    printf("Index of P1 is %i.\n", index);
    removeFromQueue(disk, processes[0]);
    printQueue(disk);
    removeFromQueue(disk,processes[2]);
    removeFromQueue(disk, processes[1]);
    printQueue(disk);
    */
//=====================================================================================

	//parse each remaining line into Process event
	//while loop with fgets reads each line
	while (fgets(str, sizeof(str), fp1) != NULL) // Line 2
	{
        fprintf(fp2, str); // Print entire line to file & console
        printf(str);

		lineP = 0;
		rch = strtok(str, ":;.");					// use strtok to break up the line by : or . or ; This would separate each line into the different events
		while (rch != NULL)
		{
			strcpy(LineInFile[lineP], rch);			//copy the events into an array of strings
			lineP++;								//keep track of how many events are in that line
			rch = strtok(NULL, ":;.");				//needed for strtok to continue in the while loop
		} 

		//for each event (e.g. Time slice for P7 expires) pull out process number and event
		for (int i = 1; i < lineP - 1; i++)
		{
			lineQ = 0;
			sch = strtok(LineInFile[i], " ");
            
			while (sch != NULL)
			{
				strcpy(tokenizedLine[lineQ], sch);		//use strtok to break up each line into separate words and put the words in the array of strings
				lineQ++;								//count number of valid elements
				sch = strtok(NULL, " ");
            }
			//tokenizedLine has the event separated by spaces (e.g. Time slice for P7 expires)
			if (strcmp(tokenizedLine[1], "requests") == 0)						//Process requests an I/O device
			{
				//fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[1]);
				fprintf(fp2, "%s %s %s ", tokenizedLine[0], tokenizedLine[1], tokenizedLine[3]);
			}
			else if ((strcmp(tokenizedLine[2], "dispatched") == 0))				//Process is dispatched
			{
				fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[2]);
			}
			else if (strcmp(tokenizedLine[0], "Time") == 0)						//Process has timed off
			{
				fprintf(fp2, "%s %s ", tokenizedLine[3], tokenizedLine[4]);
			}
			else if (strcmp(tokenizedLine[3], "out") == 0)						//Process is swapped out
			{
				//fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[3]);
                index = findProcessLocation(processes, tokenizedLine[0]); // Find process in array
                if(strcmp(processes[index]->status, "Blocked"))
                {
                    strcpy(processes[index]->status, "Blocked/Suspend"); // Send to blocked/sspd state
                    
                }
                else if(strcmp(processes[index]->status, "Ready")
                    || strcmp(processes[index]->status, "Running"))
                {
                    strcpy(processes[index]->status, "Ready/Suspend");
                    processes[index]->changed = 1; 
                }
                else
                {
                    printf("Cannot swap out %s, not in the right starting state.\n", tokenizedLine[0]);
                }
                
			}
			else if (strcmp(tokenizedLine[3], "in") == 0)						//Process is swapped in
			{
				//fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[3]);
                index = findProcessLocation(processes, tokenizedLine[0]); // find process in array
                if(strcmp(processes[index]->status, "Ready/Suspend"))
                {
                    strcpy(processes[index]->status, "Ready");
                    processes[index]->changed = 1; 
                }
                else if (strcmp(processes[index]->status, "Blocked/Suspend"))
                {
                    strcpy(processes[index]->status, "Blocked");
                    processes[index]->changed = 1; 
                }
                else{
                    printf("Cannot swap in %s, not in the right starting state.\n", tokenizedLine[0]);
                }
			}
			else if (strcmp(tokenizedLine[1], "interrupt") == 0)				//An interrupt has occured
			{
				//fprintf(fp2, "%s %s ", tokenizedLine[4], tokenizedLine[1]);
                index = findProcessLocation(processes, tokenizedLine[4]); // find process in array
                if(strcmp(processes[index]->status, "Blocked"))
                {
                    strcpy(processes[index]->status, "Ready");
                    processes[index]->changed = 1; 
                }
                else if(strcmp(processes[index]->status, "Blocked/Suspend"))
                {
                    strcpy(processes[index]->status, "Ready/Suspend");
                    processes[index]->changed = 1;                     
                }
                else{
                    printf("Cannot interrupt %s, not in the right starting state.\n", tokenizedLine[4]);
                }
			}
			else																//Process has been terminated
			{
				//fprintf(fp2, "%s %s ", tokenizedLine[0], tokenizedLine[2]);
                index = findProcessLocation(processes, tokenizedLine[0]); // find process in array
                if(strcmp(processes[index]->status, "Running"))
                {
                    strcpy(processes[index]->status, "Release");
                    processes[index]->changed = 1; 
                }
                else{
                    printf("Cannot interrupt %s, not in the right starting state.\n", tokenizedLine[0]);
                }
			}
			
		}
        // Output string command to file and terminal
        printf(str);
        fprintf(fp2, str);
        // Output all processes and corresponding status to terminal and output file
        printProcessStatus(processes, numProcesses);
        writeProcessStatus(processes, numProcesses, fp2);
        //Output queues to terminal and output file
        printQueue(disk);
        printQueue(keyboard);
        printQueue(printer);
        writeQueue(disk, fp2);
        writeQueue(keyboard, fp2);
        writeQueue(printer, fp2);

        resetChanged(processes, numProcesses); // Reset all changed values for next loop
		fprintf(fp2, "\n\n");
	}
    /*for (int i = 0; i < sizeof(LineInFile); i++)
    {
        for (int j = 0; j < sizeof(LineInFile[i]); j++)
        {
            printf("%c", LineInFile[i][j]);
        }
        printf("\n");
    }
	printf("Parsing complete\n\n");*/

	fclose(fp1);
	fclose(fp2);

	return 0;
}

struct Queue* createQueue(const char queueName[10])
// Initializes a queue of specified queue type
// Returns an empty queue of size capacity
{
    // Allocate memory location 
    struct Queue* queue = (struct Queue *)malloc(sizeof(struct Queue));
    // Initialize queue variables
    strcpy(queue->name, queueName);
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    //Initialize array
    queue->array = malloc(QUEUE_CAPACITY * sizeof(char*));
    for (int i = 0; i < QUEUE_CAPACITY; i++)
    // Initialize each pointer
    {
        queue->array[i] = malloc(MAX_NAME_LENGTH * sizeof(char));
    }
    return queue;
}

int isEmpty(struct Queue* queue)
{
    return (queue->size == 0); 
}

int isFull(struct Queue* queue)
{
    return(queue->size == QUEUE_CAPACITY);
}

void addToQueue(struct Queue* queue, struct Process* process)
// Adds a process to the end of a queue
{

    if(isFull(queue)) // Check capacity
    {
        printf("Unable to add to %s to %s queue because it is full.",
                process->name,
                queue->name);
        return;
    }

    if (!isEmpty(queue)) // Only move end pointer if array is already initialized 
    {
        queue->rear = (queue->rear+1) // Move rear pointer to new entry
                    % QUEUE_CAPACITY; // Loop around if needed
    }
    
    strcpy(queue->array[queue->rear], process->name);// Add process name to queue array
    //queue->array[queue->rear] = process->name; 
    queue->size += 1; //increase size
}

void removeFromQueue(struct Queue* queue, struct Process* process)
// Removes a processs from the beginning of the queue
{
    // Check capacity
    if(isEmpty(queue))
    {
        printf("Process %s cannot be removed becasue the %s queue is empty.",
                process->name,
                queue->name);
        return;
    }
    // Notify user if this was not next in queue
    if(strcmp(queue->array[queue->front], process->name))
    {
        printf("The process %s is not at the beginning of the %s queue.\n",
                process->name,
                queue->name);
        return;
    }
    queue->front = (queue->front+1) // Move front pointer to new entry
                    % QUEUE_CAPACITY; // Loop around if needed
    queue->size -= 1; // Decrease size
}

void printQueue(struct Queue* queue)
// Prints the queue out to the terminal
{
    printf("%s queue: ", queue->name);
    int i = queue->front; //Iter
    while(i != queue->rear + 1)
    {
        printf("%s ", queue->array[i]);   
        i++;
        if (i == QUEUE_CAPACITY) // Loop around queue if max value is reached
            i = 0; 
    }
    printf("\n");
}

void writeQueue(struct Queue* queue, FILE* outFile)
// Writes the queue to the output file
{
    fprintf(outFile, "%s queue: ", queue->name);
    int i = queue->front; //Iter
    while(i != queue->rear + 1)
    {
        fprintf(outFile, "%s ", queue->array[i]);   
        i++;
        if (i == QUEUE_CAPACITY) // Loop around queue if max value is reached
            i = 0; 
    }
    fprintf(outFile, "\n");
}

void printProcessStatus(struct Process* processes[20], int numProcesses)
// Prints each process name and status to the screen
{
    for (int i = 0; i < numProcesses; i++)
    {
        printf("%s %s", 
                processes[i]->name,
                processes[i]->status);
        if(processes[i]->changed)
            printf("*"); // Changed states recently
        printf(" "); // Add space at end
    }
}

void writeProcessStatus(struct Process* processes[20], int numProcesses, FILE* outFile)
// writes process and status to the output file
{ 
    for (int i = 0; i < numProcesses; i++)
    {
        fprintf(outFile, "%s %s ", 
                processes[i]->name,
                processes[i]->status);
        if(processes[i]->changed)
            fprintf(outFile, "*"); // Changed states recently
        fprintf(outFile, " "); // Add space at end
    }
}

int findProcessLocation(struct Process* processes[20], char* processName)
// Gets the array location of the process
{    
    for (int i = 0; i < 20; i++)
    {
        
        if (strcmp(processes[i]->name, processName) == 0)
            return i; // Process success, return index
        
    }
    // If it gets here it did not find a match
    return -1;  
}

void resetChanged(struct Process* processes[20], int numProcesses)
// Set all "changed" variables back to 0
{
    for(int i = 0; i < numProcesses; i++)
    {
        processes[i]->changed = 0;
    }
}