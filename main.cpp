#include <iostream>
#include <simlib.h>


//Default values of simulation
#define COUNT_OF_STARTS 68

#define GENERATE_PEOPLE 0.087591241 // 274 per day (17hours of work) | 11,4166666 per hour | 1human/0,087591241hours

#define CAPACITY 40

#define RUNTIME 40  //hours


//Globals
double generatePeople = GENERATE_PEOPLE;
double countOfStarts =  COUNT_OF_STARTS;
Entity *boat;

//TODO dočasné
int count1 = 0;
int count2 = 0;


//Stores Boat_place, Pier
Store Boat_place("Boat_place capacity", CAPACITY);
Facility Pier1("Pier-Podhori");
Facility Pier2("Pier-Podbaba");
Facility Boat("Boat");

//Processes Boat_place, Passager1, Time

Queue queue1;
Queue queue2;
Queue queueLoaded;


class BoatRide : public Process{
    void Behavior(){
        Wait(Uniform(0.016666667,0.033333333));
    }
};

class DayTime : public Process{

    void Behavior() {
        Seize(Pier1);
        Seize(Pier2);
        double timeBefore;
        double waitingTime;
        while(true){
            for(int i = 0; i < countOfStarts; i++){
                printf("-------Ready in Pier1-------\n");
                Release(Pier1);
                if(!queueLoaded.Empty()){
                    (queueLoaded.GetFirst())->Activate();
                }
                timeBefore = Time;
                WaitUntil(Boat_place.Empty());
                waitingTime = Time - timeBefore;

                if(!queue1.Empty()){
                    queue1.GetFirst()->Activate();
                }
                Wait(0.25-waitingTime);
                Seize(Pier1);
                printf("BoatRide1 (%f)\n\n", Time);
                (new BoatRide)->Activate();
                printf("-------Ready in Pier2-------\n");
                Release(Pier2);
                if(!queueLoaded.Empty()){
                    (queueLoaded.GetFirst())->Activate();
                }
                timeBefore = Time;
                WaitUntil(Boat_place.Empty());
                waitingTime = Time - timeBefore;

                if(!queue2.Empty()){
                    queue2.GetFirst()->Activate();
                }

                Wait(0.066666667-waitingTime);
                Seize(Pier2);
                printf("BoatRide2(back) (%f)\n\n", Time);
                (new BoatRide)->Activate();

            }
            printf("-------Ready in Pier1-------\n");
            Release(Pier1);
            if(!queueLoaded.Empty()){
                (queueLoaded.GetFirst())->Activate();
            }
            timeBefore = Time;
            WaitUntil(Boat_place.Empty());
            waitingTime = Time - timeBefore;
            Seize(Pier1);
            Wait(7-waitingTime);
        }
    }
};



class Passager1 : public Process{
    void Behavior() {
        int id = count1;
        printf("%d. Passager1 (%f)\n", count1++, Time);


        if(!queue1.Empty()){
            printf("Passeger1(%d) going to queue1\n",id);
            queue1.Insert(this);
            Passivate();
        }

        repeatBoard:
        if(!Boat_place.Full() && !Pier1.Busy()) {
            Seize(Pier1);
        }else{
            printf("Passeger1(%d) going to queue1\n",id);
            queue1.Insert(this);
            Passivate();
            goto repeatBoard;
        }
        double uniform = Uniform(0.000277778,0.000833333);
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
        uniform = Uniform(0.000277778,0.000833333);
        Wait(uniform);
        Leave(Boat_place);
        printf("Unboard1 passager1(%d) in time (%f)\n", id , Time + uniform);
        Release(Pier2);
        if(!queueLoaded.Empty()){
            (queueLoaded.GetFirst())->Activate();
        }

        //výstup
        //Cesta
    }
};

class Passager2 : public Process{
    void Behavior() {
        int id = count2;
        printf("%d. Passager2 (%f)\n", count2++, Time);

        if(!queue2.Empty()){
            printf("Passeger2(%d) going to queue2\n",id);
            queue2.Insert(this);
            Passivate();
        }

        repeatBoard:
        if(!Boat_place.Full() && !Pier2.Busy()) {
            Seize(Pier2);
        }else{
            printf("Passeger2(%d) going to queue2\n",id);
            queue2.Insert(this);
            Passivate();
            goto repeatBoard;
        }
        double uniform = Uniform(0.000277778,0.000833333);
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
        uniform = Uniform(0.000277778,0.000833333);
        Wait(uniform);
        Leave(Boat_place);
        printf("Unboard2 passager2(%d) in time (%f)\n", id, Time+uniform);
        Release(Pier1);
        if(!queueLoaded.Empty()){
            (queueLoaded.GetFirst())->Activate();
        }

        //výstup
        //Cesta
    }
};


class GeneratorOfHumans : public Event {

    void Behavior(){
        (new Passager1)->Activate();
        Activate(Time + Exponential(generatePeople));
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
    (new GeneratorOfHumans)->Activate();

    //START SIMULATION
    Run();

    //OUTPUTS


    return 0;
}