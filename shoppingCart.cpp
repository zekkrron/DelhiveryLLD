#include <bits/stdc++.h>
using namespace std;

/*
Requirements:
-sjfh vh fvo vofj 
-sfkvwh o wo jbwo
-ksfhb ojw oj wcifb

Data Model:

Shopping Cart
- shopiing cart id
- user
- vector<Items>

user
- user id
- name
- email

Products
- product id 
- title 
- stock 
- price

CostStrategy and various strategy children


Orders
- order id 
- vector<Items>


*/

class Product {
private:
    string product_id;
    string name;
    int stock;
    double price;

public:
    Product(string name_, int stock_, double price_) : name(name_), stock(stock_), price(price_) {
        product_id = name + to_string(rand());
    }

    string get_product_id() { return this->product_id; }
    string get_name() { return this->name; }
    double get_price() { return this->price; }
    int get_stock() { return this->stock; }

    void reduce_stock(int quantity) {
        if(quantity > this->stock) {
            throw runtime_error("Not Enough Stock!");
        }
        int original_stock = this->stock;
        this->stock = original_stock - quantity;
    }
};

class ShoppingCart {
private:
    map<Product*, int> items;
    
public:
    ShoppingCart() {}

    void add_product(Product* product, int quantity) {
        int original_quantity = 0; 
        if(items.find(product) != items.end()) {
            original_quantity = items[product];
        }
        items[product] = original_quantity + quantity;
        cout << "Product " << product->get_name() << " with quantity " << quantity << " added to the cart " <<endl;
    }
    
    void remove_product(Product* product) {
        if(items.find(product) != items.end()) {
            items.erase(product);
            cout << "Product with id : " << product->get_product_id() << " removed from the cart" << endl;
            return;
        }
        cout << "Product doesn't exist in the cart";
    }

    double getTotal() {
        // final cost caluculation
        double cost = 0;
        for(auto item : items) {
            cost += item.first->get_price() * item.second;
        }

        return cost;
    }

    map<Product*, int> get_items() {
        return items;
    }
};

// Forward declaration of User Class
class User;

class Order {
private:
    double final_cost;
    User* user;
    map<Product*, int> ordered_items;

public:
    Order(map<Product*, int> ordered_items_, User* user_, ShoppingCart& cart) : ordered_items(ordered_items_), user(user_) {
        final_cost = cart.getTotal();
        for(auto const& [product, quantity] : ordered_items) {
            product->reduce_stock(quantity);
        }
    }

    void display_order() {
        cout << "Order final cost : " << final_cost << endl;
        cout << "------------------ITEMS BOUGHT-------------" << endl;
        for(auto const& [product, quantity] : ordered_items) {
            cout << "Product : " << product->get_name() << " quantity : " << quantity << " price : " << product->get_price() << endl;
        }
    }

    void set_final_cost(double final_cost_) {
        this->final_cost = final_cost_;
    }
};

class User {
private:
    string user_id;
    string name;
    string email;
    ShoppingCart cart;

public:
    User(string name, string email) {
        this->user_id = name + email;
        this->name = name;
        this->email = email;
    }

    void add_product(Product* product, int quantity) {
        cart.add_product(product, quantity);
    }
    
    void remove_product(Product* product) {
        cart.remove_product(product);
    }
    
    Order checkout() {
        Order order(this->cart.get_items(), this, this->cart);
        order.set_final_cost(cart.getTotal());
        return order;
    }
};


int main() {    
    // creating the products for the catalogue
    vector<Product*> product_list;
    Product* watch = new Product("Watch", 20, 1200.0);
    Product* mouse = new Product("Mouse", 20, 1701.3);
    product_list.push_back(watch);
    product_list.push_back(mouse);

    User* user1 = new User("Akash", "akash.singh@delhivery.com");
    user1->add_product(watch, 3);
    user1->add_product(mouse, 5);

    Order user1_order = user1->checkout();
    user1_order.display_order();

    cout << "--------------STOCK AFTER ORDER------------" << endl;
    for(Product* product : product_list) {
        cout << "Product : " << product->get_name() << " quantity : " << product->get_stock() << endl;
    }

    return 0;
}
