/**
 * ============================================================================
 * hash.c - Hash Tablosu Fonksiyon Gerçekleştirimleri
 * ============================================================================
 * 
 * Bu dosya, paket bilgilerini hızlı sorgulamak için kullanılan hash tablosunun
 * tüm fonksiyonlarını içerir.
 *
 * Teknik Detaylar:
 *   Hash Fonksiyonu  : Tip A (Sayısal anahtarın modunu alan özelleştirilmiş fonksiyon)
 *   Öğrenci Numarası : 1240505902
 *   Çakışma Çözümü   : Açık Adresleme - Linear Probing
 *   Tablo Boyutu     : HASH_TABLO_BOYUTU (101 - asal sayı)
 *
 * Lineer Probing Çalışma Mantığı:
 *   Çakışma durumunda h(i) = (hash(k) + i) mod BOYUT formülü ile
 *   bir sonraki hücreye geçilir. i = 0, 1, 2, ... şeklinde artar.
 *   Bu yöntem, veri yerelliği (cache locality) avantajı sağlar ancak
 *   birincil kümeleşme (primary clustering) sorununa yol açabilir.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

/* ============================================================================
 * Dış bağlantı (extern) - graph.c dosyasındaki debug fonksiyonu
 * ============================================================================
 * Bellek izleme debug fonksiyonu graph.c dosyasında tanımlanmıştır.
 * Aynı fonksiyona hash.c dosyasından da erişim sağlanır.
 * ============================================================================
 */
extern void debug_bellek_izleme(const char *etiket, void *ptr, size_t boyut,
                                 const char *dosya, int satir);

/* Debug makrosu - satır ve dosya bilgilerini otomatik ekler */
#define DEBUG_MALLOC(etiket, ptr, boyut) \
    debug_bellek_izleme(etiket, ptr, boyut, __FILE__, __LINE__)

/* ============================================================================
 * Hash Fonksiyonu Sabitleri
 * ============================================================================
 * PRIME_MOD: Öğrenci numarasından (1240505902) türetilmiş değer.
 *            İlk 5 basamak: 12405
 *            Bu değer, hash fonksiyonunda birinci mod aşamasında kullanılır.
 * ============================================================================
 */
#define PRIME_MOD 12405


/* ============================================================================
 * hash_fonksiyonu - Tip A: Sayısal anahtarın modunu alan özelleştirilmiş fonksiyon
 * ============================================================================
 * 
 * Formül: h(k) = (k % PRIME_MOD) % HASH_TABLO_BOYUTU
 *
 * Açıklama:
 * Bu fonksiyon iki aşamalı modular aritmetik kullanır:
 *
 * 1. Aşama: k % PRIME_MOD (12405)
 *    - Anahtar değeri, öğrenci numarasından türetilen 12405 değerine modlanır
 *    - Bu, anahtarı daha küçük bir aralığa indirger
 *    - 12405 seçimi, öğrenci numarasının (1240505902) ilk 5 basamağından gelir
 *
 * 2. Aşama: sonuç % HASH_TABLO_BOYUTU (101)
 *    - İlk aşamanın sonucu, tablo boyutuna (101) modlanır
 *    - 101 asal sayı seçilmiştir çünkü asal tablo boyutları hash dağılımını iyileştirir
 *    - Asal sayılar, hash fonksiyonundaki periyodik paternleri kırar
 *
 * Neden İki Aşamalı Mod?
 * - Tek mod ile (k % 101), küçük anahtarlar tablonun başında yoğunlaşır
 * - İki aşamalı mod, anahtarların tabloya daha homojen dağılmasını sağlar
 * - Özellikle ardışık anahtarlar için daha iyi dağılım elde edilir
 *
 * Örnek Hesaplama (anahtar = 5001):
 *   1. Aşama: 5001 % 12405 = 5001
 *   2. Aşama: 5001 % 101 = 58
 *   Sonuç: Paket 5001, tabloda indeks 58'e yerleştirilir
 *
 * @param anahtar Paket kimliği (integer)
 * @return Hash tablosundaki indeks (0-100 arası)
 * ============================================================================
 */
unsigned int hash_fonksiyonu(int anahtar) {
    /* Negatif anahtarları pozitif yap */
    unsigned int pozitif_anahtar = (anahtar < 0) ? (unsigned int)(-anahtar) 
                                                  : (unsigned int)anahtar;

    /* İki aşamalı modular hash fonksiyonu */
    unsigned int ilk_mod = pozitif_anahtar % PRIME_MOD;
    unsigned int ikinci_mod = ilk_mod % HASH_TABLO_BOYUTU;

    return ikinci_mod;
}


/* ============================================================================
 * hash_tablosu_olustur - Yeni bir hash tablosu oluşturur
 * ============================================================================
 * 1. HashTablosu yapısı için bellek tahsisi (malloc)
 * 2. HashGirdi dizisi için bellek tahsisi (malloc)
 * 3. Tüm girdiler DURUM_BOS olarak başlatılır
 * 4. Her tahsis sonrası bellek izleme çıktısı yazdırılır
 * ============================================================================
 */
HashTablosu* hash_tablosu_olustur(void) {
    /* HashTablosu yapısı için bellek tahsisi */
    HashTablosu *ht = (HashTablosu*)malloc(sizeof(HashTablosu));
    if (ht == NULL) {
        printf("[HATA] Hash tablosu yapisi icin bellek tahsisi basarisiz!\n");
        return NULL;
    }
    /* Bellek izleme */
    DEBUG_MALLOC("HashTablosu yapisi malloc", ht, sizeof(HashTablosu));
    printf("  -> Icerik: eleman_sayisi=0, boyut=%d\n\n", HASH_TABLO_BOYUTU);

    /* Hash girdileri dizisi için bellek tahsisi */
    ht->girdiler = (HashGirdi*)malloc(HASH_TABLO_BOYUTU * sizeof(HashGirdi));
    if (ht->girdiler == NULL) {
        printf("[HATA] Hash girdileri dizisi icin bellek tahsisi basarisiz!\n");
        free(ht);
        return NULL;
    }
    /* Bellek izleme */
    DEBUG_MALLOC("HashGirdi dizisi malloc", ht->girdiler, 
                 HASH_TABLO_BOYUTU * sizeof(HashGirdi));
    printf("  -> Icerik: %d HashGirdi icin alan ayrildi\n\n", HASH_TABLO_BOYUTU);

    /* Tüm girdileri boş olarak başlat */
    for (int i = 0; i < HASH_TABLO_BOYUTU; i++) {
        ht->girdiler[i].durum = DURUM_BOS;
        memset(&ht->girdiler[i].paket, 0, sizeof(Paket));
    }

    ht->eleman_sayisi = 0;
    ht->boyut = HASH_TABLO_BOYUTU;

    printf("[BILGI] Hash tablosu basariyla olusturuldu. (Boyut: %d)\n", 
           HASH_TABLO_BOYUTU);
    return ht;
}


/* ============================================================================
 * hash_tablosu_yok_et - Hash tablosunu bellekten temizler
 * ============================================================================
 * ============================================================================
 */
void hash_tablosu_yok_et(HashTablosu *ht) {
    if (ht == NULL) return;

    printf("[SERBEST] Hash girdileri dizisi adres=%p serbest birakildi\n", 
           ht->girdiler);
    free(ht->girdiler);

    printf("[SERBEST] HashTablosu yapisi adres=%p serbest birakildi\n", ht);
    free(ht);

    printf("[BILGI] Hash tablosu temizlendi.\n");
}


/* ============================================================================
 * paket_ekle - Hash tablosuna yeni paket ekler (Linear Probing)
 * ============================================================================
 * 
 * Linear Probing Adımları:
 * 1. hash_fonksiyonu(anahtar) ile başlangıç indeksi hesapla
 * 2. Tablo[indeks].durum kontrol et:
 *    a) DURUM_BOS     -> Paketi buraya yerleştir, BİTİT
 *    b) DURUM_SILINDI -> Paketi buraya yerleştir, BİTİT
 *    c) DURUM_DOLU ve aynı anahtar -> HATA (duplikat)
 *    d) DURUM_DOLU ve farklı anahtar -> indeks = (indeks + 1) % BOYUT, Adım 2'ye dön
 *
 * Cluster (Kümeleşme) Analizi:
 * Linear Probing, aynı hücrenin etrafında kümeleşmeye neden olur.
 * Bu, arama performansını düşürür. Doluluk oranı %70'i geçtiğinde
 * performans belirgin şekilde azalır.
 * ============================================================================
 */
int paket_ekle(HashTablosu *ht, Paket p) {
    if (ht == NULL) {
        printf("[HATA] Gecersiz hash tablosu!\n");
        return 0;
    }

    /* Tablo doluluk kontrolü */
    if (ht->eleman_sayisi >= ht->boyut) {
        printf("[HATA] Hash tablosu dolu! (Doluluk: %d/%d)\n",
               ht->eleman_sayisi, ht->boyut);
        return 0;
    }

    /* Hash fonksiyonu ile başlangıç indeksini hesapla */
    unsigned int indeks = hash_fonksiyonu(p.id);
    unsigned int ilk_indeks = indeks;
    int probing_adimi = 0; /* Kaç adım linear probing yapıldığını sayar */

    printf("[HASH EKLEME] Paket ID: %d, Hash indeksi: %u\n", p.id, indeks);

    /* Linear Probing döngüsü */
    while (1) {
        if (ht->girdiler[indeks].durum == DURUM_BOS) {
            /* Boş hücre bulundu - paketi yerleştir */
            ht->girdiler[indeks].paket = p;
            ht->girdiler[indeks].durum = DURUM_DOLU;
            ht->eleman_sayisi++;

            if (probing_adimi > 0) {
                printf("  -> Linear Probing: %d adim kaydirildi (bos hucre)\n", 
                       probing_adimi);
            }
            printf("  -> Paket indeks %u'e yerlestirildi.\n", indeks);
            return 1;

        } else if (ht->girdiler[indeks].durum == DURUM_SILINDI) {
            /* Silinmiş hücre bulundu - paketi yerleştir */
            ht->girdiler[indeks].paket = p;
            ht->girdiler[indeks].durum = DURUM_DOLU;
            ht->eleman_sayisi++;

            printf("  -> Linear Probing: %d adim (silinmis hucre yeniden kullanildi)\n",
                   probing_adimi);
            return 1;

        } else if (ht->girdiler[indeks].durum == DURUM_DOLU) {
            /* Dolu hücre - kontrol et */
            if (ht->girdiler[indeks].paket.id == p.id) {
                printf("[HATA] Paket ID %d zaten mevcut!\n", p.id);
                return 0;
            }
            /* Çakışma - bir sonraki hücreye geç */
            probing_adimi++;
            indeks = (indeks + 1) % ht->boyut;

            /* Tüm tabloyu taradıysak tablo doludur */
            if (indeks == ilk_indeks) {
                printf("[HATA] Tablo tamamen dolu!\n");
                return 0;
            }
        }
    }
}


/* ============================================================================
 * paket_sorgula - Hash tablosundan paket arar (Linear Probing)
 * ============================================================================
 * 
 * Arama Algoritması:
 * 1. hash_fonksiyonu(anahtar) ile başlangıç indeksi hesapla
 * 2. Tablo[indeks] kontrol et:
 *    a) DURUM_BOS -> Paket bulunamadı, NULL döndür
 *    b) DURUM_DOLU ve anahtar eşleşiyor -> Paket bulundu, pointer döndür
 *    c) DURUM_DOLU ve anahtar eşleşmiyor -> indeks++, devam et
 *    d) DURUM_SILINDI -> indeks++, devam et (burada paket olamaz ama
 *       sonraki hücrelerde olabilir - zincir kopmaması için)
 *
 * Neden SILINDI hücrelerinde aramaya devam edilir?
 * Linear Probing'de, bir eleman silindiğinde hemen ardından eklenen
 * başka bir eleman silinen hücrenin "doğal" konumu olmayabilir.
 * Arama SILINDI'da durdurulursa, bu sonraki elemanlar bulunamaz.
 * ============================================================================
 */
Paket* paket_sorgula(HashTablosu *ht, int paket_id) {
    if (ht == NULL) {
        printf("[HATA] Gecersiz hash tablosu!\n");
        return NULL;
    }

    /* Hash fonksiyonu ile başlangıç indeksini hesapla */
    unsigned int indeks = hash_fonksiyonu(paket_id);
    unsigned int ilk_indeks = indeks;
    int adim_sayisi = 0;

    printf("[HASH SORGU] Paket ID: %d, Baslangic indeksi: %u\n", 
           paket_id, indeks);

    /* Linear Probing ile arama */
    while (1) {
        adim_sayisi++;

        if (ht->girdiler[indeks].durum == DURUM_BOS) {
            /* Boş hücreye ulaşıldı - paket yok */
            printf("  -> Sonuc: Paket bulunamadi (%d adim)\n", adim_sayisi);
            return NULL;

        } else if (ht->girdiler[indeks].durum == DURUM_DOLU &&
                   ht->girdiler[indeks].paket.id == paket_id) {
            /* Paket bulundu */
            printf("  -> Sonuc: Paket bulundu! (indeks: %u, %d adim)\n",
                   indeks, adim_sayisi);
            return &ht->girdiler[indeks].paket;

        } else {
            /* Çakışma veya silinmiş hücre - bir sonraki hücreye geç */
            indeks = (indeks + 1) % ht->boyut;

            if (indeks == ilk_indeks) {
                /* Tüm tablo tarandı */
                printf("  -> Sonuc: Paket bulunamadi (tum tablo tarandi)\n");
                return NULL;
            }
        }
    }
}


/* ============================================================================
 * paket_sil - Hash tablosundan paket siler (Lazy Deletion)
 * ============================================================================
 * 
 * Lazy Deletion (Gecikmeli Silme) Neden Kullanılır?
 * 
 * Açık adreslemede bir eleman doğrudan silinirse (DURUM_BOS yapılırsa),
 * Linear Probing ile o konumdan sonraki konumlara yerleştirilmiş elemanların
 * arama zinciri kopar. Bu durumda, asıl var olan paketlar bulunamaz.
 *
 * Çözüm: Silinen hücre DURUM_SILINDI (tombstone) olarak işaretlenir.
 * - Ekleme: SILINDI hücreler yeni eleman için yeniden kullanılabilir
 * - Arama: SILINDI hücreler atlanır, arama devam eder
 * - Dezavantaj: Tombstone'lar tabloyu gereksiz yere doldurur
 * ============================================================================
 */
int paket_sil(HashTablosu *ht, int paket_id) {
    if (ht == NULL) {
        printf("[HATA] Gecersiz hash tablosu!\n");
        return 0;
    }

    unsigned int indeks = hash_fonksiyonu(paket_id);
    unsigned int ilk_indeks = indeks;

    while (1) {
        if (ht->girdiler[indeks].durum == DURUM_BOS) {
            printf("[HATA] Silinecek paket bulunamadi (ID: %d)\n", paket_id);
            return 0;

        } else if (ht->girdiler[indeks].durum == DURUM_DOLU &&
                   ht->girdiler[indeks].paket.id == paket_id) {
            /* Paket bulundu - lazy deletion uygula */
            ht->girdiler[indeks].durum = DURUM_SILINDI;
            ht->eleman_sayisi--;

            printf("[SILME] Paket ID %d silindi (indeks: %u, tombstone)\n",
                   paket_id, indeks);
            return 1;

        } else {
            indeks = (indeks + 1) % ht->boyut;
            if (indeks == ilk_indeks) {
                printf("[HATA] Paket bulunamadi (ID: %d)\n", paket_id);
                return 0;
            }
        }
    }
}


/* ============================================================================
 * hash_tablosu_yazdir - Hash tablosunun tamamını ekrana yazdırır
 * ============================================================================
 * Her hücrenin indeks numarası, durumu ve içerdiği paket bilgileri
 * gösterilir. Boş ve silinmiş hücreler de listelenir.
 * ============================================================================
 */
void hash_tablosu_yazdir(const HashTablosu *ht) {
    if (ht == NULL) {
        printf("[HATA] Gosterilecek hash tablosu yok!\n");
        return;
    }

    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         HASH TABLOSU DURUMU                                ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Eleman Sayisi: %-5d  |  Boyut: %-5d  |  Doluluk: %5.1f%%           ║\n",
           ht->eleman_sayisi, ht->boyut,
           (float)ht->eleman_sayisi / ht->boyut * 100.0f);
    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");

    printf("\n  Indeks | Durum      | Paket ID | Aciklama              | Konum Pointer\n");
    printf("  -------+------------+----------+----------------------+---------------\n");

    for (int i = 0; i < ht->boyut; i++) {
        if (ht->girdiler[i].durum == DURUM_BOS) {
            /* Sadece dolu ve silinmiş hücreleri göster (tablo çok büyükse) */
            continue;
        }

        const char *durum_str;
        if (ht->girdiler[i].durum == DURUM_DOLU) {
            durum_str = "DOLU      ";
        } else {
            durum_str = "SILINDI   ";
        }

        if (ht->girdiler[i].durum == DURUM_DOLU) {
            Paket *p = &ht->girdiler[i].paket;
            printf("  %-6d | %-10s | %-8d | %-20s | %p\n",
                   i, durum_str, p->id, p->aciklama, 
                   (void*)p->current_location);
        } else {
            printf("  %-6d | %-10s | %-8s | %-20s | %s\n",
                   i, durum_str, "-", "-", "-");
        }
    }
    printf("\n  (Toplam %d bos hucre gosterilmedi)\n\n", 
           ht->boyut - ht->eleman_sayisi);
}


/* ============================================================================
 * hash_tablosu_istatistik - Hash tablosu performans istatistiklerini yazdırır
 * ============================================================================
 * ============================================================================
 */
void hash_tablosu_istatistik(const HashTablosu *ht) {
    if (ht == NULL) return;

    float doluluk = (float)ht->eleman_sayisi / ht->boyut * 100.0f;

    /* En uzun küme (cluster) uzunluğunu hesapla */
    int max_cluster = 0, current_cluster = 0;
    for (int i = 0; i < ht->boyut; i++) {
        if (ht->girdiler[i].durum == DURUM_DOLU || 
            ht->girdiler[i].durum == DURUM_SILINDI) {
            current_cluster++;
            if (current_cluster > max_cluster) max_cluster = current_cluster;
        } else {
            current_cluster = 0;
        }
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              HASH TABLOSU ISTATISTIKLERI                     ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Tablo Boyutu       : %-37d ║\n", ht->boyut);
    printf("║ Mevcut Eleman      : %-37d ║\n", ht->eleman_sayisi);
    printf("║ Doluluk Orani      : %-36.1f%% ║\n", doluluk);
    printf("║ Bos Hucre          : %-37d ║\n", ht->boyut - ht->eleman_sayisi);
    printf("║ En Uzun Cluster    : %-37d ║\n", max_cluster);
    printf("║ Hash Fonksiyonu    : Tip A (Modular, PRIME_MOD=%d)           ║\n", PRIME_MOD);
    printf("║ Cakisma Cozumu     : Acik Adresleme - Lineer Probing         ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");
}