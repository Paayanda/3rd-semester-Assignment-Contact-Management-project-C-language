#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

// Structure to store phone numbers
struct Phone {
    string type;
    string number;
};

// Structure to store email addresses
struct Email {
    string type;
    string address;
};

// Contact Class
class Contact {
private:
    int contact_id;
    string first_name;
    string last_name;
    string category;
    vector<Phone> phones;
    vector<Email> emails;

public:
    // Constructor
    Contact(int id, string fname, string lname, string cat) {
        contact_id = id;
        first_name = fname;
        last_name = lname;
        category = cat;
    }

    // Getters
    int getID() const { return contact_id; }
    string getFirstName() const { return first_name; }
    string getLastName() const { return last_name; }
    string getCategory() const { return category; }
    vector<Phone> getPhones() const { return phones; }
    vector<Email> getEmails() const { return emails; }

    // Add phone number
    void addPhone(string type, string number) {
        Phone p;
        p.type = type;
        p.number = number;
        phones.push_back(p);
    }

    // Add email address
    void addEmail(string type, string address) {
        Email e;
        e.type = type;
        e.address = address;
        emails.push_back(e);
    }

    // Display contact information
    void display() const {
        cout << "\n=== Contact Information ===" << endl;
        cout << "ID: " << contact_id << endl;
        cout << "Name: " << first_name << " " << last_name << endl;
        cout << "Category: " << category << endl;
        
        cout << "Phone Numbers:" << endl;
        for (const auto& phone : phones) {
            cout << "  " << phone.type << ": " << phone.number << endl;
        }
        
        cout << "Email Addresses:" << endl;
        for (const auto& email : emails) {
            cout << "  " << email.type << ": " << email.address << endl;
        }
        cout << "===========================" << endl;
    }

    // Display summary (for list view)
    void displaySummary() const {
        cout << "ID: " << contact_id << " | Name: " << first_name << " " << last_name 
             << " | Category: " << category << endl;
    }

    // Convert contact to string for file storage
    string toString() const {
        stringstream ss;
        ss << contact_id << "|" << first_name << "|" << last_name << "|" << category;
        
        // Add phones
        ss << "|PHONES:";
        for (const auto& phone : phones) {
            ss << phone.type << "," << phone.number << ";";
        }
        
        // Add emails
        ss << "|EMAILS:";
        for (const auto& email : emails) {
            ss << email.type << "," << email.address << ";";
        }
        
        return ss.str();
    }

    // Update contact details
    void updateContact(string fname, string lname, string cat) {
        first_name = fname;
        last_name = lname;
        category = cat;
    }

    // Clear phones and emails
    void clearPhones() { phones.clear(); }
    void clearEmails() { emails.clear(); }
};

// Contact Management System Class
class ContactManager {
private:
    vector<Contact> contacts;
    int next_id;

public:
    ContactManager() : next_id(1) {}
    
    // Add new contact
    void addContact(Contact contact) {
        contacts.push_back(contact);
    }

    // Find contact by ID
    Contact* findContactByID(int id) {
        for (auto& contact : contacts) {
            if (contact.getID() == id) {
                return &contact;
            }
        }
        return nullptr;
    }

    // Find contacts by last name
    vector<Contact*> findContactsByLastName(string lname) {
        vector<Contact*> result;
        for (auto& contact : contacts) {
            if (contact.getLastName() == lname) {
                result.push_back(&contact);
            }
        }
        return result;
    }

    // Delete contact by ID
    bool deleteContact(int id) {
        auto it = remove_if(contacts.begin(), contacts.end(),
            [id](const Contact& c) { return c.getID() == id; });
        
        if (it != contacts.end()) {
            contacts.erase(it, contacts.end());
            return true;
        }
        return false;
    }

    // Display all contacts
    void displayAllContacts() {
        if (contacts.empty()) {
            cout << "No contacts found." << endl;
            return;
        }
        
        cout << "\n=== All Contacts ===" << endl;
        for (const auto& contact : contacts) {
            contact.displaySummary();
        }
    }

    // Get next available ID
    int getNextID() {
        return next_id++;
    }

    // Set next ID (for loading from file)
    void setNextID(int id) {
        next_id = id;
    }

    // Get all contacts
    vector<Contact>& getContacts() {
        return contacts;
    }

    // Load contacts from file
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "No existing data file found. Starting fresh." << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string token;
            vector<string> tokens;
            
            // Parse the line
            while (getline(ss, token, '|')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 4) {
                int id = stoi(tokens[0]);
                Contact contact(id, tokens[1], tokens[2], tokens[3]);
                
                // Parse phones
                if (tokens.size() > 4 && tokens[4].find("PHONES:") != string::npos) {
                    string phones_str = tokens[4].substr(7);
                    stringstream phones_ss(phones_str);
                    string phone_token;
                    
                    while (getline(phones_ss, phone_token, ';')) {
                        if (!phone_token.empty()) {
                            size_t pos = phone_token.find(',');
                            if (pos != string::npos) {
                                string type = phone_token.substr(0, pos);
                                string number = phone_token.substr(pos + 1);
                                contact.addPhone(type, number);
                            }
                        }
                    }
                }
                
                // Parse emails
                if (tokens.size() > 5 && tokens[5].find("EMAILS:") != string::npos) {
                    string emails_str = tokens[5].substr(7);
                    stringstream emails_ss(emails_str);
                    string email_token;
                    
                    while (getline(emails_ss, email_token, ';')) {
                        if (!email_token.empty()) {
                            size_t pos = email_token.find(',');
                            if (pos != string::npos) {
                                string type = email_token.substr(0, pos);
                                string address = email_token.substr(pos + 1);
                                contact.addEmail(type, address);
                            }
                        }
                    }
                }
                
                contacts.push_back(contact);
                next_id = max(next_id, id + 1);
            }
        }
        file.close();
        cout << "Data loaded successfully. Total contacts: " << contacts.size() << endl;
    }

    // Save contacts to file
    void saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not save data to file." << endl;
            return;
        }

        for (const auto& contact : contacts) {
            file << contact.toString() << endl;
        }
        file.close();
        cout << "Data saved successfully. Total contacts: " << contacts.size() << endl;
    }
};

// Utility functions
void displayMenu() {
    cout << "\n=== Contact Management System ===" << endl;
    cout << "1. Add New Contact" << endl;
    cout << "2. Search Contact" << endl;
    cout << "3. Delete Contact" << endl;
    cout << "4. Update Contact" << endl;
    cout << "5. Display All Contacts" << endl;
    cout << "6. Exit" << endl;
    cout << "Enter your choice (1-6): ";
}

void addNewContact(ContactManager& manager) {
    string first_name, last_name, category;
    
    cout << "\n=== Add New Contact ===" << endl;
    cout << "Enter First Name: ";
    cin >> first_name;
    cout << "Enter Last Name: ";
    cin >> last_name;
    cout << "Enter Category (Family/Friend/Work): ";
    cin >> category;
    
    int id = manager.getNextID();
    Contact new_contact(id, first_name, last_name, category);
    
    // Add phone numbers
    char add_phone = 'y';
    while (add_phone == 'y' || add_phone == 'Y') {
        string phone_type, phone_number;
        cout << "Add phone number? (y/n): ";
        cin >> add_phone;
        if (add_phone == 'y' || add_phone == 'Y') {
            cout << "Phone Type (Mobile/Home/Work): ";
            cin >> phone_type;
            cout << "Phone Number: ";
            cin >> phone_number;
            new_contact.addPhone(phone_type, phone_number);
        }
    }
    
    // Add email addresses
    char add_email = 'y';
    while (add_email == 'y' || add_email == 'Y') {
        string email_type, email_address;
        cout << "Add email address? (y/n): ";
        cin >> add_email;
        if (add_email == 'y' || add_email == 'Y') {
            cout << "Email Type (Personal/Work): ";
            cin >> email_type;
            cout << "Email Address: ";
            cin >> email_address;
            new_contact.addEmail(email_type, email_address);
        }
    }
    
    manager.addContact(new_contact);
    cout << "Contact added successfully with ID: " << id << endl;
}

void searchContact(ContactManager& manager) {
    int choice;
    cout << "\n=== Search Contact ===" << endl;
    cout << "1. Search by ID" << endl;
    cout << "2. Search by Last Name" << endl;
    cout << "Enter your choice: ";
    cin >> choice;
    
    if (choice == 1) {
        int id;
        cout << "Enter Contact ID: ";
        cin >> id;
        Contact* contact = manager.findContactByID(id);
        if (contact) {
            contact->display();
        } else {
            cout << "Contact not found." << endl;
        }
    } else if (choice == 2) {
        string last_name;
        cout << "Enter Last Name: ";
        cin >> last_name;
        vector<Contact*> contacts = manager.findContactsByLastName(last_name);
        if (!contacts.empty()) {
            cout << "Found " << contacts.size() << " contact(s):" << endl;
            for (const auto& contact : contacts) {
                contact->display();
            }
        } else {
            cout << "No contacts found with last name: " << last_name << endl;
        }
    } else {
        cout << "Invalid choice." << endl;
    }
}

void deleteContact(ContactManager& manager) {
    int id;
    cout << "\n=== Delete Contact ===" << endl;
    cout << "Enter Contact ID to delete: ";
    cin >> id;
    
    if (manager.deleteContact(id)) {
        cout << "Contact deleted successfully." << endl;
    } else {
        cout << "Contact not found." << endl;
    }
}

void updateContact(ContactManager& manager) {
    int id;
    cout << "\n=== Update Contact ===" << endl;
    cout << "Enter Contact ID to update: ";
    cin >> id;
    
    Contact* contact = manager.findContactByID(id);
    if (!contact) {
        cout << "Contact not found." << endl;
        return;
    }
    
    string first_name, last_name, category;
    cout << "Enter new First Name (" << contact->getFirstName() << "): ";
    cin >> first_name;
    cout << "Enter new Last Name (" << contact->getLastName() << "): ";
    cin >> last_name;
    cout << "Enter new Category (" << contact->getCategory() << "): ";
    cin >> category;
    
    contact->updateContact(first_name, last_name, category);
    
    // Update phones
    char update_phones;
    cout << "Update phone numbers? (y/n): ";
    cin >> update_phones;
    if (update_phones == 'y' || update_phones == 'Y') {
        contact->clearPhones();
        char add_phone = 'y';
        while (add_phone == 'y' || add_phone == 'Y') {
            string phone_type, phone_number;
            cout << "Add phone number? (y/n): ";
            cin >> add_phone;
            if (add_phone == 'y' || add_phone == 'Y') {
                cout << "Phone Type (Mobile/Home/Work): ";
                cin >> phone_type;
                cout << "Phone Number: ";
                cin >> phone_number;
                contact->addPhone(phone_type, phone_number);
            }
        }
    }
    
    // Update emails
    char update_emails;
    cout << "Update email addresses? (y/n): ";
    cin >> update_emails;
    if (update_emails == 'y' || update_emails == 'Y') {
        contact->clearEmails();
        char add_email = 'y';
        while (add_email == 'y' || add_email == 'Y') {
            string email_type, email_address;
            cout << "Add email address? (y/n): ";
            cin >> add_email;
            if (add_email == 'y' || add_email == 'Y') {
                cout << "Email Type (Personal/Work): ";
                cin >> email_type;
                cout << "Email Address: ";
                cin >> email_address;
                contact->addEmail(email_type, email_address);
            }
        }
    }
    
    cout << "Contact updated successfully." << endl;
}

// Main function
int main() {
    ContactManager manager;
    
    // Load existing data
    manager.loadFromFile("contacts.dat");
    
    int choice;
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                addNewContact(manager);
                break;
            case 2:
                searchContact(manager);
                break;
            case 3:
                deleteContact(manager);
                break;
            case 4:
                updateContact(manager);
                break;
            case 5:
                manager.displayAllContacts();
                break;
            case 6:
                manager.saveToFile("contacts.dat");
                cout << "Thank you for using Contact Management System. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 6);
    
    return 0;
}