/**
 * ============================================================================
 * graph.h - Graf Veri Yapısı Fonksiyon Prototipleri
 * ============================================================================
 * 
 * Bu başlık dosyası, şehirler arası bağlantıları yöneten graf yapısı için
 * tüm fonksiyon prototiplerini içerir. Graf, komşuluk listesi (Adjacency List)
 * kullanılarak temsil edilir ve BFS (Genişlik Öncelikli Arama) ile
 * gezinme desteği sağlar.
 *
 * Öğrenci No: 1240505902
 * ============================================================================
 */

#ifndef GRAPH_H
#define GRAPH_H

#include "structs.h"

/* ============================================================================
 * Graf Oluşturma ve Yönetim Fonksiyonları
 * ============================================================================ */

/**
 * graf_olustur - Yeni bir graf yapısı oluşturur
 * 
 * Bellekte Graf yapısı için yer ayırır ve başlangıç değerlerini atar.
 * İlk kapasite MAX_SEHIR_SAYISI olarak belirlenir.
 * Her malloc sonrası bellek izleme debug çıktısı yazdırılır.
 *
 * @return Olusturulan graf yapısının pointer'ı, hata durumunda NULL
 */
Graf* graf_olustur(void);

/**
 * graf_yok_et - Graf yapısını ve tüm alt yapılarını bellekten temizler
 * 
 * Önce her şehrin kenar listesindeki tüm KenarDugumları serbest bırakılır,
 * ardından şehirler dizisi ve graf yapısı itself silinir.
 *
 * @param g Yok edilecek graf yapısının pointer'ı
 */
void graf_yok_et(Graf *g);

/**
 * sehir_ekle - Grafa yeni bir şehir ekler
 * 
 * Şehirler dizisinde ilk boş yere (aktif==0) yeni şehir eklenir.
 * Eğer dizi doluysa kapasite iki katına çıkarılır (realloc).
 * Her malloc/realloc sonrası bellek izleme debug çıktısı yazdırılır.
 *
 * @param g Graf yapısının pointer'ı
 * @param ad Eklenecek şehrin adı
 * @param id Şehrin benzersiz kimliği
 * @return Eklenen şehrin dizideki indeksi, hata durumunda -1
 */
int sehir_ekle(Graf *g, const char *ad, int id);

/**
 * sehir_bul - İsim veya ID ile şehir arar
 * 
 * Grafın şehirler dizisinde linear arama yaparak istenen şehri bulur.
 *
 * @param g Graf yapısının pointer'ı
 * @param ad Aranacak şehir adı (NULL ise ID ile arama yapılır)
 * @param id Aranacak şehir kimliği (ad NULL olduğunda kullanılır)
 * @return Bulunan şehrin indeksi, bulunamazsa -1
 */
int sehir_bul(const Graf *g, const char *ad, int id);

/**
 * yol_tanimla - İki şehir arasına çift yönlü bağlantı (kenar) ekler
 * 
 * Her iki şehrin kenar listesine yeni bir KenarDugumu eklenir.
 * Çift yönlü bağlantı olduğundan A->B ve B->A kenarları oluşturulur.
 * Her malloc sonrası bellek izleme debug çıktısı yazdırılır.
 *
 * @param g Graf yapısının pointer'ı
 * @param kaynak_id Kaynak şehrin kimliği
 * @param hedef_id  Hedef şehrin kimliği
 * @param mesafe    İki şehir arasındaki mesafe (km)
 * @param sure      Tahmini seyahat süresi (saat)
 * @return 1 başarılı, 0 başarısız
 */
int yol_tanimla(Graf *g, int kaynak_id, int hedef_id, int mesafe, int sure);

/**
 * graf_yazdir - Grafın tüm şehirlerini ve bağlantılarını ekrana yazdırır
 * 
 * Her şehrin adı, kimliği ve komşu şehirleri (mesafe ve süre bilgileriyle
 * birlikte) listelenir.
 *
 * @param g Graf yapısının pointer'ı
 */
void graf_yazdir(const Graf *g);

/* ============================================================================
 * BFS (Genişlik Öncelikli Arama) Fonksiyonları
 * ============================================================================ */

/**
 * bfs_rota_bul - BFS algoritması ile iki şehir arasındaki en kısa rotayı bulur
 * 
 * Kenar ağırlıkları (mesafe) dikkate alınmaksızın, en az aktarmalı rotayı
 * bulur. BFS kuyruk veri yapısı kullanılarak seviye seviye gezinme yapılır.
 * Ziyaret edilen her düğüm bir parent dizisi ile kaydedilir, böylece
 * bulunan rota geri izlenebilir.
 *
 * BFS Algoritması Adımları:
 * 1. Başlangıç şehri kuyruğa eklenir ve ziyaret edildi olarak işaretlenir
 * 2. Kuyruktan bir şehir çıkartılır
 * 3. Çıkartılan şehrin tüm komşuları kontrol edilir
 * 4. Ziyaret edilmemiş komşular kuyruğa eklenir ve parent'ları kaydedilir
 * 5. Hedef bulunana veya kuyruk boşalana kadar devam eder
 *
 * @param g    Graf yapısının pointer'ı
 * @param baslangic_id Başlangıç şehrinin kimliği
 * @param hedef_id     Hedef şehrinin kimliği
 */
void bfs_rota_bul(const Graf *g, int baslangic_id, int hedef_id);

/**
 * bfs_tam_gezinme - BFS ile bir şehirden ulaşılabilen tüm şehirleri listeler
 * 
 * Verilen başlangıç şehrinden BFS ile erişilebilen tüm şehirler, 
 * keşif sırasına göre listelenir. Her seviye (mesafe) ayrıca gösterilir.
 *
 * @param g Graf yapısının pointer'ı
 * @param baslangic_id Başlangıç şehrinin kimliği
 */
void bfs_tam_gezinme(const Graf *g, int baslangic_id);

#endif /* GRAPH_H */
