#include <string>
#include <utility>
#include <vector>
#include <ctime>

#include "DataBase.cpp"

using namespace std;

/********************************************************
*               Gateways' interfaces                    *
********************************************************/

//  Gateway Error class
//  *************************************
class Error:exception{
    string description;
public:
    explicit Error(string description){
        this->description = move(description);
    }
    string getDescription(){
        return description;
    }

};

//  Interface for Passengers
//  **************************************
class PassengerGateway{
    static inline DataBase db = DataBase("..\\db");
    Passenger* user;

public:
    static Passenger* Register(const string& login, const string& password, const string& name){
        Passenger* newP = new Passenger(name,login, password);
        if(db.getPassenger(newP->getLogin(),"", false ) == nullptr){
            db.addNewPassenger(newP);
            return newP;
        }
        return db.getPassenger(newP->getLogin(),"", false );
    }

    PassengerGateway(const string& login, const string& password){
        user = db.getPassenger(login, password);
    }

    // "See" and "Get" methods
    // ********************************
    vector<Order> seeOrderHistory(){
        if(user==nullptr)
            return *new vector<Order>;
        return user->getOrderHistory();
    }

    vector<string> seePinnedAddresses(){
        if(user==nullptr)
            return *new vector<string>;
        return user->getPinnedAddresses();
    }

    Passenger::paymentMethods seePaymentMethods(){
        if(user==nullptr)
            return Passenger::paymentMethods(0);
        return user->getPaymentMethods();
    }

    Order* getCurrentOrder(){
        if(user==nullptr)
            return nullptr;
        return user->getCurrentOrder();
    }

    int checkPrice(const string& from, const string& to, Car::types carType){
        if(user==nullptr)
            return 0;
        srand(time(NULL));
        //return (db.getCarNumber() - db.getFreeCarNumber()+1)*100 + carType*100 + rand()%100 - int(user->getRating()*10);
        return carType*100 + rand()%100 - int(user->getRating()*10);
    }

    pair<float,float> askCoordinates(){
        if(user==nullptr)
            return make_pair(0,0);
        if(user->getWaitingStatus()){
            return db.getDriver(user->getCurrentOrder()->getDriverID(),"",false)->getCoordinates();
        }
        return pair<float,float>(0,0);
    }

    string getName(){
        if(user==nullptr)
            return "";
        return user->getName();
    }

    Passenger* getUser(){
        return user;
    }

    // "Update" and "Set" methods
    // ********************************
    void updatePaymentMethods(Passenger::paymentMethods method){
        if(user==nullptr)
            return;
        user->setPaymentMethods(method);
        db.updatePassenger(user);
    };

    void updatePinnedAddresses(vector<string> pinnedAddresses){
        if(user==nullptr)
            return;
        user->setPinnedAddresses(pinnedAddresses);
        db.updatePassenger(user);
    };

    void updateRating(float rating){
        if(user==nullptr)
            return;
        user->setRating(rating);
    }

    void setWaitingStatus(bool status){
        if(user==nullptr)
            return;
        user->setWaitingStatus(status);
    }

    void clearCurrentOrder(){
        if(user==nullptr)
            return;
        user->clearCurrentOrder();
    }

    // "Special" methods
    // ********************************
    Order* orderRide(const string& from, const string& to, Car::types carType, Passenger::paymentMethods paymentMethod = Passenger::paymentMethods::CREDIT_CARD){
        if(user==nullptr)
            return nullptr;
        if(user->getWaitingStatus()) throw Error("You have already made the order!");
        if(!user->getOrderAbility()) throw Error("You are not allowed to make orders!");
        auto* order = new Order(time(NULL), from, to, carType, user->getLogin(), db.getOrdersNumber());
        user->setWaitingStatus(true);
        user->setCurrentOrder(order);

        db.addNewOrder(order);
        db.updatePassenger(user);
        return order;
    }
};


//  Interface for Drivers
//  **************************************
class DriverGateway{
    static inline DataBase db = DataBase("..\\db");
    Driver* user;
    vector<Order> declined;
public:

    static Driver* Register(const string& login, const string& password, const string& name){
        Driver* newD = new Driver(name,login, password);
        if(db.getDriver(newD->getLogin(),"", false ) == nullptr){
            db.addNewDriver(newD);
            return newD;
        }
        return db.getDriver(newD->getLogin(),"", false );
    }

    DriverGateway(const string& login, const string& password){
        user = db.getDriver(login, password);
    }

    // "See" and "Get" methods
    // ********************************
    vector<Order> seeOrderHistory(){
        if(user==nullptr)
            return *new vector<Order>;
        return user->getOrderHistory();
    }

    vector<Order> checkAvailableOrders(){
        if(user==nullptr)
            return *new vector<Order>;
        return db.getAvailableOrders(declined);
    }

    Car* getCar(){
        if(user==nullptr)
            return nullptr;
        return user->getCar();
    }

    vector<Car*> getCars(){
        if(user==nullptr)
            return *new vector<Car*>;
        return user->getCars();
    }

    string getName(){
        if(user==nullptr)
            return "";
        return user->getName();
    }

    Driver* getUser(){
        return user;
    }

    // "Update" and "Set" methods
    // ********************************
    void updateRating(float rating){
        if(user==nullptr)
            return;
        user->setRating(rating);
    }

    void setRidingStatus(bool status){
        if(user==nullptr)
            return;
        user->setRidingStatus(status);
    }

    void addCar(Car* car){
        if(user==nullptr || car== nullptr)
            return;
        if(!car->getValidity()) throw Error("Car is not validated!");
        user->addCar(car);
    }

    // "Special" methods
    // ********************************
    void declineOrder(const Order& order){
        if(user==nullptr)
            return;
        declined.push_back(order);
    }

    void acceptOrder(Order order){
        if(user==nullptr)
            return;
        if(!user->getWorkingStatus()) throw Error("To accept an order you have to be at work!");
        if(user->getRidingStatus()) throw Error("You have already accepted order!");
        if(!user->getAcceptAbility()) throw Error("You are nor allowed to accept orders!");

        Passenger* customer = db.getPassenger(order.getCustomerID(),"",false);

        order.setPickedStatus(true);
        order.setDriverID(user->getLogin());

        user->updateOrderHistory(order);
        customer->updateOrderHistory(order);

        user->setRidingStatus(true);
        customer->setWaitingStatus(true);

        db.updatePassenger(customer);
        db.updateDriver(user);
        db.updateOrder(&order);
    }
};


//  Interface for Admins
//  **************************************
class AdminGateway{
    static inline DataBase db = DataBase("..\\db");
    Admin* user;
public:

    static Admin* Register(const string& login, const string& password, const string& name){
        auto* newA = new Admin(name,login, password);
        if(db.getAdmin(newA->getLogin(),"", false ) == nullptr){
            db.addNewAdmin(newA);
            return newA;
        }
        return db.getAdmin(newA->getLogin(),"", false );
    }

    AdminGateway(const string& login, const string& password){
        user = db.getAdmin(login, password);
    }

    // "See" and "Get" methods
    // ********************************
    string getName(){
        if(user==nullptr)
            return "";
        return user->getName();
    }

    Admin* getUser(){
        return user;
    }

    string seeAllInformation(){
        if(user==nullptr)
            return "";
        return db.getAllInformation();
    }

    // "Update" and "Set" methods
    // ********************************


    // "Special" methods
    // ********************************
    void validateCar(Car* car){
        if(user==nullptr || car== nullptr)
            return;
        car->setValidity(true);
        db.updateCar(car);
    }

    void validateCar(const string& carNumber){
        if(user==nullptr)
            return;
        Car* car = db.getCar(carNumber);
        if(car==nullptr)
            return;
        car->setValidity(true);
        db.updateCar(car);
    }

    void blockPossibilityToOrder(Passenger* passenger){
        if(user==nullptr || passenger== nullptr)
            return;
        passenger->setOrderAbility(false);
        db.updatePassenger(passenger);
    }

    void blockPossibilityToOrder(const string& passenger_login){
        if(user==nullptr)
            return;
        Passenger* passenger = db.getPassenger(passenger_login,"",false);
        if(passenger==nullptr)
            return;
        passenger->setOrderAbility(false);
        db.updatePassenger(passenger);
    }

    void unblockPossibilityToOrder(Passenger* passenger){
        if(user==nullptr || passenger== nullptr)
            return;
        passenger->setOrderAbility(true);
        db.updatePassenger(passenger);
    }

    void unblockPossibilityToOrder(const string& passenger_login){
        if(user==nullptr)
            return;
        Passenger* passenger = db.getPassenger(passenger_login,"",false);
        if(passenger==nullptr)
            return;
        passenger->setOrderAbility(true);
        db.updatePassenger(passenger);
    }

    void blockPossibilityToAccept(Driver* driver){
        if(user==nullptr || driver== nullptr)
            return;
        driver->setAcceptAbility(false);
        db.updateDriver(driver);
    }

    void blockPossibilityToAccept(const string& driver_login){
        if(user==nullptr)
            return;
        Driver* driver = db.getDriver(driver_login,"",false);
        if(driver==nullptr)
            return;
        driver->setAcceptAbility(false);
        db.updateDriver(driver);
    }

    void unblockPossibilityToAccept(Driver* driver){
        if(user==nullptr || driver== nullptr)
            return;
        driver->setAcceptAbility(true);
        db.updateDriver(driver);
    }

    void unblockPossibilityToAccept(const string& driver_login){
        if(user==nullptr)
            return;
        Driver* driver = db.getDriver(driver_login,"",false);
        if(driver==nullptr)
            return;
        driver->setAcceptAbility(true);
        db.updateDriver(driver);
    }

};