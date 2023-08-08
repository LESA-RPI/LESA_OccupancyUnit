#ifndef TRACKING_ALGO_H
#define TRACKING_ALGO_H

#define N 8
#define MIN_BLOB_SIZE 5
#define BIG_BLOB_BASELINE_THRESHOLD 20
#define MAX_BLOBS (N * N / MIN_BLOB_SIZE)
#define THRESHOLD 500
#define DEBUG 0
#define FIFO_SIZE 40
#define PYTHON_VIEWER 1
#define STM_USER
#define VARIATION 100
#define MAX_FRAMES_TO_LOOK_BACK 3



typedef struct {
    float x;
    float y;
} Point;

typedef struct {
    Point center_of_mass;
    float proportional_average;
    int ID;
    float direction; // Changed direction to store angles in degrees
    float velocity;
} BlobInfo;

typedef struct {
    BlobInfo Blobs[MAX_BLOBS];
    int num_blobs;
    float delta_X;//added deltaX
    float delta_Y;//added deltaY
} FrameInfo;

typedef struct {
    FrameInfo buffer[FIFO_SIZE];
    int front;
    int rear;
    int count;
    int current_ID; // Added current_ID field
} FIFObuffer;

// Function declarations
float find_proportional_average(int matrix[N][N], Point center_of_mass);
FrameInfo parse_frame(int matrix[N][N], float deltaX, float deltaY);
bool next_permutation(int *first, int *last);
void link_latest_frame_to_previous(FIFObuffer *fifobuffer);
void calculate_blob_velocity_and_direction(FIFObuffer *fifobuffer);
void init_fifo(FIFObuffer *fifo);
bool is_full(FIFObuffer *fifo);
bool is_empty(FIFObuffer *fifo);
void dequeue(FIFObuffer *fifo);
void enqueue(FIFObuffer *fifo, FrameInfo frame_info);
void print_fifo(FIFObuffer *fifobuffer);
void print_matrix(int matrix[N][N]);
void print_frame_info(FrameInfo frame_info);
void print_frame_info_simple(FrameInfo frame_info);
void generate_random_matrix(int matrix[N][N]);
void subtract_matrices(int result[N][N], const int matrix1[N][N], const int matrix2[N][N]);

#endif // STM_DATAPACKET_H
