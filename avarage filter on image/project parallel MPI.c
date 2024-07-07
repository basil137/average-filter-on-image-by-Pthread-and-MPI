#include<stdio.h>
#include<mpi.h>
#include<math.h>
#define maxsize_row 50
#define maxsize_col 10

/*double array_copy[maxsize_row][maxsize_col]={{1,2,3},{4,5,6},{7,8,9},{10,11,12},{13,14,15},{16,17,18},{19,20,21},{22,23,24},{1,2,3},{4,5,6},{7,8,9},{10,11,12},{13,14,15},{16,17,18},{19,20,21},{22,23,24},{1,2,3},{4,5,6},{7,8,9},{10,11,12},{13,14,15},{16,17,18},{19,20,21},{22,23,24}};//here the image in array_copy*/

double filter_average[3][3]={{0.11,0.11,0.11},{0.11,0.11,0.11},{0.11,0.11,0.11}};


int c=1;
void fill_Matrix(double m[maxsize_row][maxsize_col])
{
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



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





void  main(int argc,char **argv)
{
int id,nop;

MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD,&nop);
MPI_Comm_rank(MPI_COMM_WORLD,&id);
MPI_Status status;


int x=(maxsize_row-2)/nop;
int rest=(maxsize_row-2) % nop;

double data[maxsize_row][maxsize_col];
double data_filterd[maxsize_row][maxsize_col];

double local_data[x+rest+2][maxsize_col];
double local_data_filterd[x+rest+2][maxsize_col];



if(id==0){		// the root is p0

fill_Matrix(data);

int count=1;


//-----------------------------------------------------------------------------------------
//				scatter the data to all p exept p0 by send&recv
for(int i=1;i<nop;i++)
{

	for(int j=0;j<x;j++)
	{
	if(j==0)
	MPI_Send(&data[count-1],maxsize_col,MPI_DOUBLE,i,1,MPI_COMM_WORLD);
	
	
	MPI_Send(&data[count],maxsize_col,MPI_DOUBLE,i,1,MPI_COMM_WORLD);
	
	if(j==x-1)
	MPI_Send(&data[count+1],maxsize_col,MPI_DOUBLE,i,1,MPI_COMM_WORLD);
	
	count++;
	}
	
}	


//---------------------------------------------------------------------------------------------
//				scatter data to p0
int local_count=0;
for(int i=count-1;i<maxsize_row;i++)
{	
	for(int j=0;j<maxsize_col;j++)
		local_data[local_count][j]=data[i][j]; 
		
	local_count++;
	
}



//-------------------------------------------------------------------------------------------
//				start filter on p0 (local data)


for (int i = 0; i < x+2+rest; i++)
    {
        for (int j = 0; j < maxsize_col; j++)
        {
            local_data_filterd[i][j] =local_data[i][j] ;
        }
    }
    
    
    for(int a=1;a<x+1+rest;a++)
		for(int b=1;b<maxsize_col-1;b++)
		{
		     local_data_filterd[a][b]=(local_data[a-1][b-1]*filter_average[0][0]) + (local_data[a-1][b]*filter_average[0][1]) + (local_data[a-1][b+1]*filter_average[0][2]) + (local_data[a][b-1]*filter_average[1][0]) +(local_data[a][b]*filter_average[1][1]) + (local_data[a][b+1]*filter_average[1][2]) + (local_data[a+1][b-1]*filter_average[2][0]) + (local_data[a+1][b]*filter_average[2][1]) + (local_data[a+1][b+1]*filter_average[2][2]);

		}
		
		
	
	
//-------------------------------------------------------------------------------------------
//				gathering the data filterd from all processes by send/recv	

int countReturnData=0;

for(int np=1;np<nop;np++)
{
	if(np!=1)
		for(int r=0;r<x;r++)
		{
 MPI_Recv(&data_filterd[countReturnData],maxsize_col,MPI_DOUBLE,np,1,MPI_COMM_WORLD,&status);
 
 			countReturnData++;
		}
		
	else
		for(int r=0;r<x+1;r++)
		{
 MPI_Recv(&data_filterd[countReturnData],maxsize_col,MPI_DOUBLE,np,1,MPI_COMM_WORLD,&status);
 
 			countReturnData++;
		}
			

}


//-------------------------------------------------------------------------------------------
//				gathering the data filterd from p0 by into data_filterd	


int local_count2=1;
for(int ii=countReturnData;ii<maxsize_row;ii++)
{	
	for(int j=0;j<maxsize_col;j++)
		data_filterd[ii][j]=local_data_filterd[local_count2][j]; 
		
	local_count2++;
	
}



}






//=============================================================================================
//				other p
//=============================================================================================


		

if(id!=0)
{
int rcount=0;
double recv_data[x+2][maxsize_col];
double recv_data_filterd[x+2][maxsize_col];



//---------------------------------------------------------------------------------------------
//				recv the scattering data into (recv_data)
	for(int j=0;j<x+2;j++)
	{
	MPI_Recv(&recv_data[rcount],maxsize_col,MPI_DOUBLE,0,1,MPI_COMM_WORLD,&status);
	rcount++;
	}
	
	
//---------------------------------------------------------------------------------------------
//				start filtering on recv_data




for (int i = 0; i < x+2; i++)
    {
        for (int j = 0; j < maxsize_col; j++)
        {
            recv_data_filterd[i][j] =recv_data[i][j] ;
        }
    }
 	
 	
 	
for(int a=1;a<x+1;a++)
	for(int b=1;b<maxsize_col-1;b++)
	{
		     recv_data_filterd[a][b]=(recv_data[a-1][b-1]*filter_average[0][0]) + (recv_data[a-1][b]*filter_average[0][1]) + (recv_data[a-1][b+1]*filter_average[0][2]) + (recv_data[a][b-1]*filter_average[1][0]) +(recv_data[a][b]*filter_average[1][1]) + (recv_data[a][b+1]*filter_average[1][2]) + (recv_data[a+1][b-1]*filter_average[2][0]) + (recv_data[a+1][b]*filter_average[2][1]) + (recv_data[a+1][b+1]*filter_average[2][2]);

	}
		
		


//------------------------------------------------------------------------------------------
//				send the partion(after filterd) of evry process to p0

int scount=0;

if(id!=1)
{
	scount=0;
	for(int j=1;j<x+1;j++)
	{
		MPI_Send(&recv_data_filterd[j],maxsize_col,MPI_DOUBLE,0,1,MPI_COMM_WORLD);
		scount++;
	}


}
	
else
{
	scount=0;
	for(int j=0;j<x+1;j++)
	{
		MPI_Send(&recv_data_filterd[j],maxsize_col,MPI_DOUBLE,0,1,MPI_COMM_WORLD);
		scount++;
	}	
}
	

/*printf("\n\nthe process is %d:\n",id);
for(int a=0;a<x+2;a++)
{	
	printf("the row %d are: ",a);
	for(int b=0;b<maxsize_col;b++)
		printf("%f ,",recv_data[a][b]);
	printf("\n");	}			


for(int a=0;a<x+2;a++)
{	
	printf("the row filterd %d are: ",a);
	for(int b=0;b<maxsize_col;b++)
		printf("%f ,",recv_data_filterd[a][b]);
	printf("\n");	}*/	
}


MPI_Barrier(MPI_COMM_WORLD);





//=============================================================================================
//				show image befor and after the fitering
//=============================================================================================



if(id==0)
{
printf("\n\nthe original image (old image):\n");
print_Matrix(data);

printf("\n\nthe filterd image by average filter (new image):\n");
print_Matrix(data_filterd);
}



MPI_Finalize();
 
}
