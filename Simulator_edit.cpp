/**
 * Matthew Bruffey
 * AST4994W Astrophysics Senior Thesis
 * 
 * Simulation of the polarization of light passing through elongated dust particles
 * which have partially alligned along magnetic field lines in the interstellar medium
 * and molecular clouds.
 * 
 */

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
using namespace std;

typedef struct cell {
	double eta;
	double theta;
	double tau;
} cell;

typedef struct col {
	cell* cells;
	double I;
	double Q;
	double U;
	double P;
	int size;
} col;

typedef col* colrow;
typedef colrow* vol;

void initialize_0(col*, double eta, double theta, double tau);
void initialize_1(col*, double eta, double theta, double tau, double eta_rate, int option);
void simulate_cl(col*);

int main()
{
	double eta = 0;
	double theta = 0;
	int size = 0;
	double tau = 0;
	double eta_rate = 0;
	
	col* column;
	vol cube;
	
	int width = 0;
	int height = 0;
	
	char menop;
	int option = 0;
	
	cout << "*OPTIONS*\n";
	cout << "0: Single column with uniform eta, theta and tau\n";
	cout << "1: Single column with uniform eta, and tau, but random theta\n";
	cout << "2: Single column with uniform theta and tau, but varying (linear) eta\n";
	cout << "3: Cube with custom settings\n";
	cout << "4: Exit\n";
	cout << "Selection: ";
	cin >> option;
	
	switch(option)
	{
		case(0):  //Single column uniform eta, theta and tau
			cout << "\nNumber of cells: ";
			cin >> size;
			cout << "\nEnter the desired eta: ";
			cin >> eta;
			cout << "\nEnter the desired theta: ";
			cin >> theta;
			cout << "\nOptical Depth (Tau) of each cell: ";
			cin >> tau;
			column = new col;
			column->cells = new cell[size];
			column->size = size;
			initialize_0(column, eta, theta, tau);
			simulate_cl(column);
			
			cout << "\nResults: I: " << column->I << " Q: " << column->Q << " U: " << column->U << " P: " << column->P << "%" << endl;
			
			delete[] column->cells;
			break;
		case 1:  //Single column with uniform eta, and tau, but random theta
			cout << "\nNumber of cells: ";
			cin >> size;
			cout << "\nEnter the desired eta: ";
			cin >> eta;
			cout << "\nOptical Depth (Tau) of each cell: ";
			cin >> tau;
			cout << "\nWould you like to limit fluctuations in theta? (y/n)\n";
			cin >> menop;
			if(menop == 'y')
			{
				cout << "\nSpecify maximum change in theta from cell to cell: ";
				cin >> eta_rate;
				
				cout << "\nWould you like to specify theta for the first cell? (y/n)\n";
				cin >> menop;
				if(menop == 'y')
				{	
					cout << "\nEnter theta: ";
					cin >> theta;
				}
			}
			column = new col;
			column->cells = new cell[size];
			column->size = size;
			
			initialize_1(column, eta, theta, tau, eta_rate, option);
			simulate_cl(column);
			
			cout << "\n\nResults: I: " << column->I << " Q: " << column->Q << " U: " << column->U << " P: " << column->P << "%" << endl;
			
			delete[] column->cells;
			delete column;
			break;
		case 2:  //Single column with uniform theta and tau, but varying (linear) eta
			cout << "\nNumber of cells: ";
			cin >> size;
			cout << "\nEnter the desired theta: ";
			cin >> theta;
			cout << "\nOptical Depth (Tau) of each cell: ";
			cin >> tau;
			cout << "\nEnter the initial eta: ";
			cin >> eta;
			cout << "\nEnter the rate of change of eta from cell to cell: ";
			cin >> eta_rate;
			column = new col;
			column->cells = new cell[size];
			column->size = size;
			
			initialize_1(column, eta, theta, tau, eta_rate, option);
			simulate_cl(column);
			
			cout << "\n\nResults: I: " << column->I << " Q: " << column->Q << " U: " << column->U << " P: " << column->P << "%" << endl;
				
			delete[] column->cells;
			delete column;
			break;
		case 3:  //Cube with custom settings
			
			cout << "\nWidth of Cube: ";
			cin >> width;
			cout << "\nHeight of Cube: ";
			cin >> height;
			cout << "\nLength of cube (number of cells): ";
			cin >> size;
			cube = new colrow[height];
			for(int i = 0; i < height; i++)
				cube[i] = new col[width];
			
			//delete[] cube;
			break;
		case 4:; //Exit
	}	

	return 0;
}


void initialize_0(col* column, double eta, double theta, double tau)
{
	int n = column->size;
	cell* cells = column->cells;
	column->I = 1;
	column->Q = 0;
	column->U = 0;
	column->P = 0;
	theta = theta * 180 / 3.14159;
	for(int i = 0; i < n; i++)
	{
		cells[i].eta = eta;
		cells[i].theta = theta;
		cells[i].tau = tau;
	}	
}

void initialize_1(col* column, double eta, double theta, double tau, double eta_rate, int option)
{
	int n = column->size;
	cell* cells = column->cells;
	column->I = 1;
	column->Q = 0;
	column->U = 0;
	column->P = 0;
	switch(option)
	{
		case(1):
			time_t seed;
			srand(time(&seed));
			double th_next;
			if(theta == 0 && eta_rate == 0)
			{
				for(int i = 0; i < n; i++)
				{
					cells[i].eta = eta;
					cells[i].theta = (rand() % 360 - 179) * 3.14159 / 180;
					cells[i].tau = tau;
				}
			}
			else
			{
				cells[0].eta = eta;
				if(theta != 0)
					cells[0].theta = theta * 3.14159 / 180;
				else
					cells[0].theta = (rand() % 360 - 179) * 3.14159 / 180;
				cells[0].tau = tau;
				for(int i = 1; i < n; i++)
				{
					cells[i].eta = eta;
					bool allowed = false;
					while(!allowed)
					{
						th_next = (double)rand()/RAND_MAX*360 - 179;
						if( abs( (cells[i-1].theta * 180 / 3.14159) - th_next) <= eta_rate)
							allowed = true;
					}
					cells[i].theta = th_next * 3.14159 / 180;
					cells[i].tau = tau;
				}
			}
			
			break;
		case(2):
			cells[0].eta = eta;
			cells[0].theta = theta * 3.14159 / 180;
			cells[0].tau = tau;
			for(int i = 1; i < n; i++)
			{
				cells[i].eta = cells[i-1].eta + eta_rate;
				cells[i].theta = theta * 3.14159 / 180;
				cells[i].tau = tau;
			}
	}
}

void simulate_cl(col* column)
{
	cell* cells = column->cells;
	double* I = &(column->I);
	double* Q = &(column->Q);
	double* U = &(column->U);
	double* P = &(column->P);
	
	double sigma = 0;
	double I_p = 0;
	double tau_p = 0;
	for (int i = 0; i < column->size; i++)
	{
		tau_p = cells[i].tau * (1 - cells[i].eta) / (1 + cells[i].eta);
		sigma =(*Q)*cos(2*cells[i].theta) + (*U)*sin(2*cells[i].theta);
		I_p = (*I)*sinh(tau_p) + sigma*cosh(tau_p)-sigma;
		*I = ( (*I)*cosh(tau_p) + sigma*sinh(tau_p) ) * exp(-1*cells[i].tau);
		*Q = ( I_p*cos(2*cells[i].theta) + *Q) * exp(-1*cells[i].tau);
		*U = ( I_p*sin(2*cells[i].theta) + *U) * exp(-1*cells[i].tau);
	}
	*P = 100*sqrt(pow(*Q,2) + pow(*U,2) ) / (*I);
}
