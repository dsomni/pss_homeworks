#include<iostream>
#include<vector>
#include<fstream>

#include "Schemes.cpp"

using namespace std;

/********************************************************
*          Class for managing data from DB              *
********************************************************/

class DataBase{
    const string driversDir = "drivers";
    const string carsDir = "cars";
    const string passengersDir = "passengers";
    const string ordersDir = "orders";

    string dirName;

    vector<string> carsID;
    vector<string> driversID;
    vector<string> orderID;
    vector<string> passengersID;

    // "Help" functions
    // ****************************************************************

    void split(string line, vector<string>& result){
        string delimiter = "*";

        size_t pos = 0;
        string token;
        result.clear();
        while ((pos = line.find(delimiter)) != string::npos) {
            token = line.substr(0, pos);
            result.push_back(token);
            line.erase(0, pos + delimiter.length());
        }
        result.push_back(line);
    }

    string getCollectionString(vector<string>& collection){
        string s;
        for(auto el: collection){
            s+=el+"*";
        }
        if(!s.empty())
            s.pop_back();
        return s;
    }

    bool find_Order(vector<Order>& where, Order* what){
        for(auto el:where){
            if(&el == what)
                return true;
        }
        return false;
    }

public:

    DataBase(const string &dirName){
        this->dirName = dirName;
        readConfigs();
    }

    // Configs
    // ****************************************************************

    // Read data FROM configs
    void readConfigs(){
        string line;
        ifstream in(dirName + "\\configs.txt");
        if (in.is_open())
        {
            if (getline(in, line))
                split(line,carsID);
            if (getline(in, line))
                split(line,driversID);
            if (getline(in, line))
                split(line,orderID);
            if (getline(in, line))
                split(line, passengersID);
        }
        in.close();
    }

    // Send data TO configs
    void updateConfigs(){
        string line;
        ofstream out(dirName + "\\configs.txt");
        if (out.is_open()){
            out << getCollectionString(carsID) << endl;
            out << getCollectionString(driversID) << endl;
            out << getCollectionString(orderID) << endl;
            out << getCollectionString(passengersID) << endl;
        }
        out.close();
    }


    // Passengers
    // ****************************************************************

    // Send Passenger's data to DB
    void updatePassenger(Passenger* user){
        readConfigs();
        string line;

        ofstream out(dirName + "\\" + passengersDir + "\\" + user->getLogin() + ".txt");
        if (out.is_open()){
            out << user->getName() << endl;
            out << to_string(user->getRating()) << endl;

            vector<Order> oh = user->getOrderHistory();
            string s;
            for(auto el: oh){
                s+=to_string(el.getID())+"*";
            }
            if(!s.empty())
                s.pop_back();
            out << s << endl;

            out << user->getLogin() << endl;
            out << user->getPassword() << endl;

            if(user->getWaitingStatus())
                out << "1" << endl;
            else
                out << "0" << endl;

            out << user->getPaymentMethods() << endl;

            vector<string> pA = user->getPinnedAddresses();
            s="";
            for(const string& el: pA){
                if(el.size()>1){
                    //cout << el << endl;
                    s+=el+"*";
                }
            }
            if(!s.empty())
                s.pop_back();
            out << s << endl;

            if(user->getCurrentOrder()!= nullptr)
                out << user->getCurrentOrder()->getID() << endl;
            else
                out << "-1" << endl;
        }
        out.close();
    }

    // Create new Passenger in DB
    void addNewPassenger(Passenger* user){
        updatePassenger(user);
        passengersID.push_back(user->getLogin());
        updateConfigs();
    }

    // Get Passenger's data from DB
    Passenger* getPassenger(const string& login, const string& password, bool checkPassword = true){
        readConfigs();
        string line;
        ifstream in(dirName + "\\" + passengersDir + "\\" + login + ".txt");

        if (in.is_open()){
            string name_;
            float rating_;
            vector<Order> orderHistory_;
            string login_;
            string password_;
            bool isWaiting_;
            int paymentMethod_;
            vector<string> pinnedAddresses_;
            vector<string> addresses_;
            int currentOrderID_ = -1;
            Order* order_;

            if (getline(in, line))
                name_ = move(line);
            if (getline(in, line))
                rating_ = stof(line);
            if (getline(in, line)){
                vector<string> orderIDs_;
                split(line, orderIDs_);
                for(const auto& el:orderIDs_){
                    order_ = getOrder(el);
                    if(order_!= nullptr)
                        orderHistory_.push_back(*order_);
                }
            }
            if (getline(in, line))
                login_ = move(line);
            if (getline(in, line))
                password_ = move(line);
            if(checkPassword && password_!=password){
                return nullptr;
            }
            if (getline(in, line))
                isWaiting_ = stoi(line);
            if (getline(in, line))
                paymentMethod_ = stoi(line);
            if (getline(in, line)){
                split(line, pinnedAddresses_);
                for(const string& el:pinnedAddresses_){
                    if(el.size()>1){
                        addresses_.push_back(el);
                    }
                }
            }
            if (getline(in, line))
                currentOrderID_ = stoi(line);

            auto* p= new Passenger(name_,login_,password_,rating_, orderHistory_,Passenger::CREDIT_CARD,addresses_);
            if(currentOrderID_!=-1){
                order_ = getOrder(to_string(currentOrderID_));
                if(order_!= nullptr)
                    p->setCurrentOrder(order_);
            }
            p->setPaymentMethods(paymentMethod_);
            p->setWaitingStatus(isWaiting_);

            in.close();
            return p;
        }else{
            // Passenger is not found
            return nullptr;
        }
    }

    // Returns all Passengers from DB
    vector<Passenger*> getAllPassengers(){
        readConfigs();
        vector<Passenger*> result;
        Passenger* p;
        for(const auto& el : passengersID){
            p = getPassenger(el,"",false);
            if(p!= nullptr)
                result.push_back(p);
        }
        return result;
    }


    // Drivers
    // ****************************************************************

    // Send Driver's data to DB
    void updateDriver(Driver* user){
        readConfigs();
        string line;
        ofstream out(dirName + "\\" + driversDir + "\\" + user->getLogin() + ".txt");

        if (out.is_open()){
            out << user->getName() << endl;
            out << to_string(user->getRating()) << endl;

            vector<Order> oh = user->getOrderHistory();
            string s;
            for(auto el: oh){
                s+=to_string(el.getID())+"*";
            }
            if(!s.empty())
                s.pop_back();
            out << s << endl;

            out << user->getLogin() << endl;
            out << user->getPassword() << endl;

            if(user->getCar()!= nullptr)
                out << user->getCar()->getNumber() << endl;
            else
                out << "-1" << endl;

            if(user->getWorkingStatus())
                out << "1" << endl;
            else
                out << "0" << endl;

            if(user->getRidingStatus())
                out << "1" << endl;
            else
                out << "0" << endl;
        }
        out.close();
    }

    // Create new Driver in DB
    void addNewDriver(Driver* user){
        updateDriver(user);
        driversID.push_back(user->getLogin());
        updateConfigs();
    }

    // Get Driver's data from DB
    Driver* getDriver(const string& login, const string& password, bool checkPassword = true){
        readConfigs();
        string line;
        ifstream in(dirName + "\\" + driversDir + "\\" + login + ".txt");

        if (in.is_open()){
            string name_;
            float rating_;
            vector<Order> orderHistory_;
            string login_;
            string password_;
            int carNumber_ = -1;
            bool isWorking_ = true;
            bool isInRide_ = false;
            Order* order_;

            if (getline(in, line))
                name_ = move(line);
            if (getline(in, line))
                rating_ = stof(line);
            if (getline(in, line)){
                vector<string> orderIDs_;
                split(line, orderIDs_);
                for(const auto& el:orderIDs_){
                    order_ = getOrder(el);
                    if(order_!= nullptr)
                        orderHistory_.push_back(*order_);
                }
            }
            if (getline(in, line))
                login_ = move(line);
            if (getline(in, line))
                password_ = move(line);
            if(checkPassword && password_!=password){
                return nullptr;
            }
            if (getline(in, line))
                carNumber_ = stoi(line);
            if (getline(in, line))
                isWorking_ = stoi(line);
            if (getline(in, line))
                isInRide_ = stoi(line);

            auto* d= new Driver(name_,login_,password_,rating_, orderHistory_, getCar(to_string(carNumber_)));
            d->setRidingStatus(isInRide_);
            d->setWorkingStatus(isWorking_);

            in.close();
            return d;
        }else{
            // Driver is not found
            return nullptr;
        }
    }

    // Returns all Drivers from DB
    vector<Driver*> getAllDrivers(){
        readConfigs();
        vector<Driver*> result;
        Driver* d;
        for(const auto& el : driversID){
            d = getDriver(el,"",false);
            if(d!= nullptr)
                result.push_back(d);
        }
        return result;
    }


    // Cars
    // ****************************************************************

    // Send Car's data to DB
    void updateCar(Car* car){
        readConfigs();
        string line;
        ofstream out(dirName + "\\" + carsDir + "\\" + to_string(car->getNumber()) + ".txt");

        if (out.is_open()){
            out << car->getModel() << endl;

            pair<float,float> coords_ = car->getCoordinates();
            out << to_string(coords_.first) << endl;
            out << to_string(coords_.second) << endl;

            out << car->getColour() << endl;
            out << to_string(car->getNumber()) << endl;
            out << car->getCarType() << endl;
            out << car->getNumberFreeBottleOfWater() << endl;
        }
        out.close();
    }

    // Create new Car in DB
    Car* addNewCar(Car* car){
        if(getCar(to_string(car->getNumber())) == nullptr){
            updateCar(car);
            carsID.push_back(to_string(car->getNumber()));
            updateConfigs();
            return car;
        }
        return getCar(to_string(car->getNumber()));
    }

    // Get Car's data from DB
    Car* getCar(const string& number) {
        readConfigs();
        string line;
        ifstream in(dirName + "\\" + carsDir + "\\" + number + ".txt");

        if (in.is_open() && number != "-1") {
            string model_;
            float x_ = 0;
            float y_ = 0;
            string colour_;
            int number_ = stoi(number);
            int carType_ = 0;
            int freeBottleOfWater_ = 0;

            if (getline(in, line))
                model_ = move(line);
            if (getline(in, line))
                x_ = stof(line);
            if (getline(in, line))
                y_ = stof(line);
            if (getline(in, line))
                colour_ = move(line);
            if (getline(in, line))
                number_ = stoi(line);
            if (getline(in, line))
                carType_ = stoi(line);
            if (getline(in, line))
                freeBottleOfWater_ = stoi(line);

            auto *car = new Car(model_, colour_, number_, Car::ECONOMY, x_, y_);
            car->setCarType(carType_);
            car->setNumberFreeBottleWater(freeBottleOfWater_);

            in.close();
            return car;
        } else {
            // Car is not found
            return nullptr;
        }
    }

    // Returns all Cars from DB
    vector<Car*> getAllCars(){
        readConfigs();
        vector<Car*> result;
        Car* c;
        for(const auto& el : carsID){
            c = getCar(el);
            if(c!= nullptr)
                result.push_back(c);
        }
        return result;
    }

    // Returns number of Cars in DB
    int getCarNumber() {
        readConfigs();
        return getAllCars().size();
    }


    // Orders
    // ****************************************************************

    // Send Order's data to DB
    void updateOrder(Order* order){
        readConfigs();
        string line;
        ofstream out(dirName + "\\" + ordersDir + "\\" + to_string(order->getID()) + ".txt");

        if (out.is_open()){
            out << order->getStartTime() << endl;
            out << order->getFinishTime() << endl;
            out << order->getStartAddress() << endl;
            out << order->getFinishAddress() << endl;
            out << order->getID() << endl;
            out << order->getCustomerID() << endl;
            out << order->getDriverID() << endl;

            if(order->getPickedStatus())
                out << "1" << endl;
            else
                out << "0" << endl;

            out << order->getCarType() << endl;
            out << order->getDescription() << endl;
        }
        out.close();
    }

    // Create new Order in DB
    void addNewOrder(Order* order){
        updateOrder(order);
        orderID.push_back(to_string(order->getID()));
        updateConfigs();
    }

    // Get Order's data from DB
    Order* getOrder(const string& id){
        readConfigs();
        string line;
        ifstream in(dirName + "\\" + ordersDir + "\\" + id + ".txt");

        if (in.is_open() && id!="-1") {
            int startTime_ = 0;
            int finishTime_ = 0;
            string startAddress_;
            string finishAddress_;
            int id_ = stoi(id);
            string customerID_ = "-1";
            string driverID_ = "-1";
            bool isPicked_ = false;
            int carType_ = 0;
            string description_;

            if (getline(in, line))
                startTime_ = stoi(line);
            if (getline(in, line))
                finishTime_ = stoi(line);
            if (getline(in, line))
                startAddress_ = move(line);
            if (getline(in, line))
                finishAddress_ = move(line);
            if (getline(in, line))
                id_ = stoi(line);
            if (getline(in, line))
                customerID_ = line;
            if (getline(in, line))
                driverID_ = line;
            if (getline(in, line))
                isPicked_ = stoi(line);
            if (getline(in, line))
                carType_ = stoi(line);
            if (getline(in, line))
                description_ = move(line);

            auto* order = new Order(startTime_,startAddress_,finishAddress_,Car::ECONOMY,customerID_,id_);
            order->setDescription(description_);
            order->setFinishTIme(finishTime_);
            order->setPickedStatus(isPicked_);
            order->setDriverID(driverID_);
            order->setCarType(carType_);

            in.close();
            return order;
        } else{
            // Order is not found
            return nullptr;
        }
    }

    // Returns all Orders from DB
    vector<Order*> getAllOrders(){
        readConfigs();
        vector<Order*> result;
        Order* order;
        for(const auto& el : orderID){
            order = getOrder(el);
            if(order!= nullptr)
                result.push_back(order);
        }
        return result;
    }

    // Returns number of Orders in DB
    int getOrdersNumber(){
        readConfigs();
        return getAllOrders().size();
    }

    // Returns number of non-picked Orders in DB (and which are not declined)
    vector<Order> getAvailableOrders(vector<Order> declined){
        readConfigs();
        vector<Order> result;
        Order* order;
        for(const auto& el : orderID){
            order = getOrder(el);
            if(order!= nullptr && !order->getPickedStatus() && !find_Order(declined, order))
                result.push_back(*order);
        }
        return result;
    }
};