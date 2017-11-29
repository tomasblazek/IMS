#include <iostream>
#include <simlib.h>

//Macros
#define seconds(x) (x)
#define minutes(x) ((x)*60)
#define hours(x) (minutes((x)*60))
#define days(x) (hours((x)*24))

using namespace std;

//Default values of simulation
#define COUNT_OF_STARTS 68

#define GENERATE_PEOPLE 315 // 274 per day (17hours of work) | 1passager/315seconds

#define CAPACITY 40

#define RUNTIME hours(2500) // 182days



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
Facility Boat_time("Boat");

//Processes Boat_place, Passager1, Time

Queue queue1;
Queue queue2;
Queue queueLoaded;



class DayTime : public Process{

    void Behavior() {
        Seize(Pier1);
        Seize(Pier2);
        Seize(Boat_time);

        double timeBefore;
        double waitingTime1 = 0;
        double waitingTime2 = 0;
        double uniform = 0;
        int days = 0;
        while(true){
            Release(Boat_time);
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

                Wait(minutes(15)-waitingTime1-waitingTime2-uniform);

                timeBefore = Time;
                Seize(Pier1);
                waitingTime1 = Time - timeBefore;
                printf("BoatRide1 (%f)\n\n", Time);
                uniform = Uniform(minutes(1),minutes(2));
                Wait(uniform);



                printf("-------Ready in Pier2-------\n");
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

                printf("BoatRide2(back) (%f)\n\n", Time);
                uniform = Uniform(minutes(1),minutes(2));
                Wait(uniform);
                waitingTime2 = minutes(4)+waitingTime2;
                waitingTime1 = 0;
            }
            printf("-------(Last-just uboard)Ready in Pier1-------\n");

            Release(Pier1);
            Seize(Boat_time);
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
            if(!queue2.Empty() && !Boat_time.Busy()){
                queue2.GetFirst()->Activate();
            }
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
            if(!queue1.Empty() && !Boat_time.Busy()){
                queue1.GetFirst()->Activate();
            }
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