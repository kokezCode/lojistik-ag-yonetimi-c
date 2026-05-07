# Lojistik Firma - Şehirler Arası Bağlantı Yönetim Sistemi 

Bu proje, bir lojistik firmasının şehirler arası yol ağını ve taşıdığı kargo paketlerini yönetmek amacıyla geliştirilmiş hibrit bir veri yapısı uygulamasıdır. C programlama dili ile yazılmıştır.

##  Sistem Mimarisi (Hibrit Yapı)
Sistem iki ana veri yapısının entegre çalışmasıyla kurgulanmıştır:
* **Graf (Adjacency List):** Şehirler ve aralarındaki mesafeler/süreler bellek verimliliği için komşuluk listesi ile temsil edilmiştir.
* **Hash Tablosu (Linear Probing):** Kargo paketleri, Açık Adresleme yöntemiyle O(1) hızında sorgulanmaktadır. 
* **Pointer Bağlantısı:** Hash tablosundaki her paket, Graf dizisindeki mevcut konumuna (fiziksel bellek adresine) doğrudan bir pointer ile bağlıdır.

##  Teknik Özellikler
* **Hash Fonksiyonu:** İki aşamalı modular aritmetik (Özel PRIME_MOD algoritması).
* **Çakışma Çözümü:** Açık Adresleme - Doğrusal Sınama (Linear Probing) & Gecikmeli Silme (Lazy Deletion).
* **Graf Gezinme:** Genişlik Öncelikli Arama (BFS) kullanılarak en az aktarmalı rota bulma.
* **Bellek Yönetimi:** Dinamik tahsis edilen tüm yapılar (`malloc`, `calloc`, `realloc`) özel bir debug fonksiyonu ile anlık olarak izlenmiş ve program sonunda sızıntıya (memory leak) karşı temizlenmiştir.

##  Kurulum ve Çalıştırma
Projeyi derlemek ve çalıştırmak için terminalinizde şu komutları kullanabilirsiniz:

**Windows Kullanıcıları İçin (Türkçe Karakter ve UTF-8 Desteğiyle):**
```bash
gcc -Wall -Wextra -o lojistik main.c graph.c hash.c
chcp 65001
.\lojistik
