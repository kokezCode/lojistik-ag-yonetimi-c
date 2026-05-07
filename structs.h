/**
 * ============================================================================
 * structs.h - Lojistik Firma Projesi Veri Yapıları
 * ============================================================================
 * 
 * Bu başlık dosyası, lojistik firması için şehirler arası bağlantıları
 * yöneten sistemdeki tüm temel veri yapılarını tanımlar.
 *
 * Öğrenci No: 1240505902
 *
 * İçerik:
 *   - KenarDugumu: Grafın kenar listesi için düğüm yapısı
 *   - SehirDugumu: Şehir bilgilerini tutan yapı (Graf düğümü)
 *   - Graf: Komşuluk listesi (Adjacency List) tabanlı graf yapısı
 *   - Paket: Kargo paketi bilgilerini tutan yapı
 *   - HashGirdi: Hash tablosu girdi yapısı (Açık adresleme için)
 *   - HashTablosu: Hash tablosu ana yapısı
 *
 * Not: Her paket, Graf içindeki bir şehri işaret eden bir pointer (SehirDugumu*)
 *      içermektedir.
 * ============================================================================
 */

#ifndef STRUCTS_H
#define STRUCTS_H

/* ---- Sabit Tanımlamalar ---- */
#define MAX_SEHIR_SAYISI     100      // Maksimum şehir sayısı                  
#define MAX_SEHIR_ADI_UZUN   50       // Şehir adı için maksimum karakter uzunluğu 
#define MAX_PAKET_ACIKLAMA    200      // Paket açıklaması maksimum uzunluk       
#define HASH_TABLO_BOYUTU     101      // Hash tablosu boyutu (asal sayı)         
#define OGRENCI_NO            1240505902 // Öğrenci numarası                      

/* Hash tablosu girdi durumları (Açık adresleme için) */
#define DURUM_BOS       0    // Girdi boş (hiç kullanılmamış)   
#define DURUM_DOLU      1    // Girdide geçerli veri var         
#define DURUM_SILINDI   2    // Girdideki veri silinmiş (tombstone) 

/* ---- İleriye Dönük Bildirimler (Forward Declarations) ---- */
typedef struct KenarDugumu KenarDugumu;
typedef struct SehirDugumu SehirDugumu;

/* ============================================================================
 * KenarDugumu - Graf Kenar Yapısı
 * ============================================================================
 * Komşuluk listesindeki her bir kenarı temsil eder. Her kenar, hedef şehrin
 * indeksini, iki şehir arasındaki mesafeyi (km) ve tahmini seyahat süresini
 * (saat) tutar. next pointer'ı aynı şehre bağlı diğer kenarlara bağlanır.
 * ============================================================================
 */
struct KenarDugumu {
    int hedef_sehir_indeksi;   // Bağlantı kurulan hedef şehrin dizideki indeksi 
    int mesafe;                // İki şehir arasındaki mesafe (kilometre)       
    int seyahat_suresi;        // Tahmini ulaşım süresi (saat)                 
    KenarDugumu *next;         // Bir sonraki kenar düğümüne işaretçi           
};

/* ============================================================================
 * SehirDugumu - Şehir Düğüm Yapısı
 * ============================================================================
 * Grafın her bir düğümü bir şehri temsil eder. Her şehrin bir adı, benzersiz
 * bir kimliği ve komşu şehirleri tutan bir kenar listesi (adjacency list) vardır.
 * Bu yapı, Graf yapısı içindeki dizide doğrudan depolanır ve Paket
 * yapısındaki pointer bu dizideki bir elemana işaret eder.
 * ============================================================================
 */
struct SehirDugumu {
    char ad[MAX_SEHIR_ADI_UZUN];   // Şehrin adı                            
    int id;                        // Şehrin benzersiz kimliği             
    int aktif;                     // Şehrin aktif olma durumu (1=aktif, 0=değil) 
    KenarDugumu *kenar_listesi;   // Komşu şehirlerin kenar listesi başlangıcı 
};

/* ============================================================================
 * Graf - Komşuluk Listesi (Adjacency List) Tabanlı Graf Yapısı
 * ============================================================================
 * Şehirler arası bağlantıları yöneten ana veri yapısıdır. Her şehir,
 * sehirler dizisinde bir SehirDugumu olarak saklanır. Şehirler arası
 * bağlantılar ise her şehrin kenar_listesi'nde KenarDugumları zinciri
 * şeklinde tutulur. Bu yapı, BFS gezinmesi ve rota sorgulaması için
 * optimize edilmiştir.
 * ============================================================================
 */
typedef struct {
    SehirDugumu *sehirler;   // Şehir düğümleri dizisi (dinamik tahsis)    
    int sehir_sayisi;         // Graftaki mevcut şehir sayısı               
    int kapasite;             // Dizinin toplam kapasitesi              
} Graf;

/* ============================================================================
 * Paket - Kargo Paketi Yapısı
 * ============================================================================
 * Lojistik firmasında taşınan her bir kargoyu temsil eder. Her paketin
 * benzersiz bir kimliği, açıklaması, ağırlığı, kaynak ve hedef şehir
 * bilgileri bulunur. ÖNEMLİ: current_location pointer'ı, Graf yapısındaki
 * sehirler dizisindeki bir SehirDugumu elemanına doğrudan işaret eder.
 * Bu sayede paketin anlık konumu O(1) karmaşıklıkla öğrenilebilir.
 * ============================================================================
 */
typedef struct {
    int id;                        /* Paketin benzersiz kimliği                */
    char aciklama[MAX_PAKET_ACIKLAMA]; /* Paket içeriğinin açıklaması         */
    float agirlik;                 /* Paket ağırlığı (kilogram)                */
    int kaynak_sehir_id;           /* Gönderim yapılan kaynak şehir kimliği    */
    int hedef_sehir_id;            /* Teslim edilecek hedef şehir kimliği      */
    SehirDugumu *current_location; /* Graf içindeki mevcut şehre işaretçi      */
    char durum[20];                /* Paket durumu ("Beklemede", "Yolda", etc.) */
} Paket;

/* ============================================================================
 * HashGirdi - Hash Tablosu Girdi Yapısı
 * ============================================================================
 * Açık adresleme (Open Addressing) tekniği ile çakışma çözümü için
 * tasarlanmıştır. Her girdi, bir Paket verisi ve durum bilgisini tutar.
 * durum alanı üç değer alabilir:
 *   DURUM_BOS     : Bu hücre hiç kullanılmamış
 *   DURUM_DOLU    : Bu hücrede geçerli bir paket var
 *   DURUM_SILINDI : Bu hücredeki paket silinmiş (linear probing'de devam)
 * ============================================================================
 */
typedef struct {
    Paket paket;     /* Bu girdide saklanan paket verisi  */
    int durum;       /* Girdinin durumu (BOS/DOLU/SILINDI) */
} HashGirdi;

/* ============================================================================
 * HashTablosu - Hash Tablosu Ana Yapısı
 * ============================================================================
 * Paket sorgulamalarını O(1) ortalama karmaşıklıkla gerçekleştirmek için
 * kullanılan hash tablosu yapısıdır. Linear Probing yöntemi ile çakışma
 * çözümü uygulanır. Tablo boyutu asal sayı (101) seçilerek dağılım
 * kalitesi artırılmıştır.
 * ============================================================================
 */
typedef struct {
    HashGirdi *girdiler;  /* Hash girdileri dizisi (dinamik tahsis) */
    int eleman_sayisi;     /* Tablodaki mevcut eleman sayısı        */
    int boyut;             /* Tablonun toplam boyutu                */
} HashTablosu;

#endif /* STRUCTS_H */
