/**
 * ============================================================================
 * graph.c - Graf Veri Yapısı Fonksiyon Gerçekleştirimleri
 * ============================================================================
 * 
 * Bu dosya, şehirler arası bağlantıları yöneten graf yapısının tüm
 * fonksiyonlarını içerir. Komşuluk listesi (Adjacency List) kullanılarak
 * graf temsil edilir. BFS algoritması ile gezinme desteği sağlanır.
 *
 * Öğrenci No: 1240505902
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

/* ============================================================================
 * Bellek İzleme (Memory Tracking) Debug Fonksiyonu
 * ============================================================================
 * Her malloc/realloc/calloc çağrısından sonra bu fonksiyon çağrılarak
 * ayrılan bellek adresi (%p formatında) ve içeriği ekrana yazdırılır.
 * Bu, "Bellek İzleme Raporu" hazırlamak için kullanılır.
 * ============================================================================
 */
void debug_bellek_izleme(const char *etiket, void *ptr, size_t boyut, 
                          const char *dosya, int satir) {
    /* Tek satirlik, hizalanmis profesyonel log formati */
    printf("[MEM-DEBUG] %-38s | Adres: %-10p | Boyut: %-4lu Bayt | %s:%d\n", 
           etiket, ptr, (unsigned long)boyut, dosya, satir);
}

/* Debug makrosu - satır ve dosya bilgilerini otomatik ekler */
#define DEBUG_MALLOC(etiket, ptr, boyut) \
    debug_bellek_izleme(etiket, ptr, boyut, __FILE__, __LINE__)


/* ============================================================================
 * graf_olustur - Yeni bir graf yapısı oluşturur
 * ============================================================================
 * 1. Graf yapısı için bellek tahsisi yapılır (malloc)
 * 2. Şehirler dizisi için bellek tahsisi yapılır (malloc)
 * 3. Her şehrin başlangıç değerleri sıfırlanır
 * 4. Her tahsis sonrası bellek izleme çıktısı yazdırılır
 * ============================================================================
 */
Graf* graf_olustur(void) {
    /* Graf yapısı için bellek tahsisi */
    Graf *g = (Graf*)malloc(sizeof(Graf));
    if (g == NULL) {
        printf("[HATA] Graf yapisi icin bellek tahsisi basarisiz!\n");
        return NULL;
    }
    /* Bellek izleme: Graf yapısının adresini ve içeriğini yazdır */
    DEBUG_MALLOC("Graf yapisi malloc", g, sizeof(Graf));
    printf("  -> Icerik: sehir_sayisi=0, kapasite=%d\n\n", MAX_SEHIR_SAYISI);

    /* Başlangıç değerlerini ata */
    g->sehir_sayisi = 0;
    g->kapasite = MAX_SEHIR_SAYISI;

    /* Şehirler dizisi için bellek tahsisi */
    g->sehirler = (SehirDugumu*)malloc(MAX_SEHIR_SAYISI * sizeof(SehirDugumu));
    if (g->sehirler == NULL) {
        printf("[HATA] Sehirler dizisi icin bellek tahsisi basarisiz!\n");
        free(g);
        return NULL;
    }
    /* Bellek izleme: Şehirler dizisinin adresini yazdır */
    DEBUG_MALLOC("Sehirler dizisi malloc", g->sehirler, 
                 MAX_SEHIR_SAYISI * sizeof(SehirDugumu));
    printf("  -> Icerik: %d SehirDugumu icin alan ayrildi\n\n", MAX_SEHIR_SAYISI);

    /* Tüm şehir düğümlerini başlangıç değerleriyle sıfırla */
    for (int i = 0; i < MAX_SEHIR_SAYISI; i++) {
        g->sehirler[i].id = 0;
        g->sehirler[i].ad[0] = '\0';
        g->sehirler[i].aktif = 0;
        g->sehirler[i].kenar_listesi = NULL;
    }

    printf("[BILGI] Graf yapisi basariyla olusturuldu.\n");
    return g;
}


/* ============================================================================
 * graf_yok_et - Graf yapısını ve tüm alt yapılarını bellekten temizler
 * ============================================================================
 * Bellek sızıntısını önlemek için:
 * 1. Her şehrin kenar listesindeki tüm KenarDugumları serbest bırakılır
 * 2. Şehirler dizisi serbest bırakılır
 * 3. Graf yapısı kendisi serbest bırakılır
 * ============================================================================
 */
void graf_yok_et(Graf *g) {
    if (g == NULL) return;

    /* Her şehrin kenar listesindeki tüm düğümleri temizle */
    for (int i = 0; i < g->kapasite; i++) {
        if (g->sehirler[i].aktif) {
            KenarDugumu *current = g->sehirler[i].kenar_listesi;
            while (current != NULL) {
                KenarDugumu *temp = current;
                current = current->next;
                printf("[SERBEST] KenarDugumu adres=%p serbest birakildi\n", temp);
                free(temp);
            }
        }
    }

    /* Şehirler dizisini serbest bırak */
    printf("[SERBEST] Sehirler dizisi adres=%p serbest birakildi\n", g->sehirler);
    free(g->sehirler);

    /* Graf yapısını serbest bırak */
    printf("[SERBEST] Graf yapisi adres=%p serbest birakildi\n", g);
    free(g);

    printf("[BILGI] Graf yapisi ve tum alt yapilari temizlendi.\n");
}


/* ============================================================================
 * sehir_ekle - Grafa yeni bir şehir ekler
 * ============================================================================
 * 1. Dizide boş bir yer (aktif==0) aranır
 * 2. Bulunan indekse şehir bilgileri yazılır
 * 3. Şehir aktif olarak işaretlenir
 * 4. Kenar listesi NULL olarak başlatılır
 * 5. Şehir sayısı artırılır
 * 6. Dizi doluysa kapasite artırılır (realloc ile)
 * ============================================================================
 */
int sehir_ekle(Graf *g, const char *ad, int id) {
    if (g == NULL || ad == NULL) {
        printf("[HATA] Gecersiz graf veya sehir adi!\n");
        return -1;
    }

    /* Dizide boş bir yer ara */
    int indeks = -1;
    for (int i = 0; i < g->kapasite; i++) {
        if (!g->sehirler[i].aktif) {
            indeks = i;
            break;
        }
    }

    /* Dizi doluysa kapasiteyi artır */
    if (indeks == -1) {
        int yeni_kapasite = g->kapasite * 2;
        SehirDugumu *yeni_dizi = (SehirDugumu*)realloc(
            g->sehirler, yeni_kapasite * sizeof(SehirDugumu));
        if (yeni_dizi == NULL) {
            printf("[HATA] Dizi genisletme icin bellek tahsisi basarisiz!\n");
            return -1;
        }
        /* Bellek izleme: realloc sonucunu yazdır */
        DEBUG_MALLOC("Sehirler dizisi realloc (kapasite artirma)", yeni_dizi,
                     yeni_kapasite * sizeof(SehirDugumu));

        /* Yeni alanları sıfırla */
        for (int i = g->kapasite; i < yeni_kapasite; i++) {
            yeni_dizi[i].id = 0;
            yeni_dizi[i].ad[0] = '\0';
            yeni_dizi[i].aktif = 0;
            yeni_dizi[i].kenar_listesi = NULL;
        }

        g->sehirler = yeni_dizi;
        g->kapasite = yeni_kapasite;
        indeks = g->sehir_sayisi; /* Yeni kapasitenin sonuna ekle */
    }

    /* Aynı kimliğe sahip şehir var mı kontrol et */
    for (int i = 0; i < g->kapasite; i++) {
        if (g->sehirler[i].aktif && g->sehirler[i].id == id) {
            printf("[HATA] ID %d ile zaten bir sehir mevcut: %s\n", 
                   id, g->sehirler[i].ad);
            return -1;
        }
    }

    /* Şehir bilgilerini ata */
    strncpy(g->sehirler[indeks].ad, ad, MAX_SEHIR_ADI_UZUN - 1);
    g->sehirler[indeks].ad[MAX_SEHIR_ADI_UZUN - 1] = '\0';
    g->sehirler[indeks].id = id;
    g->sehirler[indeks].aktif = 1;
    g->sehirler[indeks].kenar_listesi = NULL;

    g->sehir_sayisi++;

    /* Bellek izleme: Eklenen şehrin bellek adresini göster */
    printf("[SEHIR EKLE] Sehir \"%s\" (ID:%d) indeks=%d\n", ad, id, indeks);
    printf("  -> Bellek Adresi: %p (Graf dizisindeki konumu)\n\n", 
           (void*)&g->sehirler[indeks]);

    return indeks;
}


/* ============================================================================
 * sehir_bul - İsim veya ID ile şehir arar
 * ============================================================================
 * Grafın şehirler dizisinde linear arama yapılır. İsim araması
 * strcmp ile karşılaştırma yaparak gerçekleştirilir.
 * ============================================================================
 */
int sehir_bul(const Graf *g, const char *ad, int id) {
    if (g == NULL) return -1;

    for (int i = 0; i < g->kapasite; i++) {
        if (!g->sehirler[i].aktif) continue;

        /* İsim ile arama (ad NULL değilse) */
        if (ad != NULL && strcmp(g->sehirler[i].ad, ad) == 0) {
            return i;
        }
        /* ID ile arama (ad NULL ise) */
        if (ad == NULL && g->sehirler[i].id == id) {
            return i;
        }
    }
    return -1;
}


/* ============================================================================
 * yol_tanimla - İki şehir arasına çift yönlü bağlantı ekler
 * ============================================================================
 * Her iki yönde de yeni bir KenarDugumu oluşturulur:
 * 1. Kaynak şehrin kenar listesine hedef kenarı eklenir
 * 2. Hedef şehrin kenar listesine kaynak kenarı eklenir
 * Yeni kenar, listenin başına eklenerek O(1) karmaşıklıkta yapılır.
 * Her malloc sonrası bellek izleme çıktısı yazdırılır.
 * ============================================================================
 */
int yol_tanimla(Graf *g, int kaynak_id, int hedef_id, int mesafe, int sure) {
    if (g == NULL) {
        printf("[HATA] Gecersiz graf yapisi!\n");
        return 0;
    }

    /* Kaynak ve hedef şehirleri bul */
    int kaynak_idx = sehir_bul(g, NULL, kaynak_id);
    int hedef_idx = sehir_bul(g, NULL, hedef_id);

    if (kaynak_idx == -1) {
        printf("[HATA] Kaynak sehir (ID:%d) bulunamadi!\n", kaynak_id);
        return 0;
    }
    if (hedef_idx == -1) {
        printf("[HATA] Hedef sehir (ID:%d) bulunamadi!\n", hedef_id);
        return 0;
    }
    if (kaynak_idx == hedef_idx) {
        printf("[HATA] Bir sehire kendi kendine yol tanimlanamaz!\n");
        return 0;
    }

    /* --- Kaynak -> Hedef kenarı oluştur --- */
    KenarDugumu *kenar1 = (KenarDugumu*)malloc(sizeof(KenarDugumu));
    if (kenar1 == NULL) {
        printf("[HATA] Kenar dugumu icin bellek tahsisi basarisiz!\n");
        return 0;
    }
    kenar1->hedef_sehir_indeksi = hedef_idx;
    kenar1->mesafe = mesafe;
    kenar1->seyahat_suresi = sure;
    /* Listenin başına ekle (O(1) ekleme) */
    kenar1->next = g->sehirler[kaynak_idx].kenar_listesi;
    g->sehirler[kaynak_idx].kenar_listesi = kenar1;

    /* Bellek izleme: İlk kenar düğümü */
    DEBUG_MALLOC("KenarDugumu malloc (kaynak->hedef)", kenar1, sizeof(KenarDugumu));
    printf("  -> Icerik: %s -> %s, Mesafe=%dkm, Sure=%dsaat\n",
           g->sehirler[kaynak_idx].ad, g->sehirler[hedef_idx].ad, mesafe, sure);

    /* --- Hedef -> Kaynak kenarı oluştur (çift yönlü) --- */
    KenarDugumu *kenar2 = (KenarDugumu*)malloc(sizeof(KenarDugumu));
    if (kenar2 == NULL) {
        printf("[HATA] Geri donus kenari icin bellek tahsisi basarisiz!\n");
        free(kenar1);
        return 0;
    }
    kenar2->hedef_sehir_indeksi = kaynak_idx;
    kenar2->mesafe = mesafe;
    kenar2->seyahat_suresi = sure;
    /* Listenin başına ekle */
    kenar2->next = g->sehirler[hedef_idx].kenar_listesi;
    g->sehirler[hedef_idx].kenar_listesi = kenar2;

    /* Bellek izleme: İkinci kenar düğümü */
    DEBUG_MALLOC("KenarDugumu malloc (hedef->kaynak)", kenar2, sizeof(KenarDugumu));
    printf("  -> Icerik: %s -> %s, Mesafe=%dkm, Sure=%dsaat\n\n",
           g->sehirler[hedef_idx].ad, g->sehirler[kaynak_idx].ad, mesafe, sure);

    printf("[BILGI] Yol tanimlandi: %s <-> %s (%dkm, %dsaat)\n\n",
           g->sehirler[kaynak_idx].ad, g->sehirler[hedef_idx].ad, mesafe, sure);

    return 1;
}


/* ============================================================================
 * graf_yazdir - Grafın yapısını ekrana yazdırır
 * ============================================================================
 * Her aktif şehir ve onun komşu şehirleri listelenir.
 * Format: Şehir Adı (ID) -> [Komşu1 (Xkm, Ysaat), Komşu2 (...), ...]
 * ============================================================================
 */
void graf_yazdir(const Graf *g) {
    if (g == NULL) {
        printf("[HATA] Gosterilecek graf yok!\n");
        return;
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    GRAF YAPISI Gorsel Gosterim               ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Toplam Sehir: %-44d   ║\n", g->sehir_sayisi);
    printf("║ Kapasite:  %-44d      ║\n", g->kapasite);
    printf("╚══════════════════════════════════════════════════════════════╝\n\n");

    for (int i = 0; i < g->kapasite; i++) {
        if (!g->sehirler[i].aktif) continue;

        printf("  [%s] (ID: %d) adres: %p\n", 
               g->sehirler[i].ad, g->sehirler[i].id, 
               (void*)&g->sehirler[i]);

        /* Kenar listesini gez ve komşuları yazdır */
        KenarDugumu *current = g->sehirler[i].kenar_listesi;
        if (current == NULL) {
            printf("    -> (Baglanti yok)\n");
        }
        while (current != NULL) {
            printf("    |---> [%s] (Mesafe: %dkm, Sure: %dsaat)\n",
                   g->sehirler[current->hedef_sehir_indeksi].ad,
                   current->mesafe, current->seyahat_suresi);
            current = current->next;
        }
        printf("\n");
    }
}


/* ============================================================================
 * bfs_rota_bul - BFS ile iki şehir arasındaki en kısa rota (en az aktarmalı)
 * ============================================================================
 * BFS (Breadth-First Search) algoritması kullanılarak, başlangıç şehrinden
 * hedef şehrine en az aktarmalı rota bulunur.
 *
 * Kullanılan yardımcı veri yapıları:
 * - kuyruk: FIFO (İlk Giren İlk Çıkar) mantığıyla çalışan dizi tabanlı kuyruk
 * - ziyaret_edildi: Her şehrin ziyaret durumunu tutan dizi
 * - parent: Her şehrin BFS ağacındaki ebeveynini tutan dizi
 *
 * Zaman karmaşıklığı: O(V + E) - V: düğüm sayısı, E: kenar sayısı
 * Alan karmaşıklığı:  O(V)     - kuyruk, ziyaret ve parent dizileri
 * ============================================================================
 */
void bfs_rota_bul(const Graf *g, int baslangic_id, int hedef_id) {
    if (g == NULL) {
        printf("[HATA] Gecersiz graf!\n");
        return;
    }

    /* Başlangıç ve hedef şehirlerini bul */
    int bas_idx = sehir_bul(g, NULL, baslangic_id);
    int hedef_idx = sehir_bul(g, NULL, hedef_id);

    if (bas_idx == -1) {
        printf("[HATA] Baslangic sehri (ID:%d) bulunamadi!\n", baslangic_id);
        return;
    }
    if (hedef_idx == -1) {
        printf("[HATA] Hedef sehri (ID:%d) bulunamadi!\n", hedef_id);
        return;
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                 BFS ROTA ARAMA SONUCLARI                     ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║ Baslangic: %-17s (ID: %-20d)      ║\n", 
           g->sehirler[bas_idx].ad, baslangic_id);
    printf("║ Hedef:     %-17s (ID: %-20d)      ║\n", 
           g->sehirler[hedef_idx].ad, hedef_id);
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* Aynı başlangıç ve hedef kontrolü */
    if (bas_idx == hedef_idx) {
        printf("  [BILGI] Baslangic ve hedef ayni sehirdir!\n");
        printf("  Rota: %s\n\n", g->sehirler[bas_idx].ad);
        return;
    }

    /* BFS için yardımcı diziler */
    int *ziyaret_edildi = (int*)calloc(g->kapasite, sizeof(int));
    int *parent = (int*)malloc(g->kapasite * sizeof(int));
    int *kuyruk = (int*)malloc(g->kapasite * sizeof(int));

    /* Bellek izleme */
    DEBUG_MALLOC("BFS ziyaret_edildi dizisi calloc", ziyaret_edildi, 
                 g->kapasite * sizeof(int));
    DEBUG_MALLOC("BFS parent dizisi malloc", parent, g->kapasite * sizeof(int));
    DEBUG_MALLOC("BFS kuyruk dizisi malloc", kuyruk, g->kapasite * sizeof(int));

    if (!ziyaret_edildi || !parent || !kuyruk) {
        printf("[HATA] BFS icin bellek tahsisi basarisiz!\n");
        free(ziyaret_edildi);
        free(parent);
        free(kuyruk);
        return;
    }

    /* Parent dizisini -1 ile başlat (hiçbir düğümün ebeveyni yok) */
    for (int i = 0; i < g->kapasite; i++) {
        parent[i] = -1;
    }

    /* Kuyruk başlangıç değerleri */
    int kuyruk_bas = 0, kuyruk_son = 0;

    /* Başlangıç şehrini kuyruğa ekle */
    kuyruk[kuyruk_son++] = bas_idx;
    ziyaret_edildi[bas_idx] = 1;

    printf("\n  [BFS BASLADI] Kuyruya ilk eleman eklendi: %s\n", 
           g->sehirler[bas_idx].ad);
    printf("  [BFS ADIMLARI]\n");

    int bulundu = 0;  /* Hedef bulundu mu? */

    /* BFS ana döngüsü */
    while (kuyruk_bas < kuyruk_son) {
        /* Kuyruktan bir şehir çıkar */
        int mevcut = kuyruk[kuyruk_bas++];
        printf("    Islenen: %s (kuyruk_bas=%d, kuyruk_son=%d)\n",
               g->sehirler[mevcut].ad, kuyruk_bas, kuyruk_son);

        /* Hedef bulundu mu kontrol et */
        if (mevcut == hedef_idx) {
            bulundu = 1;
            break;
        }

        /* Mevcut şehrin tüm komşularını ziyaret et */
        KenarDugumu *kenar = g->sehirler[mevcut].kenar_listesi;
        while (kenar != NULL) {
            int komsu = kenar->hedef_sehir_indeksi;
            if (!ziyaret_edildi[komsu]) {
                /* Komşuyu ziyaret et ve kuyruğa ekle */
                ziyaret_edildi[komsu] = 1;
                parent[komsu] = mevcut;
                kuyruk[kuyruk_son++] = komsu;
                printf("      -> Kuyruga eklendi: %s (parent: %s)\n",
                       g->sehirler[komsu].ad, g->sehirler[mevcut].ad);
            }
            kenar = kenar->next;
        }
    }

    /* Rota sonucunu yazdır */
    if (bulundu) {
        /* Parent dizisi kullanarak rotayı geri izle */
        printf("\n  [BFS BASARILI] Rota bulundu!\n");
        
        /* Rota dizisini oluştur (geri izleme) */
        int *rota = (int*)malloc(g->kapasite * sizeof(int));
        DEBUG_MALLOC("BFS rota dizisi malloc", rota, g->kapasite * sizeof(int));
        int rota_uzunlugu = 0;
        
        int node = hedef_idx;
        while (node != -1) {
            rota[rota_uzunlugu++] = node;
            node = parent[node];
        }

        /* Rotayı tersine çevir (başlangıç -> hedef sırası) */
        printf("  Rota (%d aktarma):\n    ", rota_uzunlugu - 1);
        for (int i = rota_uzunlugu - 1; i >= 0; i--) {
            printf("%s", g->sehirler[rota[i]].ad);
            if (i > 0) printf(" -> ");
        }
        printf("\n");

        /* Toplam mesafeyi hesapla */
        int toplam_mesafe = 0;
        for (int i = rota_uzunlugu - 1; i > 0; i--) {
            /* rota[i]'den rota[i-1]'e olan kenarı bul */
            KenarDugumu *k = g->sehirler[rota[i]].kenar_listesi;
            while (k != NULL) {
                if (k->hedef_sehir_indeksi == rota[i-1]) {
                    toplam_mesafe += k->mesafe;
                    break;
                }
                k = k->next;
            }
        }
        printf("  Toplam mesafe: %d km\n", toplam_mesafe);
        
        free(rota);
    } else {
        printf("\n  [BFS SONUCU] %s ile %s arasinda rota bulunamadi!\n",
               g->sehirler[bas_idx].ad, g->sehirler[hedef_idx].ad);
    }

    /* BFS yardımcı dizilerini serbest bırak */
    free(ziyaret_edildi);
    free(parent);
    free(kuyruk);
    printf("\n  [TEMIZLIK] BFS icin ayrilan 3 dizi serbest birakildi.\n\n");
}


/* ============================================================================
 * bfs_tam_gezinme - BFS ile bir şehirden ulaşılabilen tüm şehirleri listeler
 * ============================================================================
 * Verilen başlangıç şehrinden başlayarak, BFS ile erişilebilen tüm şehirler
 * keşfedilir. Her seviye (BFS seviyesi = minimum aktarma sayısı) ayrıca
 * gösterilir. Bu fonksiyon, bir şehrin ulaşım ağındaki kapsamını anlamak
 * için kullanılır.
 * ============================================================================
 */
void bfs_tam_gezinme(const Graf *g, int baslangic_id) {
    if (g == NULL) {
        printf("[HATA] Gecersiz graf!\n");
        return;
    }

    int bas_idx = sehir_bul(g, NULL, baslangic_id);
    if (bas_idx == -1) {
        printf("[HATA] Baslangic sehri (ID:%d) bulunamadi!\n", baslangic_id);
        return;
    }

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║              BFS TAM GEZINME - Sehir: %-23s ║\n", 
           g->sehirler[bas_idx].ad);
    printf("╚══════════════════════════════════════════════════════════════╝\n");

    /* BFS yardımcı dizileri */
    int *ziyaret = (int*)calloc(g->kapasite, sizeof(int));
    int *mesafe = (int*)malloc(g->kapasite * sizeof(int));
    int *kuyruk = (int*)malloc(g->kapasite * sizeof(int));

    DEBUG_MALLOC("BFS tam gezinme ziyaret dizisi calloc", ziyaret, 
                 g->kapasite * sizeof(int));
    DEBUG_MALLOC("BFS tam gezinme mesafe dizisi malloc", mesafe, 
                 g->kapasite * sizeof(int));
    DEBUG_MALLOC("BFS tam gezinme kuyruk dizisi malloc", kuyruk, 
                 g->kapasite * sizeof(int));

    if (!ziyaret || !mesafe || !kuyruk) {
        printf("[HATA] Bellek tahsisi basarisiz!\n");
        free(ziyaret); free(mesafe); free(kuyruk);
        return;
    }

    /* Mesafe dizisini -1 ile başlat (ulaşılamaz) */
    for (int i = 0; i < g->kapasite; i++) {
        mesafe[i] = -1;
    }

    int kuyruk_bas = 0, kuyruk_son = 0;
    kuyruk[kuyruk_son++] = bas_idx;
    ziyaret[bas_idx] = 1;
    mesafe[bas_idx] = 0;

    int ulasilan_sayisi = 0;

    /* BFS ana döngüsü */
    while (kuyruk_bas < kuyruk_son) {
        int mevcut = kuyruk[kuyruk_bas++];
        ulasilan_sayisi++;

        printf("  [Seviye %d] %s\n", mesafe[mevcut], g->sehirler[mevcut].ad);

        /* Komşuları gez */
        KenarDugumu *kenar = g->sehirler[mevcut].kenar_listesi;
        while (kenar != NULL) {
            int komsu = kenar->hedef_sehir_indeksi;
            if (!ziyaret[komsu]) {
                ziyaret[komsu] = 1;
                mesafe[komsu] = mesafe[mevcut] + 1;
                kuyruk[kuyruk_son++] = komsu;
            }
            kenar = kenar->next;
        }
    }

    printf("\n  Toplam ulasilan sehir sayisi: %d / %d\n\n", 
           ulasilan_sayisi, g->sehir_sayisi);

    free(ziyaret);
    free(mesafe);
    free(kuyruk);
}
