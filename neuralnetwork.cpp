#include<iostream>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include<stdlib.h>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <ctime>
#include <random>
#include <semaphore.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;
int flag=0;
int n;
int index_pipe =0;
int ** forward_pipe;
int ** backward_pipe;
int layer_count=0;
int neuron_counter=0;
pthread_mutex_t lock;
int *layer_neurons;
int counter=0;
sem_t sem;
vector<double> input_data;
vector<double> output_layer_weight;
vector<vector<vector<double>>> weights(8); //
vector<int> threads;
int layer_counter;
int neuron_counter1=0;
vector<double> answers_neurons;
double * array1;
double x=0;


class neuron
{
public:
	float id;	//index of that neuron
    float layer_num;
    neuron()
    {
        id=0;
        layer_num=0;
    }
};

void* neuronFunc(void* a)
{
    pthread_mutex_lock(&lock);
    double* array= (double * )a;
    double sum =0;
    for(int i =0; i<layer_neurons[layer_counter]; i++)
    {
        double x=weights[layer_counter][i][neuron_counter];
        sum = sum + (array[i] *  x);
    }
    answers_neurons.push_back(sum);
    neuron_counter++;

    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
}

void create_layers(int layer)
{

    if (layer == layer_count)
    {
        return;
    }
    else
    {
                int fd1[2]={0,0};
                int fd2[2]={0,0};
                pipe(fd1);
                pipe(fd2);
                if (pipe(fd1) == -1 || pipe(fd2) == -1)
                {
                   std::cerr << "Failed to create pipe\n";
                }
                else
                {
                    forward_pipe[index_pipe][0]=fd1[0];
                    forward_pipe[index_pipe][1]=fd1[1];
                    backward_pipe[index_pipe][0]=fd2[0];
                    backward_pipe[index_pipe][1]=fd2[1];
                }
        index_pipe++;
        if(layer==0)
        {
            layer_counter=-1;
        }
        else 
        {
            layer_counter++;
        }

        pid_t p = fork();

        if (p == 0)
        {
            create_layers(layer+1);

        }
        else
        {
            for(int k =0; k<2; k++)
            {
                if (layer == layer_count)
                {
                    return;
                }
                pthread_t* neuron;
                int LayerN=layer_neurons[layer];
                neuron=new pthread_t [LayerN];

                if(k==1)
                {
                    cout<<"layer at k==1 : "<<layer<<endl;
                }

                if(layer!=0)
                {
                    array1 = new double[layer_neurons[layer-1]];
                    double readArray[layer_neurons[layer-1]];
                    int size =sizeof(readArray) / sizeof(double);

                    close(forward_pipe[layer-1][1]);
                    read(forward_pipe[layer-1][0], readArray, sizeof(double) *  size );
                    //close(forward_pipe[layer-1][0]);
                    cout<<"Layer " << layer << " reads: ";
                    for(int i =0; i<size; i++)
                    {
                        cout<<readArray[i]<<" "; 
                        array1[i]=readArray[i];
                    }
                    cout<<endl;
        
                    for (int i = 0; i < layer_neurons[layer]; i++)
                    {

                        sleep(0.1);
                        pthread_create(&neuron[i], NULL, neuronFunc, (void* )array1 );
                        sleep(0.1);
                    }

                    while (neuron_counter<layer_neurons[layer]);
                    neuron_counter=0;


            }
                if(layer!=layer_count-1)
                {
                    if(layer==0)
                    {
                        double doubleArray[input_data.size()] ;
                        cout<<"Layer " << layer << " writes: ";
                        for(int i =0;i<input_data.size(); i++)
                        {
                            doubleArray[i]=input_data[i];
                            cout<<doubleArray[i]<<" ";
                        }
                        cout<<endl;
                            int size = sizeof(doubleArray) / sizeof(double);
                            write(forward_pipe[layer][1], doubleArray, sizeof(double) * size);

                       // cout<<" writen bytes "<<write_byters<<endl;
                       // close(forward_pipe[layer][1]);
                    }
                    else 
                    {
                        double doubleArray[layer_neurons[layer]] ;
                        
                        cout << endl;
                        cout << "Calculating...Calculating..." << endl;
                        for(int i =0; i<answers_neurons.size();i++)
                        {
                            cout<<answers_neurons[i]<<"  "; 
                            doubleArray[i]=answers_neurons[i];
                        }
                        cout << endl;
                        cout << "Calculations acquired!" << endl;
                        cout<<endl;
                        
                        if(layer!=layer_count-1)
                            answers_neurons.resize(0);
                        
                        cout<<"Layer " << layer << " writes: ";
                        for(int i =0;i<layer_neurons[layer]; i++)
                        {
                            cout<<doubleArray[i]<<" ";
                        }
                        
                        cout<<endl;
                            int size = sizeof(doubleArray) / sizeof(double);
                            write(forward_pipe[layer][1], doubleArray, sizeof(double) * size);
                        //cout<<" writen bytes "<<write_byters<<endl;
                        //close(forward_pipe[layer][1]);
                    }
                }





                //Backward prop


	    if (layer == layer_count-1) {
	    cout << endl;
	    cout << "Congratulations!" << endl;
	    cout << "Neural Network has found your output: ";
	    }
            for(int i =0; i<answers_neurons.size(); i++)
            {
                if(x!=answers_neurons[i])
                {
                    cout<<answers_neurons[i] << endl;
                }
            }

            if(k==1)
            {
                    break;
            }


	    if (layer == layer_count-1) {
	    cout << endl;
	    cout << "Backpropagation commencing in 3...2...1..." << endl; 
	    }	 
	    cout << endl;
            double* backarray;
            if (layer != layer_count-1)
            {
                double * readArray= new double[2];
                int size =2;

                close(backward_pipe[layer][1]);
                read(backward_pipe[layer][0], readArray, sizeof(double) *  size );
                close(backward_pipe[layer][0]);
                cout << endl;
                cout << "Backward Pipe of Layer "<< layer <<" reads: ";
                backarray=new double[size];
                for(int i =0; i<size; i++)
                {
                    backarray[i]=readArray[i];
                    cout<<backarray[i]<<" "; 

                }
                cout<<endl;

            }
            if(layer==0)
            {
                cout << "Input data: ";
                for(int i =0; i<2; i++)
                {
                    input_data[i]=backarray[i];
                    cout<<input_data[i]<<endl;
                }
            }

            if (layer != 0)
            {
                close(backward_pipe[layer-1][0]);
                int write_byters;
                        double * doubleArray; 
                        {
                            doubleArray= new double[2] ;
                            cout << "Backward Pipe of Layer "<< layer <<" writes: ";
                            if(layer==layer_count-1)
                            {   
                                x= answers_neurons[0];
                                doubleArray[0]=(((x*x)+x+1)/(2));
                                doubleArray[1]=(((x*x)-x)/(2));
                                cout << endl;
                                cout << "Input 1: " << doubleArray[0] << endl;
                                cout << "Input 2: " << doubleArray[1] << endl;
                                cout << endl;
                            }
                            else
                            {
                                for(int i =0; i<2; i++)
                                {
                                    doubleArray[i]=backarray[i];

                                }
                            }
                            for(int i =0;i<2; i++)
                            {
                                cout<<doubleArray[i]<<" ";
                            }
                            int size = 2;
                            write(backward_pipe[layer-1][1], doubleArray, sizeof(double) * size);
                        }
                        //cout<<" writen bytes "<<write_byters<<endl;
                        
                        close(backward_pipe[layer-1][1]);
            }

                        cout<<endl;
                        if(layer==1)
                            sleep(1);
                            if(layer==2)
                            sleep(2);
                            if(layer==3)
                            sleep(5);
            }

            wait(NULL);
            exit(0);
        }

       }
}

int main()
{
    sem_init(&sem, 0, 1);
    pthread_mutex_init(&lock, 0);
    int layerNumber = 0;

    cout << " *********** WELCOME TO OUR NEURAL NETWORK ***********" << endl;
    cout << "Number of layers: ";
    cin >> layerNumber;

    n=layerNumber;
    forward_pipe= new int *[n];
    for(int i =0; i<n;i++)
    {
        forward_pipe[i]=new int[2];
    }
    backward_pipe= new int *[n];
    for(int i =0; i<n;i++)
    {
        backward_pipe[i]=new int[2];
    }

    if (layerNumber < 2 || layerNumber > 8)
    {
    	cout << "Enter again" << endl;
    	exit(0);
    }

    layer_count=layerNumber;
    layer_neurons=new int [layer_count];
    layer_neurons[0]=2;




    layer_neurons[layer_count-1]=1;
    for(int i=1;i<layer_count-1;i++)
    {
        cout << "Number of neurons in layer " << i+1 << ": ";
        cin >> layer_neurons[i];
        if (layer_neurons[i] < 2 || layer_neurons[i] > 8) {
    	cout << "Enter again" << endl;
    	exit(0);
    	}
    }
    
    // Resize the weights vector to accommodate the desired number of layers
    weights.resize(layer_count - 1);
    cout<<"Layers: "<<layer_count<<endl;
    cout<<"Neurons: ";

    for(int i=0;i<layer_count;i++)
    {
        cout<<layer_neurons[i]<<" ";
    }
    cout << endl;

    // Open file for reading
    ifstream infile("input.txt");

    if (!infile) {
        cerr << "Failed to open file" << endl;
        return 1;
    }

    // Initialize counters for tracking layer and neuron indices
    int layer = -1; // Updated to start at -1
    int neuron = 0;

    // Read file line by line
    string line;

    bool input = false;
    bool output = false;

    while (getline(infile, line))
    {
        // Check if line indicates a new layer
        if ((line.find("Hidden layer") != string::npos || line.find("Output layer") != string::npos) && input == false)
        {
            layer++;
            neuron = 0;
            weights[layer].resize(neuron); // Resize previous layer to correct size
            continue;
        }
        // Check if line indicates input layer
        if (line.find("Input layer") != string::npos)
        {
            layer = 0;
            neuron = 0;
            weights[layer].resize(neuron); // Resize previous layer to correct size
            continue;
        }
        if (line.find("Input data") != string::npos)
        {
            input = true;
        }

        // Remove any characters that aren't part of the weight values
        line.erase(remove_if(line.begin(), line.end(), [](char c) { return !isdigit(c) && c != '.' && c != '-'; }), line.end());

        // Parse comma-separated weight values
        stringstream ss(line);
        double weight;
        vector<double> neuron_weights;
        while (ss >> weight && layer >= 0)
        {
            if (input == false)
            {
                neuron_weights.push_back(weight);
                if (ss.peek() == ',') ss.ignore();
            }
            else
            {
                input_data.push_back(weight);
                if (ss.peek() == ',') ss.ignore();
            }
        }

        // Add neuron weights to appropriate layer and neuron indices
        if (layer >= 0 && neuron_weights.size() > 0 && input == false)
        {

            if (input == false )
            {
                weights[layer].push_back(neuron_weights);
                neuron++;
            }
            else
                input = true;
        }
    }

    for(int i =0; i<weights[6].size(); i++)
    {
        for(int k =0; k<weights[6][i].size(); k++)
        {
            output_layer_weight.push_back(weights[6][i][k]);
        }
    }
    int count1=0;
    int removeindex=0;
    for(int i=0; i<layer_count; i++)
    {
            weights[i].resize(layer_neurons[count1]);
            int nextLayerNeurons = 0;
            if (count1 + 1 < layer_count)
            {
                nextLayerNeurons = layer_neurons[count1 + 1];
            }
          //  cout<<"Next layer neurons: "<<nextLayerNeurons<<endl;
            count1++;

        for(int j =0; j<weights[i].size(); j++)
        {

            weights[i][j].resize(nextLayerNeurons);
        }
        removeindex=i;
    }
    for(int i=removeindex; i<weights.size(); i++)
    {
         weights[i].resize(0);
    }
    for(int i=removeindex-1; i<weights.size(); i++)
    {
        for(int j=0; j<weights[i].size(); j++)
        {
            for(int k=0; k<weights[i][j].size(); k++)
            {
                weights[i][j][k]=output_layer_weight[j];
            }
        }
    }

    layer_counter=0;

    cout << endl;
    cout << "Jump starting neural network..."	<< endl << endl;
    create_layers(0);


    return 0;
}