#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <queue>
#include <vector>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

class Transaction {
public:
    string ownerName;
    int type;
    int lama;
    int biaya;
    string tgl_masuk;
    int spot;
    string spotLocation;

    void calculateFee() {
        if (type == 1) {
            if (lama >= 1 && lama < 6) {
                biaya = lama * 2000;
            } else {
                biaya = 12000;
            }
        } else if (type == 2) {
            if (lama >= 1 && lama < 6) {
                biaya = lama * 4000;
            } else {
                biaya = 24000;
            }
        }
    }
};

ofstream outputFile("transaksi.txt", ios::app);

struct User {
    string username;
    string password;
};

class ParkingSpot {
public:
    string location;
    bool isAvailable;

    ParkingSpot(string loc) : location(loc), isAvailable(true) {}
};

void registerUser() {
    User user;
    cout << "\n\n\n \t\t\t ---- REGISTER ----  \t\t\t\t \n";
    cout << "\n \t\t\t Buat Username : ";
    cin >> user.username;
    cout << "\n \t\t\t Buat Password : ";
    cin >> user.password;

    ofstream userFile("users.txt", ios::app);
    userFile << user.username << " " << user.password << endl;
    userFile.close();

    cout << "\n\n User berhasil didaftarkan.\n\n";
}

bool loginUser(User& currentUser) {
    cout << "\n\n\n \t\t\t ---- LOGIN SISTEM PARKIR ----  \t\t\t\t \n";
    cout << "\n \t\t\t Username : ";
    cin >> currentUser.username;
    cout << "\n \t\t\t Password : ";
    cin >> currentUser.password;

    ifstream userFile("users.txt");
    string usernameFromFile, passwordFromFile;
    while (userFile >> usernameFromFile >> passwordFromFile) {
        if (usernameFromFile == currentUser.username && passwordFromFile == currentUser.password) {
            return true;
        }
    }
    userFile.close();
    return false;
}

int divideAndConquerSum(Transaction trx[], int start, int end) {
    if (start > end) {
        return 0;
    }
    if (start == end) {
        return trx[start].biaya;
    }
    int mid = (start + end) / 2;
    int leftSum = divideAndConquerSum(trx, start, mid);
    int rightSum = divideAndConquerSum(trx, mid + 1, end);
    return leftSum + rightSum;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string generateBarcode(int length) {
    string barcode;
    for (int i = 0; i < length; ++i) {
        barcode += (i % 2 == 0) ? "|" : ":";
    }
    return barcode;
}

void printReceipt(const ParkingSpot &spot) {
    cout << "\n\n\t\t\t======= STRUK PARKIR =======\n";
    cout << "\t\t\t Lokasi Parkir: " << spot.location << "\n";
    cout << "\t\t\t Barcode: " << generateBarcode(12) << "\n";
    cout << "\t\t\t============================\n\n";
}

int main() {
    vector<ParkingSpot> parkingSpots = {
        ParkingSpot("A1FM"), ParkingSpot("A2FM"), ParkingSpot("B1FM"), ParkingSpot("B2FM"),
        ParkingSpot("C1FM"), ParkingSpot("C2FM"), ParkingSpot("D1FM"), ParkingSpot("D2FM"),
        ParkingSpot("E1FM"), ParkingSpot("E2FM"), ParkingSpot("F1FM"), ParkingSpot("F2FM"),
        ParkingSpot("G1FM"), ParkingSpot("G2FM"), ParkingSpot("H1FM"), ParkingSpot("H2FM"),
        ParkingSpot("I1FM"), ParkingSpot("I2FM"), ParkingSpot("J1FM"), ParkingSpot("J2FM")
    };

    Transaction trx[100];
    queue<Transaction> trxQueue;
    User currentUser;
    char yn;
    int pil, m = -1, total = 0;

    registerUser();

login:
    if (!loginUser(currentUser)) {
        cout << "\n\n Username atau Password Salah \n";
        clearInputBuffer();
        goto login;
    }

    cout << "\n\n Anda berhasil login. \n" << endl;

menu:
    clearScreen();
    cout << "+--------------------------+" << endl;
    cout << "¦      Sistem Parkir       ¦" << endl;
    cout << "¦--------------------------¦" << endl;
    cout << "¦          Menu            ¦" << endl;
    cout << "¦--------------------------¦" << endl;
    cout << "¦ 1. Pemesanan             ¦" << endl;
    cout << "¦ 2. Daftar Struk          ¦" << endl;
    cout << "¦ 3. Cari Lahan Parkir     ¦" << endl;
    cout << "¦ 4. Pengeluaran           ¦" << endl;
    cout << "¦ 5. Keluar                ¦" << endl;
    cout << "+--------------------------+" << endl;
    cout << "Masukan Pilihan anda[1 s.d. 5] = ";
    cin >> pil;

    if (pil == 1) {
trxmasuk:
        clearScreen();
        m++;
        cout << "\n\n \t\t\t ------- PEMESANAN PARKIR ------- \t\t\t";
        cout << "\n Masukkan Nama Pemilik : ";
        cin >> trx[m].ownerName;
        cout << "\n Type kendaraan : \n  1. motor \n  2. mobil \n";
        cout << " Pilih type kendaraan [1 / 2 ] : ";
        cin >> trx[m].type;
        cout << " Lama Jam Parkir : ";
        cin >> trx[m].lama;

        time_t waktu;
        time(&waktu);
        trx[m].tgl_masuk = ctime(&waktu);

        cout << "\n\nDaftar Lahan Parkir yang Tersedia di PARKIR FMIPA:\n";
        for (int i = 0; i < parkingSpots.size(); ++i) {
            if (parkingSpots[i].isAvailable) {
                cout << "Spot " << i + 1 << " (" << parkingSpots[i].location << ")\n";
            }
        }

        int chosenSpot;
        cout << "\nPilih spot parkir yang tersedia (masukkan nomor spot): ";
        cin >> chosenSpot;

        // Validasi spot parkir yang dipilih
        if (chosenSpot < 1 || chosenSpot > parkingSpots.size() || !parkingSpots[chosenSpot - 1].isAvailable) {
            cout << "\nSpot tidak valid atau sudah terisi. Silakan coba lagi.\n";
            clearInputBuffer();  // Tambahkan ini untuk membersihkan buffer input
            goto trxmasuk;
        }

        parkingSpots[chosenSpot - 1].isAvailable = false;
        trx[m].spot = chosenSpot;
        trx[m].spotLocation = parkingSpots[chosenSpot - 1].location;
        printReceipt(parkingSpots[chosenSpot - 1]);

        cout << " ++++++++++++++++++++++++++++++++ \n";
        cout << " + Nama Pemilik : " << trx[m].ownerName << endl;
        cout << " + Type : ";
        if (trx[m].type == 1) {
            cout << "motor";
        }
        if (trx[m].type == 2) {
            cout << "mobil";
        }
        cout << endl;
        cout << " + Lama parkir : " << trx[m].lama << " jam";
        cout << endl;
        trx[m].calculateFee();
        cout << " + Biaya : Rp " << trx[m].biaya << endl;
        cout << " + Tanggal Masuk : " << trx[m].tgl_masuk;
        cout << " + Spot : " << trx[m].spot << " (" << trx[m].spotLocation << ")\n";
        cout << " ++++++++++++++++++++++++++++++++ \n";

        trxQueue.push(trx[m]);

        cout << " Tambah Lagi ? (y/n) :";
        cin >> yn;

        if (yn == 'y' || yn == 'Y') {
            clearScreen(); 
            goto trxmasuk;
        } else if (yn == 'n' || yn == 'N') {
            outputFile << "Nama Pemilik: " << trx[m].ownerName << ", Type: " << trx[m].type << ", Lama Parkir: " << trx[m].lama << " jam, Biaya: Rp " << trx[m].biaya << ", Tanggal Masuk: " << trx[m].tgl_masuk << ", Spot: " << trx[m].spot << " (" << trx[m].spotLocation << ")\n";
            cout << "\n\n Data Berhasil Disimpan \n" << endl;
            clearScreen(); 
            goto menu;
        } else {
            clearScreen(); 
            goto trxmasuk;
        }
    } else if (pil == 2) {
        clearScreen();
        cout << "\n" << endl;
        cout << "\t\t\t ------- STRUK ------- \t\t\t";
        if (m >= 0) {
            cout << endl;
            for (int a = 0; a <= m; a++) {
                cout << " ++++++++++++++++++++++++++++++++ \n";
                cout << " + Nama Pemilik : " << trx[a].ownerName << endl;
                cout << " + Type : ";
                if (trx[a].type == 1) {
                    cout << "motor";
                }
                if (trx[a].type == 2) {
                    cout << "mobil";
                }
                cout << endl;
                cout << " + Lama jam parkir : " << trx[a].lama << "jam";
                cout << endl;
                cout << " + Biaya : Rp " << trx[a].biaya << endl;
                cout << " + Tanggal Masuk : " << trx[a].tgl_masuk;
                cout << " + Spot : " << trx[a].spot << " (" << trx[a].spotLocation << ")\n";
                cout << " ++++++++++++++++++++++++++++++++ \n";
            }
        } else {
            cout << " -- Tidak ada transaksi -- \n";
        }

        cout << " Kembali ke menu ? (y/n) :";
        cin >> yn;

        if (yn == 'y' || yn == 'Y') {
            clearScreen(); 
            goto menu;
        } else if (yn == 'n' || yn == 'N') {
            return 0;
        } else {
            return 0;
        }
    } else if (pil == 3) {
        clearScreen();
        cout << "\n\n\t\t\t ------- CARI LAHAN PARKIR ------- \t\t\t\n\n";
        for (int i = 0; i < parkingSpots.size(); ++i) {
            cout << "Spot " << i + 1 << " (" << parkingSpots[i].location << "): ";
            if (parkingSpots[i].isAvailable) {
                cout << "Tersedia\n";
            } else {
                cout << "Terisi\n";
            }
        }

        cout << "\nKembali ke menu? (y/n) : ";
        cin >> yn;

        if (yn == 'y' || yn == 'Y') {
            clearScreen(); 
            goto menu;
        } else {
            return 0;
        }
    } else if (pil == 4) {
        clearScreen();
        cout << "\n\n\t\t\t ------- TOTAL PENGELUARAN ------- \t\t\t\n\n";
        total = divideAndConquerSum(trx, 0, m);
        cout << " Total Pengeluaran : Rp " << total << endl;

        cout << "\nKembali ke menu? (y/n) : ";
        cin >> yn;

        if (yn == 'y' || yn == 'Y') {
            clearScreen(); 
            goto menu;
        } else {
            return 0;
        }
    } else if (pil == 5) {
        return 0;
    } else {
        cout << " Pilihan tidak valid. Kembali ke menu.\n";
        goto menu;
    }

    outputFile.close();
    return 0;
}

