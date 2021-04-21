#include<iostream>

#include "Gateways.cpp"

using namespace std;

/********************************************************
*                Wendex.Taxi main class                 *
********************************************************/

class WendexTaxi{
    DataBase db = DataBase("..\\db");
    vector<Admin*> ADMINS;
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
        ADMINS = db.getAllAdmins();
        CARS = db.getAllCars();
        ORDERS = db.getAllOrders();
        PASSENGERS = db.getAllPassengers();
        DRIVERS = db.getAllDrivers();
    }

    void updateDB(){
        init();
    }

    /* Every call new admin, driver, passenger & car are generated
     *
     * Demonstrate dealing with new users + Validating car
     *
     * You can see information in the terminal or
     * explicitly in DB files
     * */
    void startSimulation1(){
        srand(time(nullptr));
        Car* car = new Car("Audi","YELLOW",1001+rand(),Car::types::COMFORT);

        db.addNewCar(car);
        CARS.push_back(car);
        string d_l = "drive"+to_string(rand());
        string p_l = "pass"+to_string(rand());
        string a_l = "admin"+ to_string(rand());
        DRIVERS.push_back(DriverGateway::Register(d_l,"4321","Driver_"+to_string(rand())));
        PASSENGERS.push_back(PassengerGateway::Register(p_l,"1234","Passenger_"+to_string(rand())));
        ADMINS.push_back(AdminGateway::Register(a_l,"1111","Admin_"+to_string(rand())));

        auto* d1 = new DriverGateway(d_l,"4321");
        auto* p1 = new PassengerGateway(p_l,"1234");
        auto* a1 = new AdminGateway(a_l,"1111");

        //  Validating car check (Driver can not add not validating car)
        //  ********************************************************
        try{
            d1->addCar(car);
        }catch (Error& err){
            cout << err.getDescription() << endl;
        }
        a1->validateCar(car);
        d1->addCar(car);


        cout << "Price:" << endl;
        cout << p1->checkPrice("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT) << endl;
        ORDERS.push_back(p1->orderRide("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT));

        auto availableOrders = d1->checkAvailableOrders();
        try{
            d1->acceptOrder(availableOrders[0]);
        }catch(Error& err){
            cout << err.getDescription() << endl;
        }


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

    /* During first call generates new admin, driver, user and car
     *
     * Next calls dealing with existing objects
     *
     * Demonstrate storing and updating information
     *
     * Every time new order and address generate: you can see them in terminal
     * or explicitly in DB files
     * */
    void startSimulation2(){
        srand(time(nullptr));
        Car* car = new Car("Audi","YELLOW",1001,Car::types::COMFORT);

        car = db.addNewCar(car);
        CARS.push_back(car);
        string d_l = "drive0";
        string p_l = "pass0";
        string a_l = "admin0";
        DRIVERS.push_back(DriverGateway::Register(d_l,"1111","Vova"));
        PASSENGERS.push_back(PassengerGateway::Register(p_l,"2222","Vanya"));
        ADMINS.push_back(AdminGateway::Register(a_l,"3333","Dima"));

        auto* d1 = new DriverGateway(d_l,"1111");
        auto* p1 = new PassengerGateway(p_l,"2222");
        auto* a1 = new AdminGateway(a_l,"3333");
        a1->validateCar(car);
        try{
            d1->addCar(car);
        }catch (Error& err){
            cout << err.getDescription() << endl;
        }

        string where = "Kazan, Bauman st. N" + to_string(rand());
        cout << "Price:" << endl;
        cout << p1->checkPrice("Innopolis, Universitetslaya d.3",where,Car::types::BUSINESS) << endl << endl;
        ORDERS.push_back(p1->orderRide("Innopolis, Universitetslaya d.3",where,Car::types::BUSINESS));

        auto newPinnedAddresses = p1->seePinnedAddresses();
        newPinnedAddresses.push_back(where);
        p1->updatePinnedAddresses(newPinnedAddresses);

        auto availableOrders = d1->checkAvailableOrders();
        try{
            d1->acceptOrder(availableOrders[0]);
        }catch(Error& err){
            cout << err.getDescription() << endl;
        }

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

    /* Every call new admin, driver, passenger & car are generated
     *
     * Demonstrate dealing with multiple devices:
     * Passenger can no order new taxi if he/she has already made order
     *
     * Also demonstrates, how Admin can "see all information"
     *
     * You can see information in the terminal or
     * explicitly in DB files
     * */
    void startSimulation3(){
        srand(time(nullptr));
        Car* car = new Car("Audi","YELLOW",1001+rand(),Car::types::COMFORT);

        car = db.addNewCar(car);
        CARS.push_back(car);
        string d_l = "drive"+to_string(rand());
        string p_l = "pass"+to_string(rand());
        string a_l = "admin"+ to_string(rand());
        DRIVERS.push_back(DriverGateway::Register(d_l,"4321","Driver_"+to_string(rand())));
        PASSENGERS.push_back(PassengerGateway::Register(p_l,"1234","Passenger_"+to_string(rand())));
        ADMINS.push_back(AdminGateway::Register(a_l,"1111","Admin_"+to_string(rand())));

        auto* d1 = new DriverGateway(d_l,"4321");
        auto* p1 = new PassengerGateway(p_l,"1234");
        auto* a1 = new AdminGateway(a_l,"1111");
        a1->validateCar(car);
        try{
            d1->addCar(car);
        }catch (Error& err){
            cout << err.getDescription() << endl;
        }

        string where = "Kazan, Bauman st. N" + to_string(rand());
        cout << "Price:" << endl;
        cout << p1->checkPrice("Innopolis, Universitetslaya d.3",where,Car::types::BUSINESS) << endl << endl;



        //  Here interesting things going on
        //  *****************************************
        ORDERS.push_back(p1->orderRide("Innopolis, Universitetslaya d.3",where,Car::types::BUSINESS)); // make order from one device
        auto* another_p1 = new PassengerGateway(p_l,"1234"); // try to make order from other device
        try{
            auto* order = another_p1->orderRide("Innopolis, Universitetslaya d.3",where,Car::types::BUSINESS);
            ORDERS.push_back(order);
        } catch (Error& err) {
            cout << err.getDescription() << endl;
        }
        //  *****************************************


        auto availableOrders = d1->checkAvailableOrders();
        try{
            d1->acceptOrder(availableOrders[0]);
        }catch(Error& err){
            cout << err.getDescription() << endl;
        }

        d1 = new DriverGateway(d_l,"4321");
        p1 = new PassengerGateway(p_l,"1234");

        startRiding(p1,d1);
        riding(p1,d1);
        finishRiding(p1,d1);

        // admin see all information
        cout << a1->seeAllInformation();

    }

    /* Every call new admin, driver, passenger & car are generated
     *
     * Demonstrate declining the order by driver
     *
     * You can see information in the terminal or
     * explicitly in DB files
     * */
    void startSimulation4(){
        srand(time(nullptr));
        Car* car = new Car("Audi","YELLOW",1001+rand(),Car::types::COMFORT);

        db.addNewCar(car);
        CARS.push_back(car);
        string d_l = "drive"+to_string(rand());
        string p_l = "pass"+to_string(rand());
        string a_l = "admin"+ to_string(rand());
        DRIVERS.push_back(DriverGateway::Register(d_l,"4321","Driver_"+to_string(rand())));
        PASSENGERS.push_back(PassengerGateway::Register(p_l,"1234","Passenger_"+to_string(rand())));
        ADMINS.push_back(AdminGateway::Register(a_l,"1111","Admin_"+to_string(rand())));

        auto* d1 = new DriverGateway(d_l,"4321");
        auto* p1 = new PassengerGateway(p_l,"1234");
        auto* a1 = new AdminGateway(a_l,"1111");

        try{
            d1->addCar(car);
        }catch (Error& err){
            cout << err.getDescription() << endl;
        }
        a1->validateCar(car);
        d1->addCar(car);

        cout << "Price:" << endl;
        cout << p1->checkPrice("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT) << endl;
        ORDERS.push_back(p1->orderRide("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT));

        // Declining the order
        auto availableOrders = d1->checkAvailableOrders();
        d1->declineOrder(availableOrders[0]);
        availableOrders = d1->checkAvailableOrders();
        if(!availableOrders.empty()){
            try{
                d1->acceptOrder(availableOrders[0]);
            }catch(Error& err){
                cout << err.getDescription() << endl;
            }
        }else{
            cout << "Driver has declined the order" << endl;
        }

    }

    /* Every call new admin, driver, passenger & car are generated
     *
     * Demonstrate blocking and unblocking functions for
     * driver and passenger by admin
     *
     * Also demonstrates, how Admin can "see all information"
     *
     * You can see information in the terminal or
     * explicitly in DB files
     * */
    void startSimulation5(){
        srand(time(nullptr));
        Car* car = new Car("Audi","YELLOW",1001+rand(),Car::types::COMFORT);

        db.addNewCar(car);
        CARS.push_back(car);
        string d_l = "drive"+to_string(rand());
        string p_l = "pass"+to_string(rand());
        string a_l = "admin"+ to_string(rand());
        DRIVERS.push_back(DriverGateway::Register(d_l,"4321","Driver_"+to_string(rand())));
        PASSENGERS.push_back(PassengerGateway::Register(p_l,"1234","Passenger_"+to_string(rand())));
        ADMINS.push_back(AdminGateway::Register(a_l,"1111","Admin_"+to_string(rand())));

        auto* d1 = new DriverGateway(d_l,"4321");
        auto* p1 = new PassengerGateway(p_l,"1234");
        auto* a1 = new AdminGateway(a_l,"1111");

        try{
            d1->addCar(car);
        }catch (Error& err){
            cout << err.getDescription() << endl;
        }
        a1->validateCar(car);
        d1->addCar(car);


        cout << "Price:" << endl;
        cout << p1->checkPrice("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT) << endl;


        // Blocking/Unblocking possibility to order for passenger
        a1->blockPossibilityToOrder(p1->getUser());
        Order* order;
        try{
            order = p1->orderRide("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT);
        }catch (Error& err){
            cout << err.getDescription() << endl;
        }
        a1->unblockPossibilityToOrder(p1->getUser());

        order = p1->orderRide("Innopolis, Universitetslaya d.3","Kazan, any Bar",Car::types::COMFORT);
        ORDERS.push_back(order);

        // Blocking/Unblocking possibility to accept order for driver
        a1->blockPossibilityToAccept(d1->getUser());
        auto availableOrders = d1->checkAvailableOrders();
        try{
            d1->acceptOrder(availableOrders[0]);
        }catch(Error& err){
            cout << err.getDescription() << endl;
        }

        cout << a1->seeAllInformation();

    }
};
