#include <iostream>
#include <simlib.h>
#include <getopt.h>

//Macros
#define seconds(x) (x)
#define minutes(x) (seconds((x)*60))
#define hours(x) (minutes((x)*60))
#define days(x) (hours((x)*24))

using namespace std;

//Default values of simulation
#define COUNT_OF_STARTS 68

//Season 1425/per day | Outseason 351/per day
#define GENERATE_PEOPLE 98

#define CAPACITY 40

#define RUNTIME days(1) // 182days



//Globals
double generatePeople = GENERATE_PEOPLE;
double countOfStarts =  COUNT_OF_STARTS;


Histogram Time_of_Passager1("Čas pasažéra(Podhoří-Podbaba) stráveného na cestě",0, minutes(1), 25);
Histogram Time_of_Passager2("Čas pasažéra(Podbaba-Podhoří) stráveného na cestě",0, minutes(1), 25);


Queue queue1("Fronta v Podhoří");
Queue queue2("Fronta v Podbabě");
Queue queueLoaded("Pasažeři nastoupení v lodi");

//Stores Boat_place, Pier
Store Boat_place("Boat_place capacity", CAPACITY);
Facility Pier1("Molo-Podhori");
Facility Pier2("Molo-Podbaba");
bool Boat_time;


//Processes Boat_place, Passager1, Time



class DayTime : public Process{

    void Behavior() {
        Seize(Pier1);
        Seize(Pier2);
        Boat_time = false;

        double timeBefore;
        double waitingTime1 = 0;
        double waitingTime2 = 0;
        double uniform = 0;

        Priority = 1;
        while(true){
            Boat_time = true;
            for(int i = 0; i < countOfStarts; i++){
                Release(Pier1);
                if(!queueLoaded.Empty()){
                    (queueLoaded.GetFirst())->Activate();
                }else{
                    if(!queue1.Empty()){
                        queue1.GetFirst()->Activate();
                    }
                }

                Wait(hours(17/countOfStarts)-waitingTime1-waitingTime2-uniform);

                timeBefore = Time;
                Seize(Pier1);
                waitingTime1 = Time - timeBefore;


                uniform = Uniform(minutes(1),minutes(2));
                Wait(uniform);




                Release(Pier2);
                if(!queueLoaded.Empty()){
                    (queueLoaded.GetFirst())->Activate();
                }else{
                    if(!queue2.Empty()){
                        queue2.GetFirst()->Activate();
                    }
                }

                Wait(minutes(4)-waitingTime1-uniform);

                timeBefore = Time;
                Seize(Pier2);
                waitingTime2 = Time - timeBefore;


                uniform = Uniform(minutes(1),minutes(2));
                Wait(uniform);
                waitingTime2 = minutes(4)+waitingTime2;
                waitingTime1 = 0;
            }

            Release(Pier1);
            Boat_time = false;


            if(!queueLoaded.Empty()){
                (queueLoaded.GetFirst())->Activate();
            }

            //When work day is gone ...People leaves queue
            while(!queue1.Empty()){
                queue1.GetFirst()->Activate();
            }
            while(!queue2.Empty()){
                queue2.GetFirst()->Activate();
            }

            Wait(hours(7)-waitingTime2-uniform); //problem s nevystupovaním
            Seize(Pier1);


            waitingTime2 = 0;
            waitingTime1 = 0;
            uniform = 0;
        }
    }
};


class Passager1 : public Process{
    void Behavior() {
        double startOfPassager = Time;


        if(!queue1.Empty()){
            queue1.Insert(this);
            Passivate();
        }

        repeatBoard:

        if(Boat_time) {
            if (!Boat_place.Full() && !Pier1.Busy()) {
                Seize(Pier1);
            } else {
                queue1.Insert(this);
                Passivate();
                goto repeatBoard;
            }
            double uniform = Uniform(seconds(1), seconds(3));//1-3s
            Wait(uniform);
            Enter(Boat_place);

            Release(Pier1);

            if (!queue1.Empty()) {
                (queue1.GetFirst())->Activate();
            }

            queueLoaded.Insert(this);
            Passivate();

            Seize(Pier2);
            uniform = Uniform(seconds(1), seconds(3));//1-3s
            Wait(uniform);
            Leave(Boat_place);

            Release(Pier2);
            if (!queueLoaded.Empty()) {
                (queueLoaded.GetFirst())->Activate();
            } else {
                if (!queue2.Empty() && Boat_time) {
                    queue2.GetFirst()->Activate();
                }
            }
        }

        Time_of_Passager1(Time - startOfPassager);
    }
};

class Passager2 : public Process{
    void Behavior() {
        double startOfPassager = Time;

        if(!queue2.Empty()){
            queue2.Insert(this);
            Passivate();
        }

        repeatBoard:

        if(Boat_time){
            if (!Boat_place.Full() && !Pier2.Busy()) {
                Seize(Pier2);
            } else {

                queue2.Insert(this);
                Passivate();
                goto repeatBoard;
            }
            double uniform = Uniform(seconds(1), seconds(3)); //1-3s
            Wait(uniform);
            Enter(Boat_place);

            Release(Pier2);

            if (!queue2.Empty()) {
                (queue2.GetFirst())->Activate();
            }

            queueLoaded.Insert(this);
            Passivate();

            Seize(Pier1);
            uniform = Uniform(seconds(1), seconds(3));//1-3s
            Wait(uniform);
            Leave(Boat_place);

            Release(Pier1);
            if (!queueLoaded.Empty()) {
                (queueLoaded.GetFirst())->Activate();
            } else {
                if (!queue1.Empty() && Boat_time) {
                    queue1.GetFirst()->Activate();
                }
            }
        }

        Time_of_Passager2(Time - startOfPassager);
    }
};

class GeneratorOfPassager1 : public Event {

    void Behavior(){
        if(Boat_time)
            (new Passager1)->Activate();
        Activate(Time + Exponential(generatePeople));
    }
};


class GeneratorOfPassager2 : public Event {

    void Behavior(){
        if(Boat_time)
            (new Passager2)->Activate();
        Activate(Time + Exponential(generatePeople));
    }
};



void printHelp(){
    printf("This Is Help!\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    unsigned long capacity = CAPACITY;
    unsigned long runtime = RUNTIME;
    //global variable
    generatePeople = GENERATE_PEOPLE;
    countOfStarts = COUNT_OF_STARTS;
    string outputFile = "output.out";


    //TODO parse aguments
    int c;
    while ((c = getopt (argc, argv, "ht:c:s:p:f:")) != -1 ) {
        switch (c) {
            case 'h':
                printHelp();
                break;
            case 't':
                runtime = days(strtoul(optarg, NULL, 0));
                break;
            case 'c':
                capacity = strtoul(optarg, NULL, 0);
                break;
            case 's':
                countOfStarts = strtoul(optarg, NULL, 0);
                break;
            case 'p':
                generatePeople = strtoul(optarg, NULL, 0);
                break;
            case 'f':
                outputFile = optarg;
                break;
            case '?':
                fprintf(stderr,"Unknown argument: %s of %c\n", optarg, c);
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }


    Boat_place.SetCapacity(capacity);

    //SET PARAMETERS
    SetOutput(outputFile.c_str());

    //INITIALIZATION OF SIMULATION
    Init(0, runtime);

    //GENERATORS

    (new DayTime)->Activate();
    (new GeneratorOfPassager1)->Activate();
    (new GeneratorOfPassager2)->Activate();

    //START SIMULATION
    Run();

    //OUTPUTS
    queue1.Output();
    queue2.Output();
    queueLoaded.Output();

    Boat_place.Output();

    Time_of_Passager1.Output();
    Time_of_Passager2.Output();


    return 0;
}