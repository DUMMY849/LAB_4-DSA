#include <iostream>
#include <string>
using namespace std;

// each bit gets its own node, the left bit is the biggest and the right bit is the smallest
struct Node {
    int bit;
    Node* prev;
    Node* next;

    Node(int b) {
        bit = b;
        prev = nullptr;
        next = nullptr;
    }
};

class binary {
private:
    Node* head;     // first bit (most significant)
    Node* tail;     // last bit (least significant)
    int size;       // how many bits are stored

    // copies all bits of another number into this one
    void copy_from(const binary& other) {
        for (Node* temp = other.head; temp != nullptr; temp = temp->next) {
            add_at_end(temp->bit);
        }
    }

    // deletes every node so there is no memory leak
    void clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        size = 0;
    }

public:
    // start with an empty number
    binary() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    // copy constructor, needed because we return binary numbers from functions
    binary(const binary& other) {
        head = nullptr;
        tail = nullptr;
        size = 0;
        copy_from(other);
    }

    // assignment does the same thing but first throws away the old bits
    binary& operator=(const binary& other) {
        if (this != &other) {
            clear();
            copy_from(other);
        }
        return *this;
    }

    ~binary() {
        clear();
    }

    // add a bit on the right side
    void add_at_end(int b) {
        Node* newNode = new Node(b);
        if (tail == nullptr) {
            head = newNode;         // list was empty
        } else {
            tail->next = newNode;
            newNode->prev = tail;
        }
        tail = newNode;
        size++;
    }

    // add a bit on the left side
    void add_at_front(int b) {
        Node* newNode = new Node(b);
        if (head == nullptr) {
            tail = newNode;         // list was empty
        } else {
            newNode->next = head;
            head->prev = newNode;
        }
        head = newNode;
        size++;
    }

    // add zeros on the left until we have full 8 bit blocks
    void pad_to_bytes() {
        while (size % 8 != 0 || size == 0) {
            add_at_front(0);
        }
    }

    // store a string like "1011" in the list, returns false if it has anything except 0 and 1
    bool store_number(string bits) {
        clear();
        if (bits.empty()) {
            return false;
        }
        for (int i = 0; i < (int)bits.length(); i++) {
            if (bits[i] != '0' && bits[i] != '1') {
                clear();
                return false;
            }
            add_at_end(bits[i] - '0');  // turn the character into a number 0 or 1
        }
        pad_to_bytes();                 // 1011 becomes 00001011
        return true;
    }

    // print the bits with a space after every 8 bit block
    void display() {
        int count = 0;
        for (Node* temp = head; temp != nullptr; temp = temp->next) {
            cout << temp->bit;
            count++;
            if (count % 8 == 0 && temp->next != nullptr) {
                cout << " ";
            }
        }
        cout << endl;
    }

    // 1's complement- we go through every node and flip the bit
    binary ones_complement() {
        binary result = *this;
        for (Node* temp = result.head; temp != nullptr; temp = temp->next) {
            temp->bit = 1 - temp->bit;
        }
        return result;
    }

    // 2's complement- flip all bits then add 1 starting from the right
    binary twos_complement() {
        binary result = ones_complement();
        int carry = 1;                  // the +1 we want to add
        Node* temp = result.tail;
        while (temp != nullptr && carry == 1) {
            int sum = temp->bit + carry;
            temp->bit = sum % 2;        // bit we keep in this node
            carry = sum / 2;            // bit we pass to the left
            temp = temp->prev;
        }
        return result;                  // carry out of the top bit is dropped
    }

    // binary addition, we start from the right of both numbers and pass the carry to the left
    binary add(binary& other) {
        binary result;
        Node* a = tail;
        Node* b = other.tail;
        int carry = 0;
        while (a != nullptr || b != nullptr) {
            int sum = carry;
            if (a != nullptr) {
                sum += a->bit;
                a = a->prev;
            }
            if (b != nullptr) {
                sum += b->bit;
                b = b->prev;
            }
            result.add_at_front(sum % 2);
            carry = sum / 2;
        }
        if (carry == 1) {
            result.add_at_front(1);     // leftover carry needs one more bit
        }
        result.pad_to_bytes();          // if it grew past 8 bits we get a new block
        return result;
    }

    // multiplication using shifting and repeated addition
    binary multiply(binary& other) {
        binary result;
        result.store_number("0");
        binary shifted = *this;         // multiplicand, it moves left every round
        // go through the multiplier from right to left
        for (Node* m = other.tail; m != nullptr; m = m->prev) {
            if (m->bit == 1) {
                result = result.add(shifted);   // bit is 1 so we add the shifted number
            }
            shifted.add_at_end(0);      // shifting left means adding a 0 on the right
        }
        return result;
    }

    // go from left to right, each step doubles the total and adds the next bit
    unsigned long long to_decimal() {
        unsigned long long total = 0;
        for (Node* temp = head; temp != nullptr; temp = temp->next) {
            total = total * 2 + temp->bit;
        }
        return total;
    }
};

// keeps asking until the user types only 0s and 1s
void read_number(string label, binary& num) {
    string bits;
    do {
        cout << "Enter " << label << " (only 0s and 1s): ";
        cin >> bits;
    } while (!num.store_number(bits));
    cout << "Stored " << label << ": ";
    num.display();
}

int main() {
    binary a, b;
    a.store_number("0");
    b.store_number("0");
    int choice;

    do {
        cout << "\nxxxxxx Binary Menu xxxxxxx\n"
             << "1. Store A\n2. Store B\n3. 1's complement of A\n4. 2's complement of A\n"
             << "5. A + B\n6. A * B\n7. Convert A and B to decimal\n0. Exit\n"
             << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            read_number("A", a);
        } else if (choice == 2) {
            read_number("B", b);
        } else if (choice == 3) {
            binary result = a.ones_complement();
            cout << "1's complement: ";
            result.display();
        } else if (choice == 4) {
            binary result = a.twos_complement();
            cout << "2's complement: ";
            result.display();
        } else if (choice == 5) {
            binary result = a.add(b);
            cout << "A + B = ";
            result.display();
            cout << "Decimal: " << result.to_decimal() << endl;
        } else if (choice == 6) {
            binary result = a.multiply(b);
            cout << "A * B = ";
            result.display();
            cout << "Decimal: " << result.to_decimal() << endl;
        } else if (choice == 7) {
            cout << "A = " << a.to_decimal() << ", B = " << b.to_decimal() << endl;
        }
    } while (choice != 0);

    return 0;
}