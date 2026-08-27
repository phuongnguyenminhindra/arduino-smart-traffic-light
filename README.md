# 🚦 Arduino Smart Traffic Light - Day 17

Đồ án đèn giao thông thông minh với 3 chế độ hoạt động, xử lý chống nhiễu nút bấm và chống nháy cảm biến ánh sáng.

### ✨ Tính năng
* **MODE_NORMAL:** Chu trình Đỏ (5s) -> Xanh (3s) -> Vàng (2s). Bấm nút nhanh để xin qua đường (kéo dài đèn đỏ lên 8s + còi buzzer).
* **MODE_NIGHT:** Tự động kích hoạt khi trời tối (LDR < 300) duy trì 2s. Đèn vàng nhấp nháy 0.5s. Chỉ thoát khi trời sáng (LDR > 500) - không dùng nút bấm để tránh nhảy mode.
* **MODE_EMERGENCY:** Giữ nút 5s để kích hoạt. Đèn đỏ + đèn người đi bộ nhấp nháy 0.3s + còi báo.
* **Xử lý nâng cao:** Code 100% non-blocking (không dùng `delay()`), dùng ngưỡng kép (hysteresis) 300/500 để chống nháy LDR.

### 🔧 Phần cứng
* Arduino Uno R3
* LED: 5 (Đỏ, Vàng, Xanh), 7 (Người đỏ), 8 (Người xanh)
* Nút bấm: D2 (INPUT_PULLUP)
* Còi Buzzer: D6
* Biến trở giả lập LDR: A0

### 🔌 Sơ đồ đấu nối
| Linh kiện | Chân Arduino |
| :--- | :--- |
| LED_RED | D5 |
| LED_YELLOW | D4 |
| LED_GREEN | D3 |
| LED_PED_RED | D7 |
| LED_PED_GREEN | D8 |
| BUTTON | D2 |
| BUZZER | D6 |
| LDR (Biến trở) | A0 |

### 🚀 Cách sử dụng
1. Nạp file `code/Day17.ino`
2. Vặn biến trở về 0 -> Tự vào ban đêm
3. Vặn lên 1023 -> Tự về ban ngày
4. Bấm nhanh khi đèn đỏ -> Xin qua đường
5. Giữ nút 5s -> Chế độ cấp cứu


Video: https://drive.google.com/file/d/1JdoWOGvSAMUI217fCvp5KiFaH618nsBi/view?usp=drive_link + https://drive.google.com/file/d/1yXnmA0Rr0R4H8SLPyFLvj7IZSSSUsAzc/view?usp=drive_link


---
*Developed by Phương Nguyễn - 2026*
