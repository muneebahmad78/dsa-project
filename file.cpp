#include <iostream>
#include <fstream>
#include <string>
using namespace std;
class FoodItem {
private:
    int id;
    string name;
    float price;
    FoodItem* next;

public:
    FoodItem(int foodId, string foodName, float foodPrice)
        : id(foodId), name(foodName), price(foodPrice), next(nullptr) {}

    int getId() const { return id; }
    string getName() const { return name; }
    float getPrice() const { return price; }
    FoodItem* getNext() const { return next; }
    void setNext(FoodItem* nextItem) { next = nextItem; }
};

class Order {
private:
    int orderId;
    string customerName;
    int foodId;
    int quantity;
    Order* next;

public:
    Order(int id, string customer, int food, int qty)
        : orderId(id), customerName(customer), foodId(food), quantity(qty), next(nullptr) {}

    int getOrderId() const { return orderId; }
    string getCustomerName() const { return customerName; }
    int getFoodId() const { return foodId; }
    int getQuantity() const { return quantity; }
    Order* getNext() const { return next; }
    void setNext(Order* nextOrder) { next = nextOrder; }
};

class FoodDeliveryApp {
private:
    FoodItem* menuHead;
    Order* ordersHead;
    int nextOrderId;

public:
    FoodDeliveryApp() : menuHead(nullptr), ordersHead(nullptr), nextOrderId(1) {}

    void addFoodItem(int id, const string& name, float price) {
        FoodItem* newFood = new FoodItem(id, name, price);
        if (!menuHead) {
            menuHead = newFood;
        } else {
            FoodItem* temp = menuHead;
            while (temp->getNext()) {
                temp = temp->getNext();
            }
            temp->setNext(newFood);
        }
    }

    void displayMenu() {
        if (!menuHead) {
            cout << "Menu is empty!" << endl;
            return;
        }
        cout << "Menu:\n";
        FoodItem* temp = menuHead;
        while (temp) {
            cout << "ID: " << temp->getId() << ", Name: " << temp->getName()
                 << ", Price: Rs" << temp->getPrice() << endl;
            temp = temp->getNext();
        }
    }

    void placeOrder(const string& customerName, int foodId, int quantity) {
        FoodItem* food = findFoodById(foodId);
        if (!food) {
            cout << "Food item with ID " << foodId << " not found." << endl;
            return;
        }

        Order* newOrder = new Order(nextOrderId++, customerName, foodId, quantity);
        if (!ordersHead) {
            ordersHead = newOrder;
        } else {
            Order* temp = ordersHead;
            while (temp->getNext()) {
                temp = temp->getNext();
            }
            temp->setNext(newOrder);
        }
        cout << "Order placed successfully! Order ID: " << newOrder->getOrderId() << endl;
    }

    void displayOrders() {
        if (!ordersHead) {
            cout << "No active orders!" << endl;
            return;
        }
        cout << "Active Orders:\n";
        Order* temp = ordersHead;
        while (temp) {
            cout << "Order ID: " << temp->getOrderId() << ", Customer: " << temp->getCustomerName()
                 << ", Food ID: " << temp->getFoodId() << ", Quantity: " << temp->getQuantity() << endl;
            temp = temp->getNext();
        }
    }

    void saveData() {
        ofstream menuFile("menu.dat", ios::binary);
        FoodItem* tempMenu = menuHead;
        while (tempMenu) {
            int id = tempMenu->getId();
            size_t nameLen = tempMenu->getName().size();
            float price = tempMenu->getPrice();

            menuFile.write(reinterpret_cast<char*>(&id), sizeof(id));
            menuFile.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            menuFile.write(tempMenu->getName().c_str(), nameLen);
            menuFile.write(reinterpret_cast<char*>(&price), sizeof(price));

            tempMenu = tempMenu->getNext();
        }
        menuFile.close();

        ofstream ordersFile("orders.dat", ios::binary);
        Order* tempOrder = ordersHead;
        while (tempOrder) {
            int orderId = tempOrder->getOrderId();
            size_t nameLen = tempOrder->getCustomerName().size();
            int foodId = tempOrder->getFoodId();
            int quantity = tempOrder->getQuantity();

            ordersFile.write(reinterpret_cast<char*>(&orderId), sizeof(orderId));
            ordersFile.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            ordersFile.write(tempOrder->getCustomerName().c_str(), nameLen);
            ordersFile.write(reinterpret_cast<char*>(&foodId), sizeof(foodId));
            ordersFile.write(reinterpret_cast<char*>(&quantity), sizeof(quantity));

            tempOrder = tempOrder->getNext();
        }
        ordersFile.close();

        cout << "Data saved successfully!" << endl;
    }

    void loadData() {
        ifstream menuFile("menu.dat", ios::binary);
        if (menuFile) {
            while (menuFile.peek() != EOF) {
                int id;
                size_t nameLen;
                string name;
                float price;

                menuFile.read(reinterpret_cast<char*>(&id), sizeof(id));
                menuFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
                name.resize(nameLen);
                menuFile.read(&name[0], nameLen);
                menuFile.read(reinterpret_cast<char*>(&price), sizeof(price));

                addFoodItem(id, name, price);
            }
            menuFile.close();
        }

        ifstream ordersFile("orders.dat", ios::binary);
        if (ordersFile) {
            while (ordersFile.peek() != EOF) {
                int orderId, foodId, quantity;
                size_t nameLen;
                string customerName;

                ordersFile.read(reinterpret_cast<char*>(&orderId), sizeof(orderId));
                ordersFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
                customerName.resize(nameLen);
                ordersFile.read(&customerName[0], nameLen);
                ordersFile.read(reinterpret_cast<char*>(&foodId), sizeof(foodId));
                ordersFile.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));

                placeOrder(customerName, foodId, quantity);
            }
            ordersFile.close();
        }

        cout << "Data loaded successfully!" << endl;
    }

    ~FoodDeliveryApp() {
        while (menuHead) {
            FoodItem* temp = menuHead;
            menuHead = menuHead->getNext();
            delete temp;
        }
        while (ordersHead) {
            Order* temp = ordersHead;
            ordersHead = ordersHead->getNext();
            delete temp;
        }
    }

private:
    FoodItem* findFoodById(int id) {
        FoodItem* temp = menuHead;
        while (temp) {
            if (temp->getId() == id) return temp;
            temp = temp->getNext();
        }
        return nullptr;
    }
};

int main() {
    FoodDeliveryApp app;
    app.loadData();

    int choice;
    do {
        cout << "\n1. Add Food Item\n2. Display Menu\n3. Place Order\n4. Display Orders\n5. Save Data\n6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            int id;
            string name;
            float price;
            cout << "Enter Food ID: ";
            cin >> id;
            cout << "Enter Food Name: ";
            cin.ignore(); 
            getline(cin, name);
            cout << "Enter Price: ";
            cin >> price;
            app.addFoodItem(id, name, price);
            break;
        }
        case 2:
            app.displayMenu();
            break;
        case 3: {
            string customerName;
            int foodId, quantity;
            cout << "Enter Customer Name: ";
            cin.ignore(); 
            getline(cin, customerName);
            cout << "Enter Food ID: ";
            cin >> foodId;
            cout << "Enter Quantity: ";
            cin >> quantity;
            app.placeOrder(customerName, foodId, quantity);
            break;
        }
        case 4:
            app.displayOrders();
            break;
        case 5:
            app.saveData();
            break;
        case 6:
            cout << "Exiting application..." << endl;
            break;
        default:
            cout << "Invalid choice, please try again!" << endl;
        }
    } while (choice != 6);

    return 0;
}
