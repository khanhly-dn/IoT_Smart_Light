# 💡 **Smart Light – Đèn Thông Minh Điều Khiển Qua Web**

## 📌 **Giới thiệu**

Dự án **Smart Light** được phát triển nhằm xây dựng hệ thống **điều khiển đèn thông minh** sử dụng **ESP32** kết nối WiFi.  
Hệ thống tích hợp **cảm biến ánh sáng** để tự động bật/tắt đèn theo môi trường, kết hợp **giao diện Web hiện đại** cho phép người dùng giám sát và điều khiển từ xa trên bất kỳ thiết bị nào (điện thoại, máy tính).  
Ngoài chế độ tự động, hệ thống còn hỗ trợ **điều khiển thủ công**, **bật/tắt đèn indicator**, và **cấu hình WiFi ngay trên giao diện web**, hướng tới ứng dụng **nhà thông minh tiết kiệm năng lượng**.

---

## ⚙️ **Chức năng chính**

- **Điều khiển đèn chính (GPIO2)** bật/tắt từ xa qua Web  
- **Chế độ Auto** – tự động bật đèn khi ánh sáng môi trường xuống thấp dưới ngưỡng  
- **Chế độ Manual** – người dùng ghi đè điều khiển thủ công  
- **Đọc cảm biến ánh sáng** (ADC GPIO34) liên tục, hiển thị giá trị thời gian thực  
- **Điều khiển đèn Indicator (GPIO4)** độc lập làm đèn trạng thái phụ  
- **Giao diện Web responsive** – truy cập từ điện thoại hoặc máy tính qua WiFi  
- **Activity Log** – ghi lại lịch sử thao tác ngay trên giao diện  
- **Cấu hình WiFi động** – nhập SSID và mật khẩu mới ngay trên web  

---

## 🧩 **Sơ đồ hoạt động**

**Cảm biến ánh sáng** đọc ADC → **ESP32 xử lý** → So sánh ngưỡng (2000) → **Điều khiển relay/LED** → **Phản hồi trạng thái lên Web**  
**Người dùng** truy cập Web → Gửi lệnh HTTP → **ESP32 WebServer** xử lý → Cập nhật trạng thái thực tế

---

## 🛠️ **Phần cứng sử dụng**

| Linh kiện | Mô tả |
|---|---|
| **ESP32** | Vi điều khiển chính, WiFi tích hợp |
| **LED / Relay** | Đèn chính – GPIO2 |
| **LED Indicator** | Đèn trạng thái phụ – GPIO4 |
| **Cảm biến ánh sáng** | Quang trở + ADC – GPIO34 |
| **Nguồn** | 5V USB hoặc pin |
<p align="center">
  <img width="600" alt="Linh kien" src="https://github.com/user-attachments/assets/4992eae4-6d65-485a-999b-b7f80848241f" />
</p>
---

## 💻 **Phần mềm & Công nghệ**

- **Ngôn ngữ:** Arduino C++, HTML/CSS/JavaScript  
- **Framework:** Arduino ESP32 Core  
- **Thư viện:** `WiFi.h`, `WebServer.h`  
- **Giao tiếp:** HTTP REST API qua WiFi  
- **Giao diện:** Web App (chạy trên ESP32, không cần server ngoài)  
- **Font/UI:** Google Fonts (Syne, DM Mono), CSS Variables, Dark Theme  

---

## 🌐 **API Endpoints**

| Endpoint | Mô tả |
|---|---|
| `GET /` | Giao diện Web chính |
| `GET /status` | Trả về trạng thái JSON (led, auto, light, indicator) |
| `GET /toggle` | Bật/tắt đèn chính (chuyển sang Manual) |
| `GET /auto?state=1\|0` | Bật/tắt chế độ tự động |
| `GET /indicator?state=1\|0` | Bật/tắt đèn indicator |
| `GET /wifi?ssid=...&pass=...` | Cập nhật thông tin WiFi |

---

## 📊 **Thông số kỹ thuật**

- **Ngưỡng ánh sáng tối:** `2000` (ADC 12-bit, tối đa 4095)  
- **Tần suất đọc cảm biến:** mỗi **1 giây**, lấy trung bình **10 mẫu**  
- **Tần suất cập nhật Web:** **2 giây/lần** (polling tự động)  
- **Cổng WebServer:** `80` (HTTP)  

---

## 🚀 **Hướng dẫn cài đặt & chạy**

1. **Cài Arduino IDE** và thêm board **ESP32** vào Board Manager  
2. **Mở file** `smart_light.ino`  
3. **Chỉnh thông tin WiFi** trong code:
   ```cpp
   const char* WIFI_SSID = "Tên_WiFi";
   const char* WIFI_PASS = "Mật_khẩu";
   ```
4. **Nạp code** vào ESP32 qua cáp USB  
5. Mở **Serial Monitor** (115200 baud) để xem địa chỉ IP  
6. Truy cập địa chỉ IP trên **trình duyệt** → Giao diện Web xuất hiện  

---

## 🚀 **Hướng phát triển**

- Tích hợp **MQTT** để kết nối hệ thống nhà thông minh (Home Assistant)  
- Thêm **điều chỉnh ngưỡng ánh sáng** trực tiếp trên giao diện Web  
- Hỗ trợ **điều khiển nhiều đèn** cùng lúc  
- Thêm **lịch hẹn giờ** bật/tắt tự động theo thời gian  
- **OTA Update** – cập nhật firmware qua WiFi  

---

## 📷 **Demo**

- **Video hoạt động:** *(đính kèm link hoặc QR code)*  
- **Hình ảnh hệ thống:** *(thêm ảnh mạch, giao diện web tại đây)*  

---

## 👤 **Thực hiện**

- **Lý Gia Khánh**  

*Khoa Công nghệ Thông tin – *(Tên trường của bạn)**
