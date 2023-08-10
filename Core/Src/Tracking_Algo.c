#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include "Tracking_Algo.h"

#ifndef TRACKING_ALGO_H


#define uint16_t int
#define uint8_t int
#define uint32_t int

#define N 8
#define MIN_BLOB_SIZE 3
#define MAX_BLOBS (N * N / MIN_BLOB_SIZE)
#define MAX_BLOBS 5
#define THRESHOLD 500
#define DEBUG 0
#define FIFO_SIZE 2
#define PYTHON_VIEWER 1
#define VARIATION 200



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


#endif

// Global variables to keep track of the blob information during DFS traversal
int blob_size;
int blob_sum_x, blob_sum_y;

int visited[N][N];  // Track visited cells

// DFS function
void DFS(int matrix[N][N], int x, int y, Point *sum, int *count) {
	// Check for out-of-bounds
	if (x < 0 || x >= N || y < 0 || y >= N) {
		return;
	}

	// Check if the cell has been visited or is below the threshold
	if (visited[x][y] || matrix[x][y] <= THRESHOLD) {
		return;
	}

	visited[x][y] = 1;
	*count += 1;
	sum->x += x;
	sum->y += y;

	// Visit the neighboring cells
	if ((matrix[x][y] + VARIATION) > matrix[x - 1][y]) {
		DFS(matrix, x - 1, y, sum, count);   // Left
	}
	if ((matrix[x][y] + VARIATION) > matrix[x + 1][y]) {
		DFS(matrix, x + 1, y, sum, count);   // Right
	}
	if ((matrix[x][y] + VARIATION) > matrix[x][y - 1]) {
		DFS(matrix, x, y - 1, sum, count);   // Up
	}
	if ((matrix[x][y] + VARIATION) > matrix[x][y + 1]) {
		DFS(matrix, x, y + 1, sum, count);   // Down
	}
}

int find_centers_of_mass(int matrix[N][N], Point *center_of_mass) {
	// Initialize visited matrix
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			visited[i][j] = 0;
		}
	}

	int blob_count = 0;

	// Traverse the matrix
	bool not_finish_visiting = 1;
	while (not_finish_visiting){
	    int newhigh , highx , highy;
	    newhigh=-1;
    	for (int i = 0; i < N; i++) {
    		for (int j = 0; j < N; j++) {
    		    //printf("%d--%d---%d--%d\n",i,j,matrix[i][j] > newhigh && matrix[i][j] > THRESHOLD && !visited[i][j],(i==j)&&i==(N-1));
    			if (matrix[i][j] > newhigh && matrix[i][j] > THRESHOLD && !visited[i][j]) {
                    newhigh=matrix[i][j];
                    highx=i;highy=j;
    			}else if((i==j)&&i==(N-1)&&newhigh==-1){not_finish_visiting=false;}
    		}
    	}
		Point sum = { 0, 0 };
		int count = 0;
		DFS(matrix, highx, highy, &sum, &count);
		if (count >= MIN_BLOB_SIZE) {
			center_of_mass[blob_count].x = sum.x / count;
			center_of_mass[blob_count].y = sum.y / count;
			blob_count++;
		}
		//printf("Blob dectected %d\n",blob_count);

	}

	return blob_count;
}

// Function to find the proportional average value of a blob around its center of mass
float find_proportional_average(int matrix[N][N], Point center_of_mass) {
	int x = (int) (center_of_mass.x);
	int y = (int) (center_of_mass.y);
	float sum = 0.0f;
	float weight_sum = 0.0f;

	// Iterate through a 2x2 grid around the center of mass
	for (int dx = 0; dx <= 1; dx++) {
		for (int dy = 0; dy <= 1; dy++) {
			// Check if the cell is within the matrix bounds and belongs to the blob (above threshold)
			if (x + dx
					>= 0&& x + dx < N && y + dy >= 0 && y + dy < N && matrix[x + dx][y + dy] >= THRESHOLD) {
				// Calculate the weight based on the distance from the center of mass
				float weight = (
						dx == 0 ?
								1.0f - fabsf(y + dy - center_of_mass.y) :
								1.0f - fabsf(x + dx - center_of_mass.x));
				sum += (float) matrix[x + dx][y + dy] * weight; // Sum the weighted values
				weight_sum += weight; // Sum the weights for normalization
#if (DEBUG&&!PYTHON_VIEWER)
                printf("cell: %f\t wight:%f\n",(float)matrix[x + dx][y + dy],weight);
                #endif
			}
		}
	}

	return sum / weight_sum; // Return the proportional average value
}
// Function to parse a frame and extract blob information
FrameInfo parse_frame(int matrix[N][N], float deltaX, float deltaY) {
	FrameInfo frame_info;
	Point centers_of_mass[MAX_BLOBS];

	frame_info.num_blobs = find_centers_of_mass(matrix, centers_of_mass);

	frame_info.delta_X = deltaX;
	frame_info.delta_Y = deltaY;

	for (int i = 0; i < frame_info.num_blobs; i++) {
		frame_info.Blobs[i].center_of_mass = centers_of_mass[i];
		frame_info.Blobs[i].proportional_average = find_proportional_average(
				matrix, centers_of_mass[i]);
		frame_info.Blobs[i].direction = 0.0f; // Set default direction as 0
		frame_info.Blobs[i].velocity = 0.0f;  // Set default velocity as 0
#if (DEBUG && !PYTHON_VIEWER)
        printf("Blob %d center of mass: (%.2f %.2f)\n", i + 1, frame_info.Blobs[i].center_of_mass.x, frame_info.Blobs[i].center_of_mass.y);
        printf("Proportional average for blob %d: %.2f\n", i + 1, frame_info.Blobs[i].proportional_average);
#endif
	}

	return frame_info;
}

bool next_permutation(int *first, int *last) {
	if (first == last)
		return false;
	int *i = last - 1;
	while (i > first && *(i - 1) >= *i)
		--i;
	if (i == first)
		return false;
	int *j = last - 1;
	while (*j <= *(i - 1))
		--j;
	int temp = *(i - 1);
	*(i - 1) = *j;
	*j = temp;
	for (j = last - 1; i < j; ++i, --j) {
		temp = *i;
		*i = *j;
		*j = temp;
	}
	return true;
}

void link_latest_frame_to_previous(FIFObuffer *fifobuffer) {
	if (fifobuffer->count == 0) {
		return;
	}

	FrameInfo *latest_frame = &(fifobuffer->buffer[fifobuffer->rear]);
	int second_latest_frame_index = (fifobuffer->rear + FIFO_SIZE - 1)
			% FIFO_SIZE;
	FrameInfo *second_latest_frame =
			&(fifobuffer->buffer[second_latest_frame_index]);

	if (fifobuffer->count == 1) {
		for (int i = 0; i < latest_frame->num_blobs; i++) {
			fifobuffer->current_ID++;
			latest_frame->Blobs[i].ID = fifobuffer->current_ID;
		}
	} else {
		int num_blobs = latest_frame->num_blobs;
		int num_blobs_prev = second_latest_frame->num_blobs;
		float min_distance = INFINITY;
		int min_permutation[num_blobs];
		int permutation[num_blobs];
		for (int i = 0; i < num_blobs; i++) {
			permutation[i] = i;
		}

		do {
			float distance_sum = 0.0f;
			for (int i = 0; i < num_blobs; i++) {
				if (permutation[i] < num_blobs_prev) {
					float dx =
							latest_frame->Blobs[i].center_of_mass.x
									- second_latest_frame->Blobs[permutation[i]].center_of_mass.x;
					float dy =
							latest_frame->Blobs[i].center_of_mass.y
									- second_latest_frame->Blobs[permutation[i]].center_of_mass.y;
					distance_sum += sqrtf(dx * dx + dy * dy);
				}
			}

			if (distance_sum < min_distance) {
				min_distance = distance_sum;
				memcpy(min_permutation, permutation, sizeof(permutation));
			}
		} while (next_permutation(permutation, permutation + num_blobs));

		for (int i = 0; i < num_blobs; i++) {
			if (min_permutation[i] < num_blobs_prev) {
				latest_frame->Blobs[i].ID =
						second_latest_frame->Blobs[min_permutation[i]].ID;
			} else {
				fifobuffer->current_ID++;
				latest_frame->Blobs[i].ID = fifobuffer->current_ID;
			}
		}
	}
}


void calculate_blob_velocity_and_direction(FIFObuffer *fifobuffer) {
	if (fifobuffer->count < 2) {
		return;
	}

	FrameInfo *latest_frame = &(fifobuffer->buffer[fifobuffer->rear]);
	int second_latest_frame_index = (fifobuffer->rear + FIFO_SIZE - 1)
			% FIFO_SIZE;
	FrameInfo *second_latest_frame =
			&(fifobuffer->buffer[second_latest_frame_index]);

	for (int i = 0; i < latest_frame->num_blobs; i++) {
		int blob_id = latest_frame->Blobs[i].ID;
		float sum_velocity = 0.0f;
		float sum_direction = 0.0f;
		float weight_sum = 1.0f;

		// Calculate the current velocity and direction using the latest two frames
		for (int j = 0; j < second_latest_frame->num_blobs; j++) {
			if (second_latest_frame->Blobs[j].ID == blob_id) {
				float dx = latest_frame->Blobs[i].center_of_mass.x
						- second_latest_frame->Blobs[j].center_of_mass.x;
				float dy = latest_frame->Blobs[i].center_of_mass.y
						- second_latest_frame->Blobs[j].center_of_mass.y;
				float distance = sqrtf(dx * dx + dy * dy);
				float direction = atan2f(dy, dx);
				sum_velocity += distance;
				sum_direction += direction * 360.0f / 2 / M_PI;
				break;
			}
		}

		// Replace the latest velocity and direction by tracing back into the previous frame
		for (int j = 0; j < second_latest_frame->num_blobs; j++) {
			if (second_latest_frame->Blobs[j].ID == blob_id) {
				sum_velocity += second_latest_frame->Blobs[j].velocity * 0.4;
				sum_direction += second_latest_frame->Blobs[j].direction * 0.4;
				weight_sum += 0.4;
				break;
			}
		}

		latest_frame->Blobs[i].velocity = sum_velocity / weight_sum;
		latest_frame->Blobs[i].direction = sum_direction / weight_sum;
	}
}

// Function to initialize the FIFObuffer
void init_fifo(FIFObuffer *fifo) {
	fifo->front = 0;
	fifo->rear = -1;
	fifo->count = 0;
	fifo->current_ID = 0; // Initialize current_ID to zero
}

// Function to check if the FIFObuffer is full
bool is_full(FIFObuffer *fifo) {
	return (fifo->count == FIFO_SIZE);
}

// Function to check if the FIFObuffer is empty
bool is_empty(FIFObuffer *fifo) {
	return (fifo->count == 0);
}

// Function to remove the oldest FrameInfo from the FIFObuffer
void dequeue(FIFObuffer *fifo) {
	if (!is_empty(fifo)) {
		fifo->front = (fifo->front + 1) % FIFO_SIZE;
		fifo->count--;
	}
}

// Function to add a new FrameInfo to the FIFObuffer
void enqueue(FIFObuffer *fifo, FrameInfo frame_info) {
	if (is_full(fifo)) {
		dequeue(fifo);
	}
	fifo->rear = (fifo->rear + 1) % FIFO_SIZE;
	fifo->buffer[fifo->rear] = frame_info;
	fifo->count++;

	link_latest_frame_to_previous(fifo);
	calculate_blob_velocity_and_direction(fifo);

#if (!PYTHON_VIEWER&&DEBUG)
    printf("Enqueued FrameInfo at address: %p\n",
            (void*) &(fifo->buffer[fifo->rear]));
#endif
}

// Function to print the contents of the FIFObuffer
void print_fifo(FIFObuffer *fifobuffer) {
	if (is_empty(fifobuffer)) {
		printf("FIFObuffer is empty.\n");
		return;
	}

	int index = fifobuffer->front;
	for (int i = 1; i <= fifobuffer->count; i++) {
		printf("Frame %d:\n", i);
		print_frame_info(fifobuffer->buffer[index]);
		index = (index + 1) % FIFO_SIZE;
	}
}

// Function to print the matrix on the console
void print_matrix(int matrix[N][N]) {
#if (1)
	printf("##Matrix##|");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d|", matrix[i][j]);
        }
    }
    printf("\n");

#else
#if (1)
	printf("Matrix:\n");
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (matrix[i][j] >= THRESHOLD) {
				printf("\033[38;5;10m%4d\033[0m|", matrix[i][j]);
			} else {
				printf("\033[38;5;9m%4d\033[0m|", matrix[i][j]);
			}
		}
		printf("\n");
	}
#else
    printf("Matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if(matrix[i][j]>=THRESHOLD){
                printf("■");
            }
            else{
                printf("□");
            }
        }
        printf("\n");
    }

#endif
#endif
}
// Function to print the information of a single frame, including Blob's ID
void print_frame_info(FrameInfo frame_info) {
	printf("Number of Blobs: %d\n", frame_info.num_blobs);
	printf("     dx: %.2f\tdy: %.2f\n", frame_info.delta_X, frame_info.delta_Y);

	for (int i = 0; i < frame_info.num_blobs; i++) {
		Point center_of_mass = frame_info.Blobs[i].center_of_mass;
		float proportional_average = frame_info.Blobs[i].proportional_average;
		int blob_id = frame_info.Blobs[i].ID; // Add this line to get the Blob's ID
		float velocity = frame_info.Blobs[i].velocity;
		float direction = frame_info.Blobs[i].direction;

		printf("     Blob %d - ID: %d\n", i + 1, blob_id); // Print the Blob's ID
		printf("             - Center of Mass: %.2f, %.2f\n", center_of_mass.x,
				center_of_mass.y);
		printf("             - Proportional Average: %.2f\n",
				proportional_average);
		printf("             - Velocity: %.2f, Direction: %.2f\n", velocity,
				direction);
	}
}

void print_frame_info_simple(FrameInfo frame_info) {
	printf("##FRAME##|%d|%.2f|%.2f|\n", frame_info.num_blobs,
			frame_info.delta_X, frame_info.delta_Y);

	for (int i = 0; i < frame_info.num_blobs; i++) {
		Point center_of_mass = frame_info.Blobs[i].center_of_mass;
		float proportional_average = frame_info.Blobs[i].proportional_average;
		int blob_id = frame_info.Blobs[i].ID; // Add this line to get the Blob's ID
		float velocity = frame_info.Blobs[i].velocity;
		float direction = frame_info.Blobs[i].direction;
		printf("##BLOBS##|%d|%.2f|%.2f|%.2f|%.2f|%.2f|\n", blob_id,
				center_of_mass.x, center_of_mass.y, proportional_average,
				velocity, direction);
	}
}

// Function that does the following
// result = matrix1 - matrix2
void subtract_matrices(int result[N][N], const int matrix1[N][N],
		const int matrix2[N][N]) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			result[i][j] = matrix1[i][j] - matrix2[i][j];
		}
	}
}

#ifndef STM_USER
//simulation usage macro
#define MAX_PEOPLE 3
#define HEAD_RADIUS 1
// Check if a position is free from other people
int is_position_free(uint16_t matrix[N][N], int x, int y) {
	for (int i = -HEAD_RADIUS; i <= HEAD_RADIUS; i++) {
		for (int j = -HEAD_RADIUS; j <= HEAD_RADIUS; j++) {
			if (x + i >= 0 && x + i < N && y + j >= 0 && y + j < N
					&& matrix[x + i][y + j] > 100) {
				return 0; // Not free, there's already a person here
			}
		}
	}
	return 1; // Position is free
}

void generate_random_matrix(uint16_t matrix[N][N]) {
	// Seeds
	srand(time(NULL));

	// Initialize matrix to random background values
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			matrix[i][j] = (rand() % (100 + 1)) - 10; // Random height between -10 to 100
		}
	}

	int num_people = rand() % (MAX_PEOPLE + 1); // Number of people to place in the matrix

	for (int p = 0; p < num_people; p++) {
		int head_x, head_y;

		// Ensure that the head position doesn't overlap with another person
		do {
			head_x = (rand() % (N - 2)) + 1; // 1 to N-2 to ensure space for head radius
			head_y = (rand() % (N - 2)) + 1;
		} while (!is_position_free(matrix, head_x, head_y));

		printf("head: %d, %d \n", head_x, head_y);

		uint16_t height = (rand() % (3000 - 1500 + 1)) + 1500; // Random height between 1500 to 3000
		printf("height: %d\n", height);

		// Draw head
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				// Check if within the circle equation for the head
				if ((i - head_x) * (i - head_x)
						+ (j - head_y)
								* (j - head_y)<= HEAD_RADIUS * HEAD_RADIUS) {
					matrix[i][j] = height;
				}
			}
		}

		// Draw shoulders
		for (int r = 1; r <= HEAD_RADIUS; r++) {
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					// Check if within the shoulder region
					if ((i - head_x) * (i - head_x)
							+ (j - head_y) * (j - head_y)
							<= (HEAD_RADIUS + r) * (HEAD_RADIUS + r)) {
						if (matrix[i][j] < (height - r * 200)) {
							matrix[i][j] = height - r * 200; // Decrease height for the shoulder region
						}
					}
				}
			}
		}
	}
}

// Main function
int main() {
	FIFObuffer fifobuffer;
	init_fifo(&fifobuffer); // Initialize the FIFObuffer

	for (int i = 0; i < 40; i++) {
		//printf("NEW MATRIX");
		int matrix[N][N];
		generate_random_matrix(matrix); //todo:remove before stm
		print_matrix(matrix);
		FrameInfo frame_info = parse_frame(matrix, 0.1, -0.1);
		enqueue(&fifobuffer, frame_info); // Store the FrameInfo into the FIFObuffer
		print_frame_info(fifobuffer.buffer[fifobuffer.rear]);
	}

	print_fifo(&fifobuffer);

	return 0;
}

#endif
