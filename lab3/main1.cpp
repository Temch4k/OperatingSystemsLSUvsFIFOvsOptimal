//Artsiom Skarakhod
//Operating Systems Concept
//Lab 3

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdio.h>
using namespace std;

// page that we read in from the input file
struct page
{
    int num = -1;
    int arrTime = -1;
    int nextTimeComing = -1;
};

// borrowed from the last project
struct Queue {
    int frontOfQ;
    int backOfQ;
    int qSize;

    page* q;

    explicit Queue(int c)
    {
        frontOfQ = backOfQ = 0;
        qSize = c;
        q = new page[qSize];
    }

    ~Queue()
    {
        delete[] q;
    }

    // adds a page to the queue
    void addToQ(page newElement)
    {
        // if the size is the same size as back of the queue then the queue is full
        if (qSize == backOfQ)
        {
            cout << "Queue is full!" << endl;
        }
            // if the queue is not full then we add an element to the back and increase the back
        else
        {
            q[backOfQ] = newElement;
            backOfQ++;
        }
    }

    // removes an element from the beginning of the queue
    void removeFromQ()
    {
        // if the back of the queue is the same as the end of the queue and its empty and theres nothing to remove
        if (backOfQ == frontOfQ)
        {
            cout << "Queue is empty!" << endl;
            return;
        }
            // otherwise just move every element up the queue by one and decrement backOfQ
        else {
            for (int i = 0; i < backOfQ - 1; i++)
            {
                if (i + 1 > backOfQ)
                {
                    return;
                }
                q[i] = q[i + 1];
            }
            backOfQ--;
        }
    }

    // returns the value at the start of the queue
    page whatIsAtStart() const
    {
        return q[frontOfQ];
    }

    // checks if the queue is empty
    bool isEmpty() const
    {
        return frontOfQ == backOfQ;
    }

    // prints the queue and everything inside of it
    void printQ() const
    {
        for (int i =0;i<backOfQ;i++)
        {
            cout<<"Spot"<<i<<" "<<q[i].num<<" ";
        }
        cout<<endl;
    }
};

// reading from the output file
vector<page> readFromFile(const string &inputFile)
{
    vector<page> v;

    ifstream myfile(inputFile);

    int a;
    int i = 0;
    page p;

    // while we are reading into a file, we put it into a temp page and then put that page
        // into the array
    while (myfile >> a)
    {
        p.num = a;
        p.arrTime = i;
        v.push_back(p);
        i++;
    }

    // return the vector that we read everything into
    return v;
}

// the page that comes in first is the one that will be replaced first
vector<double> FCFS(vector<page> input, int numOfPages)
{
    page p;
    int hits = 0;
    double miss = 0;
    int numsOfSpotsTaken=0;
    int time = 0;
    vector<double> returnVect(6,-1);

    int locForFaultsRate =1;

    vector<page> vect(numOfPages,p);
    Queue q(input.size());

    // add pages into the q from the vector
    for(int i=0; i<input.size();i++)
    {
        q.addToQ(input[i]);
    }

    // while we are not done with the queue keep on schedueling
    while(!q.isEmpty())
    {
        bool found = false;

        // checks if the process is already inside
        for(int i =0;i< vect.size();i++) {
            // if it is then we hit++ and remove the first page from the queue
            if (q.whatIsAtStart().num == vect[i].num)
            {
                hits++;
                q.removeFromQ();
                found = true;
                break;
            }
        }
        // if not inside put it into the next empty spot
        if((!found) && (numsOfSpotsTaken < numOfPages))
        {
            // goes through the scheduling vector
            for(int i =0;i< vect.size(); i++)
            {
                // if finds the next empty spot then puts a frame there
                    // but we still technically miss
                if(vect[i].num == -1)
                {
                    vect[i] = q.whatIsAtStart();
                    vect[i].arrTime = time;
                    q.removeFromQ();
                    numsOfSpotsTaken++;
                    miss++;
                    break;
                }
            }
        }
        // if there is no empty spots find the process that came first and remove it
        else if((!found)&& numsOfSpotsTaken == numOfPages)
        {
            // goes through the scheduling vector and find the page that got there first
            int earliestArrival = vect[0].arrTime;
            for(int i = 0; i < vect.size();i++)
            {
                if(vect[i].arrTime < earliestArrival)
                {
                    earliestArrival = vect[i].arrTime;
                }
            }
            // after we find the earliest page we replace it with out new one and we still
                // technically miss
            for(int i = 0; i< vect.size(); i++)
            {
                if(earliestArrival == vect[i].arrTime)
                {
                    vect[i] = q.whatIsAtStart();
                    q.removeFromQ();
                    found = true;
                    miss++;
                    break;
                }
            }
        }

        // add one to the time so we can fix when the next page is coming and know what arrived first
            // what arrived second and so on
        time++;

        // if we reached a certain point then we calculate the failure rate
        if(time%2000 == 0)
        {
            double faultsRate = miss / time;
            returnVect[locForFaultsRate] = faultsRate;
            locForFaultsRate++;
        }
    }
    // we return the vector with information in it and later print it
    returnVect[0]=miss;
    return returnVect;
}

// the page that was used the least recent will be replaced first
vector<double> LRU(vector<page> input, int numOfPages)
{
    // what we are putting into the vector for setup
    page p;

    int hits = 0;
    double miss = 0;

    int numsOfSpotsTaken = 0;
    int time = 0;
    vector<double> returnVect(6,-1);

    int locForFaultsRate =1;

    vector<page> vect(numOfPages,p);
    Queue q(input.size());

    // putting the input vector into the queue
    for(int i=0; i<input.size();i++)
    {
        q.addToQ(input[i]);
    }

    // if the queue is not empty then we keep on schedueling
    while(!q.isEmpty())
    {
        bool found = false;

        // checks if the process is already inside
        for(int i =0;i< vect.size();i++) {
            // if it is inside then we update that project's arrival time so we know the last time it
                // has been used so it doesn't get removed like a fcfs
            if (q.whatIsAtStart().num == vect[i].num)
            {
                hits++;
                q.removeFromQ();
                vect[i].arrTime = time;
                found = true;
                break;
            }
        }
        // if not inside put it into the next empty spot
        if((!found) && (numsOfSpotsTaken < numOfPages))
        {
            // goes through the scheduling vector
            for(int i =0;i< vect.size(); i++)
            {
                // find the next empty spot and puts the new page there
                // it still counts as a miss though
                if(vect[i].num == -1)
                {
                    vect[i] = q.whatIsAtStart();
                    vect[i].arrTime = time;
                    found = true;
                    q.removeFromQ();
                    numsOfSpotsTaken++;
                    miss++;
                    break;
                }
            }
        }
            // if there is no empty spots find the process that came first and remove it
        else if((!found)&& numsOfSpotsTaken == numOfPages)
        {
            // finds the page that was used the earliest
            int earliestArrival = vect[0].arrTime;
            for(int i = 0; i < vect.size();i++)
            {
                if(vect[i].arrTime < earliestArrival)
                {
                    earliestArrival = vect[i].arrTime;
                }
            }
            // replaces that page with a new process
            for(int i = 0; i< vect.size(); i++)
            {
                if(earliestArrival == vect[i].arrTime)
                {
                    vect[i] = q.whatIsAtStart();
                    q.removeFromQ();
                    found = true;
                    miss++;
                    break;
                }
            }
        }


        // adds to the time
        time++;

        // gets the needed information and then saves it into the return vector
        if(time%2000 == 0)
        {
            double faultsRate = miss / time;
            returnVect[locForFaultsRate] = faultsRate;
            locForFaultsRate++;
        }
    }
    // saves how many misses we have and returns the return vec
    returnVect[0]=miss;
    return returnVect;
}

// finds the next time thee process will be used
int findWhenItsNext(page currPage,vector<page> input)
{
    // goes through the vector, when it finds the same page that will be used later on
        // then it returns that location which means how many spots away we will use it
    for(int i =0; i<input.size();i++)
    {
        if(input[i].num == currPage.num)
        {
            return i;
        }
    }
    // if doesn't find that page being used again then return -1
    return -1;
}

// if we have multiple pages in the vector that will not be used again
// we find the one that arrived the earliest and replace that one
page findLeast(vector<page> vectorOfMinus1)
{
    // if there is noting then we simply return a page with a page num of -1
    if(vectorOfMinus1.empty())
    {
        page p;
        return p;
    }
        // if ther eis only one element that won't be used then we just return that one
    else if(vectorOfMinus1.size()==1)
    {
        return vectorOfMinus1[0];
    }
    // otherwise we find the one that arrived the earliest and return that one
    page temp = vectorOfMinus1[0];
    for(int i=0; i<vectorOfMinus1.size(); i++) {
        if(temp.arrTime < vectorOfMinus1[i].arrTime)
        {
            temp = vectorOfMinus1[i];
        }
    }
    return temp;
}

vector<double> Optimal(vector<page> input, int numOfPages)
{
    page p;

    int hits = 0;
    double miss = 0;
    double precentDone=0;

    // this is how we control if our memory is full or not
    int numsOfSpotsTaken = 0;

    // this is how we figure out what came first what came second and so on
    int time = 0;

    // returns all of vour calculations for printing
    vector<double> returnVect(6,-1);

    int locForFaultsRate  = 1;

    // the vector that is our memory that we will be doing all schedueling in
    vector<page> vect(numOfPages,p);

    // honestly we don't really need a queue for this case but I still made it to be 100% safe
    Queue q(input.size());

    // to calculate the percentage we are done with
    double inputSize = input.size();

    for(int i=0; i<input.size();i++)
    {
        q.addToQ(input[i]);
    }
    while(!q.isEmpty())
    {
        bool found = false;

        // checks if the process is already inside
        for(int i =0;i< vect.size();i++) {
            if (q.whatIsAtStart().num == vect[i].num)
            {
                hits++;
                q.removeFromQ();
                input.erase(input.begin());
                vect[i].arrTime = time;
                found = true;
                break;
            }
        }
        // if not inside put it into the next empty spot
        if((!found) && (numsOfSpotsTaken < numOfPages))
        {
            for(int i =0;i< vect.size(); i++)
            {
                if(vect[i].num == -1)
                {
                    vect[i] = q.whatIsAtStart();
                    vect[i].arrTime = time;
                    found = true;
                    q.removeFromQ();
                    input.erase(input.begin());
                    numsOfSpotsTaken++;
                    miss++;
                    break;
                }
            }
        }
        // if not inside and all spots are taken
        else if((!found)&& numsOfSpotsTaken == numOfPages)
        {
            int temp = 0;
            page p;
            vector<page> tempToo;

            // find the next time every element will be used
            // finds every element that won't be used anymore and puts it into a separate array
           for(int i =0; i<vect.size();i++)
           {
               vect[i].nextTimeComing = findWhenItsNext(vect[i],input);
               if(vect[i].nextTimeComing == -1)
               {
                   tempToo.push_back(vect[i]);
               }
           }
           // if there are processes that won't show up again
           if(!tempToo.empty())
           {
               // we find the process that showed up first and will get rid of it because we solve it thru FCFS
               p = findLeast(tempToo);

               for(int i=0;i<vect.size();i++)
               {
                   // find that process and replace it inside of our vector
                   if(p.num == vect[i].num)
                   {
                       vect[i] = q.whatIsAtStart();
                       vect[i].arrTime = time;
                       found = true;
                       q.removeFromQ();
                       input.erase(input.begin());
                       miss++;
                       break;
                   }
               }
           // if there aren't any processe that won't show up again and will all show up later
           } else{
               int tempLoc = 0;
               page temp = vect[0];
               // go through the vector
               for(int i =0;i<vect.size();i++)
               {
                   // find the process that will show up the latest
                   if(temp.nextTimeComing < vect[i].nextTimeComing)
                   {
                       temp = vect[i];
                       tempLoc = i;
                   }
               }

               // after we find the proc that will show up the latest we replace it
               vect[tempLoc] = q.whatIsAtStart();
               vect[tempLoc].arrTime = time;
               found = true;
               q.removeFromQ();
               input.erase(input.begin());
               miss++;
           }

        }
        // add time so we can control what goes in what leaves
        time++;
        // saves the stats into the return vector and prints out the precentage we are done with
        if(time%2000 == 0)
        {
            double faultsRate = miss / time;
            returnVect[locForFaultsRate] = faultsRate;
            locForFaultsRate++;

            double tempTime = time;
            precentDone = (tempTime / inputSize) * 100;
            cout<<"we are: "<<precentDone<<"%"<<" done with Optimal"<<endl;
        }
    }
    // returns the number of misses
    returnVect[0] = miss;
    return returnVect;
}

void printResults(vector<page> pages, int frameSize, FILE *ff)
{

    // calls out functions and puts everything int oa file
    fprintf(ff,"=======================================================================\n");
    fprintf(ff,"=====Page Replacement Algorithm Simulation (frame size = %d)==========\n", frameSize);
    fprintf(ff,"=======================================================================\n");
    fprintf(ff,"\t\t\t\t\tPage fault rates\n");
    fprintf(ff,"Algorithm\tTotal faults\t2000\t4000\t6000\t8000\t10000\n");
    fprintf(ff,"-----------------------------------------------------------------------\n");
    vector<double> output = FCFS(pages,frameSize);
    fprintf(ff,"1.FIFO\t\t%d\t\t%1.4f\t%1.4f\t%1.4f\t%1.4f\t%1.4f\t\n",(int)output[0],output[1],output[2],output[3],output[4],output[5]);
    output = LRU(pages,frameSize);
    fprintf(ff,"2.LRU\t\t%d\t\t%1.4f\t%1.4f\t%1.4f\t%1.4f\t%1.4f\t\n",(int)output[0],output[1],output[2],output[3],output[4],output[5]);
    output = Optimal(pages,frameSize);
    fprintf(ff,"3.Optimal\t%d\t\t%1.4f\t%1.4f\t%1.4f\t%1.4f\t%1.4f\t\n",(int)output[0],output[1],output[2],output[3],output[4],output[5]);
}

int main(int argc, char **argv)
{

    // gets used input
    char const *frameSize = argv[1];
    int intFrameSize = 0;
    stringstream strVal;
    strVal << frameSize;
    strVal >> intFrameSize;


    string inputFile = argv[2];
    char const *outputFile = argv[3];

    // reads from a file and saves it into a vec
    vector<page> pages = readFromFile(inputFile);

    // opens the file
    FILE *ff = fopen(outputFile, "a");

    // writes into a file
    printResults(pages,intFrameSize,ff);

    // closes the file
    fclose(ff);
    return 0;
}