#include<string>
#include<utility>
#include<vector>

using namespace std;

/********************************************************
*               Schemes of objects                      *
********************************************************/

class Passenger;
class Driver;

// Car Scheme
class Car{
public:
    enum types{
        ECONOMY,
        COMFORT,
        COMFORT_PLUS,
        BUSINESS
    };
    static inline const vector<string> carTypesNames = {"ECONOMY",
                                                 "COMFORT", "COMFORT+", "BUSINESS"};
private:
    string model;
    float x;
    float y;
    string colour;
    int number;
    types carType;
    int freeBottleOfWater = 20;
    bool isValidated = false;

public:
    Car(const string& model, const string& colour, int number, types carType, float x = 0, float y = 0){
        this->model = model;
        this->x = x;
        this->y = y;
        this->colour = colour;
        this->number = number;
        this->carType = carType;
    }

    // "Get" methods
    string getModel(){
        return model;
    }

    string getColour(){
        return colour;
    }

    Car::types getCarType(){
        return carType;
    }

    string getCarTypeName(){
        return carTypesNames[int(carType)];
    }

    pair<float,float> getCoordinates(){
        this->x = rand();
        this->y = rand();
        return pair<float,float>(x,y);
    }

    int getNumberFreeBottleOfWater(){
        return (carType>=COMFORT)?freeBottleOfWater:0;
    }

    int getNumber(){
        return number;
    }

    bool getValidity(){
        return isValidated;
    }

    // "Update" and "Set" methods
    void setCarType(int type){
        switch(type){
            case 0:
                carType = Car::ECONOMY;
                break;
            case 1:
                carType = Car::COMFORT;
                break;
            case 2:
                carType = Car::COMFORT_PLUS;
                break;
            case 3:
                carType = Car::BUSINESS;
                break;
            default:
                carType = Car::ECONOMY;
        }
    }

    bool updateFreeBottleOfWater(){
        freeBottleOfWater = 20;
        if(carType>=COMFORT)
            return true;
        return false;
    }

    void setNumberFreeBottleWater(int n){
        freeBottleOfWater = n;
    }

    bool pickFreeBottleOfWater(){
        if(carType>=COMFORT){
            freeBottleOfWater-=1;
            if(freeBottleOfWater==0)
                updateFreeBottleOfWater();
            return true;
        }
        return false;
    }

    void setValidity(bool validity){
        isValidated = validity;
    }

    // "Special" methods
    bool parkRightInFrontOfTheEntrance(){
        if(carType==BUSINESS)
            return true;
        return false;
    }
};


// Order Scheme
class Order {
    int startTime; // milliseconds
    int finishTime;// milliseconds
    string startAddress;
    string finishAddress;
    int id;
    string customerID;
    string driverID;
    bool isPicked = false;
    Car::types carType;
    string description;

public:
    Order(int startTime, const string &startAddress, const string &finishAddress, int carType, string passengerID, int id) {
        this->startTime = startTime;
        this->startAddress = startAddress;
        this->finishAddress = finishAddress;
        setCarType(carType);
        this->customerID = move(passengerID);
        this->id = id;
    }

    // "Get" methods
    string getCustomerID(){
        return customerID;
    }

    string getStartAddress(){
        return startAddress;
    }

    string getFinishAddress(){
        return finishAddress;
    }

    bool getPickedStatus(){
        return isPicked;
    }

    int getID(){
        return id;
    }

    int getFinishTime(){
        return finishTime;
    }

    string getDriverID(){
        return driverID;
    }

    string getDescription(){
        return description;
    }

    int getStartTime(){
        return startTime;
    }

    Car::types getCarType(){
        return carType;
    }

    // "Update" and "Set" methods
    void setPickedStatus(bool newStatus){
        isPicked = newStatus;
    }

    void setFinishTIme(int ftime){
        finishTime = ftime;
    }

    void setDriverID(string driverID_){
        this->driverID = move(driverID_);
    }

    void setDescription(const string& description){
        this->description = description;
    }

    void setCarType(int type){
        switch(type){
            case 0:
                carType = Car::ECONOMY;
                break;
            case 1:
                carType = Car::COMFORT;
                break;
            case 2:
                carType = Car::COMFORT_PLUS;
                break;
            case 3:
                carType = Car::BUSINESS;
                break;
            default:
                carType = Car::ECONOMY;
        }
    }
};


// Person Scheme (for Passengers, Drivers & Admins)
class Person{
protected:
    string login;
    string password;
    string name;
    float rating;
    vector<Order> orderHistory;
    bool isWaiting = false;
    bool orderAbility = true;

public:
    Person(const string& name, float rating, vector<Order> orderHistory, const string& login, const string& password){
        this->name = name;
        this->rating = rating;
        this->orderHistory = move(orderHistory);
        this->password = password;
        this->login = login;
    }

    Person(const string& name, const string& login, const string& password){
        this->name = name;
        this->rating = 4.0;
        this->password = password;
        this->login = login;
    }

    // "Get" methods
    vector<Order> getOrderHistory(){
        return orderHistory;
    }

    string getLogin(){
        return login;
    }

    string getPassword(){
        return password;
    }

    float getRating(){
        return rating;
    }

    bool getWaitingStatus(){
        return isWaiting;
    }

    string getName(){
        return name;
    }

    bool getOrderAbility(){
        return orderAbility;
    }

    // "Update" and "Set" methods
    void updateOrderHistory(Order order){
        orderHistory.push_back(order);
    }

    void setWaitingStatus(bool newStatus){
        isWaiting = newStatus;
    }

    void setRating(float rating){
        this->rating = (this->rating + rating)/2;
    }

    void setOrderAbility(bool ability){
        orderAbility = ability;
    }
};


// Driver Scheme
class Driver: public Person{
    vector<Car*> cars;
    bool isWorking= true;
    bool isInRide = false;
    bool acceptAbility = true;

public:
    Driver(const string &name, const string& login, const string& password, float rating, vector<Order> orderHistory,
           vector<Car*> cars) : Person(name, rating, orderHistory, login, password) {
        this->name = name;
        this->rating = rating;
        this->orderHistory = move(orderHistory);
        this->cars = move(cars);
    }
    Driver(const string &name, const string& login, const string& password) :Person(name,login,password){}

    // "Get" methods
    Car* getCar(){
        if(cars.size()>=0)
            return cars[0];
        return nullptr;
    }

    vector<Car*> getCars(){
        return cars;
    }

    bool getWorkingStatus(){
        return isWorking;
    }

    bool getRidingStatus(){
        return isInRide;
    }

    pair<float,float> getCoordinates(){
        if(cars.size()>=0)
            return cars[0]->getCoordinates();
        return make_pair(0,0);
    }

    bool getAcceptAbility(){
        return acceptAbility;
    }

    // "Update" and "Set" methods
    void setWorkingStatus(bool newStatus){
        isWorking = newStatus;
    }

    void setRidingStatus(bool newStatus){
        isInRide = newStatus;
    }

    void setCars(vector<Car*> cars){
        this->cars = move(cars);
    }

    void addCar(Car* car){
        if(car== nullptr) return;
        for(auto existing_car : this->cars)
            if(existing_car!= nullptr && existing_car->getNumber() == car->getNumber())
                return;
        this->cars.push_back(car);
    }

    void setAcceptAbility(bool ability){
        acceptAbility = ability;
    }
};


// Passenger Scheme
class Passenger: public Person{
public:
    enum paymentMethods{
        CREDIT_CARD,
        CASH
    };

private:
    paymentMethods paymentMethod = CREDIT_CARD;
    vector<string> pinnedAddresses;
    Order* currentOrder = nullptr;

public:
    Passenger(const string& name, const string& login, const string& password, float rating, vector<Order> orderHistory,
              paymentMethods pM, vector<string> pinnedAddresses) : Person(name, rating, orderHistory,login, password){
        this->name = name;
        this->rating = rating;
        this->orderHistory = move(orderHistory);
        this->paymentMethod = pM;
        this->pinnedAddresses = pinnedAddresses;
    }

    Passenger(const string& name, const string& login, const string& password) : Person(name, login , password){
        this->name = name;
        this->paymentMethod = CREDIT_CARD;
    }

    // "Get" methods
    vector<string> getPinnedAddresses(){
        return pinnedAddresses;
    }

    paymentMethods getPaymentMethods(){
        return paymentMethod;
    }

    Order* getCurrentOrder(){
        return currentOrder;
    }

    // "Update" and "Set" methods
    void setPinnedAddresses(vector<string> pinned_addresses){
        pinnedAddresses = pinned_addresses;
    }

    void setPaymentMethods(int method){
        switch (method) {
            case 0:
                paymentMethod = CREDIT_CARD;
                break;
            case 1:
                paymentMethod = CASH;
                break;
            default:
                paymentMethod = CREDIT_CARD;
        }
    }

    void setPaymentMethods(paymentMethods method){
        paymentMethod = method;
    }

    void setCurrentOrder(Order* order){
        currentOrder = order;
    }

    void clearCurrentOrder(){
        this->currentOrder = nullptr;
    }
};


// Admin Scheme
class Admin: public Person{

public:

    Admin(const string &name, const string& login, const string& password) :Person(name,login,password){}
};