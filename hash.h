/**
 * ============================================================================
 * hash.h - Hash Tablosu Fonksiyon Prototipleri
 * ============================================================================
 * 
 * Bu başlık dosyası, paket bilgilerini hızlı sorgulamak için kullanılan
 * hash tablosu yapısının tüm fonksiyon prototiplerini içerir.
 *
 * Hash Fonksiyonu: Tip A - Sayısal anahtarın modunu alan özelleştirilmiş fonksiyon
 * Öğrenci No: 1240505902
 * Çakışma Çözümü: Açık Adresleme (Open Addressing) - Lineer Probing
 * ============================================================================
 */

#ifndef HASH_H
#define HASH_H

#include "structs.h"

/* ============================================================================
 * Hash Tablosu Yaşam Döngüsü Fonksiyonları
 * ============================================================================ */

/**
 * hash_tablosu_olustur - Yeni bir hash tablosu oluşturur
 * 
 * Hash tablosu için bellek tahsisi yapılır ve tüm girdiler
 * DURUM_BOS olarak başlatılır. Her malloc sonrası bellek izleme
 * debug çıktısı yazdırılır.
 *
 * @return Oluşturulan hash tablosunun pointer'ı, hata durumunda NULL
 */
HashTablosu* hash_tablosu_olustur(void);

/**
 * hash_tablosu_yok_et - Hash tablosunu bellekten temizler
 * 
 * Tablo dizisi serbest bırakılır ve tablo yapısı itself silinir.
 *
 * @param ht Yok edilecek hash tablosunun pointer'ı
 */
void hash_tablosu_yok_et(HashTablosu *ht);

/* ============================================================================
 * Hash Fonksiyonu
 * ============================================================================ */

/**
 * hash_fonksiyonu - Tip A: Sayısal anahtarın modunu alan özelleştirilmiş fonksiyon
 * 
 * Öğrenci numarası (1240505902) kullanılarak özelleştirilmiştir.
 * 
 * Formül: h(k) = (k % PRIME_MOD) % HASH_TABLO_BOYUTU
 * 
 * PRIME_MOD, öğrenci numarasından türetilmiş asal sayıdır (12405).
 * İki aşamalı mod işlemi, farklı anahtar aralıkları için daha iyi
 * dağılım sağlar. İlk mod, anahtarı öğrenci numarası ile sınırlar;
 * ikinci mod, tablo boyutuna eşler.
 *
 * @param anahtar Paket kimliği (hash anahtarı)
 * @return Hash tablosundaki indeks (0 - HASH_TABLO_BOYUTU-1)
 */
unsigned int hash_fonksiyonu(int anahtar);

/* ============================================================================
 * Hash Tablosu İşlemleri
 * ============================================================================ */

/**
 * paket_ekle - Hash tablosuna yeni bir paket ekler
 * 
 * Linear Probing ile çakışma çözümü uygulanır:
 * 1. Hash fonksiyonu ile başlangıç indeksi hesaplanır
 * 2. Eğer indeks doluysa, bir sonraki indeks kontrol edilir
 * 3. Silinmiş (DURUM_SILINDI) veya boş (DURUM_BOS) hücre bulana kadar devam eder
 * 4. İlk uygun hücreye paket yerleştirilir
 *
 * Zaman karmaşıklığı: Ortalama O(1), en kötü O(n)
 *
 * @param ht  Hash tablosunun pointer'ı
 * @param p   Eklenecek paket yapısı
 * @return 1 başarılı, 0 başarısız (tablo dolu veya aynı ID mevcut)
 */
int paket_ekle(HashTablosu *ht, Paket p);

/**
 * paket_sorgula - Hash tablosundan paket kimliği ile paket arar
 * 
 * Linear Probing ile arama yapılır:
 * 1. Hash fonksiyonu ile başlangıç indeksi hesaplanır
 * 2. Eğer hücre dolu ve anahtar eşleşmiyorsa, bir sonraki indeks kontrol edilir
 * 3. Boş (DURUM_BOS) hücreye ulaşılırsa arama sonlandırılır (paket yok)
 * 4. Silinmiş (DURUM_SILINDI) hücreler atlanır ve arama devam eder
 *
 * Zaman karmaşıklığı: Ortalama O(1), en kötü O(n)
 *
 * @param ht      Hash tablosunun pointer'ı
 * @param paket_id Aranacak paket kimliği
 * @return Bulunan paketin pointer'ı, bulunamazsa NULL
 */
Paket* paket_sorgula(HashTablosu *ht, int paket_id);

/**
 * paket_sil - Hash tablosundan paket siler
 * 
 * Lazy deletion (gecikmeli silme) tekniği kullanılır:
 * Paket verisi silinmez, sadece girdi durumu DURUM_SILINDI olarak işaretlenir.
 * Bu, Linear Probing zincirinin bozulmasını engeller.
 *
 * @param ht      Hash tablosunun pointer'ı
 * @param paket_id Silinecek paket kimliği
 * @return 1 başarılı, 0 bulunamadı
 */
int paket_sil(HashTablosu *ht, int paket_id);

/**
 * hash_tablosu_yazdir - Hash tablosunun tüm durumunu ekrana yazdırır
 * 
 * Her hücrenin indeksi, durumu ve varsa paket bilgileri listelenir.
 * Boş ve silinmiş hücreler de gösterilir.
 *
 * @param ht Hash tablosunun pointer'ı
 */
void hash_tablosu_yazdir(const HashTablosu *ht);

/**
 * hash_tablosu_istatistik - Hash tablosu performans istatistiklerini yazdırır
 * 
 * Doluluk oranı, toplam sorgu sayısı, ortalama arama uzunluğu gibi
 * istatistikler gösterilir.
 *
 * @param ht Hash tablosunun pointer'ı
 */
void hash_tablosu_istatistik(const HashTablosu *ht);

#endif /* HASH_H */
