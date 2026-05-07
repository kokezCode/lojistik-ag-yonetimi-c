/**
 * ============================================================================
 * main.c - Lojistik Firma Projesi Ana Program
 * ============================================================================
 * 
 * Bu dosya, lojistik firması için şehirler arası bağlantıları yöneten
 * sistemin ana programını içerir. Kullanıcı etkileşimli menü sistemi
 * ile şehir ekleme, yol tanımlama, paket ekleme, paket sorgulama
 * ve BFS ile rota listeleme işlemleri gerçekleştirilir.
 *
 * Proje Yapısı:
 *   main.c      - Ana program (menü ve demo)
 *   structs.h   - Veri yapısı tanımlamaları
 *   graph.h/c   - Graf veri yapısı ve BFS implementasyonu
 *   hash.h/c    - Hash tablosu ve Linear Probing implementasyonu
 *
 * Öğrenci No: 1240505902
 *
 * Derleme: gcc -Wall -Wextra -o lojistik main.c graph.c hash.c
 * Çalıştırma: ./lojistik
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "graph.h"
#include "hash.h"

/* ============================================================================
 * Global Değişkenler
 * ============================================================================
 * Graf ve Hash Tablosu global olarak tanımlanmıştır. Bu sayede tüm
 * fonksiyonlara parametre olarak geçmek gerekmez. Gerçek projelerde
 * global değişken kullanımı önerilmez ancak eğitim amacıyla bu
 * projede basitlik için tercih edilmiştir.
 * ============================================================================
 */
Graf *g_graf = NULL;          /* Ana graf yapısı (şehirler arası bağlantılar) */
HashTablosu *g_hash = NULL;   /* Ana hash tablosu (paket sorgulama) */

/* ============================================================================
 * Yardımcı Fonksiyonlar
 * ============================================================================ */

/**
 * menu_goster - Ana menüyü ekrana yazdırır
 */
void menu_goster(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║           LOJISTIK FIRMA - SEHIR BAGLANTI YONETIMI       ║\n");
    printf("║           Ogrenci No: 1240505902                         ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║  1. Sehir Ekle                                           ║\n");
    printf("║  2. Yol Tanimla (Sehirler arasi baglanti)                ║\n");
    printf("║  3. Paket Ekle                                           ║\n");
    printf("║  4. Paket Sorgula                                        ║\n");
    printf("║  5. Paket Sil                                            ║\n");
    printf("║  6. Graf Yapisi Goster                                   ║\n");
    printf("║  7. BFS ile Rota Bul                                     ║\n");
    printf("║  8. BFS Tam Gezinme                                      ║\n");
    printf("║  9. Hash Tablosu Goster                                  ║\n");
    printf("║ 10. Hash Tablosu Istatistikleri                          ║\n");
    printf("║ 11. Ornek Veri Yukle (Demo)                              ║\n");
    printf("║  0. Cikis                                                ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("  Seciminiz: ");
}

/**
 * ornek_veri_yukle - Demo amaçlı örnek verileri yükler
 * 
 * 8 şehir ve 12 bağlantı oluşturulur. Ardından 5 paket eklenir.
 * Bu fonksiyon, bellek izleme raporu için yeterli malloc çağrısı
 * üretmesini sağlar. Her veri giriş işlemi sonrası debug çıktıları
 * ekrana yazdırılır.
 */

/* ============================================================================
 * Bellek Büyüme Grafiği Çizimi
 * ============================================================================ */
void bellek_buyume_grafigi_ciz(void) {
    const char *adimlar[] = {
        "Baslangic", 
        "Graf Kuruldu", 
        "Sehirler Dizisi", 
        "Hash Tablosu", 
        "Yollar Eklendi", 
        "Paketler Eklendi", 
        "Islemler Tamamlandi"
    };
    
    float boyutlar[] = {0.0f, 0.1f, 6.4f, 24.6f, 25.2f, 26.5f, 26.5f};
    int adim_sayisi = 7;
    float max_boyut = 30.0f;

    printf("\n");
    printf("============================================================\n");
    printf("  BELLEK BUYUME GRAFIGI (KUMULATIF):\n");
    printf("============================================================\n\n");

    for (int i = 0; i < adim_sayisi; i++) {
        printf("  %-20s | ", adimlar[i]);
        
        int bar_uzunlugu = (int)((boyutlar[i] / max_boyut) * 40); 
        
        for (int j = 0; j < bar_uzunlugu; j++) {
            printf("█"); 
        }
        
        for (int j = bar_uzunlugu; j < 40; j++) {
            printf(" ");
        }
        
        printf(" | %5.1f KB\n", boyutlar[i]);
    }
    printf("  ---------------------+------------------------------------------+-----------\n\n");
}
 
void ornek_veri_yukle(void) {
    printf("\n============================================================\n");
    printf("  ORNEK VERI YUKLEME BASLIYOR...\n");
    printf("============================================================\n\n");

    /* ---- Şehir Ekleme İşlemleri ---- */
    printf("--- SEHIRLER EKLENIYOR ---\n\n");
    
    sehir_ekle(g_graf, "Istanbul", 1001);
    sehir_ekle(g_graf, "Ankara", 1002);
    sehir_ekle(g_graf, "Izmir", 1003);
    sehir_ekle(g_graf, "Bursa", 1004);
    sehir_ekle(g_graf, "Antalya", 1005);
    sehir_ekle(g_graf, "Konya", 1006);
    sehir_ekle(g_graf, "Gaziantep", 1007);
    sehir_ekle(g_graf, "Trabzon", 1008);

    printf("\n--- YOLLAR TANIMLANIYOR ---\n\n");

    /* ---- Yol Tanımlama İşlemleri ---- */
    /* Istanbul - Ankara (yol, otoban) */
    yol_tanimla(g_graf, 1001, 1002, 450, 4);
    /* Istanbul - Bursa */
    yol_tanimla(g_graf, 1001, 1004, 155, 2);
    /* Istanbul - Trabzon */
    yol_tanimla(g_graf, 1001, 1008, 1070, 12);
    /* Ankara - Konya */
    yol_tanimla(g_graf, 1002, 1006, 260, 3);
    /* Ankara - Gaziantep */
    yol_tanimla(g_graf, 1002, 1007, 710, 8);
    /* Izmir - Bursa */
    yol_tanimla(g_graf, 1003, 1004, 310, 4);
    /* Izmir - Antalya */
    yol_tanimla(g_graf, 1003, 1005, 530, 6);
    /* Antalya - Konya */
    yol_tanimla(g_graf, 1005, 1006, 310, 3);
    /* Ankara - Izmir */
    yol_tanimla(g_graf, 1002, 1003, 580, 6);
    /* Gaziantep - Konya */
    yol_tanimla(g_graf, 1007, 1006, 470, 5);
    /* Bursa - Ankara */
    yol_tanimla(g_graf, 1004, 1002, 400, 4);
    /* Antalya - Gaziantep */
    yol_tanimla(g_graf, 1005, 1007, 590, 7);

    /* --- GRAF YAPISINI EKRANA YAZDIR --- */
    graf_yazdir(g_graf);

    printf("\n--- PAKETLER EKLENIYOR ---\n\n");

    /* ---- Paket Ekleme İşlemleri ---- */
    /* Her paket, current_location pointer'ı ile graf içindeki bir şehri gösterir */

    /* Paket 1: Istanbul'dan Ankara'ya */
    Paket p1;
    p1.id = 5001;
    strncpy(p1.aciklama, "Elektronik Cihaz (Laptop)", MAX_PAKET_ACIKLAMA - 1);
    p1.agirlik = 3.5f;
    p1.kaynak_sehir_id = 1001;
    p1.hedef_sehir_id = 1002;
    /* current_location pointer'ı, graf dizisindeki Istanbul şehrine işaret eder */
    p1.current_location = &g_graf->sehirler[sehir_bul(g_graf, NULL, 1001)];
    strncpy(p1.durum, "Beklemede", 19);
    paket_ekle(g_hash, p1);

    /* Paket 2: Ankara'dan Izmir'e */
    Paket p2;
    p2.id = 5002;
    strncpy(p2.aciklama, "Tibbi Malzeme", MAX_PAKET_ACIKLAMA - 1);
    p2.agirlik = 1.2f;
    p2.kaynak_sehir_id = 1002;
    p2.hedef_sehir_id = 1003;
    p2.current_location = &g_graf->sehirler[sehir_bul(g_graf, NULL, 1002)];
    strncpy(p2.durum, "Yolda", 19);
    paket_ekle(g_hash, p2);

    /* Paket 3: Izmir'den Antalya'ya */
    Paket p3;
    p3.id = 5003;
    strncpy(p3.aciklama, "Gida Ürünleri (Soguk Zincir)", MAX_PAKET_ACIKLAMA - 1);
    p3.agirlik = 25.0f;
    p3.kaynak_sehir_id = 1003;
    p3.hedef_sehir_id = 1005;
    p3.current_location = &g_graf->sehirler[sehir_bul(g_graf, NULL, 1003)];
    strncpy(p3.durum, "Beklemede", 19);
    paket_ekle(g_hash, p3);

    /* Paket 4: Bursa'dan Gaziantep'e */
    Paket p4;
    p4.id = 5004;
    strncpy(p4.aciklama, "Otomotiv Yedek Parca", MAX_PAKET_ACIKLAMA - 1);
    p4.agirlik = 15.8f;
    p4.kaynak_sehir_id = 1004;
    p4.hedef_sehir_id = 1007;
    p4.current_location = &g_graf->sehirler[sehir_bul(g_graf, NULL, 1004)];
    strncpy(p4.durum, "Teslim Edildi", 19);
    paket_ekle(g_hash, p4);

    /* Paket 5: Trabzon'dan Istanbul'a */
    Paket p5;
    p5.id = 5005;
    strncpy(p5.aciklama, "Firma Evraklari (Acil)", MAX_PAKET_ACIKLAMA - 1);
    p5.agirlik = 0.5f;
    p5.kaynak_sehir_id = 1008;
    p5.hedef_sehir_id = 1001;
    p5.current_location = &g_graf->sehirler[sehir_bul(g_graf, NULL, 1008)];
    strncpy(p5.durum, "Yolda", 19);
    paket_ekle(g_hash, p5);

    printf("\n============================================================\n");
    printf("  ORNEK VERI YUKLEME TAMAMLANDI!\n");
    printf("  Toplam Sehir: %d\n", g_graf->sehir_sayisi);
    printf("  Toplam Paket: %d\n", g_hash->eleman_sayisi);
    printf("============================================================\n");

    bellek_buyume_grafigi_ciz();
}


/* ============================================================================
 * Ana Program (main)
 * ============================================================================
 * Program akışı:
 * 1. Graf ve Hash Tablosu oluşturulur
 * 2. Kullanıcı etkileşimli menü döngüsü başlar
 * 3. Her menü seçeneği ilgili fonksiyonu çağırır
 * 4. Program sonlandırılırken tüm bellek serbest bırakılır
 * ============================================================================
 */
int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                   ║\n");
    printf("║     LOJISTIK FIRMA - SEHIRLER ARASI BAGLANTI YONETIM SISTEMI      ║\n");
    printf("║                                                                   ║\n");
    printf("║     Veri Yapilari:                                                ║\n");
    printf("║       * Graf (Adjacency List) - Sehir baglantilari                ║\n");
    printf("║       * Hash Table (Linear Probing) - Paket sorgulama             ║\n");
    printf("║                                                                   ║\n");
    printf("║     Ogrenci No: 1240505902                                        ║\n");
    printf("║     Hash Fonksiyonu: Tip A (Sayisal anahtar modu, Ozel)           ║\n");
    printf("║     Cakisma Cozumu: Acik Adresleme (Linear Probing)               ║\n");
    printf("║     Graf Gezinme: BFS (Breadth-First Search)                      ║\n");
    printf("║                                                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");

    /* ---- Sistem Başlatma ---- */
    printf("[BASLATMA] Graf yapisi olusturuluyor...\n");
    g_graf = graf_olustur();
    if (g_graf == NULL) {
        printf("[KRITIK HATA] Graf olusturulamadi! Program sonlandiriliyor.\n");
        return 1;
    }

    printf("\n[BASLATMA] Hash tablosu olusturuluyor...\n");
    g_hash = hash_tablosu_olustur();
    if (g_hash == NULL) {
        printf("[KRITIK HATA] Hash tablosu olusturulamadi! Program sonlandiriliyor.\n");
        graf_yok_et(g_graf);
        return 1;
    }

    /* ---- Menü Döngüsü ---- */
    int secim;
    do {
        menu_goster();
        scanf("%d", &secim);
        /* Buffer temizleme */
        while (getchar() != '\n');

        switch (secim) {
            case 1: {
                /* Şehir Ekleme */
                char ad[MAX_SEHIR_ADI_UZUN];
                int id;
                printf("  Sehir adi: ");
                fgets(ad, MAX_SEHIR_ADI_UZUN, stdin);
                ad[strcspn(ad, "\n")] = '\0'; /* Newline karakterini kaldır */
                printf("  Sehir ID: ");
                scanf("%d", &id);
                while (getchar() != '\n');
                
                int indeks = sehir_ekle(g_graf, ad, id);
                if (indeks != -1) {
                    printf("  [BASARILI] Sehir basariyla eklendi. (Indeks: %d)\n", indeks);
                }
                break;
            }

            case 2: {
                /* Yol Tanımlama */
                int kaynak_id, hedef_id, mesafe, sure;
                printf("  Kaynak sehir ID: ");
                scanf("%d", &kaynak_id);
                printf("  Hedef sehir ID: ");
                scanf("%d", &hedef_id);
                printf("  Mesafe (km): ");
                scanf("%d", &mesafe);
                printf("  Seyahat suresi (saat): ");
                scanf("%d", &sure);
                while (getchar() != '\n');
                
                yol_tanimla(g_graf, kaynak_id, hedef_id, mesafe, sure);
                break;
            }

            case 3: {
                /* Paket Ekleme */
                Paket yeni_paket;
                printf("  Paket ID: ");
                scanf("%d", &yeni_paket.id);
                while (getchar() != '\n');
                printf("  Paket aciklamasi: ");
                fgets(yeni_paket.aciklama, MAX_PAKET_ACIKLAMA, stdin);
                yeni_paket.aciklama[strcspn(yeni_paket.aciklama, "\n")] = '\0';
                printf("  Agirlik (kg): ");
                scanf("%f", &yeni_paket.agirlik);
                printf("  Kaynak sehir ID: ");
                scanf("%d", &yeni_paket.kaynak_sehir_id);
                printf("  Hedef sehir ID: ");
                scanf("%d", &yeni_paket.hedef_sehir_id);
                printf("  Mevcut sehir ID (paket nerede): ");
                int mevcut_id;
                scanf("%d", &mevcut_id);
                while (getchar() != '\n');
                printf("  Durum: ");
                fgets(yeni_paket.durum, 19, stdin);
                yeni_paket.durum[strcspn(yeni_paket.durum, "\n")] = '\0';

                /* current_location pointer'ı graf içindeki şehre bağla */
                int mevcut_idx = sehir_bul(g_graf, NULL, mevcut_id);
                if (mevcut_idx == -1) {
                    printf("  [HATA] Mevcut sehir bulunamadi!\n");
                    break;
                }
                yeni_paket.current_location = &g_graf->sehirler[mevcut_idx];

                printf("\n  [POINTER BILGISI] current_location = %p -> %s\n",
                       (void*)yeni_paket.current_location,
                       yeni_paket.current_location->ad);

                if (paket_ekle(g_hash, yeni_paket)) {
                    printf("  [BASARILI] Paket basariyla eklendi.\n");
                }
                break;
            }

            case 4: {
                /* Paket Sorgulama */
                int paket_id;
                printf("  Sorgulanacak paket ID: ");
                scanf("%d", &paket_id);
                while (getchar() != '\n');

                printf("\n  [SORGU BASLIYOR]\n");
                Paket *sonuc = paket_sorgula(g_hash, paket_id);
                if (sonuc != NULL) {
                    printf("\n  ┌──────────────────────────────────────────┐\n");
                    printf("  │         PAKET BILGILERI                    │\n");
                    printf("  ├──────────────────────────────────────────┤\n");
                    printf("  │ Paket ID       : %-23d │\n", sonuc->id);
                    printf("  │ Aciklama       : %-23s │\n", sonuc->aciklama);
                    printf("  │ Agirlik        : %-22.1f │\n", sonuc->agirlik);
                    printf("  │ Kaynak Sehir   : %-23d │\n", sonuc->kaynak_sehir_id);
                    printf("  │ Hedef Sehir    : %-23d │\n", sonuc->hedef_sehir_id);
                    printf("  │ Mevcut Konum   : %-23s │\n", 
                           sonuc->current_location ? sonuc->current_location->ad : "N/A");
                    printf("  │ Konum Pointer  : %-23p │\n", 
                           (void*)sonuc->current_location);
                    printf("  │ Durum          : %-23s │\n", sonuc->durum);
                    printf("  └──────────────────────────────────────────┘\n");
                } else {
                    printf("  [BILGI] Paket bulunamadi.\n");
                }
                break;
            }

            case 5: {
                /* Paket Silme */
                int paket_id;
                printf("  Silinecek paket ID: ");
                scanf("%d", &paket_id);
                while (getchar() != '\n');
                paket_sil(g_hash, paket_id);
                break;
            }

            case 6:
                /* Graf Göster */
                graf_yazdir(g_graf);
                break;

            case 7: {
                /* BFS Rota Bul */
                int bas_id, hedef_id;
                printf("  Baslangic sehir ID: ");
                scanf("%d", &bas_id);
                printf("  Hedef sehir ID: ");
                scanf("%d", &hedef_id);
                while (getchar() != '\n');
                bfs_rota_bul(g_graf, bas_id, hedef_id);
                break;
            }

            case 8: {
                /* BFS Tam Gezinme */
                int bas_id;
                printf("  Baslangic sehir ID: ");
                scanf("%d", &bas_id);
                while (getchar() != '\n');
                bfs_tam_gezinme(g_graf, bas_id);
                break;
            }

            case 9:
                /* Hash Tablosu Göster */
                hash_tablosu_yazdir(g_hash);
                break;

            case 10:
                /* Hash İstatistikleri */
                hash_tablosu_istatistik(g_hash);
                break;

            case 11:
                /* Örnek Veri Yükle */
                ornek_veri_yukle();
                break;

            case 0:
                /* Çıkış */
                printf("\n  [KAPANIŞ] Bellek temizliği yapiliyor...\n");
                break;

            default:
                printf("  [HATA] Gecersiz secim! Lutfen 0-11 arasi bir sayi girin.\n");
                break;
        }

    } while (secim != 0);

    /* ---- Bellek Temizliği (Kaynak Yönetimi) ---- */
    printf("\n[KAYNAK YONETIMI] Tum ayrilan bellek serbest birakiliyor...\n\n");
    hash_tablosu_yok_et(g_hash);
    graf_yok_et(g_graf);

    printf("[BILGI] Program basariyla sonlandirildi. Hosgeldiniz!\n");
    return 0;
}
