#include <iostream>
#include <simlib.h>

//Macros
#define seconds(x) (x)
#define minutes(x) ((x)*60)
#define hours(x) (minutes((x)*60))
#define days(x) (hours((x)*24))

using namespace std;

//Default values of simulation
#define COUNT_OF_STARTS 51

#define GENERATE_PEOPLE 315 // 274 per day (17hours of work) | 1passager/315seconds

#define CAPACITY 40

#define RUNTIME hours(49) // 182days



//Globals
double generatePeople = GENERATE_PEOPLE;
double countOfStarts =  COUNT_OF_STARTS;

//TODO dočasné
int count1 = 0;
int count2 = 0;



Histogram Time_of_Passager1("Čas pasažéra(Podhoří-Podbaba) stráveného na cestě",0, minutes(1), 25);
Histogram Time_of_Passager2("Čas pasažéra(Podbaba-Podhoří) stráveného na cestě",0, minutes(1), 25);


Queue queue1("Fronta v Podhoří");
Queue queue2("Fronta v Podbabě");
Queue queueLoaded("Fronta v lodi na výstup");

//Stores Boat_place, Pier
Store Boat_place("Boat_place capacity", CAPACITY);
Facility Pier1("Pier-Podhori");
Facility Pier2("Pier-Podbaba");
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
        int days = 0;
        while(true){
            Boat_time = true;
            printf("==========DAY: %d (%f)\n==========", days++, Time);
            for(int i = 0; i < countOfStarts; i++){
                printf("-------(%d)Ready in Pier1-------\n",i);
                Release(Pier1);
                if(!queueLoaded.Empty()){
                    (queueLoaded.GetFirst())->Activate();
                }else{
                    if(!queue1.Empty()){
                        queue1.GetFirst()->Activate();
                    }
                }

                Wait(minutes(20)-waitingTime1-waitingTime2-uniform);

                Boat_time = false;
                timeBefore = Time;
                Seize(Pier1);
                waitingTime1 = Time - timeBefore;
                Boat_time = true;


                printf("BoatRide1 (%f)\n\n", Time);
                uniform = Uniform(minutes(1),minutes(2));
                Wait(uniform);



                printf("-------(%d)Ready in Pier2-------\n",i);
                Release(Pier2);
                if(!queueLoaded.Empty()){
                    (queueLoaded.GetFirst())->Activate();
                }else{
                    if(!queue2.Empty()){
                        queue2.GetFirst()->Activate();
                    }
                }

                Wait(minutes(4)-waitingTime1-uniform);

                Boat_time = false;
                timeBefore = Time;
                Seize(Pier2);
                waitingTime2 = Time - timeBefore;
                Boat_time = true;

                printf("BoatRide2(back) (%f)\n\n", Time);
                uniform = Uniform(minutes(1),minutes(2));
                Wait(uniform);
                waitingTime2 = minutes(4)+waitingTime2;
                waitingTime1 = 0;
            }
            printf("-------(Last-justUnboard)Ready in Pier1-------\n");

            Release(Pier1);
            Boat_time = false;
            if(!queueLoaded.Empty()){
                (queueLoaded.GetFirst())->Activate();
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
        int id = count1;
        printf("%d. Passager1 (%f)\n", count1++, Time);


        if(!queue1.Empty()){
            printf("Passeger1(%d) going to queue1\n",id);
            queue1.Insert(this);
            Passivate();
        }

        repeatBoard:
        if(!Boat_place.Full() && !Pier1.Busy() && Boat_time) {
            Seize(Pier1);
        }else{
            printf("Passeger1(%d) going to queue1\n",id);
            queue1.Insert(this);
            Passivate();
            goto repeatBoard;
        }
        double uniform = Uniform(seconds(1),seconds(3));//1-3s
        Wait(uniform);
        Enter(Boat_place);
        printf("Board passager1(%d) in Pier1 (%f)\n",id ,Time+uniform);
        Release(Pier1);

        if(!queue1.Empty()){
            (queue1.GetFirst())->Activate();
        }

        queueLoaded.Insert(this);
        Passivate();

        Seize(Pier2);
        uniform = Uniform(seconds(1),seconds(3));//1-3s
        Wait(uniform);
        Leave(Boat_place);
        printf("Unboard1 passager1(%d) in time (%f)\n", id , Time + uniform);
        Release(Pier2);
        if(!queueLoaded.Empty()){
            (queueLoaded.GetFirst())->Activate();
        } else{
            if(!queue2.Empty() && Boat_time){
                queue2.GetFirst()->Activate();
            }
        }

        Time_of_Passager1(Time - startOfPassager);
    }
};

class Passager2 : public Process{
    void Behavior() {
        double startOfPassager = Time;
        int id = count2;
        printf("%d. Passager2 (%f)\n", count2++, Time);

        if(!queue2.Empty()){
            printf("Passeger2(%d) going to queue2\n",id);
            queue2.Insert(this);
            Passivate();
        }

        repeatBoard:
        if(!Boat_place.Full() && !Pier2.Busy() && Boat_time) {
            Seize(Pier2);
        }else{
            printf("Passeger2(%d) going to queue2\n",id);
            queue2.Insert(this);
            Passivate();
            goto repeatBoard;
        }
        double uniform = Uniform(seconds(1),seconds(3)); //1-3s
        Wait(uniform);
        Enter(Boat_place);
        printf("Board passager2(%d) in Pier2 (%f)\n", id, Time + uniform);
        Release(Pier2);

        if(!queue2.Empty()){
            (queue2.GetFirst())->Activate();
        }

        queueLoaded.Insert(this);
        Passivate();

        Seize(Pier1);
        uniform = Uniform(seconds(1),seconds(3));//1-3s
        Wait(uniform);
        Leave(Boat_place);
        printf("Unboard2 passager2(%d) in time (%f)\n", id, Time+uniform);
        Release(Pier1);
        if(!queueLoaded.Empty()){
            (queueLoaded.GetFirst())->Activate();
        } else {
            if(!queue1.Empty() && Boat_time){
                queue1.GetFirst()->Activate();
            }
        }

        Time_of_Passager2(Time - startOfPassager);
        //výstup
        //Cesta
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


int main() {
    unsigned long capacity = CAPACITY;
    //global variable
    generatePeople = GENERATE_PEOPLE;
    countOfStarts = COUNT_OF_STARTS;

    //TODO parse aguments

    Boat_place.SetCapacity(capacity);

    //SET PARAMETERS
    SetOutput("ims_output.out");

    //INITIALIZATION OF SIMULATION
    Init(0, RUNTIME);

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

    Time_of_Passager1.Output();
    Time_of_Passager2.Output();



    return 0;
}