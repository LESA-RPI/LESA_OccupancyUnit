#ifndef INTEGRATEDTRACKINGALGORITHM_H   /* Include guard */
#define INTEGRATEDTRACKINGALGORITHM_H

extern int currID;
extern float occupantArray[5];
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
    //int nextHop[3];//Next possible sensors
     
};
struct Occupant getOccupant(int** grid);
char equiv(struct tuple a, struct tuple b);
char findLocation(struct tuple locs[64], struct tuple loc);
void filterLocations(struct Occupant *occ, int** grid);
char findCenterIndex(struct Occupant person);
void printOccupantData(struct Occupant person);
void getOccupantData(float* output,struct Occupant person_t1, struct Occupant person_t2);
void loadFrame(char* fileName, int (*result)[8]);
void destroyArray(int (*grid)[8]);
void printArray(int** grid);
void subtractArray(int ** dest,int (*first)[8],int (*second)[8]);
void exampleRun(float *output, int (*control)[8],int (*firstData)[8],int (*lastData)[8]);

#endif // INTEGRATEDTRACKINGALGORITHM_H