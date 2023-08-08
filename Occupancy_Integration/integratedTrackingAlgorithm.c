#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

//Current Person's ID
int currID = 0;

//Class struct to hold the current position of an occupant
struct tuple{
    uint row;
    uint col;
};

//Class Struct to hold and manipulate the data from one ToF reading
struct Occupant{
    int ID; //ID of the person in the grid
    struct tuple location[64];//Possible current locations of the occupant
    time_t timestamp;//Timestamp of the reading
    int color;
    float direction;//Current Direction the person is moving
    float distance;//Distance moved on the last 2 frames
    float velocity;//Current velocity of occupant
    int nextHop[3];//Next possible sensors
     
};

//Function to find the locations of the occupants at a given time.
//Input - 8x8 2D array passed by reference
//Output - An occupant object containing all its locations on the grid
struct Occupant getOccupant(int (*grid)[8]){

    //Creates the person object
    struct Occupant person;//The occupant
    person.ID = currID;//Sets the ID of the occupant to the current ID
    unsigned char index = 0;//Follows the current number of "Possible Locations" that the occupant can be at in the grid

    //For each entry in the grid
    for(int row = 0; row<8;row++){
        for(int col = 0; col<8;col++){

            //If there is there is a possible occupant in the coordinate
            if(grid[row][col]>500){

                //Create a coordinate tuple object and set it to the current row and col
                struct tuple coordinate;
                coordinate.row = row;
                coordinate.col = col;

                //Save the coordinate to the person's location
                person.location[index] = coordinate;
                index++;//Increase the index
            }
        }
    }
    for(unsigned char i = index; i<64; i++){
        struct tuple coordinate;
        coordinate.row = 0;
        coordinate.col = 0;
        person.location[i] = coordinate;
    }
    
    currID++;//Increase the current ID
    return person;//Return the person created
}

//Checks if two tuple structures are equivalent
//Inputs: Two tuple objects
//Output: If the two tuples are the same, 1. If they are not, 0.
char equiv(struct tuple a, struct tuple b){

    //If the row and col values of the two tuples are the same return 1
    if(a.row == b.row && a.col == b.col){
        return 1;
    }
    //Else, return 0
    return 0;
}

//If a tuple is present in an array of tuples, its index is returned.
//If the tuple is not found on the array, -1 is returned
//Input: A tuple array with 64 entires, and a tuple object
//Output: If the tuple is present in the list, its index is returned, else, -1 will be returned.
char findLocation(struct tuple locs[64], struct tuple loc){

    //Iterate through the array
    for(int i = 0; i<64; i++){
        //If the tuple is found, return its index
        if(equiv(locs[i],loc)){
            return i;
        }
    }
    //If the tuple is not found, return -1
    return -1;
}

//Filters the grid by eliminating data points that are not connected to a cluster
//The locations without people will be removed from the grid and the occupant's properties
//Inputs: A pointer to an occupant object, and the 8x8 grid
//Output: No output, but the person object and the grid objects are modified.
void filterLocations(struct Occupant *occ, int (*grid)[8]){

    struct Occupant person = *occ;
    //A temporary array for manipulating the occupant's location array
    struct tuple locs[64];
    memcpy(locs, person.location,64);//The occupant's location is copied into the temporary array
    
    //For each possible occupant location recorded in the occupant's object
    for(int i = 0; i<64; i++){
        //If the end of location is achieved, with the remaining entries being (0,0), break the for loop
        if(i!=0&&locs[i].col==0&&locs[i].row==0){
            break;
        }

        //Tuples to register the position right above, below, to the left, and to the right of the current coordinate
        struct tuple top,bot,left,right;
        top.col = locs[i].col;
        top.row = locs[i].row -1;
        bot.col = locs[i].col;
        bot.row = locs[i].row+1;
        left.col = locs[i].col-1;
        left.row = locs[i].row;
        right.col = locs[i].col+1;
        right.row = locs[i].row;

        //Checks if the top, left, bottom, and right locations are occupied, forming a cluster
        char connectedTop = findLocation(locs,top);
        char connectedBot = findLocation(locs,bot);
        char connectedLeft = findLocation(locs,left);
        char connectedRight = findLocation(locs,right);

        //If the current node is not connected to a cluster, set it to (0,0)
        if(!((connectedTop!=-1)||(connectedBot!=-1)||(connectedLeft!=-1)||(connectedRight!=-1))){
            
            //Set the grid location to zero
            grid[(int)locs[i].row][(int)locs[i].col] = 0;

            //Create a tuple with properties (0,0)
            struct tuple tup;
            tup.row = 0;
            tup.col = 0;

            //Set the location to the tuple
            locs[i] = tup;
        }
    }
    //Replace the occupant's original list of positions for the modified version
    memcpy(person.location,locs,64);
    *occ = person;
}

//Finds the center index of the cluster of possible positions recorded in the occupant's object
//Index: An occupant object
//Output: The index in the center of the cluster
char findCenterIndex(struct Occupant person){

    //A temporary array for manipulating the occupant's location array
    //struct tuple locs[64];
    //memcpy(locs, person.location,64);//The occupant's location is copied into the temporary array
    //printf("%d\n",locs[2].row);
    unsigned char index = 0;//The current index being looked at
    
    //Iterate through the possible locations
    for(int i = 0; i < 64; i++){
        //If the valid locations are over, break the loop
        printf("%d\n",person.location[i].row);
        if(i!=0&&person.location[i].col==0&&person.location[i].row==0){
            break;
        }
        //Increment the index
        index++;
    }

    //Divide the index by 2
    index /=2;

    //Return index
    return index;
}

//Function to print the occupant relevant data such as direction, speed, and distance travelled
void printOccupantData(struct Occupant person){
    printf("The occupant of ID %d is currently moving at %f units of distance per second and in the %f angled direction.\n", 
    person.ID, person.velocity, person.direction);

    printf("The distance of %f units of distance have been travelled in the last 2 seconds", person.distance);
}

//Calculates speed, direction, and distance travelled and sets it in an occupant object
//Input: 2 Occupant Objects
//Output: An occupant object
struct Occupant getOccupantData(struct Occupant person_t1, struct Occupant person_t2){

    //The result occupant that we will set the data
    struct Occupant result;
    //Sets the ID of the occupant to the same ID as the input occupants
    result.ID = person_t1.ID;

    //Temporary tuple arrays to store all possible locations
    struct tuple locs1[64];
    struct tuple locs2[64];

    memcpy(locs1, person_t1.location,64);
    memcpy(locs2, person_t1.location,64);

    //The center indexes of the "occupant clusters"
    unsigned char index1 = findCenterIndex(person_t1);
    unsigned char index2 = findCenterIndex(person_t2);
    


    //The initial position of the occupant
    unsigned char row_t1 = person_t1.location[index1].row;
    unsigned char col_t1 = person_t1.location[index1].col;
    
    //The final position of the occupant
    unsigned char row_t2 = person_t2.location[index2].row;
    unsigned char col_t2 = person_t2.location[index2].col;

    //The change in position of the occupant
    float delta_x = col_t2 - col_t1;
    float delta_y = row_t2 - row_t1;
    

    //The shortest possible distance traveled by the occupant (hipotenuse)
    float dist = sqrt((pow(delta_x,2))+(pow(delta_y,2)));
    
    //If the distance traveled is non zero
    if(dist!=0){

        //Calculate the sin, and the direction the person is currently travelling to
        float sin = delta_x/dist;
        float angle = asin(sin);

        //Sets the direction, distance travelled, and velocity of the occupant
        result.direction = angle;
        result.distance = dist;
        result.velocity = dist/2;
    
    }else{ //If the occupant has not moved

        //Sets the direction to an unvalid (out of range) value
        result.direction = 360;
        //Sets the distance and velocity to zero
        result.distance = 0;
        result.velocity = 0;
    }

    memcpy(result.location, person_t2.location, 64);
    //Returns the result
    return result;
}

//Loads the ToF sensor reading into a 8x8 integer 2D array
//Inputs : (filename) - A char array containing the name of the file
// (result) - A 2D array passed by reference to store the reading
void loadFrame(char* fileName, int (*result)[8]){

    FILE* grid; //The file
    char ch; //Character to hold the current character in the file
    char distance[4] = "";//The number to be added to the array

    // Opening file in reading mode
    grid = fopen(fileName, "r");

    //Column and rows to follow current location on the grid
    unsigned char column = 0;
    unsigned char row = 0;
    
    //Counter to count which is the current letter index from the number
    unsigned char counter = 0;

    //While not at the end of the file
    while (!feof(grid)) {

        //Get the current character
        ch = fgetc(grid);
        
        //If we are at the end of a line
        if(ch == ']'){

            //Set the counter to 0
            counter = 0;
            //Turn the current number into an integer and save it into the result
            result[row][column] = atoi(distance);
            
            //Reset the distance
            for(int i = 0; i< 4;i++){
                distance[i] = '\0';
            }

            //Increment the row 
            row++;
            //Reset the column number
            column = 0;

        //If not at the end of the line
        }else{
            //If the character is not a number, do nothing
            if(ch == ' '|| ch == '[' || ch == '\n'){
            
            }
            //If the character separates numbers
            else if(ch == ','){
                counter = 0;
                //Add the number to the result
                result[row][column] = atoi(distance);
                //Increment column
                column++;
                
                //Reset the distance
                for(int i = 0; i< 4;i++){
                    distance[i] = '\0';
                }
            }
            //If the character is a number
            else{
                //Save the current character to the distance
                distance[counter] = ch;
                //Increase the counter
                counter++;
            }
        }
    }
    //Close the grid
    fclose(grid);
}

//Frees the memory of the array
//Input: (grid) - 2D 8x8 Grid Array to pass by reference
void destroyArray(int (*grid)[8])
{
    //Frees the pointer to the array
    free(*grid);
    //Frees the array
    free(grid);
}

//Prints the 2D 8x8 Grid
void printArray(int (*grid)[8]){
    //Iterates through the array
    for(int i = 0; i< 8; i++){
        printf("[");
        for(int j = 0; j<8; j++){
            //Prints the item on the grid
            printf("%d", grid[i][j]);
            if(j!=7){
                printf("\t");
            }
        }
        printf("]\n");
    }
}

//Subtracts two 2D grids, and stores on another grid
//Inputs: dest   - The destination where the subtraction will be stored
//        first  - The array to be subtracted from
//        second - The array that will be subtracted from the first array
//Comments: a closed form equation of this would look like:   
//          dest = first - second
void subtractArray(int (*dest)[8],int (*first)[8],int (*second)[8]){

    //Iterate through the grid
    for(int i = 0; i<8;i++){
        for(int j = 0; j<8;j++){
            //Subtracts second from first and stores it on the dest
            dest[i][j] = first[i][j] - second[i][j];
        }
    }

}



//An example of how running the entire process would look like
//Made for future reference
void exampleRun(){
    int control[8][8] = {{2236, 2167, 2670, 2752, 2826, 2486, 2833, 2595},
    {2235, 2179, 2713, 2754, 2823, 2451, 2831, 2602},
    {2230, 2169, 2686, 2765, 2810, 2487, 2839, 2584},
    {2230, 2173, 2675, 2726, 2814, 2473, 2828, 2602},
    {2232, 2170, 2680, 2745, 2802, 2464, 2836, 2583},
    {2247, 2178, 2670, 2742, 2811, 2475, 2822, 2587},
    {2232, 2179, 2690, 2740, 2845, 2476, 2840, 2620},
    {2235, 2168, 2711, 2751, 2826, 2467, 2819, 2596}};

    int firstData[8][8] = {{2236, 2167, 2670, 2752, 2826, 2486, 2833, 2595},
    {2235, 2179, 2713, 2754, 2823, 2451, 2831, 2602},
    {2230, 2169, 2686, 2765, 2810, 2487, 2839, 2584},
    {2230, 2173, 2675, 2726, 2814, 2473, 2828, 2602},
    {2232, 2170, 2680, 2745, 2802, 2464, 2836, 2583},
    {2247, 2178, 2670, 2742, 2811, 2475, 2822, 2587},
    {1132, 2179, 2690, 2740, 2845, 2476, 2840, 2620},
    {1035, 2168, 2711, 2751, 2826, 2467, 2819, 2596}};

    int lastData[8][8] = {{2236, 2167, 2670, 2752, 2826, 2486, 2833, 2595},
    {2235, 979, 2713, 2754, 2823, 2451, 2831, 2602},
    {2230, 1069, 2686, 2765, 2810, 2487, 2839, 2584},
    {2230, 2173, 2675, 2726, 2814, 2473, 2828, 2602},
    {2232, 2170, 2680, 2745, 2802, 2464, 2836, 2583},
    {2247, 2178, 2670, 2742, 2811, 2475, 2822, 2587},
    {2232, 2179, 2690, 2740, 2845, 2476, 2840, 2620},
    {2235, 2168, 2711, 2751, 2826, 2467, 2819, 2596}};

    //Creates 2D arrays to hold the normalized data
    int norm_data1[8][8];
    int norm_data2[8][8];

    //Subtracts the values of the readings from the baseline data to create normalized grids
    subtractArray(norm_data1, control, firstData);
    subtractArray(norm_data2, control, lastData);

    //Processes the data and finds the location of the occupants in the normalized grids
    struct Occupant frame1 = getOccupant(norm_data1);
    struct Occupant frame2 = getOccupant(norm_data2);

    //Filters the locations of the frames
    filterLocations(&frame1, norm_data1);
    filterLocations(&frame2, norm_data2);


    //Gets the final occupant data by processing the last two occupant frames
    struct Occupant finalOccupant = getOccupantData(frame1,frame2);

    //Prints the normalized arrays
    printf("First Reading\n");
    printArray(norm_data1);
    printf("\n+++++++++++++++++++++++++++++++++++++\n");
    printf("Second Reading\n");
    printArray(norm_data2);
    printf("\n");
 
    //Prints the resulted occupant data
    printOccupantData(finalOccupant);
    vTaskDelete(NULL);

}


void app_main(void)
{   
    xTaskCreate(&exampleRun, "exampleRun", 8192, NULL,5,NULL);  
}