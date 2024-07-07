#include <pthread.h>
#include <stdio.h>
#define num_threads 6
#define maxsize_row 50
#define maxsize_col 10

double filter_average[3][3]={{0.11,0.11,0.11},{0.11,0.11,0.11},{0.11,0.11,0.11}};


pthread_mutex_t mutex1;

double data[maxsize_row][maxsize_col];
double data_filterd[maxsize_row][maxsize_col];




void fill_Matrix(double m[maxsize_row][maxsize_col])
{
int c=1;
    for (int i = 0; i < maxsize_row; i++)
    {
        for (int j = 0; j < maxsize_col; j++)
        {
            m[i][j] =c;
            c++;
        }
    }
}


void print_Matrix(double m[maxsize_row][maxsize_col])
{
    	
	for(int a=0;a<maxsize_row;a++)
	{	
		//printf("row number %d:",a);
		
		for(int b=0;b<maxsize_col;b++)
			printf("%f ",m[a][b]);
			
		printf("\n");	
	}			

}




void *slave(void *tn)
{


long threadN=(long )tn;

int x=(maxsize_row-2)/num_threads;
int rest=(maxsize_row-2) % num_threads;
int start=rest+1;

int low=0,high=0;



if(threadN==0)
{
	low=1;
	high=start+x;
}

else
{
	low=threadN*x+start;
	high=low+x;
}



for(int a=low;a<high;a++)
		for(int b=1;b<maxsize_col-1;b++)
		{
		
		pthread_mutex_lock(&mutex1);
		
		     data_filterd[a][b]=(data[a-1][b-1]*filter_average[0][0]) + (data[a-1][b]*filter_average[0][1]) + (data[a-1][b+1]*filter_average[0][2]) + (data[a][b-1]*filter_average[1][0]) +(data[a][b]*filter_average[1][1]) + (data[a][b+1]*filter_average[1][2]) + (data[a+1][b-1]*filter_average[2][0]) + (data[a+1][b]*filter_average[2][1]) + (data[a+1][b+1]*filter_average[2][2]);

		pthread_mutex_unlock(&mutex1);
		
		}

   
   
    
}








int main()
{
	
fill_Matrix(data);
fill_Matrix(data_filterd);


pthread_mutex_init(&mutex1, NULL);
pthread_t thread[num_threads];
    
for (long i = 0; i < num_threads; i++)
	pthread_create(&thread[i], NULL, slave, (void *) i);
           

for (long i = 0; i < num_threads; i++)
	pthread_join(thread[i], NULL);
	


printf("\n\nthe original image (old image):\n");
print_Matrix(data);

printf("\n\nthe filterd image by average filter (new image):\n");
print_Matrix(data_filterd);
            
    
    
    return 0;
    
}

