// ESP32 3 Kademeli (Auto-Ranging) Hassas Multimetre
// Geliştiren: Kevser - Ankara Üniversitesi Biyomedikal Mühendisliği

// GPIO Pinleri (Güç bu pinlerden sırayla verilecek)
const int kademePinleri[] = {25, 26, 27};
const int KADEME_SAYISI = 3;

// Multimetre ile ölçülen net referans direnç değerleri (Ohm)
const float referansDirencler[] = {325.8, 990.0, 4650.0};

// Ölçüm ve ADC Ayarları
const int adcPin = 35; // Ortak düğümün bağlı olduğu pin
const float adcCozunurluk = 4095.0;

// BAŞLANGIÇ KADEMESİ
int aktifKademe = 1; // 990 Ohm kademesinden başla

void setup() {
  Serial.begin(115200);
  
  // ADC çözünürlüğünü 12-bit (0-4095) olarak ayarla
  analogReadResolution(12);
  
  // Pinleri başlangıçta kapat (Yüksek Empedans)
  for (int i = 0; i < KADEME_SAYISI; i++) {
    pinMode(kademePinleri[i], INPUT);
  }
  
  Serial.println("========================================");
  Serial.println("  HASSAS AUTO-RANGING MULTIMETRE v2.0   ");
  Serial.println("  Mekanik Cekme Testi Icin Hazir       ");
  Serial.println("========================================");
}

void loop() {
  // --- AKTİF KADEMEYİ AÇ ---
  for (int i = 0; i < KADEME_SAYISI; i++) {
    if (i == aktifKademe) {
      pinMode(kademePinleri[i], OUTPUT);
      digitalWrite(kademePinleri[i], HIGH);
    } else {
      pinMode(kademePinleri[i], INPUT); 
    }
  }

  // Voltajın oturması için bekleme
  delay(15);

  // --- GÜRÜLTÜ FİLTRELEME (100 Örneklem Ortalama) ---
  long toplamAdc = 0;
  int orneklem = 100;
  for (int i = 0; i < orneklem; i++) {
    toplamAdc += analogRead(adcPin);
    delayMicroseconds(500);
  }
  float ortalamaAdc = (float)toplamAdc / orneklem;

  // --- OTOMATİK KADEME DEĞİŞTİRME MANTIĞI ---
  // ADC değeri çok düşükse (hassasiyet azalırsa) vites küçült
  if (ortalamaAdc < 800 && aktifKademe > 0) {
    aktifKademe--; 
    return; 
  }
  // ADC değeri çok yüksekse (3.3V'a yaklaşırsa) vites büyüt
  else if (ortalamaAdc > 3300 && aktifKademe < KADEME_SAYISI - 1) {
    aktifKademe++; 
    return; 
  }

  // --- HESAPLAMA VE KALİBRASYON ---
  float vOut = (ortalamaAdc / adcCozunurluk) * 3.3; 
  
  // HER KADEME İÇİN ÖZEL VOLTAJ REFERANSI (Diyot Kayıpları Dahil)
  float dinamikVRef;
  if (aktifKademe == 0)      dinamikVRef = 2.254; // 325.8 Ohm kademesi kalibrasyonu
  else if (aktifKademe == 1) dinamikVRef = 2.376; // 990.0 Ohm kademesi kalibrasyonu
  else                       dinamikVRef = 2.550; // 4650.0 Ohm kademesi (tahmini)

  float rx = 0.0;
  // Güvenlik: vOut'un vRef'i geçmediğinden emin ol
  if (dinamikVRef - vOut > 0.01) {
    rx = referansDirencler[aktifKademe] * (vOut / (dinamikVRef - vOut));
  } else {
    rx = -1; // Açık devre veya limit dışı
  }

  // --- SONUÇLARI YAZDIR ---
  Serial.print("KADEME: ");
  Serial.print(aktifKademe + 1);
  Serial.print(" [Ref: ");
  Serial.print(referansDirencler[aktifKademe]);
  Serial.print(" Ohm] | Vout: ");
  Serial.print(vOut, 3);
  Serial.print("V | HESAPLANAN DIRENC: ");
  
  if (rx > 0) {
    Serial.print(rx, 2);
    Serial.println(" Ohm");
  } else {
    Serial.println("ACIK DEVRE");
  }

  delay(350); 
}
