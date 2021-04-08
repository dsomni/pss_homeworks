#include <string>
#include <vector>
#include <ctime>

#include "DataBase.cpp"

using namespace std;

/********************************************************
*               Gateways' interfaces                    *
********************************************************/

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
        return user->getOrderHistory();
    }

    vector<string> seePinnedAddresses(){
        return user->getPinnedAddresses();
    }

    Passenger::paymentMethods seePaymentMethods(){
        return user->getPaymentMethods();
    }

    Order* getCurrentOrder(){
        return user->getCurrentOrder();
    }

    int checkPrice(const string& from, const string& to, Car::types carType){
        srand(time(NULL));
        //return (db.getCarNumber() - db.getFreeCarNumber()+1)*100 + carType*100 + rand()%100 - int(user->getRating()*10);
        return carType*100 + rand()%100 - int(user->getRating()*10);
    }

    pair<float,float> askCoordinates(){
        if(user->getWaitingStatus()){
            return db.getDriver(user->getCurrentOrder()->getDriverID(),"",false)->getCoordinates();
        }
        return pair<float,float>(0,0);
    }

    string getName(){
        return user->getName();
    }

    Passenger* getUser(){
        return user;
    }

    // "Update" and "Set" methods
    // ********************************
    void updatePaymentMethods(Passenger::paymentMethods method){
        user->setPaymentMethods(method);
        db.updatePassenger(user);
    };

    void updatePinnedAddresses(vector<string> pinnedAddresses){
        user->setPinnedAddresses(pinnedAddresses);
        db.updatePassenger(user);
    };

    void updateRating(float rating){
        user->setRating(rating);
    }

    void setWaitingStatus(bool status){
        user->setWaitingStatus(status);
    }

    void clearCurrentOrder(){
        user->clearCurrentOrder();
    }

    // "Special" methods
    // ********************************
    Order* orderRide(const string& from, const string& to, Car::types carType, Passenger::paymentMethods paymentMethod = Passenger::paymentMethods::CREDIT_CARD){
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
        return user->getOrderHistory();
    }

    vector<Order> checkAvailableOrders(){
        return db.getAvailableOrders(declined);
    }

    Car* getCar(){
        return user->getCar();
    }

    string getName(){
        return user->getName();
    }

    Driver* getUser(){
        return user;
    }

    // "Update" and "Set" methods
    // ********************************
    void updateRating(float rating){
        user->setRating(rating);
    }

    void setRidingStatus(bool status){
        user->setRidingStatus(status);
    }

    void setCar(Car* car){
        user->setCar(car);
    }

    // "Special" methods
    // ********************************
    void declineOrder(const Order& order){
        declined.push_back(order);
    }

    void acceptOrder(Order order){
        if(user->getWorkingStatus() && !user->getRidingStatus()){
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
    }
};