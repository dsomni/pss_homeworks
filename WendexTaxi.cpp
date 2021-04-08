#include<iostream>

#include "Gateways.cpp"

using namespace std;

/********************************************************
*                Wendex.Taxi main class                 *
********************************************************/

class WendexTaxi{
    DataBase db = DataBase("..\\db");
    vector<Car*> CARS;
    vector<Order*> ORDERS;
    vector<Passenger*> PASSENGERS;
    vector<Driver*> DRIVERS;

    // Simulate situation before riding
    void startRiding(PassengerGateway* p, DriverGateway* d){
        cout << "Passenger " + p->getName() + " is waiting he/her taxi\n\nCoordinates of car:" << endl;
        for(int i=0;i<5;i++){
            auto coords = d->getCar()->getCoordinates();
            cout << coords.first << " " << coords.second << endl;
        }
        cout << endl;
    }

    // Simulate actual riding
    void riding(PassengerGateway* p, DriverGateway* d){
        cout << "Riding for passenger " + p->getName() + " starts" << endl;
        int f_time = p->getCurrentOrder()->getStartTime();

        for(int i=0;i<rand()%10;i++){
            f_time+=5*60*1000; // add five minutes
            if(rand()%3) d->getCar()->pickFreeBottleOfWater();
            db.updateCar(d->getCar());
        }
        p->getCurrentOrder()->setFinishTIme(f_time);
    }

    // Simulate situation after riding
    void finishRiding(PassengerGateway* p, DriverGateway* d){
        cout << "Riding for passenger " + p->getName() + " ends\n" << endl;

        auto order = p->getCurrentOrder();
        order->setDescription("from: " + order->getStartAddress() +
        "; to: " + order->getFinishAddress() +
        "; time (minutes): " +to_string((order->getFinishTime() - order->getStartTime())/1000/60) +
        "; driver: " + d->getName() + "; passenger: " + p->getName());

        p->updateRating(5.0);
        d->updateRating( 4.5);

        d->setRidingStatus(false);
        p->setWaitingStatus(false);
        p->clearCurrentOrder();

        db.updatePassenger(p->getUser());
        db.updateDriver(d->getUser());
        db.updateOrder(order);

        updateDB();
    }

public:

    WendexTaxi(){
        init();
    }

    // Initialize Collections (can be useful in general)
    void init(){
        CARS = db.getAllCars();
        ORDERS = db.getAllOrders();
        PASSENGERS = db.getAllPassengers();
        DRIVERS = db.getAllDrivers();
    }

    void updateDB(){
        init();
    }

    /* Every call new driver, passenger & car are generated
     * Demonstrate dealing with new users
     *
     * You can see information in the terminal or
     * explicitly in DB files
     * */
    void startSimulationNewUsers(){
        srand(time(NULL));
        Car* car = new Car("Audi","YELLOW",1001+rand(),Car::types::COMFORT);

        db.addNewCar(car);
        CARS.push_back(car);
        string d_l = "drive"+to_string(rand());
        string p_l = "pass"+to_string(rand());
        DRIVERS.push_back(DriverGateway::Register(d_l,"4321","Driver_"+to_string(rand())));
        PASSENGERS.push_back(PassengerGateway::Register(p_l,"1234","Passenger_"+to_string(rand())));

        auto* d1 = new DriverGateway(d_l,"4321");
        auto* p1 = new PassengerGateway(p_l,"1234");
        d1->setCar(car);

        cout << "Price:" << endl;
        cout << p1->checkPrice("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT) << endl;
        ORDERS.push_back(p1->orderRide("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT));

        auto availableOrders = d1->checkAvailableOrders();
        d1->acceptOrder(availableOrders[0]);

        // It is necessary to update information from DB (from time to time)
        d1 = new DriverGateway(d_l,"4321");
        p1 = new PassengerGateway(p_l,"1234");

        startRiding(p1,d1);
        riding(p1,d1);
        finishRiding(p1,d1);

        d1 = new DriverGateway(d_l,"4321");
        p1 = new PassengerGateway(p_l,"1234");

        cout << "Passenger's order history:" << endl;
        for(auto order : p1->seeOrderHistory()){
            cout << order.getDescription() << endl;
        }
        cout << endl;

        cout << "Driver's order history:" << endl;
        for(auto order : d1->seeOrderHistory()){
            cout << order.getDescription() << endl;
        }
        cout << endl;
    }

    /* During first call generates new driver, user and car
     * Next calls dealing with existing objects
     *
     * Demonstrate storing and updating information
     *
     * Every time ew order and address generate: you can see them in terminal
     * or explicitly in DB files
     * */
    void startSimulationExistingUsers(){
        srand(time(NULL));
        Car* car = new Car("Audi","YELLOW",1001,Car::types::COMFORT);

        car = db.addNewCar(car);
        CARS.push_back(car);
        string d_l = "drive0";
        string p_l = "pass0";
        DRIVERS.push_back(DriverGateway::Register(d_l,"1111","Vova"));
        PASSENGERS.push_back(PassengerGateway::Register(p_l,"2222","Vanya"));

        auto* d1 = new DriverGateway(d_l,"1111");
        auto* p1 = new PassengerGateway(p_l,"2222");
        d1->setCar(car);

        string where = "Kazan, Bauman st. N" + to_string(rand());
        cout << "Price:" << endl;
        cout << p1->checkPrice("Innopolis, Universitetslaya d.3",where,Car::types::BUSINESS) << endl << endl;
        ORDERS.push_back(p1->orderRide("Innopolis, Universitetslaya d.3",where,Car::types::BUSINESS));

        auto newPinnedAddresses = p1->seePinnedAddresses();
        newPinnedAddresses.push_back(where);
        p1->updatePinnedAddresses(newPinnedAddresses);

        auto availableOrders = d1->checkAvailableOrders();
        d1->acceptOrder(availableOrders[0]);

        d1 = new DriverGateway(d_l,"1111");
        p1 = new PassengerGateway(p_l,"2222");

        startRiding(p1,d1);
        riding(p1,d1);
        finishRiding(p1,d1);

        d1 = new DriverGateway(d_l,"1111");
        p1 = new PassengerGateway(p_l,"2222");

        cout << "Passenger's order history:" << endl;
        for(auto order : p1->seeOrderHistory()){
            cout << order.getDescription() << endl;
        }
        cout << endl;

        cout << "Driver's order history:" << endl;
        for(auto order : d1->seeOrderHistory()){
            cout << order.getDescription() << endl;
        }
        cout << endl;
    }
};
