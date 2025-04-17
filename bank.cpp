/*
    ini syafiqnya aja yang emang rada meticulous
    saldo disini itu disimpen dalam satuan yen
    nominal x 100
    misal Rp1000,00 --> 1000.00 x 100 = 100000
    gunanya untuk menghindari error operasi float dan boolean
    yaitu 0.0 bisa jadi 0.000000001
*/

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

// ------------------------------ FORWARD DECLARATION --------------------------------------
// fungsi2 disini dideklarasi duluan tanpa defini dia ngapain
// fungsi lain yang didefinsikan duluan bisa manggil fungsi forward declaration
// misal cariNasabah_Norek. Dia dipanggil di constructor class Transfer
// meskipun didefinisikan di bawah class Transfer

void kapitalisasi(string* input);
int binarySearchRecursive(vector<unsigned int>& arr, int target, int left, int right);
class Transaksi;
class Rekening;
Rekening* cariNasabah_Norek(unsigned int norek);

// -----------------------------------------------------------------------------------------


// -------------------------------- GLOBAL VARIABLE ----------------------------------------
// Variable yang bisa diakses di seluruh letak program

double threshold = 0.75;
int slotTerisi = 0;
vector<unsigned int> norekTerpakai;
vector<Rekening*> daftarRekening(4, nullptr);
Transaksi* HeadTransaksiSemua = nullptr;

// ------------------------------------------------------------------------------------------


// --------------------------------- CLASS & ENUM -------------------------------------------

enum jenisTabungan {
    PELAJAR, 
    PLATINUM, 
    GOLD, 
    DIAMOND
} typedef jenisTabungan;
string jenTab[4] = {"PELAJAR", "PLATINUM", "GOLD", "DIAMOND"};

enum jenistransaksi{
    SETOR,
    TARIK,
    TRANSFER
} typedef jenisTransaksi;
string jenTra[3]{"SETOR", "TARIK", "TRANSFER"};

class Transaksi {
    protected:
        unsigned int norekAsal, jumlah;
        time_t tanggal;
        jenisTransaksi jenisTrans;
        Rekening* rekeningAsal;
    
    public:
        Transaksi* nextTransaksiSemua;

        Transaksi(unsigned int asal, int jml, jenisTransaksi jns)
            {
            norekAsal = asal;
            jumlah = jml * 100;
            jenisTrans = jns;
            tanggal = time(nullptr);
            rekeningAsal = cariNasabah_Norek(norekAsal);
            
            if (HeadTransaksiSemua == nullptr)
            {
                HeadTransaksiSemua = this;
                nextTransaksiSemua = nullptr;
            } else {
                nextTransaksiSemua = HeadTransaksiSemua;
                HeadTransaksiSemua = this;
            }
        }
    
        int getjumlah(){ return jumlah; }
        jenisTransaksi getJenisTransaksi(){ return jenisTrans; }
        Rekening* getRekeningAsal(){ return rekeningAsal; }
        virtual Rekening* getRekeningTujuan(){ return nullptr; }
        virtual int getBiayaAdmin(){ return -1; }

        virtual void printTransaksi() {
            cout << "Jenis Transaksi: " << jenTra[jenisTrans] << endl;
            cout << "Dari Rekening: " << norekAsal << endl;
            cout << "jumlah: Rp" << jumlah / 100 << "," << (jumlah % 100 < 10 ? "0" : "") << jumlah % 100 << endl;
            cout << "Tanggal: " << ctime(&tanggal);
        }

        void rekeningDihapus(){
            rekeningAsal = nullptr;
        }

        virtual void rekeningTujuanDihapus(){};
    
        virtual ~Transaksi() {}
};

class Rekening {
private:
    unsigned int norek, saldo;
    string pin, namaNasabah, NIK, domisili, noTelp, namaIbu;
    jenisTabungan jenisTab;
    time_t waktuDibuat, waktuBerubah;

public:
    vector<Transaksi*> histori;
    Rekening(string p, string nn, string nik, string d, string nt, string ni, jenisTabungan j, double s) {
        // menyalin setiap data input
        pin = p;
        namaNasabah = nn; kapitalisasi(&namaNasabah);
        NIK = nik;
        domisili = d; kapitalisasi(&domisili);
        noTelp = nt;
        namaIbu = ni; kapitalisasi(&namaIbu);
        jenisTab = j;
        saldo = s * 100;
        norek = 0;

        // Menetapkan waktu Rekening dibuat menjadi waktu sistem saat ini
        time(&waktuDibuat);
        time(&waktuBerubah);

        // Menggenerasi nomor Rekening unik (terpisah dari hash value hash map)
        // Hal ini bertujuan menghindari nomor Rekening ikut berubah ketika rehashing
        // Semacam encryption
        generateNorek(namaNasabah);
    }

    void generateNorek(string namaNasabah) {
        // A-Z + " " + "'" = 28, kadi ada 28 kemungkinkan char
        for (int i = 0; i < namaNasabah.length(); i++) {
            norek += namaNasabah[i] * static_cast<int>(pow(28, i % 6));
        }

        // Memastikan norek berisi 10 digit
        if (norek / 1000000000 == 0) norek += 1000000000 * (namaNasabah.length() % 10 + 1);

        // Memastikan norek unik
        int iProbNorek = 0;
        while (binarySearchRecursive(norekTerpakai, norek, 0, norekTerpakai.size() - 1) != -1)
        {
            iProbNorek++;
            norek = (norek + iProbNorek * iProbNorek);
            
        }
        norekTerpakai.push_back(norek);
        sort(norekTerpakai.begin(), norekTerpakai.end()); // binarySearchRecursive hanya bisa bekerja pada data terurut
    }


    // ------------------------- get attribute  -------------------------
    
    unsigned int getNorek() { return norek; }
    unsigned int getSaldo() { return saldo; }
    string getNamaNasabah() { return namaNasabah; }
    jenisTabungan getJenisTab() { return jenisTab; }


    // -------------------------- print func ----------------------------

    string printSaldo(){;
        string tepatbelkoma;
        (saldo % 100 < 10 ? tepatbelkoma = "0" : tepatbelkoma = "");
        return to_string(saldo / 100) + "," + tepatbelkoma + to_string(saldo % 100);
    }

    void printInfo() {
        cout << "Nomor Rekening: " << norek << endl;
        cout << "Pin: " << pin << endl;
        cout << "Nama Nasabah: " << namaNasabah << endl;
        cout << "NIK: " << NIK << endl;
        cout << "Saldo: Rp" << printSaldo() << endl;
        cout << "Domisili: " << domisili << endl;
        cout << "No Telp: " << noTelp << endl;
        cout << "Nama Ibu: " << namaIbu << endl;
        cout << "Jenis Tabungan: " << jenTab[jenisTab] << endl;
        cout << "Tanggal dibuat: " << ctime(&waktuDibuat);
        cout << "Tanggal berubah: " << ctime(&waktuBerubah);
    }


    //  ----------------------- transaksi func -------------------------

    void setor(double jumlah) {
        if (jumlah > 0) {
            jumlah *= 100;      // konversi dulu ke "sen"
            saldo += jumlah;
            Transaksi* trans = new Transaksi(norek, jumlah, SETOR); // bikin log transaksi setor
            tambahTransaksiAkun(trans);     // tambah log transaksi ke histori transkasi
            cout << "Setoran berhasil. Saldo sekarang: " << printSaldo() << endl;
        } else {
            cout << "Jumlah setoran tidak valid." << endl;
        }
    }

    void balikinSetor(double jumlah){
        jumlah *= 100;
        saldo -= jumlah;
    }
    
    void tarik(double jumlah) {
        jumlah *= 100;
        if (jumlah > 0 && jumlah <= saldo) {
            saldo -= jumlah;
            Transaksi* trans = new Transaksi(norek, jumlah, TARIK);
            tambahTransaksiAkun(trans);
            cout << "Penarikan berhasil. Saldo sekarang: " << printSaldo() << endl;
        } else {
            cout << "Penarikan gagal. Saldo tidak mencukupi atau jumlah tidak valid." << endl;
        }
    }

    void balikinTarik(double jumlah){
        jumlah *= 100;
        saldo += jumlah;
    }

    bool kirimTransfer(double jumlah){
        jumlah *= 100;
        
        int biayaAdmin;
        switch (jenisTab) {
        case PELAJAR:   biayaAdmin = 0;         break;
        case PLATINUM:  biayaAdmin = 500000;    break;
        case GOLD:      biayaAdmin = 250000;    break;
        case DIAMOND:   biayaAdmin = 0;         break;
        }

        if (jumlah > 0 && jumlah + biayaAdmin <= saldo){
            saldo = saldo - (jumlah + biayaAdmin);
            return true;    // berhasil transfer
        } else return false; // gagal transfer
    }

    void balikinKirimTransfer(double jumlah, int biayaAdmin){
        int balik = (jumlah + static_cast<double>(biayaAdmin));
        saldo += balik;
    }

    void terimaTransfer(double jumlah){
        jumlah *= 100;
        saldo += jumlah;
    }

    void balikinTerimaTransfer(double jumlah){
        saldo -= jumlah;
    }

    void tambahTransaksiAkun(Transaksi* t) {
        histori.push_back(t);
        time(&waktuBerubah);
    }


    ~Rekening() {
        for (Transaksi* t : histori) {
            delete t;
        }
    }
};

// Transfer di bawah rekening soalnya manggil fungsi milik class rekening
class Transfer : public Transaksi {
    private:
        Rekening* rekeningTujuan;
        unsigned int norekTujuan;
        int biayaAdmin;
    
    public:
        Transfer(unsigned int asal, int jml, unsigned int tujuan)
            : Transaksi(asal, jml, TRANSFER){
                norekTujuan = tujuan;
                rekeningTujuan = cariNasabah_Norek(tujuan);
                switch (rekeningAsal->getJenisTab()) {
                    case PELAJAR:   biayaAdmin = 0;         break;
                    case PLATINUM:  biayaAdmin = 500000;    break;
                    case GOLD:      biayaAdmin = 250000;    break;
                    case DIAMOND:   biayaAdmin = 0;         break;
                }
            }
    
        Rekening* getRekeningTujuan() override { return rekeningTujuan; }
        int getBiayaAdmin() override { return biayaAdmin; }

        void printTransaksi() override {
            Transaksi::printTransaksi();
            cout << "Ke Rekening: " << norekTujuan << endl;
            cout << "Biaya Admin: Rp" << biayaAdmin / 100 << "," << (biayaAdmin % 100 < 10 ? "0" : "") << biayaAdmin % 100 
            << endl;
        }

        void rekeningTujuanDihapus() override {
            rekeningTujuan = nullptr;
        }
};

// -----------------------------------------------------------------------------------------


// ---------------------------- INSERTION & REHASHING --------------------------------------

// Fungsi insertion ke hashmap yang tidak perlu cek threshold, dipanggil saat rehashing
void insertToHashMapWithoutRehashing(vector<Rekening*>* targetMap, Rekening* input) {

    // hash fucntion dasar
    int iProbeInsert = 0;
    unsigned int norek = input->getNorek();
    double hashVal = static_cast<double>(norek) * 0.61;
    double frac = hashVal - floor(hashVal);
    int index = floor(frac * (targetMap->size()));

    // probing
    while ((*targetMap)[index] != nullptr) {
        ++iProbeInsert;
        index = (index + iProbeInsert * iProbeInsert) % (targetMap->size());
    }

    // Rekening baru disimpan ke slot kosong hash map, jumlah slot terisi bertambah
    (*targetMap)[index] = input;
    slotTerisi++;
}

// Fungsi rehashing ketika load factor mencapai threshold yang telah ditentukan 
void rehashing(vector<Rekening*>* targetMap) {
    float loadFactor = static_cast<float>(slotTerisi) / static_cast<float>(targetMap->size());

    if (loadFactor >= threshold) {
        // Membuat hash map baru
        vector<Rekening*> hashMapBaru(targetMap->size()*2, nullptr);

        // Memindahkan tiap pointer Rekening ke hash map yang baru
        for (int i = 0; i < targetMap->size(); i++) {
            if ((*targetMap)[i] != nullptr) {
                insertToHashMapWithoutRehashing(&hashMapBaru, (*targetMap)[i]);
            }
        }

        // Pointer daftar Rekening sekarang merujuk ke hashmap yang baru
        *targetMap = hashMapBaru;
    }
}

// Fungsi untuk memasukkan Rekening ke dalam struktur hash map
// Hashing dengen metode multiplication method agar ukuran hash map tidak kritis
void insertToHashMap(vector<Rekening*>* targetMap, Rekening* input) {

    // memastikan load factor tidak melewati threshold
    rehashing(targetMap);

    // hash fucntion dasar
    int iProbeInsert = 0;
    unsigned int norek = input->getNorek();
    double hashVal = static_cast<double>(norek) * 0.61;
    double frac = hashVal - floor(hashVal);
    int index = floor(frac * (targetMap->size()));

    // probing
    while ((*targetMap)[index] != nullptr) {
        ++iProbeInsert;
        index = (index + iProbeInsert * iProbeInsert) % (targetMap->size());
    }

    // Rekening baru disimpan ke slot kosong hash map, jumlah slot terisi bertambah
    (*targetMap)[index] = input;
    slotTerisi++;
}

// -----------------------------------------------------------------------------------------


// ----------------------------- SEARCHING & DELETION --------------------------------------

Rekening* cariNasabah_Nama(const string& nama) {
    string query = nama;
    kapitalisasi(&query);

    // loop untuk mencari pointer dengan nama nasabah dicari
    for (int i = 0; i < daftarRekening.size(); i++) {
        if (daftarRekening[i] != nullptr) {                             // kalau null gak usah dicek
            if (query == daftarRekening[i]->getNamaNasabah()) {         // mencocokkan nama nasabah
                return daftarRekening[i];                               // benar: balikkan pointer
            }
        }
    }

    // kalau tidak kena return saat semua rekening sudah dicek berarti nama tidak ada
    cout << "Nasabah dengan nama " << query << " tidak ditemukan" << endl;
    return nullptr;
}

Rekening* cariNasabah_Norek(unsigned int norek) {
    
    if (binarySearchRecursive(norekTerpakai, norek, 0, norekTerpakai.size()) != -1) // cek dulu apakah ada
    {
        // hash fucntion dasar
        int iProbeSearch = 0;
        double hashVal = static_cast<double>(norek) * 0.61;
        double frac = hashVal - floor(hashVal);
        int index = floor(frac * (daftarRekening.size()));

        // probing
        while (daftarRekening[index] == nullptr) {
            ++iProbeSearch;
            index = (index + iProbeSearch * iProbeSearch) % (daftarRekening.size());
        }

        return daftarRekening[index];
    } else {
        // kalau tidak kena return saat semua rekening sudah dicek berarti nama tidak ada
        cout << "Nasabah dengan nomor rekening " << norek << " tidak ditemukan" << endl;
        return nullptr;
    }
}

void hapusRekening(unsigned int norek) {
    if (binarySearchRecursive(norekTerpakai, norek, 0, norekTerpakai.size()) != -1) // cek dulu apakah ada
    {
        // hash fucntion dasar
        int iProbeSearch = 0;
        double hashVal = static_cast<double>(norek) * 0.61;
        double frac = hashVal - floor(hashVal);
        int index = floor(frac * (daftarRekening.size()));

        // probing
        while (daftarRekening[index] == nullptr) {
            ++iProbeSearch;
            index = (index + iProbeSearch * iProbeSearch) % (daftarRekening.size());
        }

        // menghindari pointer dangling pada transaksi
        Transaksi* bantu = HeadTransaksiSemua;
        while (bantu != nullptr)
        {
            if (bantu->getRekeningAsal()->getNorek() == norek) bantu->rekeningDihapus();

            if (bantu->getJenisTransaksi() == TRANSFER)
            {
                if (bantu->getRekeningTujuan()->getNorek() == norek) bantu->rekeningTujuanDihapus();
            }
            
            bantu = bantu->nextTransaksiSemua;
        }

        // hapus rekening
        Rekening* hapus = daftarRekening[index];
        daftarRekening[index] = nullptr;
        delete hapus;
        cout << "Rekening dengan norek " << norek << " berhasil dihapus.\n";
        return;
    } else {
        // kalau tidak kena return saat semua rekening sudah dicek berarti nama tidak ada
        cout << "Nasabah dengan nomor rekening " << norek << " tidak ditemukan" << endl;
        return;
    }
}

// -----------------------------------------------------------------------------------------


// ---------------------------------- MISCELLANEOUS -----------------------------------------

void kapitalisasi(string* input) {
    for (int i = 0; i < input->length(); i++) {
        (*input)[i] = toupper((*input)[i]);
    }
}

int binarySearchRecursive(vector<unsigned int>& arr, int target, int left, int right) {
    // Base case: jika left lebih besar dari right, berarti elemen tidak ditemukan
    if (left > right) {
    return -1;
    }

    // Hitung indeks tengah
    int mid = left + (right - left) / 2;

    // Base case: jika elemen tengah adalah target, kembalikan indeksnya
    if (arr[mid] == target) {
    return mid;
    }
    
    // Jika target lebih kecil dari elemen tengah, cari ke kiri
    if (target < arr[mid]) {
    return binarySearchRecursive(arr, target, left, mid - 1);
    }
    // Jika target lebih besar dari elemen tengah, cari ke kanan
    else {
    return binarySearchRecursive(arr, target, mid + 1, right);
    }
}    

void transfer(Rekening *pengirim, Rekening *penerima, double jumlah) {
    if (pengirim->kirimTransfer(jumlah) == true) {
        penerima->terimaTransfer(jumlah);
        Transaksi* trans = new Transfer(pengirim->getNorek(), jumlah, penerima->getNorek());
        pengirim->tambahTransaksiAkun(trans);
        penerima->tambahTransaksiAkun(trans);
        cout << "Transfer berhasil dari " << pengirim->getNamaNasabah() << " ke " << penerima->getNamaNasabah() << "." << endl;
        cout << "Saldo " << pengirim->getNamaNasabah() << ": " << pengirim->printSaldo() << endl;
        cout << "Saldo " << penerima->getNamaNasabah() << ": " << penerima->printSaldo() << endl;
    } else {
        cout << "Transfer gagal. Saldo tidak mencukupi atau jumlah tidak valid." << endl;
    }
}

void tampilkanHistori(unsigned int norek) {
    cout << "Histori Transaksi untuk Rekening " << norek << ":\n";
    Rekening* Rekening = cariNasabah_Norek(norek);
    for (auto& t : Rekening->histori) {
        t->printTransaksi();
        cout << "---------------------\n";
    }
}

void undoTransaksi(){
    bool success = true;

    // balikin uangnya
    switch (HeadTransaksiSemua->getJenisTransaksi())
    {
    case SETOR:
        HeadTransaksiSemua->getRekeningAsal()->balikinSetor(HeadTransaksiSemua->getjumlah());
        break;

    case TARIK:
        HeadTransaksiSemua->getRekeningAsal()->balikinTarik(HeadTransaksiSemua->getjumlah());
        break;

    case TRANSFER:
        if (HeadTransaksiSemua->getRekeningAsal() != nullptr && HeadTransaksiSemua->getRekeningTujuan() != nullptr)
        {
            // balikin uang pengirim
            HeadTransaksiSemua->getRekeningAsal()
            ->balikinKirimTransfer(HeadTransaksiSemua->getjumlah(), HeadTransaksiSemua->getBiayaAdmin());
            // balikin uang penerima
            HeadTransaksiSemua->getRekeningTujuan()
            ->balikinTerimaTransfer(HeadTransaksiSemua->getjumlah());
        } else {
            success = false;
            cout << "Salah satu rekening sudah dihapus sehingga transaksi tidak bisa di-undo\n";
        }
        break;

    default:
        break;
    }

    // geser head
    if (success)
    {
        Transaksi* hapus = HeadTransaksiSemua;
        HeadTransaksiSemua = HeadTransaksiSemua->nextTransaksiSemua;
        delete hapus;
        cout << "Transaksi terakhir berhasil dibatalkan\n";
    }
}

// -----------------------------------------------------------------------------------------


int main() {

    // Tambahkan beberapa akun untuk memicu rehashing
    vector<Rekening*> semuaRekening;
    semuaRekening.push_back(new Rekening("1234", "Najma Hamida", "3214567890123456", "Jakarta", "08123456789", "Sari", PLATINUM, 15000.76));
    semuaRekening.push_back(new Rekening("5678", "Rizky Aditya", "3214567890123457", "Bandung", "08129876543", "Lina", GOLD, 18000));
    semuaRekening.push_back(new Rekening("9012", "Putri Melati", "3214567890123458", "Surabaya", "08123455555", "Maya", DIAMOND, 20000));
    semuaRekening.push_back(new Rekening("3456", "Adit Saputra", "3214567890123459", "Medan", "08127778888", "Rika", PELAJAR, 5000));
    semuaRekening.push_back(new Rekening("7890", "Dina Rahayu", "3214567890123460", "Yogyakarta", "08121112222", "Rani", PLATINUM, 12000));

    for (auto& r : semuaRekening) {
        insertToHashMap(&daftarRekening, r);
    }

    cout << "Isi data Rekening setelah rehashing:\n";
    for (auto& r : daftarRekening) {
        if (r != nullptr)
        {
            r->printInfo();
            cout << "----------\n";
        }
    }


    // ---------------------------------------------------------------------------
    cout << "\n--- TEST cari nasabah nama dan norek ---\n";
    // ---------------------------------------------------------------------------

    cout << "Mencari nasabah bernama PUTRI MELATI\n";
    Rekening* hasil = cariNasabah_Nama("Putri Melati");
    if (hasil != nullptr) {
        hasil->printInfo();
    } else {
        cout << "Nasabah tidak ditemukan.\n";
    }
    cout << "----------\n";

    cout << "Mencari nasabah dengan nomor rekening 1906946485\n";
    Rekening* hasil2 = cariNasabah_Nama("Putri Melati");
    if (hasil2 != nullptr) {
        hasil2->printInfo();
    } else {
        cout << "Nasabah tidak ditemukan.\n";
    }
    cout << "----------\n";


    // ---------------------------------------------------------------------------
    cout << "\n--- TEST hapus rekening ---\n";
    // ---------------------------------------------------------------------------

    hapusRekening(1906946485); // Putri Melati
    cout << "\nMencari nasabah bernama PUTRI MELATI...\n";
    hasil = cariNasabah_Nama("Putri Melati");
    if (hasil != nullptr) {
        hasil->printInfo();
    } else {
        cout << "Nasabah tidak ditemukan.\n";
    }
    cout << "----------\n";


    // ---------------------------------------------------------------------------
    cout << "\n--- TEST setor, tarik, transfer ---\n";
    // ---------------------------------------------------------------------------

    // Ambil pointer ke Rekening tertentu
    Rekening* najma = semuaRekening[0]; // Najma Hamida
    Rekening* rizky = semuaRekening[1]; // Rizky Aditya

    // Tes Setor
    cout << "\n[Test] Setor Rp5000 ke Rekening Najma\n";
    cout << "Saldo Najma sebelum setor: Rp" << najma->printSaldo() << endl;
    najma->setor(5000.00); // Tambah Rp5000
    cout << "Saldo Najma setelah setor: Rp" << najma->printSaldo() << endl;

    // Tes Tarik - berhasil
    cout << "\n[Test] Tarik Rp3000 dari Rekening Najma\n";
    cout << "Saldo Najma sebelum tarik: Rp" << najma->printSaldo() << endl;
    najma->tarik(3000.00); // Kurangi Rp3000
    cout << "Saldo Najma setelah tarik: Rp" << najma->printSaldo() << endl;

    // Tes Tarik - gagal (melebihi saldo)
    cout << "\n[Test] Tarik Rp999999 dari Rekening Najma (harus gagal)\n";
    najma->tarik(999999.00); // Harus gagal
    cout << "Saldo Najma tetap: Rp" << najma->printSaldo() << endl;

    // Tes Transfer - berhasil
    cout << "\n[Test] Transfer Rp2000 dari Najma ke Rizky\n";
    transfer(najma, rizky, 2000.00);

    // Tes Transfer - gagal (saldo Najma kecil)
    cout << "\n[Test] Transfer Rp999999 dari Najma ke Rizky (harus gagal)\n";
    transfer(najma, rizky, 999999.00);
    
    // ---------------------------------------------------------------------------
    cout << "\n--- TEST histori transaksi ---\n";
    // ---------------------------------------------------------------------------

    cout << "Menampilkan histori transaksi dari Najma 1754569918\n";
    tampilkanHistori(1754569918);

    // ---------------------------------------------------------------------------
    cout << "\n--- TEST undo transaksi ---\n";
    // ---------------------------------------------------------------------------

    cout << "Membatalkan transaksi terakhir yaitu:\n";
    HeadTransaksiSemua->printTransaksi();
    cout << "Saldo Najma sebelum pembatalan transaksi: " << cariNasabah_Nama("NAJMA HAMIDA")->printSaldo() << endl;
    cout << "Saldo Rizky sebelum pembatalan transaksi: " << cariNasabah_Nama("Rizky Aditya")->printSaldo() << endl;
    undoTransaksi();
    cout << "Saldo Najma setelah pembatalan transaksi: " << cariNasabah_Nama("NAJMA HAMIDA")->printSaldo() << endl;
    cout << "Saldo Rizky setelah pembatalan transaksi: " << cariNasabah_Nama("Rizky Aditya")->printSaldo() << endl;



    // Entah kenapa error
    // Pembersihan memori
    // for (Rekening* r : daftarRekening) {
    //     delete r;  // akan memanggil destructor dan membersihkan histori transaksi juga
    // }

    // for (Rekening* r : semuaRekening) {
    //     delete r;
    // }

    return 0;
}
