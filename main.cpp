#include <iostream>
#include <simlib.h>


//Default values of simulation
#define COUNT_OF_STARTS 68

#define GENERATE_PEOPLE 0.087591241 // 274 per day (17hours of work) | 11,4166666 per hour | 1human/0,087591241hours

#define CAPACITY 40

#define RUNTIME 1  //hours


//Globals
double generatePeople = GENERATE_PEOPLE;
double countOfStarts =  COUNT_OF_STARTS;


//TODO dočasné
int count = 0;


//Stores Boat, Pier
Store Boat("Boat capacity", CAPACITY);
Store Pier1("Pier-Podhori", 1);
Store Pier2("Pier-Podbaba", 1);

//Processes Boat, Passager, Time

class WorkTime : public Process{
    void Behavior(){
        printf("Call boat %f\n", Time);
        DayTime->Activate;
    }
};



class DayTime : public Process{
    void Behavior(){
        for(int i = 0; i < countOfStarts; i++){
            (new WorkTime)->Activate(Time+(15/60));
            Passivate();
            //call activate boat
            printf("Call boat %f\n", Time);
        }

        Activate(Time+(7));
        printf("Call boat %f\n", Time);

        //call activate boat
    }
};


class Boat : public Process{
    void Behavior(){

    }
};

class Passager : public Process{
    void Behavior(){
            printf("%d. Passager (%f)\n", count++ , Time);

    }
};



class GeneratorOfHumans : public Event {

    void Behavior(){
        (new Passager)->Activate();
        Activate(Time + Exponential(generatePeople));
    }
};


int main() {
    unsigned long capacity = CAPACITY;
    //global variable
    generatePeople = GENERATE_PEOPLE;
    countOfStarts = COUNT_OF_STARTS;

    //TODO parse aguments

    Boat.SetCapacity(capacity);

    //SET PARAMETERS
    SetOutput("ims_output.out");

    //INITIALIZATION OF SIMULATION
    Init(0, RUNTIME);

    //GENERATORS

    (new DayTime)->Activate();
    (new GeneratorOfHumans)->Activate();

    //START SIMULATION
    Run();

    //OUTPUTS
    std::cout << "Hello, World!" << std::endl;


    return 0;
}