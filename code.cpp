#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int MAX_DOCTORS = 10;
const int MAX_SLOTS = 10;
const int MAX_HISTORY = 10;
const int MAX_PREDEFINED_SLOTS = 10;

const string predefined_slots[MAX_PREDEFINED_SLOTS] = {
    "09:00 AM", "10:00 AM", "11:00 AM", "12:00 PM",
    "01:00 PM", "02:00 PM", "03:00 PM", "04:00 PM",
    "05:00 PM", "06:00 PM"
};

// Helper function to trim leading and trailing whitespace
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

// Base class for medical professionals
class MedicalProfessional {
public:
    virtual string getName() = 0;
    virtual string getSpecialization() = 0;
};

// Derived class for doctors
class Doctor : public MedicalProfessional {
protected:
    int doctor_id, slot_count = 0;
    string name, specialization, slots[MAX_SLOTS];

public:
    Doctor(int id = 0, string n = "", string spec = "") : doctor_id(id), name(n), specialization(spec) {}
    string getName() { return name; }
    string getSpecialization() { return specialization; }
    
    bool isValidSlot(const string& slot) {
    string trimmed_slot = trim(slot);
    for (int i = 0; i < MAX_PREDEFINED_SLOTS; ++i) {
        if (predefined_slots[i] == trimmed_slot) return true;
    }
    return false;
    }
    void addSlot(const string& slot) {
        if (isValidSlot(slot) && slot_count < MAX_SLOTS) {
            slots[slot_count++] = slot;
        } else {
            cerr << "Error: Invalid or full slot list." << endl;
        }
    }

    bool isSlotAvailable(const string& slot) {
        for (int i = 0; i < slot_count; ++i) if (slots[i] == slot) return true;
        return false;
    }

    void displaySlots() const {
        cout << "Available slots for " << name << ": ";
        for (int i = 0; i < slot_count; ++i) cout << slots[i] << " ";
        cout << endl;
    }

    void display() const {
        cout << "Doctor ID: " << doctor_id << ", Name: " << name << ", Specialization: " << specialization << endl;
    }
};

// Class for patients
class Patient {
private:
    int patient_id, age, history_count = 0;
    string name, medical_history[MAX_HISTORY];

public:
    Patient(int id = 0, string n = "", int a = 0) : patient_id(id), name(n), age(a) {}
    void updateMedicalHistory(const string& new_record) {
        if (history_count < MAX_HISTORY) medical_history[history_count++] = new_record;
        else cerr << "Error: Medical history is full." << endl;
    }

    string serialize() const {
        string serialized_data = to_string(patient_id) + "," + name + "," + to_string(age);
        for (int i = 0; i < history_count; ++i) serialized_data += "," + medical_history[i];
        return serialized_data;
    }

    void display() const {
        cout << "Patient ID: " << patient_id << ", Name: " << name << ", Age: " << age << endl;
        cout << "Medical History: ";
        for (int i = 0; i < history_count; ++i) cout << medical_history[i] << "; ";
        cout << endl;
    }
};

// Class to manage doctor schedules
class DoctorSchedule {
private:
    Doctor doctors[MAX_DOCTORS];
    int doctor_count = 0;

public:
    void addDoctor(const Doctor& doctor) {
        if (doctor_count < MAX_DOCTORS) doctors[doctor_count++] = doctor;
        else cerr << "Error: Doctor list is full." << endl;
    }

    void addAppointmentSlot(const string& doctor_name, const string& slot) {
        for (int i = 0; i < doctor_count; ++i) {
            if (doctors[i].getName() == doctor_name) {
                doctors[i].addSlot(slot);
                return;
            }
        }
        cerr << "Error: Doctor not found." << endl;
    }

    bool isAvailable(const string& doctor_name, const string& slot) {
        for (int i = 0; i < doctor_count; ++i)
            if (doctors[i].getName() == doctor_name && doctors[i].isSlotAvailable(slot)) return true;
        return false;
    }

    void displayDoctors() const {
        for (int i = 0; i < doctor_count; ++i) {
            doctors[i].display();
            doctors[i].displaySlots();
        }
    }
};

// Write patient data to a file
void writePatientData(const Patient& patient) {
    ofstream file("patient_records.txt", ios::app);
    if (file.is_open()) {
        file << patient.serialize() << endl;
        file.close();
        cout << "Patient record saved successfully." << endl;
    } else {
        cerr << "Error: Unable to open file for writing." << endl;
    }
}

// Display the main menu
void displayMenu() {
    cout << "Healthcare Management System" << endl;
    cout << "1. Add Patient\n2. View Patients\n3. Add Doctor\n4. View Doctors\n5. Book Appointment\n6. Add Doctor Slot\n7. Exit\nEnter your choice: ";
}

// Handle adding a new patient
void handleAddPatient() {
    int patient_id, patient_age;
    string patient_name, history_entry;

    cout << "Enter patient details:\nPatient ID: ";
    cin >> patient_id;
    cout << "Name: ";
    cin.ignore();
    getline(cin, patient_name);
    cout << "Age: ";
    cin >> patient_age;

    Patient patient(patient_id, patient_name, patient_age);
    cout << "Enter medical history entries (up to " << MAX_HISTORY << "). Type 'done' to finish:\n";
    cin.ignore();
    for (int i = 0; i < MAX_HISTORY; ++i) {
        getline(cin, history_entry);
        if (history_entry == "done") break;
        patient.updateMedicalHistory(history_entry);
    }

    writePatientData(patient);
}

// Handle viewing all patients
void handleViewPatients() {
    ifstream file("patient_records.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) cout << line << endl;
        file.close();
    } else cerr << "Error: Unable to open file for reading." << endl;
}

// Handle adding a new doctor
void handleAddDoctor(DoctorSchedule& ds) {
    int doctor_id;
    string doctor_name, specialization;

    cout << "Enter doctor details:\nDoctor ID: ";
    cin >> doctor_id;
    cout << "Name: ";
    cin.ignore();
    getline(cin, doctor_name);
    cout << "Specialization: ";
    getline(cin, specialization);

    ds.addDoctor(Doctor(doctor_id, doctor_name, specialization));
}

// Handle viewing all doctors and their slots
void handleViewDoctors(const DoctorSchedule& ds) {
    ds.displayDoctors();
}

// Handle booking an appointment with a doctor
void handleBookAppointment(DoctorSchedule& ds) {
    string doctor_name, slot;

    cout << "Enter doctor name: ";
    cin.ignore();
    getline(cin, doctor_name);
    cout << "Enter appointment slot: ";
    getline(cin, slot);

    for (int i = 0; i < MAX_PREDEFINED_SLOTS; ++i) {
        if (predefined_slots[i] == trim(slot)) {
            ds.addAppointmentSlot(doctor_name, slot);
            cout << "Appointment booked successfully." << endl;
            return;
        }
    }

    cerr << "Error: Invalid slot. Please enter a slot from the predefined list." << endl;
}


// Handle adding a new slot to a doctor's schedule
void handleAddDoctorSlot(DoctorSchedule& ds) {
    string doctor_name, slot;

    cout << "Enter doctor name: ";
    getline(cin, doctor_name);
    cout << "Enter slot to add: ";
    getline(cin, slot);

    if (ds.isAvailable(doctor_name, slot)) {
        cerr << "Error: Slot is already available." << endl;
        return;
    }

    bool validSlot = false;
    for (const auto& predefined_slot : predefined_slots) {
        if (trim(slot) == predefined_slot) {
            validSlot = true;
            break;
        }
    }

    if (validSlot) {
        ds.addAppointmentSlot(doctor_name, slot);
        cout << "Slot added successfully." << endl;
    } else {
        cerr << "Error: Invalid slot. Please enter a slot from the predefined list." << endl;
    }
}

// Main function to display menu and handle user choices
int main() {
    DoctorSchedule ds;
    int choice;

    while (true) {
        displayMenu();
        cin >> choice;
        cin.ignore();
switch (choice) {
            case 1:
                handleAddPatient();
                break;
            case 2:
                handleViewPatients();
                break;
            case 3:
                handleAddDoctor(ds);
                break;
            case 4:
                handleViewDoctors(ds);
                break;
            case 5:
                handleBookAppointment(ds);
                break;
            case 6:
                handleAddDoctorSlot(ds);
                break;
            case 7:
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
    }