# AGENTS.md - Quy Tắc Dành Cho AI Agent trong Dự Án ESP_Automation

## ⚠️ QUY TẮC BẮT BUỘC: NGUYÊN TẮC GIT COMMIT & PUSH BẢO VỆ MÃ NGUỒN

1. **LUÔN LUÔN CHỜ XÁC NHẬN TỪ NGƯỜI DÙNG BẰNG LỜI (MANDATORY APPROVAL):**
   - TUYỆT ĐỐI KHÔNG ĐƯỢC tự động chạy các lệnh `git commit`, `git push`, `git revert`, `git merge`, `git rebase` hoặc bất kỳ lệnh Git làm thay đổi lịch sử repository khi CHƯA CÓ LỆNH XÁC NHẬN CỤ THỂ từ người dùng.
   - Khi tạo hoặc chỉnh sửa file mã nguồn/tài liệu, AI chỉ được phép thao tác và lưu file tại máy local.
   - Chỉ khi người dùng gõ lệnh trực tiếp trong chat (Ví dụ: *"commit giúp anh"*, *"hãy commit đi"*, *"push lên git nhé"*...) thì AI mới được thực thi lệnh Git.

2. **CHẶN LỘ BIẾN MÔI TRƯỜNG (.ENV):**
   - Luôn luôn đảm bảo các file chứa mật khẩu thực tế như `.env` nằm trong `.gitignore` và không bao giờ được add vào git stage.
