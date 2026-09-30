#include <iostream>
#include <vector>
using namespace std;

// each person in the circle only needs an id and a pointer to the next person(circle)
struct Node {
    int id;
    Node* next;

    Node(int i) {
        id = i;
        next = nullptr;
    }
};

class circle {
private:
    Node* head;             // person 1, where the counting starts
    int size;               // how many people are still standing
    vector<int> eliminated; // ids in the order they were eliminated

public:
    // start with an empty circle
    circle() {
        head = nullptr;
        size = 0;
    }

    // if anyone is still left when the program ends we delete them so there is no memory leak
    ~circle() {
        while (size > 0) {
            Node* temp = head;
            head = head->next;
            delete temp;
            size--;
        }
    }

    // function to build the circle 1 -> 2 -> ... -> n -> back to 1
    void create_circle(int n) {
        head = new Node(1);
        Node* last = head;
        for (int i = 2; i <= n; i++) {
            last->next = new Node(i);   // attach the new person after the last one
            last = last->next;
        }
        last->next = head;              // last person points back to the first so its a circle
        size = n;
    }

    // function to eliminate every k-th person until only one is left
    void eliminate(int k) {
        // before always stays one node behind current so we can unlink current
        Node* before = head;
        while (before->next != head) {
            before = before->next;
        }
        Node* current = head;

        while (size > 1) {
            // to count k people starting from current we only need to move k-1 steps
            for (int i = 1; i < k; i++) {
                before = current;
                current = current->next;
            }
            eliminated.push_back(current->id);
            before->next = current->next;   // skip over the eliminated person
            delete current;
            size--;
            current = before->next;         // counting starts again from the next person
        }
        head = current;                     // the survivor is now the only node left
    }

    // show the order in which people were eliminated
    void display_eliminated() {
        cout << "Eliminated order: ";
        for (int i = 0; i < (int)eliminated.size(); i++) {
            cout << eliminated[i] << " ";
        }
        cout << endl;
    }

    // show the last person standing
    void display_survivor() {
        cout << "Survivor: " << head->id << endl;
    }
};

int main() {
    int n, k;
    cout << "Number of people (N): ";
    cin >> n;
    cout << "Step count (k): ";
    cin >> k;

    if (n < 1 || k < 1) {
        cout << "N and k must be at least 1.\n";
        return 0;
    }

    circle josephus;
    josephus.create_circle(n);
    josephus.eliminate(k);
    josephus.display_eliminated();
    josephus.display_survivor();

    return 0;
}