#include <iostream>
#include <string>
using namespace std;

// EACH SONG HAS MULTIPLE PROPERTIES IN ITSELF SO WE DEFINE A DATA TYPE FOR IT (STRUCT)
struct song {
    int SongID;
    string Name;
    int minutes;
    int seconds;
};

// now we define the basic node structure where data is the song and 2 pointers
// prev points to node before and next points to node after
struct Node {
    song data;
    Node* prev;
    Node* next;

    Node(song s) {
        data = s;
        prev = nullptr;
        next = nullptr;
    }
};

class playlist {
private:
    Node* head;     // first node
    Node* tail;     // last node
    Node* current;  // song that is playing right now

    // loop on temp itself so the last node gets checked too
    Node* find_id(int id) {
        Node* temp = head;
        while (temp != nullptr) {
            if (temp->data.SongID == id) {
                return temp;    // return pointer to that song node
            }
            temp = temp->next;
        }
        return nullptr;         // reached the end without finding it
    }

    // prints one song in a readable way, seconds get a leading 0 if below 10
    void print_song(Node* n) {
        cout << "[" << n->data.SongID << "] " << n->data.Name << " - " << n->data.minutes << ":";
        if (n->data.seconds < 10) {
            cout << "0";
        }
        cout << n->data.seconds << endl;
    }

public:
    // start with an empty playlist, all 3 pointers point to nothing
    playlist() {
        head = nullptr;
        tail = nullptr;
        current = nullptr;
    }

    // when playlist is destroyed we delete every node so there is no memory leak
    ~playlist() {   // destructor
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // function to add a song at the end of the playlist
    void add_song(song s) {
        if (find_id(s.SongID) != nullptr) {
            cout << "A song with this ID already exists.\n";
            return;
        }
        Node* newNode = new Node(s);
        if (head == nullptr) {
            head = newNode;         // list was empty so new node is both first and last
        } else {
            tail->next = newNode;   // old last node points forward to the new one
            newNode->prev = tail;   // new node points back to the old last node
        }
        tail = newNode;             // new node is always the last node now
        cout << "Song added.\n";
    }

    // function to delete a song, returns true if deleted and false if id not found
    bool delete_song(int id) {
        Node* temp = head;
        if (head == nullptr) {
            return false;           // empty list nothing to delete
        }

        // case 1: the song is the first node so we shift the head to node 2 and delete old head
        if (head->data.SongID == id) {
            if (current == temp) {
                current = temp->next;   // move the player off the deleted song
            }
            head = head->next;
            if (head != nullptr) {
                head->prev = nullptr;   // new first node has nothing before it
            } else {
                tail = nullptr;         // it was the only node so list is empty now
            }
            delete temp;
            return true;                // stop here so we dont touch deleted memory
        }

        // case 2: look one node ahead so temp->next is always a real node
        while (temp->next != nullptr) {
            if (temp->next->data.SongID == id) {
                Node* del = temp->next;
                if (current == del) {
                    // move player to the next song, or the previous one if this was the last
                    current = (del->next != nullptr) ? del->next : temp;
                }
                temp->next = del->next;         // skip over the node we are deleting
                if (del->next != nullptr) {
                    del->next->prev = temp;     // node after also skips back over it
                } else {
                    tail = temp;                // we deleted the last node so tail moves back
                }
                delete del;
                return true;
            }
            temp = temp->next;
        }
        return false;               // reached the end, id not found
    }

    // go from first to last using next pointers
    void display_forward() {
        if (head == nullptr) {
            cout << "Playlist iss empty.\n";
            return;
        }
        for (Node* temp = head; temp != nullptr; temp = temp->next) {
            print_song(temp);
        }
    }

    // go from last to first using prev pointers
    void display_backward() {
        if (tail == nullptr) {
            cout << "Playlist is empty.\n";
            return;
        }
        for (Node* temp = tail; temp != nullptr; temp = temp->prev) {
            print_song(temp);
        }
    }

    // search by id and show details of that song
    void search_song(int id) {
        Node* found = find_id(id);
        if (found == nullptr) {
            cout << "Song not found.\n";
        } else {
            print_song(found);
        }
    }

    // move player forward, if nothing is playing yet we start from the first song
    void play_next() {
        if (head == nullptr) {
            cout << "Playlist is empty.\n";
            return;
        }
        if (current == nullptr) {
            current = head;
        } else if (current->next != nullptr) {
                current = current->next;
            } else {
                cout << "Already at the last song.\n";
                return;
            }
            cout << "Now playing: ";
        print_song(current);
    }

    // move player backward, if nothing is playing yet we start from the last song
    void play_previous() {
        if (head == nullptr) {
            cout << "Playlist is empty.\n";
            return;
        }
        if (current == nullptr) {
            current = tail;
        } else if (current->prev != nullptr) {
            current = current->prev;
        } else {
            cout << "Already at the first song.\n";
            return;
        }
        cout << "Now playing: ";
        print_song(current);
    }

    // reverse in place, we swap next and prev of every node and then swap head and tail
    void reverse_playlist() {
        Node* temp = head;
        while (temp != nullptr) {
            Node* saved = temp->next;   // save next before we overwrite it
            temp->next = temp->prev;
            temp->prev = saved;
            temp = temp->prev;          // old next is now stored in prev so we move using prev
        }
        Node* swap = head;
        head = tail;
        tail = swap;
        cout << "Playlist reversed.\n";
    }
};

int main() {
    playlist list;
    int choice;

    do {
        cout << "\nxxxxxxx Playlist Menu xxxxxxxx\n"
             << "1. Add song\n2. Delete song\n3. Display forward\n4. Display backward\n"
             << "5. Search song\n6. Play next\n7. Play previous\n8. Reverse playlist\n0. Exit\n"
             << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            song s;
            char colon;
            cout << "Song ID: ";
            cin >> s.SongID;
            cin.ignore();               // clear the newline so getline works
            cout << "Song name: ";
            getline(cin, s.Name);
            cout << "Duration (mm:ss): ";
            cin >> s.minutes >> colon >> s.seconds;
            list.add_song(s);
        } else if (choice == 2) {
            int id;
            cout << "ID to delete: ";
            cin >> id;
            if (list.delete_song(id)) {
                cout << "Song deleted.\n";
            } else {
                cout << "Song not found.\n";
            }
        } else if (choice == 3) {
            list.display_forward();
        } else if (choice == 4) {
            list.display_backward();
        } else if (choice == 5) {
            int id;
            cout << "ID to search: ";
            cin >> id;
            list.search_song(id);
        } else if (choice == 6) {
            list.play_next();
        } else if (choice == 7) {
            list.play_previous();
        } else if (choice == 8) {
            list.reverse_playlist();
        }
    } while (choice != 0);

    return 0;
}